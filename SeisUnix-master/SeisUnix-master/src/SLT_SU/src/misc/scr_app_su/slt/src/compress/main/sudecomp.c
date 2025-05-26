char *sdoc = "SUDECOMP - wavelet decompression of SU datasets in multiple parts\n"
"\n"
"sudecomp < file.comp [options]		report statistics of comp file\n"
"sudecomp < file.comp > file.su [options]\n"
"\n"
"options:\n"
"start=  count=    end=       decompress these blocks (groups) only\n"
"                             default decompress all blocks \n"
"min=    max=                 decompress only key values between this range\n"
"                             default min=-2000000000 max=2000000000 \n"
"head=1                       assume SEGY file header is prepended to file\n"
"verbose=0                    report key value, size, and decompression speed\n"
"                             if > 1, report every verbose# chunks\n"
"datain=                      input compressed dataset name \n"
"                             (instead of standard input) \n"
"                             specify datain=file1 datain=file2 ... \n"
"                             datain=filen for multiple input data sets \n"
"skipfile=                    name of the file to indicate how many traces \n"
"                             to skip before output. The number of traces \n"
"                             to skip is writen starting at the scolumn on \n"
"                             the last line of the skipfile, as explained \n"
"                             below.  \n"
"scolumn=29                   starting colume position of the last line of \n"
"                             the skipfile to indicate number of traces \n"
"                             to skip \n"
"on2trace=0                   output total number of traces in input to \n"
"                             the key word tracl (first 4 bytes) in the \n"
"                             first output trace \n"
"                             0=no 1=yes \n"
"headercheck=1                check to see if the compress tape id header \n"
"                             is stored at segy C40 card. \n"
"                             If not, read through the trace headers \n" 
"                             and update the compress tape id header \n"
"                             at segy C40 card (input must be specified \n"
"                             as datain=... to be updated)    \n"
"                             If yes, skip the first pass of reading \n"
"                             through the trace headers. This should  \n"
"                             reduce the time of decompression process \n"
"                             1=yes \n"
"                             0=no; it will read through the trace headers \n"
"                               first, before decompressing the data.  \n"
"output=1                     output decompressed traces (0=no 1=yes) \n"
" Examples: \n"
"  1. to add the compression tape id header to the compressed data: \n"
"       sudecomp datain=input.su.comp output=0 \n"
"  2. to decompress two compressed data sets and use skipfile \n"
"     for the trace counter control (e.g. for kzmig): \n"
"       sudecomp datain=input.su.1.comp datain=input.su.2.comp \n"
"         on2trace=1 skipfile=kzmig.his.file | kzmig ...  \n" 
"\n"
"Notes:\n"
"1) MSE is mean squared error of compression.\n"
"   It is non-zero if it was specified as a forward compression parameter.\n"
"\n";
#include <stdio.h>
#include <su.h>
#include <segy.h>
#include <cwp.h>
#include <hdr.h>
#include "bswap.h"
#include "sucomp.h"
#include "seilib.h"

size_t fread_cmh( char *cbuf, size_t chsize, FILE *infp);
void gather_convert( char *cbuf, int ntrace, int ns);

comp_header ch;
segytrace tr;


