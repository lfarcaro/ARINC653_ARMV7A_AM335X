#include <stdint.h>
#include "hw_types.h"
#include "debug.h"
#include "usb.h"
#include "usblib.h"
#include "usbdevice.h"
#include "usbhid.h"
#include "usbdhid.h"
#include "usbdhidjoystick.h"

// The report descriptor for the joystick class device.
static const unsigned char g_pucJoystickReportDescriptor[] =
{
    UsagePage(USB_HID_GENERIC_DESKTOP),
    Usage(USB_HID_GAME_PAD),
    Collection(USB_HID_APPLICATION),
        Collection(USB_HID_PHYSICAL),

            // Buttons
            UsagePage(USB_HID_BUTTONS),
            UsageMinimum(1),
            UsageMaximum(16),
            LogicalMinimum(0),
            LogicalMaximum(1),
            ReportCount(16),
            ReportSize(1),
            Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),

            //  X, Y, Z, RX, RY and RZ axis
            UsagePage(USB_HID_GENERIC_DESKTOP),
            Usage(USB_HID_X),
            Usage(USB_HID_Y),
            Usage(USB_HID_Z),
            Usage(USB_HID_RX),
            Usage(USB_HID_RY),
            Usage(USB_HID_RZ),
            LogicalMinimum(-32768),
            LogicalMaximum(+32767),
            ReportSize(16),
            ReportCount(6),
            Input(USB_HID_INPUT_DATA | USB_HID_INPUT_VARIABLE | USB_HID_INPUT_ABS),

        EndCollection,
    EndCollection,
};

// The HID class descriptor table.
static const unsigned char * const g_pJoystickClassDescriptors[] =
{
    g_pucJoystickReportDescriptor
};

// The HID descriptor for the joystick device.
static const tHIDDescriptor g_sJoystickHIDDescriptor =
{
    sizeof(tHIDDescriptor),            // bLength
    USB_HID_DTYPE_HID,                 // bDescriptorType
    0x111,                             // bcdHID (version 1.11 compliant)
    0,                                 // bCountryCode (not localized)
    1,                                 // bNumDescriptors
    {
        {
            USB_HID_DTYPE_REPORT,                  // Report descriptor
            sizeof(g_pucJoystickReportDescriptor)  // Size of report descriptor
        }
    }
};

// Forward references for joystick device callback functions.
static unsigned int HIDJoystickRxHandler(void *pvCBData, unsigned int ulEvent, unsigned int ulMsgData, void *pvMsgData);
static unsigned int HIDJoystickTxHandler(void *pvCBData, unsigned int ulEvent, unsigned int ulMsgData, void *pvMsgData);

//*****************************************************************************
// Main HID device class event handler function.
//
// \param pvCBData is the event callback pointer provided during USBDHIDInit().
// This is a pointer to our HID device structure (&g_sHIDJoystickDevice).
// \param ulEvent identifies the event we are being called back for.
// \param ulMsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID device class driver to inform the
// application of particular asynchronous events related to operation of the
// joystick HID device.
//
// \return Returns a value which is event-specific.
//*****************************************************************************
static unsigned int HIDJoystickRxHandler(void *pvCBData, unsigned int ulEvent, unsigned int ulMsgData, void *pvMsgData)
{
    tHIDJoystickInstance *psInst;
    tUSBDHIDJoystickDevice *psDevice;

    // Make sure we didn't get a NULL pointer.
    ASSERT(pvCBData);

    // Get a pointer to our instance data
    psDevice = (tUSBDHIDJoystickDevice *)pvCBData;
    psInst = psDevice->psPrivateHIDJoystickData;

    // Which event were we sent?
    switch (ulEvent)
    {
        // The host has connected to us and configured the device.
        case USB_EVENT_CONNECTED:
        {
            psInst->ucUSBConfigured = true;

            // Pass the information on to the client.
            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_CONNECTED, 0, (void *)0);

            break;
        }

        // The host has disconnected from us.
        case USB_EVENT_DISCONNECTED:
        {
            psInst->ucUSBConfigured = false;

            // Pass the information on to the client.
            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_DISCONNECTED, 0, (void *)0);

            break;
        }

        // The host is polling us for a particular report and the HID driver
        // is asking for the latest version to transmit.
        case USBD_HID_EVENT_IDLE_TIMEOUT:
        case USBD_HID_EVENT_GET_REPORT:
        {
            // We only support a single input report so we don't need to check
            // the ulMsgValue parameter in this case.  Set the report pointer
            // in *pvMsgData and return the length of the report in bytes.
            *(unsigned char **)pvMsgData = (unsigned char *)&psInst->sReport;
            return (sizeof(tHIDJoystickReport));
        }

        // The device class driver has completed sending a report to the
        // host in response to a Get_Report request.
        case USBD_HID_EVENT_REPORT_SENT:
        {
            // We have nothing to do here.
            break;
        }

        // This event is sent in response to a host Set_Report request.  The
        // joystick device has no output reports so we return a NULL pointer and
        // zero length to cause this request to be stalled.
        case USBD_HID_EVENT_GET_REPORT_BUFFER:
        {
            // We are being asked for a report that does not exist for
            // this device.
            *(unsigned char **)pvMsgData = (void *)0;
            return (0);
        }

        // The host is asking us to set either boot or report protocol (not
        // that it makes any difference to this particular joystick).
        case USBD_HID_EVENT_SET_PROTOCOL:
        {
            psInst->ucProtocol = ulMsgData;
            break;
        }

        // The host is asking us to tell it which protocol we are currently
        // using, boot or request.
        case USBD_HID_EVENT_GET_PROTOCOL:
        {
            return (psInst->ucProtocol);
        }

        // Pass ERROR, SUSPEND and RESUME to the client unchanged.
        case USB_EVENT_ERROR:
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
        {
            return(psDevice->pfnCallback(psDevice->pvCBData, ulEvent, ulMsgData, pvMsgData));
        }

        // We ignore all other events.
        default:
        {
            break;
        }
    }
    return (0);
}

