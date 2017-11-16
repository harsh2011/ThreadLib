#ifndef LIBFIBER_H
#define LIBFIBER_H 1

#define	T_NOERROR	0
#define T_MAXFIBERS  1
#define T_MAXQUEUE 6
#define T_MALLOCERROR 2
#define T_CLONEERROR 3
#define	T_INFIBER 4
#define T_SIGNALERROR 5

// PREDEFINED VARIABLES 
#define MAX_SIZE_QUEUE 3
#define MAX_THREADS 2
#define THREAD_STACK (1024*1024)

#include "threadlib.c"

/* Should be called before executing any of the other functions. */
extern void initThreads();

/* Creates a new fiber, running the function that is passed as an argument. */
extern int createThread( void (*func)(void) );

/* Yield control to another execution context. */
//extern void fiberYield();

/* Execute the fibers until they all quit. */
extern int waitForAllThreads();


#endif