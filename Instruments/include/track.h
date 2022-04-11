#ifndef TRACK_H_
#define TRACK_H_

// ARINC653 core includes
#include "arinc653_core.h"

// Track timer clock frequency
#define TRACK_TIMER_CLOCKFREQUENCY 24000000

// Track timer clock prescaler
#define TRACK_TIMER_CLOCKPRESCALER DMTIMER_PRESCALER_CLK_DIV_BY_256

// Track timer effective clock frequency
#define TRACK_TIMER_EFFECTIVECLOCKFREQUENCY 93750

// Track startup method
void TRACK_STARTUP(void);

// Track reading method
unsigned int TRACK_READ(void);

// Track termination method
void TRACK_TERMINATE(void);

#endif
