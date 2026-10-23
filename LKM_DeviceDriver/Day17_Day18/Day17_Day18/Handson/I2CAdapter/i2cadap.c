#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#define ADAPTER_NAME     "TEST_I2C_ADAPTER"
/*
** This function used to get the functionalities that are supported 
** by this bus driver.
*/
static u32 test_func(struct i2c_adapter *adapter)
{
    return (I2C_FUNC_I2C             |
            I2C_FUNC_SMBUS_QUICK     |
            I2C_FUNC_SMBUS_BYTE      |
            I2C_FUNC_SMBUS_BYTE_DATA |
            I2C_FUNC_SMBUS_WORD_DATA |
            I2C_FUNC_SMBUS_BLOCK_DATA);
}
/*
** This function will be called whenever you call I2C read, wirte APIs like
** i2c_master_send(), i2c_master_recv() etc.
*/
static s32 test_i2c_xfer( struct i2c_adapter *adap, struct i2c_msg *msgs,int num )
{
    int i;
    
    for(i = 0; i < num; i++)
    {
        int j;
        struct i2c_msg *msg_temp = &msgs[i];
        
        pr_info("[Count: %d] [%s]: [Addr = 0x%x] [Len = %d] [Data] = ", i, __func__, msg_temp->addr, msg_temp->len);
        
        for( j = 0; j < msg_temp->len; j++ )
        {
            pr_cont("[0x%02x] ", msg_temp->buf[j]);
        }
    }
    return 0;
}
/*
** This function will be called whenever you call SMBUS read, wirte APIs
*/
static s32 test_smbus_xfer(  struct i2c_adapter *adap, 
                            u16 addr,
                            unsigned short flags, 
                            char read_write,
                            u8 command, 
                            int size, 
                            union i2c_smbus_data *data
                         )
{
    pr_info("In %s\n", __func__);
    return 0;
}
/*
** I2C algorithm Structure
*/
static struct i2c_algorithm test_i2c_algorithm = {
    .smbus_xfer     = test_smbus_xfer,
    .master_xfer    = test_i2c_xfer,
    .functionality  = test_func,
};
/*
** I2C adapter Structure
*/
static struct i2c_adapter test_i2c_adapter = {
    .owner  = THIS_MODULE,
    .class  = I2C_CLASS_HWMON,
    .algo   = &test_i2c_algorithm,
    .name   = ADAPTER_NAME,
};
/*
** Module Init function
*/
static int __init test_driver_init(void)
{
    int ret = -1;
    
    ret = i2c_add_adapter(&test_i2c_adapter);
    
    pr_info("Bus Driver Added!!!\n");
    return ret;
}
/*
** Module Exit function
*/
static void __exit test_driver_exit(void)
{
    i2c_del_adapter(&test_i2c_adapter);
    pr_info("Bus Driver Removed!!!\n");
}
module_init(test_driver_init);
module_exit(test_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("test");
MODULE_DESCRIPTION("Simple I2C Bus driver explanation");
MODULE_VERSION("1.0");
