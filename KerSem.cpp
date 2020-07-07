#include "KerSem.h"
#include "Schedule.h"
#include "PCB.h"
#include "Thread.h"
#include "Timer.h"

KernelSemaphore::KernelSemaphore(int init)
{
	value=init;
	if(!blockedList)blockedList=new List();
}
KernelSemaphore::~KernelSemaphore()
{
	while(value<0)
		signal();
	delete blockedList;
}
void KernelSemaphore::wait()
{
	value--;
	if(value<0)
	{
		PCB::runningPCB->state=PCB::BLOCKED;
		blockedList->add(PCB::runningPCB);
	}
	dispatch();
}
void KernelSemaphore::signal()
{
	value++;
	if(value<=0)
	{
		PCB* temp=blockedList->get();
		if(PCB::allPCB->find(temp->id))
		{
			temp->state=PCB::READY;
			Scheduler::put(temp);
		}
	}
}
int KernelSemaphore::val()const
{
	return value;
}