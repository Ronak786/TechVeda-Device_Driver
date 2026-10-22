# Embedded Linux: Platform vs Bus Devices & Drivers Guide

A comprehensive guide explaining the difference between Platform and Bus devices/drivers, and how device-driver matching works during kernel boot.

---

## 1. How to Differentiate Platform and Bus Devices / Drivers

### 1.1 The Linux Unified Device Model
In the Linux kernel, hardware management revolves around three core primitives:
1. **Device (`struct device`)**: Represents physical or virtual hardware.
2. **Driver (`struct device_driver`)**: The code that controls the device.
3. **Bus (`struct bus_type`)**: The communication medium linking devices with drivers.

> **Crucial Rule:** Every device in Linux **must** belong to exactly one bus (`dev->bus`).

```
                   +------------------------+
                   |  Linux Device Model    |
                   |   (drivers/base/)      |
                   +------------------------+
                               |
              +----------------+----------------+
              |                                 |
              v                                 v
   +----------------------+          +----------------------+
   |  Platform Bus        |          |  Dedicated Bus       |
   |  (platform_bus_type) |          |  (i2c, spi, usb,     |
   |  *Virtual Bus*       |          |   mdio, pci, ...)    |
   +----------------------+          +----------------------+
```

---

### 1.2 What is a Platform Device / Driver?
* **Concept:** Peripherals integrated directly inside the System-on-Chip (SoC) — such as UART, GPIO controllers, timers, memory controllers, and I2C/SPI master controllers.
* **Connection:** Wired directly to the CPU's internal system interconnect (AXI, AHB, APB). They are **memory-mapped** directly into the CPU's address space (MMIO) and wired to CPU interrupt lines (IRQs).
* **Discoverability:** **Non-discoverable**. The CPU cannot probe the hardware over a wire to ask for a vendor ID.
* **Why "Platform"?** Because they lack a physical bus controller querying them, the kernel provides a **pseudo/virtual bus** called `platform_bus_type` to integrate them into the device model without writing dummy bus protocols.
* **Kernel Structures:** `struct platform_device` and `struct platform_driver`.

---

### 1.3 What is a Bus Device / Driver?
* **Concept:** External or off-chip peripherals connected via standard communication protocols such as **I2C, SPI, MDIO, USB, or PCI**.
* **Connection:** Communicates over bus data lines (SDA/SCL, MOSI/MISO, D+/D-, MDIO clock/data).
* **Discoverability:**
  * **Dynamic / Plug-and-Play:** Buses like USB and PCI automatically enumerate connected devices via hardware descriptors.
  * **Static / Embedded:** Buses like I2C and SPI cannot self-discover chips safely, so their connected chips are declared statically in the Device Tree.
* **Kernel Structures:** Subsystem-specific wrappers:
  * **I2C:** `struct i2c_client` (device) and `struct i2c_driver` (driver) on `i2c_bus_type`.
  * **SPI:** `struct spi_device` (device) and `struct spi_driver` (driver) on `spi_bus_type`.
  * **MDIO:** `struct phy_device` (device) and `struct phy_driver` (driver) on `mdio_bus_type`.
  * **USB:** `struct usb_device` and `struct usb_driver` on `usb_bus_type`.

---

### 1.4 How to Spot the Difference in Device Tree (`.dts`)

#### A. Platform Devices in DTS
Platform devices appear **directly under the root `/`** OR inside a container with `compatible = "simple-bus"`. Their `reg` property specifies **physical CPU memory addresses**:

```dts
/ {
    /* Platform device at root */
    leds {
        compatible = "gpio-leds";
    };

    /* Container for internal SoC memory-mapped peripherals */
    ocp {
        compatible = "simple-bus";

        /* Platform device (UART controller) */
        uart3: serial@4806a000 {
            compatible = "ti,omap4-uart";
            reg = <0x4806a000 0x1000>;  /* CPU Physical Memory Address */
            interrupts = <...>;
        };

        /* Platform device (I2C Master Controller) */
        i2c1: i2c@48070000 {
            compatible = "ti,omap4-i2c";
            reg = <0x48070000 0x1000>;  /* CPU Physical Memory Address */
            ...
```

#### B. Bus Devices in DTS
Bus devices are defined as **child nodes inside their bus controller node**. Their `reg` property represents a **bus-specific address**, NOT a CPU memory address:

```dts
        /* Continued from i2c1 above */
        i2c1: i2c@48070000 {
            compatible = "ti,omap4-i2c";
            reg = <0x48070000 0x1000>;

            /* --- BUS DEVICES START HERE --- */

            /* I2C Bus Device: Temperature Sensor */
            tmp102: tmp102@48 {
                compatible = "ti,tmp102";
                reg = <0x48>;           /* 7-bit I2C Slave Address (0x48) */
            };

            /* I2C Bus Device: EEPROM */
            eeprom: eeprom@50 {
                compatible = "atmel,24c32";
                reg = <0x50>;           /* 7-bit I2C Slave Address (0x50) */
            };
        };
```

