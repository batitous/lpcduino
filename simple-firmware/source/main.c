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


//------------------------ private variables

KTask           usbTask;


void usbTaskCode(uint32_t size)
{

    
    
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
    
    initUsbClock();
    
    //-------------------------- UART0 and UART1 initialization
//    assignMovableFunctionToGpio(SWM_UART0_RXD_I , UART_RX_IN);
//    assignMovableFunctionToGpio(SWM_UART0_TXD_O , UART_TX_OUT);
    
    
        
    //-------------------------- Kernel and Tasks initialization
    initSimpleKernel();
    
    
    initUsbUart();
    initTask(&usbTask, usbTaskCode, USB_RCV_TASK_PRIORITY);
    assignTaskOnUsbUart(&usbTask);
    
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