//*****************************************************************************
// HID device class transmit channel event handler function.
//
// \param pvCBData is the event callback pointer provided during USBDHIDInit().
// This is a pointer to our HID device structure (&g_sHIDJoystickDevice).
// \param ulEvent identifies the event we are being called back for.
// \param ulMsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID device class driver to inform the
// application of particular asynchronous events related to report
// transmissions made using the interrupt IN endpoint.
//
// \return Returns a value which is event-specific.
//*****************************************************************************
static unsigned int HIDJoystickTxHandler(void *pvCBData, unsigned int ulEvent, unsigned int ulMsgData, void *pvMsgData)
{
    tHIDJoystickInstance *psInst;
    tUSBDHIDJoystickDevice *psDevice;

    // Make sure we didn't get a NULL pointer.
    ASSERT(pvCBData);

    // Get a pointer to our instance data
    psDevice = (tUSBDHIDJoystickDevice *)pvCBData;
    psInst = psDevice->psPrivateHIDJoystickData;

    // Which event were we sent?
    switch (ulEvent)
    {
        // A report transmitted via the interrupt IN endpoint was acknowledged
        // by the host.
        case USB_EVENT_TX_COMPLETE:
        {
            // Our last transmission is complete.
            psInst->eJoystickState = HID_JOYSTICK_STATE_IDLE;

            // Pass the event on to the client.
            psDevice->pfnCallback(psDevice->pvCBData, USB_EVENT_TX_COMPLETE, ulMsgData, (void *)0);

            break;
        }

        // We ignore all other events related to transmission of reports via
        // the interrupt IN endpoint.
        default:
        {
            break;
        }
    }

    return (0);
}

//*****************************************************************************
// Initializes HID joystick device operation for a given USB controller.
//
// \param ulIndex is the index of the USB controller which is to be
// initialized for HID joystick device operation.
// \param psDevice points to a structure containing parameters customizing
// the operation of the HID joystick device.
//
// An application wishing to offer a USB HID joystick interface to a USB host
// must call this function to initialize the USB controller and attach the
// joystick device to the USB bus.  This function performs all required USB
// initialization.
//
// On successful completion, this function will return the \e psDevice pointer
// passed to it.  This must be passed on all future calls to the HID joystick
// device driver.
//
// When a host connects and configures the device, the application callback
// will receive \b USB_EVENT_CONNECTED after which calls can be made to
// USBDHIDJoystickStateChange() to report pointer movement and button presses
// to the host.
//
// \note The application must not make any calls to the lower level USB device
// interfaces if interacting with USB via the USB HID joystick device API.
// Doing so will cause unpredictable (though almost certainly unpleasant)
// behavior.
//
// \return Returns NULL on failure or the psDevice pointer on success.
//*****************************************************************************
void * USBDHIDJoystickInit(unsigned int ulIndex, const tUSBDHIDJoystickDevice *psDevice)
{
    void *pvRetcode;
    tUSBDHIDDevice *psHIDDevice;

    // Check parameter validity.
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDJoystickData);
    ASSERT(psDevice->pfnCallback);

    // Get a pointer to the HID device data.
    psHIDDevice = &psDevice->psPrivateHIDJoystickData->sHIDDevice;

    // Call the common initialization routine.
    pvRetcode = USBDHIDJoystickCompositeInit(ulIndex, psDevice);

    // If we initialized the HID layer successfully, pass our device pointer
    // back as the return code, otherwise return NULL to indicate an error.
    if(pvRetcode)
    {
        // Initialize the lower layer HID driver
        pvRetcode = USBDHIDInit(ulIndex, psHIDDevice);

        return((void *)psDevice);
    }
    else
    {
        return((void *)0);
    }
}

