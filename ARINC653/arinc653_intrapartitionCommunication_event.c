// ARINC653 includes
#include "arinc653_core.h"

// Create event method
void CREATE_EVENT(EVENT_NAME_TYPE EVENT_NAME, EVENT_ID_TYPE *EVENT_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	EVENT_ID_TYPE EVENT_IDENTIFIER;
	EVENT_INFORMATION_TYPE *EVENT_INFORMATION;

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

	// Verifies operating mode
	if (_CURRENT_PARTITION_INFORMATION->OPERATING_MODE == NORMAL) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets free identifier
	EVENT_IDENTIFIER = _CURRENT_PARTITION_INFORMATION->EVENT_COUNT;

	// Verifies event count
	if (EVENT_IDENTIFIER >= _CURRENT_PARTITION_INFORMATION->PARTITION_CONFIGURATION->MAXIMUM_EVENT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets event identifier
	GET_EVENT_ID(EVENT_NAME, &EVENT_IDENTIFIER, RETURN_CODE);

	// Verifies event identifier
	if (*RETURN_CODE == NO_ERROR) {

		// Sets return code
		*RETURN_CODE = NO_ACTION;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets event information
	EVENT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_IDENTIFIER];

	// Allocates memory for name - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&_CURRENT_PARTITION_INFORMATION->REC_HEAP, (heapPOINTER *) &EVENT_INFORMATION->NAME, MAX_NAME_LENGTH, false);

	// Verifies name memory
	if (EVENT_INFORMATION->NAME == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Copies name
	if (!COMMON_COPYSTRING(EVENT_NAME, EVENT_INFORMATION->NAME, MAX_NAME_LENGTH)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Sets up event
	EVENT_INFORMATION->IDENTIFIER = _CURRENT_PARTITION_INFORMATION->EVENT_COUNT;
	// EVENT_INFORMATION->NAME = EVENT_NAME; // Already copied
	EVENT_INFORMATION->EVENT_STATE = DOWN;

	// Clears event priority queue
	PRIORITYQUEUE_CLEAR(&EVENT_INFORMATION->REC_EVENT, true, false);

	// Counts created event
	_CURRENT_PARTITION_INFORMATION->EVENT_COUNT++;

	// Sets event identifier
	*EVENT_ID = EVENT_IDENTIFIER;

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Set event method
void SET_EVENT(EVENT_ID_TYPE EVENT_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	EVENT_INFORMATION_TYPE *EVENT_INFORMATION;

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

	// Verifies event identifier
	if (EVENT_ID >= _CURRENT_PARTITION_INFORMATION->EVENT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets event information
	EVENT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_ID];

	// Sets event state
	EVENT_INFORMATION->EVENT_STATE = UP;

	// Verifies priority queue
	if (!PRIORITYQUEUE_ISEMPTY(&EVENT_INFORMATION->REC_EVENT)) {

		// Process release loop
		while (!PRIORITYQUEUE_ISEMPTY(&EVENT_INFORMATION->REC_EVENT)) {

			// Signals resource
			SIGNAL_RESOURCE(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, &EVENT_INFORMATION->REC_EVENT, false, RETURN_CODE);
		}

		// Verifies lock level
		if (_CURRENT_PARTITION_INFORMATION->LOCK_LEVEL == 0) {

			// Yields
			PORT_YIELD();
		}
	}

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Reset event method
void RESET_EVENT(EVENT_ID_TYPE EVENT_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	EVENT_INFORMATION_TYPE *EVENT_INFORMATION;

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

	// Verifies event identifier
	if (EVENT_ID >= _CURRENT_PARTITION_INFORMATION->EVENT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets event information
	EVENT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_ID];

	// Sets event state
	EVENT_INFORMATION->EVENT_STATE = DOWN;

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Wait event method
void WAIT_EVENT(EVENT_ID_TYPE EVENT_ID, SYSTEM_TIME_TYPE TIME_OUT, RETURN_CODE_TYPE *RETURN_CODE) {
	EVENT_INFORMATION_TYPE *EVENT_INFORMATION;

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

	// Verifies current process information
	if (_CURRENT_PROCESS_INFORMATION == null) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies event identifier
	if (EVENT_ID >= _CURRENT_PARTITION_INFORMATION->EVENT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets event information
	EVENT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_ID];

	// Verifies event state
	if (EVENT_INFORMATION->EVENT_STATE == UP) {

		// Sets return code
		*RETURN_CODE = NO_ERROR;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Waits for resource
	WAIT_RESOURCE(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, _CURRENT_PROCESS_INFORMATION->IDENTIFIER, &EVENT_INFORMATION->REC_EVENT, TIME_OUT, RETURN_CODE);

	// Exits core
	EXIT_CORE();
}

// Event identifier getter
void GET_EVENT_ID(EVENT_NAME_TYPE EVENT_NAME, EVENT_ID_TYPE *EVENT_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	EVENT_ID_TYPE EVENT_IDENTIFIER;

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

	// Iterates events
	for (EVENT_IDENTIFIER = 0; EVENT_IDENTIFIER < _CURRENT_PARTITION_INFORMATION->EVENT_COUNT; EVENT_IDENTIFIER++) {

		// Compares names
		if (COMMON_COMPARESTRINGS(EVENT_NAME, _CURRENT_PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_IDENTIFIER].NAME, MAX_NAME_LENGTH)) {

			// Sets event identifier
			*EVENT_ID = EVENT_IDENTIFIER;

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

// Event status getter
void GET_EVENT_STATUS(EVENT_ID_TYPE EVENT_ID, EVENT_STATUS_TYPE *EVENT_STATUS, RETURN_CODE_TYPE *RETURN_CODE) {
	EVENT_INFORMATION_TYPE *EVENT_INFORMATION;

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

	// Verifies event identifier
	if (EVENT_ID >= _CURRENT_PARTITION_INFORMATION->EVENT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets event information
	EVENT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->EVENT_INFORMATION[EVENT_ID];

	// Copies status
	EVENT_STATUS->EVENT_STATE = EVENT_INFORMATION->EVENT_STATE;
	EVENT_STATUS->WAITING_PROCESSES = PRIORITYQUEUE_GETCOUNT(&EVENT_INFORMATION->REC_EVENT);

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}
