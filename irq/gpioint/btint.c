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

#include <linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/platform_device.h>
#include<linux/slab.h>
#include<linux/of_gpio.h>
#include<linux/kobject.h>
#include<linux/sysfs.h>
#include<asm/io.h>

unsigned int pinno;
int irq;
int irq_counter;

static irqreturn_t interrupt_handler(int irq,void *dev_id)
{
        irq_counter++;
        pr_info("In the ISR; counter = %d\n",irq_counter);
        return IRQ_HANDLED;
}

static int gpiobt_probe(struct platform_device *pdev)
{
	int ret;
	pr_info("gpiobt: %s invoked\n",pdev->name);
	
	pinno = of_get_named_gpio(pdev->dev.of_node , "gpios",0);
	
	if(gpio_request(pinno,"gpio-button")<0)
	{
		pr_err("gpio_request failed\n");
		return -EINVAL;
	}

	gpio_set_debounce(pinno,200);

	irq = gpio_to_irq(pinno);
	if(irq < 0)
	{
		pr_err("gpio to irq failed\n");
		return -EINVAL;
	}

	ret = request_irq(irq, interrupt_handler, IRQF_TRIGGER_RISING, "gpio-button-interrupt", NULL);

	return 0;
}

static int gpiobt_remove(struct platform_device *pdev)
{
	pr_info("onboardled device: %s driver removed\n",pdev->name);
	free_irq(irq,NULL);
	gpio_free(pinno);
        return 0;
}

static const struct of_device_id gpiobt_of_mtable[] = {
	{ .compatible = "gpio-button",},
	{},
};

MODULE_DEVICE_TABLE(of,gpiobt_of_mtable);

static struct platform_driver gpiobt_driver = {
	.driver = {
		.name = "onboard_button",
		.owner = THIS_MODULE,
		.of_match_table = gpiobt_of_mtable, /* structure used for matching a device */
	},
	.probe = gpiobt_probe,
	.remove = gpiobt_remove,
};

module_platform_driver(gpiobt_driver);

MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION("GPIO interrupts");
MODULE_LICENSE("Dual MIT/GPL");
