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
#include <linux/miscdevice.h>

#define DRV_NAME "cdrv_misc"
#define COUNT 1
#define SUCCESS 0

struct device *dev;

static int vdev_open(struct inode *inode, struct file *file)
{
	 dev_dbg(dev, "%s() invoked\n"
                " minor # is %d\n",
                __func__, iminor(inode));

	return -1;
}

static int vdev_release(struct inode *inode, struct file *file)
{
	dev_dbg(dev, "%s() invoked \n", __func__);
	return SUCCESS;
}

static ssize_t vdev_write(struct file *file, const char __user
			  * buf, size_t lbuf, loff_t * offset)
{
	dev_dbg(dev, "%s() invoked:Rec'vd data of len = %ld\n", __func__,
		lbuf);
	return lbuf;
}

static ssize_t vdev_read(struct file *file, char __user * buf,
			 size_t count, loff_t * off)
{
	dev_dbg(dev, "%s() invoked.\n"
		"request to read %zu bytes\n", __func__, count);
	dump_stack();
	return count;
}

static struct file_operations vdev_fops = {
	.owner = THIS_MODULE,
	.write = vdev_write,
	.read = vdev_read,
	.open = vdev_open,
	.release = vdev_release
};

static struct miscdevice vDevMisc = {
        .minor = MISC_DYNAMIC_MINOR,    /* allocate misc minor */
        .name = DRV_NAME,
        .fops = &vdev_fops,         /* drivers fops instance */
};


static int drv_probe(struct platform_device *pdev)
{
	int ret;

	dev = &pdev->dev;
	ret = misc_register(&vDevMisc);
        if (ret < 0) {
        	dev_err(dev, "%s:failed registering with minor %d",__func__, vDevMisc.minor);
        	return ret;
        }

	dev_dbg(dev, "Driver Registered %s with Minor %d\n", DRV_NAME,vDevMisc.minor);
	return SUCCESS;
}

static int drv_remove(struct platform_device *pdev)
{
	misc_deregister(&vDevMisc);
	dev_dbg(dev, "%s: Device detached\n", __func__);
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
