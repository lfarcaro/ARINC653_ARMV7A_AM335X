//*****************************************************************************
//
// usbenum.c - Enumeration code to handle all endpoint zero traffic.
//
// Copyright (c) 2007-2017 Texas Instruments Incorporated.  All rights reserved.
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

#include <stdint.h>
#include "hw_usb.h"
#include "hw_types.h"
#include "debug.h"
#include "usb.h"
#include "usblib.h"
#include "usbdevice.h"
#include "usbdevicepriv.h"
#include "usblibpriv.h"
#include "delay.h"
#include "interrupt.h"

//*****************************************************************************
//
// External prototypes.
//
//*****************************************************************************
//extern tUSBMode g_eUSBMode;
static tUSBMode g_iUSBMode = USB_MODE_DEVICE;


//*****************************************************************************
//
// USB instance Object
//
//*****************************************************************************
extern tUSBInstanceObject g_USBInstance[];

//*****************************************************************************
//
// Local functions prototypes.
//
//*****************************************************************************
static void USBDGetStatus(void *pvInstance, tUSBRequest *psUSBRequest, uint32_t ui32Index);
static void USBDClearFeature(void *pvInstance, tUSBRequest *psUSBRequest, uint32_t ui32Index);
static void USBDSetFeature(void *pvInstance, tUSBRequest *psUSBRequest, uint32_t ui32Index);
static void USBDSetAddress(void *pvInstance, tUSBRequest *psUSBRequest, uint32_t ui32Index);
static void USBDGetDescriptor(void *pvInstance, tUSBRequest *psUSBRequest, uint32_t ui32Index);
static void USBDSetDescriptor(void *pvInstance, tUSBRequest *psUSBRequest, uint32_t ui32Index);
static void USBDGetConfiguration(void *pvInstance,tUSBRequest *psUSBRequest, uint32_t ui32Index);
static void USBDSetConfiguration(void *pvInstance,tUSBRequest *psUSBRequest, uint32_t ui32Index);
static void USBDGetInterface(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                        uint32_t ui32Index);
static void USBDSetInterface(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                        uint32_t ui32Index);
static void USBDSyncFrame(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                        uint32_t ui32Index);
static void USBDEP0StateTx(uint32_t ui32Index);
static void USBDEP0StateTxConfig(uint32_t ui32Index);
static int USBDStringIndexFromRequest(uint16_t ui16Lang,
                                      uint16_t ui16Index, uint32_t ui32Index);

//*****************************************************************************
//
//! \addtogroup device_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! The default USB endpoint FIFO configuration structure.  This structure
//! contains definitions to set all USB FIFOs into single buffered mode with
//! no DMA use.  Each endpoint's FIFO is sized to hold the largest maximum
//! packet size for any interface alternate setting in the current
//! configuration descriptor.  A pointer to this structure may be passed in the
//! psFIFOConfig field of the tDeviceInfo structure passed to USBCDCInit if the
//! application does not require any special handling of the USB controller
//! FIFO.
//
//*****************************************************************************
const tFIFOConfig g_sUSBDefaultFIFOConfig =
{
    {
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 }
    },
    {
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 },
        { 1, false, 0 }
    },
};

//*****************************************************************************
//
// Indices into the ucHalt array to select the IN or OUT endpoint group.
//
//*****************************************************************************
#define HALT_EP_IN              0
#define HALT_EP_OUT             1

//*****************************************************************************
//
// Define the max packet size for endpoint zero.
//
//*****************************************************************************
#define EP0_MAX_PACKET_SIZE     64

//*****************************************************************************
//
// This is a flag used with g_sUSBDeviceState.ui32DevAddress to indicate that a
// device address change is pending.
//
//*****************************************************************************
#define DEV_ADDR_PENDING        0x80000000

//*****************************************************************************
//
// This label defines the default configuration number to use after a bus
// reset.  This may be overridden by calling USBDCDSetDefaultConfiguration()
// during processing of the device reset handler if required.
//
//*****************************************************************************
#define DEFAULT_CONFIG_ID       1

//*****************************************************************************
//
// This label defines the number of milliseconds that the remote wake up signal
// must remain asserted before removing it. Section 7.1.7.7 of the USB 2.0 spec
// states that "the remote wake up device must hold the resume signaling for at
// least 1ms but for no more than 15ms" so 10mS seems a reasonable choice.
//
//*****************************************************************************
#define REMOTE_WAKEUP_PULSE_MS 10

//*****************************************************************************
//
// This label defines the number of milliseconds between the point where we
// assert the remote wake up signal and calling the client back to tell it that
// bus operation has been resumed.  This value is based on the timings provided
// in section 7.1.7.7 of the USB 2.0 specification which indicates that the
// host (which takes over resume signaling when the device's initial signal is
// detected) must hold the resume signaling for at least 20mS.
//
//*****************************************************************************
#define REMOTE_WAKEUP_READY_MS 20

//*****************************************************************************
//
// The LPM states.
//
//*****************************************************************************
#define USBLIB_LPM_STATE_DISABLED   0x00000000
#define USBLIB_LPM_STATE_AWAKE      0x00000001
#define USBLIB_LPM_STATE_SLEEP      0x00000002

//*****************************************************************************
//
// The buffer for reading data coming into EP0
//
//*****************************************************************************
static uint8_t g_pui8DataBufferIn[EP0_MAX_PACKET_SIZE];

//*****************************************************************************
//
// This is 480000000/60000000 or a PLL Divide of 8.
//
//*****************************************************************************
static uint32_t g_ui32PLLDiv = 8;

//*****************************************************************************
//
// Holds the ULPI configuration.
//
//*****************************************************************************
static uint32_t g_ui32ULPISupport;

//*****************************************************************************
//
// This is the instance data for the USB controller itself and not a USB
// device class.
//
//*****************************************************************************
typedef struct
{
    //
    // The device information for the USB device.
    //
    tDeviceInfo *pvCBData;

    //
    // The instance data for the USB device.
    //
    void *pvInstance;

    //
    // The current state of endpoint zero.
    //
    volatile tEP0State iEP0State;

    //
    // The devices current address, this also has a change pending bit in the
    // MSB of this value specified by DEV_ADDR_PENDING.
    //
    volatile uint32_t ui32DevAddress;

    //
    // This holds the current active configuration for this device.
    //
    uint32_t ui32Configuration;

    //
    // This holds the configuration id that will take effect after a reset.
    //
    uint32_t ui32DefaultConfiguration;

    //
    // This holds the current alternate interface for this device.
    //
    uint8_t pui8AltSetting[USB_MAX_INTERFACES_PER_DEVICE];

    //
    // This is the pointer to the current data being sent out or received
    // on endpoint zero.
    //
    uint8_t *pui8EP0Data;

    //
    // This is the number of bytes that remain to be sent from or received
    // into the g_sUSBDeviceState.pEP0Data data buffer.
    //
    volatile uint32_t ui32EP0DataRemain;

    //
    // The amount of data being sent/received due to a custom request.
    //
    uint32_t ui32OUTDataSize;

    //
    // Holds the current device status.
    //
    uint8_t ui8Status;

    //
    // Holds the endpoint status for the HALT condition.  This array is sized
    // to hold halt status for all IN and OUT endpoints.
    //
    uint8_t ucHalt[2][NUM_USB_EP - 1];

    //
    // Holds the configuration descriptor section number currently being sent
    // to the host.
    //
    uint8_t ui8ConfigSection;

    //
    // Holds the offset within the configuration descriptor section currently
    // being sent to the host.
    //
    uint8_t ui16SectionOffset;

    //
    // Holds the index of the configuration that we are currently sending back
    // to the host.
    //
    uint8_t ui8ConfigIndex;

    //
    // This flag is set to true if the client has called USBDPowerStatusSet
    // and tells the USB library not to try to determine the current power
    // status from the configuration descriptor.
    //
    tBoolean bPwrSrcSet;

    //
    // This flag indicates whether or not remote wake up signaling is in
    // progress.
    //
    tBoolean bRemoteWakeup;

    //
    // During remote wake up signaling, this counter is used to track the
    // number of milliseconds since the signaling was initiated.
    //
    uint8_t ui8RemoteWakeupCount;
}
tDeviceInstance;

tDeviceInstance g_psUSBDevice[USB_NUM_INSTANCE];

//*****************************************************************************
//
// Function table to handle standard requests.
//
//*****************************************************************************
static const tStdRequest g_psUSBDStdRequests[] =
{
    USBDGetStatus,
    USBDClearFeature,
    0,
    USBDSetFeature,
    0,
    USBDSetAddress,
    USBDGetDescriptor,
    USBDSetDescriptor,
    USBDGetConfiguration,
    USBDSetConfiguration,
    USBDGetInterface,
    USBDSetInterface,
    USBDSyncFrame
};

//*****************************************************************************
//
// TODO: The OS Descriptor request response.
//
//*****************************************************************************
const uint8_t g_pOSDescriptorPresentString[] =
{
    0x14,
    USB_DTYPE_STRING,
    'M', 0, 'S', 0, 'F', 0, 'T', 0, '1', 0, '0', 0, '0', 0,
    0xBE, // TODO: Vendor code Preprocessor definition?
    0
};


