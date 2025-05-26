#include <string.h>
#include <cwp.h>
#include <segy.h>
#include <su.h>

char * sdoc = 
"sumerge - merge segys data file	\n"
"\n"
"USAGE:\n"
"sumerge file1 file2 file3 ... > newfile\n"
"Or \n"
"sumerge datain1=file1 datain2=file2 datain3=file3 ... dataout=newfile\n"
"Or \n"
"sumerge datain=file1 datain=file2 datain=file3 ... skipfile=... >newfile\n"
"\n"
"REQUIRED PARAMETERS \n"
"\n"
"OPTIONAL PARAMETERS \n"
"\trm=0\t =1 will remove input segy file after merging \n"
"\theader=1\t =0 don't add the header to the output file \n"
"\tskipfile= name of the file to indicate how many traces \n"
"\t          to skip before output. The number of traces \n"
"\t          to skip is writen starting at the scolumn on \n"
"\t          the last line of the skipfile, as explained blow. \n"
"\t          (when skipfile is specified, input data must be given \n"
"\t          as the format of datain=file1 datain=file2 ...) \n"
"\tscolumn=29 starting colume position of the last line of the \n"
"\t          skipfile to indicate number of traces to skip \n"
"\ton2trace=0 output total number of traces in input to the key \n"
"\t          word tracl (first 4 bytes) in the first output trace \n" 
"\t          0=no 1=yes \n"
"\t          (when on2trace=1, input data must be given \n"
"\t          as the format of datain=file1 datain=file2 ...) \n"
"AUTHOR:		J.C. Dulac	9/17/91	\n"
"MODIFICATIONS:\n"
"     added datain1,... arguments	J.C. Dulac	6/01/93	\n"
"     added skipfile options            Z. Li 12/24/01 \n"
"\n"
;

void copy(char *in,FILE *outfp,int header,int rm) ;

segytrace tr;
segybhdr bh;
segychdr ch;


main(int argc,char *argv[]) {
     int rm, header, i;
     char *dataout;
     FILE *outfp = stdout, *infp;
     char *skipfile;
     char *datain;
     char *cbuf = (char *) NULL;
     int ntraceskip, ndata;
     int scolumn, on2trace, ntotal;
     int *nitrace, mtrace, ifirsttrace=0;
     int icount=0, itrace;

     long long i64;

     int nsegy;

     initargs(argc,argv) ;
     askdoc(1);

     if( !getparint("rm",&rm) ) rm = 0 ;

     if( !getparint("header",&header) ) header = 1 ;
     if(!getparint("on2trace",&on2trace)) on2trace=0;
     if (on2trace==1) icount=1;


     if( getparstring("dataout",&dataout) ) outfp = fopen(dataout,"w") ;

/* find out the skip trace number */
     if (getparstring("skipfile",&skipfile)) {
		icount = 1;
		if(!getparint("scolumn",&scolumn)) scolumn=29;
		if(scolumn>80) err("scolumn must be less than 80");
		if( (infp = fopen(skipfile,"r"))!=NULL ) {

			cbuf = (char*) malloc(80*sizeof(char));
			for(i=0;i<2000000000;i++) {
			      if(feof(infp)!=0) break;
			      bzero(cbuf,80);
			      fgets(cbuf,80,infp);
			      sscanf(cbuf+scolumn-1,"%d",&ntraceskip);
		      	}
		      	fprintf(stderr," number of traces to skip = %d \n",
				ntraceskip);

		      	fclose(infp);
		      	free(cbuf);
  		} else {
  		ntraceskip = 0;
		fprintf(stderr," skipfile not present: set ntraceskip=0 \n");
	   	fprintf(stderr," number of traces to skip = %d \n",ntraceskip);
     		}
     } else {
	        ntraceskip = 0;
     }
/* find out total number of traces for each input */
     if(icount==1) {
	ndata = 0;
	ndata = countparname("datain");
	if(ndata==0) err(" datain missing \n");
	nitrace = (int*) malloc(ndata*sizeof(int));
	ntotal = 0;
	fprintf (stderr," total of %d input datasets \n",ndata);

	for( i=0; i < ndata; i++ ) {
		getnparstring(i+1,"datain",&datain);
		infp = fopen(datain, "r");
		nitrace[i] = 0;
		fseek64(infp,0,1);
		i64 = 0;
		fseek64(infp,3600,0);
		if (!fgettr(infp,&tr))  err("can't get first trace");
		nsegy  = tr.ns * sizeof(float) + 240;
		fseek64(infp,i64,SEEK_END);
		i64 = ftell64(infp);
		fclose(infp);
		i64 = (i64 - 3600);
		i64 = i64 / nsegy;
		itrace = i64;
		nitrace[i] = itrace;
		fprintf(stderr," input %d: %s has %d traces \n",
			i+1,datain,itrace);
		ntotal = ntotal + itrace;
        }
	fprintf (stderr," total input %d traces \n",ntotal);
     }

/* ouput */
     fseek64(outfp, 0, SEEK_SET ); 

     if(icount==0) {
     	for( i=1; i < argc; i++ ) {
        	char * eq ;
		if( (eq=strchr(argv[i],'=')) == 0 ) {
            		copy(argv[i],outfp,header,rm) ;
		} else if( strncmp(argv[i],"datain",6) == 0 ) {
            		copy(eq+1,outfp,header,rm) ;
        	}
      	}
     } else {
	mtrace = 0;
	for ( i=0; i < ndata; i++) {
		if(mtrace+nitrace[i]>ntraceskip) {
			itrace = ntraceskip - mtrace;
			if ( itrace < 0 ) itrace = 0;
			getnparstring(i+1,"datain",&datain);
			infp = fopen(datain, "r");
			file2g(infp);
			if(ifirsttrace==0) fgethdr(infp,&ch,&bh);

			fseek64(infp,3600,0);
			if (!fgettr(infp,&tr))  err("can't get first trace");
			nsegy  = tr.ns * sizeof(float) + 240;
			i64 = itrace;
			i64 = i64*nsegy + 3600;
			fseek64(infp,i64,0);
			fgettr(infp,&tr);

			if(ifirsttrace==0) {
				fputhdr(outfp,&ch,&bh);
				if(on2trace==1) tr.tracl = ntotal;
			fprintf(stderr," first output at trace %d of %s \n",
				itrace+1,datain);
				ifirsttrace = 1;
			}
			do {
				fputtr(outfp,&tr);
			} while (fgettr(infp,&tr));

			fclose(infp);
		}
		mtrace = mtrace + nitrace[i];
	}
     }
}

void copy(char *in,FILE *outfp,int header,int rm) {
     segybhdr bhdr ;
     segychdr chdr;
     segy tra ;
	 int ierr;

     static int first = 1 ;
     FILE * infp ;
     if( (infp=fopen(in,"r")) ) {
         fseek64( infp, 0, SEEK_SET );
         fgethdr(infp,&chdr,&bhdr) ;
	 	 if( first && header ) fputhdr(outfp,&chdr,&bhdr) ;
         first = 0 ;
	 	 ierr = fgettr(infp,&tra);
	 	 if(ierr==0) err(" no trace input for %s ",in);
	 	 do {
	 		fputtr(outfp,&tra) ;
	 	 } while( fgettr(infp,&tra) );
	 	 fclose(infp) ;
	  	 if( rm ) unlink(in) ;
     }
}
