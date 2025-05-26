char *sdoc =
"SUCOMPRESS - wavelet compression of SU datasets in multiple parts\n"
"\n"
"sucompress <file.su >file.comp [options]\n"
"\n"
"Parameters:\n"
"compress=0        compression ratio\n"
"mse=0             alternative compression specification- mean \n"
"                  square error limit\n"
"key=              SEGY key that determines chunks, e.g. cdp\n"
"start=  end=      compress this sequential ranges of groups (chunks) \n"
"                  default start=0 end=2000000000 \n"
"min=    max=      compress this range of values of the key \n"
"                  default min=-2000000000 max=2000000000 \n"
"head=1            prepend SEGY file header to compressed file\n"
"verbose=0         report key value, size, and compression speed\n"
"                  if > 1, reports every verbose# gathers\n"
"headerupdate=1    after compression, store compression information \n"
"                  in the segy C40 card location (bytes 21-80). \n"
"                  This should speed up the decompression process. \n"
"                   1=yes 0=no \n"
"Example: \n"
"  sucompress < input.su compress=10 key=cdp > output.su.comp \n"
"              \n" 
"\nNotes:\n"
"1) Sucompress puts a SU wrapper arounds the AWARE compression routines.\n"
"   This routine compresses a 2-D chunk of data at a time.\n"
"   The chunk should be somewhat laterally coherent.\n"
"2) Experiment with different size chunks for speed.  Typical compression\n"
"   rates are 4MB to 8MB per second. The verbose option reports speed.\n"
"   Generally the more coherent the data chunk, the faster the compression.\n"
"3) MSE parameter value depends upon data amplitudes.\n"
"\n";
#include <su.h>
#include <segy.h>
#include <hdr.h>
#include <seilib.h>
#include "sucomp.h"

comp_header ch;
segybhdr bhdr;
segychdr chdr;

#define MAXTRACE 10000
#if 0
void dmphdr( char* tp){

   int i,j,k;

   char p;

   k=0;

   for( i=0; i<12; i++ ){
      for( j=0; j<20; j++ ){
         p = tp[k++];
         fprintf( stderr ,"%d " ,p );
      }
      fprintf( stderr ,"\n" );
   }

}
#endif

