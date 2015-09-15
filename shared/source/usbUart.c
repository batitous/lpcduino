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

#include "../include/usbUart.h"

//------------------ private defines

#define USB_CDC_BUFFER_SIZE     512     // Size of USB CDC memory (in second RAM bank memory)

#ifndef UARTUSB_BUFFER_SIZE
#       define UARTUSB_BUFFER_SIZE     128     // Size of USB Uart stream (in kernel memory)
#endif

typedef struct _virtual_uart_driver_ 
{
    USBD_HANDLE_T       hUsb;
    USBD_HANDLE_T       hCdc;
    uint8_t *           receiveMemory;
    uint16_t            receiveSize;
} VirtualUartDriver;

#define USB_CONNECTED           0
#define USB_TX_BUSY             1

//------------------ private variables


const  USBD_API_T *             g_pUsbApi;      // ROM USB API
static USBD_HANDLE_T            usbHandle;      
static USBD_API_INIT_PARAM_T    usbApiParam;
static USB_CORE_DESCS_T         usbCoreDesc;

static volatile uint16_t        usbFlags;       // Flag for connection and tx busy
static VirtualUartDriver        virtualUartDriver;

static KIOStream                uartUsbStream;
static uint8_t                  uartUsbBuffer[UARTUSB_BUFFER_SIZE];
static KTask*                   uartUsbTask;


//------------------ private functions


extern USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass);

void USB_IRQHandler(void)
{
    USBD_API->hw->ISR(usbHandle);
}

/* VCOM bulk EP_IN endpoint handler */
static ErrorCode_t virtualComBulkInHandler(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
    if (event == USB_EVT_IN)
    {
        CLRBIT(usbFlags, USB_TX_BUSY);
    }
    return LPC_OK;
}

/* VCOM bulk EP_OUT endpoint handler */
static ErrorCode_t virtualComBulkOutHandler(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
    uint32_t i;
    VirtualUartDriver *pVcom = (VirtualUartDriver *) data;

    switch (event) 
    {
    case USB_EVT_OUT:
        pVcom->receiveSize = USBD_API->hw->ReadEP(hUsb, USB_CDC_OUT_EP, pVcom->receiveMemory);

        for (i=0 ; i < pVcom->receiveSize; i++)
        {
            writeByteToIOStream(&uartUsbStream,pVcom->receiveMemory[i]);
        }

        postEventToTask(uartUsbTask,(uint32_t)pVcom->receiveSize);
        break;
    case USB_EVT_OUT_NAK:
        pVcom->receiveSize = USBD_API->hw->ReadReqEP(hUsb, USB_CDC_OUT_EP, pVcom->receiveMemory, USB_CDC_BUFFER_SIZE);

        for (i=0 ; i < pVcom->receiveSize; i++)
        {
            writeByteToIOStream(&uartUsbStream,pVcom->receiveMemory[i]);
        }

        postEventToTask(uartUsbTask,(uint32_t)pVcom->receiveSize);
        break;
    default:
        break;
    }

    return LPC_OK;
}

/* Set line coding call back routine */
static ErrorCode_t virtualComSetLineCode(USBD_HANDLE_T hCDC, CDC_LINE_CODING *line_coding)
{
    /* Called when baud rate is changed/set. Using it to know host connection state */
    usbFlags = BIT(USB_CONNECTED);
    
    return LPC_OK;
}


static ErrorCode_t virtualComInit(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T *pDesc, USBD_API_INIT_PARAM_T *pUsbParam)
{
    USBD_CDC_INIT_PARAM_T cdc_param;
    ErrorCode_t ret = LPC_OK;
    uint32_t ep_indx;

    initIOStream(&uartUsbStream,uartUsbBuffer,UARTUSB_BUFFER_SIZE);
    usbFlags = 0;
    
    virtualUartDriver.hUsb = hUsb;
    memset((void *) &cdc_param, 0, sizeof(USBD_CDC_INIT_PARAM_T));
    cdc_param.mem_base = pUsbParam->mem_base;
    cdc_param.mem_size = pUsbParam->mem_size;
    cdc_param.cif_intf_desc = (uint8_t *) find_IntfDesc(pDesc->high_speed_desc, CDC_COMMUNICATION_INTERFACE_CLASS);
    cdc_param.dif_intf_desc = (uint8_t *) find_IntfDesc(pDesc->high_speed_desc, CDC_DATA_INTERFACE_CLASS);
    cdc_param.SetLineCode = virtualComSetLineCode;

    ret = USBD_API->cdc->init(hUsb, &cdc_param, &virtualUartDriver.hCdc);

    if (ret == LPC_OK) 
    {
        /* allocate transfer buffers */
        virtualUartDriver.receiveMemory = (uint8_t *) cdc_param.mem_base;
        cdc_param.mem_base += USB_CDC_BUFFER_SIZE;
        cdc_param.mem_size -= USB_CDC_BUFFER_SIZE;

        /* register endpoint interrupt handler */
        ep_indx = (((USB_CDC_IN_EP & 0x0F) << 1) + 1);
        ret = USBD_API->core->RegisterEpHandler(hUsb, ep_indx, virtualComBulkInHandler, &virtualUartDriver);
        if (ret == LPC_OK) 
        {
            /* register endpoint interrupt handler */
            ep_indx = ((USB_CDC_OUT_EP & 0x0F) << 1);
            ret = USBD_API->core->RegisterEpHandler(hUsb, ep_indx, virtualComBulkOutHandler, &virtualUartDriver);
        }
        
        /* update mem_base and size variables for cascading calls. */
        pUsbParam->mem_base = cdc_param.mem_base;
        pUsbParam->mem_size = cdc_param.mem_size;
    }

    return ret;
}

