/*  read wgc code-4 data from FTPed dataset, convert and write to output */
/*  (each record is ftped without 4-byte control word) */

#include "par.h"
#include "wgc.h"


char *sdoc = 
"C4TOFP - wgc code-4 to ieee floating-point conversion  	\n"
"\n"
"c4tofp [parameters] <input >output			\n" 
"\n"
"Required parameters:							\n"
"None \n"
"\n"
"Optional parameters:							\n"
"n2=all			number of traces to convert 			 \n"
"o2=1			first trace to convert 				 \n"
"printfile=null		file name to print id header and other infomation \n" 
"\n"
"AUTHOR:		Zhiming Li 	5/14/91   \n"		    
;

main(int argc, char **argv)
{
    int nlhd,n1,n2,mlhd;
    int i1,i2,nskip, ncpl, itype, lin, lout, lrec, len ;
    int ltrhd, lidhd, iprint, itmp;
    int o2, tmfs, i;
    int residual=0;
    short recordsize, ssindx;
    float *outbuf,tmp;
    char *cbuf , *inbuf, dmy;
    char *idhd, *printfile, c;
    char max_ref_time[5], sampling_interval[3];
    int mtime, ibypsm, isamp, ic;
    short *trhd;
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
    for (mlhd=0;mlhd<nlhd;mlhd++)
	  {
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
    lin = (lout+ssindx-1)*ibypsm; 
    if(iprint==1) fprintf(printfp,"input trace length in bytes = %d \n",lin);
    /* output floating-point trace length (without header) */

    /* allocate input and output buffers */
    inbuf = (char*)malloc(lin*sizeof(char));
    outbuf = (float*)malloc(lout*sizeof(float));

    if ( itype == 0 ) ltrhd = (ssindx-1)*4;
    if ( itype == 1 ) ltrhd = (ssindx-1)*2;

    if(iprint==1) 
    fprintf(printfp,"trace headher length in bytes = %d \n", ltrhd);
    if(iprint==1) fprintf(printfp,"output samples per trace = %d \n",lout);

    /* trace header buffer allocation */
    ltrhd = ltrhd/2;
    trhd = (short*)malloc(ltrhd*sizeof(short)); 	

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
	  /* store trace header */
	  trhdstore(inbuf,trhd,ltrhd);
	  /* you can print out trace header info to printfile */
	  /*
	  tmfs = trhd[26];
	  if (iprint==1) fprintf(printfp,"trace= %d tmfs= %d \n",i2+1,tmfs);
	  */

	  if ( itype == 0 ) {
	     /* if floating point data input, use conv_float
		to convert ibm floating point to ieee floating point */
	     fltcopy(inbuf,outbuf,lout,ssindx);
	     conv_float((char*)outbuf,(char*)outbuf,lout,1);
	  } else if ( itype == 1 ) {
	     /* if int*2 (FX type) data input, convert to ieee floating point */
	     fxtcopy(inbuf,outbuf,lout,ssindx);
	  }

          /* sequentially write trace to output */	 
	  fwrite(outbuf,sizeof(float),lout,outfp);

    }	
    /* close printfile if needed */
    if ( iprint == 1 ) fclose(printfp);

}
 
/* trace header store */
/* author: zhiming li	10/30/90   */
/****
 input:
     tin       ---   input character array after sqread
     n         ---   number of half words in trace header
 output:
     tout      ---   output trace header array 
****/
int trhdstore(tin,tout,n) 
short tin[];
short tout[];
int n;  
{
int i;
for(i=0;i<n;i++) tout[i] = tin[i];
}

/* fl-type trace copy */
/* author: zhiming li	 */
/****
 input:
     tin       ---   input character array after sqread
     n         ---   number of samples per trace
     ssindx    ---   sample start index
 output:
     tout      ---   output floating point array of ibm format
***/
int fltcopy(tin,tout,n,ssindx) 
float tin[];
float tout[];
int n,ssindx;  
{
int i,i0;
i0 = (ssindx-1);
for(i=0;i<n;i++) tout[i] = tin[i0+i];
}

/* fx-type trace copy */
/* author: zhiming li	  */
/****
 input:
     tin       ---   input character array after sqread
     n         ---   number of samples per trace
     ssindx    ---   sample start index
 output:
     tout      ---   output floating point array of ieee format
***/
int fxtcopy(tin,tout,n,ssindx) 
short tin[];
float tout[];
int n,ssindx;  
{
int i,i0;
i0 = (ssindx-1);
for(i=0;i<n;i++) tout[i] = tin[i0+i];
}

/* sequentially read in a record of length lbyte (in bytes) */
/* author: zhiming li	 */
/*****

 input:
   rdfp       ---    file pointer of input data set
   lbyte      ---    number of bytes to read from input
   residual   ---    number of bytes unread in last record 
		     (first time, should be 0, leave it untouched 
		     afterwards)
 output:
   cbuf       ---    buffer holding a read record of length lbyte
   residual   ---    number of bytes remained (unreaded) in current record

 note:
	to use this routine to convert code-4 data:
	1. code-4 data in mainframe must be in VBS format 
	2. transfer of code-4 data to workstation must be done using
	   binary mode of ftp

*****/
int sqread(rdfp,cbuf,lbyte,residual)
FILE *rdfp;
char cbuf[];
int lbyte, *residual;
{
short recordsize, dummy;
int i, ireads;
int lrec;
int i0, lread;
char c;

   i0 = 0 ; 
   lread = lbyte;
   ireads = 0;


   read_continue:
   ireads = ireads + 1;

   if ( *residual <= 0 ) 
      {
      /* obtain the record length */ 
      fread(&recordsize,sizeof(short),1,rdfp);
      fread(&dummy,sizeof(short),1,rdfp);
      lrec = recordsize - 4; 
      }
   else 
      {
      lrec = *residual;
      }
   /* read the record */
   if ( lrec == lread ) 
      {
      fread(&cbuf[i0],sizeof(char),lread,rdfp);
      *residual = 0;
      }
   else if ( lrec > lread ) 
      {
      fread(&cbuf[i0],sizeof(char),lread,rdfp);
      *residual = lrec - lread;
      }
   else 
      {
      fread(&cbuf[i0],sizeof(char),lrec,rdfp);
      lread = lread - lrec;
      i0 = i0 + lrec;
      *residual = 0;
      /* read next record if needed */
      if ( ireads < 2 ) goto read_continue;
      /* a code-4 trace can only take at most 2 records */
      for (i=i0;i<lbyte;i++) cbuf[i] = '\0';

      }
}

