#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include<linux/proc_fs.h>

int32_t value = 0;
char test_array[20]="try_proc_array\n";
static int len = 1;
 
 
static struct proc_dir_entry *parent;
static int      __init test_driver_init(void);
static void     __exit test_driver_exit(void);
 
/***************** Procfs Functions *******************/
static int      open_proc(struct inode *inode, struct file *file);
static int      release_proc(struct inode *inode, struct file *file);
static ssize_t  read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);
/*
** procfs operation sturcture
*/
static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};

/*
** This function will be called when we open the procfs file
*/
static int open_proc(struct inode *inode, struct file *file)
{
    pr_info("proc file opend.....\t");
    return 0;
}
/*
** This function will be called when we close the procfs file
*/
static int release_proc(struct inode *inode, struct file *file)
{
    pr_info("proc file released.....\n");
    return 0;
}
/*
** This function will be called when we read the procfs file
*/
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{
    pr_info("proc file read.....\n");
    if(len)
    {
        len=0;
    }
    else
    {
        len=1;
        return 0;
    }
    
    if( copy_to_user(buffer,test_array,20) )
    {
        pr_err("Data Send : Err!\n");
    }
 
    return length;;
}
/*
** This function will be called when we write the procfs file
*/
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    pr_info("proc file wrote.....\n");
    
    memset(test_array,0x0,sizeof(test_array));
    if( copy_from_user(test_array,buff,len) )
    {
        pr_err("Data Write : Err!\n");
    }
    
    return len;
}
 
/*
** Module Init function
*/
static int __init test_driver_init(void)
{
        
        /*Create proc directory. It will create a directory under "/proc" */
        parent = proc_mkdir("procfs1",NULL);
        
        if( parent == NULL )
        {
            pr_info("Error creating proc entry");
            return -1;
        }
        
        /*Creating Proc entry under "/proc/procfs1/" */
        proc_create("test_proc", 0666, parent, &proc_fops);
 
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
}
 
/*
** Module exit function
*/
static void __exit test_driver_exit(void)
{
        /* remove complete /proc/test */
        proc_remove(parent);
        
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(test_driver_init);
module_exit(test_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Test");
MODULE_DESCRIPTION("Simple Linux device driver (procfs)");
MODULE_VERSION("1.0");
