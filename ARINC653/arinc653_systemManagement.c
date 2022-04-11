// ARINC653 includes
#include "arinc653_core.h"

// System startup method
void STARTUP_SYSTEM(RETURN_CODE_TYPE *RETURN_CODE) {

	// Sets system configuration
	SYSTEM_INFORMATION.SYSTEM_CONFIGURATION = &SYSTEM_CONFIGURATION;

	// Calls hook
	if (!PORT_HOOK_STARTUP_SYSTEM()) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	// Sets return code
	*RETURN_CODE = NO_ERROR;
}

// System running method
void RUN_SYSTEM(RETURN_CODE_TYPE *RETURN_CODE) {

	// Calls hook
	if (!PORT_HOOK_RUN_SYSTEM()) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	// Sets return code
	*RETURN_CODE = NO_ERROR;
}
