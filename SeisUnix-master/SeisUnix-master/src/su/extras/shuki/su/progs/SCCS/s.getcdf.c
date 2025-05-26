h46348
s 00001/00001/00096
d D 1.2 88/11/15 14:03:35 shuki 2 1
c 
e
s 00097/00000/00000
d D 1.1 88/11/06 09:54:05 shuki 1 0
c date and time created 88/11/06 09:54:05 by shuki
e
u
U
t
T
I 1
#include <stdio.h>
D 2
#include "/src/su/include/su.h"
E 2
I 2
#include "../include/su.h"
E 2
#define BLOCK 1000000
#define BLOCK1 1000
extern bool verbose;
extern int nalloc,dalloc,nalloc1,dalloc1;
extern float *x;
getcdf(infd,acoff,cmp,all)
Section *acoff;
int infd,all;
  float cmp;
{
	int nx,nread=0,rgettr;
        float cmp0,flag;
	static nt4;
	static bool first=true;
	static Sutrace tr;

	if(first) {

		tr.data = (float*)malloc(acoff->n1*sizeof(float));

		/* READ THE FIRST TRACE */
		nread = gettr(infd,&tr);
		if(!nread) err(__FILE__,__LINE__,"getco: can't get first trace\n");

		nt4 = acoff->n1*sizeof(float);

		/* (FIRST) MEMORY ALLOCATION */
                nalloc1 = BLOCK1;
                dalloc1 = nalloc1;
                x= (float*) malloc(nalloc1);
		nalloc = MAX(BLOCK,nt4);
		dalloc = nalloc;
		acoff->data = (float*) malloc(nalloc);
		if(acoff->data==NULL) err(__FILE__,__LINE__,"Can't malloc %d bytes",nalloc);
	}

	cmp0 = tr.cdp;
	acoff->o2 = cmp0;
         if(all==1) cmp=cmp0;
         flag=tr.cdp-cmp;


	/* READ THE REST OF THE CDF (+ MAYBE FIRST TRACE OF NEXT ONE) */
         while(flag) {
        gettr(infd,&tr);  
         flag=tr.cdp-cmp;}
		bcopy(tr.data,acoff->data,nt4);
          x[0]=tr.offset;
      fprintf(stderr," New cdf=%f Current cdf=%f  x=%f\n",cmp,cmp0,x[0]);
	
	for(nx=1;(rgettr=gettr(infd,&tr))>0;nx++) {
                  cmp0=tr.cdp;
			x[nx] = tr.offset;
      if(cmp==cmp0) {

      fprintf(stderr," New cdf=%f Current cdf=%f  x=%f\n",cmp,cmp0,x[nx]);
                    }
		/* IS IT A NEW OFFSET */
		if( cmp0 != cmp) {
			break;
		}

		nread += rgettr;



		/* MEMORY REALLOCATION */
		if( (nx+1) >= nalloc1) {
			nalloc1 += dalloc1;
			x = (float*) realloc(x,nalloc1);
			if(x==NULL)
				err(__FILE__,__LINE__,"Can't realloc %d bytes",nalloc1);
		}
		if( (nx+1)*nt4 >= nalloc) {
			nalloc += dalloc;
			acoff->data = (float*) realloc(acoff->data,nalloc);
			if(acoff->data==NULL)
				err(__FILE__,__LINE__,"Can't realloc %d bytes",nalloc);
		}



		bcopy(tr.data,acoff->data+nx*acoff->n1,nt4);
/*           if(cmp0 !=cmp && nx) return(nread) */

	}

	first = false;
	acoff->n2 = nx;

         fprintf(stderr,"-------------------------------------------\n");
        if(!nread)fprintf(stderr,"finish no more traces\n");
           return(nread);

}
E 1
