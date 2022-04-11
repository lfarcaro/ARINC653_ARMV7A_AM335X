#ifndef __USBDHIDJOYSTICK_H__
#define __USBDHIDJOYSTICK_H__

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// PRIVATE
//
// The first few sections of this header are private defines that are used by
// the USB HID joystick code and are here only to help with the application
// allocating the correct amount of memory for the HID joystick device code.
//*****************************************************************************

//*****************************************************************************
// PRIVATE
//
// The joystick report sent to the host.
//*****************************************************************************
typedef struct
{

    // Buttons state
    unsigned short ucButtons;

    // X axis
    unsigned short ucX;

    // Y axis
    unsigned short ucY;

    // Z axis
    unsigned short ucZ;

    // RX axis
    unsigned short ucRX;

    // RY axis
    unsigned short ucRY;

    // RZ axis
    unsigned short ucRZ;
}
tHIDJoystickReport;

//*****************************************************************************
// PRIVATE
//
// This enumeration holds the various states that the joystick can be in during
// normal operation.
//*****************************************************************************
typedef enum
{
    // Unconfigured.
    HID_JOYSTICK_STATE_UNCONFIGURED,

    // No keys to send and not waiting on data.
    HID_JOYSTICK_STATE_IDLE,

    // Waiting on report data from the host.
    HID_JOYSTICK_STATE_WAIT_DATA,

    // Waiting on data to be sent out.
    HID_JOYSTICK_STATE_SEND
}
tJoystickState;

//*****************************************************************************
// PRIVATE
//
// This structure provides the private instance data structure for the USB
// HID Joystick device.  This structure forms the RAM workspace used by each
// instance of the joystick.
//*****************************************************************************
typedef struct
{
    // The USB configuration number set by the host or 0 of the device is
    // currently unconfigured.
    unsigned char ucUSBConfigured;

    // The protocol requested by the host, USB_HID_PROTOCOL_BOOT or
    // USB_HID_PROTOCOL_REPORT.
    unsigned char ucProtocol;

    // The last report sent to the host.
    tHIDJoystickReport sReport;

    // The current state of the joystick interrupt IN endpoint.
    volatile tJoystickState eJoystickState;

    // The idle timeout control structure for our input report.  This is
    // required by the lower level HID driver.
    tHIDReportIdle sReportIdle;

    // The lower level HID driver's instance data.
    tHIDInstance sHIDInstance;

    // This is needed for the lower level HID driver.
    tUSBDHIDDevice sHIDDevice;
}
tHIDJoystickInstance;

#ifndef DEPRECATED
//*****************************************************************************
// The number of bytes of workspace required by the HID joystick driver.
// The client must provide a block of RAM of at least this size in the
// tHIDJoystickInstance field of the tUSBHIDJoystickDevice structure passed on
// USBDHIDJoystickInit().  The HID joystick driver needs space for the generic HID
// interface + the Joystick Report Buffer + HID joystick interface.
//
// This value is deprecated and should not be used, any new code should just
// pass in a tHIDJoystickInstance structure in the psPrivateHIDJoystickData field.
//*****************************************************************************
#define USB_HID_JOYSTICK_WORKSPACE_SIZE (sizeof(tHIDJoystickInstance))
#endif

//*****************************************************************************
//! This structure is used by the application to define operating parameters
//! for the HID joystick device.
//*****************************************************************************
typedef struct
{
    //! The vendor ID that this device is to present in the device descriptor.
    unsigned short usVID;

    //! The product ID that this device is to present in the device descriptor.
    unsigned short usPID;

    //! The maximum power consumption of the device, expressed in milliamps.
    unsigned short usMaxPowermA;

    //! Indicates whether the device is self- or bus-powered and whether or not
    //! it supports remote wakeup.  Valid values are USB_CONF_ATTR_SELF_PWR or
    //! USB_CONF_ATTR_BUS_PWR, optionally ORed with USB_CONF_ATTR_RWAKE.
    unsigned char ucPwrAttributes;

    //! A pointer to the callback function which will be called to notify
    //! the application of events relating to the operation of the joystick.
    tUSBCallback pfnCallback;

    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the joystick callback, pfnCallback.
    void *pvCBData;

    //! A pointer to the string descriptor array for this device.  This array
    //! must contain the following string descriptor pointers in this order.
    //! Language descriptor, Manufacturer name string (language 1), Product
    //! name string (language 1), Serial number string (language 1),HID
    //! Interface description string (language 1), Configuration description
    //! string (language 1).
    //!
    //! If supporting more than 1 language, the descriptor block (except for
    //! string descriptor 0) must be repeated for each language defined in the
    //! language descriptor.
    const unsigned char * const *ppStringDescriptors;

    //! The number of descriptors provided in the ppStringDescriptors
    //! array.  This must be (1 + (5 * (num languages))).
    unsigned int ulNumStringDescriptors;

    //! A pointer to private instance data for this device.  This memory must
    //! remain accessible for as long as the joystick device is in use and must
    //! not be modified by any code outside the HID joystick driver.
    tHIDJoystickInstance *psPrivateHIDJoystickData;
}
tUSBDHIDJoystickDevice;

