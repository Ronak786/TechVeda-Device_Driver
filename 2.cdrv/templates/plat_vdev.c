/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * ******************************************************************************
 * This program is part of the source code provided with "Linux Device Drivers"
 * Course offered by Techveda <www.techveda.org>
 *
 * Copyright (C) 2020 Techveda
 * Author: Raghu Bharadwaj
 * Git repository:
 *   https://gitlab.com/techveda/ldd-0920
 * ******************************************************************************
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/platform_device.h>

/* Platform Device */
static struct platform_device plat_vDev = { 
        .name = "vDev0",
        .id = 0,
        .dev = { 
                .platform_data = NULL, /* (old method):
                     was used to specify list of device's resources here: memory ranges/IRQs/
                     IOports/DMA channels/clocks/etc + optionally 'data'
                     (typically a private structure).
                    
		     The new way is of course to use the Device Tree (DTS file)
                     to specify both the 'data' and platform-specific resources
                     */
            //    .release = plat0_release,
        },
};

static struct platform_device *vDev_platform_devices[] __initdata = { 
        &plat_vDev,
};

static int __init vDev_init(void)
{
        int ret;
	/* platform_add_devices() is a wrapper over platform_device_register() */
        ret = platform_add_devices(vDev_platform_devices,
                                  ARRAY_SIZE(vDev_platform_devices));
        if (ret != 0) {
		pr_alert("%s: platform_add_devices failed!\n",__func__);
                return -EINVAL;
        }
        return 0;
}

static void __exit vDev_exit(void)
{
	platform_device_unregister(&plat_vDev);
}

module_init(vDev_init);
module_exit(vDev_exit);


MODULE_AUTHOR("Raghu Bharadwaj<raghu@techveda.org>");
MODULE_DESCRIPTION("Enumerating platform devices(old method)");
MODULE_LICENSE("Dual MIT/GPL");

