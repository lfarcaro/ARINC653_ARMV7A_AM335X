// ARINC653 includes
#include "arinc653_core.h"

// Partition internal identifier getter
void GET_PARTITION_INTERNAL_IDENTIFIER(PARTITION_ID_TYPE PARTITION_EXTERNAL_IDENTIFIER, PARTITION_ID_TYPE *PARTITION_INTERNAL_IDENTIFIER, RETURN_CODE_TYPE *RETURN_CODE) {
	PARTITION_ID_TYPE PARTITION_IDENTIFIER;

	// Enters core
	ENTER_CORE();

	// Iterates partitions
	for (PARTITION_IDENTIFIER = 0; PARTITION_IDENTIFIER < MODULE_INFORMATION.PARTITION_COUNT; PARTITION_IDENTIFIER++) {

		// Compares external identifiers
		if (MODULE_INFORMATION.PARTITION_INFORMATION[PARTITION_IDENTIFIER].PARTITION_CONFIGURATION->EXTERNAL_IDENTIFIER == PARTITION_EXTERNAL_IDENTIFIER) {

			// Sets partition internal identifier
			*PARTITION_INTERNAL_IDENTIFIER = PARTITION_IDENTIFIER;

			// Sets return code
			*RETURN_CODE = NO_ERROR;

			// Exits core
			EXIT_CORE();
			return;
		}
	}

	// Sets return code
	*RETURN_CODE = INVALID_CONFIG;

	// Exits core
	EXIT_CORE();
}

// Partition identifier getter
void GET_PARTITION_ID(PARTITION_NAME_TYPE PARTITION_NAME, PARTITION_ID_TYPE *PARTITION_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	PARTITION_ID_TYPE PARTITION_IDENTIFIER;

	// Enters core
	ENTER_CORE();

	// Iterates partitions
	for (PARTITION_IDENTIFIER = 0; PARTITION_IDENTIFIER < MODULE_INFORMATION.PARTITION_COUNT; PARTITION_IDENTIFIER++) {

		// Compares names
		if (COMMON_COMPARESTRINGS(PARTITION_NAME, MODULE_INFORMATION.PARTITION_INFORMATION[PARTITION_IDENTIFIER].PARTITION_ATTRIBUTE->NAME, MAX_NAME_LENGTH)) {

			// Sets partition identifier
			*PARTITION_ID = PARTITION_IDENTIFIER;

			// Sets return code
			*RETURN_CODE = NO_ERROR;

			// Exits core
			EXIT_CORE();
			return;
		}
	}

	// Sets return code
	*RETURN_CODE = INVALID_CONFIG;

	// Exits core
	EXIT_CORE();
}

