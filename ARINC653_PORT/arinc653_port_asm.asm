	; Imports C declarations
	.cdecls C,LIST,"soc_AM335x.h"
	.cdecls C,LIST,"hw_intc.h"

	; References global symbols

	; Variables and functions addresses
	.ref CURRENT_CONTEXT
	.ref SCHEDULER
	.ref TICK
	.ref PORT_ABORTERRORHANDLER
	.ref PORT_UNDEFINEDINSTRUCTIONERRORHANDLER
	.ref IRQHandlerVectorTable

; Mode constants
MODE_MASK .set 0x1F
MODE_USR .set 0x10
MODE_FIQ .set 0x11
MODE_IRQ .set 0x12
MODE_SVC .set 0x13
MODE_ABT .set 0x17
MODE_UND .set 0x1B
MODE_SYS .set 0x1F

; Service constants
SERVICE_TICK .set 0
SERVICE_ENTERCORE .set 1
SERVICE_EXITCORE .set 2
SERVICE_YIELD .set 3
SERVICE_SWITCH .set 4
SERVICE_DISABLEINTERRUPTS .set 5
SERVICE_ENABLEINTERRUPTS .set 6

; Save general context macro - Should be used in SYS mode
macroPORT_SAVEGENERALCONTEXT .macro AUXILIARY_MODE
	; Pushes R0 into auxiliary mode stack
	CPS AUXILIARY_MODE
	STMFD SP!, {R0}
	CPS #MODE_SYS

	; Gets CURRENT_CONTEXT pointer
	LDR R0, _CURRENT_CONTEXT
	LDR R0, [R0]
	; Gets CURRENT_CONTEXT.PORT_CONTEXT element pointer
	LDR R0, [R0, #12]

	; Points context end
	ADD R0, R0, #204

	; Saves general purpose registers
	; Saves R1 to R14
	STMFD R0!, {R1-R14}
	; Pops R0 into R1 from auxiliary mode stack
	CPS AUXILIARY_MODE
	LDMFD SP!, {R1}
	CPS #MODE_SYS
	; Saves R0 from R1
	STMFD R0!, {R1}

	; Saves NEON/VFP coprocessor registers
	; Saves FPSCR and FPEXC
	VMRS R1, FPSCR
	VMRS R2, FPEXC
	STMFD R0!, {R1, R2}
	; Saves D0 to D15
	VSTMDB R0!, {D0-D15}
	.endm
; Save specific context macro
; R2 = LR
; R3 = CPSR
macroPORT_SAVESPECIFICCONTEXT .macro
	; Gets CURRENT_CONTEXT pointer
	LDR R0, _CURRENT_CONTEXT
	LDR R0, [R0]
	; Gets CURRENT_CONTEXT.PORT_CONTEXT element pointer
	LDR R0, [R0, #12]

	; Points other information start
	ADD R1, R0, #204

	; Nothing to save

	; Points CPSR and LR end
	ADD R1, R0, #8

	; Saves CPSR and LR
	STMFD R1!, {R2, R3}
	.endm
; Restore specific context macro
; R2 = LR
; R3 = CPSR
macroPORT_RESTORESPECIFICCONTEXT .macro
	; Gets CURRENT_CONTEXT pointer
	LDR R0, _CURRENT_CONTEXT
	LDR R0, [R0]
	; Gets CURRENT_CONTEXT.PORT_CONTEXT element pointer
	LDR R0, [R0, #12]

	; Points other information start
	ADD R1, R0, #204

	; Restores CURRENT_CONTEXT.PORT_CONTEXT.CONTEXT_IDENTIFIER element
	LDR R2, [R1, #0]
	MCR p15, #0, R2, c13, c0, #1 ; Sets CONTEXTIDR
	; Restores CURRENT_CONTEXT.PORT_CONTEXT.FLTRANSLATIONTABLE_ADDRESS element
	LDR R2, [R1, #4]
	MCR p15, #0, R2, c2, c0, #1 ; Sets TTBR1

	; Points context start
	ADD R1, R0, #0

	; Restores CPSR and LR
	LDMFD R1!, {R2, R3}
	.endm
; Restore general context macro - Should be used in SYS mode
macroPORT_RESTOREGENERALCONTEXT .macro AUXILIARY_MODE
	; Gets CURRENT_CONTEXT pointer
	LDR R0, _CURRENT_CONTEXT
	LDR R0, [R0]
	; Gets CURRENT_CONTEXT.PORT_CONTEXT element pointer
	LDR R0, [R0, #12]

	; Points context start
	ADD R1, R0, #8

	; Restores NEON/VFP coprocessor registers
	; Restores D0 to D15
	VLDMIA R1!, {D0-D15}
	; Restores FPSCR and FPEXC
	LDMFD R1!, {R2, R3}
	VMSR FPSCR, R2
	VMSR FPEXC, R3

	; Restores general registers
	; Restores R0 to R14
	LDMFD R1, {R0-LR}^
	.endm

	.text
	.state32

; IRQ handler method
	.align 4
	.def PORT_IRQHANDLER
PORT_IRQHANDLER: .asmfunc
	; Corrects LR
	SUB LR, LR, #4

	; Saves critical context
	STMFD SP!, {R0-R12,LR}

	; Loads IRQ handler vector table address
	LDR R0, _IRQHandlerVectorTable
	; Reads active IRQ
	LDR R1, ADDR_SIR_IRQ
	LDR R1, [R1]
	AND R1, R1, #INTC_SIR_IRQ_ACTIVEIRQ
	; Calculates ISR address
	LDR R0, [R0, R1, lsl #2]
	; Saves ISR address into stack (without moving SP)
	STR R0, [SP, #-4]

	; Restores critical context (without moving SP)
	LDMFD SP, {R0-R12,LR}

	; Loads default return address
	LDR LR, _PORT_IRQHANDLER_EXITISR_RESTORECONTEXT

	; Enters ISR using address in stack (without moving SP)
	LDR PC, [SP, #-4]
	.endasmfunc

; IRQ handler exit ISR and restore context method
	.align 4
PORT_IRQHANDLER_EXITISR_RESTORECONTEXT: .asmfunc
	; Enables new IRQ generation
	LDR R0, ADDR_CONTROL
	MOV R1, #INTC_CONTROL_NEWIRQAGR
	STR R1, [R0]
	DSB

	; Restores critical context
	LDMFD SP!, {R0-R12,LR}

	; Returns
	MOVS PC, LR
	.endasmfunc

; IRQ handler exit ISR and keep context method
	.align 4
PORT_IRQHANDLER_EXITISR_KEEPCONTEXT: .asmfunc
	; Saves temporary registers
	STMFD SP!, {R0-R1}

	; Enables new IRQ generation
	LDR R0, ADDR_CONTROL
	MOV R1, #INTC_CONTROL_NEWIRQAGR
	STR R1, [R0]
	DSB

	; Restores temporary registers
	LDMFD SP!, {R0-R1}

	; Discards critical context
	ADD SP, SP, #56

	; Returns
	MOVS PC, LR
	.endasmfunc

; FIQ handler method
	.align 4
	.def PORT_FIQHANDLER
PORT_FIQHANDLER: .asmfunc
	; Corrects LR
	SUB LR, LR, #4
	; Returns
	MOVS PC, LR
	.endasmfunc

; Abort handler method
	.align 4
	.def PORT_ABORTHANDLER
PORT_ABORTHANDLER: .asmfunc
	; Corrects LR
	SUB LR, LR, #8

	; Switches into SYS mode
	CPS #MODE_SYS
	; Saves general context
	macroPORT_SAVEGENERALCONTEXT #MODE_ABT
	; Saves specific context
	CPS #MODE_ABT
	MOV R2, LR
	MRS R3, SPSR
	CPS #MODE_SYS
	macroPORT_SAVESPECIFICCONTEXT
	; Switches into ABT mode
	CPS #MODE_ABT

	; Prepares handler parameters
	MOV R0, LR
	STMFD SP!, {R0}
	MRC p15, #0, R0, c6, c0, #0 ; Reads DFAR
	MRC p15, #0, R1, c5, c0, #0 ; Reads DFSR
	MRC p15, #0, R2, c6, c0, #2 ; Reads IFAR
	MRC p15, #0, R3, c5, c0, #1 ; Reads IFSR
	; Enters handler
	BL PORT_ABORTERRORHANDLER
	; Pops handler parameters
	LDMFD SP!, {R0}

	; Clears data and prefetch exception registers
	MOV R0, #0
	MCR p15, #0, R0, c6, c0, #0 ; Clears DFAR
	MCR p15, #0, R0, c5, c0, #0 ; Clears DFSR
	MCR p15, #0, R0, c6, c0, #2 ; Clears IFAR
	MCR p15, #0, R0, c5, c0, #1 ; Clears IFSR

	; Calls scheduler
	BL SCHEDULER

	; Switches into SYS mode
	CPS #MODE_SYS
	; Restores specific context
	macroPORT_RESTORESPECIFICCONTEXT
	CPS #MODE_ABT
	MOV LR, R2
	MSR SPSR_cxfs, R3
	CPS #MODE_SYS
	; Restores general context
	macroPORT_RESTOREGENERALCONTEXT #MODE_ABT
	; Switches into ABT mode
	CPS #MODE_ABT

	; Returns
	MOVS PC, LR
	.endasmfunc

; Undefined instruction handler method
	.align 4
	.def PORT_UNDEFINEDINSTRUCTIONHANDLER
PORT_UNDEFINEDINSTRUCTIONHANDLER: .asmfunc
	; Corrects LR
	SUB LR, LR, #4

	; Switches into SYS mode
	CPS #MODE_SYS
	; Saves general context
	macroPORT_SAVEGENERALCONTEXT #MODE_UND
	; Saves specific context
	CPS #MODE_UND
	MOV R2, LR
	MRS R3, SPSR
	CPS #MODE_SYS
	macroPORT_SAVESPECIFICCONTEXT
	; Switches into UND mode
	CPS #MODE_UND

	; Prepares handler parameters
	MOV R0, LR
	; Enters handler
	BL PORT_UNDEFINEDINSTRUCTIONERRORHANDLER

	; Calls scheduler
	BL SCHEDULER

	; Switches into SYS mode
	CPS #MODE_SYS
	; Restores specific context
	macroPORT_RESTORESPECIFICCONTEXT
	CPS #MODE_UND
	MOV LR, R2
	MSR SPSR_cxfs, R3
	CPS #MODE_SYS
	; Restores general context
	macroPORT_RESTOREGENERALCONTEXT #MODE_UND
	; Switches into UND mode
	CPS #MODE_UND

	; Returns
	MOVS PC, LR
	.endasmfunc

; SVC handler method
	.align 4
	.def PORT_SVCHANDLER
PORT_SVCHANDLER: .asmfunc
	; Reserves stack space for handler address
	SUB SP, SP, #4

	; Pushes service number register
	STMFD SP!, {R0}

	; Reads SVC number
	SUB R0, LR, #4 ; Calculates SVC instruction address
	LDR R0, [R0] ; Reads SVC instruction
	BIC R0, R0, #0xFF000000 ; Masks SVC number

	; Handles tick service
	CMP R0, #SERVICE_TICK
	LDREQ R0, _PORT_SVCHANDLER_TICK
	STREQ R0, [SP, #4]
	LDMFDEQ SP!, {R0, PC}

	; Handles enter core service
	CMP R0, #SERVICE_ENTERCORE
	LDREQ R0, _PORT_SVCHANDLER_ENTERCORE
	STREQ R0, [SP, #4]
	LDMFDEQ SP!, {R0, PC}

	; Handles exit core service
	CMP R0, #SERVICE_EXITCORE
	LDREQ R0, _PORT_SVCHANDLER_EXITCORE
	STREQ R0, [SP, #4]
	LDMFDEQ SP!, {R0, PC}

	; Handles yield service
	CMP R0, #SERVICE_YIELD
	LDREQ R0, _PORT_SVCHANDLER_YIELD
	STREQ R0, [SP, #4]
	LDMFDEQ SP!, {R0, PC}

	; Handles switch service
	CMP R0, #SERVICE_SWITCH
	LDREQ R0, _PORT_SVCHANDLER_SWITCH
	STREQ R0, [SP, #4]
	LDMFDEQ SP!, {R0, PC}

	; Handles disable interrupts service
	CMP R0, #SERVICE_DISABLEINTERRUPTS
	LDREQ R0, _PORT_SVCHANDLER_DISABLEINTERRUPTS
	STREQ R0, [SP, #4]
	LDMFDEQ SP!, {R0, PC}

	; Handles enable interrupts service
	CMP R0, #SERVICE_ENABLEINTERRUPTS
	LDREQ R0, _PORT_SVCHANDLER_ENABLEINTERRUPTS
	STREQ R0, [SP, #4]
	LDMFDEQ SP!, {R0, PC}

	; Invalid service called
PORT_SVCHANDLER_INFINITELOOP:
	B PORT_SVCHANDLER_INFINITELOOP
	.endasmfunc

; Tick service handler
	.align 4
PORT_SVCHANDLER_TICK: .asmfunc
	; Switches into SYS mode
	CPS #MODE_SYS
	; Saves general context
	macroPORT_SAVEGENERALCONTEXT #MODE_SVC
	; Saves specific context
	CPS #MODE_IRQ
	LDR R2, [SP, #52] ; LR is read from IRQ stack
	MRS R3, SPSR
	CPS #MODE_SYS
	macroPORT_SAVESPECIFICCONTEXT
	; Switches into SVC mode
	CPS #MODE_SVC

	; Calls tick
	STMFD SP!, {LR}
	BL TICK
	LDMFD SP!, {LR}

	; Switches into SYS mode
	CPS #MODE_SYS
	; Restores specific context
	macroPORT_RESTORESPECIFICCONTEXT
	CPS #MODE_IRQ
	MOV LR, R2
	MSR SPSR_cxfs, R3
	CPS #MODE_SYS
	; Restores general context
	macroPORT_RESTOREGENERALCONTEXT #MODE_SVC
	; Switches into SVC mode
	CPS #MODE_SVC

	; Returns
	MOVS PC, LR
	.endasmfunc

; Enter core service handler
	.align 4
PORT_SVCHANDLER_ENTERCORE: .asmfunc
	; Pushes temporary registers
	STMFD SP!, {R0-R1}

	; Should disable interrupts and, if not in privileged mode,
	; enter privileged mode

	; If in USR mode, disables interrupts and enters SYS mode
	MRS R0, SPSR
	AND R1, R0, #MODE_MASK
	CMP R1, #MODE_USR
	ORR R0, R0, #0xC0
	BICEQ R0, R0, #MODE_MASK
	ORREQ R0, R0, #MODE_SYS
	MSR SPSR_cxfs, R0

	; Pops temporary registers
	LDMFD SP!, {R0-R1}

	; Returns
	MOVS PC, LR
	.endasmfunc

; Exit core service handler
	.align 4
PORT_SVCHANDLER_EXITCORE: .asmfunc
	; Pushes temporary registers
	STMFD SP!, {R0-R2}

	; Should enable interrupts and, if in privileged mode and
	; not in system partition context, exit privileged mode

	; Gets CURRENT_CONTEXT pointer
	LDR R2, _CURRENT_CONTEXT
	LDR R2, [R2]
	; Gets CURRENT_CONTEXT.SYSTEM_PARTITION_CONTEXT element
	LDRB R2, [R2, #20]
	CMP R2, #0
	MOVEQ R2, #MODE_USR ; If it's not a system partition context, enter USR mode
	MOVNE R2, #MODE_SYS ; If it's a system partition context, stay in SYS mode

	; If in SYS mode, enables interrupts and enters USR or SYS mode
	MRS R0, SPSR
	AND R1, R0, #MODE_MASK
	CMP R1, #MODE_SYS
	BIC R0, R0, #0xC0
	BICEQ R0, R0, #MODE_MASK
	ORREQ R0, R0, R2
	MSR SPSR_cxfs, R0

	; Pops temporary registers
	LDMFD SP!, {R0-R2}

	; Returns
	MOVS PC, LR
	.endasmfunc

; Yield service handler
	.align 4
PORT_SVCHANDLER_YIELD: .asmfunc
	; Switches into SYS mode
	CPS #MODE_SYS
	; Saves general context
	macroPORT_SAVEGENERALCONTEXT #MODE_SVC
	; Saves specific context
	MOV R2, LR
	CPS #MODE_SVC
	MRS R3, SPSR
	CPS #MODE_SYS
	macroPORT_SAVESPECIFICCONTEXT
	; Switches into SVC mode
	CPS #MODE_SVC

	; Calls scheduler
	BL SCHEDULER

	; Switches into SYS mode
	CPS #MODE_SYS
	; Restores specific context
	macroPORT_RESTORESPECIFICCONTEXT
	CPS #MODE_SVC
	MOV LR, R2
	MSR SPSR_cxfs, R3
	CPS #MODE_SYS
	; Restores general context
	macroPORT_RESTOREGENERALCONTEXT #MODE_SVC
	; Switches into SVC mode
	CPS #MODE_SVC

	; Returns
	MOVS PC, LR
	.endasmfunc

; Switch service handler
	.align 4
PORT_SVCHANDLER_SWITCH: .asmfunc
	; Switches into SYS mode
	CPS #MODE_SYS
	; Restores specific context
	macroPORT_RESTORESPECIFICCONTEXT
	CPS #MODE_SVC
	MOV LR, R2
	MSR SPSR_cxfs, R3
	CPS #MODE_SYS
	; Restores general context
	macroPORT_RESTOREGENERALCONTEXT #MODE_SVC
	; Switches into SVC mode
	CPS #MODE_SVC

	; Returns
	MOVS PC, LR
	.endasmfunc

; Disable interrupts service handler
	.align 4
PORT_SVCHANDLER_DISABLEINTERRUPTS: .asmfunc
	; Pushes temporary registers
	STMFD SP!, {R0}

	; Disables interrupts
	MRS R0, SPSR
	ORR R0, R0, #0xC0
	MSR SPSR_cxfs, R0

	; Pops temporary registers
	LDMFD SP!, {R0}

	; Returns
	MOVS PC, LR
	.endasmfunc

; Enable interrupts service handler
	.align 4
PORT_SVCHANDLER_ENABLEINTERRUPTS: .asmfunc
	; Pushes temporary registers
	STMFD SP!, {R0}

	; Enables interrupts
	MRS R0, SPSR
	BIC R0, R0, #0xC0
	MSR SPSR_cxfs, R0

	; Pops temporary registers
	LDMFD SP!, {R0}

	; Returns
	MOVS PC, LR
	.endasmfunc

; Get call address - Should return the address from which the current function was called, previously stored by PORT_PREPARECALLADDRESS
	.align 4
	.def PORT_GETCALLADDRESS
PORT_GETCALLADDRESS: .asmfunc
	; Pops call address
	LDMFD SP!, {R0}
	; Corrects call address
	SUB R0, R0, #4
	; Returns
	MOV PC, LR
	.endasmfunc

; Tick method
	.align 4
	.def PORT_TICK
PORT_TICK: .asmfunc
	; Pushes temporary registers
	STMFD SP!, {R0-R1}

	; Clears DMTimer2 interrupt status
	MOVW R0, #0x0028
	MOVT R0, #0x4804
	MOV R1, #2
	STR R1, [R0]

	; Pops temporary registers
	LDMFD SP!, {R0-R1}

	; Calls tick service
	SVC #SERVICE_TICK

	; Exits IRQ handler and keeps context
	B PORT_IRQHANDLER_EXITISR_KEEPCONTEXT
	.endasmfunc

; Enter core method
	.align 4
	.def PORT_ENTERCORE
PORT_ENTERCORE: .asmfunc
	; Enters
	STMFD SP!, {LR}
	; Calls enter core service
	SVC #SERVICE_ENTERCORE
	; Returns
	LDMFD SP!, {PC}
	.endasmfunc

; Exit core method
	.align 4
	.def PORT_EXITCORE
PORT_EXITCORE: .asmfunc
	; Enters
	STMFD SP!, {LR}
	; Calls exit core service
	SVC #SERVICE_EXITCORE
	; Returns
	LDMFD SP!, {PC}
	.endasmfunc

; Yield method
	.align 4
	.def PORT_YIELD
PORT_YIELD: .asmfunc
	; Calls yield service
	SVC #SERVICE_YIELD
	; Enters infinite loop (yield service shouldn't return)
	B PORT_INFINITELOOP
	.endasmfunc

; Switch method
	.align 4
	.def PORT_SWITCH
PORT_SWITCH: .asmfunc
	; Calls switch service
	SVC #SERVICE_SWITCH
	; Enters infinite loop (switch service shouldn't return)
	B PORT_INFINITELOOP
	.endasmfunc

; Start scheduler method
	.align 4
	.def PORT_STARTSCHEDULER
PORT_STARTSCHEDULER: .asmfunc
	; Calls yield service
	SVC #SERVICE_YIELD
	; Enters infinite loop (start scheduler service shouldn't return)
	B PORT_INFINITELOOP
	.endasmfunc

; Disable interrupts method
	.align 4
	.def PORT_DISABLEINTERRUPTS
PORT_DISABLEINTERRUPTS: .asmfunc
	; Enters
	STMFD SP!, {LR}
	; Calls disable interrupts service
	SVC #SERVICE_DISABLEINTERRUPTS
	; Returns
	LDMFD SP!, {PC}
	.endasmfunc

; Enable interrupts method
	.align 4
	.def PORT_ENABLEINTERRUPTS
PORT_ENABLEINTERRUPTS: .asmfunc
	; Enters
	STMFD SP!, {LR}
	; Calls enable interrupts service
	SVC #SERVICE_ENABLEINTERRUPTS
	; Returns
	LDMFD SP!, {PC}
	.endasmfunc

; Exit privileged mode method
	.align 4
	.def PORT_EXITPRIVILEGEDMODE
PORT_EXITPRIVILEGEDMODE: .asmfunc
	; Enters
	STMFD SP!, {LR}
	; Switches into USR mode
	CPS #MODE_USR
	; Returns
	LDMFD SP!, {PC}
	.endasmfunc

; Infinite loop
PORT_INFINITELOOP: .asmfunc
	; Loops forever
	B PORT_INFINITELOOP
	.endasmfunc

; Local label addresses
_PORT_SVCHANDLER_TICK .word PORT_SVCHANDLER_TICK
_PORT_SVCHANDLER_ENTERCORE .word PORT_SVCHANDLER_ENTERCORE
_PORT_SVCHANDLER_EXITCORE .word PORT_SVCHANDLER_EXITCORE
_PORT_SVCHANDLER_YIELD .word PORT_SVCHANDLER_YIELD
_PORT_SVCHANDLER_SWITCH .word PORT_SVCHANDLER_SWITCH
_PORT_SVCHANDLER_DISABLEINTERRUPTS .word PORT_SVCHANDLER_DISABLEINTERRUPTS
_PORT_SVCHANDLER_ENABLEINTERRUPTS .word PORT_SVCHANDLER_ENABLEINTERRUPTS

; Variables and functions addresses
_CURRENT_CONTEXT .word CURRENT_CONTEXT
_PORT_IRQHANDLER_EXITISR_RESTORECONTEXT .word PORT_IRQHANDLER_EXITISR_RESTORECONTEXT
_PORT_IRQHANDLER_EXITISR_KEEPCONTEXT .word PORT_IRQHANDLER_EXITISR_KEEPCONTEXT
_PORT_UNDEFINEDINSTRUCTIONERRORHANDLER .word PORT_UNDEFINEDINSTRUCTIONERRORHANDLER
_IRQHandlerVectorTable .word IRQHandlerVectorTable

; Interrupt controller register constants
ADDR_CONTROL .word SOC_AINTC_REGS + INTC_CONTROL
ADDR_SIR_IRQ .word SOC_AINTC_REGS + INTC_SIR_IRQ
ADDR_SIR_FIQ .word SOC_AINTC_REGS + INTC_SIR_FIQ

	.end
