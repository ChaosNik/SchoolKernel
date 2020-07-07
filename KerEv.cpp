#include "KerEv.h"
#include "Schedule.h"
#include "PCB.h"
#include "Thread.h"

KernelEvent::KernelEvent(unsigned char ivtNumber)
{
	this->ivtNumber=ivtNumber;
	ivtEntry=IVTEntry::ivtTable[ivtNumber];
	ivtEntry->myKernelEvent=this;
	myPCB=PCB::runningPCB;
	flag=0;
}
KernelEvent::~KernelEvent()
{
	if(ivtEntry->myKernelEvent)
		ivtEntry->myKernelEvent=0;
	signal();
	flag=0;
}
void KernelEvent::wait()
{
	if(myPCB==PCB::runningPCB)
	{
		myPCB->state=PCB::BLOCKED;
		flag=1;
		dispatch();
	}
}
void KernelEvent::signal()
{
	if(flag&&myPCB)
	{
		myPCB->state=PCB::READY;
		flag=0;
		Scheduler::put(myPCB);
	}
	if(flag)flag=0;
}