# Linux Device Drivers: Filesystem Inspection & Command Reference Guide

When you insert a kernel module into a running Linux system (via `insmod` or `modprobe`), the Linux kernel dynamically exposes the driver's internal state across several **virtual filesystems** (`/proc`, `/sys`, and `/dev`).

This guide details **where to look**, the **purpose of each filesystem location**, and the **essential commands** used to inspect and manage Linux device drivers.

---

## 1. Visual Overview: The Driver Lifecycle in the Filesystem

```
                    +──────────────────────────────+
                    |      insmod my_driver.ko     |
                    +──────────────┬───────────────+
                                   │
                                   ▼
             +───────────────────────────────────────────+
             |               LINUX KERNEL                |
             +─────┬───────────────────────┬───────────┬─+
                   │                       │           │
                   ▼                       ▼           ▼
        +────────────────────+  +───────────────────+  +──────────────────────+
        |   /proc/modules    |  |   /proc/devices   |  |     /sys/module/     |
        |  (Memory & State)  |  |  (Major Numbers)  |  | (Sysfs Attributes)   |
        +────────────────────+  +─────────┬─────────+  +──────────────────────+
                                          │
                   Creates device file    │  (mknod /dev/my_dev c <MAJOR> 0)
                   or udev auto-creation  │
                                          ▼
                                +───────────────────+
                                |  /dev/my_device   |  <─── User App (open/read/write)
                                +───────────────────+
```

---

## 2. Where to Look in the Filesystem

### A. `/proc/devices` — Device Registration & Major Numbers

* **What it is**: A text file generated dynamically by the kernel that lists all **character** and **block** device drivers currently registered with the Virtual File System (VFS).
* **Purpose**: Verifies that your driver successfully called `register_chrdev()` or `alloc_chrdev_region()`, and tells you the **Major Number** assigned to your driver.
* **How to inspect**:
  ```bash
  cat /proc/devices | grep <driver_name>
  ```
