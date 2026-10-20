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
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define MAJORNO 190
#define MINORNO  0
#define DRV_NAME "cdrv"
#define COUNT 1
#define SUCCESS 0








/**
************* Important Note: ****************
*If you are writing this device driver for a real device, you will need to use the platform device's resources
*If this is a virtual device, you can skip the resource allocation part and just register the device with the kernel.

*If this driver happens for controller e.g I2C/SPI/UART CONTROLLER, Then upper layer will be bus manager.
*If this driver happens for a peripheral device e.g I2C/SPI/UART end devce, then we have to choose how we will bind this
 to upper layereither char driver or block driver or network driver or any other type of driver for application layer. In this case, we are choosing char driver. 
 
 Binding will be done at bus manager level for controller and at device driver level for peripheral device. In this case, we are binding at device driver level.
 */










static struct drv_prv {
	struct cdev *veda_cdev;
	dev_t mydev;
	struct device *dev;
} ctx;

static int vdev_open(struct inode *inode, struct file *file)
{
	dump_stack();
	dev_dbg(ctx.dev, "%s() invoked \n", __func__);
	return SUCCESS;
}

static int vdev_release(struct inode *inode, struct file *file)
{
	dump_stack();
	dev_dbg(ctx.dev, "%s() invoked \n", __func__);
	return SUCCESS;
}

static ssize_t vdev_write(struct file *file, const char __user
			  * buf, size_t lbuf, loff_t * offset)
{
	dump_stack();
	dev_dbg(ctx.dev, "%s() invoked:Rec'vd data of len = %ld\n", __func__,
		lbuf);
	return lbuf;
}

static ssize_t vdev_read(struct file *file, char __user * buf,
			 size_t count, loff_t * off)
{
	dump_stack();
	dev_dbg(ctx.dev, "%s() invoked.\n"
		"request to read %zu bytes\n", __func__, count);
	return count;
}

static struct file_operations vdev_fops = {
	.owner = THIS_MODULE,
	.write = vdev_write,
	.read = vdev_read,
	.open = vdev_open,
	.release = vdev_release
};

/*
 *
 * 1. Gather device config data
 * 2. Carry out device init operations(if any)
 * 3. Bind driver operations with the upper layer
 *
 */
static int drv_probe(struct platform_device *pdev)
{

	/*
	1. Gather device config data.
	2. Carry out device init operations(if any e,g registering with bus manager, configuring device registers, etc)
	3. Bind driver operations with the upper layer.
	4. Character device is abstracted as file operations. So, we need to register the device with the kernel and bind the file operations with the device.
	*/
	int ret;
	ctx.dev = &pdev->dev;

	ctx.mydev = MKDEV(MAJORNO, MINORNO);
	register_chrdev_region(ctx.mydev, COUNT, DRV_NAME);

	/* Allocate cdev instance */
	ctx.veda_cdev = cdev_alloc();

	/* initialize cdev with fops object */
	cdev_init(ctx.veda_cdev, &vdev_fops);

	/* register cdev */
	ret = cdev_add(ctx.veda_cdev, ctx.mydev, COUNT);
	if (ret < 0) {
		pr_err("Error registering device driver");
		return ret;
	}
	dev_dbg(ctx.dev, "Driver Registered %s\n", DRV_NAME);
	return SUCCESS;
}

static int drv_remove(struct platform_device *pdev)
{
	/* remove cdev */
	cdev_del(ctx.veda_cdev);

	/* free major/minor no's used */
	unregister_chrdev_region(ctx.mydev, COUNT);

	dev_dbg(ctx.dev, "%s: Device detached\n", __func__);
	return 0;
}

static struct platform_driver sample_platdrv = {
	.probe = drv_probe,
	.remove = drv_remove,
	.driver = {
		   .name = "vDev0"	// matches platform device name
		   },
};

module_platform_driver(sample_platdrv);

MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION("char driver skeleton");
MODULE_LICENSE("Dual MIT/GPL");
