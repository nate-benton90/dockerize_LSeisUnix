/* BSWAP program */
#include "su.h"

char *sdoc = 
"BSWAP - Byte swap program to convert data between PC and SUN   \n"
"\n"
"bswap <in.file [parameters] >out.file			\n" 
"\n"
"Required parameters:							\n"
"in.file=     name of input file 				\n"
"out.file=    name of outptu file 					\n"
"Optional parameters:							\n"
"segy=1       convert segy file 	\n"
"             =0 convert velocity grid file \n"
"             =-1 convert segy trace header file (fxymig trace header file) \n"
"ntmax=4096   maximum number of samples per trace \n"
"mitype=1     machine (to run bswap) and input file types \n"
"             =1  --- Unix host and PC input file; or PC host and Unix input file  \n"
"             =0  --- Unix host and Unix input file; or PC host and PC input file  \n"
"NOTES:						 			\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	1/12/98   		\n"
;


void bhconvert(char *cin,char *cout);
void trhdconvert(char *cin,char *cout);
void traceconvert(char *cin,char *cout,int nt);
void bswap4(char *cin,char *cout);
void bswap2(char *cin,char *cout);


main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;
	int segy;
	char c4i[4], c4o[4];
	char *cbuf, *cout;
	int iread, ntmax, nt, nsegy, itrace;
	short ns;
	int mitype=1;

  	/* initialization */
  	initargs(argc,argv);
   	askdoc(1);

	file2g(infp);
	file2g(outfp);

	/* get parameters */
	if( !getparint("segy",&segy) ) segy=1;
	if( !getparint("ntmax",&ntmax) ) ntmax=4096;
	if( !getparint("mitype",&mitype) ) mitype=1;

	if(segy==1) {
		/* copy 3200-byte C-cards to output */
		cbuf = (char*) emalloc(3200*sizeof(char));
		iread = fread(cbuf,sizeof(char),3200,infp);
		if(iread!=3200) err(" error reading 1st 3200 bytes of input \n");
		fwrite(cbuf,sizeof(char),3200,outfp);
		free(cbuf);

		/* convert 400-byte binary file */
		cbuf = (char*) emalloc(400*sizeof(char));
		cout = (char*) emalloc(400*sizeof(char));
		fread(cbuf,sizeof(char),400,infp);
		bhconvert(cbuf,cout);
		fwrite(cout,sizeof(char),400,outfp);
		free(cout);
		free(cbuf);

		/* read in the traces and convert */
		nsegy = ntmax*sizeof(float)+240;
		cbuf = (char*) emalloc(nsegy*sizeof(char));
		cout = (char*) emalloc(nsegy*sizeof(char));

		itrace = 0;
		do {
			if(iread==240) {
				itrace = itrace + 1; 
				if(mitype==1) {
					/* convert trace header */
					trhdconvert(cbuf,cout);
					/* get number of samples of this trace */
					bcopy(&cout[114],&ns,2);
				} else {
					/* get number of samples of this trace */
					bcopy(&cbuf[114],&ns,2);
					/* convert trace header */
					trhdconvert(cbuf,cout);
				} 
				nt = ns;
				if(nt>ntmax) 
				err(" number of samples %d at trace %d exceeds %d \n",
					nt,itrace,ntmax);
				/* read in trace data and convert */ 
				iread=fread(&cbuf[240],sizeof(float),nt,infp);
			if(iread!=nt) err(" error reading trace  %d nt=%d \n",itrace,nt);
				traceconvert(&cbuf[240],&cout[240],nt);
				nsegy = nt*sizeof(float)+240;
				fwrite(cout,sizeof(char),nsegy,outfp);
			}
		} while (iread=fread(cbuf,sizeof(char),240,infp));

		free(cbuf);
		free(cout);
	} else if(segy==0) {
		do {
			if(iread==4) {
				bswap4(c4i,c4o);
				fwrite(c4o,sizeof(char),4,outfp);
			}
		} while (iread=fread(c4i,sizeof(char),4,infp));
	} else if(segy==-1) {

		/* read in the trace headers and convert */
		nsegy = 240;
		cbuf = (char*) emalloc(nsegy*sizeof(char));
		cout = (char*) emalloc(nsegy*sizeof(char));

		do {
			if(iread==240) {
				if(mitype==1) {
					/* convert trace header */
					trhdconvert(cbuf,cout);
				} 
				fwrite(cout,sizeof(char),nsegy,outfp);
			}
		} while (iread=fread(cbuf,sizeof(char),240,infp));

		free(cbuf);
		free(cout);
	}

}

/* convert binary header file */
void bhconvert(char *cin,char *cout) {
	int i, ibyte;

	ibyte = 0;
	for (i=0;i<3;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 3 * 4;
	for (i=0;i<24;i++) {
		bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 24 * 2;
	/* the rest using 4-byte swap */
	for (i=0;i<85;i++) {
		bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
}
/* convert trace header */
void trhdconvert(char *cin,char *cout) {
	int i, ibyte;

	ibyte = 0;
	/* tracl - cdpt   bytes 1-28 */
	for(i=0;i<7;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 7 * 4;
	/* trid - duse   bytes 29-36 */
	for(i=0;i<4;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 4 * 2;
	/* offset - gwdep   bytes 37-68 */
	for(i=0;i<8;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 8 * 4;
	/* scalel - scalco   bytes 69-72 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* sx - gy           bytes 73-88 */
	for(i=0;i<4;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 4 * 4;
	/* counit - otrav    bytes 89-180 */
	for(i=0;i<46;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 46 * 2;
	/* d1 - unscale    bytes 181-204 */
	for(i=0;i<6;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 6 * 4;
	/* mark - mutb    bytes 205-208 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* dz - fz        bytes 209-216 */
	for(i=0;i<2;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 2 * 4;
	/* n2 -           bytes 217-220 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* the rest using 4-byte swap */ 
	for (i=0;i<5;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
}

void traceconvert(char *cin,char *cout,int nt) {
	int i;
	for (i=0;i<nt;i++) {
	 	bswap4(&cin[i*4],&cout[i*4]);
	}
}

/* byte swapping of 4-bytes interger or float */
void bswap4(char *c4i, char *c4o)  {
	c4o[0] = c4i[3];	
	c4o[1] = c4i[2];	
	c4o[2] = c4i[1];	
	c4o[3] = c4i[0];	
}
/* byte swapping of 2-bytes interger */
void bswap2(char *c2i, char *c2o)  {
	c2o[0] = c2i[1];	
	c2o[1] = c2i[0];	
}
