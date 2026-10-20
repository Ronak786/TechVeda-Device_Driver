/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * ******************************************************************************
 * This source code is part of the coursware provided with "Linux Device Drivers"
 * training program offered by Techveda <www.techveda.org>
 *
 * Copyright (C) 2020 Techveda
 *
 * Author: Raghu Bharadwaj
 *
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>

#include "rtc.h"

struct device *dev;
static unsigned char get_rtc(unsigned char offset)
{
	outb(offset, ADDRESS_REG);
	return inb(DATA_REG);
}

static void set_rtc(unsigned char data, unsigned char offset)
{

	outb(offset, ADDRESS_REG);
	outb(data, DATA_REG);
}
/* open-->| sys_open--> drv_open
  1. verify calling application's access privilages to enforce choosen driver access policy
  2. init app specific resources(if any)
*/

static int rtc_open(struct inode *rtc_inode, struct file *file)
{
	dev_info(dev,"%s: rtc node opened\n",__func__);
	return 0;
}
/* close-->|sys_release--> drv_release

*/
static int rtc_close(struct inode *rtc_inode, struct file *file)
{
	dev_info(dev,"%s: rtc node closed\n",__func__);
	return 0;
}


/*
  read -> | sys_read-> driv_read() 
  char buf[100];
  read(fd, buf, 100);

   step 1: validate read request
   step 2: verify if device is ready to be read
   step 3: program device and gather requested data
   step 4: process data if needed
   step 5: transfer data to application buffer
   step 6: return no of bytes transferred
*/

static ssize_t
rtc_read(struct file *file, char __user * buf, size_t cnt, loff_t * f_pos)
{
	unsigned long ret;
	struct rtc_time time = { 0 };

	if (cnt != sizeof(struct rtc_time)) {
		dev_err(dev, "invalid request");
		return -EINVAL;
	}
	dev_info(dev, "%s: rtc read invoked\n",__func__);

	time.sec = get_rtc(SECOND);
	time.min = get_rtc(MINUTE);
	time.hour = get_rtc(HOUR);
	time.day = get_rtc(DAY);
	time.mon = get_rtc(MONTH);
	time.year = get_rtc(YEAR);

	ret = copy_to_user(buf, &time, sizeof(time));

	return cnt;
}


/*  write -> sys_write--> driv_write
    char buf[100];
    write(fd, buf, 100);

   step 1: validate write request
   step 2: verify if device is ready 
   step 3: Transfer data from caller buffer 
   step 4: process data if needed
   step 5: program device & initiate relavent operation
   step 6: return status
*/
static ssize_t
rtc_write(struct file *file, const char __user * buf, size_t cnt,
	  loff_t * f_pos)
{

	unsigned int ret;
	 struct rtc_time time = { 0 };

        if (cnt != sizeof(struct rtc_time)) {
                dev_err(dev, "invalid request");
                return -EINVAL;
        }

	ret = copy_from_user(&time, buf, sizeof(time));

        set_rtc(time.sec, SECOND);
        set_rtc(time.min, MINUTE);
        set_rtc(time.hour, HOUR);
        set_rtc(time.day, DAY);
        set_rtc(time.mon, MONTH);
        set_rtc(time.year, YEAR);

	return cnt;
}

static struct file_operations rtc_ops = {
	.open = rtc_open,
	.release = rtc_close,
	.read = rtc_read,
	.write = rtc_write,
};

static struct miscdevice RtcMisc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEV_NAME,
	.fops = &rtc_ops,
};

static int __init rtc_init(void)
{
	int ret;
	dev = RtcMisc.this_device;

	ret = misc_register(&RtcMisc);
	if (ret < 0) {
		dev_err(dev, "%s: failed registering with minor %d",__func__, RtcMisc.minor);
		return ret;
	}

	dev_info(dev,"%s: driver registered with Minor %d",__func__, RtcMisc.minor);

	return SUCCESS;
}

static void __exit rtc_exit(void)
{
	misc_deregister(&RtcMisc);

}

module_init(rtc_init);
module_exit(rtc_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION("LKD_CW: sample char driver for cmos realtime clock");
