//*****************************************************************************
//
// usbdevicepriv.h - Private header file used to share internal variables and
//                   function prototypes between the various device-related
//                   modules in the USB library.  This header MUST NOT be
//                   used by application code.
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

#ifndef __USBDEVICEPRIV_H__
#define __USBDEVICEPRIV_H__

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
// The states for endpoint zero during enumeration.
//
//*****************************************************************************
typedef enum
{
    //
    // The USB device is waiting on a request from the host controller on
    // endpoint zero.
    //
    eUSBStateIdle,

    //
    // The USB device is sending data back to the host due to an IN request.
    //
    eUSBStateTx,

    //
    // The USB device is sending the configuration descriptor back to the host
    // due to an IN request.
    //
    eUSBStateTxConfig,

    //
    // The USB device is receiving data from the host due to an OUT
    // request from the host.
    //
    eUSBStateRx,

    //
    // The USB device has completed the IN or OUT request and is now waiting
    // for the host to acknowledge the end of the IN/OUT transaction.  This
    // is the status phase for a USB control transaction.
    //
    eUSBStateStatus,

    //
    // This endpoint has signaled a stall condition and is waiting for the
    // stall to be acknowledged by the host controller.
    //
    eUSBStateStall
}
tEP0State;


extern tDeviceInfo *g_ppsDevInfo[];

//*****************************************************************************
//
// Device enumeration functions provided by device/usbenum.c and called from
// the interrupt handler in device/usbhandler.c
//
//*****************************************************************************
extern tBoolean USBDeviceConfig(unsigned int ulIndex,
                                const tConfigHeader *psConfig,
                                const tFIFOConfig *psFIFOConfig);
extern tBoolean USBDeviceConfigAlternate(unsigned int ulIndex,
                                         const tConfigHeader *psConfig,
                                         unsigned char ucInterfaceNum,
                                         unsigned char ucAlternateSetting);
extern void USBDeviceResumeTickHandler(unsigned int ulIndex);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBDEVICEPRIV_H__
