/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/* SUNRM: $Revision: 1.7 $ ; $Date: 1996/03/26 20:57:27 $	*/

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 	   							",
" SUNaN - Check for unreasonable values of amplitudes           ", 
"         Edits traces that have amplittudes that a not finite  ",
" 	   							",
" sunrm <stdin >stdout                                          ",
" 								",
" Required parameters:						",
"	NONE                                            	",
"								",
NULL};

/*
 * Author: RC 1998
 *
 *
 * Trace header fields accessed: ns
 */
/**************** end self doc ***********************************/
segy tr;

int chkamp(float *data, int *n1);
int isinf(double value);
int isnan(double value);



int main(int argc, char **argv)
{
	int idm,ns;

	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

	/* Get parameters */
	if (!fgettr(stdin, &tr)) err("can't read first trace");
	ns = (int) tr.ns;

	/* Loop over traces */

	do {
                idm=chkamp(&tr.data[0],&ns); 
		if ( !idm ) puttr(&tr);
	} while (gettr(&tr));

	return EXIT_SUCCESS;
}


int chkamp(z,n)

float *z;
int  *n;

/*
   Subroutine : chkamp.c
   Function : cheks amplitude values
   Author : Ramon Carbonell
   Version :
   Date : June, 1996
*/

{
   float rsq=0;
   int i,idm;
   double dr0;
    
   for (i=0;i<(*n);i++) rsq=rsq + (*(z+i))*(*(z+i));
   rsq=(sqrt(rsq))/(*n);
   dr0=(double)rsq;
   idm=finite(dr0);
   if(! idm ||  rsq) idm=0;  
   return (idm);
}