//*****************************************************************************
//
//! Initialize the USB library device control driver for a given hardware
//! controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! initialized.
//! \param psDevice is a pointer to a structure containing information that
//! the USB library requires to support operation of this application's
//! device.  The structure contains event handler callbacks and pointers to the
//! various standard descriptors that the device wishes to publish to the
//! host.
//! \param pvDCDCBData is the callback data for any device callbacks.
//!
//! This function must be called by a device class which wishes to operate
//! as a USB device and is not typically called by an application.  This
//! function initializes the USB device control driver for the given
//! controller and saves the device information for future use.  Prior to
//! returning from this function, the device is connected to the USB bus.
//! Following return, the caller can expect to receive a callback to the
//! supplied <tt>pfnResetHandler</tt> function when a host connects to the
//! device.  The \e pvDCDCBData contains a pointer to data that is returned
//! with the DCD calls back to the function in the psDevice->psCallbacks()
//! functions.
//!
//! The device information structure passed in \e psDevice must remain
//! unchanged between this call and any matching call to USBDCDTerm() because
//! it is not copied by the USB library.
//!
//! The USBStackModeSet() function can be called with eUSBModeForceDevice in
//! order to cause the USB library to force the USB operating mode to a device
//! controller.  This allows the application to used the USBVBUS and USBID pins
//! as GPIOs on devices that support forcing OTG to operate as a device only
//! controller.  By default the USB library will assume that the USBVBUS and
//! USBID pins are configured as USB pins and not GPIOs.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDInit(uint32_t ui32Index, tDeviceInfo *psDevice)
{
    const tConfigHeader *psHdr;
    const tConfigDescriptor *psDesc;

    //
    // Check the arguments.
    //
    ASSERT(ui32Index == 0);
    ASSERT(psDevice != 0);

    //
    // Should not call this if the stack is in host mode.
    //
    ASSERT(g_iUSBMode != USB_MODE_HOST)

    //
    // Initialize a couple of fields in the device state structure.
    //
    g_psUSBDevice[ui32Index].ui32Configuration = DEFAULT_CONFIG_ID;
    g_psUSBDevice[ui32Index].ui32DefaultConfiguration = DEFAULT_CONFIG_ID;

    //
    // Remember the device information pointer.
    //
    g_psUSBDevice[ui32Index].pvCBData = psDevice;
    g_psUSBDevice[ui32Index].pvInstance = psDevice->pvInstance;
    g_psUSBDevice[ui32Index].iEP0State = eUSBStateIdle;
     
    //
    // If no mode is set then make the mode become device mode.
    //
    if(g_iUSBMode == USB_MODE_NONE)
    {
        g_iUSBMode = USB_MODE_DEVICE;
    }

    //
    // Only do hardware update if the stack is in Device mode, do not touch the
    // hardware for OTG mode operation.
    //
    if(g_iUSBMode == USB_MODE_DEVICE)
    {
        //
        // Enable Clocking to the USB controller.
        //
    
        USBModuleClkEnable(ui32Index, g_USBInstance[ui32Index].uiBaseAddr);

        USBReset(g_USBInstance[ui32Index].uiSubBaseAddr);
        //
        // Turn on USB Phy clock.
        //
        UsbPhyOn(ui32Index);
    }

    

    //
    // Only do hardware update if the stack is in Device mode, do not touch the
    // hardware for OTG mode operation.
    //
    if(g_iUSBMode == USB_MODE_DEVICE)
    {
        //
        // Ask for the interrupt status.  As a side effect, this clears all
        // pending USB interrupts.
        //
        USBIntStatusControl(g_USBInstance[ui32Index].uiBaseAddr);
           USBIntStatusEndpoint(g_USBInstance[ui32Index].uiBaseAddr);
        if(USB_REV_AM1808 == USBVersionGet())
        {
            USBClearOtgIntr(g_USBInstance[ui32Index].uiSubBaseAddr);
        }

        USBEnableOtgIntr(g_USBInstance[ui32Index].uiSubBaseAddr);
        //
        // Enable USB Interrupts.
        //
        USBIntEnableControl(g_USBInstance[ui32Index].uiBaseAddr, USB_INTCTRL_RESET |
                                       USB_INTCTRL_DISCONNECT |
                                       USB_INTCTRL_RESUME |
                                       USB_INTCTRL_SUSPEND |
                                       USB_INTCTRL_SOF);
        USBIntEnableEndpoint(g_USBInstance[ui32Index].uiBaseAddr, USB_INTEP_ALL);
        
    }

    //
    // Get a pointer to the default configuration descriptor.
    //
    psHdr = psDevice->ppsConfigDescriptors[
                g_psUSBDevice[ui32Index].ui32DefaultConfiguration - 1];
    psDesc = (const tConfigDescriptor *)(psHdr->psSections[0]->pui8Data);

    //
    // Default to the state where remote wake up is disabled.
    //
    g_psUSBDevice[ui32Index].ui8Status = 0;
    g_psUSBDevice[ui32Index].bRemoteWakeup = false;

    //
    // Determine the self- or bus-powered state based on the flags the
    // user provided.
    //
    g_psUSBDevice[ui32Index].bPwrSrcSet = false;

    if((psDesc->bmAttributes & USB_CONF_ATTR_PWR_M) == USB_CONF_ATTR_SELF_PWR)
    {
        g_psUSBDevice[ui32Index].ui8Status |= USB_STATUS_SELF_PWR;
    }
    else
    {
        g_psUSBDevice[ui32Index].ui8Status &= ~USB_STATUS_SELF_PWR;
    }

    //
    // Only do hardware update if the stack is in Device mode, do not touch the
    // hardware for OTG mode operation.
    //
    if(g_iUSBMode == USB_MODE_DEVICE)
    {
        //
        // Make sure we disconnect from the host for a while.  This ensures
        // that the host will enumerate us even if we were previously
        // connected to the bus.
        //
        USBDevDisconnect(g_USBInstance[ui32Index].uiBaseAddr);

        //
        // Wait about 100mS.
        //
        delay(100);


        //
        // Attach the device using the soft connect.
        //
        USBDevConnect(g_USBInstance[ui32Index].uiBaseAddr);

        //
        // Enable the USB interrupt.
        //
#ifdef _TMS320C6X
        /* No DSP API to enable USB0 event */
#else
        //Disabled for operation in polling mode
        //IntSystemEnable(g_USBInstance[ui32Index].uiInterruptNum);
#endif


    }
}

//*****************************************************************************
//
//! Free the USB library device control driver for a given hardware controller.
//!
//! \param ui32Index is the index of the USB controller which is to be
//! freed.
//!
//! This function should be called by an application if it no longer requires
//! the use of a given USB controller to support its operation as a USB device.
//! It frees the controller for use by another client.
//!
//! It is the caller's responsibility to remove its device from the USB bus
//! prior to calling this function.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDTerm(uint32_t ui32Index)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Index == 0);

    g_psUSBDevice[ui32Index].pvCBData = (tDeviceInfo *)0;
    g_psUSBDevice[ui32Index].pvInstance = 0;

    //
    // Disable the USB interrupts.
    //
#ifdef _TMS320C6X
    /* No DSP API to disable USB0 event */
#else
    IntSystemDisable(g_USBInstance[ui32Index].uiInterruptNum);
#endif

    USBIntDisableControl(g_USBInstance[ui32Index].uiBaseAddr, USB_INTCTRL_ALL);
    USBIntDisableEndpoint(g_USBInstance[ui32Index].uiBaseAddr, USB_INTEP_ALL);

    //
    // Detach the device using the soft connect.
    //
    USBDevDisconnect(g_USBInstance[ui32Index].uiBaseAddr);

    //
    // Clear any pending interrupts.
    //
    USBIntStatusControl(g_USBInstance[ui32Index].uiBaseAddr);
    USBIntStatusEndpoint(g_USBInstance[ui32Index].uiBaseAddr);
    if(USB_REV_AM1808 == USBVersionGet())
    {
        USBClearOtgIntr(g_USBInstance[ui32Index].uiSubBaseAddr);
    }

    //
    // Turn off USB Phy clock.
    //
    UsbPhyOff(ui32Index);

    //
    // Disable the USB peripheral
    //
    USBModuleClkDisable(ui32Index, g_USBInstance[ui32Index].uiBaseAddr);
     
}

//*****************************************************************************
//
//! This function starts the request for data from the host on endpoint zero.
//!
//! \param ui32Index is the index of the USB controller from which the data
//! is being requested.
//! \param pui8Data is a pointer to the buffer to fill with data from the USB
//! host.
//! \param ui32Size is the size of the buffer or data to return from the USB
//! host.
//!
//! This function handles retrieving data from the host when a custom command
//! has been issued on endpoint zero.  If the application needs notification
//! when the data has been received,
//! <tt>psCallbacks->pfnDataReceived()</tt> in the tDeviceInfo structure
//! must contain valid function pointer.  In nearly all cases this is necessary
//! because the caller of this function would likely need to know that the data
//! requested was received.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDRequestDataEP0(uint32_t ui32Index, uint8_t *pui8Data, uint32_t ui32Size)
{
    ASSERT(ui32Index == 0);

    //
    // Enter the RX state on end point 0.
    //
    g_psUSBDevice[ui32Index].iEP0State = eUSBStateRx;

    //
    // Save the pointer to the data.
    //
    g_psUSBDevice[ui32Index].pui8EP0Data = pui8Data;

    //
    // Location to save the current number of bytes received.
    //
    g_psUSBDevice[ui32Index].ui32OUTDataSize = ui32Size;

    //
    // Bytes remaining to be received.
    //
    g_psUSBDevice[ui32Index].ui32EP0DataRemain = ui32Size;
}

//*****************************************************************************
//
//! This function requests transfer of data to the host on endpoint zero.
//!
//! \param ui32Index is the index of the USB controller which is to be used to
//! send the data.
//! \param pui8Data is a pointer to the buffer to send via endpoint zero.
//! \param ui32Size is the amount of data to send in bytes.
//!
//! This function handles sending data to the host when a custom command is
//! issued or non-standard descriptor has been requested on endpoint zero.  If
//! the application needs notification when this is complete,
//! <tt>psCallbacks->pfnDataSent</tt> in the tDeviceInfo structure must
//! contain a valid function pointer.  This callback could be used to free up
//! the buffer passed into this function in the \e pui8Data parameter.  The
//! contents of the \e pui8Data buffer must remain unchanged until the
//! <tt>pfnDataSent</tt> callback is received.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDSendDataEP0(uint32_t ui32Index, uint8_t *pui8Data, uint32_t ui32Size)
{
    ASSERT(ui32Index == 0);

    //
    // Return the externally provided device descriptor.
    //
    g_psUSBDevice[ui32Index].pui8EP0Data = pui8Data;

    //
    // The size of the device descriptor is in the first byte.
    //
    g_psUSBDevice[ui32Index].ui32EP0DataRemain = ui32Size;

    //
    // Save the total size of the data sent.
    //
    g_psUSBDevice[ui32Index].ui32OUTDataSize = ui32Size;

    //
    // Now in the transmit data state.
    //
    USBDEP0StateTx(ui32Index);
}

