// Partition include
//#include "systempartition_usb.h"

// SNIPPET_START GLOBAL_INCLUDES
// ARINC653 includes
#include "arinc653_core.h"
// SNIPPET_END

// -------------------- PARTITION DESCRIPTION START --------------------
// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_DESCRIPTION
// SNIPPET_END
// -------------------- PARTITION DESCRIPTION END --------------------

// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_INCLUDES
// SNIPPET_END

// USB_INTERRUPT process identifier
static PROCESS_ID_TYPE USB_INTERRUPT_PROCESS_ID;

// USB process identifier
static PROCESS_ID_TYPE USB_PROCESS_ID;

// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_GLOBAL_VARIABLES

// GPIO definitions
#define GPIO_BUTTON01 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 4) & 1) ^ 1)
#define GPIO_BUTTON02 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 0) & 1) ^ 1)
#define GPIO_BUTTON03 (((HWREG(SOC_GPIO_2_REGS + GPIO_DATAIN) >> 22) & 1) ^ 1)
#define GPIO_BUTTON04 (((HWREG(SOC_GPIO_2_REGS + GPIO_DATAIN) >> 23) & 1) ^ 1)
#define GPIO_BUTTON05 (((HWREG(SOC_GPIO_2_REGS + GPIO_DATAIN) >> 25) & 1) ^ 1)
#define GPIO_BUTTON06 (((HWREG(SOC_GPIO_2_REGS + GPIO_DATAIN) >> 24) & 1) ^ 1)
#define GPIO_BUTTON07 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 29) & 1) ^ 1)
#define GPIO_BUTTON08 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 1) & 1) ^ 1)
#define GPIO_BUTTON09 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 5) & 1) ^ 1)
#define GPIO_BUTTON10 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 31) & 1) ^ 1)
#define GPIO_BUTTON11 (((HWREG(SOC_GPIO_2_REGS + GPIO_DATAIN) >> 1) & 1) ^ 1)
#define GPIO_BUTTON12 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 14) & 1) ^ 1)
#define GPIO_BUTTON13 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 15) & 1) ^ 1)
#define GPIO_BUTTON14 (((HWREG(SOC_GPIO_0_REGS + GPIO_DATAIN) >> 27) & 1) ^ 1)
#define GPIO_BUTTON15 (((HWREG(SOC_GPIO_0_REGS + GPIO_DATAIN) >> 22) & 1) ^ 1)
#define GPIO_BUTTON16 (((HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) >> 30) & 1) ^ 1)

// Macro for defining USB strings
#define USB_CHAR(c) c, 0

