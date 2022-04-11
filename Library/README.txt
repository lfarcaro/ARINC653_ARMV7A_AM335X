This folder's files were obtained from TI's AM335X StarterWare (https://sourceforge.net/p/starterwarefree/code/ci/master/tree/).

Changes and new implementations were performed as follows.

- Interrupts enabling suppressed in "usbdenum.c", for operation in polling mode.
- Fixed conditional code in "usbhostenum.c".
- Added joystick-related constants in "usbhid.h".