//*****************************************************************************
//
//! This function sets the default configuration for the device.
//!
//! \param ui32Index is the index of the USB controller whose default
//! configuration is to be set.
//! \param ui32DefaultConfig is the configuration identifier (byte 6 of the
//! standard configuration descriptor) which is to be presented to the host
//! as the default configuration in cases where the configuration descriptor is
//! queried prior to any specific configuration being set.
//!
//! This function allows a device to override the default configuration
//! descriptor that will be returned to a host whenever it is queried prior
//! to a specific configuration having been set.  The parameter passed must
//! equal one of the configuration identifiers found in the
//! <tt>ppsConfigDescriptors</tt> array for the device.
//!
//! If this function is not called, the USB library will return the first
//! configuration in the <tt>ppsConfigDescriptors</tt> array as the default
//! configuration.
//!
//! \note The USB device stack assumes that the configuration IDs (byte 6 of
//! the configuration descriptor, <tt>bConfigurationValue</tt>) stored within
//! the configuration descriptor array, <tt>ppsConfigDescriptors</tt>,
//! are equal to the array index + 1.  In other words, the first entry in the
//! array must contain a descriptor with <tt>bConfigurationValue</tt> 1, the
//! second must have <tt>bConfigurationValue</tt> 2 and so on.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDSetDefaultConfiguration(uint32_t ui32Index, uint32_t ui32DefaultConfig)
{
    ASSERT(ui32Index == 0);

    g_psUSBDevice[ui32Index].ui32DefaultConfiguration = ui32DefaultConfig;
}

//*****************************************************************************
//
//! This function generates a stall condition on endpoint zero.
//!
//! \param ui32Index is the index of the USB controller whose endpoint zero is
//! to be stalled.
//!
//! This function is typically called to signal an error condition to the host
//! when an unsupported request is received by the device.  It should be
//! called from within the callback itself (in interrupt context) and not
//! deferred until later since it affects the operation of the endpoint zero
//! state machine in the USB library.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDStallEP0(uint32_t ui32Index)
{
    ASSERT(ui32Index == 0);

    //
    // Stall the endpoint in question.
    //
    USBDevEndpointStall(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, USB_EP_DEV_OUT);

    //
    // Enter the stalled state.
    //
    g_psUSBDevice[ui32Index].iEP0State = eUSBStateStall;
}
#ifndef DEPRECATED

//*****************************************************************************
//
//! Reports the device power status (bus- or self-powered) to the library.
//!
//! \param ui32Index is the index of the USB controller whose device power
//! status is being reported.
//! \param ui8Power indicates the current power status, either
//! \b USB_STATUS_SELF_PWR or \b USB_STATUS_BUS_PWR.
//!
//! Applications which support switching between bus- or self-powered
//! operation should call this function whenever the power source changes
//! to indicate the current power status to the USB library.  This information
//! is required by the library to allow correct responses to be provided when
//! the host requests status from the device.
//!
//! \return None.
//
//*****************************************************************************
void
USBDCDPowerStatusSet(uint32_t ui32Index, uint8_t ui8Power)
{
    //
    // Check for valid parameters.
    //
    ASSERT((ui8Power == USB_STATUS_BUS_PWR) ||
           (ui8Power == USB_STATUS_SELF_PWR));
    ASSERT(ui32Index == 0);

    //
    // Update the device status with the new power status flag.
    //
    g_psUSBDevice[ui32Index].bPwrSrcSet = true;
    g_psUSBDevice[ui32Index].ui8Status &= ~USB_STATUS_PWR_M;
    g_psUSBDevice[ui32Index].ui8Status |= ui8Power;
}
#endif


//*****************************************************************************
//
//! Requests a remote wake up to resume communication when in suspended state.
//!
//! \param ui32Index is the index of the USB controller that will request
//! a bus wake up.
//!
//! When the bus is suspended, an application which supports remote wake up
//! (advertised to the host via the configuration descriptor) may call this
//! function to initiate remote wake up signaling to the host.  If the remote
//! wake up feature has not been disabled by the host, this will cause the bus
//! to resume operation within 20mS.  If the host has disabled remote wake up,
//! \b false will be returned to indicate that the wake up request was not
//! successful.
//!
//! \return Returns \b true if the remote wake up is not disabled and the
//! signaling was started or \b false if remote wake up is disabled or if
//! signaling is currently ongoing following a previous call to this function.
//
//*****************************************************************************
tBoolean
USBDCDRemoteWakeupsRequest(uint32_t ui32Index)
{
    //
    // Check for parameter validity.
    //
    ASSERT(ui32Index == 0);

    //
    // Is remote wake up signaling currently enabled?
    //
    if(g_psUSBDevice[ui32Index].ui8Status & USB_STATUS_REMOTE_WAKE)
    {
        //
        // The host has not disabled remote wake up. Are we still in the
        // middle of a previous wake up sequence?
        //
        if(!g_psUSBDevice[ui32Index].bRemoteWakeup)
        {
            //
            // No - we are not in the middle of a wake up sequence so start
            // one here.
            //
            g_psUSBDevice[ui32Index].ui8RemoteWakeupCount = 0;
            g_psUSBDevice[ui32Index].bRemoteWakeup = true;
            USBHostResume(g_USBInstance[ui32Index].uiBaseAddr, true);
            return(true);
        }
    }

    //
    // If we drop through to here, signaling was not initiated so return
    // false.
    return(false);
}

//*****************************************************************************
//
// Internal Functions, not to be called by applications
//
//*****************************************************************************

//*****************************************************************************
//
// This internal function is called on the SOF interrupt to process any
// outstanding remote wake up requests.
//
// \return None.
//
//*****************************************************************************
void
USBDeviceResumeTickHandler(uint32_t ui32Index)
{
    if(g_psUSBDevice[ui32Index].bRemoteWakeup)
    {
        //
        // Increment the millisecond counter we use to time the resume
        // signaling.
        //
        g_psUSBDevice[ui32Index].ui8RemoteWakeupCount++;

        //
        // Have we reached the 10mS mark? If so, we need to turn the signaling
        // off again.
        //
        if(g_psUSBDevice[ui32Index].ui8RemoteWakeupCount == REMOTE_WAKEUP_PULSE_MS)
        {
            USBHostResume(g_USBInstance[ui32Index].uiBaseAddr, false);
        }

        //
        // Have we reached the point at which we can tell the client that the
        // bus has resumed? The controller does not give us an interrupt if we
        // initiated the wake up signaling so we just wait until 20mS have
        // passed then tell the client all is well.
        //
        if(g_psUSBDevice[ui32Index].ui8RemoteWakeupCount == REMOTE_WAKEUP_READY_MS)
        {
            //
            // We are now finished with the remote wake up signaling.
            //
            g_psUSBDevice[ui32Index].bRemoteWakeup = false;

            //
            // If the client has registered a resume callback, call it.  In the
            // case of a remote wake up request, we do not get a resume
            // interrupt from the controller so we need to fake it here.
            //
            if(g_psUSBDevice[ui32Index].pvCBData->psCallbacks.pfnResumeHandler)
            {
                g_psUSBDevice[ui32Index].pvCBData->psCallbacks.pfnResumeHandler(
                    g_psUSBDevice[ui32Index].pvInstance);
            }
        }
    }
}

//*****************************************************************************
//
// This internal function reads a request data packet and dispatches it to
// either a standard request handler or the registered device request
// callback depending upon the request type.
//
// \return None.
//
//*****************************************************************************
static void
USBDReadAndDispatchRequest(uint32_t ui32Index)
{
    uint32_t ui32Size;
    tUSBRequest *psRequest;

    //
    // Cast the buffer to a request structure.
    //
    psRequest = (tUSBRequest *)g_pui8DataBufferIn;

    //
    // Set the buffer size.
    //
    ui32Size = EP0_MAX_PACKET_SIZE;

    //
    // Get the data from the USB controller end point 0.
    //
    USBEndpointDataGet(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, g_pui8DataBufferIn,
                       &ui32Size);

    //
    // If there was a null setup packet then just return.
    //
    if(!ui32Size)
    {
        return;
    }

    //
    // See if this is a standard request or not.
    //
    if((psRequest->bmRequestType & USB_RTYPE_TYPE_M) != USB_RTYPE_STANDARD)
    {
        //
        // Since this is not a standard request, see if there is
        // an external handler present.
        //
        if(g_psUSBDevice[ui32Index].pvCBData->psCallbacks.pfnRequestHandler)
        {
            g_psUSBDevice[ui32Index].pvCBData->psCallbacks.pfnRequestHandler(
                    g_psUSBDevice[ui32Index].pvInstance, psRequest, ui32Index);
        }
        else
        {
            //
            // If there is no handler then stall this request.
            //
            USBDCDStallEP0(ui32Index);
        }
    }
    else
    {
        //
        // Assure that the jump table is not out of bounds.
        //
        if((psRequest->bRequest <
           (sizeof(g_psUSBDStdRequests) / sizeof(tStdRequest))) &&
           (g_psUSBDStdRequests[psRequest->bRequest] != 0))
        {
            //
            // Jump table to the appropriate handler.
            //
            g_psUSBDStdRequests[psRequest->bRequest](&g_psUSBDevice[ui32Index],
                                                    psRequest, ui32Index);
        }
        else
        {
            //
            // If there is no handler then stall this request.
            //
            USBDCDStallEP0(ui32Index);
        }
    }
}

