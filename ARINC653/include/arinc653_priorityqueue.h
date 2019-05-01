#ifndef ARINC653_PRIORITYQUEUE_H_
#define ARINC653_PRIORITYQUEUE_H_

// Priority
typedef portUINT64 priorityqueuePRIORITY;

// Value
typedef portUINT64 priorityqueueVALUE;

// Size
typedef portSIZE priorityqueueSIZE;

// Entry
struct _priorityqueueENTRY {
	priorityqueuePRIORITY PRI_PRIORITY;
	priorityqueueVALUE VAL_VALUE;
	struct _priorityqueueRECORD *REC_ENQUEUED;
	struct _priorityqueueENTRY *ENT_NEXT;
	struct _priorityqueueENTRY *ENT_PREVIOUS;
};
typedef struct _priorityqueueENTRY priorityqueueENTRY;

// Record
struct _priorityqueueRECORD {
	struct _priorityqueueENTRY *ENT_HEAD;
	priorityqueueSIZE SIZ_COUNT;
	BOOLEAN_TYPE PRIORITIZED;
	BOOLEAN_TYPE ASCENDING;
};
typedef struct _priorityqueueRECORD priorityqueueRECORD;

// Empty verifier
#define PRIORITYQUEUE_ISEMPTY(_REC_PRIORITYQUEUE) ((_REC_PRIORITYQUEUE)->SIZ_COUNT == 0)

// Prioritized verifier
#define PRIORITYQUEUE_ISPRIORITIZED(_REC_PRIORITYQUEUE) ((_REC_PRIORITYQUEUE)->PRIORITIZED)

// Ascending verifier
#define PRIORITYQUEUE_ISASCENDING(_REC_PRIORITYQUEUE) ((_REC_PRIORITYQUEUE)->ASCENDING)

// Count getter
#define PRIORITYQUEUE_GETCOUNT(_REC_PRIORITYQUEUE) ((_REC_PRIORITYQUEUE)->SIZ_COUNT)

// Enqueued verifier
#define PRIORITYQUEUE_ISENQUEUED(_ENT_ENTRY) ((_ENT_ENTRY)->REC_ENQUEUED != null)

// Startup method
void PRIORITYQUEUE_STARTUP(priorityqueueRECORD *REC_PRIORITYQUEUE, BOOLEAN_TYPE PRIORITIZED, BOOLEAN_TYPE ASCENDING);

// Initialize entry method
void PRIORITYQUEUE_INITIALIZEENTRY(priorityqueueENTRY *ENT_ENTRY);

// Enqueue method
void PRIORITYQUEUE_ENQUEUE(priorityqueueRECORD *REC_PRIORITYQUEUE, priorityqueueENTRY *ENT_ENTRY);

// Remove method
void PRIORITYQUEUE_REMOVE(priorityqueueENTRY *ENT_ENTRY);

// Clear method
void PRIORITYQUEUE_CLEAR(priorityqueueRECORD *REC_PRIORITYQUEUE, BOOLEAN_TYPE PRIORITIZED, BOOLEAN_TYPE ASCENDING);

#endif
