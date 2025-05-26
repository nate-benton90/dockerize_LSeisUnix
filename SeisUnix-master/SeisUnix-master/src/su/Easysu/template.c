#include <stdio.h>
#include "lists.h"

#define MARK1 "Required Parameters:\n"
#define MARK2 "Optional Parameters:\n"
#define MARK3 "Other Information:\n"
#define MARK4 "-\n"
#define MARK5 "END.\n"

char *malloc();

/************************************************************************
 *		LoadTemplate loads the template file into the		*
 *		structures for latter use.  The template file MUST	*
 *		be of the right form to avoid problems.			*
 *		The form of the template is:				*
 *		<Lines of header>					*
 *		Required Parameters:					*
 *		[<variable name>					*
 *		 <description>						*
 *		  [<more lines of description>]				*
 *		 -							*
 *		]							*
 *		Optional Parameters:					*
 *		[<variable name>					*
 *		 <default value>					*
 *		 <description>						*
 *		  [<more lines of description>]				*
 *		 -							*
 *		]							*
 *		Other Information:					*
 *		[<lines of extra information>]				*
 *		END.							*
 ************************************************************************/
LoadTemplate(name, required, optional, header)
char *name;
struct info2 **header;
struct list **required;
struct list **optional;
{	FILE *template;
	int len;
	char str[2*StringLength];
	struct list *current, *current2;
	struct info *finger, *finger2;
	struct info2 *point, *point2;

	if ((template = fopen(name,"r")) == NULL) exit(0);
	*optional = NULL;
	*required = NULL;

/****************************************
 *	Gets the first line of the	*
 *	file and puts it in the header	*
 ****************************************/
	fgets(str, (2*StringLength), template);
	*header = (struct info2 *)malloc(sizeof(struct info2));
	strcpy((*header)->stuff, str);
	(*header)->next = NULL;
	point = *header;

	fgets(str, (2*StringLength), template);

	while(strcmp(str, MARK1) != 0) {
		point2 = (struct info2 *)malloc(sizeof(struct info2));
		strcpy(point2->stuff, str);
		point2->next = NULL;
		point->next = point2;
		point = point2;
		fgets(str, (2*StringLength), template);
	}
		
	if (strcmp(str,MARK1) == 0) {
/********************************
 *	Loops until it reaches 	*
 *	optional parameters.	*
 ********************************/
	   fgets(str, (2*StringLength), template);
	   while(strcmp(str, MARK2) != 0) {
		if (strcmp(str,MARK4) != 0) {
			current = (struct list *)malloc(sizeof(struct list));
			current->description = NULL;
			current->next = NULL;
			len = (int)strlen(str);
			str[len - 1] = '\0';
			strcpy(current->variable, str);
			fgets(str, (2*StringLength), template);
			while(strcmp(str, MARK4) != 0) {
				finger = (struct info *)malloc(sizeof(struct info));
				if (current->description == NULL) 
					current->description = finger;
				else finger2->next = finger;
				len = (int)strlen(str);
				str[len - 1] = '\0';
				strcpy(finger->stuff, str);
				finger->next = NULL;
				finger2 = finger;
				fgets(str, (2*StringLength), template);
			}
			if (*required == NULL) {
				*required = current;
				current2 = current;
			}
			else {
				current2->next = current;
				current->prev = current2;
				current2 = current;
			}
			fgets(str, (2*StringLength), template);
		}
	   }
	   fgets(str, (2*StringLength), template);
	   while(strcmp(str, MARK3) != 0) {
		if (strcmp(str,MARK4) != 0) {
			current = (struct list *)malloc(sizeof(struct list));
			current->description = NULL;
			current->next = NULL;
			current->changed = 'f';
			len = (int)strlen(str);
			str[len - 1] = '\0';
			strcpy(current->variable, str);
			fgets(str, (2*StringLength), template);
			len = (int)strlen(str);
			str[len - 1] = '\0';
			strcpy(current->value, str);
			fgets(str, (2*StringLength), template);
			while(strcmp(str, MARK4) != 0) {
				finger = (struct info *)malloc(sizeof(struct info));
				finger->next = NULL;
				if (current->description == NULL) 
					current->description = finger;
				else finger2->next = finger;
				len = (int)strlen(str);
				str[len - 1] = '\0';
				strcpy(finger->stuff, str);
				finger2 = finger;
				fgets(str, (2*StringLength), template);
			}
			if (*optional == NULL) {
				*optional = current;
				current2 = current;
			}
			else {
				current2->next = current;
				current->prev = current2;
				current2 = current;
			}
			fgets(str, (2*StringLength), template);
		}
	   }
	   fgets(str, (2*StringLength), template);
	   while(strcmp(str, MARK5) != 0) {
		point2 = (struct info2 *)malloc(sizeof(struct info2));
		point2->next = NULL;
		strcpy(point2->stuff, str);
		point->next = point2;
		point = point2;
	   	fgets(str, (2*StringLength), template);
	   }
	}
	fclose(template);
}


/************************************************************************
 *			Simple input routine to make the		*
 *			curses input a little more portable		*
 ************************************************************************/
input(str)
char str[StringLength];
{	int i;
	char a, cr;

	char buffer[80];

	i = read(0, str, StringLength);
		
	str[i-1] = '\0';
}


/************************************************************************
 *			Another input routine.  Same as input()		*
 *			except it takes the extra window parameter	*
 *			so as to remain consitent with curses calls	*
 ************************************************************************/
winput(win, str)
WINDOW *win;
char str[StringLength];
{	int i;
	char *a, cr;
	
	i = read(0, str, StringLength);
	str[i-1] = '\0';
	
}


