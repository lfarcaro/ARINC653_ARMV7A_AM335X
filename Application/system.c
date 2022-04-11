// System include
#include "system.h"

// Configuration include
#include "configuration.h"

// SNIPPET_START GLOBAL_INCLUDES
// Test include
#include "test.h"
// SNIPPET_END

// -------------------- SYSTEM DESCRIPTION START --------------------
// SNIPPET_START SYSTEM_DESCRIPTION
// SNIPPET_END
// -------------------- SYSTEM DESCRIPTION END --------------------

// SNIPPET_START SYSTEM_INCLUDES
// SNIPPET_END

// SNIPPET_START SYSTEM_GLOBAL_DEFINITIONS
// SNIPPET_END

// SNIPPET_START SYSTEM_GLOBAL_VARIABLES
// SNIPPET_END

// SNIPPET_START SYSTEM_FUNCTIONS
// SNIPPET_END

// System default module
void SYSTEM_DEFAULTMODULE(void) {
	RETURN_CODE_TYPE RETURN_CODE;
	// SNIPPET_START SYSTEM_DEFAULTMODULE_VARIABLES
	// SNIPPET_END

	// Starts system up
	STARTUP_SYSTEM(&RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START STARTUP_SYSTEM_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// SNIPPET_START SYSTEM_DEFAULTMODULE_INITIALIZATION
	// Starts up test
	TEST_STARTUP();
	// SNIPPET_END

	// Runs system
	RUN_SYSTEM(&RETURN_CODE);
	if (RETURN_CODE != NO_ERROR) {
		// SNIPPET_START RUN_SYSTEM_ERROR_HANDLING_CODE
		while (true) {
		}
		// SNIPPET_END
	}

	// Exit loop (should never be reached)
	while (true);
}
