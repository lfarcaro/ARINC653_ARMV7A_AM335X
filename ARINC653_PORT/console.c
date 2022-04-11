// Includes
#include "console.h"

// Number printing digits
static const char * const PRINTN_DIGITS_LOWER = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char * const PRINTN_DIGITS_UPPER = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Startup method
unsigned char CONSOLE_STARTUP(void) {

    // Configures the console UART clock
    UART0ModuleClkConfig();

    // Configures the console UART pins
    UARTPinMuxSetup(0);

    // Resets the console UART peripheral
    UARTModuleReset(CONSOLE_UARTBASEADDRESS);

    // Ensures DMA is disabled
    UARTFIFOConfig(CONSOLE_UARTBASEADDRESS, UART_FIFO_CONFIG(UART_TRIG_LVL_GRANULARITY_1, UART_TRIG_LVL_GRANULARITY_1, 1, 1, 1, 1, UART_DMA_EN_PATH_SCR, UART_DMA_MODE_0_ENABLE));

    // Programs the divisor latches according to baud rate
    UARTDivisorLatchWrite(CONSOLE_UARTBASEADDRESS, UARTDivisorValCompute(CONSOLE_UARTINPUTCLOCK, CONSOLE_UARTBAUDRATE, UART16x_OPER_MODE, UART_MIR_OVERSAMPLING_RATE_42));

    // Sets default line characteristics
    UARTRegConfigModeEnable(CONSOLE_UARTBASEADDRESS, UART_REG_CONFIG_MODE_B);

    // Sets custom line characteristics
    UARTLineCharacConfig(CONSOLE_UARTBASEADDRESS, (UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1), UART_PARITY_NONE);

    // Disables access to divisor latches
    UARTDivisorLatchDisable(CONSOLE_UARTBASEADDRESS);

    // Disables break control
    UARTBreakCtl(CONSOLE_UARTBASEADDRESS, UART_BREAK_COND_DISABLE);

    // Switches to UART16x operating mode
    UARTOperatingModeSelect(CONSOLE_UARTBASEADDRESS, UART16x_OPER_MODE);
	return 1;
}

// Byte writing method
unsigned char CONSOLE_WRITE_BYTE(unsigned char BYTE) {
	// Waits until transmission register is available
	if ((HWREG(CONSOLE_UARTBASEADDRESS + UART_LSR) & UART_LSR_TX_SR_E) == 0) {
		return 0;
	}
	// Writes byte to transmission register
	HWREG(CONSOLE_UARTBASEADDRESS + UART_THR) = BYTE;
	return 1;
}

// Byte writing with count-out method
unsigned char CONSOLE_WRITE_BYTE_COUNTOUT(unsigned char BYTE, int COUNT) {
	while (!CONSOLE_WRITE_BYTE(BYTE)) {
		if (COUNT == 0) {
			return 0;
		} else if (COUNT != -1) {
			COUNT--;
		}
	}
	return 1;
}

// Byte reading method
unsigned char CONSOLE_READ_BYTE(unsigned char *BYTE) {
	// Verifies if a byte is currently available
	if ((HWREG(CONSOLE_UARTBASEADDRESS + UART_LSR) & UART_LSR_RX_FIFO_E) == 0) {
		return 0;
	}
	// Reads byte from reception register
	*BYTE = ((unsigned char) HWREG(CONSOLE_UARTBASEADDRESS + UART_RHR));
	return 1;
}

// Byte reading with count-out method
unsigned char CONSOLE_READ_BYTE_COUNTOUT(unsigned char *BYTE, int COUNT) {
	while (!CONSOLE_READ_BYTE(BYTE)) {
		if (COUNT == 0) {
			return 0;
		} else if (COUNT != -1) {
			COUNT--;
		}
	}
	return 1;
}

