/*  read wgc code-4 data from FTPed dataset, convert and write to segy output */
/*  (each record is ftped without 4-byte control word) */

#include "su.h"
#include "segy.h"
#include "header.h"
#include "wgc.h"
#include "wgc4.h"


char *sdoc = 
"WGC2SGY - wgc ibm code-4 to segy ieee conversion  	\n"
"\n"
"wgc2sgy [parameters] <input >output			\n" 
"\n"
"Required parameters:							\n"
"None \n"
"\n"
"Optional parameters:							\n"
"n2=all			number of traces to convert 			 \n"
"o2=1			first trace to convert 				 \n"
"printfile=null		file name to print id header and other infomation \n" 
"\n"
"AUTHOR:		Zhiming Li 	8/14/92   \n"		    
;

segytrace tr;
wgc4trace wr;
segybhdr bh;
segychdr ch;

main(int argc, char **argv)
{
    int nlhd,n1,n2,mlhd;
    int i1,i2,nskip, ncpl, itype, lin, lout, lrec, len ;
    int ltrhd, lidhd, iprint, itmp;
    int o2, tmfs, i, sort;
    int residual=0;
    short recordsize, ssindx;
    float *outbuf,tmp;
    char *cbuf , *inbuf, dmy;
    char *idhd, *printfile, c;
    char max_ref_time[5], sampling_interval[3];
    int mtime, ibypsm, isamp, ic;
    FILE *infp=stdin,*outfp=stdout, *printfp;

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);


    if(!getparint("o2",&o2)) o2 = 1 ; 
    o2 = o2 - 1;
    iprint = 1;
    if(!getparstring("printfile",&printfile)) iprint = 0 ;
    if(iprint == 1 ) 
    {
       if((printfp=fopen(printfile,"w"))==NULL)
       err("error opening printfile = %s \n");
    }

/* maximum number of lines of id header */
    nlhd=500; 

    /* allocate space */
    ncpl = 80;
    cbuf = (char*)malloc(ncpl*sizeof(char));

/* if id header needed, uncommented the following line */
    /* idhd = (char*)malloc(nlhd*ncpl*sizeof(char)); */

/* read in id header */
    sort = 1;
    for (mlhd=0;mlhd<nlhd;mlhd++) {
	  /* read in a record of 80 bytes into cbuf */
	  fread(cbuf,sizeof(char),ncpl,infp);

	  /* convert to ascii text using subroutine tascii_ */
 	  tascii_((unsigned char*)cbuf,(unsigned char*)cbuf,ncpl,0);

	  /* store id header, uncommented the following if needed */
          /*  for (i1=0;i1<ncpl;i1++) idhd[mlhd*80+i1] = cbuf[i1];   */

          /* store id header into printfile if needed */
	  if(iprint==1) fprintf(printfp,"%s \n",cbuf);

	  /* check max ref time */
	  if( (strncmp(&cbuf[2],"MAX REF TIME(MS)",16)==0 ) ||
	      (strncmp(&cbuf[2],"TRACE LENGTH MAX",16)==0 ) )
	     { 
	     strncpy(max_ref_time,&cbuf[19],5);
	     mtime = atoi(max_ref_time);
	     }

	  /* check sampling interval */
	  if( (strncmp(&cbuf[26],"SAMPLING INTERVAL",17)==0 ) ) 
	     { 
	     strncpy(sampling_interval,&cbuf[43],3);
	     isamp = atoi(sampling_interval);
	     }

	  /* check data type */
	  if( (strncmp(&cbuf[53],"CDF ORDERED",11)==0) &&
	      (strncmp(&cbuf[2],"DATA TRACE NO.(MAX)",19)==0) ) sort = 2; 
	  if( (strncmp(&cbuf[53],"SHOT GATHER",11)==0) &&
	      (strncmp(&cbuf[2],"DATA TRACE NO.(MAX)",19)==0) ) sort = 1; 
	  if( (strncmp(&cbuf[53],"STACK",5)==0) &&
	      (strncmp(&cbuf[2],"DATA TRACE NO.(MAX)",19)==0) ) sort = 4; 

	  /* check to see if last card in id header is reached */
	  if(cbuf[0]=='C' && cbuf[2]=='E' && cbuf[3]=='O' && cbuf[4]=='F')
	  break;
    }

    /* determine number of c cards in id header */
    if ( mlhd < nlhd ) mlhd = mlhd + 1;
    if(iprint==1) 
    fprintf(printfp,"number of lines in the ID header =  %d \n",mlhd);
    lidhd = mlhd*80;

    /* determine trace length, sample start index and data type     */
    lout = mtime/isamp;
    nskip = 2; 	
    fseek(infp,nskip,1);
    fread(&ssindx,sizeof(short),1,infp);
    if ( ssindx == 26 || ssindx == 101 ) itype = 0;
    if ( ssindx == 51 || ssindx == 201 ) itype = 1;
    ibypsm = 4; if ( itype ==  1 ) ibypsm = 2;

    if (itype==1) err("Input data must be floating-point type");
    if (ssindx !=101) err("Input trace header not extended format");

    lin = (lout+ssindx-1)*ibypsm; 
    if(iprint==1) fprintf(printfp,"input trace length in bytes = %d \n",lin);

    /* cread id headers for output */
    idhdrs(&ch,&bh,lout);
    fputhdr(outfp,&ch,&bh);

    /* allocate input and output buffers */
    inbuf = (char*)malloc(lin*sizeof(char));
    outbuf = (float*)malloc(lout*sizeof(float));

    ltrhd = (ssindx-1)*4;

    if(iprint==1) 
    fprintf(printfp,"trace headher length in bytes = %d \n", ltrhd);
    if(iprint==1) fprintf(printfp,"output samples per trace = %d \n",lout);

    /* trace header buffer allocation */
    ltrhd = ltrhd/2;

    /* determine number of total traces, if not specified */
    fseek(infp,0L,0);
    if(!getparint("n2",&n2)) 
       {
       fseek(infp,0L,2);
       n2 = (ftell(infp) - mlhd*80)/lin;
       }
    if ( iprint==1) fprintf(printfp,"number of traces to convert = %d \n", n2); 

    /* skip id header */ 
    nskip = mlhd * 80; 
    fseek(infp,nskip+o2*lin,0); 
 
    /* read in traces */
    for (i2=0;i2<n2;i2++) {
	  /* if end of file, exit */ 
	  if (feof(infp) != 0 ) break;
	  /* read in a code-4 trace */
	  fread(inbuf,sizeof(char),lin,infp);
	  bcopy(inbuf,&wr,lin);
	  /* convert */
	  wgc2sgy(&wr,&tr,sort,isamp*1000,lout,i2+1,i2+1);
	  /* output */ 
	  fputtr(stdout,&tr);
    }	
  /* close printfile if needed */
  if ( iprint == 1 ) fclose(printfp);

}
 
