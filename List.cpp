#include "List.h"
#include "PCB.h"
#include <iostream.h>

PCB* temp;

List::List()
{
	asm cli;
	size=0;
	asm sti;
}
PCB* List::get()
{
	asm cli;
	PCB* temp=list[0];
	volatile int i=0;
	while(i<size)
	{
		list[i]=list[i+1];
		++i;
	}
	size--;
	asm sti;
	return temp;
}
void List::remove(unsigned id)
{
	asm cli;
	volatile int i=0;
	int k=0;
	while(i+1<size)
	{
		if(id==list[i]->id)k=1;
		if(k!=0)list[i]=list[i+1];
		i=i+1;
	}
	size--;
	asm sti;
}
PCB* List::find(unsigned id)
{
	asm cli;
	volatile int i=0;
	temp=0;
	while(i<size)
	{
		if(id==list[i]->id)temp=list[i];
		i++;
	}
	asm sti;
	return temp;
}
void List::add(PCB* data)
{
	asm cli;
	size++;
	list[size-1]=data;
	asm sti;
}
int List::numOfElem()
{
	return size;
}
PCB* List::elem(int i)
{
	return list[i];
}
void List::print()
{
	asm cli;
	volatile int i=0;
	while(i<size)
	{
		cout<<list[i];
		i++;
	}
	asm sti;
}