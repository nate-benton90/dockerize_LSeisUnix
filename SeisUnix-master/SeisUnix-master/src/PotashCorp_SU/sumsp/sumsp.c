/* sumsp.c */
/* B.Nemeth */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
char *sdoc[] = {"SUMSP - Modify sample in su file                       ",
"									",
" sumsp < infile > outfile						",
"									",
" ns=0		sample location						",
" a=1.0		sample value						",
"									",
" key=          Header word where sample time can be taken from if set	",
"									",
NULL};
   
/* Segy data constans */
segy tr;				/* SEGY trace */

int main( int argc, char *argv[] )
{
	int ns;			  /* sample number */
	float a;		  /* sample value */
	cwp_String key;	/* header key word from segy.h		*/
	cwp_String type;/* ... its type				*/
	int indx;	/* ... its index			*/
	Value val;	/* value of key in current gather	*/
	float dt;
	
	initargs(argc, argv);
   	requestdoc(1);
	
	if (!getparint("ns", &ns))	ns=0;
	if (!getparfloat("a", &a))	a=1.0;
	if (!getparstring("key", &key)) key = "NULL";
	
	if (key!="NULL") {
		type = hdtype(key);
		indx = getindex(key);
	}
        
	/* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
	if(tr.ns<ns) err("ns is out of bound\n");
	dt= (double) tr.dt/1000000.0;

	do {
		if (key!="NULL") {
			gethval(&tr, indx, &val);
			ns = NINT((vtof(type,val)-tr.delrt/1000.0)/dt);
		} 
		if (ns < 0 || ns > tr.ns )
			err(" ns or header word time is out of trace bounds\n");
		tr.data[ns]=a;
		puttr(&tr);
		
		
	} while(gettr(&tr));
	return EXIT_SUCCESS;
}
