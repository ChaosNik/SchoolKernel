#ifndef _PCB_h_
#define _PCB_h_

#include "List.h"

class Semaphore;
class Thread;
class List;

class PCB
{
public:
	PCB(unsigned long stackSize,
		unsigned int timeSlice,
		Thread* myThread);
	~PCB();
    void start();
    void waitToComplete();
    void sleep(unsigned int timeToSleep);
	void goFromIdleThreadToMainFunction();
//private:
	void run();
	static void wrapperRun();
	
	friend class Timer;
	friend class List;
	friend class KernelSemaphore;
	
	static int noMoreTime;
	static int mustSwitchContext;

	static PCB* runningPCB;
	static PCB* mainPCB;
	static PCB* idlePCB;
	static List* blockedPCB;
	static List* terminatedPCB;
	static List* allPCB;

	enum State
	{
		NEW,
		RUNNING,
		BLOCKED,
		READY,
		TERMINATED,
	};

    unsigned long stackSize;
    unsigned stackPointer;
	unsigned stackSegment;
    State state;
	unsigned int timeToSleep;
	unsigned int timeToRun;
	unsigned int timeSlice;
	unsigned int* stack;
    Semaphore* semaphore;
	unsigned int id;
	Thread* myThread;
};

#endif