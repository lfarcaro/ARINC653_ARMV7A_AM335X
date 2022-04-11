#ifndef ARINC653_CORE_H_
#define ARINC653_CORE_H_

// ARINC653 includes
#include "arinc653.h"
#include "arinc653_configuration.h"
#include "arinc653_clock.h"
#include "arinc653_scheduler.h"
#include "arinc653_common.h"
#include "arinc653_heap.h"
#include "arinc653_priorityqueue.h"
#include "arinc653_partitionManagement.h"
#include "arinc653_processManagement.h"
#include "arinc653_healthMonitoring.h"
#include "arinc653_interpartitionCommunication_samplingPort.h"
#include "arinc653_interpartitionCommunication_queuingPort.h"

// ARINC653 port includes
#include "arinc653_port.h"

// -------------------- OPTIONAL MODULES START --------------------

// Interpartition communication - Sampling port - Sampling port information
struct _SAMPLINGPORT_INFORMATION_TYPE {
	SAMPLING_PORT_ID_TYPE IDENTIFIER;
	SAMPLING_PORT_NAME_TYPE NAME;
	MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
	PORT_DIRECTION_TYPE PORT_DIRECTION; // Port direction in the partition
	SYSTEM_TIME_TYPE REFRESH_PERIOD;
	SYSTEM_TIME_TYPE LAST_READ; // Stores the time at which the message was last read
	SYSTEM_TIME_TYPE LAST_WRITE; // Stores the time at which the message was last written
	VALIDITY_TYPE LAST_VALIDITY; // Stores the last message validity
	EMPTY_INDICATOR_TYPE EMPTY_INDICATOR;
	CHARACTER_TYPE *MESSAGE; // Stores the current message
	MESSAGE_SIZE_TYPE LENGTH; // Stores the current message length
};
typedef struct _SAMPLINGPORT_INFORMATION_TYPE SAMPLINGPORT_INFORMATION_TYPE;

// Interpartition communication - Queuing port - Queuing port information
struct _QUEUINGPORT_INFORMATION_TYPE {
	QUEUING_PORT_ID_TYPE IDENTIFIER;
	QUEUING_PORT_NAME_TYPE NAME;
	MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
	MESSAGE_RANGE_TYPE MAX_NB_MESSAGE; // Maximum number of messages
	PORT_DIRECTION_TYPE PORT_DIRECTION; // Port direction in the partition
	QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE; // Process queuing discipline
	SIZE_TYPE MESSAGE_COUNT; // Current number of messages
	INDEX_TYPE NEXT_MESSAGE_INDEX; // Next message index to read
	BOOLEAN_TYPE OVERFLOW; // Indicates if the queue has overflowed since it was last read
	CHARACTER_TYPE *MESSAGES; // Points a region of size MAX_MESSAGE_SIZE*MAX_NB_MESSAGE that stores the messages in FIFO order
	MESSAGE_SIZE_TYPE *MESSAGES_LENGTH; // Stores the message lengths in same order as in MESSAGES
	priorityqueueRECORD REC_QUEUINGPORT; // Queuing port priority queue - Sorts processes according to QUEUING_DISCIPLINE
};
typedef struct _QUEUINGPORT_INFORMATION_TYPE QUEUINGPORT_INFORMATION_TYPE;

// Intrapartition communication - Buffer - Buffer information
struct _BUFFER_INFORMATION_TYPE {
	BUFFER_ID_TYPE IDENTIFIER;
	BUFFER_NAME_TYPE NAME;
	MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
	MESSAGE_RANGE_TYPE MAX_NB_MESSAGE; // Maximum number of messages
	QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE; // Process queuing discipline
	SIZE_TYPE MESSAGE_COUNT; // Current number of messages
	INDEX_TYPE NEXT_MESSAGE_INDEX; // Next message index to read
	CHARACTER_TYPE *MESSAGES; // Points a region of size MAX_MESSAGE_SIZE*MAX_NB_MESSAGE that stores the messages in FIFO order
	MESSAGE_SIZE_TYPE *MESSAGES_LENGTH; // Stores the message lengths in same order as in MESSAGES
	priorityqueueRECORD REC_BUFFER; // Buffer priority queue - Sorts processes according to QUEUING_DISCIPLINE
};
typedef struct _BUFFER_INFORMATION_TYPE BUFFER_INFORMATION_TYPE;

// Intrapartition communication - Blackboard - Blackboard information
struct _BLACKBOARD_INFORMATION_TYPE {
	BLACKBOARD_ID_TYPE IDENTIFIER;
	BLACKBOARD_NAME_TYPE NAME;
	EMPTY_INDICATOR_TYPE EMPTY_INDICATOR;
	MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
	CHARACTER_TYPE *MESSAGE; // Stores the current message
	MESSAGE_SIZE_TYPE LENGTH; // Stores the current message length
	priorityqueueRECORD REC_BLACKBOARD; // Blackboard priority queue - Sorts processes by descending priority
};
typedef struct _BLACKBOARD_INFORMATION_TYPE BLACKBOARD_INFORMATION_TYPE;