// Buffer writing method
unsigned int CONSOLE_WRITE_BUFFER(unsigned char *BUFFER, unsigned int LENGTH) {
	unsigned int INDEX;
	// Writes buffer bytes
	for (INDEX = 0; INDEX < LENGTH; INDEX++) {
		CONSOLE_WRITE_BYTE_COUNTOUT(BUFFER[INDEX], -1);
	}
	// Waits until transmission is finished
	while ((HWREG(CONSOLE_UARTBASEADDRESS + UART_LSR) & (UART_LSR_TX_SR_E | UART_LSR_TX_FIFO_E)) != (UART_LSR_TX_SR_E | UART_LSR_TX_FIFO_E));
	return LENGTH;
}

// Buffer reading method
unsigned int CONSOLE_READ_BUFFER(unsigned char *BUFFER, unsigned int LENGTH) {
	unsigned int INDEX;
	for (INDEX = 0; INDEX < LENGTH; INDEX++) {
		if (!CONSOLE_READ_BYTE(&BUFFER[INDEX])) {
			break;
		}
	}
	return INDEX;
}

// String printing method similar to 'puts'.
void CONSOLE_PUTS(const char *STRING, unsigned int LENGTH) {
	CONSOLE_WRITE_BUFFER((unsigned char *) STRING, LENGTH);
}

// Number printing method
unsigned int CONSOLE_PRINTN(int VALUE, unsigned char BASE, unsigned char SIGNED, unsigned char UPPERCASE, unsigned char PADDING_CHARACTER, int PADDING_LENGTH) {
	unsigned char NEGATIVE;
	unsigned int DIMENSION, INDEX;
	unsigned char BUFFER[32];

	// Standardizes value and stores negative indicator
	NEGATIVE = (SIGNED && (VALUE < 0) ? 1 : 0);
	if (NEGATIVE) {
		VALUE = -VALUE;
	}

	// Calculates dimension according to the number's base
	DIMENSION = 1;
	while (((DIMENSION * BASE) <= VALUE) && (((DIMENSION * BASE) / BASE) == DIMENSION)) {
		DIMENSION *= BASE;
		if (PADDING_LENGTH > 0) {
			PADDING_LENGTH--;
		}
	}

	// Eliminates a padding character for the signal
	if (NEGATIVE && (PADDING_LENGTH > 0)) {
		PADDING_LENGTH--;
	}

	// Initializes index
	INDEX = 0;

	// For zero-padded negative numbers, the first character is the signal
	if(NEGATIVE && (PADDING_CHARACTER == '0')) {
		BUFFER[INDEX++] = '-';
		NEGATIVE = 0;
	}

	// Appends padding characters
	if ((PADDING_LENGTH > 1) && (PADDING_LENGTH < 32)) {
		while (PADDING_LENGTH > 0) {
			BUFFER[INDEX++] = PADDING_CHARACTER;
			PADDING_LENGTH--;
		}
	}

	// Appends signal for negative numbers
	if (NEGATIVE) {
		BUFFER[INDEX++] = '-';
	}

	// Converts the value to string
	while (DIMENSION != 0) {
		BUFFER[INDEX++] = (UPPERCASE ? PRINTN_DIGITS_UPPER[(VALUE / DIMENSION) % BASE] : PRINTN_DIGITS_LOWER[(VALUE / DIMENSION) % BASE]);
		DIMENSION /= BASE;
	}

	// Writes the string
	return CONSOLE_WRITE_BUFFER(BUFFER, INDEX);
}

