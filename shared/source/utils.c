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

#include "../include/usbUart.h"


//----------------------------- public functions

uint32_t getDeltaTime(uint32_t start, uint32_t end)
{
    uint32_t delta;
    uint32_t max = 0xFFFFFFFFU;
    
    if (end >= start)
    {
        delta = end - start;
    }
    else
    {
        delta = ((max) - start) + end;
    }
    
    return delta;
}


void waitUs(uint32_t waitInUs)
{
    uint32_t delta;
    uint32_t current;
    uint32_t max = 0xFFFFFFFFU;
    uint32_t start = getTickFromRit();
        
    while(1)
    {
        current = getTickFromRit();
     
        if (current >= start)
        {
            delta = current - start;
        }
        else
        {
            delta = ((max) - start) + current;
        }
        
        if (delta >= waitInUs)
        {
            break;
        }

    }
}

void waitMs(uint32_t ms)
{
    waitUs(ms*1000);
}

uint32_t getHashFromUid(uint32_t uid[4])
{
    // FNV-1a algorithm
    uint32_t i;
    uint32_t hash = 2166136261U;

    for ( i=0; i < 4; i++)
    {
        hash = hash ^ uid[i];
        hash = hash * 16777619U;
    }
    
    return hash;
}

#define PIX_SORT(a,b) { if ((a)>(b)) PIX_SWAP((a),(b)); } 
#define PIX_SWAP(a,b) { uint16_t temp=(a);(a)=(b);(b)=temp; }

/*pointer to array of 3 pixel values
a pixelvalue
optimized search of the median of 3 pixel values found on sci.image.processing
cannot go faster unless assumptions are made
on the nature of the input signal.
*/ 
uint16_t median3On16Bits(uint16_t * p)
{
    PIX_SORT(p[0],p[1]) ; 
    PIX_SORT(p[1],p[2]) ; 
    PIX_SORT(p[0],p[1]) ;
    
    return(p[1]) ;
}


void sendByteToUsb(uint8_t b)
{
    if (isUsbUartConnected()==true)
    {
        while(sendByteToUsbUart(b)==0);
    }
}
