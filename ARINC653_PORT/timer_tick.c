// Includes
#include "timer_tick.h"

// Startup method
unsigned char TIMER_TICK_STARTUP(void (*HANDLER)(void)) {

	// First tick must occur after the first period, not immediately,
	// once the scheduler is already called at time zero

	// Enables clocks for DMTimer
	DMTimer2ModuleClkConfig();

	// Registers system tick ISR
	IntRegister(SYS_INT_TINT2, HANDLER);

	// Sets interrupt priority
	IntPrioritySet(SYS_INT_TINT2, 0, AINTC_HOSTINT_ROUTE_IRQ);

	// Enables system interrupt
	IntSystemEnable(SYS_INT_TINT2);

	// Load initial count value
	DMTimerCounterSet(SOC_DMTIMER_2_REGS, 0xFFFFA23Fu);

	// Loads reload count value
	DMTimerReloadSet(SOC_DMTIMER_2_REGS, 0xFFFFA23Fu);

	// Configures DMTimer
	DMTimerModeConfigure(SOC_DMTIMER_2_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);

	// Enables DMTimer interrupts
	DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

	// Enables DMTimer
	DMTimerEnable(SOC_DMTIMER_2_REGS);
	return 1;
}

// Shutdown method
unsigned char TIMER_TICK_SHUTDOWN(void) {

	// Disables DMTimer
	DMTimerDisable(SOC_DMTIMER_2_REGS);

	// Disables DMTimer interrupts
	DMTimerIntDisable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

	// Disables system interrupt
	IntSystemDisable(SYS_INT_TINT2);

	// Unregisters system tick ISR
	IntUnRegister(SYS_INT_TINT2);
	return 1;
}
