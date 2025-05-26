/*
 * iopkge
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
/* #include <errno.h> */

#include "../include/su.h"
/* #include "../include/hdr.h" */
#include "../include/local.h"

/* #define STDIN 0 */
/* #define STDOUT 1 */

extern int xargc;
extern char **xargv;
char *gname();
extern bool verbose;

#define MAXFILES 20
#define BUFSZ 80

static struct {
	filetype type;
	bool hclosed;
	int alen;		/* Ascii header length */
	int dbpt;		/* Data bytes per trace */
	char name[64];
	char buf[BUFSZ];
	char *pbuf;
	int left;
	bool first;
} sufile[] = {
/* type, hclosed, alen, dbpt, name,      buf, pbuf, left, first */
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
 CLOSED, false,   0,    -1,   "UNKNOWN", "",  0,    0,    true,
};

#define MAXRECSZ 65536
char tapebuf[MAXRECSZ];

filetype gettype(fd)
int fd;
{
	return(sufile[fd].type);
}

/* typedef enum {UNKNOWN = -1, CLOSED, TTY, DISK, DIR, TAPE, PIPE} filetype; */
char *strtype(ftype)
filetype ftype;
{
	switch(ftype) {

		case UNKNOWN:	return("UNKNOWN");

		case CLOSED:	return("CLOSED");

		case TTY:	return("TTY");

		case DISK:	return("DISK");

		case DIR:	return("DIR");

		case PIPE:	return("PIPE");

		case TAPE:	return("TAPE");

		case DEVNULL:	return("DEVNULL");
	}

	warn(__FILE__,__LINE__,"strtype: Unknown type");

	return("UNKNOWN FILE TYPE");
}

/*
 * input - open standard SU input
 */
int input()
{
	static bool called=false;
	int infd;
	char *nam;

	if(called==true) {
		err(__FILE__,__LINE__,"input: can't be called more than once");
	} else {
		called = true;
	}

	if(xargc>1) if(!strcmp(xargv[1],"-")) selfdoc();

	infd = STDIN;

	sufile[infd].type = statfil(infd);

	if( sufile[infd].type==TTY ) {
		warn(__FILE__,__LINE__,"stdin can't be a TTY");
		selfdoc();
	}
/* 	suckfile(infd); */
	if( !(sufile[infd].type==DISK ||
	      sufile[infd].type==DEVNULL ||
	      sufile[infd].type==TAPE ||
	      sufile[infd].type==PIPE ))
		warn(__FILE__,__LINE__,"input: Illegal input file type (filetype of %s is %s)",
			sufile[infd].name,strtype(sufile[infd].type));

	nam = gname(infd);
	strcpy(sufile[infd].name,nam);

	return(infd);
}

/*
 * sucreat
 */
int sucreat(fname,mode)
char *fname;
int mode;
{
	int outfd;

	outfd = creat(fname,mode);

	if(outfd<0) {
		warn(__FILE__,__LINE__,"sucreat: can't creat %s",fname);
		return(outfd);
	}

	sufile[outfd].type = statfil(outfd);
	strcpy(sufile[outfd].name, fname);

/* 	suckfile(outfd); */
	if( !(sufile[outfd].type==DISK ||
	      sufile[outfd].type==DEVNULL ||
	      sufile[outfd].type==TAPE ||
	      sufile[outfd].type==PIPE ))
		warn(__FILE__,__LINE__,"sucreat: Illegal input file type (filetype of %s is %s)",
			sufile[outfd].name,strtype(sufile[outfd].type));

	return(outfd);
}
/*
 * suopen
 */
int suopen(fname,flag)
char *fname;
int flag;
{
	int infd;

	infd = open(fname,flag);

	if(infd<0) {
		warn(__FILE__,__LINE__,"suopen: can't open %s",fname);
		return(infd);
	}

	sufile[infd].type = statfil(infd);
	strcpy(sufile[infd].name, fname);

/* 	suckfile(infd); */
	if( !(sufile[infd].type==DISK ||
	      sufile[infd].type==DEVNULL ||
	      sufile[infd].type==TAPE ||
	      sufile[infd].type==PIPE ))
		warn(__FILE__,__LINE__,"suopen: Illegal input file type (filetype of %s is %s)",
			sufile[infd].name,strtype(sufile[infd].type));

	return(infd);
}