// Intrapartition communication - Semaphore - Semaphore information
struct _SEMAPHORE_INFORMATION_TYPE {
	SEMAPHORE_ID_TYPE IDENTIFIER;
	SEMAPHORE_NAME_TYPE NAME;
	SEMAPHORE_VALUE_TYPE CURRENT_VALUE;
	SEMAPHORE_VALUE_TYPE MAXIMUM_VALUE;
	QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE; // Process queuing discipline
	priorityqueueRECORD REC_SEMAPHORE; // Semaphore priority queue - Sorts processes according to QUEUING_DISCIPLINE
};
typedef struct _SEMAPHORE_INFORMATION_TYPE SEMAPHORE_INFORMATION_TYPE;

// Intrapartition communication - Event - Event information
struct _EVENT_INFORMATION_TYPE {
	EVENT_ID_TYPE IDENTIFIER;
	EVENT_NAME_TYPE NAME;
	EVENT_STATE_TYPE EVENT_STATE; // Current event state (up or down)
	priorityqueueRECORD REC_EVENT; // Event priority queue - Sorts processes by descending priority
};
typedef struct _EVENT_INFORMATION_TYPE EVENT_INFORMATION_TYPE;

// -------------------- OPTIONAL MODULES END --------------------

// Process information
struct _PROCESS_INFORMATION_TYPE {
	PROCESS_ATTRIBUTE_TYPE *PROCESS_ATTRIBUTE;
	PROCESS_CONFIGURATION_TYPE *PROCESS_CONFIGURATION;
	PROCESS_ID_TYPE IDENTIFIER;
	PRIORITY_TYPE CURRENT_PRIORITY;
	PROCESS_STATE_TYPE PROCESS_STATE;
	SYSTEM_TIME_TYPE DELAY_TIME; // Delay time before start running
	SYSTEM_TIME_TYPE RELEASE_TIME; // Next or last release time
	SYSTEM_TIME_TYPE DEADLINE_TIME; // Next or last deadline time
	SYSTEM_TIME_TYPE WAIT_TIME; // Time when last wait happened
	SYSTEM_TIME_TYPE WAKE_TIME; // Time when last wake happened
	CONTEXT_TYPE CONTEXT;
	priorityqueueENTRY ENT_PROCESSREADY; // Entry for process ready priority queue
	priorityqueueENTRY ENT_PROCESSDEADLINE; // Entry for process deadline priority queue
	priorityqueueENTRY ENT_PROCESSWAITING_TIMEOUT; // Entry for process waiting timeout priority queue
	priorityqueueENTRY ENT_PROCESSWAITING_RESOURCE; // Entry for use in resource priority queues
	BOOLEAN_TYPE SUSPENDED; // Indicates the process is suspended
	BOOLEAN_TYPE SUSPENDED_TIMEOUT; // Indicates the process is suspended with timeout

	// -------------------- OPTIONAL MODULES START --------------------

	// Intrapartition communication - Buffer
	MESSAGE_ADDR_TYPE BUFFER_SEND_ADDRESS; // Address of the pending SEND_BUFFER call
	MESSAGE_SIZE_TYPE BUFFER_SEND_LENGTH; // Length of the pending SEND_BUFFER call
	MESSAGE_ADDR_TYPE BUFFER_RECEIVE_ADDRESS; // Address of the pending RECEIVE_BUFFER call
	MESSAGE_SIZE_TYPE *BUFFER_RECEIVE_LENGTH; // Length of the pending RECEIVE_BUFFER call

	// -------------------- OPTIONAL MODULES END --------------------
};
typedef struct _PROCESS_INFORMATION_TYPE PROCESS_INFORMATION_TYPE;

// Health monitoring callback information
struct _HEALTHMONITORINGCALLBACK_INFORMATION_TYPE {
	BOOLEAN_TYPE EXISTS;
	SYSTEM_ADDRESS_TYPE ENTRY_POINT;
	STACK_SIZE_TYPE STACK_SIZE;
	CONTEXT_TYPE CONTEXT;
	BOOLEAN_TYPE SYSTEM_PARTITION_CONTEXT; // Indicates the health monitoring callback was triggered by a system partition context
	BOOLEAN_TYPE START; // Indicates scheduler should start the health monitoring callback
	BOOLEAN_TYPE STOP; // Indicates scheduler should stop the health monitoring callback
	BOOLEAN_TYPE RUNNING; // Indicates the health monitoring callback is currently running
	SYSTEM_STATE_TYPE SYSTEM_STATE; // System state for which the health monitoring callback is running
	ERROR_IDENTIFIER_TYPE ERROR_IDENTIFIER; // Error identifier for which the health monitoring callback is running
};
typedef struct _HEALTHMONITORINGCALLBACK_INFORMATION_TYPE HEALTHMONITORINGCALLBACK_INFORMATION_TYPE;

