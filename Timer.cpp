#include "Timer.h"
#include "Schedule.h"
#include "List.h"
#include "PCB.h"
#include "Thread.h"
#include <dos.h>
#include <iostream.h>
#include "IVTEntry.h"

PCB* tempPCB;
unsigned firstDispatch=1;

static unsigned tempsp, tempss, mainss, mainsp;
unsigned freeInterruptVectorAddress = 0x08;
unsigned freeInterruptVectorAddressPlace = 0x60;

unsigned offsetAdress;
unsigned segmentAdress;
unsigned emptyOffset;
unsigned emptySegment;

unsigned locked=0;
void lockISR()
{
	locked=1;
}
void unlockISR()
{
	locked=0;
}
void interrupt ISR()
{
	asm cli;
	if(!PCB::mustSwitchContext)
	{
		PCB::runningPCB->timeToRun--; 
		for(int i=0; i<PCB::blockedPCB->numOfElem(); ++i)
		{
			tempPCB=PCB::blockedPCB->elem(i);
			tempPCB->timeToSleep--;
			if(tempPCB->timeToSleep<=0)
			{
				tempPCB->state=PCB::READY; 
				Scheduler::put(tempPCB);
				PCB::blockedPCB->remove(tempPCB->id);
				
			}
		}
		tick();
	}
	
	if(PCB::runningPCB->timeSlice != 0 && !PCB::mustSwitchContext)
	{
		if(PCB::runningPCB->timeToRun == 0)
		{
			PCB::noMoreTime = 1;
			PCB::runningPCB->timeToRun = PCB::runningPCB->timeSlice;
		}
	} 
	if(PCB::mustSwitchContext||PCB::noMoreTime)
	{
		
		//izlazak iz main-a prvim dispatch-om
		if(firstDispatch)
		{
			systemsp=_SP;
			systemss=_SS;
			
			firstDispatch=0;
		}
		
		//preuzimanje konteksta
		PCB::runningPCB->stackPointer = _SP;
		PCB::runningPCB->stackSegment = _SS;
		
		//postavljanje statea niti iz koje izlazimo
		if(PCB::runningPCB->state==PCB::NEW)
			PCB::runningPCB->state=PCB::READY;
		if(PCB::runningPCB->state==PCB::RUNNING)
			PCB::runningPCB->state=PCB::READY;
		if(PCB::runningPCB->state==PCB::NEW)
			PCB::runningPCB->state=PCB::READY;
		
		//prazna, idle ili blokirana nit se 
		//ne stavlja u Scheduler
		if(PCB::runningPCB!=0&&
			PCB::runningPCB!=PCB::idlePCB&&
			PCB::runningPCB->state!=PCB::BLOCKED)
			Scheduler::put(PCB::runningPCB);
		
		//preuzimamo sljedecu nit
		PCB::runningPCB = Scheduler::get();

		//provjera statea niti
		if(PCB::runningPCB)
		{
			//ako je nit zavrsila
			if(PCB::runningPCB->state==PCB::TERMINATED)
				PCB::runningPCB=PCB::idlePCB;
			//ako nit spava ili je blokirana
			if(PCB::runningPCB->state==PCB::BLOCKED)
				PCB::runningPCB=PCB::idlePCB;
		}
		//ako je Scheduler prazan
		//if(!PCB::runningPCB)
		if(!PCB::runningPCB||PCB::runningPCB==PCB::idlePCB)
		{
			//povratak u main
			if(PCB::allPCB->numOfElem()==2&&
				PCB::mainPCB->state==PCB::TERMINATED)
			{				
				delete PCB::idlePCB;
				delete PCB::mainPCB;
				PCB::runningPCB=0;
				
				_SP=systemsp;
				_SS=systemss;
				asm sti;
				return;
			}
			PCB::runningPCB = PCB::idlePCB;
		}
		
		//dodjelimo ponovo timeSlice niti
		PCB::runningPCB->timeToRun = PCB::runningPCB->timeSlice;
		
		//dodjelimo adresu konteksta
		_SP=PCB::runningPCB->stackPointer;
		_SS=PCB::runningPCB->stackSegment;
	}
	
	asm sti;
	if(!PCB::mustSwitchContext) asm int 60h;
	PCB::mustSwitchContext = 0;
	PCB::noMoreTime = 0;
}

volatile unsigned staraRutinaOff, staraRutinaSeg, timerISRoff, timerISRseg;
void setISR()
{
	asm cli;
	offsetAdress=freeInterruptVectorAddress*4;
	segmentAdress=freeInterruptVectorAddress*4+2;
	emptyOffset=freeInterruptVectorAddressPlace*4;
	emptySegment=freeInterruptVectorAddressPlace*4+2;
	
	timerISRoff=FP_OFF(&ISR);
	timerISRseg=FP_SEG(&ISR);
	
	asm {
		push es
		push ax
		push di
		mov ax,0
		mov es,ax

		mov di, word ptr segmentAdress
		mov ax, word ptr es:di
		mov word ptr staraRutinaSeg, ax
		mov ax, timerISRseg
		mov word ptr es:di, ax

		mov di, word ptr offsetAdress
		mov ax, word ptr es:di
		mov word ptr staraRutinaOff, ax
		mov ax, timerISRoff
		mov word ptr es:di, ax

		mov di, word ptr emptyOffset
		mov ax, word ptr staraRutinaOff
		mov word ptr es:di, ax
		mov di, word ptr emptySegment
		mov ax, word ptr staraRutinaSeg
		mov word ptr es:di, ax

		pop di
		pop ax
		pop es
	}
	asm sti;
}

void restoreISR()
{
	asm cli;
	offsetAdress=freeInterruptVectorAddress*4;
	segmentAdress=freeInterruptVectorAddress*4+2;
	asm {
		push es
		push ax
		push di

		mov ax,0
		mov es,ax

		mov di, word ptr segmentAdress
		mov ax, word ptr staraRutinaSeg
		mov word ptr es:di, ax

		mov di, word ptr offsetAdress
		mov ax, word ptr staraRutinaOff
		mov word ptr es:di, ax

		pop di
		pop ax
		pop es
	}
	asm sti;
}