//*****************************************************************************
//! This return code from USBDHIDJoystickStateChange indicates success.
//*****************************************************************************
#define JOYSTICK_SUCCESS           0

//*****************************************************************************
//! This return code from USBDHIDJoystickStateChange indicates that an error was
//! reported while attempting to send a report to the host.  A client should
//! assume that the host has disconnected if this return code is seen.
//*****************************************************************************
#define JOYSTICK_ERR_TX_ERROR      2

//*****************************************************************************
//! USBDHIDJoystickStateChange returns this value if it is called before the
//! USB host has connected and configured the device.  All joystick state
//! information passed on the call will have been ignored.
//*****************************************************************************
#define JOYSTICK_ERR_NOT_CONFIGURED 4

//*****************************************************************************
//! Value for the ucButtons parameter to USBDHIDJoystickStateChange indicating
//@ to the USB host that no buttons on the joystick are pressed.
//*****************************************************************************
#define JOYSTICK_REPORT_BUTTON_NONE   0x0000

//*****************************************************************************
//! Setting these bits in the ucButtons parameter to USBDHIDJoystickStateChange
//! indicates to the USB host that button X on the joystick is pressed.
//*****************************************************************************
#define JOYSTICK_REPORT_BUTTON_01 (0x0001 << 0)
#define JOYSTICK_REPORT_BUTTON_02 (0x0001 << 1)
#define JOYSTICK_REPORT_BUTTON_03 (0x0001 << 2)
#define JOYSTICK_REPORT_BUTTON_04 (0x0001 << 3)
#define JOYSTICK_REPORT_BUTTON_05 (0x0001 << 4)
#define JOYSTICK_REPORT_BUTTON_06 (0x0001 << 5)
#define JOYSTICK_REPORT_BUTTON_07 (0x0001 << 6)
#define JOYSTICK_REPORT_BUTTON_08 (0x0001 << 7)
#define JOYSTICK_REPORT_BUTTON_09 (0x0001 << 8)
#define JOYSTICK_REPORT_BUTTON_10 (0x0001 << 9)
#define JOYSTICK_REPORT_BUTTON_11 (0x0001 << 10)
#define JOYSTICK_REPORT_BUTTON_12 (0x0001 << 11)
#define JOYSTICK_REPORT_BUTTON_13 (0x0001 << 12)
#define JOYSTICK_REPORT_BUTTON_14 (0x0001 << 13)
#define JOYSTICK_REPORT_BUTTON_15 (0x0001 << 14)
#define JOYSTICK_REPORT_BUTTON_16 (0x0001 << 15)

//*****************************************************************************
// API Function Prototypes
//*****************************************************************************
extern void *USBDHIDJoystickInit(unsigned int ulIndex, const tUSBDHIDJoystickDevice *psDevice);
extern void *USBDHIDJoystickCompositeInit(unsigned int ulIndex, const tUSBDHIDJoystickDevice *psDevice);
extern void USBDHIDJoystickTerm(void *pvInstance);
extern void *USBDHIDJoystickSetCBData(void *pvInstance, void *pvCBData);
extern unsigned int USBDHIDJoystickStateChange(void *pvInstance, unsigned short uButtons, unsigned short uX, unsigned short uY, unsigned short uZ, unsigned short uRX, unsigned short uRY, unsigned short uRZ);
extern void USBDHIDJoystickPowerStatusSet(void *pvInstance, unsigned char ucPower);
extern tBoolean USBDHIDJoystickRemoteWakeupRequest(void *pvInstance);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBDHIDJOYSTICK_H__