// Languages supported by the device
const unsigned char g_pLangDescriptor[] = {
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

// Manufacturer string
const unsigned char g_pManufacturerString[] = {
    (20 + 1) * 2,
    USB_DTYPE_STRING,
    USB_CHAR('A'),
    USB_CHAR('r'),
    USB_CHAR('c'),
    USB_CHAR('a'),
    USB_CHAR('r'),
    USB_CHAR('o'),
    USB_CHAR(' '),
    USB_CHAR('I'),
    USB_CHAR('n'),
    USB_CHAR('c'),
    USB_CHAR('o'),
    USB_CHAR('r'),
    USB_CHAR('p'),
    USB_CHAR('o'),
    USB_CHAR('r'),
    USB_CHAR('a'),
    USB_CHAR('t'),
    USB_CHAR('i'),
    USB_CHAR('o'),
    USB_CHAR('n'),
};

// Product string
const unsigned char g_pProductString[] = {
    2 + (18 * 2),
    USB_DTYPE_STRING,
    USB_CHAR('M'),
    USB_CHAR('y'),
    USB_CHAR(' '),
    USB_CHAR('S'),
    USB_CHAR('i'),
    USB_CHAR('m'),
    USB_CHAR('p'),
    USB_CHAR('l'),
    USB_CHAR('e'),
    USB_CHAR(' '),
    USB_CHAR('J'),
    USB_CHAR('o'),
    USB_CHAR('y'),
    USB_CHAR('s'),
    USB_CHAR('t'),
    USB_CHAR('i'),
    USB_CHAR('c'),
    USB_CHAR('k'),
};

// Serial number string
const unsigned char g_pSerialNumberString[] = {
    2 + (8 * 2),
    USB_DTYPE_STRING,
    USB_CHAR('1'),
    USB_CHAR('2'),
    USB_CHAR('3'),
    USB_CHAR('4'),
    USB_CHAR('5'),
    USB_CHAR('6'),
    USB_CHAR('7'),
    USB_CHAR('8'),
};

// HID interface description string
const unsigned char g_pHIDInterfaceDescriptionString[] = {
    2 + (15 * 2),
    USB_DTYPE_STRING,
    USB_CHAR('S'),
    USB_CHAR('i'),
    USB_CHAR('m'),
    USB_CHAR('p'),
    USB_CHAR('l'),
    USB_CHAR('e'),
    USB_CHAR(' '),
    USB_CHAR('J'),
    USB_CHAR('o'),
    USB_CHAR('y'),
    USB_CHAR('s'),
    USB_CHAR('t'),
    USB_CHAR('i'),
    USB_CHAR('c'),
    USB_CHAR('k'),
};

// Configuration description string
const unsigned char g_pConfigurationDescriptionString[] = {
    2 + (21 * 2),
    USB_DTYPE_STRING,
    USB_CHAR('D'),
    USB_CHAR('e'),
    USB_CHAR('f'),
    USB_CHAR('a'),
    USB_CHAR('u'),
    USB_CHAR('l'),
    USB_CHAR('t'),
    USB_CHAR(' '),
    USB_CHAR('C'),
    USB_CHAR('o'),
    USB_CHAR('n'),
    USB_CHAR('f'),
    USB_CHAR('i'),
    USB_CHAR('g'),
    USB_CHAR('u'),
    USB_CHAR('r'),
    USB_CHAR('a'),
    USB_CHAR('t'),
    USB_CHAR('i'),
    USB_CHAR('o'),
    USB_CHAR('n'),
};

// Descriptor string table
const unsigned char * const g_pStringDescriptors[] = {
    g_pLangDescriptor,
    g_pManufacturerString,
    g_pProductString,
    g_pSerialNumberString,
    g_pHIDInterfaceDescriptionString,
    g_pConfigurationDescriptionString
};

// HID joystick device callback function
unsigned int USBJoystickCallback(void *pvCBData, unsigned int ulEvent, unsigned int ulMsgParam, void *pvMsgData) {
    switch(ulEvent) {
        // Device connected
        case USB_EVENT_CONNECTED:
            return(0);
        // Transmission completed
        case USB_EVENT_TX_COMPLETE:
            return(0);
        // Suspension request
        case USB_EVENT_SUSPEND:
            return(0);
        // Ignore any other event
        default:
            break;
    }

    return(0);
}

// USB DCD remote wakeup request
tBoolean USBDCDRemoteWakeupRequest(unsigned int ulIndex) {
    return true;
}

// The HID device instance initialization and customization structure
tHIDJoystickInstance g_sHIDJoystickInstance;

// The HID joystick device initialization and customization structure
const tUSBDHIDJoystickDevice g_sUSBDHIDJoystickDevice = {
    USB_VID_STELLARIS, /*usVID*/
    USB_PID_SCOPE, /*usPID*/
    50, /*usMaxPowermA*/
    USB_CONF_ATTR_BUS_PWR, /*ucPwrAttributes*/
    &USBJoystickCallback, /*pfnCallback*/
    (void *) 0, /*pvCBData*/
    g_pStringDescriptors, /*ppStringDescriptors*/
    (sizeof(g_pStringDescriptors) / sizeof(unsigned char *)), /*ulNumStringDescriptors*/
    &g_sHIDJoystickInstance /*psPrivateHIDJoystickData*/
};
// SNIPPET_END

// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_FUNCTIONS
// SNIPPET_END

// -------------------- USB_INTERRUPT PROCESS START --------------------

// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_INTERRUPT_PROCESS_GLOBAL_VARIABLES
// SNIPPET_END

// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_INTERRUPT_PROCESS_FUNCTIONS
// SNIPPET_END

// USB_INTERRUPT process
static void USB_INTERRUPT(void) {
    RETURN_CODE_TYPE RETURN_CODE;
    // SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_INTERRUPT_PROCESS_VARIABLES
    // SNIPPET_END

    // SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_INTERRUPT_PROCESS_INITIALIZATION
    // SNIPPET_END

    // Main loop
    while (true) {

        // SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_INTERRUPT_PROCESS_BEFORE_SUSPENSION_CODE
        // Handles interrupts
        USB0DeviceIntHandler();
        // SNIPPET_END

        // Waits for next period
        PERIODIC_WAIT(&RETURN_CODE);
        if (RETURN_CODE != NO_ERROR) {
            // SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_INTERRUPT_PROCESS_PERIODIC_WAIT_ERROR_HANDLING_CODE
            while (true) {
            }
            // SNIPPET_END
        }

        // SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_INTERRUPT_PROCESS_AFTER_SUSPENSION_CODE
        // SNIPPET_END
    }
}

// USB_INTERRUPT process attributes
static PROCESS_ATTRIBUTE_TYPE USB_INTERRUPT_PROCESS_ATTRIBUTE = {
/*NAME*/(PROCESS_NAME_TYPE) "USB_INTERRUPT",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &USB_INTERRUPT,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*BASE_PRIORITY*/(PRIORITY_TYPE) 10,
/*PERIOD*/(SYSTEM_TIME_TYPE) 5000000,
/*TIME_CAPACITY*/(SYSTEM_TIME_TYPE) 5000000,
/*DEADLINE*/(DEADLINE_TYPE) HARD };

// -------------------- USB_INTERRUPT PROCESS END --------------------

// -------------------- USB PROCESS START --------------------

// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_PROCESS_GLOBAL_VARIABLES
// SNIPPET_END

// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_PROCESS_FUNCTIONS
// SNIPPET_END

// USB process
static void USB(void) {
    RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_PROCESS_VARIABLES
    unsigned short data;
    unsigned char stepID;
    short value;
    short sX, sY, sZ, sRX, sRY, sRZ;
    unsigned short usButtons;
	// SNIPPET_END

	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_PROCESS_INITIALIZATION
    // Initialize samples
    sX = 0;
    sY = 0;
    sZ = 0;
    sRX = 0;
    sRY = 0;
    sRZ = 0;
    usButtons = JOYSTICK_REPORT_BUTTON_NONE;
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_PROCESS_BEFORE_SUSPENSION_CODE
        // Read sample
        while (TSCADCFIFOWordCountRead(SOC_ADC_TSC_0_REGS, TSCADC_FIFO_0) > 0) {
            TSCADCFIFOADCDataStepIDRead(SOC_ADC_TSC_0_REGS, TSCADC_FIFO_0, &data, &stepID);
            value = (data * 16);
            switch (stepID) {
            case 0:
                sX = value;
                break;
            case 1:
                sY = value;
                break;
            case 2:
                sZ = value;
                break;
            case 3:
                sRX = value;
                break;
            case 4:
                sRY = value;
                break;
            case 5:
                sRZ = value;
                break;
            }
        }

        // Reads buttons
        usButtons = (GPIO_BUTTON01 << 0)
                  | (GPIO_BUTTON02 << 1)
                  | (GPIO_BUTTON03 << 2)
                  | (GPIO_BUTTON04 << 3)
                  | (GPIO_BUTTON05 << 4)
                  | (GPIO_BUTTON06 << 5)
                  | (GPIO_BUTTON07 << 6)
                  | (GPIO_BUTTON08 << 7)
                  | (GPIO_BUTTON09 << 8)
                  | (GPIO_BUTTON10 << 9)
                  | (GPIO_BUTTON11 << 10)
                  | (GPIO_BUTTON12 << 11)
                  | (GPIO_BUTTON13 << 12)
                  | (GPIO_BUTTON14 << 13)
                  | (GPIO_BUTTON15 << 14)
                  | (GPIO_BUTTON16 << 15);

        // Shows information
        ConsoleUtilsPrintf("X, Y, Z, RX, RY, RZ: %d, %d, %d, %d, %d, %d\n", sX, sY, sZ, sRX, sRY, sRZ);

        // Notifies state change
        USBDHIDJoystickStateChange((tUSBDHIDJoystickDevice *)&g_sUSBDHIDJoystickDevice, usButtons, sX, sY, sZ, sRX, sRY, sRZ);
		// SNIPPET_END

        // Waits for next period
        PERIODIC_WAIT(&RETURN_CODE);
        if (RETURN_CODE != NO_ERROR) {
            // SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_PROCESS_PERIODIC_WAIT_ERROR_HANDLING_CODE
            while (true) {
            }
            // SNIPPET_END
        }

		// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_USB_PROCESS_AFTER_SUSPENSION_CODE
		// SNIPPET_END
	}
}

