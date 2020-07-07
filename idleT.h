#ifndef _idleT_h_
#define _idleT_h_

#include "Thread.h"

class IdleThread:public Thread
{
public:
	void run();
};
#endif