//*****************************************************************************
//
// Initializes HID joystick device operation for a given USB controller.
//
// \param ulIndex is the index of the USB controller which is to be
// initialized for HID joystick device operation.
// \param psDevice points to a structure containing parameters customizing
// the operation of the HID joystick device.
//
// An application wishing to make use of a composite
// USB bulk communication channel needs to call this function.
// This function is used for initializing an instance related information of the
// HID joystick device.
//
// \return Returns zero on failure or a non-zero instance value that should be
// used with the remaining USB HID Joystick APIs.
//
//*****************************************************************************
void * USBDHIDJoystickCompositeInit(unsigned int ulIndex, const tUSBDHIDJoystickDevice *psDevice)
{
    tHIDJoystickInstance *psInst;
    tUSBDHIDDevice *psHIDDevice;

    // Check parameter validity.
    ASSERT(psDevice);
    ASSERT(psDevice->ppStringDescriptors);
    ASSERT(psDevice->psPrivateHIDJoystickData);
    ASSERT(psDevice->pfnCallback);

    // Get a pointer to our instance data
    psInst = psDevice->psPrivateHIDJoystickData;

    // Get a pointer to the HID device data.
    psHIDDevice = &psDevice->psPrivateHIDJoystickData->sHIDDevice;

    // Initialize the various fields in our instance structure.
    psInst->ucUSBConfigured = 0;
    psInst->ucProtocol = USB_HID_PROTOCOL_REPORT;
    psInst->sReport.ucButtons = JOYSTICK_REPORT_BUTTON_NONE;
    psInst->sReport.ucX = 0;
    psInst->sReport.ucY = 0;
    psInst->sReport.ucZ = 0;
    psInst->sReport.ucRX = 0;
    psInst->sReport.ucRY = 0;
    psInst->sReport.ucRZ = 0;
    psInst->eJoystickState = HID_JOYSTICK_STATE_UNCONFIGURED;
    psInst->sReportIdle.ucDuration4mS = 0;
    psInst->sReportIdle.ucReportID = 0;
    psInst->sReportIdle.ulTimeSinceReportmS = 0;
    psInst->sReportIdle.usTimeTillNextmS = 0;

    // Initialize the HID device class instance structure based on input from the caller.
    psHIDDevice->usPID = psDevice->usPID;
    psHIDDevice->usVID = psDevice->usVID;
    psHIDDevice->usMaxPowermA = psDevice->usMaxPowermA;
    psHIDDevice->ucPwrAttributes = psDevice->ucPwrAttributes;
    psHIDDevice->ucSubclass = USB_HID_SCLASS_NONE;
    psHIDDevice->ucProtocol = USB_HID_PROTOCOL_NONE;
    psHIDDevice->ucNumInputReports = 1;
    psHIDDevice->psReportIdle = &psInst->sReportIdle;
    psHIDDevice->pfnRxCallback = HIDJoystickRxHandler;
    psHIDDevice->pvRxCBData = (void *)psDevice;
    psHIDDevice->pfnTxCallback = HIDJoystickTxHandler;
    psHIDDevice->pvTxCBData = (void *)psDevice;
    psHIDDevice->bUseOutEndpoint = false;
    psHIDDevice->psHIDDescriptor = &g_sJoystickHIDDescriptor;
    psHIDDevice->ppClassDescriptors= g_pJoystickClassDescriptors;
    psHIDDevice->ppStringDescriptors = psDevice->ppStringDescriptors;
    psHIDDevice->ulNumStringDescriptors = psDevice->ulNumStringDescriptors;
    psHIDDevice->psPrivateHIDData = &psInst->sHIDInstance;

    // Initialize the lower layer HID driver and pass it the various structures
    // and descriptors necessary to declare that we are a keyboard.
    return(USBDHIDCompositeInit(ulIndex, psHIDDevice));
}

