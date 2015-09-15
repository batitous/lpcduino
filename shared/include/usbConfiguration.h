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

#ifndef USB_CONFIGUTATION_H
#define	USB_CONFIGUTATION_H

#ifdef	__cplusplus
extern "C" {
#endif


/** @ingroup EXAMPLES_USBDROM_15XX_CDC
 * @{
 */

/* Manifest constants used by USBD ROM stack. These values SHOULD NOT BE CHANGED
   for advance features which require usage of USB_CORE_CTRL_T structure.
   Since these are the values used for compiling USB stack.
 */
#define USB_MAX_IF_NUM          8		/*!< Max interface number used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_EP_NUM          5		/*!< Max number of EP used for building USBD ROM. DON'T CHANGE. */
#define USB_MAX_PACKET0         64		/*!< Max EP0 packet size used for building USBD ROM. DON'T CHANGE. */
#define USB_FS_MAX_BULK_PACKET  64		/*!< MAXP for FS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_HS_MAX_BULK_PACKET  512		/*!< MAXP for HS bulk EPs used for building USBD ROM. DON'T CHANGE. */
#define USB_DFU_XFER_SIZE       2048	/*!< Max DFU transfer size used for building USBD ROM. DON'T CHANGE. */

/* Manifest constants defining interface numbers and endpoints used by a
   particular interface in this application.
 */
#define USB_CDC_CIF_NUM         0
#define USB_CDC_DIF_NUM         1
#define USB_CDC_IN_EP           0x81
#define USB_CDC_OUT_EP          0x01
#define USB_CDC_INT_EP          0x82

/* The following manifest constants are used to define this memory area to be used
   by USBD ROM stack.
 */
#define USB_STACK_MEM_BASE      0x02008000
#define USB_STACK_MEM_SIZE      0x1000

/* USB descriptor arrays defined *_desc.c file */
extern const uint8_t USB_DeviceDescriptor[];
extern const uint8_t USB_FsConfigDescriptor[];
extern const uint8_t USB_StringDescriptor[];
extern const uint8_t USB_DeviceQualifier[];

/**
 * @brief	Find the address of interface descriptor for given class type.
 * @param	pDesc		: Pointer to configuration descriptor in which the desired class
 *			interface descriptor to be found.
 * @param	intfClass	: Interface class type to be searched.
 * @return	If found returns the address of requested interface else returns NULL.
 */
extern USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass);


#ifdef	__cplusplus
}
#endif

#endif	/* APP_USBD_CFG_H */

