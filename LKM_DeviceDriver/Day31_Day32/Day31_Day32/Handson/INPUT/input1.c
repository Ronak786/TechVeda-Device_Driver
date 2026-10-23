#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>


static struct input_dev *my_dev;


static int my_proc_open(struct inode *pinode,struct file *pfile)
{
	pr_info("proc file is opened..\n");
	return 0;
}

static int my_proc_close(struct inode *pinode,struct file *pfile)
{
	pr_info("proc_file is closed..\n");
	return 0;
}

static ssize_t my_proc_write(struct file *pfile, const char __user *buffer,size_t count,loff_t *offset)
{
	input_report_key(my_dev,KEY_A,1);
	input_report_key(my_dev,KEY_A,0);
        input_report_key(my_dev,KEY_B,1);
        input_report_key(my_dev,KEY_B,0);
        input_report_key(my_dev,KEY_C,1);
        input_report_key(my_dev,KEY_C,0);
	input_sync(my_dev);

	return count;
}



static struct proc_ops my_ops = {
	.proc_open = my_proc_open,
	.proc_release = my_proc_close,
       .proc_write = my_proc_write,
};

static struct proc_dir_entry *entry;


static int my_input_driver_init(void)
{

	int error;
	printk(KERN_ERR "Test1\n");
	my_dev = input_allocate_device();
	printk(KERN_ERR "Test2\n");
	my_dev->name="input1";
	set_bit(EV_KEY,my_dev->evbit);
	set_bit(KEY_A,my_dev->keybit);
	set_bit(KEY_B,my_dev->keybit);
	set_bit(KEY_C,my_dev->keybit);
	printk(KERN_ERR "Test3\n");
	error=input_register_device(my_dev);
        if (error) {
                printk(KERN_ERR "button.c: Failed to register device\n");
                return -1;
        }
	printk(KERN_ERR "Test4\n");
	entry = proc_create("input_proc",0777,NULL,&my_ops);
	printk(KERN_ERR "Test5\n");

	return 0;
}


static void my_input_driver_exit(void)
{
	proc_remove(entry);
	input_unregister_device(my_dev);
}


module_init(my_input_driver_init);
module_exit(my_input_driver_exit);

MODULE_LICENSE("GPL");