---

### 1.5 How to Spot the Difference in C Driver Code

| Aspect | Platform Driver Code | Bus Driver Code (e.g. I2C) |
| :--- | :--- | :--- |
| **Driver Structure** | `struct platform_driver` | `struct i2c_driver` |
| **Registration Macro** | `module_platform_driver(my_driver);` | `module_i2c_driver(my_driver);` |
| **Probe Signature** | `int (*probe)(struct platform_device *pdev)` | `int (*probe)(struct i2c_client *client)` |
| **MMIO / Address Access** | Uses `platform_get_resource()` or `devm_platform_ioremap_resource()` | Reads `client->addr` for I2C address; uses `i2c_smbus_*()` or `i2c_transfer()` |
| **Bus Assignment** | Belongs to `platform_bus_type` | Belongs to `i2c_bus_type` |

---

### 1.6 Summary Comparison Table

| Feature | Platform Device / Driver | Bus Device / Driver (I2C, SPI, MDIO) |
| :--- | :--- | :--- |
| **Underlying Bus** | Virtual (`platform_bus_type`) | Dedicated / Physical (`i2c_bus_type`, `spi_bus_type`, etc.) |
| **Physical Connection** | CPU internal interconnect (Memory-Mapped I/O) | Bus lines (SDA/SCL, MOSI/MISO, MDIO) |
| **DTS Location** | Under `/` or under `compatible = "simple-bus"` | Nested child nodes inside the bus controller node |
| **DTS `reg` Meaning** | Physical CPU Base Address (e.g. `0x48070000`) | Protocol Slave Address (e.g. `0x48` on I2C, chip select on SPI) |
| **Example Devices** | UART controller, GPIO controller, I2C host, Display IP | Temp sensors, RTC chips, EEPROMs, Ethernet PHYs |

---

## 2. Boot-Time Device Creation and Driver Matching Flow

During boot, the kernel instantiates devices and matches them with drivers in distinct phases.

```
+------------------------------------------------------------------------------------+
| 1. Bootloader (U-Boot) passes flattened Device Tree binary (.dtb) in RAM to Kernel |
+------------------------------------------------------------------------------------+
                                         │
                                         ▼
+------------------------------------------------------------------------------------+
| 2. Kernel un-flattens .dtb into in-memory struct device_node tree                  |
+------------------------------------------------------------------------------------+
                                         │
                                         ▼
+------------------------------------------------------------------------------------+
| 3. OF Platform Layer calls of_platform_default_populate()                         |
|    - Scans root "/" and "simple-bus" containers                                    |
|    - Creates struct platform_device for SoC peripherals & bus controllers          |
|    - ⛔ STOPS at non-simple buses (e.g., skips children of i2c1)                   |
+------------------------------------------------------------------------------------+
                                         │
                                         ▼
+------------------------------------------------------------------------------------+
| 4. Platform Driver Registration & Controller Probe                                 |
|    - module_platform_driver(i2c_omap_driver) registers driver                      |
|    - platform_bus_type matches compatible string ("ti,omap4-i2c")                  |
|    - i2c_omap_probe() runs: initializes hardware, calls i2c_add_adapter()          |
+------------------------------------------------------------------------------------+
                                         │
                                         ▼
+------------------------------------------------------------------------------------+
| 5. Bus Subsystem populates Bus Devices                                             |
|    - I2C Core calls of_i2c_register_devices(adapter)                               |
|    - Parses child nodes under i2c1 (tmp102@48, eeprom@50)                          |
|    - Creates struct i2c_client with slave address from reg (<0x48>)                |
|    - Registers them onto i2c_bus_type via device_register()                        |
+------------------------------------------------------------------------------------+
                                         │
                                         ▼
+------------------------------------------------------------------------------------+
| 6. Bus Driver Registration & Peripheral Probe                                      |
|    - module_i2c_driver(tmp102_driver) registers driver                             |
|    - i2c_bus_type matches compatible string ("ti,tmp102")                          |
|    - tmp102_probe(client) runs: sensor is ready!                                   |
+------------------------------------------------------------------------------------+
```

---

### Step-by-Step Breakdown

### Step 1: Un-flattening the Device Tree
1. The bootloader passes the physical memory address of the compiled `.dtb` in CPU register (`r2` on ARM32, `x0` on ARM64).
2. During early architecture boot (`setup_arch()`), the kernel un-flattens this blob into a live hierarchy of `struct device_node` structures in RAM.
3. At this point, nodes are simply strings and properties — no device structures (`struct device`) exist yet.

---

### Step 2: The OF Layer Populates Platform Devices
During `initcall` execution, architecture code calls:
```c
of_platform_default_populate(NULL, NULL, NULL);
```
*(Located in `drivers/of/platform.c`)*

