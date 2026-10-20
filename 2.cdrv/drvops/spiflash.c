/*
 * ******************************************************************************
 * This program is part of the source code provided with "Linux Device Drivers"
 * Course offered by Techveda <www.techveda.org>
 *
 * (C) 2020 Raghu Bharadwaj/Techveda
 *
 * Git repository:
 *   https://gitlab.com/techveda/ldd
 * ******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/mutex.h>
#include<linux/i2c.h>
#include<linux/slab.h>
#include<linux/sysfs.h>
#include<linux/fs.h>
#include<linux/property.h>
#include<linux/spi/spi.h>
#include<linux/gpio.h>
#include"spi.h"

struct W25_priv *obj=NULL;

static ssize_t read(struct kobject *kobj,struct kobj_attribute *attr,char *buf)
{

	unsigned retval,i;
	u8 tx_buf[5];

	pr_info("read got invoked\n");

	tx_buf[0] = W25_READ;
	tx_buf[1] = obj->offset>>16;
	tx_buf[2] = obj->offset>>8;
	tx_buf[3] = obj->offset;
	
	retval = spi_write_then_read(obj->spi,tx_buf,4,buf,256);
	//retval = spi_read(obj->spi,tx_buf,5);
	if(retval)
		pr_info("read: spi_write_then_read got failed\n");
	return 256;
}

static ssize_t write(struct kobject *kobj,struct kobj_attribute *attr,const char *buf,size_t count) {
	u8 cmd = W25_WREN,tx_buf[260]={0};
	unsigned retval,i,j;

	tx_buf[0] = W25_WRITE;
	tx_buf[1] = obj->offset>>16;
	tx_buf[2] = obj->offset>>8;
	tx_buf[3] = obj->offset;

	j=4;
	for(i=0;i<count;i++)
	{
		tx_buf[j] = buf[i];
		j++;
	}

	retval = spi_write(obj->spi,&cmd,1);
	if(retval)
		pr_info("write (write enable): spi_write got failed\n");
	retval = spi_write(obj->spi,tx_buf,count+4);
	if(retval)
		pr_info("write (write enable): spi_write got failed\n");
	return count;

}

static ssize_t get_offset(struct kobject *kobj,struct kobj_attribute *attr,char *buf)
{
	return sprintf(buf,"%ld",obj->offset);
}

static ssize_t set_offset(struct kobject *kobj,struct kobj_attribute *attr,const char *buf,size_t count) {

	unsigned block,sect,page;
	
	sscanf(buf,"%u:%u:%u",&block,&sect,&page);
	
	if((block>64) || (sect>16) || (page>16))
		return -EAGAIN;

	obj->offset = ((block*64*1024)+(sect*4*1024)+(page*256));
	return count;
}

static ssize_t erase(struct kobject *kobj,struct kobj_attribute *attr,const char *buf,size_t count) {

	unsigned blockno,sectno,offset,status;

	u8 cmd=W25_WREN,arr[4];

	sscanf(buf,"%u:%u",&blockno,&sectno);

	if((blockno>64)&&(sectno>16))
		return -EAGAIN;
		
	offset = ((blockno*64*1024) + (sectno*4*1024));
	/* write enable latch */
		status = spi_write(obj->spi,&cmd,1);
		if(status)
			pr_info("spi_write got failed\n");
		arr[0] = W25_SEC_ERASE;
		arr[1] = offset>>16;
		arr[2] = offset>>8;
		arr[3] = offset;

		status = spi_write(obj->spi,arr,4);
		if(status)
			pr_info("spi_write got failed\n");

		return count;
}

static struct kobj_attribute data_attribute = __ATTR(w25q32,S_IRUGO|S_IWUSR,read,write);
static struct kobj_attribute offset_attribute = __ATTR(offset,S_IRUGO|S_IWUSR,get_offset,set_offset);
static struct kobj_attribute erase_attribute = __ATTR(erase,S_IRUGO|S_IWUSR,NULL,erase);

static struct attribute *attrs[] = {
                &data_attribute.attr,
                &offset_attribute.attr,
		&erase_attribute.attr,
                NULL,  /* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
        .attrs = attrs,
};

static int spi_probe(struct spi_device *spi)
{
	int retval;

	obj = (struct W25_priv *)kmalloc(sizeof(struct W25_priv),GFP_KERNEL);
	if(obj == NULL)
	{
		pr_info("Requested memory not allocated\n");
		return -ENOMEM;
	}
	
	obj->spi = spi;
	
	if(device_property_read_u32(&spi->dev,"size",&obj->size)==0){
	}
	else{
		dev_err(&spi->dev,"Error : missing missing \"size\" property\n");
		return -ENODEV;
	}

	if(device_property_read_u32(&spi->dev,"pagesize",&obj->page_size)==0){
	}
	else{
		dev_err(&spi->dev,"Error : missing missing \"pagesize\" property\n");
		return -ENODEV;
	}

	if(device_property_read_u32(&spi->dev,"address-width",&obj->addr_width)==0){
	}
	else{
		dev_err(&spi->dev,"Error : missing missing \"address-width\" property\n");
		return -ENODEV;
	}

	obj->kobj = kobject_create_and_add("spiflash",NULL);

	retval = sysfs_create_group(obj->kobj,&attr_group);
	if(retval)
		kobject_put(obj->kobj);
	return 0;

}

static int spi_remove(struct spi_device *spi)
{
	kobject_put(obj->kobj);
	return 0;
}

static const struct spi_device_id w25q32_spi_id_table[] = {
        { "w25q32" },
        {},
};
MODULE_DEVICE_TABLE(spi, w25q32_spi_id_table);

static struct spi_driver spi_driver = {
        .driver = {
                .name = "w25q32_spi",
		.owner = THIS_MODULE,
        },
        .probe = spi_probe,
	.remove = spi_remove,
        .id_table = w25q32_spi_id_table,
};
module_spi_driver(spi_driver);

MODULE_AUTHOR("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION("sample spi slave drv");
MODULE_LICENSE("Dual MIT/GPL");

