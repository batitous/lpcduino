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

#ifndef USBUART_H
#define	USBUART_H

#ifdef	__cplusplus
extern "C" {
#endif
    
extern void initUsbClock(void);
//extern void deinitUsbClock(void);

extern bool initUsbUart(void);
extern void closeUsbUart(void);

extern bool isUsbUartConnected(void);

// return number of bytes sent
extern uint32_t sendBufferToUsbUart(uint8_t *pBuf, uint32_t len);
extern uint32_t sendByteToUsbUart(uint8_t data);

extern uint32_t getBufferFromUsbUart(uint8_t * buffer, uint32_t len);
extern bool getByteFromUsbUart(uint8_t *data);

extern uint32_t getByteAvailableOnUsbUart(void);
extern void assignTaskOnUsbUart(KTask* t);



#ifdef	__cplusplus
}
#endif

#endif	/* USBUART_H */

