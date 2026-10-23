#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include "ssd1306.h"
static struct spi_device *test_spi_device;
//Register information about your slave device
struct spi_board_info test_spi_device_info = 
{
  .modalias     = "test-spi-ssd1306-driver",
  .max_speed_hz = 4000000,              // speed your device (slave) can handle
  .bus_num      = SPI_BUS_NUM,          // SPI 1
  .chip_select  = 0,                    // Use 0 Chip select (GPIO 18)
  .mode         = SPI_MODE_0            // SPI mode 0
};
/****************************************************************************
 * Name: test_spi_write
 *
 * Details : This function writes the 1-byte data to the slave device using SPI.
 ****************************************************************************/
int test_spi_write( uint8_t data )
{
  int     ret = -1;
  uint8_t rx  = 0x00;
  
  if( test_spi_device )
  {    
    struct spi_transfer  tr = 
    {
      .tx_buf  = &data,
      .rx_buf = &rx,
      .len    = 1,
    };
    spi_sync_transfer( test_spi_device, &tr, 1 );
  }
  
  //pr_info("Received = 0x%02X \n", rx);
  
  return( ret );
}

static int myssd1306_probe(struct spi_device *spi)
{
	int ret=-1;
    pr_err("Probe being called\n");
	test_spi_device=spi;
  // 8-bits in a word
  test_spi_device->bits_per_word = 8;
  // setup the SPI slave device
  ret = spi_setup( test_spi_device );
  if( ret )
  {
    pr_err("FAILED to setup slave.\n");
    spi_unregister_device( test_spi_device );
    return -ENODEV;
  }
  
  //Initialize the OLED SSD1306
  TEST_SSD1306_DisplayInit();
  
  /* Print the String */
  TEST_SSD1306_SetBrightness( 255 );           // Full brightness
  TEST_SSD1306_InvertDisplay( false );         // Invert the dispaly : OFF
  
  // Enable the Horizontal scroll for first 3 lines
  TEST_SSD1306_StartScrollHorizontal( true, 0, 2);
  
  
  TEST_SSD1306_SetCursor(0,0);                 // Set cursor at 0th line 0th col
  //Write String to OLED
  TEST_SSD1306_String("Hello World\n");
  
  TEST_SSD1306_SetCursor(4,35);                // Set cursor at 4th line 35th col
  //Write String to OLED
  TEST_SSD1306_String("SPI Linux\n");
  TEST_SSD1306_SetCursor(5,23);                // Set cursor at 5th line 23rd col
  TEST_SSD1306_String("Device Driver\n");
  
  msleep(9000);                               // 9secs delay
  
  TEST_SSD1306_ClearDisplay();                 // Clear Display
  TEST_SSD1306_DeactivateScroll();             // Deactivate the scroll
  
  pr_info("SPI driver Registered\n");
  return 0;	
}

void myssd1306_remove(struct spi_device *spi)
{
	// Clear the display
	TEST_SSD1306_ClearDisplay();                 // Clear Display
	TEST_SSD1306_DisplayDeInit();                // Deinit the SSD1306
	spi_unregister_device( spi );    // Unregister the SPI slave
	pr_info("SPI driver Unregistered\n");	
}

static const struct of_device_id myssd1306_of_match[] = {
	{ .compatible = "myssd1306" },
	{}
};
MODULE_DEVICE_TABLE(of, myssd1306_of_match);

static const struct spi_device_id myssd1306_spi_ids[] = {
	{ .name = "myssd1306" },
	{}
};
MODULE_DEVICE_TABLE(spi, myssd1306_spi_ids);

static struct spi_driver myssd1306_driver = {
	.driver = {
		.name	= "myssd1306",
		.of_match_table = of_match_ptr(myssd1306_of_match),
	},
	.probe	= myssd1306_probe,
	.remove = myssd1306_remove,
	.id_table = myssd1306_spi_ids,
};

module_spi_driver(myssd1306_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("test");
MODULE_DESCRIPTION("A simple device driver - SPI Slave Protocol Driver");
MODULE_VERSION("1.0");


