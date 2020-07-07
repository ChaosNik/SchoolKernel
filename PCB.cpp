#include "PCB.h"
#include <dos.h>
#include <iostream.h>//samo test
#include "Semaphor.h"
#include "Thread.h"
#include "Schedule.h"

int PCB::noMoreTime = 0;
int PCB::mustSwitchContext = 0;
unsigned temporaryRegister;

PCB* PCB::runningPCB = 0;
PCB* PCB::mainPCB = 0;
PCB* PCB::idlePCB = 0;
List* PCB::blockedPCB = new List();
List* PCB::terminatedPCB = new List();
List* PCB::allPCB = new List();
unsigned globalID=0;
static unsigned newsp, newss, oldsp, oldss;
static unsigned newip, newcs;

PCB::PCB(unsigned long stackSize,
	unsigned int timeSlice,
	Thread* myThread):
	stackSize(stackSize),
	timeToSleep(0),
	timeToRun(timeSlice),
	timeSlice(timeSlice),
	myThread(myThread)
{
	asm cli;
	id=globalID;
	switch(globalID)
	{
		case 0:mainPCB = this;break;
		case 1:idlePCB = this;break;
	}	
	globalID++;
	allPCB->add(this);
    state = NEW;
	semaphore = new Semaphore(0);
	
	stack = new unsigned[stackSize];
	
	newsp=stackPointer=FP_OFF(stack+stackSize-1);
	newss=stackSegment=FP_SEG(stack+stackSize-1);
	
	newip=FP_OFF(&(wrapperRun));
	newcs=FP_SEG(&(wrapperRun));
	
	oldsp=_SP;
	oldss=_SS;
	_SP=newsp;
	_SS=newss;
	asm{
		pushf
		pop ax
		or ax,0x200
		push ax
		
		mov ax,newcs
		push ax
		mov ax,newip
		push ax
		
		mov ax,0
		push ax
		push bx
		push cx
		push dx
		push es
		push ds
		push si
		push di
		push bp
	}
	newsp=_SP;
	newss=_SS;
	_SP=oldsp;
	_SS=oldss;
	stackPointer=newsp;
	stackSegment=newss;
	
	asm sti;
}
PCB::~PCB()
{
	allPCB->remove(id);
}
void PCB::start()
{
	if(id==1)
		state=TERMINATED;
    else if(state==NEW)
	{
		state=READY;
		Scheduler::put(this);
	}
}
void PCB::waitToComplete()
{
    if(runningPCB->state!=NEW&&
		runningPCB->state!=TERMINATED)
		semaphore->wait();
}
void PCB::sleep(unsigned int timeToSleep)
{
	this->timeToSleep=timeToSleep;
	state=BLOCKED;
	if(blockedPCB->find(id)==0)
		blockedPCB->add(this);
	if(runningPCB==this)
		dispatch();
}
void PCB::run()
{
	myThread->run();
	state=TERMINATED;
	semaphore->signal();
	dispatch();
}
void PCB::wrapperRun()
{
	runningPCB->run();
}