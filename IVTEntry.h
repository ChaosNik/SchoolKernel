#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

#include "PCB.h"

typedef void interrupt (*Pinterrupt)(...);
extern int callOldHandlerFlag;
class PCB;
class KernelEvent;

class IVTEntry
{
public:
	static IVTEntry* ivtTable[256];
	IVTEntry(unsigned char ivtNumber,
				Pinterrupt newHandler,
				int callOldHandlerFlag);
	~IVTEntry();

	void signal();
	Pinterrupt handlerBefore;
	unsigned char ivtNumber;
	KernelEvent* myKernelEvent;
};
#define PREPAREENTRY(ivtNumber,callOldHandlerFlag)\
void interrupt interruptRoutine##ivtNumber(...);\
IVTEntry entry##ivtNumber(ivtNumber,interruptRoutine##ivtNumber,callOldHandlerFlag);\
void interrupt interruptRoutine##ivtNumber(...)\
{\
		IVTEntry::ivtTable[ivtNumber]->signal();\
}

#endif