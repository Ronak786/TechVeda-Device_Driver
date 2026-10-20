#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/types.h>

static struct platform_device *pdev;

static int __init dummy_eth_add(void)
{
    int inst_id = 0;
    pdev = platform_device_alloc("dummy-eth", inst_id);
    platform_device_add(pdev);
    return 0;
}

static void __exit dummy_eth_put(void)
{
    platform_device_put(pdev);
}

module_init(dummy_eth_add);
module_exit(dummy_eth_put);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Madieu <john.madieu@gmail.com>");