// Formatted printing method similar to 'printf'.
// Supports the following fields:
// %c - Character field
// %d - Signed decimal integer field
// %u - Unsigned decimal integer field
// %x - Hexadecimal value using lower case letters
// %X - Hexadecimal value using upper case letters
// %p - Pointer field in hexadecimal format
// %s - String field
// %% - The % character itself
// The % indicator can be immediately followed by a decimal number indicating the minimum length of the printed string,
// which is left-padded with spaces by default or with zeroes if the '0' character immediately succeeds the % indicator.
void CONSOLE_PRINTF(const char *FORMAT, ...) {
	unsigned char PADDING_CHARACTER;
	int PADDING_LENGTH;
	unsigned int VALUE, LENGTH;
	char *VALUE_STRING;
	va_list varargs;

	// Starts varargs processing
	va_start(varargs, FORMAT);

	// Null-terminated format string iteration loop
	while (*FORMAT) {

		// Gets the contiguous section length
		LENGTH = 0;
		while ((FORMAT[LENGTH] != '%') && (FORMAT[LENGTH] != 0)) {
			LENGTH++;
		}

		// Writes contiguous section
		CONSOLE_WRITE_BUFFER((unsigned char *) FORMAT, LENGTH);

		// Skips contiguous section
		FORMAT += LENGTH;

		// Verifies field indicator
		if (*FORMAT == '%') {

			// Skips field indicator
			FORMAT++;

			// Determines padding length and character
			PADDING_LENGTH = 0;
			if (*FORMAT == '0') {
				FORMAT++;
				PADDING_CHARACTER = '0';
			} else {
				PADDING_CHARACTER = ' ';
			}
			while ((*FORMAT >= '0') && (*FORMAT <= '9')) {
				PADDING_LENGTH = (PADDING_LENGTH * 10) + (*FORMAT++ - '0');
			}

			// Verifies the field character
			switch (*FORMAT++) {
				// Character field
				case 'c': {
					// Gets the varargs value
					VALUE = va_arg(varargs, unsigned int);
					// Writes padding characters
					while (PADDING_LENGTH-- > 1) {
						CONSOLE_WRITE_BUFFER(&PADDING_CHARACTER, 1);
					}
					// Writes the character
					CONSOLE_WRITE_BUFFER((unsigned char *) &VALUE, 1);
					break;
				}
				// Decimal integer field
				case 'd': {
					// Gets the varargs value
					VALUE = va_arg(varargs, unsigned int);
					// Prints value as signed decimal
					CONSOLE_PRINTN(VALUE, 10, true, false, PADDING_CHARACTER, PADDING_LENGTH);
					break;
				}
				// Unsigned decimal integer field
				case 'u': {
					// Gets the varargs value
					VALUE = va_arg(varargs, unsigned int);
					// Prints value as unsigned decimal
					CONSOLE_PRINTN(VALUE, 10, false, false, PADDING_CHARACTER, PADDING_LENGTH);
					break;
				}
				// String field
				case 's': {
					// Gets the varargs value
					VALUE_STRING = va_arg(varargs, char *);
					// Determines the length of the string
					LENGTH = 0;
					while (VALUE_STRING[LENGTH] != 0) {
						LENGTH++;
					}
					// Writes padding characters
					while (PADDING_LENGTH-- > LENGTH) {
						CONSOLE_WRITE_BUFFER(&PADDING_CHARACTER, 1);
					}
					// Writes the string
					CONSOLE_WRITE_BUFFER((unsigned char *) VALUE_STRING, LENGTH);
					break;
				}
				// Hexadecimal integer and pointer fields
				case 'x': {
					// Gets the varargs value
					VALUE = va_arg(varargs, unsigned int);
					// Prints value as unsigned lower-case hexadecimal
					CONSOLE_PRINTN(VALUE, 16, false, false, PADDING_CHARACTER, PADDING_LENGTH);
					break;
				}
				case 'X': {
					// Gets the varargs value
					VALUE = va_arg(varargs, unsigned int);
					// Prints value as unsigned upper-case hexadecimal
					CONSOLE_PRINTN(VALUE, 16, false, true, PADDING_CHARACTER, PADDING_LENGTH);
					break;
				}
				case 'p': {
					// Gets the varargs value
					VALUE = va_arg(varargs, unsigned int);
					// Prints value as unsigned upper-case hexadecimal
					CONSOLE_PRINTN(VALUE, 16, false, true, PADDING_CHARACTER, PADDING_LENGTH);
					break;
				}
				// Field indicator escaping
				case '%': {
					// Writes the % character
					CONSOLE_WRITE_BUFFER((unsigned char *) (FORMAT - 1), 1);
					break;
				}
				// Invalid fields
				default: {
					// Writes error indicator
					CONSOLE_WRITE_BUFFER((unsigned char *) "<ERROR>", 7);
					break;
				}
			}
		}
	}

	// Finishes varargs processing
	va_end(varargs);
}
