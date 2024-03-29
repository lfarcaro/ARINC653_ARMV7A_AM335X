// Partition include
#include "partition3_partition.h"

// SNIPPET_START GLOBAL_INCLUDES
// Test include
#include "test.h"
// SNIPPET_END

// -------------------- PARTITION DESCRIPTION START --------------------
// SNIPPET_START PARTITION3_PARTITION_DESCRIPTION
// SNIPPET_END
// -------------------- PARTITION DESCRIPTION END --------------------

// SNIPPET_START PARTITION3_PARTITION_INCLUDES
// SNIPPET_END

// SNIPPET_START PARTITION3_PARTITION_GLOBAL_DEFINITIONS
// SNIPPET_END

// SNIPPET_START PARTITION3_PARTITION_GLOBAL_VARIABLES
// SNIPPET_END

// SNIPPET_START PARTITION3_PARTITION_FUNCTIONS
// SNIPPET_END

// PARTITION3 partition default process
void PARTITION3_DEFAULTPROCESS(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START PARTITION3_PARTITION_DEFAULTPROCESS_VARIABLES
	// SNIPPET_END

	// SNIPPET_START PARTITION3_PARTITION_DEFAULTPROCESS_INITIALIZATION
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

	// SNIPPET_START PARTITION3_PARTITION_IDLEPROCESS_INITIALIZATION
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START PARTITION3_PARTITION_IDLEPROCESS_CODE
		// Sets LEDs pattern
		TEST_LED1_OFF();
		TEST_LED2_OFF();
		// SNIPPET_END
	}
}
