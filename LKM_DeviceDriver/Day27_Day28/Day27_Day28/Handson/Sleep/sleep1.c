/* example1.c - The simplest kernel module. */
#include <linux/module.h> /* Needed by all modules */ 
#include <linux/kernel.h> /* Needed for KERN_ALERT */ 
#include <linux/init.h>   /* Needed for macros */
#include <linux/delay.h>


int __init example1_init(void) {
  printk(KERN_ALERT "Testing 1\n"); /* A non 0 return means init_module failed; module can't be loaded. */
  msleep(5000);  
  return 0; 
} 

void __exit example1_stop(void) { 
  printk(KERN_ALERT "Exiting Testing 1\n"); 
} 

module_init (example1_init);
module_exit (example1_stop);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Training1@gmail.com");
MODULE_DESCRIPTION("Test1 program");
MODULE_INFO(example1_initfunc,"Entry function");
MODULE_INFO(example1_stopfunc,"Exit fuction");
