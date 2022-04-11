#ifndef MEASURE_H_
#define MEASURE_H_

// ARINC653 core includes
#include "arinc653_core.h"

// Global variables
extern portBOOLEAN MEASURE_RUNNING; // Indicates a measurement was triggered and is running
extern portUINT32 MEASURE_VALUE; // Stores the value of the finished measurement
extern portUINTBASE MEASURE_CONTEXTID; // Stores the identifier of the context to be measured

// Measure startup method
void MEASURE_STARTUP(void);

// Measure starting method
portBOOLEAN MEASURE_START(portUINTBASE CONTEXTID);

// Measure pausing method
void MEASURE_PAUSE(void);

// Measure resuming method
void MEASURE_RESUME(void);

// Measure stopping method
void MEASURE_STOP(void);

// Measure reading method
portUINT32 MEASURE_READ(void);

#endif
