#ifndef TIMER_TICK_H_
#define TIMER_TICK_H_

// Includes
#include "arinc653_port_types.h"
#include "beaglebone.h"
#include "dmtimer.h"
#include "interrupt.h"

// Startup method
unsigned char TIMER_TICK_STARTUP(void (*HANDLER)(void));

// Shutdown method
unsigned char TIMER_TICK_SHUTDOWN(void);

#endif
