// ARINC653 includes
#include "arinc653_core.h"

// Module startup method
void STARTUP_MODULE(RETURN_CODE_TYPE *RETURN_CODE) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	MODULE_CONFIGURATION_HEALTHMONITORING_TYPE *MODULE_CONFIGURATION_HEALTHMONITORING;

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Calls hook
	if (!PORT_HOOK_BEFORE_STARTUP_MODULE()) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	// Starts up core
	STARTUP_CORE();

	// Initializes module heap information before starting it up
	MODULE_INFORMATION->REC_HEAP.PTR_POINTER = null;
	MODULE_INFORMATION->REC_HEAP.SIZ_SIZE = null;
	MODULE_INFORMATION->REC_HEAP.SIZ_FULL = null;

	// Starts up heap
	HEAP_STARTUP(&MODULE_INFORMATION->REC_HEAP, MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_HEAP_ADDRESS, MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_HEAP_SIZE, PORT_HEAP_ALIGNMENT);

	// Allocates memory for module partition information - Allocates even if pointer is not null
	HEAP_ALLOCATE(&MODULE_INFORMATION->REC_HEAP, (heapPOINTER *) &MODULE_INFORMATION->PARTITION_INFORMATION, sizeof(PARTITION_INFORMATION_TYPE) * MODULE_INFORMATION->MODULE_CONFIGURATION->MAXIMUM_PARTITION_COUNT, true);

	// Verifies module partition information memory
	if ((MODULE_INFORMATION->PARTITION_INFORMATION == null) && (MODULE_INFORMATION->MODULE_CONFIGURATION->MAXIMUM_PARTITION_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	// Initializes module information
	INITIALIZE_MODULE_INFORMATION();

	// Allocates memory for module default partition context - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&MODULE_INFORMATION->REC_HEAP, (heapPOINTER *) &MODULE_INFORMATION->CONTEXT.PORT_CONTEXT, sizeof(PORT_CONTEXT_TYPE), false);

	// Verifies module default partition context memory
	if (MODULE_INFORMATION->CONTEXT.PORT_CONTEXT == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	// Initializes module context
	PORT_INITIALIZECONTEXT(&MODULE_INFORMATION->CONTEXT);

	// Initializes module default partition context
	MODULE_INFORMATION->CONTEXT.IDENTIFIER = MODULE_INFORMATION->NEXT_CONTEXT_IDENTIFIER++; // Allocates context identifier
	MODULE_INFORMATION->CONTEXT.STACK_BOTTOM = null; // Runs in system stack
	MODULE_INFORMATION->CONTEXT.STACK_TOP = null; // Runs in system stack
	MODULE_INFORMATION->CONTEXT.CORE_DEPTH = 0;
	MODULE_INFORMATION->CONTEXT.SYSTEM_PARTITION_CONTEXT = false;
	MODULE_INFORMATION->CONTEXT.ENTRY_POINT = null; // Already running
	MODULE_INFORMATION->CONTEXT.RETURN_POINT = null; // Already running
	MODULE_INFORMATION->CONTEXT.PARAMETER1 = null; // Already running
	MODULE_INFORMATION->CONTEXT.PARAMETER2 = null; // Already running

	// Sets module default partition context default system state
	MODULE_INFORMATION->CONTEXT.DEFAULT_SYSTEM_STATE = SYSTEMSTATE_MODULEDEFAULTPARTITION;

	// Sets module default partition context system state
	MODULE_INFORMATION->CONTEXT.SYSTEM_STATE = SYSTEMSTATE_MODULEDEFAULTPARTITION;

	// Sets current context
	_CORE_CURRENT_CONTEXT = &MODULE_INFORMATION->CONTEXT;

	// Allocates memory for module health monitoring callback context - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&MODULE_INFORMATION->REC_HEAP, (heapPOINTER *) &MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.PORT_CONTEXT, sizeof(PORT_CONTEXT_TYPE), false);

	// Verifies module health monitoring callback context memory
	if (MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.PORT_CONTEXT == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	// Initializes module health monitoring callback context
	PORT_INITIALIZECONTEXT(&MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT);

	// Sets module health monitoring callback context default system state
	MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.DEFAULT_SYSTEM_STATE = SYSTEMSTATE_MODULEHEALTHMONITORINGCALLBACK;

	// Gets module configuration health monitoring
	MODULE_CONFIGURATION_HEALTHMONITORING = MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_CONFIGURATION_HEALTHMONITORING;

	// Verifies module configuration health monitoring
	if (MODULE_CONFIGURATION_HEALTHMONITORING != null) {

		// Verifies module configuration health monitoring callback
		if (MODULE_CONFIGURATION_HEALTHMONITORING->CALLBACK_ENTRY_POINT != null) {

			// Verifies module health monitoring callback stack configuration
			if ((MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_HMCALLBACK_STACK_ADDRESS == null) || (MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_HMCALLBACK_STACK_SIZE == null)) {

				// Sets return code
				*RETURN_CODE = INVALID_CONFIG;
				return;
			}

			// Sets up module health monitoring callback
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.EXISTS = true;
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.ENTRY_POINT = MODULE_CONFIGURATION_HEALTHMONITORING->CALLBACK_ENTRY_POINT;
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.STACK_SIZE = MODULE_CONFIGURATION_HEALTHMONITORING->CALLBACK_STACK_SIZE;
			if (MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.IDENTIFIER == null) {
				MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.IDENTIFIER = (MODULE_INFORMATION->NEXT_CONTEXT_IDENTIFIER++);
			}
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.STACK_BOTTOM = MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_HMCALLBACK_STACK_ADDRESS;
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.STACK_TOP = (STACKROW_TYPE *) ((UINTPOINTER_TYPE) MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.STACK_BOTTOM + (UINTPOINTER_TYPE) MODULE_INFORMATION->MODULE_CONFIGURATION->MODULE_HMCALLBACK_STACK_SIZE);
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.CORE_DEPTH = 0;
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.ENTRY_POINT = MODULE_CONFIGURATION_HEALTHMONITORING->CALLBACK_ENTRY_POINT;
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.RETURN_POINT = &RETURNPOINT_MODULE_HEALTHMONITORINGCALLBACK;
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.PARAMETER1 = null;
			MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.PARAMETER2 = null;

			// Verifies module health monitoring callback context identifier
			if (MODULE_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.IDENTIFIER > MODULE_INFORMATION->LAST_CONTEXT_IDENTIFIER) {

				// Sets return code
				*RETURN_CODE = INVALID_CONFIG;
				return;
			}
		}
	}

	// Calls hook
	if (!PORT_HOOK_AFTER_STARTUP_MODULE()) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;
		return;
	}

	// Sets return code
	*RETURN_CODE = NO_ERROR;
}

// Module mode set method
void SET_MODULE_MODE(OPERATING_MODE_TYPE OPERATING_MODE, RETURN_CODE_TYPE *RETURN_CODE) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;

	// Enters core
	ENTER_CORE();

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Verifies measure flag state
#ifndef MEASURE

	// To enable scheduler being restarted

	// Verifies operating mode transition
	if (MODULE_INFORMATION->OPERATING_MODE == IDLE) {

		// No code could be running
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}
#endif

	// Verifies operating mode
	if (OPERATING_MODE == IDLE) {

		// Sets operating mode
		MODULE_INFORMATION->OPERATING_MODE = OPERATING_MODE;

		// Terminates tick
		PORT_TERMINATETICK();
#if defined(TRACK)
		// Tracks termination
		CONSOLE_PRINTF("%u TERM\n", TRACK_READ());
#endif
		// Yields
		PORT_YIELD();

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies operating mode
	if ((OPERATING_MODE == COLD_START) || (OPERATING_MODE == WARM_START)) {

		// Verifies transition from cold start to warm start
		if ((MODULE_INFORMATION->OPERATING_MODE == COLD_START) && (OPERATING_MODE == WARM_START)) {

			// No transition from COLD_START to WARM_START is allowed
			*RETURN_CODE = INVALID_MODE;

			// Exits core
			EXIT_CORE();
			return;
		}

		// Sets operating mode
		MODULE_INFORMATION->OPERATING_MODE = OPERATING_MODE;

		// Restarts module
		PORT_RESTARTMODULE();

		// Must never be reached
		while (true) {
		}
	}

	// Verifies operating mode
	if (OPERATING_MODE == NORMAL) {

		// Verifies normal operation
		if (MODULE_INFORMATION->OPERATING_MODE == NORMAL) {

			// No transition
			*RETURN_CODE = NO_ACTION;

			// Exits core
			EXIT_CORE();
			return;
		}

		// Sets operating mode
		MODULE_INFORMATION->OPERATING_MODE = OPERATING_MODE;

		// Starts partition scheduler
		SCHEDULER_STARTPARTITIONSCHEDULER(RETURN_CODE);

		// Sets module default partition context default system state
		MODULE_INFORMATION->CONTEXT.DEFAULT_SYSTEM_STATE = SYSTEMSTATE_MODULEIDLEPARTITION;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Operating mode is not valid
	*RETURN_CODE = INVALID_PARAM;

	// Exits core
	EXIT_CORE();
}
