// General includes
#include "track.h"

// Track startup method
void TRACK_STARTUP(void) {

	// Enables clocks for DMTimer
	DMTimer3ModuleClkConfig();

	// Configures clock prescaler for DMTimer
	DMTimerPreScalerClkEnable(SOC_DMTIMER_3_REGS, TRACK_TIMER_CLOCKPRESCALER);

	// Configures DMTimer
	DMTimerModeConfigure(SOC_DMTIMER_3_REGS, DMTIMER_ONESHOT_NOCMP_ENABLE);

	// Loads initial value
	DMTimerCounterSet(SOC_DMTIMER_3_REGS, 0);

	// Enables DMTimer
	DMTimerEnable(SOC_DMTIMER_3_REGS);
}

// Track reading method
unsigned int TRACK_READ(void) {
	return DMTimerCounterGet(SOC_DMTIMER_3_REGS);
}

// Track termination method
void TRACK_TERMINATE(void) {

	// Disables DMTimer
	DMTimerDisable(SOC_DMTIMER_3_REGS);
}