//*****************************************************************************
// Shuts down the HID joystick device.
//
// \param pvInstance is the pointer to the device instance structure.
//
// This function terminates HID joystick operation for the instance supplied
// and removes the device from the USB bus.  Following this call, the \e
// pvInstance instance may not me used in any other call to the HID joystick
// device other than USBDHIDJoystickInit().
//
// \return None.
//*****************************************************************************
void USBDHIDJoystickTerm(void *pvInstance)
{
    tUSBDHIDJoystickDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    // Get a pointer to the device.
    psDevice = (tUSBDHIDJoystickDevice *)pvInstance;

    // Get a pointer to the HID device data.
    psHIDDevice = &psDevice->psPrivateHIDJoystickData->sHIDDevice;

    // Mark our device as no longer configured.
    psDevice->psPrivateHIDJoystickData->ucUSBConfigured = 0;

    // Terminate the low level HID driver.
    USBDHIDTerm(psHIDDevice);
}

//*****************************************************************************
// Sets the client-specific pointer parameter for the joystick callback.
//
// \param pvInstance is the pointer to the joystick device instance structure.
// \param pvCBData is the pointer that client wishes to be provided on each
// event sent to the joystick callback function.
//
// The client uses this function to change the callback pointer passed in
// the first parameter on all callbacks to the \e pfnCallback function
// passed on USBDHIDJoystickInit().
//
// If a client wants to make runtime changes in the callback pointer, it must
// ensure that the pvInstance structure passed to USBDHIDJoystickInit() resides
// in RAM.  If this structure is in flash, callback data changes will not be
// possible.
//
// \return Returns the previous callback pointer that was set for this
// instance.
//*****************************************************************************
void * USBDHIDJoystickSetCBData(void *pvInstance, void *pvCBData)
{
    void *pvOldCBData;
    tUSBDHIDJoystickDevice *psJoystick;

    // Check for a NULL pointer in the device parameter.
    ASSERT(pvInstance);

    // Get a pointer to our joystick device.
    psJoystick = (tUSBDHIDJoystickDevice *)pvInstance;

    // Save the old callback pointer and replace it with the new value.
    pvOldCBData = psJoystick->pvCBData;
    psJoystick->pvCBData = pvCBData;

    // Pass the old callback pointer back to the caller.
    return(pvOldCBData);
}

