#include <linux/init.h>
#include <linux/module.h>
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
/****************************************************************************
 * Name: test_spi_init
 *
 * Details : This function Register and Initilize the SPI.
 ****************************************************************************/
static int __init test_spi_init(void)
{
  int     ret;
  struct  spi_master *master;
  
  master = spi_busnum_to_master( test_spi_device_info.bus_num );
  if( master == NULL )
  {
    pr_err("SPI Master not found.\n");
    return -ENODEV;
  }
   
  // create a new slave device, given the master and device info
  test_spi_device = spi_new_device( master, &test_spi_device_info );
  if( test_spi_device == NULL ) 
  {
    pr_err("FAILED to create slave.\n");
    return -ENODEV;
  }
  
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
 
/****************************************************************************
 * Name: test_spi_exit
 *
 * Details : This function Unregister and DeInitilize the SPI.
 ****************************************************************************/
static void __exit test_spi_exit(void)
{ 
  if( test_spi_device )
  {
    // Clear the display
    TEST_SSD1306_ClearDisplay();                 // Clear Display
    TEST_SSD1306_DisplayDeInit();                // Deinit the SSD1306
    spi_unregister_device( test_spi_device );    // Unregister the SPI slave
    pr_info("SPI driver Unregistered\n");
  }
}
 
module_init(test_spi_init);
module_exit(test_spi_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("test");
MODULE_DESCRIPTION("A simple device driver - SPI Slave Protocol Driver");
MODULE_VERSION("1.0");