/*
 * sufileno
 */
int sufileno(pfd)
FILE *pfd;
{
	int ipfd;

	ipfd = fileno(pfd);
	if(ipfd<0) {
		warn(__FILE__,__LINE__,"sufileno: can't fileno");
		return(ipfd);
	}

	sufile[ipfd].type = statfil(ipfd);
/* 	sufile[ipfd].name */

/* 	suckfile(ipfd); */
	if( !(sufile[ipfd].type==DISK ||
	      sufile[ipfd].type==DEVNULL ||
	      sufile[ipfd].type==TAPE ||
	      sufile[ipfd].type==PIPE ))
		warn(__FILE__,__LINE__,"sufileno: Illegal input file type (filetype of %s is %s)",
			sufile[ipfd].name,strtype(sufile[ipfd].type));

	return(ipfd);
}

/*
 * surewind
 */
/* #include <sys/file.h> */
surewind(fd)
int fd;
{
	if(lseek(fd,0,0)) err(__FILE__,__LINE__,"surewind can't lseek");  /* Shulod report errno */
	sufile[fd].hclosed = false;
	sufile[fd].alen = 0;
/* 	sufile[fd].dbpt = -1; */
	strcpy("",sufile[fd].buf);
/* 	sufile[fd].pbuf = 0; */
	sufile[fd].left = 0;
	sufile[fd].first = true;
}

/*
 * suclose
 */
suclose(fd)
int fd;
{
	if(-1==close(fd)) err(__FILE__,__LINE__,"suclose can't close");
	sufile[fd].type = CLOSED;
	sufile[fd].hclosed = false;
	sufile[fd].alen = 0;
	sufile[fd].dbpt = -1;
	strcpy("UNKNOWN",sufile[fd].name);
	strcpy("",sufile[fd].buf);
/* 	sufile[fd].pbuf = 0; */
	sufile[fd].left = 0;
	sufile[fd].first = true;
}


/*
 * output - open standard SU output
 */
int output()
{
	static bool called=false;
	char *nam;
	int outfd;

	if(called==true) {
		err(__FILE__,__LINE__,"output: can't be called more than once");
	} else {
		called = true;
	}

	outfd = STDOUT;

	sufile[outfd].type = statfil(outfd);

	if(sufile[outfd].type==TTY) err("output: output file type is TTY");

/* 	suckfile(outfd); */
	if( !(sufile[outfd].type==DISK ||
	      sufile[outfd].type==DEVNULL ||
	      sufile[outfd].type==TAPE ||
	      sufile[outfd].type==PIPE ))
		warn(__FILE__,__LINE__,"output: Output file type is %s",strtype(sufile[outfd].type));

	nam = gname(outfd);
	strcpy(sufile[outfd].name,nam);

	return(outfd);
}

/* gettr - get an SU trace
 *
 * Returns:
 *	int: number of bytes read on current trace (0 after last trace)
 *
 * Synopsis:
 *	int gettr(fd,atr)	for standard float traces
 *	Sutrace *atr;
 *
 * Example:
 *	Sutrace tr;
 *	int infd,outfd;
 *	...
 *	infd = input();
 *	outfd = output();
 *	...
 *	while (gettr(infd,&tr)) {
 *		tr.offset = abs(tr.offset);
 *		puttr(outfd,&tr);
 *	}
 *	...
 *
 */