//*****************************************************************************
//
// This is interrupt handler for endpoint zero.
//
// This function handles all interrupts on endpoint zero in order to maintain
// the state needed for the control endpoint on endpoint zero.  In order to
// successfully enumerate and handle all USB standard requests, all requests
// on endpoint zero must pass through this function.  The endpoint has the
// following states: \b eUSBStateIdle, \b eUSBStateTx, \b eUSBStateRx,
// \b eUSBStateStall, and \b eUSBStateStatus.  In the \b eUSBStateIdle
// state the USB controller has not received the start of a request, and once
// it does receive the data for the request it will either enter the
// \b eUSBStateTx, \b eUSBStateRx, or \b eUSBStateStall depending on the
// command.  If the controller enters the \b eUSBStateTx or \b eUSBStateRx
// then once all data has been sent or received, it must pass through the
// \b eUSBStateStatus state to allow the host to acknowledge completion of
// the request.  The \b eUSBStateStall is entered from \b eUSBStateIdle in
// the event that the USB request was not valid.  Both the \b eUSBStateStall
// and \b eUSBStateStatus are transitional states that return to the
// \b eUSBStateIdle state.
//
// \return None.
//
// eUSBStateIdle -*--> eUSBStateTx -*-> eUSBStateStatus -*->eUSBStateIdle
//                 |                  |                     |
//                |--> eUSBStateRx                       |
//                 |                                        |
//                |--> eUSBStateStall ---------->--------
//
//  ----------------------------------------------------------------
// | Current State       | State 0           | State 1              |
// | --------------------|-------------------|----------------------
// | eUSBStateIdle      | eUSBStateTx/RX   | eUSBStateStall      |
// | eUSBStateTx        | eUSBStateStatus  |                      |
// | eUSBStateRx        | eUSBStateStatus  |                      |
// | eUSBStateStatus    | eUSBStateIdle    |                      |
// | eUSBStateStall     | eUSBStateIdle    |                      |
//  ----------------------------------------------------------------
//
//*****************************************************************************
void
USBDeviceEnumHandler(tDeviceInstance *pDevInstance, uint32_t ui32Index)
{
    uint32_t ui32EPStatus, ui32DataSize;

    //
    // Get the end point 0 status.
    //
    ui32EPStatus = USBEndpointStatus(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0);

    switch(pDevInstance->iEP0State)
    {
        //
        // Handle the status state, this is a transitory state from
        // eUSBStateTx or eUSBStateRx back to eUSBStateIdle.
        //
        case eUSBStateStatus:
        {
            //
            // Just go back to the idle state.
            //
            pDevInstance->iEP0State = eUSBStateIdle;

            //
            // If there is a pending address change then set the address.
            //
            if(pDevInstance->ui32DevAddress & DEV_ADDR_PENDING)
            {
                //
                // Clear the pending address change and set the address.
                //
                pDevInstance->ui32DevAddress &= ~DEV_ADDR_PENDING;
                USBDevAddrSet(g_USBInstance[ui32Index].uiBaseAddr, pDevInstance->ui32DevAddress);
            }

            //
            // If a new packet is already pending, we need to read it
            // and handle whatever request it contains.
            //
            if(ui32EPStatus & USB_DEV_EP0_OUT_PKTRDY)
            {
                //
                // Process the newly arrived packet.
                //
                USBDReadAndDispatchRequest(0);
            }
            break;
        }

        //
        // In the IDLE state the code is waiting to receive data from the host.
        //
        case eUSBStateIdle:
        {
            //
            // Is there a packet waiting for us?
            //
            if(ui32EPStatus & USB_DEV_EP0_OUT_PKTRDY)
            {
                //
                // Yes - process it.
                //
                USBDReadAndDispatchRequest(0);
            }
            break;
        }

        //
        // Data is still being sent to the host so handle this in the
        // EP0StateTx() function.
        //
        case eUSBStateTx:
        {
            USBDEP0StateTx(ui32Index);
            break;
        }

        //
        // We are still in the middle of sending the configuration descriptor
        // so handle this in the EP0StateTxConfig() function.
        //
        case eUSBStateTxConfig:
        {
            USBDEP0StateTxConfig(ui32Index);
            break;
        }

        //
        // Handle the receive state for commands that are receiving data on
        // endpoint zero.
        //
        case eUSBStateRx:
        {
            //
            // Set the number of bytes to get out of this next packet.
            //
            if(pDevInstance->ui32EP0DataRemain > EP0_MAX_PACKET_SIZE)
            {
                //
                // Don't send more than EP0_MAX_PACKET_SIZE bytes.
                //
                ui32DataSize = EP0_MAX_PACKET_SIZE;
            }
            else
            {
                //
                // There was space so send the remaining bytes.
                //
                ui32DataSize = pDevInstance->ui32EP0DataRemain;
            }

            //
            // Get the data from the USB controller end point 0.
            //
            USBEndpointDataGet(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, 
                                pDevInstance->pui8EP0Data, &ui32DataSize);

            //
            // If there we not more that EP0_MAX_PACKET_SIZE or more bytes
            // remaining then this transfer is complete.  If there were exactly
            // EP0_MAX_PACKET_SIZE remaining then there still needs to be
            // null packet sent before this is complete.
            //
            if(pDevInstance->ui32EP0DataRemain < EP0_MAX_PACKET_SIZE)
            {
                //
                // Return to the idle state.
                //
                pDevInstance->iEP0State =  eUSBStateStatus;

                //
                // If there is a receive callback then call it.
                //
                if((pDevInstance->pvCBData->psCallbacks.pfnDataReceived) &&
                   (pDevInstance->ui32OUTDataSize != 0))
                {
                    //
                    // Call the custom receive handler to handle the data
                    // that was received.
                    //
                    pDevInstance->pvCBData->psCallbacks.pfnDataReceived(
                        pDevInstance->pvInstance,
                        pDevInstance->ui32OUTDataSize, ui32Index);

                    //
                    // Indicate that there is no longer any data being waited
                    // on.
                    //
                    pDevInstance->ui32OUTDataSize = 0;
                }

                //
                // Need to ACK the data on end point 0 in this case and set the
                // data end as this is the last of the data.
                //
                USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, true);
            }
            else
            {
                //
                // Need to ACK the data on end point 0 in this case
                // without setting data end because more data is coming.
                //
                USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, false);
            }

            //
            // Advance the pointer.
            //
            pDevInstance->pui8EP0Data += ui32DataSize;

            //
            // Decrement the number of bytes that are being waited on.
            //
            pDevInstance->ui32EP0DataRemain -= ui32DataSize;

            break;
        }
        //
        // The device stalled endpoint zero so check if the stall needs to be
        // cleared once it has been successfully sent.
        //
        case eUSBStateStall:
        {
            //
            // If we sent a stall then acknowledge this interrupt.
            //
            if(ui32EPStatus & USB_DEV_EP0_SENT_STALL)
            {
                //
                // Clear the Setup End condition.
                //
                USBDevEndpointStatusClear(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0,
                                          USB_DEV_EP0_SENT_STALL);

                //
                // Reset the global end point 0 state to IDLE.
                //
                pDevInstance->iEP0State = eUSBStateIdle;

            }
            break;
        }
        //
        // Halt on an unknown state, but only in DEBUG mode builds.
        //
        default:
        {
            ASSERT(0);
            break;
        }
    }
}

//*****************************************************************************
//
// This function handles bus reset notifications.
//
// This function is called from the low level USB interrupt handler whenever
// a bus reset is detected.  It performs tidy-up as required and resets the
// configuration back to defaults in preparation for descriptor queries from
// the host.
//
// \return None.
//
//*****************************************************************************
void
USBDeviceEnumResetHandler(tDeviceInstance *pDevInstance)
{
    uint32_t ui32Loop;

    //
    // Disable remote wake up signaling (as per USB 2.0 spec 9.1.1.6).
    //
    pDevInstance->ui8Status &= ~USB_STATUS_REMOTE_WAKE;
    pDevInstance->bRemoteWakeup = false;

    //
    // Call the device dependent code to indicate a bus reset has occurred.
    //
    if(pDevInstance->pvCBData->psCallbacks.pfnResetHandler)
    {
        pDevInstance->pvCBData->psCallbacks.pfnResetHandler(
            pDevInstance->pvInstance);
    }

    //
    // Reset the default configuration identifier and alternate function
    // selections.
    //
    pDevInstance->ui32Configuration = pDevInstance->ui32DefaultConfiguration;

    for(ui32Loop = 0; ui32Loop < USB_MAX_INTERFACES_PER_DEVICE; ui32Loop++)
    {
        pDevInstance->pui8AltSetting[ui32Loop] = (uint8_t)0;
    }
}

//*****************************************************************************
//
// This function handles the GET_STATUS standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the request type and endpoint number if endpoint
// status is requested.
//
// This function handles responses to a Get Status request from the host
// controller.  A status request can be for the device, an interface or an
// endpoint.  If any other type of request is made this function will cause
// a stall condition to indicate that the command is not supported.  The
// \e psUSBRequest structure holds the type of the request in the
// bmRequestType field.  If the type indicates that this is a request for an
// endpoint's status, then the wIndex field holds the endpoint number.
//
// \return None.
//
//*****************************************************************************
static void
USBDGetStatus(void *pvInstance, tUSBRequest *psUSBRequest, 
                                            uint32_t ui32Index)    
{
    uint16_t ui16Data, ui16Index;
    uint32_t ui32Dir;
    tDeviceInstance *psUSBControl;

    ASSERT(psUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, false);

    //
    // Determine what type of status was requested.
    //
    switch(psUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
        //
        // This was a Device Status request.
        //
        case USB_RTYPE_DEVICE:
        {
            //
            // Return the current status for the device.
            //
            ui16Data = (uint16_t)psUSBControl->ui8Status;

            break;
        }

        //
        // This was a Interface status request.
        //
        case USB_RTYPE_INTERFACE:
        {
            //
            // Interface status always returns 0.
            //
            ui16Data = (uint16_t)0;

            break;
        }

        //
        // This was an endpoint status request.
        //
        case USB_RTYPE_ENDPOINT:
        {
            //
            // Which endpoint are we dealing with?
            //
            ui16Index = psUSBRequest->wIndex & USB_REQ_EP_NUM_M;

            //
            // Check if this was a valid endpoint request.
            //
            if((ui16Index == 0) || (ui16Index >= NUM_USB_EP))
            {
                USBDCDStallEP0(ui32Index);
                return;
            }
            else
            {
                //
                // Are we dealing with an IN or OUT endpoint?
                //
                ui32Dir = ((psUSBRequest->wIndex & USB_REQ_EP_DIR_M) ==
                         USB_REQ_EP_DIR_IN) ? HALT_EP_IN : HALT_EP_OUT;

                //
                // Get the current halt status for this endpoint.
                //
                ui16Data =
                      (uint16_t)psUSBControl->ucHalt[ui32Dir][ui16Index - 1];
            }
            break;
        }

        //
        // This was an unknown request.
        //
        default:
        {
            //
            // Anything else causes a stall condition to indicate that the
            // command was not supported.
            //
            USBDCDStallEP0(ui32Index);
            return;
        }
    }

    //
    // Send the two byte status response.
    //
    psUSBControl->ui32EP0DataRemain = 2;
    psUSBControl->pui8EP0Data = (uint8_t *)&ui16Data;

    //
    // Send the response.
    //
    USBDEP0StateTx(ui32Index);
}

