/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * ******************************************************************************
 * This program is part of the source code provided with "Linux Device Drivers"
 * Course offered by Techveda <www.techveda.org>
 *
 * Copyright (C) 2020 Techveda
 * Author: Raghu Bharadwaj
 * Git repository:
 *   https://gitlab.com/techveda/ldd-0920
 * ******************************************************************************
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/printk.h>

#define DRV_NAME "cdrv_udev"
#define COUNT 1
#define SUCCESS 0

static struct drv_prv {
  struct cdev *veda_cdev;
  dev_t mydev;
  struct device *dev;
  struct class *vclass;
  struct device *dev_ret;
} ctx;

static int vdev_open(struct inode *inode, struct file *file) {
  dev_info(ctx.dev,
           "%s() invoked\n"
           " minor # is %d\n",
           __func__, iminor(inode));

  return SUCCESS;
}

static int vdev_release(struct inode *inode, struct file *file) {
  dev_info(ctx.dev, "%s() invoked \n", __func__);
  return SUCCESS;
}

static ssize_t vdev_write(struct file *file, const char __user *buf,
                          size_t lbuf, loff_t *offset) {
  dev_info(ctx.dev, "%s() invoked:Rec'vd data of len = %ld\n", __func__, lbuf);
  return lbuf;
}

static ssize_t vdev_read(struct file *file, char __user *buf, size_t count,
                         loff_t *off) {
  dev_info(ctx.dev,
           "%s() invoked.\n"
           "request to read %zu bytes\n",
           __func__, count);
  return count;
}

static struct file_operations vdev_fops = {.owner = THIS_MODULE,
                                           .write = vdev_write,
                                           .read = vdev_read,
                                           .open = vdev_open,
                                           .release = vdev_release};

static int drv_probe(struct platform_device *pdev) {
  int ret;
  int baseminor = 0;
  ctx.dev = &pdev->dev;
  alloc_chrdev_region(&(ctx.mydev), baseminor, COUNT, DRV_NAME);

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

  /* Automate /dev by class_create and device_create */
  /* Class creation under /sys/class/vDev */
  /* This just serve to crate udev rules */
  ctx.vclass = class_create(THIS_MODULE, "vDev");
  if (!ctx.vclass) {
    unregister_chrdev_region(ctx.mydev, COUNT);
    return PTR_ERR(ctx.vclass);
  }

  /* Device creation under /sys/class/vDev */
  /* So vDevnode has information of Major number and minor number in 'uevent'
   * This is used by udev(User Space demon)to create device node under
   * /dev/vDevnode with Major and minor number information */
   
     ctx.dev_ret =
      device_create(ctx.vclass, &pdev->dev, ctx.mydev, NULL, "vDevnode");
  if (!ctx.dev_ret) {
    class_destroy(ctx.vclass);
    unregister_chrdev_region(ctx.mydev, COUNT);
    return PTR_ERR(ctx.dev_ret);
  }
  dev_info(ctx.dev, "Driver Registered %s\n", DRV_NAME);
  return SUCCESS;
}

static int drv_remove(struct platform_device *pdev) {
  /* remove cdev */
  cdev_del(ctx.veda_cdev);

  /* free major/minor no's used */
  unregister_chrdev_region(ctx.mydev, COUNT);

  device_destroy(ctx.vclass, ctx.mydev);
  class_destroy(ctx.vclass);

  dev_info(ctx.dev, "%s: Device detached\n", __func__);
  return 0;
}

static struct platform_driver sample_platdrv = {
    .probe = drv_probe,
    .remove = drv_remove,
    .driver =
        {
            .name = "vDev0" // matches platform device name
        },
};

module_platform_driver(sample_platdrv);

MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION("char driver skeleton");
MODULE_LICENSE("Dual MIT/GPL");
