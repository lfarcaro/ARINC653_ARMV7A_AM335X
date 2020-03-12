	; References global symbols
	.ref __stack
	.ref __STACK_END
	.ref DDR0_SYSTEM_DATA_BSS_START
	.ref DDR0_SYSTEM_DATA_BSS_END
	.ref MODULE_DEFAULTPARTITION

; Mode constants
MODE_USR .set 0x10
MODE_FIQ .set 0x11
MODE_IRQ .set 0x12
MODE_SVC .set 0x13
MODE_ABT .set 0x17
MODE_UND .set 0x1B
MODE_SYS .set 0x1F

; Stack sizes for different modes
MODE_STACKSIZE_IRQ .set 0xC00
MODE_STACKSIZE_SVC .set 0xC00
MODE_STACKSIZE_ABT .set 0xC00
MODE_STACKSIZE_UND .set 0xC00
MODE_STACKSIZE_SYSFIQUSR .set 0x1000

	.sect ".boot"
	.state32

; Reset handler method
	.align 4
	.def PORT_RESETHANDLER
PORT_RESETHANDLER:
	; Disables interrupts
	CPSID if
	; Reads stack pointer
	LDR R0, _STACK_TOP
	; Sets up stack for undefined mode
	CPS #MODE_UND
	MOV SP, R0
	SUB R0, R0, #MODE_STACKSIZE_UND
	; Sets up stack for abort mode
	CPS #MODE_ABT
	MOV SP, R0
	SUB R0, R0, #MODE_STACKSIZE_ABT
	; Sets up stack for IRQ mode
	CPS #MODE_IRQ
	MOV SP, R0
	SUB R0, R0, #MODE_STACKSIZE_IRQ
	; Sets up stack for SVC mode
	CPS #MODE_SVC
	MOV SP, R0
	SUB R0, R0, #MODE_STACKSIZE_SVC
	; Sets up stack for SYS/FIQ/USR mode
	CPS #MODE_SYS
	MOV SP, R0
	SUB R0, R0, #MODE_STACKSIZE_SYSFIQUSR
	; Verifies stack bottom
	LDR R1, _STACK_BOTTOM
	CMP R0, R1
PORT_RESETHANDLER_STACKERRORLOOP:
	BNE PORT_RESETHANDLER_STACKERRORLOOP
	; Clears BSS
	LDR R0, _BSS_START
	LDR R1, _BSS_END
	MOV R2, #0
PORT_RESETHANDLER_CLEARBSSLOOP:
	STR R2, [R0], #4
	CMP R0, R1
	BLE PORT_RESETHANDLER_CLEARBSSLOOP
	; Clears data and prefetch exception registers
	MOV R0, #0
	MCR p15, #0, R0, c6, c0, #0 ; Clears DFAR
	MCR p15, #0, R0, c5, c0, #0 ; Clears DFSR
	MCR p15, #0, R0, c6, c0, #2 ; Clears IFAR
	MCR p15, #0, R0, c5, c0, #1 ; Clears IFSR
	; Enables NEON/VFP coprocessor
	MRC P15, #0, R1, C1, C0, #2 ; R1 = Access control register
	ORR R1, R1, #(0x3 << 30) ; Disables D16-D31 and Advanced SIMD
	ORR R1, R1, #(0xF << 20) ; Enables full access for CP10 and CP11
	MCR P15, #0, R1, C1, C0, #2 ; Access control register = R1
	MOV R1, #0
	MCR P15, #0, R1, C7, C5, #4 ; Flushes prefetch buffer
	MOV R0, #0x40000000
	FMXR FPEXC, R0 ; Enables NEON/VFP
	; Enters module default partition
	LDR R10, _MODULE_DEFAULTPARTITION
	MOV LR, PC
	MOV PC, R10
	; Enters infinite loop
	SUB PC, PC, #8

; Variables and functions addresses
_STACK_TOP .word __STACK_END
_STACK_BOTTOM .word __stack
_BSS_START .word DDR0_SYSTEM_DATA_BSS_START
_BSS_END .word DDR0_SYSTEM_DATA_BSS_END
_MODULE_DEFAULTPARTITION .word MODULE_DEFAULTPARTITION

	.end
