#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO

//LED is connected to this GPIO
#define GPIO_21 (21)

#define DEVICE_NAME     "testchar"
static int num_major;

static int test_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "test_open called\n");
        return 0;
}

static int test_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "test_release called\n");
        return 0;
}

static ssize_t test_read(struct file *filp,  
                           char *buf,  
                           size_t len,  
                           loff_t * off)
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

static ssize_t test_write(struct file *filp,
                            const char *buf,
                            size_t len,
                            loff_t * off)
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



static struct file_operations fops= {
        .open = test_open,
        .release = test_release,
        .read = test_read,
        .write = test_write,
};


static int __init test_init(void)
{

	printk(KERN_INFO "Entering Test Character Driver \n");

	num_major=register_chrdev(0, DEVICE_NAME, &fops);
	printk(KERN_INFO "Major Number = %d \n",num_major);
	printk(KERN_INFO "Name =  %s \n",DEVICE_NAME);
	printk(KERN_INFO "Generate the device file with\
			mknod /dev/%s c %d 0 \n",DEVICE_NAME,num_major);

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
	unregister_chrdev(num_major, DEVICE_NAME);

	return -1;

}

static void __exit test_cleanup(void)
{
	gpio_unexport(GPIO_21);
	gpio_free(GPIO_21);
	unregister_chrdev(num_major, DEVICE_NAME);
	printk(KERN_INFO "Exiting Test Character Driver \n");
}

module_init(test_init);
module_exit(test_cleanup);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("test");                       
MODULE_DESCRIPTION("Character Device Driver");            