//*****************************************************************************
//
// This function handles the CLEAR_FEATURE standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the options for the Clear Feature USB request.
//
// This function handles device or endpoint clear feature requests.  The
// \e psUSBRequest structure holds the type of the request in the bmRequestType
// field and the feature is held in the wValue field.  The device can only
// clear the Remote Wake feature.  This device request should only be made if
// the descriptor indicates that Remote Wake is implemented by the device.
// Endpoints can only clear a halt on a given endpoint.  If any other
// requests are made, then the device will stall the request to indicate to
// the host that the command was not supported.
//
// \return None.
//
//*****************************************************************************
static void
USBDClearFeature(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                uint32_t ui32Index)
{
    tDeviceInstance *psUSBControl;
    uint32_t ui32Dir;
    uint16_t ui16Index;

    ASSERT(psUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, true);

    //
    // Determine what type of status was requested.
    //
    switch(psUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
        //
        // This is a clear feature request at the device level.
        //
        case USB_RTYPE_DEVICE:
        {
            //
            // Only remote wake is can be cleared by this function.
            //
            if(USB_FEATURE_REMOTE_WAKE & psUSBRequest->wValue)
            {
                //
                // Clear the remote wake up state.
                //
                psUSBControl->ui8Status &= ~USB_STATUS_REMOTE_WAKE;
            }
            else
            {
                USBDCDStallEP0(ui32Index);
            }
            break;
        }

        //
        // This is a clear feature request at the endpoint level.
        //
        case USB_RTYPE_ENDPOINT:
        {
            //
            // Which endpoint are we dealing with?
            //
            ui16Index = psUSBRequest->wIndex & USB_REQ_EP_NUM_M;

            //
            // Not a valid endpoint.
            //
            if((ui16Index == 0) || (ui16Index > NUM_USB_EP))
            {
                USBDCDStallEP0(ui32Index);
            }
            else
            {
                //
                // Only the halt feature is supported.
                //
                if(USB_FEATURE_EP_HALT == psUSBRequest->wValue)
                {
                    //
                    // Are we dealing with an IN or OUT endpoint?
                    //
                    ui32Dir = ((psUSBRequest->wIndex & USB_REQ_EP_DIR_M) ==
                             USB_REQ_EP_DIR_IN) ? HALT_EP_IN : HALT_EP_OUT;

                    //
                    // Clear the halt condition on this endpoint.
                    //
                    psUSBControl->ucHalt[ui32Dir][ui16Index - 1] = 0;

                    if(ui32Dir == HALT_EP_IN)
                    {
                        USBDevEndpointStallClear(g_USBInstance[ui32Index].uiBaseAddr,
                                                 IndexToUSBEP(ui16Index),
                                                 USB_EP_DEV_IN);
                    }
                    else
                    {
                        USBDevEndpointStallClear(g_USBInstance[ui32Index].uiBaseAddr,
                                                 IndexToUSBEP(ui16Index),
                                                 USB_EP_DEV_OUT);
                    }
                }
                else
                {
                    //
                    // If any other feature is requested, this is an error.
                    //
                    USBDCDStallEP0(ui32Index);
                    return;
                }
            }
            break;
        }

        //
        // This is an unknown request.
        //
        default:
        {
            USBDCDStallEP0(ui32Index);
            return;
        }
    }
}

//*****************************************************************************
//
// This function handles the SET_FEATURE standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the feature in the wValue field of the USB
// request.
//
// This function handles device or endpoint set feature requests.  The
// \e psUSBRequest structure holds the type of the request in the bmRequestType
// field and the feature is held in the wValue field.  The device can only
// set the Remote Wake feature.  This device request should only be made if the
// descriptor indicates that Remote Wake is implemented by the device.
// Endpoint requests can only issue a halt on a given endpoint.  If any other
// requests are made, then the device will stall the request to indicate to the
// host that the command was not supported.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetFeature(void *pvInstance, tUSBRequest *psUSBRequest, 
                                            uint32_t ui32Index)
{
    tDeviceInstance *psUSBControl;
    uint16_t ui16Index;
    uint32_t ui32Dir;

    ASSERT(psUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, true);

    //
    // Determine what type of status was requested.
    //
    switch(psUSBRequest->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
        //
        // This is a set feature request at the device level.
        //
        case USB_RTYPE_DEVICE:
        {
            //
            // Only remote wake is the only feature that can be set by this
            // function.
            //
            if(USB_FEATURE_REMOTE_WAKE & psUSBRequest->wValue)
            {
                //
                // Set the remote wake up state.
                //
                psUSBControl->ui8Status |= USB_STATUS_REMOTE_WAKE;
            }
            else
            {
                USBDCDStallEP0(ui32Index);
            }
            break;
        }

        //
        // This is a set feature request at the endpoint level.
        //
        case USB_RTYPE_ENDPOINT:
        {
            //
            // Which endpoint are we dealing with?
            //
            ui16Index = psUSBRequest->wIndex & USB_REQ_EP_NUM_M;

            //
            // Not a valid endpoint?
            //
            if((ui16Index == 0) || (ui16Index >= NUM_USB_EP))
            {
                USBDCDStallEP0(ui32Index);
            }
            else
            {
                //
                // Only the Halt feature can be set.
                //
                if(USB_FEATURE_EP_HALT == psUSBRequest->wValue)
                {
                    //
                    // Are we dealing with an IN or OUT endpoint?
                    //
                    ui32Dir = ((psUSBRequest->wIndex & USB_REQ_EP_DIR_M) ==
                             USB_REQ_EP_DIR_IN) ? HALT_EP_IN : HALT_EP_OUT;

                    //
                    // Clear the halt condition on this endpoint.
                    //
                    psUSBControl->ucHalt[ui32Dir][ui16Index - 1] = 1;
                }
                else
                {
                    //
                    // No other requests are supported.
                    //
                    USBDCDStallEP0(ui32Index);
                    return;
                }
            }
            break;
        }

        //
        // This is an unknown request.
        //
        default:
        {
            USBDCDStallEP0(ui32Index);
            return;
        }
    }
}

//*****************************************************************************
//
// This function handles the SET_ADDRESS standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the new address to use in the wValue field of the
// USB request.
//
// This function is called to handle the change of address request from the
// host controller.  This can only start the sequence as the host must
// acknowledge that the device has changed address.  Thus this function sets
// the address change as pending until the status phase of the request has
// been completed successfully.  This prevents the devices address from
// changing and not properly responding to the status phase.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetAddress(void *pvInstance, tUSBRequest *psUSBRequest, 
                                            uint32_t ui32Index)
{
    tDeviceInstance *psUSBControl;

    ASSERT(psUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, true);

    //
    // Save the device address as we cannot change address until the status
    // phase is complete.
    //
    psUSBControl->ui32DevAddress = psUSBRequest->wValue | DEV_ADDR_PENDING;

    //
    // Transition directly to the status state since there is no data phase
    // for this request.
    //
    psUSBControl->iEP0State = eUSBStateStatus;
}

//*****************************************************************************
//
// This function handles the GET_DESCRIPTOR standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the data for this request.
//
// This function will return most of the descriptors requested by the host
// controller.  The descriptor specified by \e
// pvInstance->psInfo->pui8DeviceDescriptor will be returned when the device
// descriptor is requested.  If a request for a specific configuration
// descriptor is made, then the appropriate descriptor from the \e
// g_pConfigDescriptors will be returned.  When a request for a string
// descriptor is made, the appropriate string from the
// \e pvInstance->psInfo->pStringDescriptors will be returned.  If the
// \e pvInstance->psInfo->psCallbacks->GetDescriptor is specified it will be
// called to handle the request.  In this case it must call the
// USBDCDSendDataEP0() function to send the data to the host controller.  If
// the callback is not specified, and the descriptor request is not for a
// device, configuration, or string descriptor then this function will stall
// the request to indicate that the request was not supported by the device.
//
// \return None.
//
//*****************************************************************************
static void
USBDGetDescriptor(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                uint32_t ui32Index)
{
    tBoolean bConfig;
    tDeviceInstance *psUSBControl;
    tDeviceInfo *psDevice;
    const tConfigHeader *psConfig;
    const tDeviceDescriptor *psDeviceDesc;
    uint8_t ui8Index;
    int32_t i32Index;

    ASSERT(psUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;
    psDevice = psUSBControl->pvCBData;

    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, false);

    //
    // Assume we are not sending the configuration descriptor until we
    // determine otherwise.
    //
    bConfig = false;

    //
    // Which descriptor are we being asked for?
    //
    switch(psUSBRequest->wValue >> 8)
    {
        //
        // This request was for a device descriptor.
        //
        case USB_DTYPE_DEVICE:
        {
            //
            // Return the externally provided device descriptor.
            //
            psUSBControl->pui8EP0Data =
                (uint8_t *)psDevice->pui8DeviceDescriptor;

            //
            // The size of the device descriptor is in the first byte.
            //
            psUSBControl->ui32EP0DataRemain = 
               psDevice->pui8DeviceDescriptor[0];

            break;
        }

        //
        // This request was for a configuration descriptor.
        //
        case USB_DTYPE_CONFIGURATION:
        {
            //
            // Which configuration are we being asked for?
            //
            ui8Index = (uint8_t)(psUSBRequest->wValue & 0xFF);

            //
            // Is this valid?
            //
            psDeviceDesc =
                (const tDeviceDescriptor *)psDevice->pui8DeviceDescriptor;

            if(ui8Index >= psDeviceDesc->bNumConfigurations)
            {
                //
                // This is an invalid configuration index.  Stall EP0 to
                // indicate a request error.
                //
                USBDCDStallEP0(ui32Index);
                psUSBControl->pui8EP0Data = 0;
                psUSBControl->ui32EP0DataRemain = 0;
            }
            else
            {
                //
                // Return the externally specified configuration descriptor.
                //
                psConfig = psDevice->ppsConfigDescriptors[ui8Index];

                //
                // Start by sending data from the beginning of the first
                // descriptor.
                //
                psUSBControl->ui8ConfigSection = 0;
                psUSBControl->ui16SectionOffset = 0;
                psUSBControl->pui8EP0Data = 
                                (uint8_t *)psConfig->psSections[0]->pui8Data;

                //
                // Determine the total size of the configuration descriptor
                // by counting the sizes of the sections comprising it.
                //
                psUSBControl->ui32EP0DataRemain =
                                            USBDCDConfigDescGetSize(psConfig);

                //
                // Remember that we need to send the configuration descriptor
                // and which descriptor we need to send.
                //
                psUSBControl->ui8ConfigIndex = ui8Index;

                bConfig = true;
            }
            break;
        }

        //
        // This request was for a string descriptor.
        //
        case USB_DTYPE_STRING:
        {
		//
		// TODO: Special case - Microsoft OS Descriptors
		//
		if ( psUSBRequest->wValue == 0x03EE )
		{
//			UARTwrite("[os]",4);
			//
			// Return the externally specified configuration descriptor.
			//
			psUSBControl->pui8EP0Data = (uint8_t *) g_pOSDescriptorPresentString;

			//
			// The total size of a string descriptor is in byte 0.
			//
			psUSBControl->ui32EP0DataRemain = g_pOSDescriptorPresentString[0];

			break;
		}

            //
            // Determine the correct descriptor index based on the requested
            // language ID and index.
            //
            i32Index = USBDStringIndexFromRequest(psUSBRequest->wIndex,
                                          psUSBRequest->wValue & 0xFF, ui32Index);

            //
            // If the mapping function returned -1 then stall the request to
            // indicate that the request was not valid.
            //
            if(i32Index == -1)
            {
                USBDCDStallEP0(ui32Index);
                break;
            }

            //
            // Return the externally specified configuration descriptor.
            //
            psUSBControl->pui8EP0Data =
                (uint8_t *)psDevice->ppStringDescriptors[i32Index];

            //
            // The total size of a string descriptor is in byte 0.
            //
            psUSBControl->ui32EP0DataRemain =
                psDevice->ppStringDescriptors[i32Index][0];

            break;
        }

        //
        // Any other request is not handled by the default enumeration handler
        // so see if it needs to be passed on to another handler.
        //
        default:
        {
            //
            // If there is a handler for requests that are not handled then
            // call it.
            //
            if(psDevice->psCallbacks.pfnGetDescriptor)
            {
                psDevice->psCallbacks.pfnGetDescriptor(psUSBControl->pvInstance,
                                                      psUSBRequest, ui32Index);
                return;
            }
            else
            {
                //
                // Whatever this was this handler does not understand it so
                // just stall the request.
                //
                USBDCDStallEP0(ui32Index);
            }

            return;
        }
    }

    //
    // If this request has data to send, then send it.
    //
    if(psUSBControl->pui8EP0Data)
    {
        //
        // If there is more data to send than is requested then just
        // send the requested amount of data.
        //
        if(psUSBControl->ui32EP0DataRemain > psUSBRequest->wLength)
        {
            psUSBControl->ui32EP0DataRemain = psUSBRequest->wLength;
        }

        //
        // Now in the transmit data state.  Be careful to call the correct
        // function since we need to handle the configuration descriptor
        // differently from the others.
        //
        if(!bConfig)
        {
            USBDEP0StateTx(ui32Index);
        }
        else
        {
            USBDEP0StateTxConfig(ui32Index);
        }
    }
}

