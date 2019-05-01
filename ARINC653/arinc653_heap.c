// ARINC653 includes
#include "arinc653_core.h"

// Heap startup method
void HEAP_STARTUP(heapRECORD *REC_HEAP, heapPOINTER PTR_POINTER, heapSIZE SIZ_SIZE, heapSIZE SIZ_ALIGNMENT) {

	// Starts up heap
	REC_HEAP->PTR_POINTER = PTR_POINTER;
	REC_HEAP->SIZ_SIZE = SIZ_SIZE;
	REC_HEAP->SIZ_ALIGNMENT = SIZ_ALIGNMENT;
	REC_HEAP->SIZ_FULL = 0;
}

// Heap allocate method - If pointer is not null and force flag is false, keeps it unchanged
void HEAP_ALLOCATE(heapRECORD *REC_HEAP, heapPOINTER *PTR_POINTER, heapSIZE SIZ_LENGTH, BOOLEAN_TYPE FORCE) {
	UINTBASE_TYPE *PTR_CLEAR;

	// Verifies allocation
	if ((*PTR_POINTER != null) && (!FORCE)) {
		return;
	}

	// Nulls pointer
	*PTR_POINTER = null;

	// Verifies length
	if (SIZ_LENGTH == 0) {
		return;
	}

	// Verifies alignment
	if (((UINTPOINTER_TYPE) &REC_HEAP->PTR_POINTER[REC_HEAP->SIZ_FULL]) % REC_HEAP->SIZ_ALIGNMENT > 0) {
		REC_HEAP->SIZ_FULL += (REC_HEAP->SIZ_ALIGNMENT - (((UINTPOINTER_TYPE) &REC_HEAP->PTR_POINTER[REC_HEAP->SIZ_FULL]) % REC_HEAP->SIZ_ALIGNMENT));
	}

	// Verifies full size
	if (REC_HEAP->SIZ_FULL + SIZ_LENGTH - 1 >= REC_HEAP->SIZ_SIZE) {
		return;
	}

	// Clears memory
	for (PTR_CLEAR = (UINTBASE_TYPE *) &REC_HEAP->PTR_POINTER[REC_HEAP->SIZ_FULL]; (UINTPOINTER_TYPE) PTR_CLEAR < ((UINTPOINTER_TYPE) &REC_HEAP->PTR_POINTER[REC_HEAP->SIZ_FULL] + SIZ_LENGTH); PTR_CLEAR++) {
		*PTR_CLEAR = null;
	}

	// Prepares address
	*PTR_POINTER = &REC_HEAP->PTR_POINTER[REC_HEAP->SIZ_FULL];

	// Calculates full size
	REC_HEAP->SIZ_FULL += SIZ_LENGTH;
}
