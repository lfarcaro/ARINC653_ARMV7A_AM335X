#ifndef ARINC653_PORT_TYPES_H_
#define ARINC653_PORT_TYPES_H_

// Includes
#include "hw_types.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h"

// Definitions
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#ifndef null
#define null 0
#endif

// Common types
typedef unsigned char portBOOLEAN;
typedef unsigned char portCHARACTER;
typedef unsigned char portBYTE;
typedef signed char portINT8;
typedef unsigned char portUINT8;
typedef signed short portINT16;
typedef unsigned short portUINT16;
typedef signed int portINT32;
typedef unsigned int portUINT32;
typedef signed long long int portINT64;
typedef unsigned long long int portUINT64;

// Base type (architecture base length integer)
typedef signed int portINTBASE;
typedef unsigned int portUINTBASE;

// Pointer unsigned integer (pointer length unsigned integer)
typedef unsigned int portUINTPOINTER;

// Address
typedef void *portADDRESS;

// Stack row
typedef unsigned int portSTACKROW;

// Parameter
typedef unsigned int portPARAMETER;

// Size
typedef unsigned int portSIZE;

// Index
typedef unsigned int portINDEX;

// Identifier
typedef unsigned int portIDENTIFIER;
#define MAXIMUM_IDENTIFIER_VALUE 0xFFFFFFFF

// Port context
struct _PORT_CONTEXT_TYPE {
	// Run status registers
	portADDRESS PC; // R15
	portUINT32 CPSR;
	// NEON/VFP coprocessor registers
	portUINT64 D0;
	portUINT64 D1;
	portUINT64 D2;
	portUINT64 D3;
	portUINT64 D4;
	portUINT64 D5;
	portUINT64 D6;
	portUINT64 D7;
	portUINT64 D8;
	portUINT64 D9;
	portUINT64 D10;
	portUINT64 D11;
	portUINT64 D12;
	portUINT64 D13;
	portUINT64 D14;
	portUINT64 D15;
	portUINT64 D16;
	portUINT64 D17;
	portUINT64 D18;
	portUINT64 D19;
	portUINT64 D20;
	portUINT64 D21;
	portUINT64 D22;
	portUINT64 D23;
	portUINT64 D24;
	portUINT64 D25;
	portUINT64 D26;
	portUINT64 D27;
	portUINT64 D28;
	portUINT64 D29;
	portUINT64 D30;
	portUINT64 D31;
	portUINT32 FPSCR;
	portUINT32 FPEXC;
	// General registers
	portUINT32 R0;
	portUINT32 R1;
	portUINT32 R2;
	portUINT32 R3;
	portUINT32 R4;
	portUINT32 R5;
	portUINT32 R6;
	portUINT32 R7;
	portUINT32 R8;
	portUINT32 R9;
	portUINT32 R10;
	portUINT32 R11;
	portUINT32 R12;
	portUINT32 R13; // SP
	portUINT32 R14; // LR
	// Other information
	portUINT32 CONTEXT_IDENTIFIER;
	portADDRESS FLTRANSLATIONTABLE_ADDRESS;
};
typedef struct _PORT_CONTEXT_TYPE PORT_CONTEXT_TYPE;

// Port module configuration connection table sampling port mapping pseudo partition
struct _PORT_MODULE_CONFIGURATION_CONNECTIONTABLE_SAMPLINGPORTMAPPING_PSEUDOPARTITION_TYPE {
	portCHARACTER PHYSICAL_ADDRESS[12];
};
typedef struct _PORT_MODULE_CONFIGURATION_CONNECTIONTABLE_SAMPLINGPORTMAPPING_PSEUDOPARTITION_TYPE PORT_MODULE_CONFIGURATION_CONNECTIONTABLE_SAMPLINGPORTMAPPING_PSEUDOPARTITION_TYPE;

// Port module configuration connection table queuing port mapping pseudo partition
struct _PORT_MODULE_CONFIGURATION_CONNECTIONTABLE_QUEUINGPORTMAPPING_PSEUDOPARTITION_TYPE {
	portCHARACTER PHYSICAL_ADDRESS[12];
};
typedef struct _PORT_MODULE_CONFIGURATION_CONNECTIONTABLE_QUEUINGPORTMAPPING_PSEUDOPARTITION_TYPE PORT_MODULE_CONFIGURATION_CONNECTIONTABLE_QUEUINGPORTMAPPING_PSEUDOPARTITION_TYPE;

// Port module configuration
struct _PORT_MODULE_CONFIGURATION_TYPE {
	portADDRESS VECTORTABLE_ADDRESS;
	portSIZE VECTORTABLE_SIZE;
	portADDRESS MODULE_FLTRANSLATIONTABLE_ADDRESS;
	portSIZE MODULE_FLTRANSLATIONTABLE_SIZE;
	portADDRESS MODULE_SLTRANSLATIONTABLE_ADDRESS;
	portSIZE MODULE_SLTRANSLATIONTABLE_SIZE;
	portUINTBASE *MODULE_GLOBAL_FLTRANSLATIONTABLE_ADDRESS; // Filled by the operating system
	portCHARACTER MODULE_NETWORK_PHYSICAL_ADDRESS[10];
};
typedef struct _PORT_MODULE_CONFIGURATION_TYPE PORT_MODULE_CONFIGURATION_TYPE;

// Port system configuration
struct _PORT_SYSTEM_CONFIGURATION_TYPE {
	portADDRESS STACK_AREA_START_ADDRESS;
	portADDRESS STACK_AREA_END_ADDRESS;
	portADDRESS PERIPHERALS_ADDRESS;
	portSIZE PERIPHERALS_SIZE;
	portADDRESS SYSTEM_BOOT_ADDRESS;
	portSIZE SYSTEM_BOOT_SIZE;
};
typedef struct _PORT_SYSTEM_CONFIGURATION_TYPE PORT_SYSTEM_CONFIGURATION_TYPE;

#endif