//*****************************************************************************
// Reports a joystick state change to the USB host.
//
// \param pvInstance is the pointer to the joystick device instance structure.
// \param cAbsoluteX is the absolute value for X axis
// \param cAbsoluteY is the absolute value for Y axis
// \param cAbsoluteZ is the absolute value for Z axis
// \param cAbsoluteRX is the absolute value for RX axis
// \param cAbsoluteRY is the absolute value for RY axis
// \param cAbsoluteRZ is the absolute value for RZ axis
// \param ucButtons is a bit mask indicating the state which (if any) of the three
// joystick buttons is pressed. Valid values are logical OR combinations of
// \e JOYSTICK_REPORT_BUTTON_1, \e JOYSTICK_REPORT_BUTTON_2 ...
//
// This function is called to report changes in the joystick state to the USB
// host.  These changes can be movement of the pointer, reported relative to
// its previous position, or changes in the states of up to 3 buttons that
// the joystick may support.  The return code indicates whether or not the
// joystick report could be sent to the host.  In cases where a previous
// report is still being transmitted, \b JOYSTICK_ERR_TX_ERROR will be returned
// and the state change will be ignored.
//
// \return Returns \b JOYSTICK_SUCCESS on success, \b JOYSTICK_ERR_TX_ERROR if an
// error occurred while attempting to schedule transmission of the joystick
// report to the host (typically due to a previous report which has not yet
// completed transmission or due to disconnection of the host) or \b
// JOYSTICK_ERR_NOT_CONFIGURED if called before a host has connected to and
// configured the device.
//*****************************************************************************
unsigned int USBDHIDJoystickStateChange(void *pvInstance, unsigned short uButtons, unsigned short uX, unsigned short uY, unsigned short uZ, unsigned short uRX, unsigned short uRY, unsigned short uRZ)
{
    unsigned int ulRetcode;
    unsigned int ulCount;
    tHIDJoystickInstance *psInst;
    tUSBDHIDJoystickDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    // Get a pointer to the device.
    psDevice = (tUSBDHIDJoystickDevice *)pvInstance;

    // Get a pointer to the HID device data.
    psHIDDevice = &psDevice->psPrivateHIDJoystickData->sHIDDevice;

    // Get a pointer to our instance data
    psInst = psDevice->psPrivateHIDJoystickData;

    // Update the global joystick report with the information passed.
    psInst->sReport.ucButtons = uButtons;
    psInst->sReport.ucX = uX;
    psInst->sReport.ucY = uY;
    psInst->sReport.ucZ = uZ;
    psInst->sReport.ucRX = uRX;
    psInst->sReport.ucRY = uRY;
    psInst->sReport.ucRZ = uRZ;

    // If we are not configured, return an error here before trying to send anything.
    if(!psInst->ucUSBConfigured)
    {
        return(JOYSTICK_ERR_NOT_CONFIGURED);
    }

    // Only send a report if the transmitter is currently free.
    if(USBDHIDTxPacketAvailable((void *)psHIDDevice))
    {
        // Send the report to the host.
        psInst->eJoystickState = HID_JOYSTICK_STATE_SEND;
        ulCount = USBDHIDReportWrite((void *)psHIDDevice, (unsigned char *)&psInst->sReport, sizeof(tHIDJoystickReport), true);

        // Did we schedule a packet for transmission correctly?
        if(!ulCount)
        {
            // No - report the error to the caller.
            ulRetcode = JOYSTICK_ERR_TX_ERROR;
        }
        else
        {
            ulRetcode = JOYSTICK_SUCCESS;
        }
    }
    else
    {
        ulRetcode = JOYSTICK_ERR_TX_ERROR;
    }
    // Return the relevant error code to the caller.
    return(ulRetcode);
}

//*****************************************************************************
// Reports the device power status (bus- or self-powered) to the USB library.
//
// \param pvInstance is the pointer to the joystick device instance structure.
// \param ucPower indicates the current power status, either \b
// USB_STATUS_SELF_PWR or \b USB_STATUS_BUS_PWR.
//
// Applications which support switching between bus- or self-powered
// operation should call this function whenever the power source changes
// to indicate the current power status to the USB library.  This information
// is required by the USB library to allow correct responses to be provided
// when the host requests status from the device.
//
// \return None.
//*****************************************************************************
void USBDHIDJoystickPowerStatusSet(void *pvInstance, unsigned char ucPower)
{
    tUSBDHIDJoystickDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    // Get the keyboard device pointer.
    psDevice = (tUSBDHIDJoystickDevice *)pvInstance;

    // Get a pointer to the HID device data.
    psHIDDevice = &psDevice->psPrivateHIDJoystickData->sHIDDevice;

    // Pass the request through to the lower layer.
    USBDHIDPowerStatusSet((void *)psHIDDevice, ucPower);
}

//*****************************************************************************
//
// Requests a remote wake up to resume communication when in suspended state.
//
// \param pvInstance is the pointer to the joystick device instance structure.
//
// When the bus is suspended, an application which supports remote wake up
// (advertised to the host via the configuration descriptor) may call this
// function to initiate remote wake up signaling to the host.  If the remote
// wake up feature has not been disabled by the host, this will cause the bus
// to resume operation within 20mS.  If the host has disabled remote wake up,
// \b false will be returned to indicate that the wake up request was not
// successful.
//
// \return Returns \b true if the remote wake up is not disabled and the
// signaling was started or \b false if remote wake up is disabled or if
// signaling is currently ongoing following a previous call to this function.
//
//*****************************************************************************
tBoolean USBDHIDJoystickRemoteWakeupRequest(void *pvInstance)
{
    tUSBDHIDJoystickDevice *psDevice;
    tUSBDHIDDevice *psHIDDevice;

    ASSERT(pvInstance);

    // Get the keyboard device pointer.
    psDevice = (tUSBDHIDJoystickDevice *)pvInstance;

    // Get a pointer to the HID device data.
    psHIDDevice = &psDevice->psPrivateHIDJoystickData->sHIDDevice;

    // Pass the request through to the lower layer.
    return(USBDHIDRemoteWakeupRequest((void *)&psHIDDevice));
}
