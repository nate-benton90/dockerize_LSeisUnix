/* Copyright (c) Colorado School of Mines, 2000.*/
/* All rights reserved.                       */

/* SUFILTER: $Revision: 1.15 $ ; $Date: 1998/08/24 20:10:26 $        */


#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"								              ",
" SUIF  - Marks the traces identifiedd by the values of the key0, key1            ",
"         headers which are specified by ky0, ky1                                                  ",
"									",
" suIF <stdin >stdout [optional parameters]                	                            ",
"									",
" Rquired parameters:							",
"       key0=fldr           Primary key to select  traces                                         ",
"       key1=offset       Secondary key to select the traces                                ",
"       ifmode=0            0=include  1=exclude                                                       ", 
"       lstfl=0                 0=range 1=list                                                                 ", 
"       ky0=112,134       Values of the key0 header of traces                              ",
"       ky1=10000,20000   Values of the key1 header of traces                          ", 
"                                                                                                                            ",
"									",
NULL};

/* Credits:
 *      ICTJA: Ramon Carbonell
 *
 * Possible optimization: Do assignments instead of crmuls where
 * filter is 0.0.
 *
 * Trace header fields accessed: ns, dt
 */
/**************** end self doc ***********************************/

/* Prototype of function used internally */

segy tr;

int chkeven(int m0);

