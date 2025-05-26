/* Program to launch gather oriented processing flows with SU */
/* Balazs Nemeth, May-26-2000 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = { " SUPR_G - Gather oriented processing ",
"                       				",
"  supr_g < infile c='' > outfile [optional param.]	",
"							",
" Required parameters:					",
"							",
" c=''		seismic unix command to excecute on the	",
"		gathers.                 		",
"		        				",
" Optional parameters:					",
"							",
" key=ep 	segy keyword that defines the gather	",
"		If this word changes the next few	",
"		traces with the same keyword will be	",
"		treated as the next gather.		",
"		Consequently the input has to be sorted	",
"		according to the key.			",
"							",
"  This program reads n traces with the same keyword	",
"  specified by key, than forks the program given in c	",
"  as the argument. The gather is than piped through	",
"  the forked program.					",
"							",
"  Example: 					        ",
"  supr_g < infile key=ep c='sugain pbal=1 panel=1' > outfile",
"  does a shot gather oriented scaling.	                ",
"  Infile has to be sorted according to ep.		",
"							",
"  Example: For 3D records, put the receiver line # into",
"  headerword cdpt. Than to do a fk spectra for each 	",
"  receiver line gather:				",
"  supr_g < infile key=cdpt c='suspecfk' > outfile	",
"							",
"							",
NULL};

segy **get_gather(cwp_String *key,cwp_String *type,Value *n_val, int *nt,int *ntr,
		      float *dt,int *first);
segy **put_gather(segy **rec,int *nt, int *ntr);
segy tr;
int verbose;
int main( int argc, char *argv[] )
{
	cwp_String key;		/* header key word from segy.h		*/
	cwp_String type;	/* ... its type				*/
	Value val;
	segy **rec_o;		/* trace header+data matrix */
	int first=0;		/* true when we passed the first gather */
	int ng=0;
	float dt;
	int nt;
	int ntr;
	int verbose=0;
	
	/* Command execution */
	FILE *pf;		/* pipe */
	cwp_String CMD_BUFF;	/* command buffer */
	
	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);
	
	if (!getparstring("key", &key))	key = "ep";
	if (!getparint("verbose", &verbose)) verbose=0;
	MUSTGETPARSTRING("c", &CMD_BUFF);


	if (verbose) fprintf(stderr," %s\n",CMD_BUFF);
	/* get the first record */
	rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	if(ntr==0) err("Can't get first record\n");
	do {
		if (( pf = popen(CMD_BUFF,"w")) == NULL) {
			perror("popen");
			exit(1);
		}
			
		{ register int itr;
			
			/* write the traces to the pipe */
			for(itr=0;itr<ntr;itr++) {
				fwrite(rec_o[itr],(size_t) nt*FSIZE+HDRBYTES,1,pf);
				free((void*) rec_o[itr]);
			}
		}
		/* close the pipe wait for child process to exit */
		pclose(pf);
		
		ng++;
		rec_o = get_gather(&key,&type,&val,&nt,&ntr,&dt,&first);
	} while(ntr);

	if(verbose) warn("Number of gathers %10d\n",ng);
	 
	return EXIT_SUCCESS;
}

segy **get_gather(cwp_String *key,cwp_String *type,Value *n_val, int *nt,int *ntr,
		      float *dt,int *first)
{
	int nsegy;
	FILE *tracefp;
	FILE *headerfp;
	segy **rec=NULL;
	int ntrr=0;
	int indx=0;
	static Value val;
	
	
	*type = hdtype(*key);
	indx = getindex(*key);
        *ntr = 0;

	if(*first==0) {

		/* Take a trace count */
        	/* get info from first trace */
        	nsegy = gettr(&tr);
		if (nsegy==0)  err("can't get first trace");
        	ntrr++;
		*nt = tr.ns;
        	*dt = (float) tr.dt/1000000.0;
		gethval(&tr, indx, n_val);
		*first=1;
	}

        /* Store traces in tmpfile while getting a count */
	tracefp = etmpfile();
	headerfp = etmpfile();
	if (verbose) warn("using tmpfile() call");
        do {
                efwrite(&tr, 1, HDRBYTES, headerfp);
                efwrite(tr.data, FSIZE, *nt, tracefp);
		/* read the next trace */
		*ntr+=1;
		val=*n_val;
		nsegy = gettr(&tr);
		if(nsegy) ntrr++;
		gethval(&tr, indx, n_val);
	} while (nsegy && !valcmp(*type,val,*n_val));

	/* No more data */
	if(nsegy==0 && ntrr==0 ) {
		 *ntr=0;
		 return(rec=NULL);
	}
        
	if(rec!=NULL) free(rec);
	
	/* allocate memory for the record */
	{ register int i;
		rec = ealloc1(*ntr,sizeof(segy *));
		for(i=0;i<*ntr;i++)
			rec[i] = (segy *)ealloc1((*nt*FSIZE+HDRBYTES),sizeof(char));
	}
	
	
	/* load traces into an array and close temp file */
	erewind(headerfp);
        erewind(tracefp);
	{ register int ix;
        	for (ix=0; ix<*ntr; ix++)
                	fread (rec[ix],HDRBYTES, 1, headerfp);
        	efclose (headerfp);
		for(ix=0; ix<*ntr; ix++)
                	fread ((*rec[ix]).data,FSIZE, *nt, tracefp);
        	efclose (tracefp);
	}
	
	return(rec);	
}

segy **put_gather(segy **rec,int *nt, int *ntr)
{


	segy tr;
	
	{ register int i;
		for(i=0;i<*ntr;i++) {
			memcpy( (void *) &tr, (const void *) rec[i],
					*nt*FSIZE+HDRBYTES);
			puttr(&tr);
			free1((void *)rec[i]);
		}
	}
	return(rec=NULL);
}
