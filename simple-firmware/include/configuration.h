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

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#ifdef	__cplusplus
extern "C" {
#endif

#define WATCHDOG_REFRESH_MS     1500 


    
#define CAN_BUS_TASK_PRIORITY           150
#define USB_RCV_TASK_PRIORITY           101
#define UART_RCV_TASK_PRIORITY          100
#define UART_DEBUG_TASK_PRIORITY        PRIORITY_LOW
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* CONFIGURATION_H */