//*****************************************************************************
//
// This function determines which string descriptor to send to satisfy a
// request for a given index and language.
//
// \param ui16Lang is the requested string language ID.
// \param ui16Index is the requested string descriptor index.
//
// When a string descriptor is requested, the host provides a language ID and
// index to identify the string ("give me string number 5 in French").  This
// function maps these two parameters to an index within our device's string
// descriptor array which is arranged as multiple groups of strings with
// one group for each language advertised via string descriptor 0.
//
// We assume that there are an equal number of strings per language and
// that the first descriptor is the language descriptor and use this fact to
// perform the mapping.
//
// \return The index of the string descriptor to return or -1 if the string
// could not be found.
//
//*****************************************************************************
static int32_t
USBDStringIndexFromRequest(uint16_t ui16Lang, uint16_t ui16Index, 
                                                            uint32_t ui32Index)
{
    tString0Descriptor *pLang;
    uint32_t ui32NumLangs, ui32NumStringi16PerLang, ui32Loop;

    //
    // Make sure we have a string table at all.
    //
    if((g_psUSBDevice[ui32Index].pvCBData == 0) ||
       (g_psUSBDevice[ui32Index].pvCBData->ppStringDescriptors == 0))
    {
        return(-1);
    }

    //
    // First look for the trivial case where descriptor 0 is being
    // requested.  This is the special case since descriptor 0 contains the
    // language codes supported by the device.
    //
    if(ui16Index == 0)
    {
        return(0);
    }

    //
    // How many languages does this device support?  This is determined by
    // looking at the length of the first descriptor in the string table,
    // subtracting 2 for the header and dividing by two (the size of each
    // language code).
    //
    ui32NumLangs = 
            (g_psUSBDevice[ui32Index].pvCBData->ppStringDescriptors[0][0] - 2) / 2;

    //
    // We assume that the table includes the same number of strings for each
    // supported language.  We know the number of entries in the string table,
    // so how many are there for each language?  This may seem an odd way to
    // do this (why not just have the application tell us in the device info
    // structure?) but it's needed since we didn't want to change the API
    // after the first release which did not support multiple languages.
    //
    ui32NumStringi16PerLang = 
        ((g_psUSBDevice[ui32Index].pvCBData->ulNumStringDescriptors - 1) / ui32NumLangs);

    //
    // Just to be sure, make sure that the calculation indicates an equal
    // number of strings per language.  We expect the string table to contain
    // (1 + (strings_per_language * languages)) entries.
    //
    if((1 + (ui32NumStringi16PerLang * ui32NumLangs)) !=
       g_psUSBDevice[ui32Index].pvCBData->ulNumStringDescriptors)
    {
        return(-1);
    }

    //
    // Now determine which language we are looking for.  It is assumed that
    // the order of the groups of strings per language in the table is the
    // same as the order of the language IDs listed in the first descriptor.
    //
    pLang = (tString0Descriptor *)
                        (g_psUSBDevice[ui32Index].pvCBData->ppStringDescriptors[0]);

    //
    // Look through the supported languages looking for the one we were asked
    // for.
    //
    for(ui32Loop = 0; ui32Loop < ui32NumLangs; ui32Loop++)
    {
        //
        // Have we found the requested language?
        //
        if(pLang->wLANGID[ui32Loop] == ui16Lang)
        {
            //
            // Yes - calculate the index of the descriptor to send.
            //
            return((ui32NumStringi16PerLang * ui32Loop) + ui16Index);
        }
    }

    //
    // If we drop out of the loop, the requested language was not found so
    // return -1 to indicate the error.
    //
    return(-1);
}

//*****************************************************************************
//
// This function handles the SET_DESCRIPTOR standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the data for this request.
//
// This function currently is not supported and will respond with a Stall
// to indicate that this command is not supported by the device.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetDescriptor(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                uint32_t ui32Index)
{
    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, false);

    //
    // This function is not handled by default.
    //
    USBDCDStallEP0(ui32Index);
}

//*****************************************************************************
//
// This function handles the GET_CONFIGURATION standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the data for this request.
//
// This function responds to a host request to return the current
// configuration of the USB device.  The function will send the configuration
// response to the host and return.  This value will either be 0 or the last
// value received from a call to SetConfiguration().
//
// \return None.
//
//*****************************************************************************
static void
USBDGetConfiguration(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                    uint32_t ui32Index)
{
    uint8_t ui8Value;
    tDeviceInstance *psUSBControl;

    ASSERT(psUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, false);

    //
    // If we still have an address pending then the device is still not
    // configured.
    //
    if(psUSBControl->ui32DevAddress & DEV_ADDR_PENDING)
    {
        ui8Value = 0;
    }
    else
    {
        ui8Value = (uint8_t)psUSBControl->ui32Configuration;
    }

    psUSBControl->ui32EP0DataRemain = 1;
    psUSBControl->pui8EP0Data = &ui8Value;

    //
    // Send the single byte response.
    //
    USBDEP0StateTx(ui32Index);
}

//*****************************************************************************
//
// This function handles the SET_CONFIGURATION standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the data for this request.
//
// This function responds to a host request to change the current
// configuration of the USB device.  The actual configuration number is taken
// from the structure passed in via \e psUSBRequest.  This number should be one
// of the configurations that was specified in the descriptors.  If the
// \e ConfigChange callback is specified in \e pvInstance->psInfo->psCallbacks->
// it will be called so that the application can respond to a change in
// configuration.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetConfiguration(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                    uint32_t ui32Index)
{
    tDeviceInstance *psUSBControl;
    tDeviceInfo *psDevice;
    const tConfigHeader *psHdr;
    const tConfigDescriptor *psDesc;


    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;
    psDevice = psUSBControl->pvCBData;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, true);

    //
    // Cannot set the configuration to one that does not exist so check the
    // enumeration structure to see how many valid configurations are present.
    //
    if(psUSBRequest->wValue > psUSBControl->pvCBData->pui8DeviceDescriptor[17])
    {
        //
        // The passed configuration number is not valid.  Stall the endpoint to
        // signal the error to the host.
        //
        USBDCDStallEP0(ui32Index);
    }
    else
    {
        //
        // Save the configuration.
        //
        psUSBControl->ui32Configuration = psUSBRequest->wValue;

        //
        // If passed a configuration other than 0 (which tells us that we are
        // not currently configured), configure the endpoints (other than EP0)
        // appropriately.
        //
        if(psUSBControl->ui32Configuration)
        {
            //
            // Get a pointer to the configuration descriptor.  This will always
            // be the first section in the current configuration.
            //
            psHdr = psDevice->ppsConfigDescriptors[psUSBRequest->wValue - 1];
            psDesc = 
                (const tConfigDescriptor *)(psHdr->psSections[0]->pui8Data);

            //
            // Remember the new self- or bus-powered state if the user has not
            // already called us to tell us the state to report.
            //
            if(!psUSBControl->bPwrSrcSet)
            {
                if((psDesc->bmAttributes & USB_CONF_ATTR_PWR_M) ==
                    USB_CONF_ATTR_SELF_PWR)
                {
                    psUSBControl->ui8Status |= USB_STATUS_SELF_PWR;
                }
                else
                {
                    psUSBControl->ui8Status &= ~USB_STATUS_SELF_PWR;
                }
            }

            //
            // Configure endpoints for the new configuration.
            //
            USBDeviceConfig(0,
                         psDevice->ppsConfigDescriptors[psUSBRequest->wValue - 1],
                         psDevice->psFIFOConfig);
        }

        //
        // If there is a configuration change callback then call it.
        //
        if(psDevice->psCallbacks.pfnConfigChange)
        {
            psDevice->psCallbacks.pfnConfigChange(psUSBControl->pvInstance, 
                                     psUSBControl->ui32Configuration, ui32Index);
        }
    }
}

