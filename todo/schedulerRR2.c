#include <scheduler.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

#define QUANTUM 2	// Para lograr RR con q=2
int q = 0;	// Contar 2 quantum de tiempo

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;
	
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready,callingthread);
	}
	
	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
	}
	
	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
	}
	
	if(event==TIMER)// Si el evento es el TIMER
	{
		q++;	// Incrementamos el Quantum de tiempo en 1
		
		if(q==QUANTUM)	// QUANTUM = 2
		{
			// El hilo que está en ejecución se pone en el estado de listo
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);	// y se va a la cola de listos
			changethread=1;	// Hacer cambio de hilo
			q = 0;	// Reiniciamos los quantum
		}
	}
	
	if(changethread)
	{
		old=currthread;
		next=_dequeue(&ready);
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
