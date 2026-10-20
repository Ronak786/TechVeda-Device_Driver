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

#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include "rtc.h"

static DEFINE_MUTEX(mymtx);

static unsigned char get_rtc(unsigned char addr)
{
	int ret;
	unsigned char c;
	ret = mutex_lock_killable(&mymtx);
	if (ret < 0) {
		pr_err("interrupted while waiting for mutex\n");
		return -1;
	}
	outb(addr, ADDRESS_REG);
	c = inb(DATA_REG);
	mutex_unlock(&mymtx);
	return c;
}

static int set_rtc(unsigned char data, unsigned char addr)
{

	int ret;
	ret = mutex_lock_killable(&mymtx);
	if (ret < 0) {
		pr_err("interrupted while waiting for mutex\n");
		return -EINTR;
	}
	outb(addr, ADDRESS_REG);
	outb(data, DATA_REG);
	mutex_unlock(&mymtx);
	return 0;
}

/* read routine for time entry */
static ssize_t tm_show(struct kobject *kobj, struct kobj_attribute *attr,
		       char *buf)
{
	int cnt;
	struct rtc_time time = { 0 };

	pr_info("%s: Invoked\n", __func__);

	time.sec = get_rtc(SECOND);
	time.min = get_rtc(MINUTE);
	time.hour = get_rtc(HOUR);

	//Copying aquired data to "/sys/time" entry
	cnt =
	    sprintf(buf, "%02hhx:%02hhx:%02hhx", time.hour, time.min, time.sec);
	return cnt;

}

/* write routine for time entry */
static ssize_t tm_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	struct rtc_time time = { 0 };

	pr_info("%s: Invoked\n", __func__);

	sscanf(buf, "%hhx:%hhx:%hhx", &time.hour, &time.min, &time.sec);
	set_rtc(time.sec, SECOND);
	set_rtc(time.min, MINUTE);
	set_rtc(time.hour, HOUR);

	return count;

}

/* read routine for date entry */
static ssize_t dt_show(struct kobject *kobj, struct kobj_attribute *attr,
		       char *buf)
{
	int ret;
	struct rtc_time time = { 0 };

	pr_info("%s: Invoked\n", __func__);

	time.day = get_rtc(DAY);
	time.mon = get_rtc(MONTH);
	time.year = get_rtc(YEAR);

	//Copying acquired data to "/sys/date" entry
	ret =
	    sprintf(buf, "%02hhx:%02hhx:%02hhx", time.day, time.mon,
		    time.year);

	return ret;

}

/* function invoked when file in sysfs is updated through echo */
static ssize_t dt_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	struct rtc_time time = { 0 };

	pr_info("%s: Invoked\n", __func__);

	sscanf(buf, "%hhx:%hhx:%hhx", &time.day, &time.mon, &time.year);
	set_rtc(time.day, DAY);
	set_rtc(time.mon, MONTH);
	set_rtc(time.year, YEAR);

	return count;

}

/* Linking routines to particular entry */
/* Use __ATTR family to ensure that naming convention */


static struct kobj_attribute tm_attribute =
__ATTR(time, 0644, tm_show, tm_store);
static struct kobj_attribute dt_attribute =
__ATTR(date, 0644, dt_show, dt_store);


static struct attribute *attrs[] = {
	&tm_attribute.attr,
	&dt_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *my_rtc;

static int __init rtc_init(void)
{
	int retval;

	pr_info("%s: Invoked\n", __func__);

	/*
	 * kobject_create_and_add - create a struct kobject dynamically and 
           register it with sysfs
	 */
	my_rtc = kobject_create_and_add("my_rtc", NULL /*kernel_kobj */ );
	if (!my_rtc)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(my_rtc, &attr_group);
	if (retval)
		kobject_put(my_rtc);

	return retval;
}

static void __exit rtc_exit(void)
{
	pr_info("%s: Invoked\n", __func__);
	/* Removing sysfs entry */
	kobject_put(my_rtc);
}

module_init(rtc_init);
module_exit(rtc_exit);

MODULE_LICENSE("GPL");
