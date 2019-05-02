// ARINC653 includes
#include "arinc653_core.h"

// Partition queuing port identifier getter
void GET_PARTITION_QUEUING_PORT_ID(PARTITION_ID_TYPE PARTITION_ID, QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME, QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	PARTITION_INFORMATION_TYPE *PARTITION_INFORMATION;
	QUEUING_PORT_ID_TYPE QUEUINGPORT_IDENTIFIER;

	// Enters core
	ENTER_CORE();

	// Verifies partition identifier
	if (PARTITION_ID >= MODULE_INFORMATION.PARTITION_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets partition information
	PARTITION_INFORMATION = &MODULE_INFORMATION.PARTITION_INFORMATION[PARTITION_ID];

	// Iterates queuing ports
	for (QUEUINGPORT_IDENTIFIER = 0; QUEUINGPORT_IDENTIFIER < PARTITION_INFORMATION->QUEUINGPORT_COUNT; QUEUINGPORT_IDENTIFIER++) {

		// Compares names
		if (COMMON_COMPARESTRINGS(QUEUING_PORT_NAME, PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUINGPORT_IDENTIFIER].NAME, MAX_NAME_LENGTH)) {

			// Sets queuing port identifier
			*QUEUING_PORT_ID = QUEUINGPORT_IDENTIFIER;

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

// Send queuing message method
void SEND_PARTITION_QUEUING_MESSAGE(PARTITION_ID_TYPE PARTITION_ID, QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, MESSAGE_ADDR_TYPE MESSAGE_ADDR, MESSAGE_SIZE_TYPE LENGTH, RETURN_CODE_TYPE *RETURN_CODE) {
	PARTITION_INFORMATION_TYPE *PARTITION_INFORMATION;
	QUEUINGPORT_INFORMATION_TYPE *QUEUINGPORT_INFORMATION;
	INDEX_TYPE MESSAGE_INDEX;
	INDEX_TYPE CHARACTER_INDEX;

	// Enters core
	ENTER_CORE();

	// Verifies partition identifier
	if (PARTITION_ID >= MODULE_INFORMATION.PARTITION_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets partition information
	PARTITION_INFORMATION = &MODULE_INFORMATION.PARTITION_INFORMATION[PARTITION_ID];

	// Verifies queuing port identifier
	if (QUEUING_PORT_ID >= PARTITION_INFORMATION->QUEUINGPORT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets queuing port information
	QUEUINGPORT_INFORMATION = &PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUING_PORT_ID];

	// Verifies length
	if (LENGTH > QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies port direction
	if (QUEUINGPORT_INFORMATION->PORT_DIRECTION != DESTINATION) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies message count and queuing port priority queue
	if (QUEUINGPORT_INFORMATION->MESSAGE_COUNT >= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE) {

		// Sets overflow flag
		QUEUINGPORT_INFORMATION->OVERFLOW = true;

		// Sets return code
		*RETURN_CODE = NOT_AVAILABLE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Calculates message index
	MESSAGE_INDEX = QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX + QUEUINGPORT_INFORMATION->MESSAGE_COUNT;

	// Fixes message index
	if (MESSAGE_INDEX >= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE) {
		MESSAGE_INDEX -= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE;
	}

	// Copies message into queue
	for (CHARACTER_INDEX = 0; CHARACTER_INDEX < LENGTH; CHARACTER_INDEX++) {
		QUEUINGPORT_INFORMATION->MESSAGES[(MESSAGE_INDEX * QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE) + CHARACTER_INDEX] = MESSAGE_ADDR[CHARACTER_INDEX];
	}
	QUEUINGPORT_INFORMATION->MESSAGES_LENGTH[MESSAGE_INDEX] = LENGTH;

	// Counts message
	QUEUINGPORT_INFORMATION->MESSAGE_COUNT++;

	// Verifies queuing port priority queue
	if (!PRIORITYQUEUE_ISEMPTY(&QUEUINGPORT_INFORMATION->REC_QUEUINGPORT)) {

		// Signals resource
		SIGNAL_RESOURCE(PARTITION_INFORMATION->IDENTIFIER, &QUEUINGPORT_INFORMATION->REC_QUEUINGPORT, false, RETURN_CODE);
	}

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Receive partition queuing message method
void RECEIVE_PARTITION_QUEUING_MESSAGE(PARTITION_ID_TYPE PARTITION_ID, QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, MESSAGE_ADDR_TYPE MESSAGE_ADDR, MESSAGE_SIZE_TYPE *LENGTH, BOOLEAN_TYPE PEEK, RETURN_CODE_TYPE *RETURN_CODE) {
	PARTITION_INFORMATION_TYPE *PARTITION_INFORMATION;
	QUEUINGPORT_INFORMATION_TYPE *QUEUINGPORT_INFORMATION;
	INDEX_TYPE CHARACTER_INDEX;

	// Enters core
	ENTER_CORE();

	// Verifies partition identifier
	if (PARTITION_ID >= MODULE_INFORMATION.PARTITION_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets partition information
	PARTITION_INFORMATION = &MODULE_INFORMATION.PARTITION_INFORMATION[PARTITION_ID];

	// Verifies queuing port identifier
	if (QUEUING_PORT_ID >= PARTITION_INFORMATION->QUEUINGPORT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets queuing port information
	QUEUINGPORT_INFORMATION = &PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUING_PORT_ID];

	// Verifies port direction
	if (QUEUINGPORT_INFORMATION->PORT_DIRECTION != SOURCE) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies message count
	if (QUEUINGPORT_INFORMATION->MESSAGE_COUNT <= 0) {

		// Sets return code
		*RETURN_CODE = NOT_AVAILABLE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Copies message
	*LENGTH = QUEUINGPORT_INFORMATION->MESSAGES_LENGTH[QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX];
	for (CHARACTER_INDEX = 0; CHARACTER_INDEX < *LENGTH; CHARACTER_INDEX++) {
		MESSAGE_ADDR[CHARACTER_INDEX] = QUEUINGPORT_INFORMATION->MESSAGES[(QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX * QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE) + CHARACTER_INDEX];
	}

	// Verifies peek flag
	if (!PEEK) {

		// Counts message
		QUEUINGPORT_INFORMATION->MESSAGE_COUNT--;

		// Moves next message
		QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX++;

		// Fixes message index
		if (QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX >= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE) {
			QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX -= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE;
		}

		// Verifies queuing port priority queue
		if (!PRIORITYQUEUE_ISEMPTY(&QUEUINGPORT_INFORMATION->REC_QUEUINGPORT)) {

			// Signals resource
			SIGNAL_RESOURCE(PARTITION_INFORMATION->IDENTIFIER, &QUEUINGPORT_INFORMATION->REC_QUEUINGPORT, false, RETURN_CODE);
		}
	}

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Create queuing port method
void CREATE_QUEUING_PORT(QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME, MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE, MESSAGE_RANGE_TYPE MAX_NB_MESSAGE, PORT_DIRECTION_TYPE PORT_DIRECTION, QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE, QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID, RETURN_CODE_TYPE *RETURN_CODE) {
	QUEUING_PORT_ID_TYPE QUEUING_PORT_IDENTIFIER;
	QUEUINGPORT_INFORMATION_TYPE *QUEUINGPORT_INFORMATION;

	// The following validations are not performed, assuming that the code is generated based on a valid configuration file:

	// 1) If no queuing port of the partition is named QUEUING_PORT_NAME in the configuration, RETURN_CODE := INVALID_CONFIG
	// 2) If MAX_MESSAGE_SIZE is not equal to the value specified in the configuration data, RETURN_CODE := INVALID_CONFIG
	// 3) If MAX_NB_MESSAGE is not equal to the value specified in the configuration data, RETURN_CODE := INVALID_CONFIG
	// 4) If PORT_DIRECTION is not equal to the value specified in the configuration data, RETURN_CODE := INVALID_CONFIG

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
	QUEUING_PORT_IDENTIFIER = _CURRENT_PARTITION_INFORMATION->QUEUINGPORT_COUNT;

	// Verifies queuing port count
	if (QUEUING_PORT_IDENTIFIER >= _CURRENT_PARTITION_INFORMATION->PARTITION_CONFIGURATION->MAXIMUM_QUEUINGPORT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets queuing port identifier
	GET_QUEUING_PORT_ID(QUEUING_PORT_NAME, &QUEUING_PORT_IDENTIFIER, RETURN_CODE);

	// Verifies queuing port identifier
	if (*RETURN_CODE == NO_ERROR) {

		// Sets return code
		*RETURN_CODE = NO_ACTION;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies maximum message count
	if ((MAX_NB_MESSAGE < PORT_INTERPARTITIONCOMMUNICATION_QUEUINGPORT_MINIMUMMESSAGECOUNT) || (MAX_NB_MESSAGE > PORT_INTERPARTITIONCOMMUNICATION_QUEUINGPORT_MAXIMUMMESSAGECOUNT)) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies maximum message size
	if ((MAX_MESSAGE_SIZE < PORT_INTERPARTITIONCOMMUNICATION_QUEUINGPORT_MINIMUMMESSAGESIZE) || (MAX_MESSAGE_SIZE > PORT_INTERPARTITIONCOMMUNICATION_QUEUINGPORT_MAXIMUMMESSAGESIZE)) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies port direction
	if ((PORT_DIRECTION != SOURCE) && (PORT_DIRECTION != DESTINATION)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies queuing discipline
	if ((QUEUING_DISCIPLINE != FIFO) && (QUEUING_DISCIPLINE != PRIORITY)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets queuing port information
	QUEUINGPORT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUING_PORT_IDENTIFIER];

	// Allocates memory for messages - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&_CURRENT_PARTITION_INFORMATION->REC_HEAP, (heapPOINTER *) &QUEUINGPORT_INFORMATION->MESSAGES, MAX_NB_MESSAGE * MAX_MESSAGE_SIZE, false);

	// Verifies messages memory
	if (QUEUINGPORT_INFORMATION->MESSAGES == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Allocates memory for message lengths - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&_CURRENT_PARTITION_INFORMATION->REC_HEAP, (heapPOINTER *) &QUEUINGPORT_INFORMATION->MESSAGES_LENGTH, MAX_NB_MESSAGE * sizeof(MESSAGE_SIZE_TYPE), false);

	// Verifies message lengths memory
	if (QUEUINGPORT_INFORMATION->MESSAGES_LENGTH == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Allocates memory for name - If pointer is not null, keeps it unchanged
	HEAP_ALLOCATE(&_CURRENT_PARTITION_INFORMATION->REC_HEAP, (heapPOINTER *) &QUEUINGPORT_INFORMATION->NAME, MAX_NAME_LENGTH, false);

	// Verifies name memory
	if (QUEUINGPORT_INFORMATION->NAME == null) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Copies name
	if (!COMMON_COPYSTRING(QUEUING_PORT_NAME, QUEUINGPORT_INFORMATION->NAME, MAX_NAME_LENGTH)) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Sets up queuing port
	QUEUINGPORT_INFORMATION->IDENTIFIER = _CURRENT_PARTITION_INFORMATION->QUEUINGPORT_COUNT;
	// QUEUINGPORT_INFORMATION->NAME = QUEUING_PORT_NAME; // Already copied
	QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE = MAX_MESSAGE_SIZE;
	QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE = MAX_NB_MESSAGE;
	QUEUINGPORT_INFORMATION->PORT_DIRECTION = PORT_DIRECTION;
	QUEUINGPORT_INFORMATION->QUEUING_DISCIPLINE = QUEUING_DISCIPLINE;

	// Clears queuing port priority queue
	PRIORITYQUEUE_CLEAR(&QUEUINGPORT_INFORMATION->REC_QUEUINGPORT, (QUEUING_DISCIPLINE == PRIORITY ? true : false), false);

	// Counts created queuing port
	_CURRENT_PARTITION_INFORMATION->QUEUINGPORT_COUNT++;

	// Sets queuing port identifier
	*QUEUING_PORT_ID = QUEUING_PORT_IDENTIFIER;

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Send queuing message method
void SEND_QUEUING_MESSAGE(QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, MESSAGE_ADDR_TYPE MESSAGE_ADDR, MESSAGE_SIZE_TYPE LENGTH, SYSTEM_TIME_TYPE TIME_OUT, RETURN_CODE_TYPE *RETURN_CODE) {
	QUEUINGPORT_INFORMATION_TYPE *QUEUINGPORT_INFORMATION;
	INDEX_TYPE MESSAGE_INDEX;
	INDEX_TYPE CHARACTER_INDEX;

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

	// Verifies queuing port identifier
	if (QUEUING_PORT_ID >= _CURRENT_PARTITION_INFORMATION->QUEUINGPORT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets queuing port information
	QUEUINGPORT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUING_PORT_ID];

	// Verifies length
	if (LENGTH > QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE) {

		// Sets return code
		*RETURN_CODE = INVALID_CONFIG;

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

	// Verifies port direction
	if (QUEUINGPORT_INFORMATION->PORT_DIRECTION != SOURCE) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies message count and queuing port priority queue
	if ((QUEUINGPORT_INFORMATION->MESSAGE_COUNT >= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE) || (!PRIORITYQUEUE_ISEMPTY(&QUEUINGPORT_INFORMATION->REC_QUEUINGPORT))) {

		// Verifies current process information
		if (_CURRENT_PROCESS_INFORMATION == null) {

			// Sets return code
			*RETURN_CODE = NOT_AVAILABLE;

			// Exits core
			EXIT_CORE();
			return;
		}

		// Waits for resource
		WAIT_RESOURCE(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, _CURRENT_PROCESS_INFORMATION->IDENTIFIER, &QUEUINGPORT_INFORMATION->REC_QUEUINGPORT, TIME_OUT, RETURN_CODE);

		// Verifies return code
		if (*RETURN_CODE != NO_ERROR) {

			// Exits core
			EXIT_CORE();
			return;
		}
	}

	// Calculates message index
	MESSAGE_INDEX = QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX + QUEUINGPORT_INFORMATION->MESSAGE_COUNT;

	// Fixes message index
	if (MESSAGE_INDEX >= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE) {
		MESSAGE_INDEX -= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE;
	}

	// Copies message into queue
	for (CHARACTER_INDEX = 0; CHARACTER_INDEX < LENGTH; CHARACTER_INDEX++) {
		QUEUINGPORT_INFORMATION->MESSAGES[(MESSAGE_INDEX * QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE) + CHARACTER_INDEX] = MESSAGE_ADDR[CHARACTER_INDEX];
	}
	QUEUINGPORT_INFORMATION->MESSAGES_LENGTH[MESSAGE_INDEX] = LENGTH;

	// Counts message
	QUEUINGPORT_INFORMATION->MESSAGE_COUNT++;

	// Verifies queuing port priority queue
	if (!PRIORITYQUEUE_ISEMPTY(&QUEUINGPORT_INFORMATION->REC_QUEUINGPORT)) {

		// Signals resource
		SIGNAL_RESOURCE(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, &QUEUINGPORT_INFORMATION->REC_QUEUINGPORT, true, RETURN_CODE);
	}

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}

// Receive queuing message method
void RECEIVE_QUEUING_MESSAGE(QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, SYSTEM_TIME_TYPE TIME_OUT, MESSAGE_ADDR_TYPE MESSAGE_ADDR, MESSAGE_SIZE_TYPE *LENGTH, RETURN_CODE_TYPE *RETURN_CODE) {
	QUEUINGPORT_INFORMATION_TYPE *QUEUINGPORT_INFORMATION;
	INDEX_TYPE CHARACTER_INDEX;

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

	// Verifies queuing port identifier
	if (QUEUING_PORT_ID >= _CURRENT_PARTITION_INFORMATION->QUEUINGPORT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets queuing port information
	QUEUINGPORT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUING_PORT_ID];

	// Verifies timeout
	if ((TIME_OUT < 0) && (TIME_OUT != INFINITE_TIME_VALUE)) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies port direction
	if (QUEUINGPORT_INFORMATION->PORT_DIRECTION != DESTINATION) {

		// Sets return code
		*RETURN_CODE = INVALID_MODE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies message count
	if (QUEUINGPORT_INFORMATION->MESSAGE_COUNT > 0) {

		// Copies message
		*LENGTH = QUEUINGPORT_INFORMATION->MESSAGES_LENGTH[QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX];
		for (CHARACTER_INDEX = 0; CHARACTER_INDEX < *LENGTH; CHARACTER_INDEX++) {
			MESSAGE_ADDR[CHARACTER_INDEX] = QUEUINGPORT_INFORMATION->MESSAGES[(QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX * QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE) + CHARACTER_INDEX];
		}

		// Counts message
		QUEUINGPORT_INFORMATION->MESSAGE_COUNT--;

		// Moves next message
		QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX++;

		// Fixes message index
		if (QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX >= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE) {
			QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX -= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE;
		}

		// Verifies overflow flag
		if (QUEUINGPORT_INFORMATION->OVERFLOW) {

			// Clears overflow flag (NOT MENTIONED IN THE SPECIFICATION)
			QUEUINGPORT_INFORMATION->OVERFLOW = false;

			// Sets return code
			*RETURN_CODE = INVALID_CONFIG;
		} else {

			// Sets return code
			*RETURN_CODE = NO_ERROR;
		}

		// Exits core
		EXIT_CORE();
		return;
	}

	// Verifies current process information
	if (_CURRENT_PROCESS_INFORMATION == null) {

		// Sets return code
		*RETURN_CODE = NOT_AVAILABLE;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Waits for resource
	WAIT_RESOURCE(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, _CURRENT_PROCESS_INFORMATION->IDENTIFIER, &QUEUINGPORT_INFORMATION->REC_QUEUINGPORT, TIME_OUT, RETURN_CODE);

	// Verifies return code
	if (*RETURN_CODE != NO_ERROR) {

		// Exits core
		EXIT_CORE();
		return;
	}

	// Copies message
	*LENGTH = QUEUINGPORT_INFORMATION->MESSAGES_LENGTH[QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX];
	for (CHARACTER_INDEX = 0; CHARACTER_INDEX < *LENGTH; CHARACTER_INDEX++) {
		MESSAGE_ADDR[CHARACTER_INDEX] = QUEUINGPORT_INFORMATION->MESSAGES[(QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX * QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE) + CHARACTER_INDEX];
	}

	// Counts message
	QUEUINGPORT_INFORMATION->MESSAGE_COUNT--;

	// Moves next message
	QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX++;

	// Fixes message index
	if (QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX >= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE) {
		QUEUINGPORT_INFORMATION->NEXT_MESSAGE_INDEX -= QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE;
	}

	// Verifies queuing port priority queue
	if (!PRIORITYQUEUE_ISEMPTY(&QUEUINGPORT_INFORMATION->REC_QUEUINGPORT)) {

		// Signals resource
		SIGNAL_RESOURCE(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, &QUEUINGPORT_INFORMATION->REC_QUEUINGPORT, true, RETURN_CODE);
	}

	// Exits core
	EXIT_CORE();
}

// Queuing port identifier getter
void GET_QUEUING_PORT_ID(QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME, QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID, RETURN_CODE_TYPE *RETURN_CODE) {

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

	// Gets partition queuing port identifier
	GET_PARTITION_QUEUING_PORT_ID(_CURRENT_PARTITION_INFORMATION->IDENTIFIER, QUEUING_PORT_NAME, QUEUING_PORT_ID, RETURN_CODE);

	// Exits core
	EXIT_CORE();
}

// Queuing port status getter
void GET_QUEUING_PORT_STATUS(QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, QUEUING_PORT_STATUS_TYPE *QUEUING_PORT_STATUS, RETURN_CODE_TYPE *RETURN_CODE) {
	QUEUINGPORT_INFORMATION_TYPE *QUEUINGPORT_INFORMATION;

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

	// Verifies queuing port identifier
	if (QUEUING_PORT_ID >= _CURRENT_PARTITION_INFORMATION->QUEUINGPORT_COUNT) {

		// Sets return code
		*RETURN_CODE = INVALID_PARAM;

		// Exits core
		EXIT_CORE();
		return;
	}

	// Gets queuing port information
	QUEUINGPORT_INFORMATION = &_CURRENT_PARTITION_INFORMATION->QUEUINGPORT_INFORMATION[QUEUING_PORT_ID];

	// Copies status
	QUEUING_PORT_STATUS->NB_MESSAGE = QUEUINGPORT_INFORMATION->MESSAGE_COUNT;
	QUEUING_PORT_STATUS->MAX_NB_MESSAGE = QUEUINGPORT_INFORMATION->MAX_NB_MESSAGE;
	QUEUING_PORT_STATUS->MAX_MESSAGE_SIZE = QUEUINGPORT_INFORMATION->MAX_MESSAGE_SIZE;
	QUEUING_PORT_STATUS->PORT_DIRECTION = QUEUINGPORT_INFORMATION->PORT_DIRECTION;
	QUEUING_PORT_STATUS->WAITING_PROCESSES = PRIORITYQUEUE_GETCOUNT(&QUEUINGPORT_INFORMATION->REC_QUEUINGPORT);

	// Sets return code
	*RETURN_CODE = NO_ERROR;

	// Exits core
	EXIT_CORE();
}