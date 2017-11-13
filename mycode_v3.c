#define _GNU_SOURCE
#include <sched.h> 
#include <signal.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <stdio.h>

// PREDEFINED VARIABLES 
#define MAX_SIZE_QUEUE 3
#define MAX_FIBERS 2
#define FIBER_STACK (1024*1024)

//STRUCTURE OF FIBER CONTAINING NECESSARY DETAILS
typedef struct
{
	pid_t pid; 
	void* stack; 
	int active;
} fiber;

//QUEUE OF FIBERS
struct queue
{
	void (*function)();
	
	struct queue *next;
	
};


struct queue * rear = NULL;
struct queue * front = NULL;


//ARRAY OF FIBERS
static fiber fiberList[ MAX_FIBERS ];

// PARENT PROCESS PID

static pid_t parentPid;

// NUMBER OF FIBERS WHICH ARE CURRENTLY RUNNING
static int numFibers = 0;

static int cnt=0; 

//////////////////////////////////////////////
void initThreads() // This will be executed in the beginning of the main function.
{
	int i;
	for ( i = 0; i < MAX_FIBERS; ++ i )
	{
		fiberList[i].pid = 0;
		fiberList[i].stack = 0;
		fiberList[i].active = 0;
	}
	
	parentPid = getpid();
}
/////////////////////////////////////////////////

int freechecker() // Returning the first index from the array of fiber for allocating the function.
{
	int i;
	for(i=0;i<MAX_FIBERS;i++)
	{
		if(fiberList[i].active==0)
			return i;
	}
	return -1;
}
/////////////////////////////////////////////////

int calcative() // Return the number of fibers which are currently active.
{
	int i,no=0;
	for(i=0;i<MAX_FIBERS;i++)
	{
		if(fiberList[i].active==1)
			no++;
	}
	return no;
}

///////////////////////////////////////////////

void enqueue(void (*func)(void))
{
	if(front == NULL)
	{
		printf("in if ");
		front = (struct queue *)malloc( sizeof(struct queue) );
		front->next=NULL;
		rear=front;
		front->function=func;
		cnt++;
		printf("Enqueue successful..!\n");
		return;
	}
	else
	{
		printf("in else ");
		struct queue *temp;

		for(temp=front;temp->next != NULL;temp=temp->next);
		temp->next=(struct queue *)malloc( sizeof(struct queue) );
		temp=temp->next;
		temp->next = NULL;
		rear = temp;
		temp->function=func;
		cnt++;
		printf("Enqueue successful..!\n");
		return;
	}

}

//////////////////////////////////////////////////////////////

struct queue * dequeue()
{
	if(front==NULL || rear==NULL)
		printf("Queue is empty");
	else if(front == rear)
	{
		struct queue * temp;
		temp =  front;
		front = NULL;
		rear=front;
		cnt--;
		printf("Function dequeued successfully.!");
		return temp;
	}
	else
	{
		struct queue * temp;
		temp =  front;	
		front=front->next;
		cnt--;
		printf("Function dequeued successfully..!");
		return temp;
	}
}

/////////////////////////////////////////////////

int isEmpty()
{
    if(front == NULL){
        printf("\nQueue is empty!");
    	return 1;
    }
    else{
        printf("\nQueue is heavy.!!");
    	return 0;
    }
}

///////////////////////////////////////////////////

void isFull()
{
    if(cnt >= MAX_SIZE_QUEUE)
        printf("\nQueue is full!");
    else
        printf("\nSpace remaining..!");
}

/////////////////////////////////////////////////

struct FiberArguments { // Fiber argument structure which contains the function to be executed by the thread.
	void (*function)();
};

////////////////////////////////////////////////

static int fiberStart( void* arg ) //  Called in clone method. It is the main function which executed by every thread.
{
	struct FiberArguments* arguments = (struct FiberArguments*) arg;
	void (*function)() = arguments->function;
	free( arguments );
	arguments = NULL;

	function();
	return 0;
}
//////////////////////////////////////////////////

int spawnFiber( void (*func)(void) ) //  Called when new job is to be allocated to thread.
{
 	struct FiberArguments* arguments = NULL;
	
	int idx=freechecker();

	//check all the fibres are full. If yes, then enter the function into queue.
	if(idx==-1)// Queuing the functions so that later it can be executed.
	{
		if(cnt < MAX_SIZE_QUEUE)
		{
			enqueue(func);
		}
		else{
			return -1;
		}
	}else{

		fiberList[idx].stack = malloc(FIBER_STACK);
		if ( fiberList[idx].stack == 0 )
		{
			printf("Error : Could not allocates stack");
			return -1;
		}

		arguments = (struct FiberArguments*)malloc(sizeof(*arguments));
		if ( arguments == 0 ) {
			free( fiberList[idx].stack);
			printf("Error: Could not allocate fiber arguments.");
			return -1;
		}
		arguments->function = func;

		// Child thread creation.	
		fiberList[idx].pid = clone( &fiberStart,fiberList[idx].stack+FIBER_STACK ,SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, arguments );
		
		if (fiberList[idx].pid == -1)
		{
			//clone doesn't works
			free(fiberList[idx].stack);
			free(arguments);
			printf("Error: clone system call failed.");
			return -2;

		}
		else
		{
			//clone work
			//make the thread is active
			fiberList[idx].active = 1;
			printf("%d pid get active\n",fiberList[idx].pid);
		}
		
	}
	return 0;
}

void queuetoActive(){
	struct queue * temp = dequeue();

	void (*func)() = temp->function;

	spawnFiber(func);
} 

////////////////////////////////////////////////////////////////////////////

int waitForAllFibers()// Non-preemptive threads.
{
	pid_t pid;
	int i;
	/* Check to see if we are in a fiber, since we don't get signals in the child threads */
	pid = getpid();
	if ( pid != parentPid ) return 0;			
		
	/* Wait for the fibers to quit, then free the stacks */
	while (1)
	{

		pid = wait(0);
		if (pid == -1)
		{
			printf("Error : wait system call failed");
			exit(1);
		}
		
		int found = 0;
		for( i = 0; i < MAX_FIBERS; i++ )
		{
			if( fiberList[i].pid == pid )
			{

				fiberList[i].active=0;
				printf("Child fiber pid = %d exited\n", pid);
				
				free( fiberList[i].stack );

				if(isEmpty() == 0)
					queuetoActive();

				found = 1;
				break;
			}
		}
		if ( found == 0)
		{
			printf("Did not find pid = %d in the fiber list",pid);
		}
		int active = calcative();
		if(active == 0 ){
			break;
		}
	}
	
	return 0;
}


void fiber1()
{
	int i;
	for ( i = 0; i < 2; ++ i )
	{
		printf( "Hey, I'm fiber #1: %d\n", i );
	}
	return;
}

void fiber2()
{
	int j;
	for ( j = 0; j < 2; ++ j )
	{
		printf( "Hey, I'm fiber #2: %d\n", j );
	}
	return;
}



int main()
{
	/* Initialize the fiber library */
	initThreads();
	
	spawnFiber(&fiber1);

	spawnFiber(&fiber2);

	spawnFiber(&fiber2);


	spawnFiber(&fiber2);

	waitForAllFibers();
	
	return 0;
}