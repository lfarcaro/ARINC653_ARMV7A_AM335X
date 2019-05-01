// ARINC653 includes
#include "arinc653_core.h"

// Startup method
void PRIORITYQUEUE_STARTUP(priorityqueueRECORD *REC_PRIORITYQUEUE, BOOLEAN_TYPE PRIORITIZED, BOOLEAN_TYPE ASCENDING) {

	// Initializes
	REC_PRIORITYQUEUE->ENT_HEAD = null;
	REC_PRIORITYQUEUE->SIZ_COUNT = 0;
	REC_PRIORITYQUEUE->PRIORITIZED = PRIORITIZED;
	REC_PRIORITYQUEUE->ASCENDING = ASCENDING;
}

// Initialize entry method
void PRIORITYQUEUE_INITIALIZEENTRY(priorityqueueENTRY *ENT_ENTRY) {

	// Initializes
	ENT_ENTRY->REC_ENQUEUED = null;
	ENT_ENTRY->ENT_NEXT = null;
	ENT_ENTRY->ENT_PREVIOUS = null;
}

// Enqueue method
void PRIORITYQUEUE_ENQUEUE(priorityqueueRECORD *REC_PRIORITYQUEUE, priorityqueueENTRY *ENT_ENTRY) {
	priorityqueueENTRY *ENT_ITERATOR;
	BOOLEAN_TYPE PRIORITY_COMPARISON;

	// Verifies entry
	if (ENT_ENTRY == null) {
		return;
	}

	// Verifies enqueued record
	if (ENT_ENTRY->REC_ENQUEUED != null) {
		return;
	}

	// Verifies prioritized flag
	if (!REC_PRIORITYQUEUE->PRIORITIZED) {

		// Uses minimum priority to all entries
		ENT_ENTRY->PRI_PRIORITY = MIN_PRIORITY_VALUE;
	}

	// Verifies head
	if (REC_PRIORITYQUEUE->ENT_HEAD == null) {

		// Enqueues
		REC_PRIORITYQUEUE->ENT_HEAD = ENT_ENTRY;

		// Sets pointers
		ENT_ENTRY->ENT_NEXT = null;
		ENT_ENTRY->ENT_PREVIOUS = null;
	} else {

		// Initializes iterator
		ENT_ITERATOR = REC_PRIORITYQUEUE->ENT_HEAD;

		// Position lookup loop
		while (true) {

			// Compares priority position
			PRIORITY_COMPARISON = (REC_PRIORITYQUEUE->ASCENDING ? (ENT_ENTRY->PRI_PRIORITY < ENT_ITERATOR->PRI_PRIORITY) : (ENT_ENTRY->PRI_PRIORITY > ENT_ITERATOR->PRI_PRIORITY));

			// Verifies position condition
			if ((ENT_ITERATOR->ENT_NEXT == null) || (PRIORITY_COMPARISON)) {

				// Verifies priority comparison
				if (PRIORITY_COMPARISON) {

					// Enqueues
					ENT_ENTRY->ENT_PREVIOUS = ENT_ITERATOR->ENT_PREVIOUS;
					ENT_ENTRY->ENT_NEXT = ENT_ITERATOR;

					// Fixes references
					if (ENT_ITERATOR->ENT_PREVIOUS != null) {
						ENT_ITERATOR->ENT_PREVIOUS->ENT_NEXT = ENT_ENTRY;
					}
					ENT_ITERATOR->ENT_PREVIOUS = ENT_ENTRY;

					// Fixes head
					if (REC_PRIORITYQUEUE->ENT_HEAD == ENT_ITERATOR) {
						REC_PRIORITYQUEUE->ENT_HEAD = ENT_ENTRY;
					}
				} else {

					// Enqueues
					ENT_ENTRY->ENT_NEXT = ENT_ITERATOR->ENT_NEXT;
					ENT_ENTRY->ENT_PREVIOUS = ENT_ITERATOR;

					// Fixes references
					if (ENT_ITERATOR->ENT_NEXT != null) {
						ENT_ITERATOR->ENT_NEXT->ENT_PREVIOUS = ENT_ENTRY;
					}
					ENT_ITERATOR->ENT_NEXT = ENT_ENTRY;
				}

				// Breaks
				break;
			}

			// Moves iterator
			ENT_ITERATOR = ENT_ITERATOR->ENT_NEXT;
		}
	}

	// Counts entry
	REC_PRIORITYQUEUE->SIZ_COUNT++;

	// Sets enqueued record
	ENT_ENTRY->REC_ENQUEUED = REC_PRIORITYQUEUE;
}

// Remove method
void PRIORITYQUEUE_REMOVE(priorityqueueENTRY *ENT_ENTRY) {

	// Verifies entry
	if (ENT_ENTRY == null) {
		return;
	}

	// Verifies enqueued record
	if (ENT_ENTRY->REC_ENQUEUED == null) {
		return;
	}

	// Verifies enqueued record
	if (ENT_ENTRY->REC_ENQUEUED->SIZ_COUNT == 0) {
		return;
	}

	// Removes entry
	if (ENT_ENTRY->ENT_NEXT != null) {
		ENT_ENTRY->ENT_NEXT->ENT_PREVIOUS = ENT_ENTRY->ENT_PREVIOUS;
	}
	if (ENT_ENTRY->ENT_PREVIOUS != null) {
		ENT_ENTRY->ENT_PREVIOUS->ENT_NEXT = ENT_ENTRY->ENT_NEXT;
	}

	// Fixes head
	if (ENT_ENTRY->REC_ENQUEUED->ENT_HEAD == ENT_ENTRY) {
		ENT_ENTRY->REC_ENQUEUED->ENT_HEAD = ENT_ENTRY->ENT_NEXT;
	}

	// Counts entry
	ENT_ENTRY->REC_ENQUEUED->SIZ_COUNT--;

	// Sets enqueued record
	ENT_ENTRY->REC_ENQUEUED = null;
}

// Clear method
void PRIORITYQUEUE_CLEAR(priorityqueueRECORD *REC_PRIORITYQUEUE, BOOLEAN_TYPE PRIORITIZED, BOOLEAN_TYPE ASCENDING) {

	// Remove head loop
	while (REC_PRIORITYQUEUE->ENT_HEAD != null) {
		PRIORITYQUEUE_REMOVE(REC_PRIORITYQUEUE->ENT_HEAD);
	}

	// Reinitializes
	REC_PRIORITYQUEUE->ENT_HEAD = null;
	REC_PRIORITYQUEUE->SIZ_COUNT = 0;
	REC_PRIORITYQUEUE->PRIORITIZED = PRIORITIZED;
	REC_PRIORITYQUEUE->ASCENDING = ASCENDING;
}
