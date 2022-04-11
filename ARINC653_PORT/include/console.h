#ifndef CONSOLE_H_
#define CONSOLE_H_

// Includes
#include <stdarg.h>
#include "arinc653_port_types.h"
#include "beaglebone.h"
#include "uart_irda_cir.h"
#include "consoleUtils.h"
#include "uartStdio.h"

// Console UART base address
#define CONSOLE_UARTBASEADDRESS SOC_UART_0_REGS

// Console UART baud rate
#define CONSOLE_UARTBAUDRATE 230400

// Console UART input clock
#define CONSOLE_UARTINPUTCLOCK 48000000

// Startup method
unsigned char CONSOLE_STARTUP(void);

// Byte writing method
unsigned char CONSOLE_WRITE_BYTE(unsigned char BYTE);

// Byte writing with count-out method
unsigned char CONSOLE_WRITE_BYTE_COUNTOUT(unsigned char BYTE, int COUNT);

// Byte reading method
unsigned char CONSOLE_READ_BYTE(unsigned char *BYTE);

// Byte reading with count-out method
unsigned char CONSOLE_READ_BYTE_COUNTOUT(unsigned char *BYTE, int COUNT);

// Buffer writing method
unsigned int CONSOLE_WRITE_BUFFER(unsigned char *BUFFER, unsigned int LENGTH);

// Buffer reading method
unsigned int CONSOLE_READ_BUFFER(unsigned char *BUFFER, unsigned int LENGTH);

// String printing method
void CONSOLE_PUTS(const char *STRING, unsigned int LENGTH);

// Number printing method
unsigned int CONSOLE_PRINTN(int VALUE, unsigned char BASE, unsigned char SIGNED, unsigned char UPPERCASE, unsigned char PADDING_CHARACTER, int PADDING_LENGTH);

// Formatted printing method
void CONSOLE_PRINTF(const char *STRING, ...);

#endif
