#define OMAP_GPIO_OE                    0x0134
#define OMAP_GPIO_SETDATAOUT            0x0194
#define OMAP_GPIO_CLEARDATAOUT          0x0190
#define OMAP_GPIO_DATAIN                0x0138

struct gpiopin_bank {
        u32 gpio_base;
	unsigned int pinno;
};

