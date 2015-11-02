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

#include "../include/configuration.h"
#include "../include/pinout.h"

#include "../../shared/include/usbUart.h"
#include "../../shared/include/utils.h"
#include "../../shared/include/shared-pinout.h"


//------------------------ private variables

KTask           usbTask;


void usbTaskCode(uint32_t size)
{
    uint8_t buffer[96];
    uint32_t receive;
  
    receive = getBufferFromUsbUart(buffer, size);
    
    if (receive!=0)
    {
        switch(buffer[0])
        {
            case '1':
                printf("Test USB UART is working !\r\n");
                
                break;
            case 'v':
                printf("Firmware compiled at %s - %s\r\n", __DATE__, __TIME__);
                break;
            default:
                NVIC_SystemReset();
           break;
        }
    }
    
}

int main(void)
{
    //-------------------------- Low level initialization
    initGpio();
    initGpioIrq();
    
    // set systick clock divider
    LPC_SYSCTL->SYSTICKCLKDIV = 1;
    
    initSwitchMatrix();
    initRitWithTick();
    
    // Turn on the Eeprom block
    SETBIT(LPC_SYSCON->SYSAHBCLKCTRL[0], 9);
    
    initUsbUart();
    
    //-------------------------- UART0 and UART1 initialization
    assignMovableFunctionToGpio(SWM_UART0_RXD_I , RX);
    assignMovableFunctionToGpio(SWM_UART0_TXD_O , TX);
    
    initUart0(115200);
    
        
    //-------------------------- Kernel and Tasks initialization
    initSimpleKernel();
    
    
    initTask(&usbTask, usbTaskCode, USB_DEBUG_TASK_PRIORITY);
    assignTaskOnUsbUart(&usbTask);
    setPrintfInterface(sendByteToUart0);
    
    //-------------------------- Applications initialization
    
    printf("Simple firmware started...\r\n");
        
//    initWatchdog(WATCHDOG_REFRESH_MS);
   
    
    //-------------------------- Main loop
    while(1)
    {
//        feedTheWatchdog();
        if (scheduleTask()==false)
        {
            idleTask();
        }
    }
    
    
    return 0;
}