//*****************************************************************************
//
// This function handles the GET_INTERFACE standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the data for this request.
//
// This function is called when the host controller request the current
// interface that is in use by the device.  This simply returns the value set
// by the last call to SetInterface().
//
// \return None.
//
//*****************************************************************************
static void
USBDGetInterface(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                uint32_t ui32Index)
{
    uint8_t ui8Value;
    tDeviceInstance *psUSBControl;

    ASSERT(psUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;

    //
    // Need to ACK the data on end point 0 without setting last data as there
    // will be a data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, false);

    //
    // If we still have an address pending then the device is still not
    // configured.
    //
    if(psUSBControl->ui32DevAddress & DEV_ADDR_PENDING)
    {
        ui8Value = (uint8_t)0;
    }
    else
    {
        //
        // Is the interface number valid?
        //
        if(psUSBRequest->wIndex < USB_MAX_INTERFACES_PER_DEVICE)
        {
            //
            // Read the current alternate setting for the required interface.
            //
            ui8Value = psUSBControl->pui8AltSetting[psUSBRequest->wIndex];
        }
        else
        {
            //
            // An invalid interface number was specified.
            //
            USBDCDStallEP0(ui32Index);
            return;
        }
    }

    //
    // Send the single byte response.
    //
    psUSBControl->ui32EP0DataRemain = 1;
    psUSBControl->pui8EP0Data = &ui8Value;

    //
    // Send the single byte response.
    //
    USBDEP0StateTx(ui32Index);
}

//*****************************************************************************
//
// This function handles the SET_INTERFACE standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the data for this request.
//
// This function is called when a standard request for changing the interface
// is received from the host controller.  If this is a valid request the
// function will call the function specified by the InterfaceChange in the
// \e pvInstance->psInfo->psCallbacks->variable to notify the application that
// the interface has changed and will pass it the new alternate interface
// number.
//
// \return None.
//
//*****************************************************************************
static void
USBDSetInterface(void *pvInstance, tUSBRequest *psUSBRequest, 
                                                uint32_t ui32Index)
{
    const tConfigHeader *psConfig;
    tInterfaceDescriptor *psInterface;
    uint32_t ui32Loop, ui32Section, ui32NumInterfaces;
    uint8_t ui8Interface;
    tBoolean bRetcode;
    tDeviceInstance *psUSBControl;
    tDeviceInfo *psDevice;

    ASSERT(psUSBRequest != 0);
    ASSERT(pvInstance != 0);

    //
    // Create the device information pointer.
    //
    psUSBControl = (tDeviceInstance *)pvInstance;
    psDevice = psUSBControl->pvCBData;

    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, true);

    //
    // Use the current configuration.
    //
    psConfig = 
        psDevice->ppsConfigDescriptors[psUSBControl->ui32Configuration - 1];

    //
    // How many interfaces are included in the descriptor?
    //
    ui32NumInterfaces = USBDCDConfigDescGetNum(psConfig, USB_DTYPE_INTERFACE);

    //
    // Find the interface descriptor for the supplied interface and alternate
    // setting numbers.
    //
    for(ui32Loop = 0; ui32Loop < ui32NumInterfaces; ui32Loop++)
    {
        //
        // Get the next interface descriptor in the configuration descriptor.
        //
        psInterface = USBDCDConfigGetInterface(psConfig, ui32Loop,
                                               USB_DESC_ANY, &ui32Section);

        //
        // Is this the required interface with the correct alternate setting?
        //
        if(psInterface &&
           (psInterface->bInterfaceNumber == psUSBRequest->wIndex) &&
           (psInterface->bAlternateSetting == psUSBRequest->wValue))
        {
            ui8Interface = psInterface->bInterfaceNumber;

            //
            // Make sure we don't write outside the bounds of the
            // pui8AltSetting array (in a debug build, anyway, since this
            // indicates an error in the device descriptor).
            //
            ASSERT(ui8Interface < USB_MAX_INTERFACES_PER_DEVICE);

            //
                // This is the correct interface descriptor so save the
                // setting.
                //
                psUSBControl->pui8AltSetting[ui8Interface] =
                                                psInterface->bAlternateSetting;

                //
                // Reconfigure the endpoints to match the requirements of the
                // new alternate setting for the interface.
                //
                bRetcode = USBDeviceConfigAlternate(0, psConfig, 
                                               ui8Interface,
                                               psInterface->bAlternateSetting);

                //
                // If there is a callback then notify the application of the
                // change to the alternate interface.
                //
                if(bRetcode && psDevice->psCallbacks.pfnInterfaceChange)
                {
                    psDevice->psCallbacks.pfnInterfaceChange(
                                                    psUSBControl->pvInstance,
                                                    psUSBRequest->wIndex,
                                                    psUSBRequest->wValue);
                }

            //
            // All done.
            //
            return;
        }
    }

    //
    // If we drop out of the loop, we didn't find an interface descriptor
    // matching the requested number and alternate setting or there was an
    // error while trying to set up for the new alternate setting.
    //
    USBDCDStallEP0(ui32Index);
}

//*****************************************************************************
//
// This function handles the SYNC_FRAME standard USB request.
//
// \param pvInstance is the USB device controller instance data.
// \param psUSBRequest holds the data for this request.
//
// This is currently a stub function that will stall indicating that the
// command is not supported.
//
// \return None.
//
//*****************************************************************************
static void
USBDSyncFrame(void *pvInstance, tUSBRequest *psUSBRequest, 
                                            uint32_t ui32Index)
{
    //
    // Need to ACK the data on end point 0 with last data set as this has no
    // data phase.
    //
    USBDevEndpointDataAck(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, true);

    //
    // Not handled yet so stall this request.
    //
    USBDCDStallEP0(ui32Index);
}

//*****************************************************************************
//
// This internal function handles sending data on endpoint zero.
//
// \param ui32Index is the index of the USB controller which is to be
// initialized.
//
// \return None.
//
//*****************************************************************************
static void
USBDEP0StateTx(uint32_t ui32Index)
{
    uint32_t ui32NumBytes;
    uint8_t *pui8Data;

    ASSERT(ui32Index == 0);

    //
    // In the TX state on endpoint zero.
    //
    g_psUSBDevice[ui32Index].iEP0State = eUSBStateTx;

    //
    // Set the number of bytes to send this iteration.
    //
    ui32NumBytes = g_psUSBDevice[ui32Index].ui32EP0DataRemain;

    //
    // Limit individual transfers to 64 bytes.
    //
    if(ui32NumBytes > EP0_MAX_PACKET_SIZE)
    {
        ui32NumBytes = EP0_MAX_PACKET_SIZE;
    }

    //
    // Save the pointer so that it can be passed to the USBEndpointDataPut()
    // function.
    //
    pui8Data = (uint8_t *)g_psUSBDevice[ui32Index].pui8EP0Data;

    //
    // Advance the data pointer and counter to the next data to be sent.
    //
    g_psUSBDevice[ui32Index].ui32EP0DataRemain -= ui32NumBytes;
    g_psUSBDevice[ui32Index].pui8EP0Data += ui32NumBytes;

    //
    // Put the data in the correct FIFO.
    //
    USBEndpointDataPut(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, pui8Data, ui32NumBytes);

    //
    // If this is exactly 64 then don't set the last packet yet.
    //
    if(ui32NumBytes == EP0_MAX_PACKET_SIZE)
    {
        //
        // There is more data to send or exactly 64 bytes were sent, this
        // means that there is either more data coming or a null packet needs
        // to be sent to complete the transaction.
        //
        USBEndpointDataSend(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, USB_TRANS_IN);
    }
    else
    {
        //
        // Now go to the status state and wait for the transmit to complete.
        //
        g_psUSBDevice[ui32Index].iEP0State = eUSBStateStatus;

        //
        // Send the last bit of data.
        //
        USBEndpointDataSend(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, USB_TRANS_IN_LAST);

        //
        // If there is a sent callback then call it.
        //
        if((g_psUSBDevice[ui32Index].pvCBData->psCallbacks.pfnDataSent) &&
           (g_psUSBDevice[ui32Index].ui32OUTDataSize != 0))
        {
            //
            // Call the custom handler.
            //
            g_psUSBDevice[ui32Index].pvCBData->psCallbacks.pfnDataSent(
                                                           g_psUSBDevice[ui32Index].pvInstance, 
                                                           g_psUSBDevice[ui32Index].ui32OUTDataSize, 
                                                           ui32Index);

            //
            // There is no longer any data pending to be sent.
            //
            g_psUSBDevice[ui32Index].ui32OUTDataSize = 0;
        }
    }
}

