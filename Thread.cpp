#include "Thread.h"
#include "PCB.h"
#include "Timer.h"

void Thread::start()
{
	myPCB->start();
}

void Thread::waitToComplete()
{
	myPCB->waitToComplete();
}

Thread::~Thread()
{
	delete myPCB;
}

void Thread::sleep(unsigned int timeToSleep)
{
	myPCB->sleep(timeToSleep);
}

Thread::Thread(unsigned long stackSize, unsigned int timeSlice)
{
	myPCB = new PCB(stackSize,timeSlice,this);
}

void dispatch()
{
	PCB::mustSwitchContext=1;
	ISR();
}