h32739
s 00010/00011/00092
d D 1.7 88/11/15 14:01:59 shuki 7 6
c 
e
s 00001/00001/00102
d D 1.6 88/05/12 12:51:01 shemer 6 5
c make
e
s 00001/00001/00102
d D 1.5 88/05/12 12:07:27 shemer 5 4
c 
e
s 00001/00001/00102
d D 1.4 88/05/12 11:53:53 shemer 4 3
c 
e
s 00002/00002/00101
d D 1.3 88/05/08 10:24:13 shemer 3 2
c 
e
s 00000/00001/00103
d D 1.2 88/04/28 16:21:35 shemer 2 1
c 
e
s 00104/00000/00000
d D 1.1 88/04/28 09:52:46 shemer 1 0
c date and time created 88/04/28 09:52:46 by shemer
e
u
U
t
T
I 1
D 7

E 7
/* velfile               
 *
 * Technical Reference:
 *
 * Credits:
 *
*/
#include <stdio.h>
#include <math.h>
#include "../include/su.h" 
D 7
 extern bool verbose;
 extern int ncmp;
 extern  char *sdoc;
 extern int xargc;
 extern char **xargv;
 extern  struct v {float *v;float *tv}  *vtv;
 double atof();
extern char vfname[50];
E 7
I 7
extern bool verbose;
extern int ncmp;
extern  char *sdoc;
extern int xargc;
extern char **xargv;
extern  struct v {float *v;float *tv}  *vtv;
double atof();
extern char *vfname;
E 7
extern int *nvmax;
extern float *vxtcmp; 
velfile(nt)
int nt;
{

	static unsigned nv;/* number of v's read by getpar	*/
	static int nmax,iv;
	int j;/* counter over v's and tv's		*/
	char s1[50],s2[50];			/* string buffer */
	FILE *pvelf;	/* for the velocity file */
	bool opened;
		/* Allocate space for nmo operations */
		/* Allocate space for velocity model using maxgetpar() */
D 2
        fprintf(stderr,"nt nt nt nt nt=%d\n",nt);
E 2
		nv = maxgetpar();
		/* Velocity model */
D 7
        if(sgetpar("vfname",vfname)) 
E 7
I 7
        if(sgetpar("vfname",&vfname)) 
E 7
                           {
			pvelf = fopen(vfname,"r");		/* Try to open */
			if(pvelf != NULL)			/* A file ? */
				opened = true;
			 else  				/* Illegal */
                                {
D 3
				warn("can't open");
E 3
I 3
				warn(__FILE__,__LINE__,"can't open");
E 3
				selfdoc();
			        }
			 if (verbose)  

			fprintf(stderr,
         " \tlateraly varying velocity model from vfile=%s\n",

						vfname);
	    	                for(ncmp=0,nmax=0,nv=0;fscanf(pvelf,"%s %s",s1,s2)!=EOF;)
                       {    
				if(*s1=='m')  /* New cmp */
                                 {       
					ncmp++;
					nv = 0;
				 } 
                                   else        
                                 {
                                   nv++;      
				   nmax = MAX(nmax,nv);
			         } 
                        }
D 7
		                 if(1)
E 7
I 7
		                if(verbose)
E 7
				fprintf(stderr,"ncmp=%d	nmax=%d\n",ncmp,nmax);
	
			/* Allocate space for vxt  for all ncmp*/
    vtv = (struct v*)
				malloc((unsigned)(ncmp*sizeof(struct v)));
                       /* Allocate space for every velocity function in cmp */
			        for(j=0;j<ncmp;j++){
	 			vtv[j].v = (float*)
					malloc((unsigned)(nt*sizeof(float)));
	 			vtv[j].tv = (float*)
					malloc((unsigned)(nmax*sizeof(float)));
                                       }
			/* Allocate space for v and tv */
               nvmax=(int *) malloc((unsigned) (ncmp*sizeof(int)));
               vxtcmp=(float *) malloc((unsigned) (ncmp *sizeof(float)));
			/* scan pass 2 to read the vfile and convert it to vxt */
			rewind(pvelf);
			fscanf(pvelf,"%s %s",s1,s2);
			/* fprintf(stderr,"Read %s %s\n",s1,s2); */
			for(j=0;j<ncmp;j++) {		/* Loop over midpoint */
D 3
				if(*s1!='m') err("Bad velfile (%s)",vfname);
E 3
I 3
				if(*s1!='m') err(__FILE__,__LINE__,"Bad velfile (%s)",vfname);
E 3
				vxtcmp[j] = atoi(s2);
			   fprintf(stderr,"\tvxtcmp[%d]\n",j,vxtcmp[j]);   
				for(iv=0;iv<=nmax;iv++) {/* Loop over velocities */ 
					if(fscanf(pvelf,"%s %s",s1,s2)==EOF) break;
					/* fprintf(stderr,"Read %s %s\n",s1,s2); */
					if(*s1=='m') break;
                                         nvmax[j]=iv;
					vtv[j].v[iv] = atof(s2);
				       	vtv[j].tv[iv] = atof(s1);
D 4
					/* fprintf(stderr,"\ttv[%d]=%f\tv[%d]=%f\n",iv,tv[iv],iv,v[iv]); */
E 4
I 4
D 5
				 fprintf(stderr,"\tvtv[%d].v[%d]=%f\tvtv[%d].tv[%d]=%f\n",j,iv,vtv[j].v[iv],j,iv,vtv[j].tv[iv]);
E 5
I 5
D 6
				 fprintf(stderr,"\ttv=%f\tv=%f\n",j,iv,vtv[j].tv[iv],j,iv,vtv[j].v[iv]);
E 6
I 6
				 fprintf(stderr,"\ttv=%f\tv=%f\n",vtv[j].tv[iv],vtv[j].v[iv]);
E 6
E 5
E 4
				}
	
                    }
			fclose(pvelf);
                 }
                 return(1);  
               }
E 1