//------------------ public functions

bool initUsbUart(void)
{
    ErrorCode_t ret = LPC_OK;
        
    /* initialize USBD ROM API pointer. */
    g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->pUSBD;

    /* initialize call back structures */
    memset((void *) &usbApiParam, 0, sizeof(USBD_API_INIT_PARAM_T));
    usbApiParam.usb_reg_base = LPC_USB0_BASE;
    /*	WORKAROUND for artf44835 ROM driver BUG:
        Code clearing STALL bits in endpoint reset routine corrupts memory area
        next to the endpoint control data. For example When EP0, EP1_IN, EP1_OUT,
        EP2_IN are used we need to specify 3 here. But as a workaround for this
        issue specify 4. So that extra EPs control structure acts as padding buffer
        to avoid data corruption. Corruption of padding memory doesn’t affect the
        stack/program behaviour.
     */
    usbApiParam.max_num_ep = 3 + 1;
    usbApiParam.mem_base = USB_STACK_MEM_BASE;
    usbApiParam.mem_size = USB_STACK_MEM_SIZE;

    /* Set the USB descriptors */
    usbCoreDesc.device_desc = (uint8_t *) &USB_DeviceDescriptor[0];
    usbCoreDesc.string_desc = (uint8_t *) &USB_StringDescriptor[0];
    /* Note, to pass USBCV test full-speed only devices should have both
       descriptor arrays point to same location and device_qualifier set to 0.
     */
    usbCoreDesc.high_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
    usbCoreDesc.full_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
    usbCoreDesc.device_qualifier = 0;

    /* USB Initialization */
    ret = USBD_API->hw->Init(&usbHandle, &usbCoreDesc, &usbApiParam);
    if (ret == LPC_OK)
    {
        ret = virtualComInit(usbHandle, &usbCoreDesc, &usbApiParam);
        if (ret == LPC_OK) 
        {
            NVIC_EnableIRQ(USB0_IRQn);
            USBD_API->hw->Connect(usbHandle, 1);
        }
    }
    else
    {
        return false;
    }
    
    return true;
}

void closeUsbUart(void)
{
    USBD_API->hw->Connect(usbHandle, 0);
}

uint32_t sendByteToUsbUart(uint8_t data)
{
    VirtualUartDriver *pVcom = &virtualUartDriver;
    uint32_t ret = 0;

    if ( ((usbFlags & BIT(USB_CONNECTED))!=0) && ((usbFlags & BIT(USB_TX_BUSY)) == 0) )
    {
        SETBIT(usbFlags, USB_TX_BUSY);

        NVIC_DisableIRQ(USB0_IRQn);
        ret = USBD_API->hw->WriteEP(pVcom->hUsb, USB_CDC_IN_EP, &data, 1);
        NVIC_EnableIRQ(USB0_IRQn);
    }

    return ret;
}

uint32_t sendBufferToUsbUart(uint8_t *pBuf, uint32_t len)
{
    VirtualUartDriver *pVcom = &virtualUartDriver;
    uint32_t ret = 0;

    if ( ((usbFlags & BIT(USB_CONNECTED))!=0) && ((usbFlags & BIT(USB_TX_BUSY)) == 0) )
    {
        SETBIT(usbFlags, USB_TX_BUSY);

        NVIC_DisableIRQ(USB0_IRQn);
        ret = USBD_API->hw->WriteEP(pVcom->hUsb, USB_CDC_IN_EP, pBuf, len);
        NVIC_EnableIRQ(USB0_IRQn);
    }

    return ret;
}

uint32_t getBufferFromUsbUart(uint8_t * buffer, uint32_t len)
{
    return  readBufferFromIOStream(&uartUsbStream, buffer, len);
}

bool getByteFromUsbUart(uint8_t *data)
{
    return readByteFromIOStream(&uartUsbStream, data);
}

uint32_t getByteAvailableOnUsbUart(void)
{
    return getByteAvailableFromIOStream(&uartUsbStream);
}

void assignTaskOnUsbUart(KTask* t)
{
    uartUsbTask = t;
}

bool isUsbUartConnected(void) 
{
    return (usbFlags & BIT(USB_CONNECTED));
}
