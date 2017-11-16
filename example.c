#include "lib.h"
#include <stdio.h>

#include <unistd.h>
	
void fiber1()
{
	int i;

	for ( i = 0; i < 5; ++ i )
	{
		printf( "Hey, I'm fiber #1: %d\n", i );
		//fiberYield();
	}
	return;
}

void fibonacchi()
{
	int i;
	int fib[2] = { 0, 1 };
	
	/*sleep( 2 ); */
	printf( "fibonacchi(0) = 0\nfibonnachi(1) = 1\n" );
	for( i = 2; i < 15; ++ i )
	{
		int nextFib = fib[0] + fib[1];
		printf( "fibonacchi(%d) = %d\n", i, nextFib );
		fib[0] = fib[1];
		fib[1] = nextFib;
		//fiberYield();
	}
}


void readFile(){
	FILE *fptr;
    char filename[15];
    char ch;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int value;

	int add = 0;

	int temp;

	fptr = fopen("one.txt", "r");

	if (fptr == NULL)
	{
		printf("Cannot open file \n");
		exit(0);
	}
	while ((read = getline(&line, &len, fptr)) != -1) {
		//printf("Retrieved line of length %zu :\n", read);
		char var = line[0];
		temp= (int)(var - '0');
		printf("%d\n",temp);

		add = add+temp;

		
	}


	printf("sum %d\n",add);
	fclose(fptr);
}

void readFile1(){
	FILE *fptr;
    char filename[15];
    char ch;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int value;

	int add = 0;

	int temp;

	fptr = fopen("two.txt", "r");

	if (fptr == NULL)
	{
		printf("Cannot open file \n");
		exit(0);
	}
	while ((read = getline(&line, &len, fptr)) != -1) {
		//printf("Retrieved line of length %zu :\n", read);
		char var = line[0];
		temp= (int)(var - '0');
		printf("%d\n",temp);

		add = add+temp;

		
	}


	printf("sum %d\n",add);
	fclose(fptr);
}	
void squares()
{
	int i;
	
	/*sleep( 5 ); */
	for ( i = 0; i < 10; ++ i )
	{
		printf( "%d*%d = %d\n", i, i, i*i );
		//fiberYield();
	}
}



void fileread(){
	int i;

}

int main()
{

	int i,j;

	/* Initialize the library */
	initThreads();
	
	/* Create Thread ! */
	createThread( &fiber1 );
	createThread( &fibonacchi );
	createThread( &squares );
	createThread( &readFile );
	createThread( &readFile1 );
	/* Since these are nonpre-emptive, we must allow them to run */
	waitForAllThreads();
	
	/* The program quits */
	return 0;
}