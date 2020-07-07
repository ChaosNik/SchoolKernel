#include "Timer.h"
#include "idleT.h"
#include "userMT.h"

int globalIP, globalCS;
unsigned system_sp,system_ss;

extern int result,argcum;
extern char** argvum;

Thread* idleThread;
int main(int argc, char* argv[])
{	
	asm cli;
	argcum=argc;
	argvum=argv;
	
	//for(int i=0;i<30000;++i);//Malo cekanja
	Thread* userMain=new UserMain();
	userMain->start();
	idleThread=new IdleThread();
	idleThread->start();
	asm sti;
	setISR();
	dispatch();
	restoreISR();
	
	return result;
}