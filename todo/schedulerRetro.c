#include <scheduler.h>
#include <stdio.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int totthreads;	// Para saber cuando solo hay 1 proceso y que no descienda
extern int blockevent;
extern int unblockevent;

#define LEVELS 100	// Numero de niveles de prioridad
int priority[LEVELS];	// Llevar la cuenta

QUEUE ready[LEVELS];	// Varias colas de listos por niveles de prioridad
QUEUE waitinginevent[MAXTHREAD];

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;
	
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;
	
	int i;	// Contador de busqueda en colas de prioridad
	
	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		
		priority[callingthread] = 0;	// Iniciamos la prioridad 0
		_enqueue(&ready[priority[callingthread]],callingthread);	// Entra a la cola nivel 0
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
			_enqueue(&ready[priority[callingthread]],callingthread);
	}

	if(event==TIMER)// Si el evento es el TIMER
	{
		if(priority[callingthread] <= LEVELS)	// LLegar a un limite de niveles
			priority[callingthread]++;	// Aumentamos un nivel de prioridad
		
		// Si es el unico proceso, no debe descender niveles de prioridad
		if(totthreads==0)	// 0 porque, tomamos en cuenta al hilo 0
		{
			priority[callingthread]--;	// Compensamos para NO descender
		}
		
		threads[callingthread].status=READY;	// El hilo en ejecución se pone en estado de listo
		_enqueue(&ready[priority[callingthread]],callingthread);	// y se va a la cola de listos
		changethread=1;	// Hacer cambio de hilo
	}
	
	if(changethread)
	{
//		printf("-Prioridad: %d del hilo %d \n", priority[callingthread], callingthread);
		
		// Buscar si las colas de mayor prioridad estan vacias, y si no, realiza el cambio
		i=0;
//		printf("Comienza a buscar la mayor prioridad \n");
		while(_emptyq(&ready[i]) == 1) // Si la cola esta vacia sigue buscando...
		{
			//printf("Nivel: %d ", i);
			i++;
		} // end while
		
//		printf("Saca proceso de la cola: %d \n\n", i);
		old=currthread;
		next=_dequeue(&ready[i]);	// Saca proceso de la cola de prioridad mas alta

		threads[next].status=RUNNING;
		_swapthreads(old,next);
	} // end if

}
