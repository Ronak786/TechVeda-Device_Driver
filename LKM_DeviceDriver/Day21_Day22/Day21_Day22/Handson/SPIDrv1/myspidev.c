#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/spi/spi.h>

static int myspidev_probe(struct spi_device *spi)
{
        pr_err("Probe being called\n");
	return 0;
}

static const struct of_device_id myspidev_of_match[] = {
	{ .compatible = "myspidev" },
	{}
};
MODULE_DEVICE_TABLE(of, myspidev_of_match);

static const struct spi_device_id myspidev_spi_ids[] = {
	{ .name = "myspidev" },
	{}
};
MODULE_DEVICE_TABLE(spi, myspidev_spi_ids);

static struct spi_driver myspidev_driver = {
	.driver = {
		.name	= "myspidev",
		.of_match_table = of_match_ptr(myspidev_of_match),
	},
	.probe	= myspidev_probe,
	.id_table = myspidev_spi_ids,
};

module_spi_driver(myspidev_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("test");
MODULE_DESCRIPTION("A simple device driver - SPI Slave Protocol Driver");
MODULE_VERSION("1.0");
