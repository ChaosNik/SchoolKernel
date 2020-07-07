#ifndef _Timer_h_
#define _timer_h_

#include "Thread.h"

void tick();
static volatile unsigned systemsp, systemss;
extern Thread* idleThread;
void setISR();
void interrupt ISR();
void restoreISR();

void lockISR();
void unlockISR();
extern unsigned locked;
#endif