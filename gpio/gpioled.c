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
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of_gpio.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <asm/io.h>

#define PINNO 27

static struct kobject *kobj;

static ssize_t extled_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int data;
	pr_info("extled_read: invoked from sysfs...\n");

	data = gpio_get_value(PINNO);
	if(data == 0)
		pr_info("extled_write: led is off\n");
	else if(data == 1)
		pr_info("extled_write: led is on\n");
	
	return data;
}

static ssize_t extled_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{

	pr_info("extled_write: invoked from sysfs...\n");
	if(*buf == '1') {
		gpio_set_value(PINNO,1);
		pr_info("extled_write: led is on\n");
	}
        else if(*buf == '0'){
                gpio_set_value(PINNO,0);
		pr_info("extled_write: led is off\n");
	}

	return count;
}

static struct kobj_attribute led_attr =__ATTR(tled, 0660, extled_read, extled_write);

static int onboardled_probe(struct platform_device *pdev)
{
	int ret;
	ret = gpio_request(PINNO, "gpio led");
        if (ret) {
                pr_err("gpio_rqueset failed\n");
                return ret;
        }

	ret = gpio_direction_output(PINNO,0);
	if(ret!=0 ){
		pr_err("gpio_direction_output failed\n");
                return ret;
        }

	kobj = kobject_create_and_add("gpioled", NULL);
	if(!kobj)
		return -ENOMEM;

	ret = sysfs_create_file(kobj, &led_attr.attr);
	if(ret)
		kobject_put(kobj);

	return 0;
}

static int onboardled_remove(struct platform_device *pdev)
{
	kobject_put(kobj);
	pr_info("onboardled device: %s driver removed\n",pdev->name);
        return 0;
}

static const struct of_device_id onboardled_of_mtable[] = {
	{ .compatible = "gpio-extled",},
	{},
};

MODULE_DEVICE_TABLE(of,onboardled_of_mtable);

static struct platform_driver onboardled_driver = {
	.driver = {
		.name = "onboard_led",
		.owner = THIS_MODULE,
		.of_match_table = onboardled_of_mtable, /* structure used for matching a device */
	},
	.probe = onboardled_probe,
	.remove = onboardled_remove,
};

module_platform_driver(onboardled_driver);

MODULE_AUTHOR("Raghu Bharadwaj");
MODULE_DESCRIPTION("gpio lib test");
MODULE_LICENSE("Dual MIT/GPL");
