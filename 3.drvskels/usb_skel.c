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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION("Usb driver initialization");

#define VENDOR 0x090c
#define DEVICE 0x1000  

static int test_probe(struct usb_interface *intf, const struct usb_device_id *id)
{

	struct usb_device *dev = interface_to_usbdev(intf);

	pr_info("\nmy_usb_probe\n");
	pr_info("devnum=%d, speed=%d\n", dev->devnum, (int)dev->speed);
	pr_info("idVendor=0x%hX, idProduct=0x%hX, bcdDevice=0x%hX\n",
		dev->descriptor.idVendor,
		dev->descriptor.idProduct, dev->descriptor.bcdDevice);
	pr_info("class=0x%hX, subclass=0x%hX\n",
		dev->descriptor.bDeviceClass, dev->descriptor.bDeviceSubClass);
	pr_info("protocol=0x%hX, packetsize=%hu\n",
		dev->descriptor.bDeviceProtocol,
		dev->descriptor.bMaxPacketSize0);
	pr_info("manufacturer=0x%hX, product=0x%hX, serial=%hu\n",
		dev->descriptor.iManufacturer, dev->descriptor.iProduct,
		dev->descriptor.iSerialNumber);

	return 0;
}

static void test_disconnect(struct usb_interface *intf)
{
	pr_info("my_usb_disconnect\n");
}

/* structure to specify device identification details */
static struct usb_device_id dev_table[] = {
	{USB_DEVICE(VENDOR,DEVICE)},	/* device */
	{}				/* Null terminator (required) */
};


MODULE_DEVICE_TABLE(usb, dev_table);

/* structure to register with usb-core  */
static struct usb_driver my_usb_driver = {
	.name = "usb-test",
	.probe = test_probe,  // driver's init call
	.disconnect = test_disconnect, // driver's exit call
	.id_table = dev_table,// table of devices we are looking for 
};

/* Helper Macro to replace module init/exit to get driver object registered 
   with Usb core */
module_usb_driver(my_usb_driver);
