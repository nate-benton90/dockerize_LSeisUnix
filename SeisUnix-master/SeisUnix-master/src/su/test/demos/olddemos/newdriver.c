/* Catch repeated interrupts */

#include <signal.h>
#include <setjmp.h>

jmp_buf env;

main()
{
	void catch_int();
	char line[81];


	signal(SIGINT, catch_int);

	setjmp(env);
	printf("READY\n");

	gets(line);
}


void catch_int()
{
	signal(SIGINT, catch_int);
	longjmp(env);
}