int gettr(fd,atr)
int fd;
Sutrace *atr;		/* pointer to segy trace */
{
	int nread,bpt;
	static int nbh;
	static bool first=true;

	if(first) {
		first = false;
/* 		nbh = sizeof(Sutrace)-sizeof(float*); */
		nbh = NTRHB;
	}

	suckfile(fd);

	/* TAPE */
	if(sufile[fd].type==TAPE) {

		bpt = nbh + sufile[fd].dbpt;

		if(bpt>MAXRECSZ) {
			err(__FILE__,__LINE__,"gettr: bpt=%d > MAXRECSZ=%d",bpt,MAXRECSZ);
		}

		nread = read(fd, tapebuf, bpt);
		if(nread==0) return(0);
		if(nread!=bpt) {
	  	warn(__FILE__,__LINE__,"gettr: read error. read %d bytes out of %d",
	  				nread,sufile[fd].dbpt);
		}
		bcopy(tapebuf, (char*)atr, nbh);
		bcopy(tapebuf+nbh, (char*)(atr->data), sufile[fd].dbpt);
		return(nread);
	}

	/* DISK OR PIPE */
	nread = pread(fd, (char*)atr, nbh );

	if(nread==0) return(0);

	nread = pread(fd, (char*)(atr->data), sufile[fd].dbpt);

	if(nread!=sufile[fd].dbpt) {
	  err(__FILE__,__LINE__,"gettr: read error. read %d bytes out of %d",
	  				nread,sufile[fd].dbpt);
	}

	return(nread+nbh);
}

suchns(fd,ns)
int fd,ns;
{
	sufile[fd].dbpt = ns*sizeof(float);
}

/* puttr - put an SU trace
 *
 * Returns:
 *	int: number of bytes written on current trace (exit on error)
 *
 * Synopsis:
 *	int puttr(fd,atr)	for standard float traces
 *	Sutrace *atr;
 *
 * Example:
 *	Sutrace tr;
 *	int infd,outfd;
 *	...
 *	infd = input();
 *	outfd = output();
 *	...
 *	while (gettr(infd,&tr)) {
 *		tr.offset = abs(tr.offset);
 *		puttr(outfd,&tr);
 *	}
 *	...
 *
*/

int puttr(fd,atr)
int fd;
Sutrace *atr;		/* pointer to su trace */
{
	int nwrite,bpt;
	static int nbh;
	static bool first=true;

	if(first) {
		first = false;
/* 		nbh = sizeof(Sutrace)-sizeof(float*); */
		nbh = NTRHB;
	}

	suckfile(fd);

	/* TAPE */
	if(sufile[fd].type==TAPE) {

		bcopy((char*)atr, tapebuf, nbh);
		bcopy((char*)(atr->data), tapebuf+nbh, sufile[fd].dbpt);

		bpt = nbh + sufile[fd].dbpt;

		if(bpt>MAXRECSZ) {
			warn(__FILE__,__LINE__,"puttr: bpt=%d > MAXRECSZ=%d",bpt,MAXRECSZ);
		}

		nwrite = write(fd, tapebuf, bpt);
		if(nwrite!=bpt) {
	  	err(__FILE__,__LINE__,"puttr: write error. wrote %d bytes out of %d",
	  				nwrite, sufile[fd].dbpt);
		}
		return(nwrite);
	}

	/* DISK OR PIPE */
/* 	nbh = sizeof(Sutrace)-sizeof(float*); */
	nbh = NTRHB;

	nwrite = write(fd, (char*)atr, nbh );

	if(nwrite!=nbh) {
	  err(__FILE__,__LINE__,"puttr: write error. wrote %d bytes out of %d",
	  nwrite,nbh);
	}

	nwrite = write(fd, (char*)(atr->data), sufile[fd].dbpt);

	if(nwrite!=sufile[fd].dbpt) {
	  err(__FILE__,__LINE__,"puttr: write error. wrote %d bytes out of %d",
	  nwrite,sufile[fd].dbpt);
	}

	return(nwrite+nbh);
}


int getntr(fd)
int fd;
{
	int nbpt,len,ntr;

	if(sufile[fd].type!=DISK) err(__FILE__,__LINE__,"getntr: non disk sufile");

/* 	nbpt = sizeof(Sutrace) - sizeof(float*) + sufile[fd].dbpt; */
	nbpt = NTRHB + sufile[fd].dbpt;

	len = lseek(fd,0,2);

	ntr = (len-sufile[fd].alen-sizeof(Subhed))/nbpt;

	return(ntr);
}

/* gettra - get disk trace by trace number
 *
 * Returns number of bytes read
 *
*/


