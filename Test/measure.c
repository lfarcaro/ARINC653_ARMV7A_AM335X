// General includes
#include "measure.h"

// Measure startup method
void MEASURE_STARTUP(void) {

	// Initializes state
	MEASURE_TRIGGERED = false;
	MEASURE_RUNNING = false;
	MEASURE_FINISHED = false;
	MEASURE_VALUE = 0;

	// Enables clocks for DMTimer
	DMTimer3ModuleClkConfig();

	// Configures DMTimer
	DMTimerModeConfigure(SOC_DMTIMER_3_REGS, DMTIMER_ONESHOT_NOCMP_ENABLE);
}

// Measure configuring method
void MEASURE_CONFIGURE(portUINT32 FLAGS) {

	// Enters core
	ENTER_CORE();

	// Cache flush: writes modified cached data to the next level in the memory hierarchy.
	// Cache invalidate: marks cache lines as invalid without writing content to the next level.

	// Disables branch prediction
	if (FLAGS & MEASURE_FLAG_BPRED_DIS) {
		CP15BranchPredictionDisable();
	}

	// Disables data cache
	if (FLAGS & MEASURE_FLAG_DCACHE_DIS) {
		CacheDisable(CACHE_DCACHE);
	}

	// Disables instruction cache
	if (FLAGS & MEASURE_FLAG_ICACHE_DIS) {
		CacheDisable(CACHE_ICACHE);
	}

	// Invalidates branch prediction
	if (FLAGS & MEASURE_FLAG_BPRED_CLR) {
		CP15BranchPredictorInvalidate();
	}

	// Clears data cache
	if (FLAGS & MEASURE_FLAG_DCACHE_CLR) {
		CacheDataCleanInvalidateAll();
	}

	// Clears instruction cache
	if (FLAGS & MEASURE_FLAG_ICACHE_CLR) {
		CacheInstInvalidateAll();
	}

	// Enables branch prediction
	if (FLAGS & MEASURE_FLAG_BPRED_ENA) {
		CP15BranchPredictionEnable();
	}

	// Enables data cache
	if (FLAGS & MEASURE_FLAG_DCACHE_ENA) {
		if (!(FLAGS & MEASURE_FLAG_DCACHE_DIS)) {
			CacheDisable(CACHE_DCACHE);
		}
		CacheEnable(CACHE_DCACHE);
	}

	// Enables instruction cache
	if (FLAGS & MEASURE_FLAG_ICACHE_ENA) {
		if (!(FLAGS & MEASURE_FLAG_ICACHE_DIS)) {
			CacheDisable(CACHE_ICACHE);
		}
		CacheEnable(CACHE_ICACHE);
	}

	// Exits core
	EXIT_CORE();
}

void MEASURE_TRIGGER(void) {

	// Verifies triggered and running flag
	if ((MEASURE_TRIGGERED) || (MEASURE_RUNNING)) {
		return;
	}

	// Sets triggered flag
	MEASURE_TRIGGERED = true;

	// Resets running and finished flags
	MEASURE_RUNNING = false;
	MEASURE_FINISHED = false;
}

// Measure starting method
inline portBOOLEAN MEASURE_START(void) {

	// Verifies triggered flag
	if (!MEASURE_TRIGGERED) {
		return false;
	}

	// Resets triggered flag
	MEASURE_TRIGGERED = false;

	// Disables DMTimer
	DMTimerDisable(SOC_DMTIMER_3_REGS);

	// Loads initial value
	DMTimerCounterSet(SOC_DMTIMER_3_REGS, 0);

	// Sets running flag
	MEASURE_RUNNING = true;

	// Enables DMTimer
	DMTimerEnable(SOC_DMTIMER_3_REGS);

	// Start executed
	return true;
}

// Measure stopping method
inline void MEASURE_STOP(void) {
	unsigned int VALUE;

	// Reads counter
	VALUE = DMTimerCounterGet(SOC_DMTIMER_3_REGS);

	// Verifies running flag
	if (!MEASURE_RUNNING) {
		return;
	}

	// Disables DMTimer
	DMTimerDisable(SOC_DMTIMER_3_REGS);

	// Stores measured value
	MEASURE_VALUE = VALUE;

	// Sets finished flag
	MEASURE_FINISHED = true;
}

// Measure reading method
inline portUINT32 MEASURE_READ(void) {
	unsigned int VALUE;

    // Waits for measure
    while (!MEASURE_FINISHED);

	// Stores value
	VALUE = MEASURE_VALUE;

	// Resets state
	MEASURE_TRIGGERED = false;
	MEASURE_RUNNING = false;
	MEASURE_FINISHED = false;
	MEASURE_VALUE = 0;

	// Returns value
	return VALUE;
}
