// ARINC653 includes
#include "arinc653_core.h"

// Create semaphore method
void CREATE_SEMAPHORE(SEMAPHORE_NAME_TYPE SEMAPHORE_NAME, SEMAPHORE_VALUE_TYPE CURRENT_VALUE, SEMAPHORE_VALUE_TYPE MAXIMUM_VALUE, QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE, SEMAPHORE_ID_TYPE *SEMAPHORE_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	SEMAPHORE_ID_TYPE SEMAPHORE_IDENTIFIER;
	SEMAPHORE_INFORMATION_TYPE *SEMAPHORE_INFORMATION;

	// Enters core
	ENTER_CORE();

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

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
	SEMAPHORE_IDENTIFIER = _CURRENT_PARTITION_INFORMATION->SEMAPHORE_COUNT;

	// Verifies semaphore count
	if (SEMAPHORE_IDENTIFIER >= _CURRENT_PARTITION_INFORMATION->PARTITION_CONFIGURATION->MAXIMUM_SEMAPHORE_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets semaphore identifier
	GET_SEMAPHORE_ID(SEMAPHORE_NAME, &SEMAPHORE_IDENTIFIER, RETURN_CODE);

	// Verifies semaphore identifier
	if (*RETURN_CODE == NO_ERROR) {

		// Sets return code
		*RETURN_CODE = NO_ACTION;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies current value
	if (CURRENT_VALUE > PORT_INTRAPARTITIONCOMMUNICATION_SEMAPHORE_MAXIMUMVALUE) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies maximum value
	if (MAXIMUM_VALUE > PORT_INTRAPARTITIONCOMMUNICATION_SEMAPHORE_MAXIMUMVALUE) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies current and maximum values
	if (CURRENT_VALUE > MAXIMUM_VALUE) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies queuing discipline
	if ((QUEUING_DISCIPLINE != FIFO) && (QUEUING_DISCIPLINE != PRIORITY)) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets semaphore information
	SEMAPHORE_INFORMATION = &_CURRENT_PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER];

	// Allocates memory for name - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&_CURRENT_PARTITION_INFORMATION->REC_HEAP, (heapPOINTER *) &SEMAPHORE_INFORMATION->NAME, MAX_NAME_LENGTH, false);

	// Verifies name memory
	if (SEMAPHORE_INFORMATION->NAME == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Copies name
	if (!COMMON_COPYSTRING(SEMAPHORE_NAME, SEMAPHORE_INFORMATION->NAME, MAX_NAME_LENGTH)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Sets up semaphore
	SEMAPHORE_INFORMATION->IDENTIFIER = _CURRENT_PARTITION_INFORMATION->SEMAPHORE_COUNT;
	// SEMAPHORE_INFORMATION->NAME = SEMAPHORE_NAME; // Already copied
	SEMAPHORE_INFORMATION->CURRENT_VALUE = CURRENT_VALUE;
	SEMAPHORE_INFORMATION->MAXIMUM_VALUE = MAXIMUM_VALUE;
	SEMAPHORE_INFORMATION->QUEUING_DISCIPLINE = QUEUING_DISCIPLINE;

	// Clears semaphore priority queue
	PRIORITYQUEUE_CLEAR(&SEMAPHORE_INFORMATION->REC_SEMAPHORE, (QUEUING_DISCIPLINE == PRIORITY ? true : false), false);

	// Counts created semaphore
	_CURRENT_PARTITION_INFORMATION->SEMAPHORE_COUNT++;

	// Sets semaphore identifier
	*SEMAPHORE_ID = SEMAPHORE_IDENTIFIER;

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Wait semaphore method
void WAIT_SEMAPHORE(SEMAPHORE_ID_TYPE SEMAPHORE_ID, SYSTEM_TIME_TYPE TIME_OUT, RETURN_CODE_TYPE *RETURN_CODE) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	SEMAPHORE_INFORMATION_TYPE *SEMAPHORE_INFORMATION;

	// Enters core
	ENTER_CORE();

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

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

	// Verifies timeout
	if ((TIME_OUT < 0) && (TIME_OUT != INFINITE_TIME_VALUE)) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies semaphore identifier
	if (SEMAPHORE_ID >= _CURRENT_PARTITION_INFORMATION->SEMAPHORE_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets semaphore information
	SEMAPHORE_INFORMATION = &_CURRENT_PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_ID];

	// Verifies current value
	if (SEMAPHORE_INFORMATION->CURRENT_VALUE > 0) {

		// Decrements current value
		SEMAPHORE_INFORMATION->CURRENT_VALUE--;

		// Sets return code
		*RETURN_CODE = NO_ERROR;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Waits for resource
	WAIT_RESOURCE(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, _CURRENT_PROCESS_INFORMATION->IDENTIFIER, &SEMAPHORE_INFORMATION->REC_SEMAPHORE, TIME_OUT, RETURN_CODE);

	// Exits core
	EXIT_CORE();
}

// Signal semaphore method
void SIGNAL_SEMAPHORE(SEMAPHORE_ID_TYPE SEMAPHORE_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	SEMAPHORE_INFORMATION_TYPE *SEMAPHORE_INFORMATION;

	// Enters core
	ENTER_CORE();

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Verifies current partition information
	if (_CURRENT_PARTITION_INFORMATION == null) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies semaphore identifier
	if (SEMAPHORE_ID >= _CURRENT_PARTITION_INFORMATION->SEMAPHORE_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets semaphore information
	SEMAPHORE_INFORMATION = &_CURRENT_PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_ID];

	// Verifies current value
	if (SEMAPHORE_INFORMATION->CURRENT_VALUE == SEMAPHORE_INFORMATION->MAXIMUM_VALUE) {

		// Sets return code
		*RETURN_CODE = NO_ACTION;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies semaphore priority queue
	if (PRIORITYQUEUE_ISEMPTY(&SEMAPHORE_INFORMATION->REC_SEMAPHORE)) {

		// Increments current value
		SEMAPHORE_INFORMATION->CURRENT_VALUE++;

		// Sets return code
		*RETURN_CODE = NO_ERROR;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies priority queue
	if (!PRIORITYQUEUE_ISEMPTY(&SEMAPHORE_INFORMATION->REC_SEMAPHORE)) {

		// Signals resource
		SIGNAL_RESOURCE(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, &SEMAPHORE_INFORMATION->REC_SEMAPHORE, false, RETURN_CODE);

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

// Semaphore identifier getter
void GET_SEMAPHORE_ID(SEMAPHORE_NAME_TYPE SEMAPHORE_NAME, SEMAPHORE_ID_TYPE *SEMAPHORE_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	SEMAPHORE_ID_TYPE SEMAPHORE_IDENTIFIER;

	// Enters core
	ENTER_CORE();

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Verifies current partition information
	if (_CURRENT_PARTITION_INFORMATION == null) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Iterates semaphores
	for (SEMAPHORE_IDENTIFIER = 0; SEMAPHORE_IDENTIFIER < _CURRENT_PARTITION_INFORMATION->SEMAPHORE_COUNT; SEMAPHORE_IDENTIFIER++) {

		// Compares names
		if (COMMON_COMPARESTRINGS(SEMAPHORE_NAME, _CURRENT_PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_IDENTIFIER].NAME, MAX_NAME_LENGTH)) {

			// Sets semaphore identifier
			*SEMAPHORE_ID = SEMAPHORE_IDENTIFIER;

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

// Semaphore status getter
void GET_SEMAPHORE_STATUS(SEMAPHORE_ID_TYPE SEMAPHORE_ID, SEMAPHORE_STATUS_TYPE *SEMAPHORE_STATUS, RETURN_CODE_TYPE *RETURN_CODE) {
	MODULE_INFORMATION_TYPE *MODULE_INFORMATION;
	SEMAPHORE_INFORMATION_TYPE *SEMAPHORE_INFORMATION;

	// Enters core
	ENTER_CORE();

	// Gets module information
	MODULE_INFORMATION = _CORE_MODULE_INFORMATION;

	// Verifies current partition information
	if (_CURRENT_PARTITION_INFORMATION == null) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies semaphore identifier
	if (SEMAPHORE_ID >= _CURRENT_PARTITION_INFORMATION->SEMAPHORE_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets semaphore information
	SEMAPHORE_INFORMATION = &_CURRENT_PARTITION_INFORMATION->SEMAPHORE_INFORMATION[SEMAPHORE_ID];

	// Copies status
	SEMAPHORE_STATUS->CURRENT_VALUE = SEMAPHORE_INFORMATION->CURRENT_VALUE;
	SEMAPHORE_STATUS->MAXIMUM_VALUE = SEMAPHORE_INFORMATION->MAXIMUM_VALUE;
	SEMAPHORE_STATUS->WAITING_PROCESSES = PRIORITYQUEUE_GETCOUNT(&SEMAPHORE_INFORMATION->REC_SEMAPHORE);

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}