// Error handler information
struct _ERRORHANDLER_INFORMATION_TYPE {
	BOOLEAN_TYPE EXISTS;
	SYSTEM_ADDRESS_TYPE ENTRY_POINT;
	STACK_SIZE_TYPE STACK_SIZE;
	CONTEXT_TYPE CONTEXT;
	BOOLEAN_TYPE SYSTEM_PARTITION_CONTEXT; // Indicates the error handler was triggered by a system partition context
	BOOLEAN_TYPE START; // Indicates scheduler should start the error handler
	BOOLEAN_TYPE STOP; // Indicates scheduler should stop the error handler
	BOOLEAN_TYPE RUNNING; // Indicates the error handler is currently running
	PROCESS_ID_TYPE PREEMPTED_PROCESS_IDENTIFIER; // Identifier of the process that was preempted by the error handler
};
typedef struct _ERRORHANDLER_INFORMATION_TYPE ERRORHANDLER_INFORMATION_TYPE;

// Partition information
struct _PARTITION_INFORMATION_TYPE {
	BOOLEAN_TYPE INITIALIZED; // Indicates the partition initialization is finished and the memory allocation for the partition is complete
	PARTITION_ATTRIBUTE_TYPE *PARTITION_ATTRIBUTE;
	PARTITION_CONFIGURATION_TYPE *PARTITION_CONFIGURATION;
	PARTITION_ID_TYPE IDENTIFIER;
	OPERATING_MODE_TYPE OPERATING_MODE;
	START_CONDITION_TYPE START_CONDITION;
	LOCK_LEVEL_TYPE LOCK_LEVEL;
	BOOLEAN_TYPE HEALTHMONITORING_PARTITION_RESTART; // Indicates that the ongoing partition restart is caused by the health monitoring mechanism
	heapRECORD REC_HEAP;
	CONTEXT_TYPE CONTEXT;
	PROCESS_ID_TYPE PROCESS_COUNT; // Process count
	PROCESS_INFORMATION_TYPE *PROCESS_INFORMATION; // Process information, indexed by process identifier
	PROCESS_INFORMATION_TYPE *CURRENT_PROCESS_INFORMATION; // Points the running process information
	ERRORHANDLER_INFORMATION_TYPE ERRORHANDLER_INFORMATION;
	HEALTHMONITORINGCALLBACK_INFORMATION_TYPE HEALTHMONITORINGCALLBACK_INFORMATION;
	INDEX_TYPE SCHEDULE_WINDOW_INDEX; // Points the partition's current schedule window
	priorityqueueENTRY ENT_PARTITIONREADY; // Entry for partition ready priority queue
	priorityqueueRECORD REC_PROCESSREADY; // Process ready priority queue - Sorts processes by descending priority
	priorityqueueRECORD REC_PROCESSDEADLINE; // Process deadline priority queue - Sorts processes by ascending deadline time
	priorityqueueRECORD REC_PROCESSWAITING_TIMEOUT; // Process waiting timeout priority queue - Sorts processes by ascending time
	SIZE_TYPE ERROR_STATUS_COUNT; // Current error status count
	INDEX_TYPE NEXT_ERROR_STATUS_INDEX; // Next error status index to read
	ERROR_STATUS_TYPE *ERROR_STATUS; // Stores error status entries

	// -------------------- OPTIONAL MODULES START --------------------

	// Interpartition communication - Sampling port
	SAMPLING_PORT_ID_TYPE SAMPLINGPORT_COUNT;
	SAMPLINGPORT_INFORMATION_TYPE *SAMPLINGPORT_INFORMATION;

	// Interpartition communication - Queuing port
	QUEUING_PORT_ID_TYPE QUEUINGPORT_COUNT;
	QUEUINGPORT_INFORMATION_TYPE *QUEUINGPORT_INFORMATION;

	// Intrapartition communication - Buffer
	BUFFER_ID_TYPE BUFFER_COUNT;
	BUFFER_INFORMATION_TYPE *BUFFER_INFORMATION;

	// Intrapartition communication - Blackboard
	BLACKBOARD_ID_TYPE BLACKBOARD_COUNT;
	BLACKBOARD_INFORMATION_TYPE *BLACKBOARD_INFORMATION;

	// Intrapartition communication - Semaphore
	SEMAPHORE_ID_TYPE SEMAPHORE_COUNT;
	SEMAPHORE_INFORMATION_TYPE *SEMAPHORE_INFORMATION;

