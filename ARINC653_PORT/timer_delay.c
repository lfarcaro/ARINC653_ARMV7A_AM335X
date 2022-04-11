// Includes
#include "timer_delay.h"

// Startup method
unsigned char TIMER_DELAY_STARTUP(void) {

	// Enables clocks for DMTimer
    DMTimer7ModuleClkConfig();

	// Configures DMTimer
	DMTimerModeConfigure(SOC_DMTIMER_7_REGS, DMTIMER_ONESHOT_NOCMP_ENABLE);

	// Enables DMTimer
	DMTimerEnable(SOC_DMTIMER_7_REGS);
	return 1;
}

// Delay method for milliseconds
void TIMER_DELAY_MS(unsigned int MS) {

    // Iterates units
    while (MS != 0) {

        // Sets timer to zero
        DMTimerCounterSet(SOC_DMTIMER_7_REGS, 0);

        // Enables timer
        DMTimerEnable(SOC_DMTIMER_7_REGS);

        // Waits for timer to reach 1ms
        while (DMTimerCounterGet(SOC_DMTIMER_7_REGS) < TIMER_DELAY_COUNT_1MS);

        // Disables timer
        DMTimerDisable(SOC_DMTIMER_7_REGS);

        // Decrements unit
        MS--;
    }
}

// Shutdown method
unsigned char TIMER_DELAY_SHUTDOWN(void) {

	// Disables DMTimer
	DMTimerDisable(SOC_DMTIMER_7_REGS);
	return 1;
}
