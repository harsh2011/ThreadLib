#define _GNU_SOURCE

#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>


#define MAX_FIBERS 10

#define FIBER_STACK (1024*1024)

/* The Fiber Structure
*  Contains the information about individual fibers.
*/
typedef struct
{
	ucontext_t context; /* Stores the current context */
	int active; /* A boolean flag, 0 if it is not active, 1 if it is */
	/* Original stack pointer. On Mac OS X, stack_t.ss_sp is changed. */
	void* stack; 
} fiber;

static ucontext_t mainContext;

/* The fiber "queue" */
static fiber fiberList[ MAX_FIBERS ];


/* A boolean flag indicating if we are in the main process or if we are in a fiber */
static int inFiber = 0;

/* The number of active fibers */
static int numFibers = 0;



/* Sets all the fibers to be initially inactive */
void initThreads()
{
	int i;
	for ( i = 0; i < MAX_FIBERS; ++ i )
	{
		fiberList[i].active = 0;
	}
		
	return;
}

int spawnFiber(void (*func)(void) ){

	if (getcontext(&fiberList[numFibers].context) == -1)
        printf("getcontext error");
    fiberList[numFibers].context.uc_link = 0;
	fiberList[numFibers].stack = malloc( FIBER_STACK );
	fiberList[numFibers].context.uc_stack.ss_sp = fiberList[numFibers].stack;
	fiberList[numFibers].context.uc_stack.ss_size = FIBER_STACK;
	fiberList[numFibers].context.uc_stack.ss_flags = 0;
	
    makecontext(&fiberList[numFibers].context, func, 0);


    numFibers++;
}
int currentFiber = -1;

int FiberController(){
	if(currentFiber == numFibers)
		return 1;

	
	printf("currentFiber : %d\n", currentFiber);
	if(inFiber){
		printf("Swap to main\n");
		printf("infiber : %d\n",inFiber);
		inFiber = 0;
		swapcontext(&fiberList[currentFiber].context,&mainContext);
		
	}
	else{

		printf("swap to fibre\n");
		printf("infiber : %d\n",inFiber);
		inFiber = 1;
		currentFiber++;
		swapcontext(&mainContext,&fiberList[currentFiber].context);
		
		//printf("infiber : %d\n",inFiber);
	}
}

void waitForAllFibers(){
	
	int i = 0;
	while(i<numFibers){
		FiberController();
		i++;
	}

}



void fiber1()


{
	int i;
	for ( i = 0; i < 5; ++ i )
	{
		printf( "Hey, I'm fiber #1: %d\n", i );
	}
	FiberController();
	return;
}

void fiber2()
{
	int j;
	for ( j = 0; j < 5; ++ j )
	{
		printf( "Hey, I'm fiber #2: %d\n", j );
	}
	FiberController();
	return;
}



int main(){
	/* Initialize the fiber library */
	initThreads();
	
	/* Go fibers! */
	
	spawnFiber(&fiber1);
	spawnFiber(&fiber2);

	/* Since these are nonpre-emptive, we must allow them to run */
	waitForAllFibers();
	
	/* The program quits */
	return 0;
}