// USB process attributes
static PROCESS_ATTRIBUTE_TYPE USB_PROCESS_ATTRIBUTE = {
/*NAME*/(PROCESS_NAME_TYPE) "USB",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &USB,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*BASE_PRIORITY*/(PRIORITY_TYPE) 10,
/*PERIOD*/(SYSTEM_TIME_TYPE) 50000000,
/*TIME_CAPACITY*/(SYSTEM_TIME_TYPE) 50000000,
/*DEADLINE*/(DEADLINE_TYPE) HARD };

// -------------------- USB PROCESS END --------------------

// SYSTEMPARTITION_USB partition error handler
static void ERRORHANDLER(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	ERROR_STATUS_TYPE ERROR_STATUS;
	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_ERRORHANDLER_VARIABLES
	// SNIPPET_END

	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_ERRORHANDLER_INITIALIZATION
	// SNIPPET_END

	// Error handling loop
	while (true) {

		// Gets error status
		GET_ERROR_STATUS(&ERROR_STATUS, &RETURN_CODE);
		if (RETURN_CODE == NO_ACTION) {
			break;
		} else if (RETURN_CODE != NO_ERROR) {
			// SNIPPET_START GET_ERROR_STATUS_ERROR_HANDLING_CODE
			// SNIPPET_END
		}

		// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_ERRORHANDLER_CODE
		// Stops failed process
		STOP(ERROR_STATUS.FAILED_PROCESS_ID, &RETURN_CODE);

		// Restarts failed process
		START(ERROR_STATUS.FAILED_PROCESS_ID, &RETURN_CODE);
		// SNIPPET_END
	}

	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_ERRORHANDLER_TERMINATION
	// SNIPPET_END

	// Stops
	STOP_SELF();
}