// Create partition method
void CREATE_PARTITION(PARTITION_ATTRIBUTE_TYPE *ATTRIBUTES, PARTITION_ID_TYPE *PARTITION_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	PARTITION_ID_TYPE PARTITION_IDENTIFIER;
	INDEX_TYPE SCHEDULE_WINDOW_INDEX;
	SYSTEM_TIME_TYPE START_TIME;
	PARTITION_INFORMATION_TYPE *PARTITION_INFORMATION;
	PARTITION_CONFIGURATION_TYPE *PARTITION_CONFIGURATION;
	PROCESS_ID_TYPE PROCESS_IDENTIFIER;
	PARTITION_CONFIGURATION_HEALTHMONITORING_TYPE *PARTITION_CONFIGURATION_HEALTHMONITORING;
	UINTBASE_TYPE *PTR_SOURCE;
	UINTBASE_TYPE *PTR_TARGET;
	// -------------------- OPTIONAL MODULES START --------------------
	SAMPLING_PORT_ID_TYPE SAMPLINGPORT_IDENTIFIER;
	QUEUING_PORT_ID_TYPE QUEUINGPORT_IDENTIFIER;
	BUFFER_ID_TYPE BUFFER_IDENTIFIER;
	BLACKBOARD_ID_TYPE BLACKBOARD_IDENTIFIER;
	SEMAPHORE_ID_TYPE SEMAPHORE_IDENTIFIER;
	EVENT_ID_TYPE EVENT_IDENTIFIER;
	// -------------------- OPTIONAL MODULES END --------------------

	// Enters core
	ENTER_CORE();

	// Verifies operating mode
	if (MODULE_INFORMATION.OPERATING_MODE == NORMAL) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets free identifier
	PARTITION_IDENTIFIER = MODULE_INFORMATION.PARTITION_COUNT;

	// Verifies partition count
	if (PARTITION_IDENTIFIER >= MODULE_INFORMATION.MODULE_CONFIGURATION->MAXIMUM_PARTITION_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets partition identifier
	GET_PARTITION_ID(ATTRIBUTES->NAME, &PARTITION_IDENTIFIER, RETURN_CODE);

	// Verifies partition identifier
	if (*RETURN_CODE == NO_ERROR) {

		// Sets return code
		*RETURN_CODE = NO_ACTION;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets partition information
	PARTITION_INFORMATION = &MODULE_INFORMATION.PARTITION_INFORMATION[PARTITION_IDENTIFIER];

	// Gets partition configuration
	PARTITION_CONFIGURATION = CONFIGURATION_GETPARTITIONCONFIGURATION(ATTRIBUTES->NAME);

	// Verifies period range
	if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->PERIOD > MODULE_INFORMATION.MODULE_CONFIGURATION->MAJORFRAME_DURATION) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies period granularity
	if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->PERIOD % PORT_TICKWINDOW != 0) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies duration range
	if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->DURATION > MODULE_INFORMATION.MODULE_CONFIGURATION->MAJORFRAME_DURATION) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies duration granularity
	if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->DURATION % PORT_TICKWINDOW != 0) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies schedule size
	if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW_SIZE <= 0) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Initializes start time
	START_TIME = PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW[0].START;

	// Iterates schedule windows
	for (SCHEDULE_WINDOW_INDEX = 0; SCHEDULE_WINDOW_INDEX < PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW_SIZE; SCHEDULE_WINDOW_INDEX++) {

		// Verifies start granularity
		if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW[SCHEDULE_WINDOW_INDEX].START % PORT_TICKWINDOW != 0) {

			// Sets return code
			*RETURN_CODE = INVALID_CONFIG;

			// Exits core
			EXIT_CORE();
			return;
		}

		// Verifies duration granularity
		if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW[SCHEDULE_WINDOW_INDEX].DURATION % PORT_TICKWINDOW != 0) {

			// Sets return code
			*RETURN_CODE = INVALID_CONFIG;

			// Exits core
			EXIT_CORE();
			return;
		}

		// Verifies start strict growing
		if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW[SCHEDULE_WINDOW_INDEX].START < START_TIME) {

			// Sets return code
			*RETURN_CODE = INVALID_CONFIG;

			// Exits core
			EXIT_CORE();
			return;
		}

		// Sets start time
		START_TIME = PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW[SCHEDULE_WINDOW_INDEX].START;

		// Verifies contention in major frame
		if (PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW[SCHEDULE_WINDOW_INDEX].START + PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->SCHEDULE_WINDOW[SCHEDULE_WINDOW_INDEX].DURATION > MODULE_INFORMATION.MODULE_CONFIGURATION->MAJORFRAME_DURATION) {

			// Sets return code
			*RETURN_CODE = INVALID_CONFIG;

			// Exits core
			EXIT_CORE();
			return;
		}
	}

	// Verifies partition configuration
	if (PARTITION_CONFIGURATION == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Sets partition configuration
	PARTITION_INFORMATION->PARTITION_CONFIGURATION = PARTITION_CONFIGURATION;

	// Verifies partition data image region size
	if (PARTITION_CONFIGURATION->PARTITION_DATA_IMAGE_SIZE != PARTITION_CONFIGURATION->PARTITION_DATA_SIZE) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Calls hook
	if (!PORT_HOOK_BEFORE_CREATE_PARTITION(PARTITION_INFORMATION)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Allocates memory for partition default process context - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->CONTEXT.PORT_CONTEXT, sizeof(PORT_CONTEXT_TYPE), false);

	// Verifies partition default process context memory
	if (PARTITION_INFORMATION->CONTEXT.PORT_CONTEXT == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies partition initialized flag
	if (!PARTITION_INFORMATION->INITIALIZED) {

		// Initializes partition default process context
		PORT_INITIALIZECONTEXT(&PARTITION_INFORMATION->CONTEXT);

		// Sets partition default process context default system state
		PARTITION_INFORMATION->CONTEXT.DEFAULT_SYSTEM_STATE = SYSTEMSTATE_PARTITIONDEFAULTPROCESS;
	}

	// Allocates memory for partition error handler context - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->ERRORHANDLER_INFORMATION.CONTEXT.PORT_CONTEXT, sizeof(PORT_CONTEXT_TYPE), false);

	// Verifies partition error handler context memory
	if (PARTITION_INFORMATION->ERRORHANDLER_INFORMATION.CONTEXT.PORT_CONTEXT == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies partition initialized flag
	if (!PARTITION_INFORMATION->INITIALIZED) {

		// Initializes partition error handler context
		PORT_INITIALIZECONTEXT(&PARTITION_INFORMATION->ERRORHANDLER_INFORMATION.CONTEXT);

		// Sets partition error handler context default system state
		PARTITION_INFORMATION->ERRORHANDLER_INFORMATION.CONTEXT.DEFAULT_SYSTEM_STATE = SYSTEMSTATE_PARTITIONERRORHANDLER;
	}

	// Allocates memory for partition health monitoring callback context - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.PORT_CONTEXT, sizeof(PORT_CONTEXT_TYPE), false);

	// Verifies partition health monitoring callback context memory
	if (PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.PORT_CONTEXT == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies partition initialized flag
	if (!PARTITION_INFORMATION->INITIALIZED) {

		// Initializes partition health monitoring callback context
		PORT_INITIALIZECONTEXT(&PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT);

		// Sets partition health monitoring callback context default system state
		PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.DEFAULT_SYSTEM_STATE = SYSTEMSTATE_PARTITIONHEALTHMONITORINGCALLBACK;
	}

	// Starts up heap
	HEAP_STARTUP(&PARTITION_INFORMATION->REC_HEAP, PARTITION_CONFIGURATION->PARTITION_HEAP_ADDRESS, PARTITION_CONFIGURATION->PARTITION_HEAP_SIZE, PORT_HEAP_ALIGNMENT);

	// Allocates memory for process information - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->PROCESS_INFORMATION, sizeof(PROCESS_INFORMATION_TYPE) * PARTITION_CONFIGURATION->MAXIMUM_PROCESS_COUNT, false);

	// Verifies process information memory
	if ((PARTITION_INFORMATION->PROCESS_INFORMATION == null) && (PARTITION_CONFIGURATION->MAXIMUM_PROCESS_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Iterates process entries
	for (PROCESS_IDENTIFIER = 0; PROCESS_IDENTIFIER < PARTITION_CONFIGURATION->MAXIMUM_PROCESS_COUNT; PROCESS_IDENTIFIER++) {

		// (Re)initializes process information
		INITIALIZE_PROCESS_INFORMATION(PARTITION_IDENTIFIER, PROCESS_IDENTIFIER);
	}

	// Allocates memory for partition error status - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->ERROR_STATUS, sizeof(ERROR_STATUS_TYPE) * PARTITION_CONFIGURATION->MAXIMUM_ERRORSTATUS_COUNT, false);

	// Verifies partition error status memory
	if ((PARTITION_INFORMATION->ERROR_STATUS == null) && (PARTITION_CONFIGURATION->MAXIMUM_ERRORSTATUS_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// -------------------- OPTIONAL MODULES START --------------------

	// -------------------- SAMPLING PORT --------------------

	// Allocates memory for partition sampling port information - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION, sizeof(SAMPLINGPORT_INFORMATION_TYPE) * PARTITION_CONFIGURATION->MAXIMUM_SAMPLINGPORT_COUNT, false);

	// Verifies partition sampling port information memory
	if ((PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION == null) && (PARTITION_CONFIGURATION->MAXIMUM_SAMPLINGPORT_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Iterates sampling port entries
	for (SAMPLINGPORT_IDENTIFIER = 0; SAMPLINGPORT_IDENTIFIER < PARTITION_CONFIGURATION->MAXIMUM_SAMPLINGPORT_COUNT; SAMPLINGPORT_IDENTIFIER++) {

		// (Re)initializes sampling port information
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].IDENTIFIER = 0;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].NAME = null;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].MAX_MESSAGE_SIZE = 0;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].PORT_DIRECTION = SOURCE;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].REFRESH_PERIOD = INFINITE_TIME_VALUE;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].LAST_READ = INFINITE_TIME_VALUE;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].LAST_WRITE = INFINITE_TIME_VALUE;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].LAST_VALIDITY = INVALID;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].EMPTY_INDICATOR = EMPTY;
		PARTITION_INFORMATION->SAMPLINGPORT_INFORMATION[SAMPLINGPORT_IDENTIFIER].LENGTH = 0;
	}

	// -------------------- QUEUING PORT --------------------

	// Allocates memory for partition queuing port information - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->QUEUINGPORT_INFORMATION, sizeof(QUEUINGPORT_INFORMATION_TYPE) * PARTITION_CONFIGURATION->MAXIMUM_QUEUINGPORT_COUNT, false);

	// Verifies partition queuing port information memory
	if ((PARTITION_INFORMATION->QUEUINGPORT_INFORMATION == null) && (PARTITION_CONFIGURATION->MAXIMUM_QUEUINGPORT_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Iterates queuing port entries
	for (QUEUINGPORT_IDENTIFIER = 0; QUEUINGPORT_IDENTIFIER < PARTITION_CONFIGURATION->MAXIMUM_QUEUINGPORT_COUNT; QUEUINGPORT_IDENTIFIER++) {

		// (Re)initializes queuing port information
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].IDENTIFIER = 0;
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].NAME = null;
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].MAX_MESSAGE_SIZE = 0;
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].MAX_NB_MESSAGE = 0;
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].PORT_DIRECTION = SOURCE;
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].QUEUING_DISCIPLINE = FIFO;
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].MESSAGE_COUNT = 0;
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].NEXT_MESSAGE_INDEX = 0;
		PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].OVERFLOW = false;

		// Verifies partition initialized flag
		if (!PARTITION_INFORMATION->INITIALIZED) {

			// Initializes queuing port information
			PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].MESSAGES = null;
			PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].MESSAGES_LENGTH = null;

			// Starts up priority queues
			PRIORITYQUEUE_STARTUP(&PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].REC_QUEUINGPORT, true, false);
		} else {

			// Clears priority queues
			PRIORITYQUEUE_CLEAR(&PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].REC_QUEUINGPORT, true, false);
		}
	}

	// -------------------- BUFFER --------------------

	// Allocates memory for partition buffer information - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->BUFFER_INFORMATION, sizeof(BUFFER_INFORMATION_TYPE) * PARTITION_CONFIGURATION->MAXIMUM_BUFFER_COUNT, false);

	// Verifies partition buffer information memory
	if ((PARTITION_INFORMATION->BUFFER_INFORMATION == null) && (PARTITION_CONFIGURATION->MAXIMUM_BUFFER_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Iterates buffer entries
	for (BUFFER_IDENTIFIER = 0; BUFFER_IDENTIFIER < PARTITION_CONFIGURATION->MAXIMUM_BUFFER_COUNT; BUFFER_IDENTIFIER++) {

		// (Re)initializes buffer information
		PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].IDENTIFIER = 0;
		PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].NAME = null;
		PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].MAX_MESSAGE_SIZE = 0;
		PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].MAX_NB_MESSAGE = 0;
		PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].QUEUING_DISCIPLINE = FIFO;
		PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].MESSAGE_COUNT = 0;
		PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].NEXT_MESSAGE_INDEX = 0;

		// Verifies partition initialized flag
		if (!PARTITION_INFORMATION->INITIALIZED) {

			// Initializes buffer information
			PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].MESSAGES = null;
			PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].MESSAGES_LENGTH = null;

			// Starts up priority queues
			PRIORITYQUEUE_STARTUP(&PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].REC_BUFFER, true, false);
		} else {

			// Clears priority queues
			PRIORITYQUEUE_CLEAR(&PARTITION_INFORMATION->BUFFER_INFORMATION[BUFFER_IDENTIFIER].REC_BUFFER, true, false);
		}
	}

	// -------------------- BLACKBOARD --------------------

	// Allocates memory for partition blackboard information - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->BLACKBOARD_INFORMATION, sizeof(BLACKBOARD_INFORMATION_TYPE) * PARTITION_CONFIGURATION->MAXIMUM_BLACKBOARD_COUNT, false);

	// Verifies partition blackboard information memory
	if ((PARTITION_INFORMATION->BLACKBOARD_INFORMATION == null) && (PARTITION_CONFIGURATION->MAXIMUM_BLACKBOARD_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Iterates blackboard entries
	for (BLACKBOARD_IDENTIFIER = 0; BLACKBOARD_IDENTIFIER < PARTITION_CONFIGURATION->MAXIMUM_BLACKBOARD_COUNT; BLACKBOARD_IDENTIFIER++) {

		// (Re)initializes blackboard information
		PARTITION_INFORMATION->BLACKBOARD_INFORMATION[BLACKBOARD_IDENTIFIER].IDENTIFIER = 0;
		PARTITION_INFORMATION->BLACKBOARD_INFORMATION[BLACKBOARD_IDENTIFIER].NAME = null;
		PARTITION_INFORMATION->BLACKBOARD_INFORMATION[BLACKBOARD_IDENTIFIER].EMPTY_INDICATOR = EMPTY;
		PARTITION_INFORMATION->BLACKBOARD_INFORMATION[BLACKBOARD_IDENTIFIER].MAX_MESSAGE_SIZE = 0;
		PARTITION_INFORMATION->BLACKBOARD_INFORMATION[BLACKBOARD_IDENTIFIER].LENGTH = 0;

		// Verifies partition initialized flag
		if (!PARTITION_INFORMATION->INITIALIZED) {

			// Starts up priority queues
			PRIORITYQUEUE_STARTUP(&PARTITION_INFORMATION->BLACKBOARD_INFORMATION[BLACKBOARD_IDENTIFIER].REC_BLACKBOARD, true, false);
		} else {

			// Clears priority queues
			PRIORITYQUEUE_CLEAR(&PARTITION_INFORMATION->BLACKBOARD_INFORMATION[BLACKBOARD_IDENTIFIER].REC_BLACKBOARD, true, false);
		}
	}

	// -------------------- SEMAPHORE --------------------

	// Allocates memory for partition semaphore information - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->SEMAPHORE_INFORMATION, sizeof(SEMAPHORE_INFORMATION_TYPE) * PARTITION_CONFIGURATION->MAXIMUM_SEMAPHORE_COUNT, false);

	// Verifies partition semaphore information memory
	if ((PARTITION_INFORMATION->SEMAPHORE_INFORMATION == null) && (PARTITION_CONFIGURATION->MAXIMUM_SEMAPHORE_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Iterates semaphore entries
	for (SEMAPHORE_IDENTIFIER = 0; SEMAPHORE_IDENTIFIER < PARTITION_CONFIGURATION->MAXIMUM_SEMAPHORE_COUNT; SEMAPHORE_IDENTIFIER++) {

		// (Re)initializes semaphore information
		PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER].IDENTIFIER = 0;
		PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER].NAME = null;
		PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER].CURRENT_VALUE = 0;
		PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER].MAXIMUM_VALUE = 0;
		PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER].QUEUING_DISCIPLINE = FIFO;

		// Verifies partition initialized flag
		if (!PARTITION_INFORMATION->INITIALIZED) {

			// Starts up priority queues
			PRIORITYQUEUE_STARTUP(&PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER].REC_SEMAPHORE, true, false);
		} else {

			// Clears priority queues
			PRIORITYQUEUE_CLEAR(&PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER].REC_SEMAPHORE, true, false);
		}
	}

	// -------------------- EVENT --------------------

	// Allocates memory for partition event information - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&SYSTEM_INFORMATION.REC_HEAP, (heapPOINTER *) &PARTITION_INFORMATION->EVENT_INFORMATION, sizeof(EVENT_INFORMATION_TYPE) * PARTITION_CONFIGURATION->MAXIMUM_EVENT_COUNT, false);

	// Verifies partition event information memory
	if ((PARTITION_INFORMATION->EVENT_INFORMATION == null) && (PARTITION_CONFIGURATION->MAXIMUM_EVENT_COUNT > 0)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Iterates event entries
	for (EVENT_IDENTIFIER = 0; EVENT_IDENTIFIER < PARTITION_CONFIGURATION->MAXIMUM_EVENT_COUNT; EVENT_IDENTIFIER++) {

		// (Re)initializes event information
		PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_IDENTIFIER].IDENTIFIER = 0;
		PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_IDENTIFIER].NAME = null;
		PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_IDENTIFIER].EVENT_STATE = DOWN;

		// Verifies partition initialized flag
		if (!PARTITION_INFORMATION->INITIALIZED) {

			// Starts up priority queues
			PRIORITYQUEUE_STARTUP(&PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_IDENTIFIER].REC_EVENT, true, false);
		} else {

			// Clears priority queues
			PRIORITYQUEUE_CLEAR(&PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_IDENTIFIER].REC_EVENT, true, false);
		}
	}

	// -------------------- OPTIONAL MODULES END --------------------

	// Gets partition configuration health monitoring
	PARTITION_CONFIGURATION_HEALTHMONITORING = PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_HEALTHMONITORING;

	// Verifies partition configuration health monitoring
	if (PARTITION_CONFIGURATION_HEALTHMONITORING != null) {

		// Verifies partition configuration health monitoring callback
		if (PARTITION_CONFIGURATION_HEALTHMONITORING->CALLBACK_ENTRY_POINT != null) {

			// Verifies partition health monitoring callback stack configuration
			if ((PARTITION_CONFIGURATION->PARTITION_HMCALLBACK_STACK_ADDRESS == null) || (PARTITION_CONFIGURATION->PARTITION_HMCALLBACK_STACK_SIZE == null)) {

				// Sets return code
				*RETURN_CODE = INVALID_CONFIG;

				// Exits core
				EXIT_CORE();
				return;
			}

			// Sets up partition health monitoring callback
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.EXISTS = true;
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.ENTRY_POINT = PARTITION_CONFIGURATION_HEALTHMONITORING->CALLBACK_ENTRY_POINT;
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.STACK_SIZE = PARTITION_CONFIGURATION_HEALTHMONITORING->CALLBACK_STACK_SIZE;
			if (PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.IDENTIFIER == null) {
				PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.IDENTIFIER = (SYSTEM_INFORMATION.NEXT_CONTEXT_IDENTIFIER++);
			}
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.STACK_BOTTOM = PARTITION_CONFIGURATION->PARTITION_HMCALLBACK_STACK_ADDRESS;
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.STACK_TOP = (STACKROW_TYPE *) ((UINTPOINTER_TYPE) PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.STACK_BOTTOM + (UINTPOINTER_TYPE) PARTITION_CONFIGURATION->PARTITION_HMCALLBACK_STACK_SIZE);
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.CORE_DEPTH = 0;
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.ENTRY_POINT = PARTITION_CONFIGURATION_HEALTHMONITORING->CALLBACK_ENTRY_POINT;
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.RETURN_POINT = &RETURNPOINT_PARTITION_HEALTHMONITORINGCALLBACK;
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.PARAMETER1 = null;
			PARTITION_INFORMATION->HEALTHMONITORINGCALLBACK_INFORMATION.CONTEXT.PARAMETER2 = null;
		}
	}

	// Verifies partition default process stack configuration
	if ((PARTITION_CONFIGURATION->PARTITION_DEFAULTPROCESS_STACK_ADDRESS == null) || (PARTITION_CONFIGURATION->PARTITION_DEFAULTPROCESS_STACK_SIZE == null)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies partition initialized flag
	if (!PARTITION_INFORMATION->INITIALIZED) {

		// Prepares source and target addresses
		PTR_SOURCE = (UINTBASE_TYPE *) PARTITION_CONFIGURATION->PARTITION_DATA_ADDRESS;
		PTR_TARGET = (UINTBASE_TYPE *) PARTITION_CONFIGURATION->PARTITION_DATA_IMAGE_ADDRESS;

		// Copies partition data region into partition data image region
		for (; (UINTPOINTER_TYPE) PTR_SOURCE < ((UINTPOINTER_TYPE) PARTITION_CONFIGURATION->PARTITION_DATA_ADDRESS + PARTITION_CONFIGURATION->PARTITION_DATA_SIZE);) {
			*PTR_TARGET = *PTR_SOURCE;

			// Moves addresses
			PTR_SOURCE++;
			PTR_TARGET++;
		}
	} else {

		// Not tested, only used if module restart is implemented

		// Prepares source and target addresses
		PTR_SOURCE = (UINTBASE_TYPE *) PARTITION_CONFIGURATION->PARTITION_DATA_IMAGE_ADDRESS;
		PTR_TARGET = (UINTBASE_TYPE *) PARTITION_CONFIGURATION->PARTITION_DATA_ADDRESS;

		// Copies partition data image region back into partition data region
		for (; (UINTPOINTER_TYPE) PTR_TARGET < ((UINTPOINTER_TYPE) PARTITION_CONFIGURATION->PARTITION_DATA_ADDRESS + PARTITION_CONFIGURATION->PARTITION_DATA_SIZE);) {
			*PTR_TARGET = *PTR_SOURCE;

			// Moves addresses
			PTR_SOURCE++;
			PTR_TARGET++;
		}
	}

	// Sets up partition
	PARTITION_INFORMATION->PARTITION_ATTRIBUTE = ATTRIBUTES;
	PARTITION_INFORMATION->IDENTIFIER = MODULE_INFORMATION.PARTITION_COUNT;
	PARTITION_INFORMATION->START_CONDITION = NORMAL_START;
	if (PARTITION_INFORMATION->CONTEXT.IDENTIFIER == null) {
		PARTITION_INFORMATION->CONTEXT.IDENTIFIER = (SYSTEM_INFORMATION.NEXT_CONTEXT_IDENTIFIER++);
	}
	PARTITION_INFORMATION->CONTEXT.STACK_BOTTOM = PARTITION_CONFIGURATION->PARTITION_DEFAULTPROCESS_STACK_ADDRESS;
	PARTITION_INFORMATION->CONTEXT.STACK_TOP = (STACKROW_TYPE *) ((UINTPOINTER_TYPE) PARTITION_INFORMATION->CONTEXT.STACK_BOTTOM + (UINTPOINTER_TYPE) PARTITION_CONFIGURATION->PARTITION_DEFAULTPROCESS_STACK_SIZE);
	PARTITION_INFORMATION->CONTEXT.CORE_DEPTH = 0;
	PARTITION_INFORMATION->CONTEXT.SYSTEM_PARTITION_CONTEXT = ATTRIBUTES->SYSTEMPARTITION;
	PARTITION_INFORMATION->CONTEXT.ENTRY_POINT = ATTRIBUTES->ENTRY_POINT;
	PARTITION_INFORMATION->CONTEXT.RETURN_POINT = &RETURNPOINT_PARTITIONDEFAULTPROCESS;
	PARTITION_INFORMATION->CONTEXT.PARAMETER1 = null;
	PARTITION_INFORMATION->CONTEXT.PARAMETER2 = null;

	// Counts created partition
	MODULE_INFORMATION.PARTITION_COUNT++;

	// Calls hook
	if (!PORT_HOOK_AFTER_CREATE_PARTITION(PARTITION_INFORMATION)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Sets partition identifier
	*PARTITION_ID = PARTITION_IDENTIFIER;

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Partition status getter
void GET_PARTITION_STATUS(PARTITION_STATUS_TYPE *PARTITION_STATUS, RETURN_CODE_TYPE *RETURN_CODE) {

	// Enters core
	ENTER_CORE();

	// Verifies current partition information
	if (_CURRENT_PARTITION_INFORMATION == null) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Sets partition status
	PARTITION_STATUS->IDENTIFIER = _CURRENT_PARTITION_INFORMATION->IDENTIFIER;
	PARTITION_STATUS->PERIOD = _CURRENT_PARTITION_INFORMATION->PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->PERIOD;
	PARTITION_STATUS->DURATION = _CURRENT_PARTITION_INFORMATION->PARTITION_CONFIGURATION->PARTITION_CONFIGURATION_SCHEDULE->DURATION;
	PARTITION_STATUS->LOCK_LEVEL = _CURRENT_PARTITION_INFORMATION->LOCK_LEVEL;
	PARTITION_STATUS->OPERATING_MODE = _CURRENT_PARTITION_INFORMATION->OPERATING_MODE;
	PARTITION_STATUS->START_CONDITION = _CURRENT_PARTITION_INFORMATION->START_CONDITION;

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Partition mode setter
void SET_PARTITION_MODE(OPERATING_MODE_TYPE OPERATING_MODE, RETURN_CODE_TYPE *RETURN_CODE) {
	PARTITION_CONFIGURATION_TYPE *PARTITION_CONFIGURATION;
	UINTBASE_TYPE *PTR_SOURCE;
	UINTBASE_TYPE *PTR_TARGET;

	// Enters core
	ENTER_CORE();

	// Verifies current partition information
	if (_CURRENT_PARTITION_INFORMATION == null) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies operating mode transition
	if (_CURRENT_PARTITION_INFORMATION->OPERATING_MODE == IDLE) {

		// No code could be running
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies operating mode
	if (OPERATING_MODE == IDLE) {

		// Reinitializes partition information
		INITIALIZE_PARTITION_INFORMATION(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, true);

		// Sets operating mode
		_CURRENT_PARTITION_INFORMATION->OPERATING_MODE = OPERATING_MODE;

		// Yields
		PORT_YIELD();

		// Partition will not be scheduled again because it's in IDLE operating mode

		// Must never be reached
		while (true) {
		}
	}

	// Verifies operating mode
	if ((OPERATING_MODE == COLD_START) || (OPERATING_MODE == WARM_START)) {

		// Verifies transition from cold start to warm start
		if ((_CURRENT_PARTITION_INFORMATION->OPERATING_MODE == COLD_START) && (OPERATING_MODE == WARM_START)) {

			// No transition from COLD_START to WARM_START is allowed
			*RETURN_CODE = INVALID_MODE;

			// Exits core
			EXIT_CORE();
			return;
		}

		// Gets partition configuration
		PARTITION_CONFIGURATION = _CURRENT_PARTITION_INFORMATION->PARTITION_CONFIGURATION;

		// Verifies health monitoring partition restart flag
		if (_CURRENT_PARTITION_INFORMATION->HEALTHMONITORING_PARTITION_RESTART) {
			_CURRENT_PARTITION_INFORMATION->START_CONDITION = HM_PARTITION_RESTART;
		} else {
			_CURRENT_PARTITION_INFORMATION->START_CONDITION = PARTITION_RESTART;
		}

		// Reinitializes partition information
		INITIALIZE_PARTITION_INFORMATION(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, true);

		// Sets operating mode
		_CURRENT_PARTITION_INFORMATION->OPERATING_MODE = OPERATING_MODE;

		// Prepares partition default process context
		PORT_PREPARECONTEXT(&_CURRENT_PARTITION_INFORMATION->CONTEXT);

		// Prepares source and target addresses
		PTR_SOURCE = (UINTBASE_TYPE *) PARTITION_CONFIGURATION->PARTITION_DATA_IMAGE_ADDRESS;
		PTR_TARGET = (UINTBASE_TYPE *) PARTITION_CONFIGURATION->PARTITION_DATA_ADDRESS;

		// Copies partition data image region back into partition data region
		for (; (UINTPOINTER_TYPE) PTR_TARGET < ((UINTPOINTER_TYPE) PARTITION_CONFIGURATION->PARTITION_DATA_ADDRESS + PARTITION_CONFIGURATION->PARTITION_DATA_SIZE);) {
			*PTR_TARGET = *PTR_SOURCE;

			// Moves addresses
			PTR_SOURCE++;
			PTR_TARGET++;
		}

		// Sets partition default process default system state
		_CURRENT_PARTITION_INFORMATION->CONTEXT.DEFAULT_SYSTEM_STATE = SYSTEMSTATE_PARTITIONDEFAULTPROCESS;

		// Resets partition default process system state
		_CURRENT_PARTITION_INFORMATION->CONTEXT.SYSTEM_STATE = _CURRENT_PARTITION_INFORMATION->CONTEXT.DEFAULT_SYSTEM_STATE;

		// Resets current context
		CURRENT_CONTEXT = &_CURRENT_PARTITION_INFORMATION->CONTEXT;

		// Switches into new context
		PORT_SWITCH();

		// Context will be restored back into initialization method

		// Must never be reached
		while (true) {
		}
	}

	// Verifies operating mode
	if (OPERATING_MODE == NORMAL) {

		// Verifies normal operation
		if (_CURRENT_PARTITION_INFORMATION->OPERATING_MODE == NORMAL) {

			// No transition
			*RETURN_CODE = NO_ACTION;

			// Exits core
			EXIT_CORE();
			return;
		}

		// Sets operating mode
		_CURRENT_PARTITION_INFORMATION->OPERATING_MODE = OPERATING_MODE;

		// Sets partition initialized flag
		_CURRENT_PARTITION_INFORMATION->INITIALIZED = true;

		// Starts process scheduler
		SCHEDULER_STARTPROCESSSCHEDULER(RETURN_CODE);

		// Sets partition default process default system state
		_CURRENT_PARTITION_INFORMATION->CONTEXT.DEFAULT_SYSTEM_STATE = SYSTEMSTATE_PARTITIONIDLEPROCESS;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Operating mode is not valid
	*RETURN_CODE = INVALID_PARAM;

	// Exits core
	EXIT_CORE();
}
