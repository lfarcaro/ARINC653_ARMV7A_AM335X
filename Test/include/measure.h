#ifndef MEASURE_H_
#define MEASURE_H_

// ARINC653 core includes
#include "arinc653_core.h"

// Definitions
#define MEASURE_FLAG_BPRED_DIS 0x00000001
#define MEASURE_FLAG_BPRED_ENA 0x00000002
#define MEASURE_FLAG_BPRED_CLR 0x00000004
#define MEASURE_FLAG_DCACHE_DIS 0x00000010
#define MEASURE_FLAG_DCACHE_ENA 0x00000020
#define MEASURE_FLAG_DCACHE_CLR 0x00000040
#define MEASURE_FLAG_ICACHE_DIS 0x00000100
#define MEASURE_FLAG_ICACHE_ENA 0x00000200
#define MEASURE_FLAG_ICACHE_CLR 0x00000400

// Global variables
static portBOOLEAN MEASURE_TRIGGERED; // Should be set to trigger a (single) measurement
static portBOOLEAN MEASURE_RUNNING; // Indicates a measurement was triggered and is running
static portBOOLEAN MEASURE_FINISHED; // Indicates a measurement was triggered and is finished
static portUINT32 MEASURE_VALUE; // Stores the value of the finished measurement

// Measure startup method
void MEASURE_STARTUP(void);

// Measure configuring method
void MEASURE_CONFIGURE(portUINT32 FLAGS);

// Measure triggering method
void MEASURE_TRIGGER(void);

// Measure starting method
inline portBOOLEAN MEASURE_START(void);

// Measure stopping method
inline void MEASURE_STOP(void);

// Measure reading method
inline portUINT32 MEASURE_READ(void);

#endif
