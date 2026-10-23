#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <asm/io.h>


int arraykeycode[] = {
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,
};

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
	char buf[100];
	int i;
	int index;
	
	memset(buf,0x0,sizeof(buf));
	copy_from_user(buf,buffer,count);

	pr_info("data: %s\n",buf);

	for(i=0;i<strlen(buf);i++){
		if(buf[i]>='a' && buf[i]<= 'z'){
			index=buf[i]-'a';
			input_report_key(my_dev,arraykeycode[index],1);
			input_report_key(my_dev,arraykeycode[index],0);
			input_sync(my_dev);
		}else if(buf[i]>='A' && buf[i]<= 'Z') {
			index=buf[i]-'A';
			input_report_key(my_dev,KEY_RIGHTSHIFT,1);
			input_report_key(my_dev,arraykeycode[index],1);
			input_report_key(my_dev,KEY_RIGHTSHIFT,0);
			input_report_key(my_dev,arraykeycode[index],0);
			input_sync(my_dev);
		}
	}
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
	int i;
	my_dev = input_allocate_device();

	my_dev->name = "input2";
	set_bit(EV_KEY,my_dev->evbit);
	
	for(i=0;i<26;i++){
	set_bit(arraykeycode[i],my_dev->keybit);

	}
	set_bit(KEY_RIGHTSHIFT,my_dev->keybit);

	input_register_device(my_dev);

	entry = proc_create("input_proc",0777,NULL,&my_ops);


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