main (int argc , char **argv) {
int verbose=0, ioff, group1=0, group2=2000000000, igroup, head=1;
long long nbyte;
int alltrace=0, new=0, member=0, ngroup=0, min=-2000000000, max=2000000000;
float *data=0, time0=0, alltime=0, timex, lambda=LAMBDA, compress=0, mse=0;
char *key;
char *cbuf = (char *) NULL;
FILE *outfd=stdout, *infp=stdin;
segy trace;
Value value;
float tmp;
int itmp;

char *c80;
int headerupdate=1;
int maxbyte=0,maxtrace=0;
float iicomp,iimse;

/* SU initializations */
initargs (argc,argv);
askdoc (1);

strncpy( ch.id ,SUCOMPID, strlen(SUCOMPID) );

file2g(infp);
file2g(outfd);

/* fetch parameters */

getparfloat ("ratio",&compress);
getparfloat ("compress",&compress);
getparfloat ("mse",&mse);
if (compress == 0 && mse == 0) err ("compress and mse factors both zero");
if (getparstring ("key",&key) == 0) err ("key= missing");
strcpy (ch.key,key);
hdtype (key);
getparint ("group1",&group1);
getparint ("start",&group1);
getparint ("group2",&group2);
getparint ("end",&group2);
getparint ("min",&min);
getparint ("max",&max);
getparint ("head",&head);
getparint ("verbose",&verbose);
getparint ("headerupdate",&headerupdate);

fprintf (stderr, " compress=%g mse=%g \n",compress,mse);
fprintf (stderr, " key=%s start=%d end=%d min=%d max=%d head=%d\n", 
	key, group1, group2, min, max, head);
fprintf (stderr, " headerupdate=%d \n",headerupdate);
fprintf (stderr, " DATA_FORMAT=%d \n",DATA_FORMAT); 
fprintf (stderr, "compress_header size=%d\n" ,sizeof(ch) );

/* file header */
if (head) {
	gethdr (&chdr,&bhdr);
	fwrite (&chdr,1,sizeof(chdr),outfd);
        if( LINUX == 1 ){
           char bhdr_tmp[400];
           bhconvert( &bhdr ,bhdr_tmp );
           fwrite(bhdr_tmp, 1, sizeof(bhdr), outfd);
        }else{
           fwrite(&bhdr, 1, sizeof(bhdr), outfd);
        }
}

/* global initializations */
nbyte = sizeof(ch);
lambda = LAMBDA;
alltime = 0.0;
alltrace = 0;
c80 = (char*) malloc(80*sizeof(char));

/* read all traces */
for (igroup=0; gettr(&trace) > 0 && igroup < group2;) {
	/* initialize from first trace */
	if (!data) {
		ch.ns = trace.ns;
		data = (float*) calloc (4,(ch.ns+60)*MAXTRACE);
		gethdval (&trace,key,&ch.value);
		}
	/* detect new group */
	new = 0;
	member = 0;
	gethdval (&trace,key,&value);
	if (valcmp (hdtype(key),value,ch.value)) new = 1;
	if (vtoi(hdtype(key),value) >= min && 
		vtoi(hdtype(key),value) <= max) member = 1;
	if (new) {
		if (igroup >= group1 && igroup <= group2 && member == 1) {

			time0 = cputime();

			cbuf = (char *) NULL;

			if(ch.ntrace>=18) {
				seismic_compress ((char*)data,(ch.ns*4+240)*ch.ntrace,
					&ch.ntrace,&ch.ns,DATA_FORMAT,0,240,ENDIAN,
					compress,mse,&lambda,&cbuf,&ch.nbyte,
					&ch.compress,0,0,0,0,0);
               			if( LINUX == 1 ){
                  			comp_header ch_tmp;
                  			cmhconvert( &ch ,&ch_tmp );
	          			fwrite(&ch_tmp, 1, sizeof(ch_tmp), outfd);
	          			fwrite(cbuf, 1, ch.nbyte, outfd);
                  
               			}else{
	          			fwrite(&ch, 1, sizeof(ch), outfd);
	          			fwrite(cbuf, 1, ch.nbyte, outfd);
               			}
			} else {
				ch.nbyte = (ch.ns*4+240)*ch.ntrace;
				ch.compress = 1.0;
               			if( LINUX == 1 ){
                  			comp_header ch_tmp;
                  			cmhconvert( &ch ,&ch_tmp );
	          			fwrite(&ch_tmp, 1, sizeof(ch_tmp), outfd);
	          			fwrite(data, 1, (ch.ns * 4 + 240) * ch.ntrace, outfd);
                  
               			}else{
	          			fwrite(&ch, 1, sizeof(ch), outfd);
	          			fwrite(data, 1, (ch.ns * 4 + 240) * ch.ntrace, outfd);
               			}
			}
			if (verbose && ngroup%verbose == 0) 
			fprintf (stderr, "%d %s=%d ntrace=%d nbyte=%d compress=%g lambda=%g sec=%g rate=%g\n",
			ngroup+1, key, vtoi(hdtype(key),ch.value), ch.ntrace, ch.nbyte, 
			ch.compress, lambda, timex=cputime()-time0, (ch.ns*ch.ntrace)*.000004/timex);
			else timex = cputime() - time0;
			alltime += timex;
			alltrace += ch.ntrace;

			if(cbuf != (char *) NULL ) free (cbuf);

			iicomp = iicomp + ch.compress;
			iimse = iimse + mse;
			if(maxbyte<ch.nbyte) maxbyte = ch.nbyte;
			if(maxtrace<ch.ntrace) maxtrace = ch.ntrace;

			nbyte += ch.nbyte;
			ngroup++;
			ch.ntrace = 0;

			}
		ch.value = value;
		igroup++;
		}
        if( LINUX == 1 ){
                header_flop( (short*)&trace);
        }
	bcopy ((char*)&trace,(char*)(data+ch.ntrace*(ch.ns+60)),ch.ns*4+240);
	ch.ntrace++;
	}
/* last group */
if (igroup >= group1 && igroup <= group2 && member == 1 && ch.ntrace > 0) {

	time0 = cputime();
	cbuf = (char *) NULL;

	if(ch.ntrace>=18) {
		seismic_compress ((char*)data,(ch.ns*4+240)*ch.ntrace,
			&ch.ntrace,&ch.ns,DATA_FORMAT,0,240,ENDIAN,
			compress,mse,&lambda,&cbuf,&ch.nbyte,
			&ch.compress,0,0,0,0,0);
         		if( LINUX == 1 ){
            			comp_header ch_tmp;
            			cmhconvert( &ch ,&ch_tmp );
            			fwrite(&ch_tmp, 1, sizeof(ch_tmp), outfd);
	    			fwrite(cbuf, 1, ch.nbyte, outfd);
                  
         		}else{
	    			fwrite(&ch, 1, sizeof(ch), outfd);
	    			fwrite(cbuf, 1, ch.nbyte, outfd);
         		}
	} else {
		ch.nbyte = (ch.ns*4+240)*ch.ntrace;
		ch.compress = 1.0;
         	if( LINUX == 1 ){
        	    	comp_header ch_tmp;
            		cmhconvert( &ch ,&ch_tmp );
	    		fwrite(&ch_tmp, 1, sizeof(ch_tmp), outfd);
	    		fwrite(data, 1, (ch.ns * 4 + 240) * ch.ntrace, outfd);
            
         	}else{
	    		fwrite(&ch, 1, sizeof(ch), outfd);
	    		fwrite(data, 1, (ch.ns * 4 + 240) * ch.ntrace, outfd);
         	}
	}
	iicomp = iicomp + ch.compress;
	iimse = iimse + mse;
	if(maxbyte<ch.nbyte) maxbyte = ch.nbyte;
	if(maxtrace<ch.ntrace) maxtrace = ch.ntrace;

	if (verbose && ngroup%verbose == 0) 
fprintf (stderr, "%d %s=%d ntrace=%d nbyte=%d compress=%g lambda=%g sec=%g rate=%g\n",
	ngroup+1, key, vtoi(hdtype(key),ch.value), ch.ntrace, ch.nbyte, ch.compress, 
	lambda, timex=cputime()-time0, (ch.ns*ch.ntrace)*.000004/timex);
	else timex = cputime() - time0;
	alltime += timex;
	alltrace += ch.ntrace;

	if(cbuf != (char *) NULL ) free (cbuf);

	nbyte += ch.nbyte;
	ngroup++;
	ch.ntrace = 0;
	}
/* updating the compression id header */
if(headerupdate==1) {
	fseeko(outfd,80*39,0);
	iicomp = iicomp / ngroup;
	iimse = iimse / ngroup;

	sprintf(c80,"C40 END EBCDIC      %s %d %d %d %d %8.3f %8.3f",
	key,ngroup,alltrace,maxbyte,maxtrace,iicomp,iimse);
	sprintf(c80+79,"\n");

	fseeko(outfd,80*39,0);
	fwrite(c80,80,sizeof(char),outfd);
}

/* final message */
tmp = alltrace;
tmp = tmp*ch.ns/250000.;
itmp = tmp;
if(alltime>0.0) tmp = tmp / alltime;

if (alltime > 0.0) 
fprintf (stderr, "%d %s's and %d megabytes compressed at %g megabytes/sec\n",
ngroup, key, itmp, tmp);

return 0;

}
