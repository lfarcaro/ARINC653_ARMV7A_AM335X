#ifndef ARINC653_SCHEDULER_H_
#define ARINC653_SCHEDULER_H_

// Start partition scheduler method
void SCHEDULER_STARTPARTITIONSCHEDULER(RETURN_CODE_TYPE *RETURN_CODE);

// Start process scheduler method
void SCHEDULER_STARTPROCESSSCHEDULER(RETURN_CODE_TYPE *RETURN_CODE);

// Scheduler
void SCHEDULER(void);

// Tick method
void TICK(void);

#endif
