#ifndef ARINC653_HEAP_H_
#define ARINC653_HEAP_H_

// Pointer
typedef portBYTE *heapPOINTER;

// Size
typedef portSIZE heapSIZE;

// Record
struct _heapRECORD {
	heapPOINTER PTR_POINTER;
	heapSIZE SIZ_SIZE;
	heapSIZE SIZ_ALIGNMENT;
	heapSIZE SIZ_FULL;
};
typedef struct _heapRECORD heapRECORD;

// Heap startup method
void HEAP_STARTUP(heapRECORD *REC_HEAP, heapPOINTER PTR_POINTER, heapSIZE SIZ_SIZE, heapSIZE SIZ_ALIGNMENT);

// Heap allocate method - If pointer is not null and force flag is false, keeps it unchanged
void HEAP_ALLOCATE(heapRECORD *REC_HEAP, heapPOINTER *PTR_POINTER, heapSIZE SIZ_LENGTH, BOOLEAN_TYPE FORCE);

#endif
