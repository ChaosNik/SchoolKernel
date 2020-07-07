#ifndef _List_h_
#define _List_h_

class PCB;

class List
{
public:
	List();
	int size;
	PCB* list[100];
	int numOfElem();
	PCB* get();
	void add(PCB* data);
	void remove(unsigned id);
	PCB* find(unsigned id);
	PCB* elem(int i);
	void print();
};
#endif
