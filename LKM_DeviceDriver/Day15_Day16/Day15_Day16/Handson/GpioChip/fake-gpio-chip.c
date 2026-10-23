#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>      /* For platform devices */
#include <linux/of.h>                   /* For DT*/

#define GPIO_NUM 16
static struct gpio_chip chip;

static int fake_get_value(struct gpio_chip *gc, unsigned offset)
{
	printk(KERN_ERR "Fake GPIO Chip Get Value\n");
	return 0;
}

static void fake_set_value(struct gpio_chip *gc, unsigned offset, int val)
{
	printk(KERN_ERR "Fake GPIO Chip Set Value\n");
}

static int fake_direction_output(struct gpio_chip *gc,
				       unsigned offset, int val)
{
	printk(KERN_ERR "Fake GPIO Chip direction out \n");
	return 0;
}

static int fake_direction_input(struct gpio_chip *gc,
				       unsigned offset)
{
	printk(KERN_ERR "Fake GPIO Chip direction input \n");
    return 0;
}

int fake_request(struct gpio_chip *chip, unsigned offset)
{
    printk(KERN_ERR "Fake GPIO fake_request \n");
    return 0;	
}

void fake_free(struct gpio_chip *chip, unsigned offset)
{
    printk(KERN_ERR "Fake GPIO fake_free\n");
}

static const struct of_device_id fake_gpiochip_ids[] = {
    { .compatible = "test,fake-gpio-chip", },
    { /* sentinel */ }
};

static int my_pdrv_probe (struct platform_device *pdev)
{
	int status;
	chip.label = pdev->name;
	chip.base = -1;
	//chip.parent = &pdev->dev;
	chip.owner = THIS_MODULE;
	chip.ngpio = GPIO_NUM;
	chip.can_sleep = 1;
	chip.request=fake_request;
	chip.free=fake_free;
	chip.get = fake_get_value;
	chip.set = fake_set_value;
	chip.direction_output = fake_direction_output;
	chip.direction_input = fake_direction_input;
	
	status = gpiochip_add(&chip);
	printk(KERN_ERR "Fake GPIO Chip added = %d\n",chip.base);
	return status;
}

static int my_pdrv_remove(struct platform_device *pdev)
{
	printk(KERN_ERR "Fake GPIO Chip removed\n");
	gpiochip_remove(&chip);
    return 0;
}

static struct platform_driver mypdrv = {
    .probe      = my_pdrv_probe,
    .remove     = my_pdrv_remove,
    .driver     = {
        .name     = "fake-gpio-chip",
        .of_match_table = of_match_ptr(fake_gpiochip_ids),
        .owner    = THIS_MODULE,
    },
};
module_platform_driver(mypdrv);
MODULE_AUTHOR("test");
MODULE_LICENSE("GPL");
