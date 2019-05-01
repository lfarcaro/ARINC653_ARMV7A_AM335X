// Module include
#include "module.h"

// Configuration include
#include "configuration.h"

// SNIPPET_START GLOBAL_INCLUDES
// Test include
#include "test.h"
// SNIPPET_END

// -------------------- MODULE DESCRIPTION START --------------------
// SNIPPET_START MODULE_DESCRIPTION
// SNIPPET_END
// -------------------- MODULE DESCRIPTION END --------------------

// SNIPPET_START MODULE_INCLUDES
// SNIPPET_END

// PARTITION1 partition attributes
static PARTITION_ATTRIBUTE_TYPE PARTITION1_PARTITION_ATTRIBUTE = {
/*NAME*/(PARTITION_NAME_TYPE) "PARTITION1",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &PARTITION1_DEFAULTPROCESS,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*SYSTEMPARTITION*/(BOOLEAN_TYPE) false };

// PARTITION1 partition identifier
static PARTITION_ID_TYPE PARTITION1_PARTITION_ID;

// PARTITION2 partition attributes
static PARTITION_ATTRIBUTE_TYPE PARTITION2_PARTITION_ATTRIBUTE = {
/*NAME*/(PARTITION_NAME_TYPE) "PARTITION2",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &PARTITION2_DEFAULTPROCESS,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*SYSTEMPARTITION*/(BOOLEAN_TYPE) false };

// PARTITION2 partition identifier
static PARTITION_ID_TYPE PARTITION2_PARTITION_ID;

// PARTITION3 partition attributes
static PARTITION_ATTRIBUTE_TYPE PARTITION3_PARTITION_ATTRIBUTE = {
/*NAME*/(PARTITION_NAME_TYPE) "PARTITION3",
/*ENTRY_POINT*/(SYSTEM_ADDRESS_TYPE) &PARTITION3_DEFAULTPROCESS,
/*STACK_SIZE*/(STACK_SIZE_TYPE) 256,
/*SYSTEMPARTITION*/(BOOLEAN_TYPE) false };

// PARTITION3 partition identifier
static PARTITION_ID_TYPE PARTITION3_PARTITION_ID;

// SNIPPET_START MODULE_GLOBAL_VARIABLES
// SNIPPET_END

// SNIPPET_START MODULE_FUNCTIONS
// SNIPPET_END

// Module default partition
void MODULE_DEFAULTPARTITION(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START MODULE_DEFAULTPARTITION_VARIABLES
	// SNIPPET_END

	// Starts system up
	STARTUP_SYSTEM(&RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START STARTUP_SYSTEM_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Starts module up
	STARTUP_MODULE(&RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START STARTUP_MODULE_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Creates PARTITION1 partition
	CREATE_PARTITION(&PARTITION1_PARTITION_ATTRIBUTE, &PARTITION1_PARTITION_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PARTITION_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Creates PARTITION2 partition
	CREATE_PARTITION(&PARTITION2_PARTITION_ATTRIBUTE, &PARTITION2_PARTITION_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PARTITION_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Creates PARTITION3 partition
	CREATE_PARTITION(&PARTITION3_PARTITION_ATTRIBUTE, &PARTITION3_PARTITION_ID, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START CREATE_PARTITION_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// SNIPPET_START MODULE_DEFAULTPARTITION_INITIALIZATION
	// Starts up test
	TEST_STARTUP();
	// SNIPPET_END

	// Sets module mode
	SET_MODULE_MODE(NORMAL, &RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START SET_MODULE_MODE_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Start of idle partition

	// SNIPPET_START MODULE_IDLEPARTITION_INITIALIZATION
	// SNIPPET_END

	// Main loop
	while (true) {

		// SNIPPET_START MODULE_IDLEPARTITION_CODE
		// Sets LEDs pattern
		TEST_LED1_OFF();
		TEST_LED2_OFF();
		// SNIPPET_END
	}
}
