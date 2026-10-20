/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * ******************************************************************************
 * This program is part of the source code provided with "Linux Device Drivers"
 * Course offered by Techveda <www.techveda.org>
 *
 * Copyright (C) 2020 Techveda
 * Author: Raghu Bharadwaj
 * Git repository:
 *   https://gitlab.com/techveda/ldd-0920
 * ******************************************************************************
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* Relevant headers */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/...h>


MODULE_AUTHOR("<Author name, email id>");
MODULE_DESCRIPTION("<driver  decription>");
MODULE_LICENSE("<license>");
	/* one of:
	 * "GPL"						[GNU Public License v2 or later]
	 * "GPL v2"						[GNU Public License v2]
	 * "GPL and additional rights"	[GNU Public License v2 rights and more]
	 * "Dual BSD/GPL"				[GNU Public License v2
	 *									or BSD license choice]
	 * "Dual MIT/GPL"				[GNU Public License v2
	 *									or MIT license choice]
	 * "Dual MPL/GPL"				[GNU Public License v2
	 *									or Mozilla license choice]
	 * "Proprietary"				[Non free products]
	 */



/* The probe method of our driver */
static int <chipname>_probe(struct <foo>_client/dev *client/dev, // named as 'client' or 'dev'
                const struct <foo>_device_id *id)
{
	/*
	 * first param: struct <foo>_client *client
	 * the specialized structure for this kernel framework (eg. i2c, spi, usb,
	 * pci, etc); we often/usually extract the 'device' pointer from it..
	 */
	struct device *dev = client->dev;

	/*
	 * Your work in the probe() routine:
	 * 1. Initialize the device
	 * 2. Prepare driver work: allocate a structure for a suitable
	 *    framework, allocate memory, map I/O memory, register interrupts...
	 * 3. When everything is ready, register the new device with chosen kernel framework
	 */

	return 0;
}


/*------- Matching the driver to the device ------------------
 * 3 different ways: 
 *  by name : for platform & I2C devices
 *  by DT 'compatible' property : for devices on the Device Tree
 *                                 (ARM32, Aarch64, PPC, etc)
 *  by ACPI ID : for devices on ACPI tables (x86)
 */

/* 
 * 1. By name : for platform & I2C devices
 * The <foo>_device_id structure:
 * where <foo> is one of:
 *  acpi_button, cnic, cpufreq, gameport, hid, i2c, ide_pci, ipmi, mbus, mmc,
 *  pnp, scsi, sdio, serio, spi, tty, usb, vme
 */
static const struct <foo>_device_id <chipname>_id[] = { 
    { "<chipname>", 0 },		/* matching by name; required for platform and i2c
									devices & drivers */
    // f.e.: { "pcf8563", 0 },
    { } 
};
MODULE_DEVICE_TABLE(<foo>, <chipname>_id);

/* 2. By DT 'compatible' property : for devices on the Device Tree
 *                                 (ARM32, Aarch64, PPC, etc)
 */
#ifdef CONFIG_OF
static const struct of_device_id <chipname>_of_match[] = {
						/* ARM/PPC/etc: matching by DT 'compatible' property */
    { .compatible = "<manufacturer>,<model>" },
 // f.e.:   { .compatible = "nxp,pcf8563" },
    {}
};
MODULE_DEVICE_TABLE(of, <chipname>_of_match);
#endif

/* 3. By ACPI ID : for devices on ACPI tables (x86) */
static const struct acpi_device_id <chipname>_acpi_id[] = {
						/* x86: matching by ACPI ID */
	{"MMA7660", 0},
	{}
};
MODULE_DEVICE_TABLE(acpi, <chipname>_acpi_id);



/*
 * The <foo>_driver structure:
 * where <foo> is one of:
 *  acpi_button, cnic, cpufreq, gameport, hid, i2c, ide_pci, ipmi, mbus, mmc,
 *  pci, pnp, scsi, sdio, serio, spi, tty, usb, vme
 */
static struct <foo>_driver <chipname>_driver = {
	.driver     = {
		.name   = "<driver-name-used-for-match>",  /* platform and I2C use the
					'name' field for the match and thius the bind between the
					DT desc/device and driver */
		.of_match_table = of_match_ptr(<chipname>_of_match),
	},
	.probe      = <chipname>_probe,		// invoked on driver/device bind
	.remove     = <chipname>_remove,	// optional; invoked on driver/device detach
	.disconnect = <chipname>_disconnect,// optional; invoked on device disconnect

	.id_table   = <chipname>_id,

	.suspend    = <chipname>_suspend,	// optional
	.resume     = <chipname>_resume,	// optional
};


/* 
 * Init and Cleanup ::
 * Instead of manually specifiying the init and cleanup handlers in the
 * 'usual' manner, a lot of boilerplate is avoided (when nothing special is
 * required in the init and cleanup code paths) by using the
 *
 * module_foo_driver() macro;
 *
 * where <foo> is one of:
 *  acpi_button, cnic, cpufreq, gameport, hid, i2c, ide_pci, ipmi, mbus, mmc,
 *  pci, pnp, scsi, sdio, serio, spi, tty, usb, vme
 */
module_<foo>_driver(&<chipname>_driver);
