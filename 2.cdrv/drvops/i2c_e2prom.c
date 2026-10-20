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

#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/mutex.h>

/*
 *Driver private struct to hold global config data 
 */

struct eeprom_data_prv {
	struct i2c_client client_prv;
	struct kobject *eeprom_kobj;
	char *ptr;
	unsigned int size;
	unsigned int pagesize;
	unsigned int address_width;
	unsigned int page_no;
};

struct eeprom_data_prv *prv = NULL;

/*
* Steps:
*  1. Register the driver with i2c subsystem using i2c_add_driver() 
*/





static ssize_t
eeprom_device_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	struct i2c_msg msg[2];

	u8 data[2];

	int offset, ret;
	offset = prv->page_no * 32;

	msg[0].addr = prv->client_prv.addr;
	msg[0].flags = 0;
	msg[0].len = 2;

	data[0] = offset >> 8;
	data[1] = offset;
	
	msg[0].buf = data;

	msg[1].addr = prv->client_prv.addr;
	msg[1].flags = 1;
	msg[1].len = prv->pagesize;

	msg[1].buf = buf;

	ret = i2c_transfer(prv->client_prv.adapter, msg, 2);
	if (ret < 0) {
		pr_err("i2c transfer failed\n");
		return -1;
	}
	return prv->pagesize;
}

static ssize_t
eeprom_device_write(struct kobject *kobj, struct kobj_attribute *attr,
		    const char *buf, size_t count)
{
	struct i2c_msg msg[2];

	u8 data[2];

	int offset, ret;

	offset = prv->page_no * 32;
	
	
	msg[0].addr = prv->client_prv.addr;
	msg[0].flags = 0;
	msg[0].len = 2;
	data[0] = offset >> 8;
	data[1] = offset;
	msg[0].buf = data;


	msg[1].addr = prv->client_prv.addr;
	msg[1].flags = 0;
	msg[1].len = prv->pagesize;
	msg[1].buf = (u8 *) buf;

	ret = i2c_transfer(prv->client_prv.adapter, msg, 2);
	if (ret < 0) {
		pr_err("i2c transfer failed\n");
		return -1;
	}

	return count;
}

static ssize_t
eeprom_pageno_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	sprintf(buf, "%d", prv->page_no);
	return prv->page_no;
}

static ssize_t
eeprom_pageno_write(struct kobject *kobj, struct kobj_attribute *attr,
		    const char *buf, size_t count)
{
	int ret;
	unsigned int pageno;
	ret = kstrtou8(buf, 10, (u8 *) &pageno);
	if (ret < 0) {
		pr_err("kstrtou8 failed\n");
		return -1;
	}
	if (pageno < 0 || pageno > 128) {
		pr_info("128 pages are available\n");
		pr_info("choose pages from 0x0 - 0x80\n");
		return -ENOMEM;
	}
	prv->page_no = pageno;
	pr_info("page : %d\n", prv->page_no);
	return count;
}

static struct kobj_attribute eeprom_device_attribute =
__ATTR(eeprom_device, 0664, eeprom_device_read, eeprom_device_write);
static struct kobj_attribute eeprom_pageno_attribute =
__ATTR(eeprom_pageno, 0664, eeprom_pageno_read, eeprom_pageno_write);

static struct attribute *attrs[] = {
	&eeprom_device_attribute.attr,
	&eeprom_pageno_attribute.attr,
	NULL,			/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int
i2c_slave_eeprom_24c32_probe(struct i2c_client *client,
			     const struct i2c_device_id *id)
{
	int retval;

	prv =
	    (struct eeprom_data_prv *)kzalloc(sizeof(struct eeprom_data_prv),
					      GFP_KERNEL);
	if (prv == NULL) {
		pr_info("DATA not allocated");
	}

	prv->client_prv = *client;

	if (device_property_read_u32(&client->dev, "size", &prv->size) == 0) {
	} else {
		dev_err(&client->dev, "Error: missing \"size\" property \n");
		return -ENODEV;
	}

	if (device_property_read_u32(&client->dev, "pagesize", &prv->pagesize)
	    == 0) {
	} else {
		dev_err(&client->dev,
			"Error: missing \"pagesize\" property \n");
		return -ENODEV;
	}

	if (device_property_read_u32
	    (&client->dev, "address-width", &prv->address_width) == 0) {
	} else {
		dev_err(&client->dev,
			"Error: missing \"address_width\" property \n");
		return -ENODEV;
	}


	prv->eeprom_kobj =
	    kobject_create_and_add("at24c32_eeprom", NULL /* kernel_kobj */ );
	if (prv->eeprom_kobj == NULL) {
		pr_err("failed to create a object in sysfs : i2c\n");
		return -ENOMEM;
	}

	retval = sysfs_create_group(prv->eeprom_kobj, &attr_group);
	if (retval) {
		pr_err("sysfs_create_group failed\n");
		kobject_put(prv->eeprom_kobj);
		return -ENOMEM;
	}

	return retval;
}

static int i2c_slave_eeprom_24c32_remove(struct i2c_client *client)
{
	kobject_put(prv->eeprom_kobj);
	return 0;
}


/*
 * Device ID table
 * As this is bus specific driver, we need to provide a list of devices that this driver can support.
 * This is used by the i2c core to match the device with the driver.
 * The device name in the dts file should match with the name provided in this table.
 * In platform devices, the name of the device is provided in the platform_device struct.
 */
static const struct i2c_device_id i2c_slave_eeprom_24c32_id[] = {
	{"e2prom", 0x50},  //	0x50 bus address of eeprom device / 'e2prom' device name in dts file
	{}
};

MODULE_DEVICE_TABLE(i2c, i2c_slave_eeprom_24c32_id);

static struct i2c_driver i2c_slave_eeprom_24c32_driver = {
	.driver = {
		   .name = "i2c-slave-eeprom",
		   .owner = THIS_MODULE,
		   },
	.probe = i2c_slave_eeprom_24c32_probe,
	.remove = i2c_slave_eeprom_24c32_remove,
	.id_table = i2c_slave_eeprom_24c32_id,
};
/*
 * This macro will create init and exit functions for the driver and register it with i2c subsystem
 * It will call i2c_add_driver() and i2c_del_driver() functions internally.
 */
module_i2c_driver(i2c_slave_eeprom_24c32_driver);

MODULE_AUTHOR("Raghu Bhardawaj");
MODULE_DESCRIPTION("i2c-eeprom-driver");
MODULE_LICENSE("GPL");
