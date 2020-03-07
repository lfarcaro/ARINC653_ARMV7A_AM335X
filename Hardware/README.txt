This folder's files were mainly obtained from TI's AM335X StarterWare.

Changes and new implementations were performed as follows.

- Files specifically targeted to BeagleBone had "_bb" appended to their names.
- Files "ethernet.c" and "ethernet.h" were implemented from scratch.
- Files "protocol.c" and "protocol.h" were implemented from scratch.
- Files "mmu.c" and "mmu.h" were implemented from scratch, and replace StarterWare's original files.
- The "hw_types.h" include at "pruss.c" had its path fixed.
- Changes in "cp15.asm" and "cp15.h":
	- Added function "CP15Ttb1Set".
	- Added function "CP15SetContextID".
	- Function "CP15TtbCtlTtb0Config" was renamed to "CP15TtbCtlConfig", and had the N value added as a parameter.
- Changes in "cpsw.c" and "cpsw.h":
	- Added function "CPSWCPDMATxHdrDescPtrRead".
	- Added function "CPSWCPDMARxHdrDescPtrRead".
- Variable "fnRAMVectors" in "interrupt.c" was renamed to "IRQHandlerVectorTable".
- The following files were not included:
	- "init.asm"
	- "usb.c"
	- "usb_bb.c" (BeagleBone-specific)
	- "hw_usb.h"
	- "hw_usbOtg_AM335x.h"
	- "usbphyGS70.c"
	- "hw_usbphyGS70.h"
	- "cppi41dma.c"
	- "cppi41dma.h"