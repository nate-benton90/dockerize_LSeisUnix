#include <stdio.h>
/* #include <cur00.h> */
#include "lists.h"

main()
{
	int x,y;
	char str[StringLength], name[StringLength], name2[StringLength];
	char out[StringLength];
	FILE *jobfile, *Template;
	struct list *requ, *opt, *current;
	struct info2 *Header;
	WINDOW *win;
	int firstflag;

/****************************************
 *	initialize curses and 		*
 *	all the needed variables	*
 ****************************************/
	initscr();
	strcpy(out, "pipe\0");
	strcpy(str, "stdin\0");
	firstflag = 0;
	TermGet(&col, &lin);
	clear();
	erase();
	echo();
	refresh();

	Startup();		/* Prints the intro type stuff. */

	jobfile = GetJob(jobfile, &firstflag);

	while(strcmp(out,"pipe") == 0) {
/****************************************
 *		Loop until the end	*
 *		of the pipeline has	*
 *		been reached, or the	*
 *		the user aborts.	*
 ****************************************/

		GetProg(name);
				/* Gets the name of the program to work
				 * with and checks to see if a template
				 * exists for it yet.  Returns nil if there
				 * is no template.	*/
		while (name[0] == NULL) {
			clear();
			erase();
			refresh();
			move(10, 10);
			standout();
			printw(" No Template File For Program ");	
			move(11, 11);
			printw(" And Unable To Generate One ");
			standend();
			refresh();
			GetProg(name);
		}

/********************************
 *	Extract the name of the	*
 *	su program for safe	*
 *	keeping.		*
 ********************************/
		x = 0;
		while(name[x] != '.') {
			name2[x] = name[x];
			x++;
		}
		name2[x] = '\0';

		LoadTemplate(name, &requ, &opt, &Header);

		y = DrawTop(Header);	/* Puts up the header	*/

/********************************
 *	Check to see where	*
 *	input is coming from	*
 *	if it is the first	*
 *	command.		*
 *	(assume a pipe if it	*
 *	is not the first)	*
 ********************************/
		if (firstflag == 0) {
			move(y+5, 10);
			printw("Input ([stdin], file) : ");
			refresh();
			input(str);
			if (str[0] == NULL) strcpy(str, "stdin\0");
			firstflag = 1;
			move(y+4, 10);
			printw(".          .          .          .          .");
			move(y+5, 10);
			printw("..          .....          ...          .....");
			refresh();
			move(y+4, 10);
			printw("                                             ");
			move(y+5, 10);
			printw("                                             ");
			refresh();
		}


		win = MakeWindow(y);
		y += 3;
		if (requ != NULL) {
			SetRequired(y, requ, win);
			wclear(win);
			wrefresh(win);
		}
		if (opt != NULL) {
			SetOptional(y, opt,win);
			wclear(win);
			wrefresh(win);
       		 }
/*	ask about the output here */

		move(14,10);
		printw("Output ([pipe], file, stdout) : ");
		refresh();
		input(out);
		if (out[0] == NULL) strcpy(out, "pipe\0");
	
		MakeJobFile(jobfile, name2, requ, opt, str, out);

	}

/********************************
 *	close files and clean	*
 *	up after curses.	*
 ********************************/
	echo();
	fclose(jobfile);
	erase();
	clear();
	refresh();
	endwin();
}


/************************************************************************
 *	GetJob checks to see if the job file exists or not		*
 *	and handles both situations.  Exits the program if		*
 *	the user doesn't want to use the file, and returns 		*
 *	the file pointer in any other case.				*
 ************************************************************************/
FILE *GetJob(name, append)
FILE *name;
int *append;
{	FILE *fp;
	int flag;		/*  flag for file status
				 *  1 = append to existing file
				 *  2 = create new file
				 *  3 = exit leaving file as is */
	char temp[StringLength], a;

	move(10,5);
	printw("Please enter a name for the job file : ");
	refresh();
	input(temp);
	if ((fp = fopen(temp,"r")) != NULL) {
		move(12,20);
		standout();
		printw("File Already Exists ");
		standend();
		move(13,10);
		printw("append to file? [y] ");
		refresh();
		a = getch();
		if ((a == 'n') || (a == 'N')) {
			move(14,10);
			printw("delete file? [n] ");
			refresh();
			a = getch();
			if ((a == 'y') || (a == 'Y')) flag = 2;
			else flag = 3;
		}
		else flag = 1;
		fclose(fp);
	}
	else flag =2;
	if (flag == 3) exit(0);
	if (flag == 2) name = fopen(temp, "w");
	else {
		*append = 1;
		name = fopen(temp, "a");
	}
	return(name);
}


/************************************************************************
 *		Just sticks up the little title				*
 ************************************************************************/
Startup()
{	move(2,25);
	printw(" Easy SU Interface \n                Job File Builder for Seismic UNIX");
	refresh();
}

/************************************************************************
 *		Gets the name of the su program.  Appends the 		*
 *		extention for the template files onto the end		*
 *		and checks for the existance of the template file	*
 *		A return will exit the program.				*
 ************************************************************************/
GetProg(name)
char *name;
{	FILE *fp;
	char temp[StringLength], other[StringLength];
	char temp2[StringLength], other2[StringLength];
	char *name2, *comand;
	int len;

	move(16,5);
	printw("Which SU program would you like to use? ");
	refresh();
	input(name);
	strcpy(temp2, name);

	/*	deal with a return	*/
	if (name[0] == NULL) {
		clear();
		echo();
		refresh();
		endwin();
		exit(0);
	}

	strcat(name, Extension);
	if ((fp = fopen(name, "r")) == NULL) {
		strcpy(other, PREFIX);
		len = strlen(temp2);
		name2 = strcat(other, (strcat(temp2, ".c\0")));
		if ((fp = fopen(name2, "r")) == NULL) name[0] = NULL;
		else {
			strcpy(other2, "generate \0");
			temp2[len] = '\0';
			comand = strcat(other2, temp2);
			system(comand);
		}
	}
	else fclose(fp);
}


/************************************************************************
 *		Writes the su program name, and all of its		*
 *		parameters out to the file.  Adds any input		*
 *		or output redirection, and goes to the next line	*
 *		of the file.						*
 ************************************************************************/
MakeJobFile(jobfile,template,required,optional, source, outfile)
FILE *jobfile;
char template[StringLength], *source, outfile[StringLength];
struct list *required, *optional;
{	struct list *current;


	current = required;
        fprintf(jobfile, "%s ", template);

	if (strcmp(source,"stdin\0") != 0) {
		fprintf(jobfile, "< %s ", source);
		strcpy(source, "stdin\0");
	}

	while (current != NULL){
		fprintf(jobfile, "%s", current->variable);
		fprintf(jobfile, "=");
                fprintf(jobfile, "%s ", current->value);
		current=current->next;
	}

	current = optional;
	while (current != NULL){
		if (current->changed == 'y'){
		     fprintf(jobfile, "%s", current->variable);
		     fprintf(jobfile, "=");
		     fprintf(jobfile, "%s ", current->value);
		}     
		current=current->next;
	}

/****************************************
 *	finishes the line and does the	*
 *	line feed.  The backslach on	*
 *	the end of the line is needed	*
 *	so that UNIX knows the the	*
 *	commmand is continued on the	*
 *	next line.			*
 ****************************************/
	if (strcmp(outfile,"pipe") == 0) fprintf(jobfile, "| \\\n");
	else if (strcmp(outfile,"stdout") != 0)
		 fprintf(jobfile, "> %s \n",outfile);
}






