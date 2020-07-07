#include "IVTEntry.h"
#include "KerEv.h"
#include "Timer.h"
#include <dos.h>

int callOldHandlerFlag;
IVTEntry* IVTEntry::ivtTable[256]={0};
IVTEntry::IVTEntry(unsigned char ivtNumber,
					Pinterrupt pinterrupt,
					int callOldHandlerFlagnow)
{
	myKernelEvent=0;
	callOldHandlerFlag=callOldHandlerFlagnow;
	ivtTable[ivtNumber]=this;
	this->ivtNumber=ivtNumber;
	handlerBefore=getvect(ivtNumber);
	setvect(ivtNumber,pinterrupt);
}

IVTEntry::~IVTEntry()
{
	setvect(ivtNumber, handlerBefore);
}

void IVTEntry::signal()
{
	if(myKernelEvent)
		myKernelEvent->signal();
	if(callOldHandlerFlag)
	{
		for(int i=0;i<30000;++i)
			for(int j=0;j<30000;j++);
		(*handlerBefore)();
	}
	dispatch();
}