main (int argc , char **argv) {
	int verbose=0, igroup=0, group1=0, group2=2000000000;
	int count=0, maxtrace=0, ntrace=0, ns=0, output=1;
	int hbytes=0, hflag=0, alltrace=0, ngroup=0;
	int min=-2000000000, max=2000000000, head=1;
	int hmin=2000000000, hmax=-2000000000;
	long maxbyte=0;
	long blen = 0;
	float *data=0, time0=0, alltime=0, timex=0, compress=0, mse=0;
	char *cbuf = (char *) NULL;
	FILE *outfp=stdout;
	float tmp;
	int itmp;
	char *c80,key[10];
	char *bh_in, *bh_out;

	FILE *infp; 
	char *datain;
	int ndata;
	int *nitrace, mtrace;
	char *skipfile;
	int ntraceskip=0, i, idata, idatain;
	int scolumn, on2trace, firsttrace, itrace, ntotal;

	int headercheck, iigroup, iitrace, iimaxbyte, iimaxtrace;
	float iicomp, iimse;
	int idcheck=1;

/* SU initializations */
	initargs (argc,argv);
	askdoc (1);

	if(!getparint ("output",&output)) {
		if (filestat(1) != TTY) {
			output = 1;
			file2g(outfp);
		} else {
			output = 0;
		}
	}

/* fetch parameters */
	getparint ("start",&group1);
	getparint ("end",&group2);
	if (getparint ("count",&count)) group2 = group1 + count - 1;
	getparint ("min",&min);
	getparint ("max",&max);
	getparint ("verbose",&verbose);
	getparint ("head",&head);
	if(!getparint("on2trace",&on2trace)) on2trace=0;
	if(!getparint("headercheck",&headercheck)) headercheck=1;

	ndata = 0;
	ndata = countparname("datain");
	idatain = 1;
	if(ndata==0) {
		ndata = 1;
		idatain = 0;
	}

	if(headercheck==1 && idatain==0) {
	err(" datain= must be specified for input when headercheck=1 \n");
	}
	nitrace = (int*) malloc(ndata*sizeof(int));
	c80 = (char*)malloc(80*sizeof(char));

/* scan input compress id header */
    
    if(headercheck==1) {
	fprintf (stderr,"checking compressed tape headers ...\n"); 
	fflush (stderr);
	ngroup = 0;
	for(i=0;i<ndata;i++) {
		if(idatain==0) {
			infp = stdin;
		} else {
		  	getnparstring(i+1,"datain",&datain);
			fprintf (stderr," reading id headers of %s\n",datain);
			infp = fopen(datain, "r");
		}
		iimse = 0.;
		iicomp = 0.;
		iigroup = 0;
		iitrace = 0;
		iimaxbyte = 0;
		iimaxtrace = 0;
		file2g(infp);
		nitrace[i] = 0;

		bzero(c80,80);
		fseeko(infp,80*39,0);
		fread(c80,80,sizeof(char),infp);
		fseeko(infp,0,0);
		sscanf(c80+20,"%s %d %d %d %d %f %f",
		&key,&iigroup,&iitrace,&iimaxbyte,&iimaxtrace,&iicomp,&iimse);

		if(iigroup>0 && iitrace>0 && iimaxbyte>0 && iimaxtrace>0) {
			nitrace[i] = iitrace;

	fprintf (stderr," key=%s ngroup=%d ntrace=%d max-bytes/gath=%d\n",
			&key,iigroup,iitrace,iimaxbyte);
			fprintf (stderr," max-fold=%d compress=%f mse=%f\n",
			iimaxtrace,iicomp,iimse);
			ngroup = ngroup + iigroup;
		maxtrace = maxtrace > iimaxtrace ? maxtrace : iimaxtrace;
			alltrace += iitrace;
			maxbyte = maxbyte > iimaxbyte ? maxbyte : iimaxbyte;
			compress += iicomp*iigroup;
			mse += iimse*iigroup;
		} else {
			fprintf (stderr," no compression id header found \n");
			nitrace[i] = 0;
			idcheck = 0;
		}
		if(idatain!=0) fclose(infp);
	fflush (stderr);
	}
    } else {
	idcheck = 0;
    }

/* check every traces if needed */
    if (LINUX==1) {
    	bh_in = (char*) malloc(400);
    	bh_out = (char*) malloc(400);
    }

    if(idcheck==0) {
	if (output) {fprintf (stderr,"preliminary pass through headers ...\n"); 
	fflush (stderr);}
	if (!output && verbose) fprintf (stderr,"Headers:\n");

	ngroup = 0;
	for(i=0;i<ndata;i++) {
		if(idatain==0) {
			infp = stdin;
		} else {
		  	getnparstring(i+1,"datain",&datain);
			fprintf (stderr," reading headers of %s\n",datain);
			if(headercheck==1) {
				infp = efopen(datain, "r+");
			} else {
				infp = efopen(datain, "r");
			}
		}
		file2g(infp);
		nitrace[i] = 0;

		iimse = 0.;
		iicomp = 0.;
		iigroup = 0;
		iitrace = 0;
		iimaxbyte = 0;
		iimaxtrace = 0;

		if (head) fseeko(infp,3600,0);
		fread_cmh((char*)&ch,56,infp);

		do {
			if (strcmp (ch.id,SUCOMPID)) {
				fprintf (stderr,"error at %d: %s\n",ngroup,ch.id);
				err("header error=%d loc=%d message=%s\n",
					ngroup,(int)fseeko(infp,0,1),ch.id);
			} else	{
				if (ch.ntrace < 0) err ("ch.ntrace group %d negative",ngroup);
				maxtrace = maxtrace > ch.ntrace ? maxtrace : ch.ntrace;
				iimaxtrace = iimaxtrace > ch.ntrace ? iimaxtrace : ch.ntrace;
				alltrace += ch.ntrace;

				nitrace[i] += ch.ntrace;
				iitrace += ch.ntrace;

				if (ch.nbyte < 0) err ("ch.nbyte group %d negative",ngroup);
				maxbyte = maxbyte > ch.nbyte ? maxbyte : ch.nbyte;
				iimaxbyte = iimaxbyte > ch.nbyte ? iimaxbyte : ch.nbyte;

				hmin = hmin < vtoi(hdtype(ch.key),ch.value) ? hmin : vtoi(hdtype(ch.key),ch.value);
				hmax = hmax > vtoi(hdtype(ch.key),ch.value) ? hmax : vtoi(hdtype(ch.key),ch.value);
				compress += ch.compress;
				iicomp += ch.compress;

				mse += ch.mse;
				iimse += ch.mse;

				if (!output && verbose && ngroup%verbose==0 
					&& ngroup>=group1 && ngroup<=group2 
					&& vtoi(hdtype(ch.key),ch.value) >= min 
					&& vtoi(hdtype(ch.key),ch.value) <= max) {
					fprintf (stderr, "%s=%d ntraces=%d bytes=%d compress=%g mse=%g\n", ch.key, vtoi(hdtype(ch.key),ch.value), ch.ntrace, ch.nbyte, ch.compress, ch.mse);
				}

			}
			ngroup += 1;
			iigroup += 1;
			if (fseeko(infp,ch.nbyte,1) < 0) break;

		} while( fread_cmh((char*)&ch,56,infp) );

		if(idatain!=0) fprintf (stderr," There are %d traces in %s\n",
			nitrace[i],datain);

		if(headercheck==1) {
	          fprintf (stderr," updating compress id header ...\n");

		  iicomp = iicomp/iigroup;
		  iimse = iimse/iigroup;

		  sprintf(c80,"C40 END EBCDIC      %s %d %d %d %d %8.3f %8.3f",
		  ch.key,iigroup,iitrace,iimaxbyte,iimaxtrace,iicomp,iimse);
		  sprintf(c80+79,"\n");

	          fprintf (stderr," updating compress id header done \n");

		  fseeko(infp,80*39,0);
		  fwrite(c80,80,sizeof(char),infp);
		}
		if(idatain!=0) fclose(infp);
	}
	group2 = ngroup < group2 ? ngroup-1 : group2;
	ngroup = ngroup > 0 ? ngroup : 1;
    }

    free(c80);

	fprintf (stderr,"Compressed file summary:\n");
	fprintf (stderr,"key=%s min=%d max=%d\n",ch.key,hmin,hmax);
	fprintf (stderr,"ngroup=%d start=%d end=%d\n",ngroup,group1,group2);
	fprintf (stderr,"total_traces=%d largest_traces=%d largest_bytes=%d\n",alltrace, maxtrace, maxbyte);
	fprintf (stderr,
		"mean_compress=%g mean_mse=%g\n",compress/ngroup,mse/ngroup);

/* decompression */
	if (!output) exit (0);

/* find out the skip trace number */
	if (getparstring("skipfile",&skipfile)) {
		if(!getparint("scolumn",&scolumn)) scolumn=29;
		if(scolumn>80) err("scolumn must be less than 80");
		if( (infp = fopen(skipfile,"r"))!=NULL ) {
			cbuf = (char*) malloc(80*sizeof(char));
			for(i=0;i<2000000000;i++) {
				if(feof(infp) !=0) break;
				bzero(cbuf,80);
				fgets(cbuf,80,infp);
				sscanf(cbuf+scolumn-1,"%d",&ntraceskip);
			}
			fprintf(stderr," number of traces to skip = %d \n",ntraceskip);
			fclose(infp);
			free(cbuf);
		} else {
			ntraceskip = 0;
			fprintf(stderr," skipfile not present: set ntraceskip=0  \n");
			fprintf(stderr," number of traces to skip = %d \n",ntraceskip);
		}
	} else {
		ntraceskip = 0;
	}

	idata = ndata;
	itmp = 0;
	for(i=0;i<ndata;i++) {
		itmp += nitrace[i];
		if(itmp>ntraceskip) {
			idata = i;
			break;
		}
	}
	ntotal = alltrace;


/* allocate storage */
	data = (float*) malloc (maxbyte>3600?maxbyte:3600);
	alltime = 0;
	alltrace = 0;
	ngroup = 0;
	igroup = 0;
	mtrace = 0;
	for(i=0;i<idata;i++) mtrace += nitrace[i];
	firsttrace = 1;

	for(i=idata;i<ndata;i++) {
		if(idatain!=0) {
		  	getnparstring(i+1,"datain",&datain);
			infp = fopen(datain, "r");
		}
		file2g(infp);

		/* restore header */
		fseeko (infp,0,0);
		if (head) {
			if(LINUX==1) {
				fread (data,1,3200,infp);
				if(i==idata) fwrite (data,1,3200,outfp);
				fread (bh_in,1,400,infp);
				if(i==idata) {
					bhconvert(bh_in,bh_out);
				        fwrite (bh_out,1,400,outfp);
				}
			} else {
				fread (data,1,3600,infp);
				if(i==idata) fwrite (data,1,3600,outfp);
			}
		}
		fread_cmh ((char*)&ch,56,infp);

/* decompress each chunk */
		do {
			if (   vtoi(hdtype(ch.key),ch.value) >= min 
			&& vtoi(hdtype(ch.key),ch.value) <= max
			&& mtrace+ch.ntrace>ntraceskip ) {
				if (verbose && igroup%verbose == 0) 
				fprintf (stderr, "#%d %s=%d ntraces=%d comp_bytes=%d compress=%g mse=%g", igroup, ch.key, vtoi(hdtype(ch.key),ch.value), ch.ntrace, ch.nbyte, ch.compress, ch.mse);

				time0 = cputime ();
				if (fread (data,1,ch.nbyte,infp) < ch.nbyte) break;
				cbuf = (char *) NULL;
				if(mtrace>=ntraceskip) {
					if(ch.ntrace>=18) {
						seismic_decompress ((char*)data,DATA_FORMAT,&ntrace, &ns,&hbytes,&hflag, ENDIAN,&blen,&cbuf);

						if(LINUX==1)
						gather_convert(cbuf,ntrace,ns);

						if(firsttrace==1 && on2trace==1 ) {
							firsttrace=0;
							bcopy(cbuf,&tr,240);
							tr.tracl = ntotal;
							bcopy(&tr,cbuf,240);
						}
						fwrite (cbuf,1,(ch.ns+60)*ch.ntrace*4,outfp);
					} else {
						if(LINUX==1)
					gather_convert((char*)data,ch.ntrace,ch.ns);
						if(firsttrace==1 && on2trace==1) {
							firsttrace=0;
							bcopy(data,&tr,240);
							tr.tracl = ntotal;
							bcopy(&tr,data,240);
						}
						fwrite (data,1,(ch.ns+60)*ch.ntrace*4,outfp);
					}
				} else {
					if(ch.ntrace>=18) {
						seismic_decompress ((char*)data,DATA_FORMAT,&ntrace,
							&ns,&hbytes,&hflag, ENDIAN,&blen,&cbuf);
						if(LINUX==1) gather_convert(cbuf,ntrace,ns);
						for(itrace=0;itrace<ch.ntrace;itrace++) {
							if(itrace+mtrace<ntraceskip) continue;
							bcopy(cbuf+itrace*(ch.ns+60)*4,&tr,(ch.ns+60)*4);
							if(firsttrace==1 && on2trace==1 ) {
								firsttrace=0;
								tr.tracl = ntotal;
							}
							fwrite (&tr,1,(ch.ns+60)*4,outfp);
						}
					} else {
						if(LINUX==1)
						gather_convert((char*)data,ch.ntrace,ch.ns);
						for(itrace=0;itrace<ch.ntrace;itrace++) {
							if(itrace+mtrace<ntraceskip) continue;
							bcopy(data+itrace*(ch.ns+60),&tr,(ch.ns+60)*4);
							if(firsttrace==1 && on2trace==1 ) {
								firsttrace=0;
								tr.tracl = ntotal;
							}
							fwrite (&tr,1,(ch.ns+60)*4,outfp);
						}
					}
				}
				if(cbuf != (char *) NULL ) free (cbuf);
				timex = cputime() - time0;
				if (verbose && igroup%verbose == 0) fprintf (stderr," sec=%g rate=%g\n",timex,(ch.ntrace*ch.ns*4)*.000001/timex);
				alltime += timex;
				alltrace += ch.ntrace;
				ngroup++;
			} else {
				fseeko (infp,ch.nbyte,1);
			}

			/* if unit in bounds */
			igroup += 1;
			if(igroup>group2) break;
			mtrace += ch.ntrace;

		} while(fread_cmh((char*)&ch,56,infp));

		if(igroup>group2) break;
		fclose(infp);
	}


	tmp = alltrace;
	tmp = tmp * ch.ns;
	tmp = tmp / 250000.;
	itmp = tmp;
	if(alltime>0.) tmp = tmp /alltime;

	if (alltime > 0.) 
	fprintf (stderr,"%d %s's with %d traces and %d megabytes decompressed at %g megabytes per second\n", ngroup, ch.key, alltrace, itmp,tmp);
}

size_t fread_cmh( char *cbuf, size_t chsize, FILE *infp) {
	size_t nread;
	char *cmh_in;

	if ( LINUX==1 ) {
		cmh_in = (char*) malloc(chsize);
		nread = fread (cmh_in,1,chsize,infp);
		if(nread==chsize) {
			cmhconvert(cmh_in,cbuf);
		} else {
			return nread;
		}
		free(cmh_in);
	} else {
		nread = fread (cbuf,1,chsize,infp);
	}
	return nread;
}

void gather_convert( char *cbuf, int ntrace, int ns) {
	char *cin, *cout;
	int j, nsegy;

	nsegy = ns*sizeof(float)+240;
	cin  = (char *) malloc( nsegy);
	cout  = (char *) malloc( nsegy);

	for(j=0;j<ntrace;j++) {
		bcopy(cbuf+j*nsegy,cin,nsegy);
		trhdconvert(cin,cout);
		traceconvert(&cin[240],&cout[240],ns);
		bcopy(cout,cbuf+j*nsegy,nsegy);
	}
	free(cin);
	free(cout);
}
