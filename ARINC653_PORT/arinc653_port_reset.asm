	; Imports C declarations
	; References global symbols
	.ref _bss_start
	.ref _bss_end
	.ref PORT_VECTORTABLE_CORE0
	.ref _core0_module_stack_start
	.ref _core0_module_stack_end
	.ref SYSTEM_DEFAULTMODULE

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

	; Reads core revision
	MRC p15, #0, R0, c0, c0, #0
	AND R1, R0, #0x00f00000
	AND R2, R0, #0x0000000f
	ORR R0, R2, R1, LSR #20-4

	; Handles ARM errata 742230 for devices up to r2p2
	CMP R0, #0x22
	MRCLE p15, #0, R1, c15, c0, #1
	ORRLE R1, R1, #1 << 4
	MCRLE p15, #0, R1, c15, c0, #1

	; Handles ARM errata 743622 for devices r2b*
	TEQ R0, #0x00200000
	MRCEQ p15, #0, R1, c15, c0, #1
	ORREQ R1, R1, #1 << 6
	MCREQ p15, #0, R1, c15, c0, #1

	; Clears BSS
	LDR R0, _BSS_START
	LDR R1, _BSS_END
	MOV R2, #0
PORT_RESETHANDLER_CLEARBSSLOOP:
	STR R2, [R0], #4
	CMP R0, R1
	BLT PORT_RESETHANDLER_CLEARBSSLOOP

	; Disables data cache
	MRC p15, #0, R0, c1, c0, #0
	BIC R0, R0, #0x0004
	MCR p15, #0, R0, c1, c0, #0
	DSB

	; Disables instruction cache
	MRC p15, #0, R0, c1, c0, #0
	BIC R0, R0, #0x1000
	MCR p15, #0, R0, c1, c0, #0
	ISB

	; Disables branch prediction
	MRC p15, #0, R0, c1, c0, #0
	BIC R0, R0, #0x0800
	MCR p15, #0, R0, c1, c0, #0
	ISB

	; Disables MMU
	MRC	p15, #0, R0, c1, c0, #0
	BIC	R0, R0, #0x1
	MCR	p15, #0, R0, c1, c0, #0

	; Temporarily sets the vector table address to the DDR location
	LDR	R0, _PORT_VECTORTABLE_CORE0
	MCR	p15, #0, R0, c12, c0, #0

	; Invalidates TLBs
	MOV R0, #0
	MCR p15, #0, R0, c8, c7, #0 ; Invalidates TLBs
	MCR p15, #0, R0, c7, c5, #0 ; Invalidates instruction cache
	MCR p15, #0, R0, c7, c5, #6 ; Invalidates branch predictor array

	; Reads stack pointer
	LDR R0, _STACK_TOP_CORE0
	; Sets up stack for undefined mode
	CPS #MODE_UND
	MOV SP, R0
	MRS R1, CPSR
	BIC R1, R1, #0x200 ; Little endian
	MSR SPSR_fsxc, R1
	SUB R0, R0, #MODE_STACKSIZE_UND
	; Sets up stack for abort mode
	CPS #MODE_ABT
	MOV SP, R0
	MRS R1, CPSR
	BIC R1, R1, #0x200 ; Little endian
	MSR SPSR_fsxc, R1
	SUB R0, R0, #MODE_STACKSIZE_ABT
	; Sets up stack for IRQ mode
	CPS #MODE_IRQ
	MOV SP, R0
	MRS R1, CPSR
	BIC R1, R1, #0x200 ; Little endian
	MSR SPSR_fsxc, R1
	SUB R0, R0, #MODE_STACKSIZE_IRQ
	; Sets up stack for SVC mode
	CPS #MODE_SVC
	MOV SP, R0
	MRS R1, CPSR
	BIC R1, R1, #0x200 ; Little endian
	MSR SPSR_fsxc, R1
	SUB R0, R0, #MODE_STACKSIZE_SVC
	; Sets up stack for SYS/FIQ/USR mode
	CPS #MODE_SYS
	MOV SP, R0
	MRS R1, CPSR
	BIC R1, R1, #0x200 ; Little endian
	MSR SPSR_fsxc, R1
	SUB R0, R0, #MODE_STACKSIZE_SYSFIQUSR
	; Verifies stack bottom
	LDR R1, _STACK_BOTTOM_CORE0
	CMP R0, R1
PORT_RESETHANDLER_CORE0_STACKERRORLOOP:
	BNE PORT_RESETHANDLER_CORE0_STACKERRORLOOP

	; Enables NEON/VFP coprocessor
	MRC P15, #0, R0, C1, C0, #2 ; R0 = Coprocessor Access Control Register (CACR)
	ORR R0, R0, #(0xF << 20) ; Enables full access for CP10 and CP11
	MCR P15, #0, R0, C1, C0, #2 ; Access control register = R0
	MOV R0, #0
	MCR P15, #0, R0, C7, C5, #4 ; Flushes prefetch buffer
	FMRX R0, FPEXC
	ORR R0, R0, #0x40000000
	FMXR FPEXC, R0 ; Enables NEON/VFP

	.if !$$defined(DISABLE_BRANCHPREDICTION)
	; Enables branch prediction
	MRC p15, #0, R0, c1, c0, #0
	ORR R0, R0, #0x0800
	MCR p15, #0, R0, c1, c0, #0
	ISB
	.endif

	.if !$$defined(DISABLE_INSTRUCTIONCACHE)
	; Enables instruction cache
	MRC p15, #0, R0, c1, c0, #0
	ORR R0, R0, #0x1000
	MCR p15, #0, R0, c1, c0, #0
	ISB
	.endif

	.if !$$defined(DISABLE_DATACACHE)
	; Enables data cache
	MRC p15, #0, R0, c1, c0, #0
	ORR R0, R0, #0x0004
	MCR p15, #0, R0, c1, c0, #0
	DSB
	.endif

	; Enables asynchronous abort exception
	MRS R0, CPSR
	BIC R0, R0, #0x100
	MSR CPSR_xsf, R0

	; Initializes CP15 registers
	MOV R0, #0
	MCR p15, #0, R0, c6, c0, #0 ; DFAR
	MCR p15, #0, R0, c5, c0, #0 ; DFSR
	MCR p15, #0, R0, c6, c0, #2 ; IFAR
	MCR p15, #0, R0, c5, c0, #1 ; IFSR
	MCR p15, #0, R0, c9, c13, #2 ; PMXEVCNTR
	MCR p15, #0, R0, c13, c0, #2 ; TPIDRURW
	MCR p15, #0, R0, c13, c0, #3; TPIDRURO

	; Core #0 enters system default module to perform system startup
	LDR R0, _SYSTEM_DEFAULTMODULE
	BX R0

	; Enters infinite loop
	SUB PC, PC, #8

; Variables and functions addresses
_BSS_START .word _bss_start
_BSS_END .word _bss_end
_PORT_VECTORTABLE_CORE0 .word PORT_VECTORTABLE_CORE0
_STACK_TOP_CORE0 .word _core0_module_stack_start
_STACK_BOTTOM_CORE0 .word _core0_module_stack_end
_SYSTEM_DEFAULTMODULE .word SYSTEM_DEFAULTMODULE

	.end
