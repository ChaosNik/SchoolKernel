#ifndef _userMT_h_
#define _userMT_h_

#include "Thread.h"

int userMain(int, char*[]);

class UserMain:public Thread
{
public:
	void run();
};
#endif