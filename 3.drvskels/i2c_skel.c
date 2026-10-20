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

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>


#define EEPROM_SIZE 256 // size of mini2440 eeprom

MODULE_DESCRIPTION("Driver for I2C EEPROM");
MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_LICENSE("Dual MIT/GPL");

/*
 * Called by i2c-core when the i2c device with the address
 * 0x50 is found on the adapter.
 *
 * Inputs - client - i2c device pointer
 *          id     - the id that matched 
 * 
 * Return 0 on succefull probing, negative number
 * on failure
 */
static int eeprom_probe(struct i2c_client *client,
                        const struct i2c_device_id *id)
{

	pr_info("probe invoked \n");
        return 0;
}

/*Will be called from the i2c-core when our eeprom disappears*/
static int eeprom_remove(struct i2c_client *client)
{
	return 0;
}

/*seek Access to device with address 0x50 and enumerated with string "24c08"*/
static struct i2c_device_id eeprom_ids[] = {
        { "24c08",0x50 }, 
	{ } // end of the list
};

static struct i2c_driver eeprom_driver = {
	.driver = {
		.name = "eeprom",
		.owner = THIS_MODULE,
	},
	.probe = eeprom_probe,
	.remove = eeprom_remove,
	.id_table = eeprom_ids,
};

module_i2c_driver(eeprom_driver);