// SYSTEMPARTITION_USB partition health monitoring callback
void SYSTEMPARTITION_USB_HMCALLBACK(SYSTEM_STATE_TYPE SYSTEM_STATE, ERROR_IDENTIFIER_TYPE ERROR_IDENTIFIER) {
	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_HEALTHMONITORINGCALLBACK_VARIABLES
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_END

	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_HEALTHMONITORINGCALLBACK_CODE
	// Restarts partition
	SET_PARTITION_MODE(COLD_START, &RETURN_CODE);
	// SNIPPET_END
}

// SYSTEMPARTITION_USB partition default process
void SYSTEMPARTITION_USB(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_DEFAULTPROCESS_VARIABLES
	// SNIPPET_END

	// Creates USB_INTERRUPT process
	CREATE_PROCESS(&USB_INTERRUPT_PROCESS_ATTRIBUTE, &USB_INTERRUPT_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PROCESS_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR CREATING PROCESS", 22, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

    // Creates USB process
    CREATE_PROCESS(&USB_PROCESS_ATTRIBUTE, &USB_PROCESS_ID, &RETURN_CODE);
    if (RETURN_CODE != NO_ERROR) {
        // SNIPPET_START CREATE_PROCESS_ERROR_HANDLING_CODE
        RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR CREATING PROCESS", 22, &RETURN_CODE);
        return;
        // SNIPPET_END
    }

    // Starts USB_INTERRUPT process
    START(USB_INTERRUPT_PROCESS_ID, &RETURN_CODE);
    if (RETURN_CODE != NO_ERROR) {
        // SNIPPET_START START_ERROR_HANDLING_CODE
        RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR STARTING PROCESS", 22, &RETURN_CODE);
        return;
        // SNIPPET_END
    }

	// Starts USB process
	START(USB_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START START_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR STARTING PROCESS", 22, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Creates error handler
	CREATE_ERROR_HANDLER(&ERRORHANDLER, 256, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_ERROR_HANDLER_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR CREATING ERROR HANDLER", 28, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_DEFAULTPROCESS_INITIALIZATION
    // Shows information
    ConsoleUtilsPrintf("Initializing GPIO...\n");

    // Enables GPIO clocks
    GPIO0ModuleClkConfig();
    GPIO1ModuleClkConfig();
    GPIO2ModuleClkConfig();

    // Selects GPIO pins
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(11)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(10)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_CSN(0)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(1)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(5)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_CSN(2)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_CLK) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(14)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(9)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(8)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(0)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(4)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_CSN(1)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(8)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(11)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(15)) = CONTROL_CONF_MUXMODE(7) | CONTROL_CONF_GPMC_AD_CONF_GPMC_AD_RXACTIVE;

    // Enables GPIO modules
    GPIOModuleEnable(SOC_GPIO_0_REGS);
    GPIOModuleEnable(SOC_GPIO_1_REGS);
    GPIOModuleEnable(SOC_GPIO_2_REGS);

    // Resets GPIO modules
    GPIOModuleReset(SOC_GPIO_0_REGS);
    GPIOModuleReset(SOC_GPIO_1_REGS);
    GPIOModuleReset(SOC_GPIO_2_REGS);

    // Sets GPIO pins mode
    GPIODirModeSet(SOC_GPIO_2_REGS, 25, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_2_REGS, 24, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 29, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 1, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 5, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 31, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_2_REGS, 1, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 14, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_2_REGS, 23, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_2_REGS, 22, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 0, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 4, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 30, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, 22, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, 27, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_1_REGS, 15, GPIO_DIR_INPUT);

    // Shows information
    ConsoleUtilsPrintf("Initializing ADC...\n");

    // Configuration
    #define TSCADC_CLOCK 24000000
    #define TSCADC_FREQUENCY 3000000

    // Configures ADC clock
    TSCADCModuleClkConfig();

    // Configures ADC pins
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_AIN1) = CONTROL_CONF_AIN1_CONF_AIN1_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_AIN2) = CONTROL_CONF_AIN2_CONF_AIN2_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_AIN3) = CONTROL_CONF_AIN3_CONF_AIN3_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_AIN4) = CONTROL_CONF_AIN4_CONF_AIN4_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_AIN5) = CONTROL_CONF_AIN5_CONF_AIN5_RXACTIVE;
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_AIN6) = CONTROL_CONF_AIN6_CONF_AIN6_RXACTIVE;

    // Configures ADC frequency
    TSCADCConfigureAFEClock(SOC_ADC_TSC_0_REGS, TSCADC_CLOCK, TSCADC_FREQUENCY);

    // Enables transistor bias
    TSCADCTSTransistorConfig(SOC_ADC_TSC_0_REGS, TSCADC_TRANSISTOR_ENABLE);

    // Sets general purpose mode
    TSCADCTSModeConfig(SOC_ADC_TSC_0_REGS, TSCADC_GENERAL_PURPOSE_MODE);

    // Disables step configuration registers write protection
    TSCADCStepConfigProtectionDisable(SOC_ADC_TSC_0_REGS);

    // Disables IDLE step
    TSCADCIdleModeSet(SOC_ADC_TSC_0_REGS, TSC_ADC_SS_SYSCONFIG_IDLEMODE_NO_IDLE);

    // Configure X step
    {

        // Configures step to single ended operation mode
        TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 0);

        // Configures step to select channel and reference voltages
        TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 0, TSCADC_NEGATIVE_REF_VSSA, TSCADC_NEGATIVE_INP_CHANNEL3, TSCADC_POSITIVE_INP_CHANNEL3, TSCADC_POSITIVE_REF_VDDA);

        // Configures step open delay
        TSCADCTSStepOpenDelayConfig(SOC_ADC_TSC_0_REGS, 0, 50);

        // Turns XPPSW (pull-up) pin on
        // REMOVE TSCADCTSStepAnalogSupplyConfig(SOC_ADC_TSC_0_REGS, TSCADC_XPPSW_PIN_ON, TSCADC_XNPSW_PIN_OFF, TSCADC_YPPSW_PIN_OFF, 0);

        // Turns XNNSW (pull-down) pin on
        //TSCADCTSStepAnalogGroundConfig(SOC_ADC_TSC_0_REGS, TSCADC_XNNSW_PIN_ON, TSCADC_YPNSW_PIN_OFF, TSCADC_YNNSW_PIN_OFF,  TSCADC_WPNSW_PIN_OFF, 0);

        // Selects step FIFO
        TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 0, TSCADC_FIFO_0);

        // Configures step to continuous mode
        TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 0, TSCADC_CONTINIOUS_SOFTWARE_ENABLED);

        // Enables step
        TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 1, true);
    }

    // Configure Y step
    {

        // Configures step to single ended operation mode
        TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 1);

        // Configures step to select channel and reference voltages
        TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 1, TSCADC_NEGATIVE_REF_VSSA, TSCADC_NEGATIVE_INP_CHANNEL6, TSCADC_POSITIVE_INP_CHANNEL6, TSCADC_POSITIVE_REF_VDDA);

        // Configures step open delay
        TSCADCTSStepOpenDelayConfig(SOC_ADC_TSC_0_REGS, 1, 50);

        // Turns XPPSW (pull-up) pin on
        // REMOVE TSCADCTSStepAnalogSupplyConfig(SOC_ADC_TSC_0_REGS, TSCADC_XPPSW_PIN_ON, TSCADC_XNPSW_PIN_OFF, TSCADC_YPPSW_PIN_OFF, 1);

        // Turns XNNSW (pull-down) pin on
        // REMOVE TSCADCTSStepAnalogGroundConfig(SOC_ADC_TSC_0_REGS, TSCADC_XNNSW_PIN_ON, TSCADC_YPNSW_PIN_OFF, TSCADC_YNNSW_PIN_OFF,  TSCADC_WPNSW_PIN_OFF, 1);

        // Selects step FIFO
        TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 1, TSCADC_FIFO_0);

        // Configures step to continuous mode
        TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 1, TSCADC_CONTINIOUS_SOFTWARE_ENABLED);

        // Enables step
        TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 2, true);
    }

    // Configure Z step
    {

        // Configures step to single ended operation mode
        TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 2);

        // Configures step to select channel and reference voltages
        TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 2, TSCADC_NEGATIVE_REF_VSSA, TSCADC_NEGATIVE_INP_CHANNEL4, TSCADC_POSITIVE_INP_CHANNEL4, TSCADC_POSITIVE_REF_VDDA);

        // Configures step open delay
        TSCADCTSStepOpenDelayConfig(SOC_ADC_TSC_0_REGS, 2, 50);

        // Turns XPPSW (pull-up) pin on
        // REMOVE TSCADCTSStepAnalogSupplyConfig(SOC_ADC_TSC_0_REGS, TSCADC_XPPSW_PIN_ON, TSCADC_XNPSW_PIN_OFF, TSCADC_YPPSW_PIN_OFF, 2);

        // Turns XNNSW (pull-down) pin on
        // REMOVE TSCADCTSStepAnalogGroundConfig(SOC_ADC_TSC_0_REGS, TSCADC_XNNSW_PIN_ON, TSCADC_YPNSW_PIN_OFF, TSCADC_YNNSW_PIN_OFF,  TSCADC_WPNSW_PIN_OFF, 2);

        // Selects step FIFO
        TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 2, TSCADC_FIFO_0);

        // Configures step to continuous mode
        TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 2, TSCADC_CONTINIOUS_SOFTWARE_ENABLED);

        // Enables step
        TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 3, true);
    }

    // Configure RX step
    {

        // Configures step to single ended operation mode
        TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 3);

        // Configures step to select channel and reference voltages
        TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 3, TSCADC_NEGATIVE_REF_VSSA, TSCADC_NEGATIVE_INP_CHANNEL2, TSCADC_POSITIVE_INP_CHANNEL2, TSCADC_POSITIVE_REF_VDDA);

        // Configures step open delay
        TSCADCTSStepOpenDelayConfig(SOC_ADC_TSC_0_REGS, 3, 50);

        // Turns XPPSW (pull-up) pin on
        // REMOVE TSCADCTSStepAnalogSupplyConfig(SOC_ADC_TSC_0_REGS, TSCADC_XPPSW_PIN_ON, TSCADC_XNPSW_PIN_OFF, TSCADC_YPPSW_PIN_OFF, 3);

        // Turns XNNSW (pull-down) pin on
        // REMOVE TSCADCTSStepAnalogGroundConfig(SOC_ADC_TSC_0_REGS, TSCADC_XNNSW_PIN_ON, TSCADC_YPNSW_PIN_OFF, TSCADC_YNNSW_PIN_OFF,  TSCADC_WPNSW_PIN_OFF, 3);

        // Selects step FIFO
        TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 3, TSCADC_FIFO_0);

        // Configures step to continuous mode
        TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 3, TSCADC_CONTINIOUS_SOFTWARE_ENABLED);

        // Enables step
        TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 4, true);
    }

    // Configure RY step
    {

        // Configures step to single ended operation mode
        TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 4);

        // Configures step to select channel and reference voltages
        TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 4, TSCADC_NEGATIVE_REF_VSSA, TSCADC_NEGATIVE_INP_CHANNEL5, TSCADC_POSITIVE_INP_CHANNEL5, TSCADC_POSITIVE_REF_VDDA);

        // Configures step open delay
        TSCADCTSStepOpenDelayConfig(SOC_ADC_TSC_0_REGS, 4, 50);

        // Turns XPPSW (pull-up) pin on
        // REMOVE TSCADCTSStepAnalogSupplyConfig(SOC_ADC_TSC_0_REGS, TSCADC_XPPSW_PIN_ON, TSCADC_XNPSW_PIN_OFF, TSCADC_YPPSW_PIN_OFF, 4);

        // Turns XNNSW (pull-down) pin on
        // REMOVE TSCADCTSStepAnalogGroundConfig(SOC_ADC_TSC_0_REGS, TSCADC_XNNSW_PIN_ON, TSCADC_YPNSW_PIN_OFF, TSCADC_YNNSW_PIN_OFF,  TSCADC_WPNSW_PIN_OFF, 4);

        // Selects step FIFO
        TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 4, TSCADC_FIFO_0);

        // Configures step to continuous mode
        TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 4, TSCADC_CONTINIOUS_SOFTWARE_ENABLED);

        // Enables step
        TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 5, true);
    }

    // Configure RZ step
    {

        // Configures step to single ended operation mode
        TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 5);

        // Configures step to select channel and reference voltages
        TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 5, TSCADC_NEGATIVE_REF_VSSA, TSCADC_NEGATIVE_INP_CHANNEL7, TSCADC_POSITIVE_INP_CHANNEL7, TSCADC_POSITIVE_REF_VDDA);

        // Configures step open delay
        TSCADCTSStepOpenDelayConfig(SOC_ADC_TSC_0_REGS, 5, 50);

        // Turns XPPSW (pull-up) pin on
        // REMOVE TSCADCTSStepAnalogSupplyConfig(SOC_ADC_TSC_0_REGS, TSCADC_XPPSW_PIN_ON, TSCADC_XNPSW_PIN_OFF, TSCADC_YPPSW_PIN_OFF, 5);

        // Turns XNNSW (pull-down) pin on
        // REMOVE TSCADCTSStepAnalogGroundConfig(SOC_ADC_TSC_0_REGS, TSCADC_XNNSW_PIN_ON, TSCADC_YPNSW_PIN_OFF, TSCADC_YNNSW_PIN_OFF,  TSCADC_WPNSW_PIN_OFF, 5);

        // Selects step FIFO
        TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 5, TSCADC_FIFO_0);

        // Configures step to continuous mode
        TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 5, TSCADC_CONTINIOUS_SOFTWARE_ENABLED);

        // Enables step
        TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 6, true);
    }

    // Enables step ID storage
    TSCADCStepIDTagConfig(SOC_ADC_TSC_0_REGS, true);

    // Enables step configuration registers write protection
    TSCADCStepConfigProtectionEnable(SOC_ADC_TSC_0_REGS);

    // Enables module
    TSCADCModuleStateSet(SOC_ADC_TSC_0_REGS, TSCADC_MODULE_ENABLE);

    // Shows information
    ConsoleUtilsPrintf("Initializing USB...\n");

    // Initialize joystick interface
    USBDHIDJoystickInit(0, (tUSBDHIDJoystickDevice *)&g_sUSBDHIDJoystickDevice);

    // Shows information
    ConsoleUtilsPrintf("Initialized\n");
	// SNIPPET_END

	// Sets partition mode
	SET_PARTITION_MODE(NORMAL, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START SET_PARTITION_MODE_ERROR_HANDLING_CODE
		RAISE_APPLICATION_ERROR(APPLICATION_ERROR, (MESSAGE_ADDR_TYPE) "ERROR SETTING PARTITION MODE", 28, &RETURN_CODE);
		return;
		// SNIPPET_END
	}

	// Start of idle process

	// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_IDLEPROCESS_INITIALIZATION
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START SYSTEMPARTITION_USB_PARTITION_IDLEPROCESS_CODE
		// SNIPPET_END
	}
}