/* #include "../include/sysV.h"		/* <<< PATCH for the HP >>> */

int gettra(fd,atr,itr)
Sutrace *atr;
int fd,itr;
{
	long nseek,rseek;
	int nbpt;

	if(sufile[fd].type!=DISK) err(__FILE__,__LINE__,"gettra: non disk sufile");

	/* Position file pointer at start of requested trace */
/* 	nbpt = sizeof(Sutrace) - sizeof(float*) + sufile[fd].dbpt; */
	nbpt = NTRHB + sufile[fd].dbpt;

	nseek = sufile[fd].alen + sizeof(Subhed) + itr*nbpt;

/* 	if(nseek>lseek(fd,0,2) || nseek<0 ) return(0); */

	rseek = lseek(fd, nseek, 0);

	if (rseek != nseek) {
		err(__FILE__,__LINE__,"gettra: lseek failed (rseek=%d != nseek=%d",rseek,lseek);
	}

	return(gettr(fd,atr));
}

int puttra(fd,atr,itr)
Sutrace *atr;
int fd,itr;
{
	long nseek,rseek;
	int nbpt;

	if(sufile[fd].type!=DISK) err(__FILE__,__LINE__,"puttra: non disk sufile");

	/* Position file pointer at start of requested trace */
	nbpt = NTRHB + sufile[fd].dbpt;
	nseek = sufile[fd].alen + sizeof(Subhed) + itr*nbpt;

	rseek = lseek(fd, nseek, 0);

	if (rseek != nseek) {
		err(__FILE__,__LINE__,"puttra: lseek failed (rseek=%d != nseek=%d",rseek,lseek);
	}

	return(puttr(fd,atr));
}

/* pread, pfread - read from a pipe
 *
 * Returns:
 *	int: number of bytes read or -1 on error
 *
 * Synopsis:
 *	pread(fd, buf, nb)
 *	int fd;		file descriptor
 *	char *buf;	pointer to buffer
 *	int nb;		number of bytes requested
 *
 *	pfread(buf, itemsize, nitems, stream)
 *	char *buf;	pointer to buffer
 *	int itemsize;	sizeof(item pointed to)
 *	int nitems;	number of items requested
 *	FILE *stream;	input stream
 *
 * Credits:
 *	CWP: Shuki
 *
 */

int pread(fd, buf, nb)
int fd, nb;
char *buf;
{
	int nread, ntotal = 0;

/* 	fprintf(stderr, "pread: %d bytes to read\n", n); */
	while (nb) {
		switch (nread = read(fd, buf, nb)) {
		case -1:
			warn(__FILE__,__LINE__,"pread: read failed");
		case 0:	/* no more traces (or premature pipe shutdown) */
/* 			fprintf(stderr, "pread: nread=%d\n", nread); */
			return(ntotal);
		default:
/* 			fprintf(stderr, "pread: nread=%d\n", nread); */
			ntotal += nread;
			nb -= nread;
			buf += nread;
		break;
		}
/* 		fprintf(stderr,"    pread: just read %d, %d left\n",nread,nb); */
	}
	return(ntotal);
}


pfread(buf, itemsize, nitems, stream)
char *buf;
int itemsize;
int nitems;
FILE *stream;
{
	int nread, ntotal = 0;

	while(nitems) {
		switch (nread = fread(buf, itemsize, nitems, stream)) {
		case -1:
			warn(__FILE__,__LINE__,"pfread: read failed");
			return(-1);
		case 0:
			if (!feof(stream)) {
				err(__FILE__,__LINE__,"pfread: zero bytes read, but not EOF");
			}
			return(ntotal);
		default:
			if (ferror(stream)) {
				err(__FILE__,__LINE__,"pfread: I/O error from ferror");
			}
			ntotal += nread;
			nitems -= nread;
			buf += nread*itemsize;
		break;
		}
	}
	return(ntotal);
}

/* rew - rewind file */

rew(fd)
int fd;
{
	if(lseek(fd,0L,0) == -1)
		err(__FILE__,__LINE__,"rew: rewind failed");
}

