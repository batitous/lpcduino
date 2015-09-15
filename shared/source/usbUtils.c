/*
 The MIT License (MIT)
 
 Copyright (c) 2015 Baptiste Burles, Evotion SAS
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 */
#include <libs-klipos.h>

#include "../../../klipos/klipos/hw/include/mcu/lpc15xx/usbd/usbd.h"
#include "../include/usbConfiguration.h"
#include "../../../klipos/klipos/hw/include/mcu/lpc15xx/usbd/usbd_rom_api.h"


// Find the address of interface descriptor for given class type.
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
    USB_COMMON_DESCRIPTOR *pD;
    USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
    uint32_t next_desc_adr;

    pD = (USB_COMMON_DESCRIPTOR *) pDesc;
    next_desc_adr = (uint32_t) pDesc;

    while (pD->bLength) 
    {
        /* is it interface descriptor */
        if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE)
        {

            pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
            /* did we find the right interface descriptor */
            if (pIntfDesc->bInterfaceClass == intfClass)
            {
                    break;
            }
        }
        pIntfDesc = 0;
        next_desc_adr = (uint32_t) pD + pD->bLength;
        pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
    }

    return pIntfDesc;
}

#define PDRUNCFGUSEMASK 0x00000000
#define PDRUNCFGMASKTMP 0x01FFFF78

#define SYSCTL_POWERDOWN_USBPHY_PD  (1 << 9)	/*!< USB PHY wake-up power down */
#define SYSCTL_POWERDOWN_USBPLL_PD  (1 << 23)	/*!< USB PLL wake-up power down */

void sysctl_powerup(uint32_t powerupmask)
{
    uint32_t pdrun;

    pdrun = LPC_SYSCTL->PDRUNCFG & PDRUNCFGMASKTMP;
    pdrun &= ~(powerupmask & PDRUNCFGMASKTMP);

    LPC_SYSCTL->PDRUNCFG = (pdrun | PDRUNCFGUSEMASK);
}

void initUsbClock(void)
{
    LPC_SYSCTL->USBPLLCLKSEL = 1; // Main oscillator
    
    /* Setup USB PLL  (FCLKIN = 12MHz) * 4 = 48MHz
       MSEL = 3 (this is pre-decremented), PSEL = 1 (for P = 2)
       FCLKOUT = FCLKIN * (MSEL + 1) = 12MHz * 4 = 48MHz
       FCCO = FCLKOUT * 2 * P = 48MHz * 2 * 2 = 192MHz (within FCCO range) */
    uint32_t msel = 3;
    uint32_t psel = 1;
    LPC_SYSCTL->USBPLLCTRL = (msel & 0x3F) | ((psel & 0x3) << 6);

    /* Powerup USB PLL */
    sysctl_powerup(SYSCTL_POWERDOWN_USBPLL_PD);

    /* Wait for PLL to lock */
    while (!((LPC_SYSCTL->USBPLLSTAT & 1) != 0)) {}

    /* enable USB main clock */
    LPC_SYSCTL->USBCLKSEL = (uint32_t) 2;
    LPC_SYSCTL->USBCLKDIV = 1;

    /* Enable AHB clock to the USB block. */
    SETBIT(LPC_SYSCON->SYSAHBCLKCTRL[1],23);

    /* power UP USB Phy */
    sysctl_powerup(SYSCTL_POWERDOWN_USBPHY_PD);
    
    /* Reset USB block */
    SETBIT(LPC_SYSCON->PRESETCTRL[1],23);
    CLRBIT(LPC_SYSCON->PRESETCTRL[1],23);
    
}

/*
void deinitUsbClock(void)
{
    CLRBIT(LPC_SYSCON->SYSAHBCLKCTRL[1],23);
    
    LPC_SYSCTL->USBCLKSEL = 0;
    LPC_SYSCON->USBCLKDIV = 0;

//    initLowLevelCpu();
    
    LPC_SYSCON->USBPLLCLKSEL = 0;
    
    SETBIT(LPC_SYSCON->PDRUNCFG, 9);
    SETBIT(LPC_SYSCON->PDRUNCFG, 23);
}
*/