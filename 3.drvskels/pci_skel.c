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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>

#define DRV_NAME "Pci skel"
#define VERSION  "0.9.28"
#define DRIVER_NAME "A RealTek RTL-8139 Fast Ethernet with "  DRV_NAME " of Version : " VERSION


MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR ("Raghu Bharadwaj <raghu@techveda.org>");
MODULE_DESCRIPTION ("PCI skeleton");

/**
 * Probe: Drivers init, called when device is found 
 */
static int drv_init(struct pci_dev *pdev,
                                       const struct pci_device_id *ent)
{
	pr_info("%s: Compatible realtek controller detected..\n",__func__);
	pr_info("%s: setting up device\n",__func__);

	return 0;
}

/**
 * drv_exit - invoked when  device is detached
 *
 */

static void drv_exit(struct pci_dev *pdev)
{
	
}

/* list of objects describing device and vendor id's that this driver supports */
static struct pci_device_id dev_table[] = {
        {0x10ec, 0x8139, PCI_ANY_ID, PCI_ANY_ID},
	{} // end of the list
};

/* pci driver structure to register with pci core */
static struct pci_driver rtl_driver  = {
        .name                   = DRV_NAME,
        .id_table               = dev_table,
        .probe                  = drv_init,
        .remove                 = drv_exit,
};

/* helper macro that expands to init/exit functions that carry out 
   register/unregister ops with pci core */

module_pci_driver(rtl_driver);
