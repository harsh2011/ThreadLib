#define _GNU_SOURCE

#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>


#define MAX_THREAD 10

#define THREAD_STACK (1024*1024)

/* The Fiber Structure
*  Contains the information about individual fibers.
*/
typedef struct
{
	ucontext_t context; /* Stores the current context */
	int active; /* A boolean flag, 0 if it is not active, 1 if it is */
	/* Original stack pointer. On Mac OS X, stack_t.ss_sp is changed. */
	void* stack; 
} thread;

static ucontext_t mainContext;

/* The fiber "queue" */
static thread threadlist[ MAX_THREAD ];


/* A boolean flag indicating if we are in the main process or if we are in a fiber */
static int inThread = 0;

/* The number of active fibers */
static int numThreads = 0;



/* Sets all the fibers to be initially inactive */
void initThreads()
{
	int i;
	for ( i = 0; i < MAX_THREAD; ++ i )
	{
		threadlist[i].active = 0;
	}
		
	return;
}

int createThread(void (*func)(void) ){

	if (getcontext(&threadlist[numThreads].context) == -1)
        printf("getcontext error");
    threadlist[numThreads].context.uc_link = 0;
	threadlist[numThreads].stack = malloc(THREAD_STACK);
	threadlist[numThreads].context.uc_stack.ss_sp = threadlist[numThreads].stack;
	threadlist[numThreads].context.uc_stack.ss_size = THREAD_STACK;
	threadlist[numThreads].context.uc_stack.ss_flags = 0;
	
    makecontext(&threadlist[numThreads].context, func, 0);


    numThreads++;
}
int currentThread = -1;

int ThreadController(){
	if(currentThread == numThreads)
		return 1;

	
	printf("currentThread : %d\n", currentThread);
	if(inThread){
		printf("Swap to main\n");
		printf("infiber : %d\n",inThread);
		inThread = 0;
		swapcontext(&threadlist[currentThread].context,&mainContext);
		
	}
	else{

		printf("swap to fibre\n");
		printf("infiber : %d\n",inThread);
		inThread = 1;
		currentThread++;
		swapcontext(&mainContext,&threadlist[currentThread].context);
		
		//printf("infiber : %d\n",inFiber);
	}
}

void waitForAllThreads(){
	
	int i = 0;
	while(i<numThreads){
		ThreadController();
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
	ThreadController();
	return;
}

void fiber2()
{
	int j;
	for ( j = 0; j < 5; ++ j )
	{
		printf( "Hey, I'm fiber #2: %d\n", j );
	}
	ThreadController();
	return;
}



int main(){
	/* Initialize the fiber library */
	initThreads();
	
	/* Go fibers! */
	
	createThread(&fiber1);
	createThread(&fiber2);

	/* Since these are nonpre-emptive, we must allow them to run */
	waitForAllThreads();
	
	/* The program quits */
	return 0;
}