	// Intrapartition communication - Event
	EVENT_ID_TYPE EVENT_COUNT;
	EVENT_INFORMATION_TYPE *EVENT_INFORMATION;

	// -------------------- OPTIONAL MODULES END --------------------
};
typedef struct _PARTITION_INFORMATION_TYPE PARTITION_INFORMATION_TYPE;

// Module information
struct _MODULE_INFORMATION_TYPE {
	MODULE_CONFIGURATION_TYPE *MODULE_CONFIGURATION;
	heapRECORD REC_HEAP;
	IDENTIFIER_TYPE NEXT_CONTEXT_IDENTIFIER; // Next context identifier to be allocated
	IDENTIFIER_TYPE LAST_CONTEXT_IDENTIFIER; // Last context identifier to be allocated
	OPERATING_MODE_TYPE OPERATING_MODE;
	CONTEXT_TYPE CONTEXT;
	PARTITION_ID_TYPE PARTITION_COUNT; // Partition count
	PARTITION_INFORMATION_TYPE *PARTITION_INFORMATION; // Partition information, indexed by partition identifier
	PARTITION_INFORMATION_TYPE *CURRENT_PARTITION_INFORMATION; // Points the running partition information
	HEALTHMONITORINGCALLBACK_INFORMATION_TYPE HEALTHMONITORINGCALLBACK_INFORMATION;
	SYSTEM_TIME_TYPE MAJOR_FRAME_START; // Current major frame start time
	SYSTEM_TIME_TYPE MAJOR_FRAME_TIME; // Current major frame time
	SYSTEM_TIME_TYPE NEXT_MAJOR_FRAME_START; // Next major frame start time
	priorityqueueRECORD REC_PARTITIONREADY; // Partition ready priority queue - Sorts partitions by ascending start time in their current schedule window entry
};
typedef struct _MODULE_INFORMATION_TYPE MODULE_INFORMATION_TYPE;

// System information
struct _SYSTEM_INFORMATION_TYPE {
	SYSTEM_CONFIGURATION_TYPE *SYSTEM_CONFIGURATION;
};
typedef struct _SYSTEM_INFORMATION_TYPE SYSTEM_INFORMATION_TYPE;

// System information
extern SYSTEM_INFORMATION_TYPE SYSTEM_INFORMATION;

// Core module information (one per core)
extern MODULE_INFORMATION_TYPE CORE_MODULE_INFORMATION[PORT_CORECOUNT];

// Core current context - Context of the running task (one per core)
extern CONTEXT_TYPE *CORE_CURRENT_CONTEXT[PORT_CORECOUNT];

// Core next context - Context to be reached after the scheduler completes (one per core)
extern CONTEXT_TYPE *CORE_NEXT_CONTEXT[PORT_CORECOUNT];

// Core module information getter macro
#define _CORE_MODULE_INFORMATION (&CORE_MODULE_INFORMATION[PORT_GETCORE()])

// Core current context getter macro
#define _CORE_CURRENT_CONTEXT (CORE_CURRENT_CONTEXT[PORT_GETCORE()])

// Core next context getter macro
#define _CORE_NEXT_CONTEXT (CORE_NEXT_CONTEXT[PORT_GETCORE()])

// Current module information access macro
#define _CURRENT_MODULE_INFORMATION (MODULE_INFORMATION)

// Current partition information access macro
#define _CURRENT_PARTITION_INFORMATION (_CURRENT_MODULE_INFORMATION->CURRENT_PARTITION_INFORMATION)

// Current process information access macro
#define _CURRENT_PROCESS_INFORMATION (_CURRENT_PARTITION_INFORMATION->CURRENT_PROCESS_INFORMATION)

// Startup core method
void STARTUP_CORE(void);

// Enter core method
void ENTER_CORE(void);

// Exit core method
void EXIT_CORE(void);

// Process information initialization method
void INITIALIZE_PROCESS_INFORMATION(PARTITION_ID_TYPE PARTITION_IDENTIFIER, PROCESS_ID_TYPE PROCESS_IDENTIFIER);

// Partition information initialization method
void INITIALIZE_PARTITION_INFORMATION(PARTITION_ID_TYPE PARTITION_IDENTIFIER, BOOLEAN_TYPE REINITIALIZING);

// Module information initialization method
void INITIALIZE_MODULE_INFORMATION(void);

// Module health monitoring callback return point
void RETURNPOINT_MODULE_HEALTHMONITORINGCALLBACK(void);

// Partition health monitoring callback return point
void RETURNPOINT_PARTITION_HEALTHMONITORINGCALLBACK(void);

// Partition error handler return point
void RETURNPOINT_PARTITIONERRORHANDLER(void);

// Partition default process return point
void RETURNPOINT_PARTITIONDEFAULTPROCESS(void);

// Process return point
void RETURNPOINT_PROCESS(void);

#endif
