// Partition include
#include "partition1_partition.h"

// SNIPPET_START GLOBAL_INCLUDES
// Test include
#include "test.h"
// SNIPPET_END

// -------------------- PARTITION DESCRIPTION START --------------------
// SNIPPET_START PARTITION1_PARTITION_DESCRIPTION
// SNIPPET_END
// -------------------- PARTITION DESCRIPTION END --------------------

// SNIPPET_START PARTITION1_PARTITION_INCLUDES
// SNIPPET_END

// PROCESS1 process identifier
static PROCESS_ID_TYPE PROCESS1_PROCESS_ID;

// PROCESS2 process identifier
static PROCESS_ID_TYPE PROCESS2_PROCESS_ID;

// SNIPPET_START PARTITION1_PARTITION_GLOBAL_DEFINITIONS
// SNIPPET_END

// SNIPPET_START PARTITION1_PARTITION_GLOBAL_VARIABLES
// SNIPPET_END

// SNIPPET_START PARTITION1_PARTITION_FUNCTIONS
// SNIPPET_END

// -------------------- PROCESS1 PROCESS START --------------------

// SNIPPET_START PARTITION1_PARTITION_PROCESS1_PROCESS_GLOBAL_VARIABLES
// SNIPPET_END

// SNIPPET_START PARTITION1_PARTITION_PROCESS1_PROCESS_FUNCTIONS
// SNIPPET_END

// PROCESS1 process
static void PROCESS1(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START PARTITION1_PARTITION_PROCESS1_PROCESS_VARIABLES
	portUINT32 UI32_COUNTER1 = 0;
	// SNIPPET_END

	// SNIPPET_START PARTITION1_PARTITION_PROCESS1_PROCESS_INITIALIZATION
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START PARTITION1_PARTITION_PROCESS1_PROCESS_BEFORE_SUSPENSION_CODE
		// Tracks process execution
		CONSOLE_PRINTF("%u PC P1 p1 Js\r\n", TRACK_READ());

		// Sets LEDs pattern for some time
		for (UI32_COUNTER1 = 0; UI32_COUNTER1 < TEST_COUNTER_SMALL; UI32_COUNTER1++) {
			TEST_LED1_ON();
			TEST_LED2_OFF();
		}

		// Tracks process execution
		CONSOLE_PRINTF("%u PC P1 p1 Jf\r\n", TRACK_READ());
		// SNIPPET_END

		// Waits for next period
		PERIODIC_WAIT(&RETURN_CODE);
		if (RETURN_CODE != NO_ERROR) {
			// SNIPPET_START PERIODIC_WAIT_ERROR_HANDLING_CODE
			while (true) {
			}
			// SNIPPET_END
		}

		// SNIPPET_START PARTITION1_PARTITION_PROCESS1_PROCESS_AFTER_SUSPENSION_CODE
		// SNIPPET_END
	}
}

// PROCESS1 process attributes
static PROCESS_ATTRIBUTE_TYPE PROCESS1_PROCESS_ATTRIBUTE = {
/*NAME*/(PROCESS_NAME_TYPE) "PROCESS1",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &PROCESS1,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*BASE_PRIORITY*/(PRIORITY_TYPE) 30,
/*PERIOD*/(SYSTEM_TIME_TYPE) 500000000,
/*TIME_CAPACITY*/(SYSTEM_TIME_TYPE) 250000000,
/*DEADLINE*/(DEADLINE_TYPE) HARD };

// -------------------- PROCESS1 PROCESS END --------------------

// -------------------- PROCESS2 PROCESS START --------------------

// SNIPPET_START PARTITION1_PARTITION_PROCESS2_PROCESS_GLOBAL_VARIABLES
// SNIPPET_END

// SNIPPET_START PARTITION1_PARTITION_PROCESS2_PROCESS_FUNCTIONS
// SNIPPET_END

// PROCESS2 process
static void PROCESS2(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START PARTITION1_PARTITION_PROCESS2_PROCESS_VARIABLES
	portUINT32 UI32_COUNTER1 = 0;
	// SNIPPET_END

	// SNIPPET_START PARTITION1_PARTITION_PROCESS2_PROCESS_INITIALIZATION
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START PARTITION1_PARTITION_PROCESS2_PROCESS_BEFORE_SUSPENSION_CODE
		// Tracks process execution
		CONSOLE_PRINTF("%u PC P1 p2 Js\r\n", TRACK_READ());

		// Sets LEDs pattern for some time
		for (UI32_COUNTER1 = 0; UI32_COUNTER1 < TEST_COUNTER_SMALL; UI32_COUNTER1++) {
			TEST_LED1_OFF();
			TEST_LED2_ON();
		}

		// Tracks process execution
		CONSOLE_PRINTF("%u PC P1 p2 Jf\r\n", TRACK_READ());
		// SNIPPET_END

		// Waits for next period
		PERIODIC_WAIT(&RETURN_CODE);
		if (RETURN_CODE != NO_ERROR) {
			// SNIPPET_START PERIODIC_WAIT_ERROR_HANDLING_CODE
			while (true) {
			}
			// SNIPPET_END
		}

		// SNIPPET_START PARTITION1_PARTITION_PROCESS2_PROCESS_AFTER_SUSPENSION_CODE
		// SNIPPET_END
	}
}

// PROCESS2 process attributes
static PROCESS_ATTRIBUTE_TYPE PROCESS2_PROCESS_ATTRIBUTE = {
/*NAME*/(PROCESS_NAME_TYPE) "PROCESS2",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &PROCESS2,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*BASE_PRIORITY*/(PRIORITY_TYPE) 20,
/*PERIOD*/(SYSTEM_TIME_TYPE) 250000000,
/*TIME_CAPACITY*/(SYSTEM_TIME_TYPE) 100000000,
/*DEADLINE*/(DEADLINE_TYPE) HARD };

// -------------------- PROCESS2 PROCESS END --------------------

// PARTITION1 partition default process
void PARTITION1_DEFAULTPROCESS(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START PARTITION1_PARTITION_DEFAULTPROCESS_VARIABLES
	// SNIPPET_END

	// Creates PROCESS1 process
	CREATE_PROCESS(&PROCESS1_PROCESS_ATTRIBUTE, &PROCESS1_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PROCESS_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Creates PROCESS2 process
	CREATE_PROCESS(&PROCESS2_PROCESS_ATTRIBUTE, &PROCESS2_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PROCESS_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Starts PROCESS1 process
	START(PROCESS1_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START START_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Starts PROCESS2 process
	START(PROCESS2_PROCESS_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START START_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// SNIPPET_START PARTITION1_PARTITION_DEFAULTPROCESS_INITIALIZATION
	// SNIPPET_END

	// Sets partition mode
	SET_PARTITION_MODE(NORMAL, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START SET_PARTITION_MODE_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Start of idle process

	// SNIPPET_START PARTITION1_PARTITION_IDLEPROCESS_INITIALIZATION
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START PARTITION1_PARTITION_IDLEPROCESS_CODE
		// Sets LEDs pattern
		TEST_LED1_OFF();
		TEST_LED2_OFF();
		// SNIPPET_END
	}
}
