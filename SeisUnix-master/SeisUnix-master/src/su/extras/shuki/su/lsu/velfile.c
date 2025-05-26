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
extern bool verbose;
extern int ncmp;
extern  char *sdoc;
extern int xargc;
extern char **xargv;
extern  struct v {float *v;float *tv}  *vtv;
double atof();
extern char *vfname;
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
		nv = maxgetpar();
		/* Velocity model */
        if(sgetpar("vfname",&vfname)) 
                           {
			pvelf = fopen(vfname,"r");		/* Try to open */
			if(pvelf != NULL)			/* A file ? */
				opened = true;
			 else  				/* Illegal */
                                {
				warn(__FILE__,__LINE__,"can't open");
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
		                if(verbose)
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
				if(*s1!='m') err(__FILE__,__LINE__,"Bad velfile (%s)",vfname);
				vxtcmp[j] = atoi(s2);
			   fprintf(stderr,"\tvxtcmp[%d]\n",j,vxtcmp[j]);   
				for(iv=0;iv<=nmax;iv++) {/* Loop over velocities */ 
					if(fscanf(pvelf,"%s %s",s1,s2)==EOF) break;
					/* fprintf(stderr,"Read %s %s\n",s1,s2); */
					if(*s1=='m') break;
                                         nvmax[j]=iv;
					vtv[j].v[iv] = atof(s2);
				       	vtv[j].tv[iv] = atof(s1);
				 fprintf(stderr,"\ttv=%f\tv=%f\n",vtv[j].tv[iv],vtv[j].v[iv]);
				}
	
                    }
			fclose(pvelf);
                 }
                 return(1);  
               }
