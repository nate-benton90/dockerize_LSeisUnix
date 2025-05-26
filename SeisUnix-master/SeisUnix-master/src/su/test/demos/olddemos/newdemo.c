/* demo - menu for SEG demonstration
 *
 * Credits:
 *	Rochkind, Advanced Unix Programming, Prentice-Hall, 1985
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
 *
 * $Author: jkc $
 * $Source: /data/Cwpdemo/RCS/demo.c,v $
 * $Revision: 1.21 $ ; $Date: 88/10/23 04:38:47 $
*/


#include "jc.h"
#include <signal.h>
#include <setjmp.h>

jmp_buf env;


main()
Begin
	void mainloop(), help(), prompt(), catch_int();


	signal(SIGINT, catch_int);
	setjmp(env);


	setbuf(stdout, NULL);
	system("cleargp");
	help();
	mainloop();

	return SUCCEED;
End


static void mainloop()	/* process commands */
Begin
	char cmd[BUFSIZ], shcmd[BUFSIZ];

	While TRUE Do
		prompt("Choice", cmd);
		If strlen(cmd) > 1 Do
			cmd[0] = '\1'; /* force unknown command msg */
		Endif

		On cmd[0] Do
		Case 'q':
		Case 'Q':
			exit(SUCCEED);
		Case 'a':
		Case 'A':
			system("autodemo");
			system("cleargp");
			help();
			Continue;
		Case 'b':
		Case 'B':
			system("sh");
			system("cleargp");
			help();
			Continue;
		Case 'c':
		Case 'C':
			system("csh");
			system("cleargp");
			help();
			Continue;
		Case 'g':
		Case 'G':
			system("graphicsdemo");
			system("cleargp");
			help();
			Continue;
		Case 'm':
		Case 'M':
			system("moviedemo");
			system("cleargp");
			help();
			Continue;
		Case 's':
		Case 'S':
			system("sudemo");
			system("cleargp");
			help();
			Continue;
		Case 't':
		Case 'T':
			system("techdemo");
			system("cleargp");
			help();
			Continue;
		Case '1':
			system("trisodemo");
			system("cleargp");
			help();
			Continue;
		Case '2':
			system("shotdemo");
			system("cleargp");
			help();
			Continue;
		Case '3':
			system("elasdemo");
			system("cleargp");
			help();
			Continue;
		Case '4':
			system("dmodemo");
			system("cleargp");
			help();
			Continue;
		Case '5':
			system("inverdemo");
			system("cleargp");
			help();
			Continue;
		Case '!':
			prompt("Shell command", shcmd);
			system(shcmd);
			system("cleargp");
			help();
			Continue;
		Case '\0':
		Case '?':
			system("cleargp");
			help();
			Continue;
		Default:
			printf("Unknown command; use ? for help\n");
			Continue;
		Endon
	Endwhile
End


static void help()	/* display menu */
Begin
	printf("\n\n\t\t*------------CWP INTERACTIVE DEMOS-----------*\n\n");
	printf("\t\tG   CWP Processing Graphics\n\n");
	printf("\t\tS   SU Tutorial\n\n");
	printf("\t\tM   Movies\n\n");
	printf("\t\tT   Tour of the SU source code\n\n");
	printf("\n\t\t*-----------------UTILITIES------------------*\n\n");
	printf("\t\tc   spawn a C shell (^D to exit)\n\n");
	printf("\t\tb   spawn a Bourne shell (^D to exit)\n\n");
	printf("\t\t!   execute single shell command\n\n");
	printf("\t\t?   display command summary\n\n");
	printf("\t\tq   quit\n\n\n");
	printf("\t\ta  autodemo");
	printf("\t1  trisodemo");
	printf("\t2  shotdemo\n");
	printf("\t\t3  elasdemo");
	printf("\t4  dmodemo");
	printf("\t5  inverdemo\n");
	printf("\t\tNote: autodemo = 1 through 5\n");
End


static void prompt(msg, result)	/* prompt user */
char *msg, *result;
Begin
	printf("\n%s? ", msg);
	If ERR_PTR == gets(result) Do
		exit(SUCCEED);
	Endif
End


static void catch_int()
Begin
	signal(SIGINT, catch_int);
	longjmp(env);
End
