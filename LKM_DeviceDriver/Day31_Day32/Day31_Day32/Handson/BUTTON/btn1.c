#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO
#include <linux/interrupt.h>

/* Since debounce is not supported in Raspberry pi, I have addded this to disable 
** the false detection (multiple IRQ trigger for one interrupt).
** Many other hardware supports GPIO debounce, I don't want care about this even 
** if this has any overhead. Our intention is to explain the GPIO interrupt.
** If you want to disable this extra coding, you can comment the below macro.
** This has been taken from : https://raspberrypi.stackexchange.com/questions/8544/gpio-interrupt-debounce
**
** If you want to use Hardaware Debounce, then comment this EN_DEBOUNCE.
**
*/
#define EN_DEBOUNCE
#ifdef EN_DEBOUNCE
#include <linux/jiffies.h>
extern unsigned long volatile jiffies;
unsigned long old_jiffie = 0;
#endif
//LED is connected to this GPIO
#define GPIO_26_IN  (26)
//This used for storing the IRQ number for the GPIO
unsigned int GPIO_irqNumber;
static struct input_dev *my_dev;
//Interrupt handler for GPIO 26. This will be called whenever there is a raising edge detected. 
static irqreturn_t gpio_irq_handler(int irq,void *dev_id) 
{
  static unsigned long flags = 0;
  int gpioval;
  
#ifdef EN_DEBOUNCE
   unsigned long diff = jiffies - old_jiffie;
   if (diff < 50)
   {
     return IRQ_HANDLED;
   }
  
  old_jiffie = jiffies;
#endif  
  local_irq_save(flags);
 
  gpioval = gpio_get_value(GPIO_26_IN);
  
  if(1 == gpioval){
	  input_report_key(my_dev, BTN_0, 1);
  }else{
	  input_report_key(my_dev, BTN_0, 0);
  }
  input_sync(my_dev);
  local_irq_restore(flags);
  return IRQ_HANDLED;
}
 

static int __init gpio2_driver_init(void);
static void __exit gpio2_driver_exit(void);
 

/*
** Module Init function
*/ 
static int __init gpio2_driver_init(void)
{
	int error;
  //Input GPIO configuratioin
  //Checking the GPIO is valid or not
  if(gpio_is_valid(GPIO_26_IN) == false){
    pr_err("GPIO %d is not valid\n", GPIO_26_IN);
    goto r_gpio_in;
  }
  
  //Requesting the GPIO
  if(gpio_request(GPIO_26_IN,"GPIO_26_IN") < 0){
    pr_err("ERROR: GPIO %d request\n", GPIO_26_IN);
    goto r_gpio_in;
  }
  
  //configure the GPIO as input
  gpio_direction_input(GPIO_26_IN);
  
  /*
  ** I have commented the below few lines, as gpio_set_debounce is not supported 
  ** in the Raspberry pi. So we are using EN_DEBOUNCE to handle this in this driver.
  */ 
#ifndef EN_DEBOUNCE
  //Debounce the button with a delay of 200ms
  if(gpio_set_debounce(GPIO_26_IN, 200) < 0){
    pr_err("ERROR: gpio_set_debounce - %d\n", GPIO_26_IN);
    //goto r_gpio_in;
  }
#endif
  
  //Get the IRQ number for our GPIO
  GPIO_irqNumber = gpio_to_irq(GPIO_26_IN);
  pr_info("GPIO_irqNumber = %d\n", GPIO_irqNumber);
  
  if (request_irq(GPIO_irqNumber,             //IRQ number
                  (void *)gpio_irq_handler,   //IRQ handler
                  IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,        //Handler will be called in raising edge
                  "gpio2_device",               //used to identify the device name using this IRQ
                  NULL)) {                    //device id for shared IRQ
    pr_err("my_device: cannot register IRQ ");
    goto r_gpio_in;
  }
  

	my_dev = input_allocate_device();
	my_dev->name="btn1";
	set_bit(EV_KEY,my_dev->evbit);
	set_bit(BTN_0,my_dev->keybit);
	error=input_register_device(my_dev);
        if (error) {
                printk(KERN_ERR "button.c: Failed to register device\n");
                return -1;
        }
 
  pr_info("Device Driver Insert...Done!!!\n");
  return 0;
r_gpio_in:
  gpio_free(GPIO_26_IN);

  return -1;
}
/*
** Module exit function
*/
static void __exit gpio2_driver_exit(void)
{
  free_irq(GPIO_irqNumber,NULL);
  gpio_free(GPIO_26_IN);
  input_unregister_device(my_dev);
  pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(gpio2_driver_init);
module_exit(gpio2_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("test");
MODULE_DESCRIPTION("A simple device driver - Button Input ");
MODULE_VERSION("1.0");