void main(int argc, char **argv)
{
    int i,j,iky0=1,iky1=1,nky0=0,nky1=0,iny0,iny1=0;	
    int lstfl=1,ifmode=1,*ky0id,*ky1id, ntm=0;
    cwp_String key0[SU_NKEYS];      /* array of keywords */
    cwp_String key1[SU_NKEYS];      /* array of keywords */
    cwp_String typ0[SU_NKEYS];      /* array of keywords */
    cwp_String typ1[SU_NKEYS];      /* array of keywords */
    int indx0[SU_NKEYS];             /* name of type of getparred key */
    int indx1[SU_NKEYS];             /* name of type of getparred key */
    Value val0,val1;                       /* value of key field */

    /* Initialize */
    initargs(argc, argv);
    requestdoc(1);
    /* get name of infile */  
    if ( !getparstringarray("key0",key0) ) iky0=0;
    if ( !getparstringarray("key1",key1) ) iky1=0;
    if ( (!iky0) &&  (!iky1) ) err("One of key0 or key1 is reqired  !!! ");
    if ( !getparint("ifmode",&ifmode) ) ifmode=0;
    if ( !getparint("lstfl",&lstfl) ) lstfl=0;
    if ( iky0 ) {
          nky0=countparval("ky0");
          ky0id=ealloc1int(nky0);
          for (i=0;i<nky0;i++)  getnparint(i,"ky0",ky0id);
    }
    if ( iky1 ) {
          nky1=countparval("ky1");
          if  ( (!chkeven(nky1)) && (!lstfl) ) 
                  err("Using the range option for lstfl, the number of ky1 mut be even");
          ky1id=ealloc1int(nky1);
         for (i=0;i<nky1;i++)  getnparint(i,"ky1",ky1id);
    }
    if ( !fgettr(stdin, &tr)) err("Can't read first trace !!!");   
    if (!ifmode) {
        if ( (!iky0) && (!iky1) ) {
            typ0[0]=hdtype(key0[0]);
            indx0[0]=getindex(key0[0]);              
            typ1[0]=hdtype(key1[0]);
            indx1[0]=getindex(key1[0]);
            do { 
                tr.ifflg=1;
                gethval(&tr, indx0[0], &val0);
                gethval(&tr, indx1[0], &val1);
                iny0 = (int) vtof(hdtype(key0[0]), val0);
                iny1 = (int) vtof(hdtype(key1[0]), val1); 
                for (i=0; i<nky0; i++) {
                       if (iny0 == ky0id[i] ) {
	              if (!lstfl) {
                                 ntm=(int) (nky1/2);
	                   for (j=0;j<ntm;j++)  if ( (iny1 < ky1id[2*j]) && (iny1 >= ky1id[2*j+1]))  tr.ifflg=0;
                           }
                          if ( lstfl) for (j=0;j<nky1;j++) if (iny1 == ky1id[j])   tr.ifflg=0;
                      }
                }
                puttr(&tr);
             } while (gettr(&tr)); 
        }
        if ( (iky0) && (!iky1) ) {            
            typ0[0]=hdtype(key0[0]);
            indx0[0]=getindex(key0[0]);              
            do {   
                tr.ifflg=1;
                gethval(&tr, indx0[0], &val0);
                iny0 = (int) vtof(hdtype(key0[0]), val0);
                for (i=0; i<nky0; i++) if (iny0 == ky0id[i] )  tr.ifflg=0;
                puttr(&tr);
             } while (gettr(&tr)); 
        }
        if ( (!iky0) && (iky1) ) {
           typ1[0]=hdtype(key1[0]);
           indx1[0]=getindex(key1[0]);
           do {        
                tr.ifflg=1;
                gethval(&tr, indx1[0], &val1);
                iny1 = (int) vtof(hdtype(key1[0]), val1); 
                if (!lstfl) {
                    ntm=(int) (nky1/2);
	      for (j=0;j<ntm;j++) if ( (iny1 < ky1id[2*j+1]) && (iny1 >= ky1id[2*j])) tr.ifflg=0;
                }
                if ( lstfl) for (j=0;j<nky1;j++) if (iny1 == ky1id[j])   tr.ifflg=0;
                puttr(&tr);
             } while (gettr(&tr)); 
        }
    }
    if (ifmode) {
        if ( (!iky0) && (!iky1) ) {
            typ0[0]=hdtype(key0[0]);
            indx0[0]=getindex(key0[0]);              
            typ1[0]=hdtype(key1[0]);
            indx1[0]=getindex(key1[0]);
            do { 
                tr.ifflg=0;
                gethval(&tr, indx0[0], &val0);
                gethval(&tr, indx1[0], &val1);
                iny0 = (int) vtof(hdtype(key0[0]), val0);
                iny1 = (int) vtof(hdtype(key1[0]), val1); 
                for (i=0; i<nky0; i++) {
                       if (iny0 == ky0id[i] ) {
	              if (!lstfl) {
                                 ntm=(int) (nky1/2);
	                   for (j=0;j<ntm;j++) if ( (iny1 < ky1id[2*j]) && (iny1 >= ky1id[2*j+1]))  tr.ifflg=1;
                           }
                          if ( lstfl) for (j=0;j<nky1;j++) if (iny1 == ky1id[j])   tr.ifflg=1;
                      }
                }
                puttr(&tr);
             } while (gettr(&tr)); 
        }
        if ( (iky0) && (!iky1) ) {            
            typ0[0]=hdtype(key0[0]);
            indx0[0]=getindex(key0[0]);              
            do {   
                tr.ifflg=0;
                gethval(&tr, indx0[0], &val0);
                iny0 = (int) vtof(hdtype(key0[0]), val0);
                for (i=0; i<nky0; i++) if (iny0 == ky0id[i] )  tr.ifflg=1;
                puttr(&tr);
             } while (gettr(&tr)); 
        }
        if ( (!iky0) && (iky1) ) {
           typ1[0]=hdtype(key1[0]);
           indx1[0]=getindex(key1[0]);
           do {        
                tr.ifflg=0;
                gethval(&tr, indx1[0], &val1);
                iny1 = (int) vtof(hdtype(key1[0]), val1); 
                if (!lstfl) {
                    ntm=(int) (nky1/2);
	      for (j=0;j<ntm;j++) if ( (iny1 < ky1id[2*j+1]) && (iny1 >= ky1id[2*j])) tr.ifflg=1;
                }
                if ( lstfl) for (j=0;j<nky1;j++) if (iny1 == ky1id[j])   tr.ifflg=1;
                puttr(&tr);
             } while (gettr(&tr)); 
        }
    }
}

int chkeven(int m0)
{
    int ir=0;
    float r0,r1;

    r0=m0/2;
    r1=floorf(r0);
    if (r1 == r0) ir=1; 
    return (ir);
}