//*****************************************************************************
//
// This internal function handles sending the configuration descriptor on
// endpoint zero.
//
// \param ui32Index is the index of the USB controller.
//
// \return None.
//
//*****************************************************************************
static void
USBDEP0StateTxConfig(uint32_t ui32Index)
{
    uint32_t ui32NumBytes, ui32SecBytes, ui32ToSend;
    uint8_t *pui8Data;
    tConfigDescriptor sConfDesc;
    const tConfigHeader *psConfig;
    const tConfigSection *psSection;

    ASSERT(ui32Index == 0);

    //
    // In the TX state on endpoint zero.
    //
    g_psUSBDevice[ui32Index].iEP0State = eUSBStateTxConfig;

    //
    // Find the current configuration descriptor definition.
    //
    psConfig = g_psUSBDevice[ui32Index].pvCBData->ppsConfigDescriptors[
               g_psUSBDevice[ui32Index].ui8ConfigIndex];

    //
    // Set the number of bytes to send this iteration.
    //
    ui32NumBytes = g_psUSBDevice[ui32Index].ui32EP0DataRemain;

    //
    // Limit individual transfers to 64 bytes.
    //
    if(ui32NumBytes > EP0_MAX_PACKET_SIZE)
    {
        ui32NumBytes = EP0_MAX_PACKET_SIZE;
    }

    //
    // If this is the first call, we need to fix up the total length of the
    // configuration descriptor.  This has already been determined and set in
    // g_sUSBDeviceState.ulEP0DataRemain.
    //
    if((g_psUSBDevice[ui32Index].ui16SectionOffset == 0) &&
       (g_psUSBDevice[ui32Index].ui8ConfigSection == 0))
    {
        //
        // Copy the USB configuration descriptor from the beginning of the
        // first section of the current configuration.
        //
        sConfDesc = *(tConfigDescriptor *)g_psUSBDevice[ui32Index].pui8EP0Data;

        //
        // Update the total size.
        //
        sConfDesc.wTotalLength = (uint16_t)USBDCDConfigDescGetSize(psConfig);

        //
        // Write the descriptor to the USB FIFO.
        //
        ui32ToSend = (ui32NumBytes < sizeof(tConfigDescriptor)) ? ui32NumBytes :
                        sizeof(tConfigDescriptor);
        USBEndpointDataPut(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0,(uint8_t *)&sConfDesc,
                           ui32ToSend);

        //
        // Did we reach the end of the first section?
        //
        if(psConfig->psSections[0]->ui16Size == ui32ToSend)
        {
            //
            // Update our tracking indices to point to the start of the next
            // section.
            //
            g_psUSBDevice[ui32Index].ui16SectionOffset = 0;
            g_psUSBDevice[ui32Index].ui8ConfigSection = 1;
        }
        else
        {
            //
            // Note that we have sent the first few bytes of the descriptor.
            //
            g_psUSBDevice[ui32Index].ui16SectionOffset = (uint8_t)ui32ToSend;
        }

        //
        // How many bytes do we have remaining to send on this iteration?
        //
        ui32ToSend = ui32NumBytes - ui32ToSend;
    }
    else
    {
        //
        // Set the number of bytes we still have to send on this call.
        //
        ui32ToSend = ui32NumBytes;
    }

    //
    // Add the relevant number of bytes to the USB FIFO
    //
    while(ui32ToSend)
    {
        //
        // Get a pointer to the current configuration section.
        //
        psSection = psConfig->psSections[g_psUSBDevice[ui32Index].ui8ConfigSection];

        //
        // Calculate bytes are available in the current configuration section.
        //
        ui32SecBytes = (uint32_t)(psSection->ui16Size -
                g_psUSBDevice[ui32Index].ui16SectionOffset);

        //
        // Save the pointer so that it can be passed to the
        // USBEndpointDataPut() function.
        //
        pui8Data = (uint8_t *)psSection->pui8Data +
                g_psUSBDevice[ui32Index].ui16SectionOffset;

        //
        // Are there more bytes in this section that we still have to send?
        //
        if(ui32SecBytes > ui32ToSend)
        {
            //
            // Yes - send only the remaining bytes in the transfer.
            //
            ui32SecBytes = ui32ToSend;
        }

        //
        // Put the data in the correct FIFO.
        //
        USBEndpointDataPut(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, pui8Data, ui32SecBytes);

        //
        // Fix up our pointers for the next iteration.
        //
        ui32ToSend -= ui32SecBytes;
        g_psUSBDevice[ui32Index].ui16SectionOffset += (uint8_t)ui32SecBytes;

        //
        // Have we reached the end of a section?
        //
        if(g_psUSBDevice[ui32Index].ui16SectionOffset == psSection->ui16Size)
        {
            //
            // Yes - move to the next one.
            //
            g_psUSBDevice[ui32Index].ui8ConfigSection++;
            g_psUSBDevice[ui32Index].ui16SectionOffset = 0;
        }
    }

    //
    // Fix up the number of bytes remaining to be sent and the start pointer.
    //
    g_psUSBDevice[ui32Index].ui32EP0DataRemain -= ui32NumBytes;

    //
    // If we ran out of bytes in the configuration section, bail and just
    // send out what we have.
    //
    if(psConfig->ui8NumSections <= g_psUSBDevice[ui32Index].ui8ConfigSection)
    {
        g_psUSBDevice[ui32Index].ui32EP0DataRemain = 0;
    }

    //
    // If there is no more data don't keep looking or ui8ConfigSection might
    // overrun the available space.
    //
    if(g_psUSBDevice[ui32Index].ui32EP0DataRemain != 0)
    {
        pui8Data =(uint8_t *)
            psConfig->psSections[g_psUSBDevice[ui32Index].ui8ConfigSection]->pui8Data;
        ui32ToSend = g_psUSBDevice[ui32Index].ui16SectionOffset;
        g_psUSBDevice[ui32Index].pui8EP0Data = (pui8Data + ui32ToSend);
    }

    //
    // If this is exactly 64 then don't set the last packet yet.
    //
    if(ui32NumBytes == EP0_MAX_PACKET_SIZE)
    {
        //
        // There is more data to send or exactly 64 bytes were sent, this
        // means that there is either more data coming or a null packet needs
        // to be sent to complete the transaction.
        //
        USBEndpointDataSend(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0, USB_TRANS_IN);
    }
    else
    {
        //
        // Send the last bit of data.
        //
        USBEndpointDataSend(g_USBInstance[ui32Index].uiBaseAddr, USB_EP_0,USB_TRANS_IN_LAST);

        //
        // If there is a sent callback then call it.
        //
        if((g_psUSBDevice[ui32Index].pvCBData->psCallbacks.pfnDataSent) &&
           (g_psUSBDevice[ui32Index].ui32OUTDataSize != 0))
        {
            //
            // Call the custom handler.
            //
            g_psUSBDevice[ui32Index].pvCBData->psCallbacks.pfnDataSent(
                g_psUSBDevice[ui32Index].pvInstance, g_psUSBDevice[ui32Index].ui32OUTDataSize, 
                                                                    ui32Index);

            //
            // There is no longer any data pending to be sent.
            //
            g_psUSBDevice[ui32Index].ui32OUTDataSize = 0;
        }

        //
        // Now go to the status state and wait for the transmit to complete.
        //
        g_psUSBDevice[ui32Index].iEP0State = eUSBStateStatus;
    }
}

//*****************************************************************************
//
// The internal USB device interrupt handler.
//
// \param ui32Index is the USB controller associated with this interrupt.
// \param ui32Status is the current interrupt status as read via a call to
// USBIntStatusControl().
//
// This function is called from either \e USB0DualModeIntHandler() or
// \e USB0DeviceIntHandler() to process USB interrupts when in device mode.
// This handler will branch the interrupt off to the appropriate application or
// stack handlers depending on the current status of the USB controller.
//
// The two-tiered structure for the interrupt handler ensures that it is
// possible to use the same handler code in both device and OTG modes and
// means that host code can be excluded from applications that only require
// support for USB device mode operation.
//
// \return None.
//
//*****************************************************************************
void
USBDeviceIntHandlerInternal(uint32_t ui32Index, uint32_t ui32Status, 
                                                    uint32_t *endPStatus)
{
    static uint32_t ui32SOFDivide = 0;
    tDeviceInfo *psInfo;
    void *pvInstance;
    uint32_t epStatus;
    uint32_t epnStatus = 0;
    
    //
    // Get the controller interrupt status from the wrapper registers
    // Only the lower 16bits contain EP intr data
    //
    if(endPStatus == NULL)
    {
        epStatus= 0xFFFF & ui32Status;
        ui32Status >>=16;
    }
    else
    {
        epStatus = *endPStatus;
    }
    
    ui32Status |= USBIntStatusControl(g_USBInstance[ui32Index].uiBaseAddr);    

    //
    // If device initialization has not been performed then just disconnect
    // from the USB bus and return from the handler.
    //
    if(g_psUSBDevice[ui32Index].pvCBData == 0)
    {
        USBDevDisconnect(g_USBInstance[ui32Index].uiBaseAddr);
        return;
    }

    psInfo = g_psUSBDevice[ui32Index].pvCBData;
    pvInstance = g_psUSBDevice[ui32Index].pvInstance;

    //
    // Received a reset from the host.
    //
    if(ui32Status & USB_INTCTRL_RESET)
    {
        USBDeviceEnumResetHandler(&g_psUSBDevice[ui32Index]);
    }

    //
    // Suspend was signaled on the bus.
    //
    if(ui32Status & USB_INTCTRL_SUSPEND)
    {
        //
        // Call the SuspendHandler() if it was specified.
        //
        if(psInfo->psCallbacks.pfnSuspendHandler)
        {
            psInfo->psCallbacks.pfnSuspendHandler(pvInstance);
        }
    }

    //
    // Resume was signaled on the bus.
    //
    if(ui32Status & USB_INTCTRL_RESUME)
    {
        //
        // Call the ResumeHandler() if it was specified.
        //
        if(psInfo->psCallbacks.pfnResumeHandler)
        {
            psInfo->psCallbacks.pfnResumeHandler(pvInstance);
        }
    }

    //
    // USB device was disconnected.
    //
    if(ui32Status & USB_INTCTRL_DISCONNECT)
    {
        //
        // Call the DisconnectHandler() if it was specified.
        //
        if(psInfo->psCallbacks.pfnDisconnectHandler)
        {
            psInfo->psCallbacks.pfnDisconnectHandler(pvInstance);
        }
    }

    //
    // Start of Frame was received.
    //
    if(ui32Status & USB_INTCTRL_SOF)
    {
        //
        // Increment the global Start of Frame counter.
        //
        g_ui32USBSOFCount++;

        //
        // Increment our SOF divider.
        //
        ui32SOFDivide++;

        //
        // Handle resume signaling if required.
        //
        USBDeviceResumeTickHandler(ui32Index);

        //
        // Have we counted enough SOFs to allow us to call the tick function?
        //
        if(ui32SOFDivide == USB_SOF_TICK_DIVIDE)
        {
            //
            // Yes - reset the divider and call the SOF tick handler.
            //
            ui32SOFDivide = 0;
            InternalUSBStartOfFrameTick(USB_SOF_TICK_DIVIDE, ui32Index);
        }
    }

    //
    // Handle end point 0 interrupts.
    //
    if(epStatus & USB_INTEP_0)
    {
        USBDeviceEnumHandler(&g_psUSBDevice[ui32Index], ui32Index);
        epStatus &= ~USB_INTEP_0;
    }

    /*
    converting the epstatus(Wrapper register data) to ui32Status( MUSB register data)
    */

    if(endPStatus ==  NULL)
    {
        epnStatus = 0xFF & epStatus;
        epnStatus = epnStatus | ((0xFF00 & epStatus)<<8);
    }
    else
    {
        epnStatus = epStatus;
    }
    //
    // Because there is no way to detect if a uDMA interrupt has occurred,
    // check for an endpoint callback and call it if it is available.
    //
    if((psInfo->psCallbacks.pfnEndpointHandler) &&
       ((epStatus != 0)))
    {
        psInfo->psCallbacks.pfnEndpointHandler(pvInstance, epnStatus, ui32Index);
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
