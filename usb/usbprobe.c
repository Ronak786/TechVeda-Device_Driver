/*
 * ******************************************************************************
 * This program is part of the source code provided with "Linux Device Drivers"
 * Course offered by Techveda <www.techveda.org>
 *
 * (C) 2020 Raghu Bharadwaj/Techveda
 *
 * Git repository:
 *   https://gitlab.com/techveda/ldd
 * ******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
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

#define VENDOR 0x0781
#define DEVICE 0x5567

static int
test_probe(struct usb_interface *intf, const struct usb_device_id *id)
{

	/* driver initialization code */
	struct usb_device *dev = interface_to_usbdev(intf);

	pr_info("%s:my_usb_probe\n",__func__);
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

static struct usb_device_id dev_table[] = {
	{USB_DEVICE(VENDOR,DEVICE)},	/* sandisk usb storage device id's */
	{}				/* Null terminator (required) */
};

MODULE_DEVICE_TABLE(usb, dev_table);

static struct usb_driver my_usb_driver = {
	.name = "usb-test",
	.probe = test_probe,  // driver's init call
	.disconnect = test_disconnect, // driver's exit call
	.id_table = dev_table,
};

static int __init test_init(void)
{
	int err;
	pr_info("%s:Hello USB\n",__func__);
	err = usb_register(&my_usb_driver);
	return err;
}

static void __exit test_exit(void)
{
	pr_info("%s:Goodbye USB\n",__func__);
	usb_deregister(&my_usb_driver);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
