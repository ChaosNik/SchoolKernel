#include "userMT.h"
int argcum;
char** argvum;
int result;

void UserMain::run()
{
	result=userMain(argcum,argvum);
	dispatch();
}