* **Example Output**:
  ```text
  240 simple_char
  ```
  *(Here, `240` is the Major Number. The kernel uses this number to route file operations like `open()` and `read()` to your driver's `fops`).*

---

### B. `/proc/modules` — Loaded Kernel Modules

* **What it is**: A virtual file containing real-time status of all kernel modules loaded in RAM.
* **Purpose**: Used by the kernel and tools like `lsmod` to track module memory addresses, dependencies, and reference counts.
* **How to inspect**:
  ```bash
  cat /proc/modules | grep <module_name>
  ```
* **Example Output**:
  ```text
  simple_char 16384 0 - Live 0xbf000000 (O)
  ```
  * Breakdown:
    * `simple_char`: Module name.
    * `16384`: Memory footprint in bytes.
    * `0`: Instance/reference count (number of users currently accessing it).
    * `Live`: Current runtime state (`Live`, `Loading`, or `Unloading`).
    * `0xbf000000`: Starting virtual memory address in kernel space.
    * `(O)`: Out-of-tree module flag.

---

### C. `/sys/module/<name>/` — Driver Attributes in Sysfs

* **What it is**: A directory created by the kernel inside **sysfs** for every loaded module.
* **Purpose**: Exposes driver properties, parameters, licensing, and reference counts to user space.
* **Key files inside `/sys/module/<name>/`**:

| Path | Purpose | Example Command |
|---|---|---|
| `/sys/module/<name>/initstate` | State of the module (`live`, `coming`, `going`) | `cat /sys/module/simple_char/initstate` |
| `/sys/module/<name>/refcnt` | Number of active users (cannot unload if $> 0$) | `cat /sys/module/simple_char/refcnt` |
| `/sys/module/<name>/parameters/` | Module parameters passed at load time | `ls /sys/module/<name>/parameters/` |
| `/sys/module/<name>/sections/` | Memory section addresses (`.text`, `.data`, `.bss`) | Useful for GDB / kernel debugging |
| `/sys/module/<name>/version` | Driver version string (if specified in code) | `cat /sys/module/<name>/version` |

---

### D. `/sys/class/` — Device Classes (Automatic Node Generation)

* **What it is**: Sysfs hierarchy organizing devices by function (e.g. `tty`, `net`, `block`, `input`, or custom classes).
* **Purpose**: When modern drivers call `class_create()` and `device_create()`, udev/mdev reads this directory and automatically generates `/dev/<device_name>`.
* **How to inspect**:
  ```bash
  ls -l /sys/class/
  ```

---

### E. `/dev/` — Device Nodes (User-Space Entry Point)

* **What it is**: The device filesystem containing special device files (character `c` and block `b` nodes).
* **Purpose**: The bridge between user-space and kernel-space. User applications open these files (`open("/dev/simple_char", O_RDWR)`) to invoke your driver.
* **How to inspect**:
  ```bash
  ls -l /dev/<device_name>
  ```
* **Example Output**:
  ```text
  crw-rw-rw- 1 root root 240, 0 Oct 15 00:30 /dev/simple_char
  ```
  * `c`: Character device (transfers data byte-by-byte).
  * `240`: Major Number (links to your driver in `/proc/devices`).
  * `0`: Minor Number (distinguishes individual sub-devices or channels).

---

## 3. Essential Command Reference

Here is a list of commands used when developing, testing, and debugging Linux device drivers:

### 1. `insmod` — Insert Kernel Module
Loads an individual `.ko` file into the running kernel. Does not resolve dependencies.
```bash
insmod my_driver.ko
```

### 2. `rmmod` — Remove Kernel Module
Unloads an unused driver module from the kernel.
```bash
rmmod my_driver
```
> **Note**: Do not include the `.ko` extension when running `rmmod`.

### 3. `lsmod` — List Loaded Modules
Reads and formats `/proc/modules` into a clean table:
```bash
lsmod
```
Shows: `Module`, `Size`, `Used by` (reference count and dependent drivers).

### 4. `modinfo` — Module Metadata Inspector
Reads the ELF header of a `.ko` file without loading it. Displays author, license, dependencies, and parameter descriptions:
```bash
modinfo my_driver.ko
```

### 5. `dmesg` — Kernel Ring Buffer Log
Displays log messages printed by the kernel and drivers via `printk()`:
```bash
# View last 15 kernel messages
dmesg | tail -n 15

# Follow kernel logs in real-time
dmesg -w
```

### 6. `mknod` — Create Device Node Manually
Creates a special character (`c`) or block (`b`) file in `/dev/`:
```bash
# Syntax: mknod /dev/<name> <type> <MAJOR> <MINOR>
mknod /dev/simple_char c 240 0
```

### 7. `chmod` & `chown` — Device Permissions
Ensures non-root user applications can access the device node:
```bash
chmod 666 /dev/simple_char
```

### 8. `modprobe` — Smart Module Loader (System Packages)
Loads modules from `/lib/modules/$(uname -r)/` while automatically resolving and loading dependent modules:
```bash
modprobe <module_name>
modprobe -r <module_name>   # Remove module and unused dependencies
```

---

## 4. Quick Step-by-Step Verification Checklist

Whenever you test a new driver in QEMU or on hardware:

```bash
# 1. Insert module
insmod simple_char.ko

# 2. Check printk output
dmesg | tail -n 5

# 3. Check loaded module table
lsmod | grep simple_char

# 4. Check assigned Major Number
cat /proc/devices | grep simple_char

# 5. Check sysfs entry
ls -la /sys/module/simple_char/

# 6. Create device node (if driver doesn't use device_create)
mknod /dev/simple_char c <MAJOR> 0
chmod 666 /dev/simple_char

# 7. Run user application
./test_app

# 8. Check refcount (should be 0 when app closes)
cat /sys/module/simple_char/refcnt

# 9. Unload driver
rmmod simple_char
```
