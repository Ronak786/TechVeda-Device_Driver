#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO

//LED is connected to this GPIO
#define GPIO_21 (21)
#define LED_ON _IO('f',1)
#define LED_OFF _IO('f',2)

static unsigned int major; /* major number for device */
static struct class *dummy_class;
static struct cdev dummy_cdev;


int dummy_open(struct inode * inode, struct file * filp)
{
    pr_info("Someone tried to open me\n");
    return 0;
}

int dummy_release(struct inode * inode, struct file * filp)
{
    pr_info("Someone closed me\n");
    return 0;
}

ssize_t dummy_read (struct file *filp, char __user * buf, size_t len,
                                loff_t * offset)
{
    
	uint8_t gpio_state = 0;
	
	printk(KERN_INFO "test_read called\n");

	//reading GPIO value
	gpio_state = gpio_get_value(GPIO_21);

	//write to user
	len = 1;
	if( copy_to_user(buf, &gpio_state, len) > 0) {
		pr_err("ERROR: Not all the bytes have been copied to user\n");
	}

	pr_info("Read function : GPIO_21 = %d \n", gpio_state);

	return 0;
}


ssize_t dummy_write(struct file * filp, const char __user * buf, size_t len,
                                loff_t * offset)
{
	uint8_t rec_buf[10] = {0};

	printk(KERN_INFO "test_write called\n"); 


	if( copy_from_user( rec_buf, buf, len ) > 0) {
		pr_err("ERROR: Not all the bytes have been copied from user\n");
	}

	pr_info("Write Function : GPIO_21 Set = %c\n", rec_buf[0]);

	if (rec_buf[0]=='1') {
		//set the GPIO value to HIGH
		gpio_set_value(GPIO_21, 1);
	} else if (rec_buf[0]=='0') {
		//set the GPIO value to LOW
		gpio_set_value(GPIO_21, 0);
	} else {
		pr_err("Unknown command : Please provide either 1 or 0 \n");
	}

	return len;
}

long dummy_unlocked_ioctl(struct file* file,
			unsigned int cmd, 
			unsigned long arg)
{
	switch(cmd)
	{
		case LED_ON:
			printk("command 1: arg = %lu\n", arg);
			//set the GPIO value to HIGH
			gpio_set_value(GPIO_21, 1);			
		break;
		case LED_OFF:
			//set the GPIO value to LOW
			gpio_set_value(GPIO_21, 0);		
			printk("command 2: arg = %lu\n", arg);
		break;
		default:
			printk("command = 0x%x\n, arg = %lu",_IOC_NR(cmd), arg);
		break;
	}
	return 0;
}

struct file_operations dummy_fops = {
    open:       dummy_open,
    release:    dummy_release,
    read:       dummy_read,
    write:      dummy_write,
	unlocked_ioctl: dummy_unlocked_ioctl,			
};

static int __init dummy_char_init_module(void)
{
    struct device *dummy_device;
    int error;
    dev_t devt = 0;

    /* Get a range of minor numbers (starting with 0) to work with */
    error = alloc_chrdev_region(&devt, 0, 1, "dummy_char");
    if (error < 0) {
        pr_err("Can't get major number\n");
        return error;
    }
    major = MAJOR(devt);
    pr_info("dummy_char major number = %d\n",major);

    /* Create device class, visible in /sys/class */
    dummy_class = class_create(THIS_MODULE, "dummy_char");
    if (IS_ERR(dummy_class)) {
        pr_err("Error creating dummy char class.\n");
        unregister_chrdev_region(MKDEV(major, 0), 1);
        return PTR_ERR(dummy_class);
    }

    /* Initialize the char device and tie a file_operations to it */
    cdev_init(&dummy_cdev, &dummy_fops);
    dummy_cdev.owner = THIS_MODULE;
    /* Now make the device live for the users to access */
    cdev_add(&dummy_cdev, devt, 1);

    dummy_device = device_create(dummy_class,
                                NULL,   /* no parent device */
                                devt,    /* associated dev_t */
                                NULL,   /* no additional data */
                                "dummy_char");  /* device name */

    if (IS_ERR(dummy_device)) {
        pr_err("Error creating dummy char device.\n");
        class_destroy(dummy_class);
        unregister_chrdev_region(devt, 1);
        return -1;
    }

    pr_info("dummy char module loaded\n");
	
	//Checking the GPIO is valid or not
	if(gpio_is_valid(GPIO_21) == false){
		pr_err("GPIO %d is not valid\n", GPIO_21);
		goto r_unreg;
	}

	//Requesting the GPIO
	if(gpio_request(GPIO_21,"GPIO_21") < 0){
		pr_err("ERROR: GPIO %d request\n", GPIO_21);
		goto r_gpio;
	}

	//configure the GPIO as output
	gpio_direction_output(GPIO_21, 0);

	/* Using this call the GPIO 21 will be visible in /sys/class/gpio/
	** Now you can change the gpio values by using below commands also.
	** echo 1 > /sys/class/gpio/gpio21/value  (turn ON the LED)
	** echo 0 > /sys/class/gpio/gpio21/value  (turn OFF the LED)
	** cat /sys/class/gpio/gpio21/value  (read the value LED)
	** 
	** the second argument prevents the direction from being changed.
	*/
	gpio_export(GPIO_21, false);

	pr_info("Device Driver Insert...Done!!!\n");
	return 0;

r_gpio:
	gpio_free(GPIO_21);
r_unreg:
    unregister_chrdev_region(MKDEV(major, 0), 1);
    device_destroy(dummy_class, MKDEV(major, 0));
    cdev_del(&dummy_cdev);
    class_destroy(dummy_class);

	return -1;	
}

static void __exit dummy_char_cleanup_module(void)
{
    unregister_chrdev_region(MKDEV(major, 0), 1);
    device_destroy(dummy_class, MKDEV(major, 0));
    cdev_del(&dummy_cdev);
    class_destroy(dummy_class);

    pr_info("dummy char module Unloaded\n");
}

module_init(dummy_char_init_module);
module_exit(dummy_char_cleanup_module);

MODULE_AUTHOR("test");
MODULE_DESCRIPTION("Dummy character driver");
MODULE_LICENSE("GPL");