suckfile(fd)
int fd;
{
	if(fd>=MAXFILES) {
		err(__FILE__,__LINE__,"suckfile: Illegal file: fd=%d bigger than MAXFILES=%d",
						fd,MAXFILES);
	}

	if( !(sufile[fd].type==DISK ||
	      sufile[fd].type==DEVNULL ||
	      sufile[fd].type==TAPE ||
	      sufile[fd].type==PIPE )) {
		warn(__FILE__,__LINE__,"suckfile: Illegal file: type=%s",strtype(sufile[fd].type));
	}

	if(sufile[fd].alen<1) {
		warn(__FILE__,__LINE__,"suckfile: Illegal file: alen=%d less than 1 (null termination)",
					sufile[fd].alen);
	}

	if(sufile[fd].dbpt<0) {
		warn(__FILE__,__LINE__,"suckfile: Illegal file: dbpt=%d",sufile[fd].dbpt);
	}
}

/* statfil - determine type of file from file descriptor
 *
 * Returns:
 *	filetype
 *
 * Synopsis:
 *	filetype statfil(fd)
 *	int fd;	  file descriptor
 *
 * Example:
 *	filetype ftype;
 *	...
 *	ftype = statfil(STDOUT)
 *	if (ftype = TTY) {
 *		...
 *
 * Notes:
 *
 *	The check on tapes demands the RAW interface.  This is moot
 *	and easily changed.
 *
 *	A check for DIRECTORIES was added since it doesn't
 *	cost much, but the newer items such as FIFOS and SOCKETS
 *	have been ignored, though they could be treated as was the
 *	DIRECTORY type.  If other DEVICES become of interest,
 *	they can be treated as was /dev/null.  If such new types
 *	are added, the typedef "filetype" h must be extended.
 */
filetype statfil(fd)
int fd;
{
	if (isatty(fd))		return(TTY);

	if (isadisk(fd))	return(DISK);

	if (isadir(fd))		return(DIR);

	if (isapipe(fd))	return(PIPE);

	if (isatape(fd))	return(TAPE);

	if (isadevnull(fd))	return(DEVNULL);

	warn(__FILE__,__LINE__,"statfil: couldn't determine file type");

	return(UNKNOWN);
}

/*
 * apass - pass ascii header
 */
int apass(infd,outfd)
int infd,outfd;
{
	int n;
	char c;

	if(sufile[infd].alen)
		warn(__FILE__,__LINE__,"apass: nonzero alen=%d (input)",sufile[infd].alen);

	if(outfd>=0) if(sufile[outfd].alen)
		warn(__FILE__,__LINE__,"apass: nonzero alen=%d (output)",sufile[outfd].alen);

	for(n=0;;n++) {
		if(bread(infd,&c,1)!=1) err(__FILE__,__LINE__,"apass: bread error");
		if(c==0) break;
		if(verbose)  fprintf(stderr,"%c",c);
		if(outfd>=0) bwrite(outfd,&c,1);
	}

	sufile[infd].alen = n + 1;	/* includes the null termination */
	if(outfd>=0) sufile[outfd].alen = n;	/* null termination added later */

	return(n);
}

/*
 * apass2 - pass ascii header to two files
 */
int apass2(infd,outfd1,outfd2)
int infd,outfd1,outfd2;
{
	int n;
	char c;

	if(sufile[infd].alen)
		warn(__FILE__,__LINE__,"apass2: nonzero alen=%d (input)",sufile[infd].alen);

	if(outfd1>=0) if(sufile[outfd1].alen)
		warn(__FILE__,__LINE__,"apass2: nonzero alen=%d (output1)",sufile[outfd1].alen);

	if(outfd2>=0) if(sufile[outfd2].alen)
		warn(__FILE__,__LINE__,"apass2: nonzero alen=%d (output2)",sufile[outfd2].alen);

	for(n=0;;n++) {
		if(bread(infd,&c,1)!=1) err(__FILE__,__LINE__,"apass2: bread error");
		if(c==0) break;
		if(verbose)  fprintf(stderr,"%c",c);
		if(outfd1>=0) bwrite(outfd1,&c,1);
		if(outfd2>=0) bwrite(outfd2,&c,1);
	}

	sufile[infd].alen = n + 1;	/* includes the null termination */
	if(outfd1>=0) sufile[outfd1].alen = n;	/* null termination added later */
	if(outfd2>=0) sufile[outfd2].alen = n;	/* null termination added later */

	return(n);
}

