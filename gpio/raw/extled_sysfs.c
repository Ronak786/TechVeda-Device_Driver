/*
 * techveda.org
 * driver for off board LED connected on GPIO bank 0 pin 27
 * which is located on P8 header pin 17
 * By default the pin is configured as ACTIVE LOW
 * uses sysfs interface
 * GNU General Public License
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
#include "onboard_led.h"

static struct gpiopin_bank *gpbank;
static void *gbank_base;
static struct kobject *kobj;
static int ledinit;

void led_init(void)
{
	unsigned int data = 0;
	
	ledinit = 1;
	data = readl_relaxed(gbank_base + OMAP_GPIO_OE);
	data = data & 0xF7FFFFFF;
	pr_info("led_init: direction of pin is set: %x\n",data);
	writel_relaxed(data, gbank_base + OMAP_GPIO_OE);
	return;
}

static ssize_t extled_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
	unsigned int data;
	char rbuf;
	
	pr_info("extled_read: invoked from sysfs...\n");
	if (!ledinit)
		led_init();
        data = readl_relaxed(gbank_base + OMAP_GPIO_DATAIN);
	data = data & (1U << gpbank->pinno) ? 1 : 0; /* 0 => off; 1 => on */
	if(data) {
		rbuf = 1;
		pr_info("extled_read: led is on...\n");

		return sprintf(buf,"%d\n",rbuf);
	}
	else {
		rbuf = 0;
		pr_info("extled_read: led is off...\n");
		return sprintf(buf,"%d\n",rbuf);
	}
}

static ssize_t extled_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int data;
	int d;
        int ret;

	pr_info("extled_write: invoked from sysfs...\n");
	if (!ledinit)
		led_init();

	ret = sscanf(buf,"%d",&d);
	if(ret < 0 || ret > 1 || d > 1)
               return -EINVAL;

	data = readl_relaxed(gbank_base + OMAP_GPIO_DATAIN);
	
	if(d==0) {
		pr_info("extled_write: led is off\n");
		data = data | (1U << gpbank->pinno);
		writel_relaxed(data, gbank_base + OMAP_GPIO_CLEARDATAOUT);
	}
        else {
		pr_info("extled_write: led is on\n");
		data = data | (1U << gpbank->pinno);
		writel_relaxed(data, gbank_base + OMAP_GPIO_SETDATAOUT);
	}
	return data;
}

static struct kobj_attribute led_attr =__ATTR(extled, 0660, extled_read, extled_write);

static int onboardled_probe(struct platform_device *pdev)
{
	struct device_node *pnode;
	int ret;

	/* instantiate private structure */
   	gpbank = (struct gpiopin_bank *)kmalloc(sizeof(struct gpiopin_bank),GFP_KERNEL);

	gpbank->pinno = of_get_named_gpio(pdev->dev.of_node, "gpios", 0);
	pr_info("Probe: gpio pin no for led device: %u\n", gpbank->pinno);

	pnode = of_parse_phandle(pdev->dev.of_node, "gpios", 0);	

	ret = of_property_read_u32(pnode, "reg", &gpbank->gpio_base);
	pr_info("Probe: gpio bank base addr: %x\n", gpbank->gpio_base);

	gbank_base = ioremap(gpbank->gpio_base, 4095);
	if (gbank_base == NULL) 
		return -1;
	pr_info("Probe: gbank_base address as seen by kernel: %p\n", gbank_base);

	
	kobj = kobject_create_and_add("ext_led", NULL);
	if(!kobj)
		return -ENOMEM;

	ret = sysfs_create_file(kobj, &led_attr.attr);
	if(ret)
		kobject_put(kobj);

	return 0;
}

static int onboardled_remove(struct platform_device *pdev)
{
	iounmap(gbank_base);
	kobject_put(kobj);
	kfree(gpbank);
	pr_info("onboardled device: %s driver removed\n",pdev->name);
        return 0;
}

static const struct of_device_id onboardled_of_mtable[] = {
        { .compatible = "gpio-extled", },
	{}
};

MODULE_DEVICE_TABLE(of, onboardled_of_mtable);
 
static struct platform_driver onboardled_driver = {
         .driver = {
                 .name    = "onboard_led",
		 .owner   = THIS_MODULE,		
                 .of_match_table = onboardled_of_mtable, /* structure used for matching a device */
         },
         .probe = onboardled_probe,
         .remove = onboardled_remove,
};
 
static int mymod_init(void)
{
        pr_info("on-board led corresponding driver inserted...\n");
        return platform_driver_register(&onboardled_driver);
}

static void mymod_exit(void)
{
        pr_info("on-board led node corresponding driver ousted...\n");
        platform_driver_unregister(&onboardled_driver);
}

module_init(mymod_init);
module_exit(mymod_exit);

MODULE_AUTHOR("techveda.org");
MODULE_DESCRIPTION("platform driver for off-board led device");
MODULE_LICENSE("GPL");
