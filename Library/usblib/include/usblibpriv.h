//*****************************************************************************
//
// usblibpriv.h - Private header file used to share internal variables and
//                function prototypes between the various modules in the USB
//                library.  This header MUST NOT be used by application code.
//
// Copyright (c) 2008-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the Tiva USB Library.
//
//*****************************************************************************

#ifndef __USBLIBPRIV_H__
#define __USBLIBPRIV_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Internal interrupt handlers called from the main vectors in device and
// host mode.
//
//*****************************************************************************
extern void USBDeviceIntHandlerInternal(unsigned int ulIndex,
                                 unsigned int ulStatus, unsigned int *ePStatus);
extern void USBHostIntHandlerInternal(unsigned int ulIndex,
                                      unsigned int ulStatus, unsigned int *endPStatus);

//*****************************************************************************
//
// These defines are used to register the tick handlers used by the stack.
// These handlers are internal to the stack and should never be called directly
// by an application.
//
//*****************************************************************************
#define USB_TICK_HANDLER_OTG        0   // OTG mode tick handler.
#define USB_TICK_HANDLER_DEVICE     1   // Device mode tick handler.
#define USB_TICK_HANDLER_HOST       2   // Host mode tick handler.
#define USB_TICK_HANDLER_NUM        3   // Total number of tick handlers.

//*****************************************************************************
//
// This value defines the number of SOF ticks that must pass before a call
// is made to InternalUSBStartOfFrameTick.  The value 5 ensures that the
// function is called every 5 milliseconds assuming that SOF interrupts are
// enabled and SOF is present.
//
//*****************************************************************************
#define USB_SOF_TICK_DIVIDE 5

//*****************************************************************************
//
// Tick handler function pointer type.
//
//*****************************************************************************
typedef void(* tUSBTickHandler)(void *pvInstance, unsigned int ulTicksmS, 
                                                        unsigned int ulIndex);

//*****************************************************************************
//
// Internal functions use to initialize the tick handler and register tick
// callbacks.
//
//*****************************************************************************
extern void InternalUSBTickInit(void);
extern void InternalUSBRegisterTickHandler(unsigned int ulHandler,
                                           tUSBTickHandler pfHandler,
                                           void *pvInstance);
extern void InternalUSBStartOfFrameTick(unsigned int ulTicksmS, 
                                        unsigned int ulIndex);
extern void InternalUSBHCDSendEvent(unsigned int ulIndex, unsigned int ulEvent);

//*****************************************************************************
//
// g_ulCurrentUSBTick holds the elapsed time in milliseconds since the
// tick module was first initialized based on calls to the function
// InternalUSBStartOfFrameTick.  The granularity is USB_SOF_TICK_DIVIDE
// milliseconds.
//
//*****************************************************************************
extern unsigned int g_ui32CurrentUSBTick;

//*****************************************************************************
//
// g_ulUSBSOFCount is a global counter for Start of Frame interrupts.  It is
// incremented by the low level device- or host-mode interrupt handlers.
//
//*****************************************************************************
extern unsigned int g_ui32USBSOFCount;

//*****************************************************************************
//
// InternalUSBGetTime is a macro which will return the system time in
// milliseconds as calculated based on calls to the function
// InternalUSBStartOfFrameTick.  The granularity is USB_SOF_TICK_DIVIDE
// milliseconds.
//
// Currently, this merely returns the value of a global variable.
//
//*****************************************************************************
#define InternalUSBGetTime() g_ulCurrentUSBTick
//
// Maximum number of channels for Type 0 USB controllers.
//
#define USB_MAX_DMA_CHANNELS_0  6

//
// Maximum number of channels for all other USB controllers.
//
#define USB_MAX_DMA_CHANNELS    8

//*****************************************************************************
//
// Values returned by the USBLibDMAChannelStatus() function.
//
//*****************************************************************************
#define USBLIBSTATUS_DMA_IDLE   0x00000000
#define USBLIBSTATUS_DMA_COMPLETE                                             \
                                0x00000001
#define USBLIBSTATUS_DMA_ERROR  0x00000002
#define USBLIBSTATUS_DMA_PENDING                                              \
                                0x00000004

//*****************************************************************************
//
// DMA endpoint types used with the USBLibDMAChannelAllocate() function.
//
//*****************************************************************************
#define USB_DMA_EP_RX           0x00000080
#define USB_DMA_EP_TX           0x00000000
#define USB_DMA_EP_HOST         0x00000040
#define USB_DMA_EP_DEVICE       0x00000000
#define USB_DMA_EP_TYPE_CTRL    0x00000000
#define USB_DMA_EP_TYPE_ISOC    0x00000001
#define USB_DMA_EP_TYPE_BULK    0x00000002
#define USB_DMA_EP_TYPE_INT     0x00000003
#define USB_DMA_EP_TYPE_M       0x00000003

//*****************************************************************************
//
// This is the internal instance data for the DMA functions and should not
// be modified outside the usbdma.c file.
//
//*****************************************************************************
struct tUSBDMAInstance
{
    uint32_t ui32Base;

    uint32_t ui32IntNum;

    uint32_t pui32Config[USB_MAX_DMA_CHANNELS];

    uint32_t pui32MaxPacketSize[USB_MAX_DMA_CHANNELS];

    uint32_t *ppui32Data[USB_MAX_DMA_CHANNELS];

    uint32_t pui32Count[USB_MAX_DMA_CHANNELS];

    uint8_t pui8Endpoint[USB_MAX_DMA_CHANNELS];

    uint32_t pui32EPDMAMode0[USB_MAX_DMA_CHANNELS];

    uint32_t pui32EPDMAMode1[USB_MAX_DMA_CHANNELS];

    uint32_t ui32Pending;

    uint32_t ui32Complete;

};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif // __USBLIBPRIV_H__