/*
 * bpass - pass binary header
 */
bpass(infd,outfd,abh)
int infd,outfd;
Subhed *abh;
{
	getbh(infd,abh);
	hislog(outfd);
	putbh(outfd,abh);
}

/*
 * getbh - read SU binary header
 */
int getbh(fd,abh)
int fd;
Subhed *abh;
{
	int nread;

	if(fd== -1) return(0);

	nread = pread(fd,abh,sizeof(*abh));

	if(abh->esize==0) err("esize=%d on input",abh->esize);

	sufile[fd].dbpt = abh->ns * abh->esize;

	suckfile(fd);

	return (nread);
}

/*
 * putbh - close ascii header and write SU binary header
 *	writes a separation line and null termination,
 *	sets the static variable sufile[fd].hclosed = true
 *	writes binary header.
 */
putbh(fd,abh)
int fd;
Subhed *abh;
{
	int nwrite;
	char z=0;

	if(fd== -1) return(0);

	if(abh->esize==0) err("esize=%d on output",abh->esize);

/* 	hispr(fd," "); */

	while(sufile[fd].left!=2)	/* LEAVE 2 FOR \n0 */
		hispr(fd," ");

	/* NEW LINE */
	hispr(fd,"\n");

	/* AND A ZERO */
	bwrite(fd,&z,1);

	/* ASCII HEADER LENGTH INCLUDES THE ZERO BYTE */
	sufile[fd].alen++;

/* 	fprintf(stderr,"%s: putbh: alen=%d\n",xargv[0],sufile[fd].alen); */

	/* HCLOSED FLAG */
	sufile[fd].hclosed = true;

	sufile[fd].dbpt = abh->ns * abh->esize;

	suckfile(fd);

	nwrite = write(fd,abh,sizeof(*abh));

	if(nwrite!=sizeof(*abh))
		err(__FILE__,__LINE__,"putbh: write error");

	return (nwrite);
}

/*
 * hislog - write a line including the command line, user name and user id
 */
/* extern char *SccsId; */
hislog(fd)
int fd;
{
	int i;
	static char *st1,*st2,*st3,*buff;
	static bool first=true;

	if(first) {
		st1 = (char*)malloc(80);
		st2 = (char*)malloc(80);
		st3 = (char*)malloc(80);
		buff= (char*)malloc(512);
		first = false;
	}

	i = time(0);
	sprintf(st1,ctime(&i));
	strtr(st1,st2,'\n',0);
	cuserid(st1);
	gethostname(st3,16);
	sprintf(buff,"%s User=%s Host=%s Pid=%d\n",st2,st1,st3,getpid());

	if(verbose) fprintf(stderr,buff);
	if(fd!= -1) sufile[fd].alen += bwrite(fd,buff,strlen(buff));

/* 	if(verbose) fprintf(stderr,SccsId); */
/* 	if(fd!= -1) sufile[fd].alen += bwrite(fd,SccsId,strlen(SccsId)); */

	for(i=0;i<xargc;i++) {
		sprintf(buff,"%s ",xargv[i]);
		if(verbose) fprintf(stderr, "%s",buff);
		if(fd!= -1) sufile[fd].alen += bwrite(fd,buff,strlen(buff));
	}

	st1 = gname(0) ; st2 = gname(1);	/* Should be infd, outfd */

	sprintf(buff,"< %s > %s \n",st1,st2);
	if(fd!= -1) sufile[fd].alen += bwrite(fd,buff,strlen(buff));
	if(verbose) fprintf(stderr, "%s",buff);
}

strtr(s1,s2,c1,c2)
char *s1,*s2;
int c1,c2;
{
	int i;
	for(i=0;s1[i]!=0;i++) {
		if(s1[i]==c1) s2[i] = c2;
		else s2[i] = s1[i];
	}
}