The OF platform layer iterates through nodes with the following rules:
1. It registers nodes with a `compatible` string located directly under `/`.
2. It looks for container nodes matching the `of_default_bus_match_table`:
   * `"simple-bus"`
   * `"simple-mfd"`
   * `"isa"`
   * `"arm,amba-bus"`
3. For each child inside a `"simple-bus"`:
   * It allocates a **`struct platform_device`**.
   * It extracts the `reg` property (address/size) and `interrupts`, translating them into `pdev->resource`.
   * It calls `platform_device_register(pdev)`, which calls `device_register(&pdev->dev)` with:
     ```c
     pdev->dev.bus = &platform_bus_type;
     ```
4. **Where it stops:** When it reaches `i2c1: i2c@48070000` with `compatible = "ti,omap4-i2c"`, it sees that this is **not** a `"simple-bus"`. The OF layer creates a platform device for `i2c1`, but **does NOT look at its children**.

---

### Step 3: Platform Driver Matches Controller
1. The SoC I2C controller driver (`drivers/i2c/busses/i2c-omap.c`) registers via:
   ```c
   module_platform_driver(omap_i2c_driver);
   ```
   This calls `platform_driver_register()`, which calls core `driver_register()`.
2. The core Device Model invokes the platform bus matching function:
   ```c
   platform_match(dev, drv)
   ```
3. `platform_match` compares the `compatible` string from the Device Tree (`"ti,omap4-i2c"`) against the driver's `of_match_table`:
   ```c
   static const struct of_device_id omap_i2c_of_match[] = {
       { .compatible = "ti,omap4-i2c", },
       { }
   };
   ```
4. On match, the kernel invokes `omap_i2c_probe(pdev)`.

---

### Step 4: Bus Controller Instantiates Bus Devices
Inside `omap_i2c_probe()`:
1. The driver configures hardware clocks, interrupts, and registers.
2. It allocates a `struct i2c_adapter` (representing the physical I2C bus channel).
3. It calls **`i2c_add_adapter(adapter)`**.
4. The I2C subsystem (`drivers/i2c/i2c-core-of.c`) takes over and calls **`of_i2c_register_devices(adapter)`**.
5. It iterates through all child nodes of the `i2c1` node in Device Tree:
   * Finds `tmp102@48`.
   * Reads `reg = <0x48>` as the I2C slave address.
   * Allocates a **`struct i2c_client`**.
   * Sets `client->dev.bus = &i2c_bus_type`.
   * Calls `device_register(&client->dev)`.

---

### Step 5: Bus Driver Matches Bus Device
1. The sensor driver (`drivers/hwmon/tmp102.c`) registers via:
   ```c
   module_i2c_driver(tmp102_driver);
   ```
2. The core calls the I2C bus matching function:
   ```c
   i2c_device_match(dev, drv)
   ```
3. It compares `compatible = "ti,tmp102"` against the driver’s `of_match_table`:
   ```c
   static const struct of_device_id tmp102_of_match[] = {
       { .compatible = "ti,tmp102", },
       { }
   };
   ```
4. When they match, the kernel calls `tmp102_probe(client)`.
5. The sensor is now operational.

---

## 3. Matching Priority Order on Any Linux Bus
Whenever any driver or device registers, the bus's `match()` callback searches in this priority order:

```
                      +-----------------------------+
                      |   Device Tree (OF) Match    |
                      | Compares DT 'compatible'    |
                      | against drv->of_match_table |
                      +-----------------------------+
                                     │ (No Match)
                                     ▼
                      +-----------------------------+
                      |        ACPI Match           |
                      | Compares ACPI table IDs     |
                      | against drv->acpi_match_table
                      +-----------------------------+
                                     │ (No Match)
                                     ▼
                      +-----------------------------+
                      |       ID Table Match        |
                      | Compares device name string |
                      | against drv->id_table       |
                      +-----------------------------+
                                     │ (No Match)
                                     ▼
                      +-----------------------------+
                      |     Driver Name Match       |
                      | Compares device name string |
                      | directly against drv->name  |
                      +-----------------------------+
```

---

## 4. Key Takeaways
1. **Platform Bus is a Software Construct:** It exists to give memory-mapped SoC blocks a home in the Linux Device Model without requiring hardware bus controllers.
2. **Device Tree Hierarchy Matters:**
   * Nodes directly under `/` or under `"simple-bus"` become **Platform Devices**.
   * Nodes inside controller nodes (like `i2c@...` or `spi@...`) become **Bus Devices**.
3. **Two-Stage Registration:**
   * Boot begins by creating **Platform Devices** (including Bus Master Controllers).
   * Only when a Bus Master Controller driver probes does it parse its child nodes and create the corresponding **Bus Devices**.
4. **Device Tree Matching:**
   * Both platform drivers and bus drivers use `of_match_table` to match against Device Tree `compatible` strings.
