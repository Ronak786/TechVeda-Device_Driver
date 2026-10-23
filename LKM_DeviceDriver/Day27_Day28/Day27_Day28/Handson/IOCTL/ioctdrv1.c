#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>

#define DUMMY_MAJOR 0
#define DUMMY_DEVICE "dummy1"
#define  CMD1 _IO('f',1)
#define  CMD2 _IO('f',2)
 

long dummy_unlocked_ioctl(struct file* file,
			unsigned int cmd, 
			unsigned long arg)
{
	switch(_IOC_NR(cmd))
	{
		case 1:
			printk("command 1: arg = %lu\n", arg);
		break;
		case 2:
			printk("command 2: arg = %lu\n", arg);
		break;
		default:
			printk("command = 0x%x\n, arg = %lu",_IOC_NR(cmd), arg);
		break;
	}
	return 0;
}

struct file_operations dummy_ops = {
	.owner= THIS_MODULE,
	.unlocked_ioctl= dummy_unlocked_ioctl,
};

int init_module(void)
{
	int major=0;
	if((major=register_chrdev(DUMMY_MAJOR, DUMMY_DEVICE, &dummy_ops)) < 0)
		printk("major number %d not available\n", DUMMY_MAJOR);
	printk("%d\n",major);
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(DUMMY_MAJOR, DUMMY_DEVICE);
}

MODULE_LICENSE("GPL");