/*
 * hispr - print a message on the history.
 * Diagnostics:
 *	It is an error to write on a closed history.
 * Example:
 *	hispr(fd,"v=%f\n",v);
 */
/*VARARGS0*/
#include <varargs.h>
hispr(va_alist)
va_dcl
{
	int fd,nw;
	va_list args;
	char *format,buff[1600];
	static FILE *diskbuff=NULL,*tmpfile();

	va_start(args);

	fd = va_arg(args, int );

	if(sufile[fd].hclosed)
		err(__FILE__,__LINE__,"hispr: can't write ... closed history");

	format = va_arg(args, char *);

/* 	vsprintf(buff, format, args); */
/*
 * The following is a patch around the absence of vsprintf() on the Masscomp
 * Shuki Ronen 11/6/88
 */
	if(diskbuff==NULL) diskbuff = tmpfile();
	rewind(diskbuff);
	nw = _doprnt(format, args, diskbuff);
	rewind(diskbuff);
	fread(buff,1,nw,diskbuff);
	buff[nw] = (char)0;
/* End of vsprintf() patch */

	if(verbose) fprintf(stderr, buff);

	nw = bwrite(fd,buff,strlen(buff));
	if(nw!=strlen(buff)) err(__FILE__,__LINE__,"hispr: bwrite error");
	sufile[fd].alen += nw;

	va_end(args);
}

/*
 * bwrite(), bread(), bflush() - buffered I/O routines
 * All reads and writes are in blocks of BUFSZ
 */

int bwrite(fd,ptr,nb)
int fd,nb;
char *ptr;
{
	int rw=0;

/* 	if(fd== -1) return(0); */

	if(sufile[fd].first) {
		sufile[fd].first = false;
		sufile[fd].pbuf = sufile[fd].buf;
		sufile[fd].left = BUFSZ;
	}

	while(nb) {

		if(nb<sufile[fd].left) {	/* FITS INTO THE BUFFER */
			bcopy(ptr,sufile[fd].pbuf,nb);
			rw += nb;
			sufile[fd].pbuf += nb;
			sufile[fd].left -= nb;
			nb = 0;
		} else {			/* WILL FILL THE BUFFER */
			bcopy(ptr,sufile[fd].pbuf,sufile[fd].left);
			ptr += sufile[fd].left;
			rw += sufile[fd].left;
			nb -= sufile[fd].left;
			if( write(fd,sufile[fd].buf,BUFSZ) !=BUFSZ)
				err(__FILE__,__LINE__,"bwrite: write error");
			sufile[fd].left = BUFSZ;
			sufile[fd].pbuf = sufile[fd].buf;
		}
	}
	return(rw);
}

int bread(fd,ptr,nb)
int fd,nb;
char *ptr;
{
	int rr=0;

	if(sufile[fd].first || sufile[fd].left==0 ) {
		sufile[fd].first = false;
		pread(fd,sufile[fd].buf,BUFSZ);
		sufile[fd].left = BUFSZ;
		sufile[fd].pbuf = sufile[fd].buf;
	}

	while(nb) {

		if(nb<sufile[fd].left) {	/* FITS INTO THE BUFFER */
			bcopy(sufile[fd].pbuf,ptr,nb);
			rr += nb;
			sufile[fd].pbuf += nb;
			sufile[fd].left -= nb;
/* 			nb = 0; */
			return(rr);
		} else {			/* WILL FILL THE BUFFER */
			bcopy(sufile[fd].pbuf,ptr,sufile[fd].left);
			rr += sufile[fd].left;
			ptr += sufile[fd].left;
			nb -= sufile[fd].left;
			if(nb==0) {
				sufile[fd].left = 0;
				return(rr);
			}
			pread(fd,sufile[fd].buf,BUFSZ);
			sufile[fd].left = BUFSZ;
			sufile[fd].pbuf = sufile[fd].buf;
		}
	}
	warn(__FILE__,__LINE__,"bread: should never get here");
	return(-1);
}
