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

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>    
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/uaccess.h> 
#include <linux/miscdevice.h>
#include "clock.h"

struct device *dev;
static unsigned char inuse = 0; 

unsigned char cmd_arr[]={
	DAY_CMD,
	MONTH_CMD,
	YEAR_CMD,
	HOURS_CMD,
	MINUTES_CMD,
	SECONDS_CMD
	};
/* we will allow only one app to access RTC at a point of time. */

int clock_open(struct inode *inode, struct file *filp)
{
	if(inuse)
		return -EBUSY;
	inuse = 1;// indicating this is in use.
	return 0;
}

int clock_release(struct inode *inode, struct file *filp)
{
	inuse = 0;
	return 0;
}

ssize_t clock_write(struct file *filp, const char *buf, size_t count,
		    loff_t *f_pos)
{
	unsigned char cmd,data;
	if(count > 1)
		return -EIO; // IO error.
	if(*f_pos >= FILE_SIZE)
		return -EIO; // IO error.
	cmd = cmd_arr[*f_pos];
	if( copy_from_user(&data,buf,1))
		return -EFAULT;
	dev_info(dev, "cmd = %d  data = %d\n",cmd,data);
	WRITE_TO_CLOCK(cmd,data);		
	(*f_pos)++;
	return 1;
}

ssize_t clock_read(struct file *filp,char *buf, size_t count,
		    loff_t *f_pos)
{
	unsigned char cmd=0,data=0;
	data=0;
	if(count > 1)
		return -EIO; // IO error.
	if(*f_pos >= FILE_SIZE)
		return -EIO; // IO error.
	cmd = cmd_arr[*f_pos];
	READ_FROM_CLOCK(cmd,data);
	if (copy_to_user(buf,&data,1))
		return -EFAULT;
	dev_info(dev, "data = %d cmd = %d \n",data,cmd);
	(*f_pos)++;
	return 1;
}
struct file_operations clock_fops = {
	write: clock_write,
 	read: clock_read,
	open: clock_open,
	release: clock_release
};

static struct miscdevice RtcMisc = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "clock",
        .fops = &clock_fops,
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
        dev_info(dev, "%s: driver registered with Minor %d",__func__, RtcMisc.minor);

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


