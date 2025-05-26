char *sdoc =
"SUCOMPRESS2 - round-trip compression of dataset at various factors\n"
"\n"
"sucompress2 < file.su out=file.comp [options]\n"
"\n"
"options:\n"
"ratio=,,,	compress at these various ratios\n"
"mse=,,,	alternative compression specification as mean square error limit\n"
"mode=both	\"data\" only, \"diff\" difference only, \"both\" data and difference\n"
"\nNotes:\n"
"1) Uses AWARE routines to compress and decompress.\n"
"2) Use sucompress and sudecomp for production compression.\n"
"3) Output file will be N*2+1 times larger than input.\n"
"4) MSE value depends on data amplitudes.\n"
"\n";

#include <stdio.h>
#include <math.h>
#include <su.h>
#include <segy.h>
#include "sucomp.h"
#include "seilib.h"

main (int argc , char **argv) {
int i, nratio=0, ndistort=0, ns=0, ntrace=0, ntraceo=0, nso=0, heado=0, hbyteo=0, itrace=0, outfd=0;
long size=0, clen=0, nwrite=0;
short *data;
float ratio[100], distort[100], time0=0, lambda=LAMBDA, actual=0, mse=0, dmse=0;
register float *dp1, *dp2, *dp3, *de;
char *cbuf, *dout, *dbuf, *mode="both", *out;

/* SU initializations */
initargs (argc,argv);
askdoc (1);

/* fetch parameters */
if (filestat(0) == TTY) err ("no input file");
if (getparstring ("out",&out) == 0) err ("out= missing");
if ((outfd = creat(out,0664)) < 0) err ("cant create out=%s",out);
nratio = getparfloat ("compress",ratio);
if (!nratio) nratio = getparfloat ("ratio",ratio);
ndistort = getparfloat ("distort",distort);
if (!nratio && !ndistort) err ("no ratio= or distort= specified");
for (i=0; i<nratio; i++) {
	if (ratio[i] < 3) err ("ratios must be 3 or greater");
	}
getparstring ("mode",&mode);

/* storage */
size = lseek64 (0,0,2);
data = (short*)malloc(size);
lseek64 (0,0,0);
read (0,data,size);
dbuf = (char*)malloc(size);
bcopy((char*)(data+1800),(char*)dbuf,(size-3600));
ns= data[1857];
ntrace = (size - 3600) / (ns*4 + 240);
size = 3600 + ntrace * (ns*4 + 240);
if (ntrace < 1) err ("ntrace < 1");
nwrite = write (outfd,data,size);
fprintf (stderr, "ns=%d ntrace=%d mode=%s\n", ns, ntrace, mode);

/* data mse */
dp1 = (float*)data;
dmse = 0.0;
for (itrace=0, dp1+=960; itrace<ntrace; itrace++, dp1+=60) {
	for (de=dp1+ns; dp1<de; dp1++) dmse += *dp1 * *dp1;
	}
if (dmse == 0.0) err ("data appears to be all zeros");

/* cycle through compressions */
for (i=0; i<nratio; i++) {
	time0 = cputime();
	seismic_compress ((char*)(data+1800),(long)(size-3600),&ntrace,&ns,DATA_FORMAT,
		0,240,ENDIAN,ratio[i],0.0,&lambda,&cbuf,&clen,&actual,0);
	seismic_decompress (cbuf,DATA_FORMAT,&ntraceo,&nso,&heado,&hbyteo,ENDIAN,
		&clen,&dout);
	lseek64 (0,nwrite,0);
	if (strcmp(mode,"diff")) {
		nwrite += write (outfd,dout,clen);
		}
	dp1 = (float*)data;
	dp2 = (float*)dout;
	dp3 = (float*)dbuf;
	mse = 0.0;
	for (itrace=0, dp1+=960, dp2+=60, dp3+=60; itrace<ntrace; itrace++, dp1+=60, dp2+=60, dp3+=60) {
		for (de=dp1+ns; dp1<de; dp1++, dp2++, dp3++) {
			*dp3 = *dp1 - *dp2;
			mse += *dp3 * *dp3;
			}
		}
	mse = sqrt (mse / dmse);
	fprintf (stderr,"compress ratio=%g actual=%g megabytes/sec=%g mse=%g\n",ratio[i],actual,(size-3600)*.000004/(cputime()-time0),mse);
	if (strcmp(mode,"data")) {
		nwrite += write (outfd,dbuf,clen);
		}
	free (cbuf);
	free (dout);
	}
for (i=0; i<ndistort; i++) {
	time0 = cputime();
	seismic_compress ((char*)(data+1800),(long)(size-3600),&ntrace,&ns,DATA_FORMAT,
		0,240,ENDIAN,0.0,distort[i],&lambda,&cbuf,&clen,&actual,0);
	seismic_decompress (cbuf,DATA_FORMAT,&ntraceo,&nso,&heado,&hbyteo,ENDIAN,
		&clen,&dout);
	fprintf (stderr,"compress mse=%g ratio=%g megabytes/sec=%g\n",distort[i],actual,(size-3600)*.000004/(cputime()-time0));
	lseek64 (0,nwrite,0);
	nwrite += write (outfd,dout,clen);
	free (cbuf);
	free (dout);
	}
}
