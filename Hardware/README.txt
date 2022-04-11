This folder's files were mainly obtained from TI's AM335X StarterWare (https://sourceforge.net/p/starterwarefree/code/ci/master/tree/).

Changes and new implementations were performed as follows.

- Files specifically targeted to BeagleBone had "_bb" appended to their names.
- Files "usbdhidjoystick.c" and "usbdhidjoystick.h" were included as an example for using usblib.
- File "debug.c" added to implement the "__error__" debug function.
- The "hw_types.h" include at "pruss.c" had its path fixed.
- Changes in "cp15.asm" and "cp15.h":
	- Added functions "CP15Ttbr0Set" and "CP15Ttbr1Set".
	- Added function "CP15Ttb1Set".
	- Added function "CP15SetContextID".
	- Function "CP15TtbCtlTtb0Config" was renamed to "CP15TtbCtlConfig", and had the N value added as a parameter.
- Changes in "cpsw.c" and "cpsw.h":
	- Added function "CPSWCPDMATxHdrDescPtrRead".
	- Added function "CPSWCPDMARxHdrDescPtrRead".
- Variable "fnRAMVectors" in "interrupt.c" was renamed to "IRQHandlerVectorTable".
- Defined "USB_EP_TO_INDEX" in "usb.h".
- Fixed assertion in "usb.c".
- Added function "TSCADCModuleClkConfig" in "tsc_adc.h".
- Added function "TSCADCFIFOADCDataStepIDRead" in "tsc_adc.c" and "tsc_adc.h".
- The "init.asm" file was not included.
- Files "mmu.c" and "mmu.h" were not included.
- Added functions "UARTConsolePutc" and "UARTConsoleGetcTimeout" in "uartStdio.h".
- Added file "pwmss.h" to give access to functions:
	PWMSSModuleClkConfig
	PWMSSTBClkEnable
- Added the following functions to "dcan.h":
	DCANPinMuxSetUp
	DCANMsgRAMInit
	DCANModuleClkConfig
- Commented definition of "DELAY_USE_INTERRUPTS" in "sysdelay_bb.c".
- In function "DCANPinMuxSetUp" within "dcan_bb.c":
	- Removed board-specific behavior
	- Set to use instance DCAN1 instead of DCAN0
- In function "UARTPinMuxSetup" within "uart_bb.c", added support to the instance UART1.
- Implemented function "UARTConsoleGetcTimeout" in "uartConsole_bb.c".