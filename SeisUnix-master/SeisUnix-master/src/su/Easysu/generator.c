#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include "lists.h"

#define DEBUG 1

main(argv, argc)
char *argc[];
int argv;
{	char str[80], name[30], *temp, name2[30];
	char otherinfo[10][80];
	int i, next, count;
	struct list node;
	FILE *template, *docfile;
	
	if (argv !=2) {
		printf("syntax: generate <suprog> \n");
		exit(0);
	}	
	strcpy(name, argc[1]);
	strcat(name, Extension);
	if ((template = fopen(name, "r")) != NULL) {
		fclose(template);
		printf("Template file has already been created \n");
		exit(0);
	}		
	strcpy(name2, name);

	for(next=0; next < 10; next++) otherinfo[next][0] = '\0';
	next = 0;
	strcpy(name, PREFIX);
	strcat(name, argc[1]);
	strcat(name, ".c\0");
	if ((docfile = fopen(name, "r")) == NULL) {
		fclose(docfile);
		printf("Unable to find source code to produce template.  \n");
		exit(0);
	}
	template = fopen(name2, "w");

	for (i=0; argc[1][i]; i++) if (isdigit(argc[1][i])) 
					name[i] = argc[1][i];
				   else name[i] = toupper(argc[1][i]);
	name[i] = '\0';
	str[0] = '\0';

	while (compare(str, name)) fgets(str, 80, docfile);

if (DEBUG)
	fprintf(stderr, "got to here");

	strcpy(str, strip(str));
	fputs(str, template);		/* puts the header in */
	fgets(str, 80, docfile);
	while (BlankLine(str)) {
		strcpy(str, strip(str));
		fputs(str, template);
		fgets(str, 80, docfile);
if (DEBUG)
	fprintf(stderr, "got to here: %s.\n", str);
	}


	fputs("Required Parameters:\n", template);


	while ((compare(str, "Required\0")) && (compare(str,
		"Optional\0")) && (compare(str, "\";\0"))){
		 fgets(str, 80, docfile);
if (DEBUG)
	fprintf(stderr, "Looking for whatever. : %s.\n", str);
	}
	if (compare(str, "Required\0") == 0) {
		fgets(str, 80, docfile);
		while(comment(str))
			fgets(str, 80, docfile);
		while(compare(str, "Optional\0")) {
			strcpy(str, strip(str));
			if(compare(str, "none\0")) {
				if (noequal(str)) {
					strcpy(otherinfo[next++], str);
					fgets(str, 80, docfile);
					while(comment(str))
						fgets(str, 80, docfile);
				}
				else RequBreakDown(str, template, docfile);
			}
			else {
				fgets(str, 80, docfile);
				while(comment(str))
					fgets(str, 80, docfile);
				if (BlankLine(str) == 0) 
					strcpy(str, "none\0");
			}
			while(BlankLine(str) == 0) {
				fgets(str, 80, docfile);
				while(comment(str))
					fgets(str, 80, docfile);
			}	
		}	
	}


	fputs("Optional Parameters:\n", template);
	if (compare(str, "\";\0")) {
		fgets(str, 80, docfile);
		while(comment(str))
			fgets(str, 80, docfile);
	}
if (DEBUG)
	fprintf(stderr, "looking for notes1.  : %s.\n", str);

	while((compare(str,"NOTES:\0")) && (compare(str,"\";\0"))
		&& (indent(str))) {
		strcpy(str, strip(str));
		if(compare(str, "none\0")) {
			OptBreakDown(str, template, docfile);
		}
		else {
			strcpy(str, "NOTES:\0");
		}
if (DEBUG)
	fprintf(stderr, "looking for notes2.  : %s.\n", str);
		while(BlankLine(str) == 0){
			fgets(str, 80, docfile);
			while(comment(str))
				fgets(str, 80, docfile);
		}
	}

if (DEBUG)
	fprintf(stderr, "got to here : %s.\n",str);

	while (compare(str, "\";\0")) {
		if (compare(str, "NOTES:\0") == 0) {
			fgets(str, 80, docfile);
if (DEBUG)
	fprintf(stderr, "looking for end : %s.\n",str);
			while (BlankLine(str)) {
				strcpy(otherinfo[next++],strip(str));
				fgets(str,80,docfile);
if (DEBUG)
	fprintf(stderr, "looking for balnkline : %s.\n",str);
			}
		}
		fgets(str, 80, docfile);
	}

if (DEBUG)
	fprintf(stderr, "got to end string is: %s.\n", str);

	fputs("Other Information:\n", template);
	for(count=0; count < next; count++) fputs(otherinfo[count], template);

	fputs("END.\n", template);
	fclose(docfile);
	fclose(template);
}


