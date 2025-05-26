h50000
s 00000/00000/00330
d D 1.5 88/11/15 14:02:52 shuki 5 4
c 
e
s 00008/00008/00322
d D 1.4 88/06/06 13:12:21 shuki 4 3
c Cancel ns in trace headers
e
s 00002/00000/00328
d D 1.3 88/05/25 14:53:56 shemer 3 2
c with SccsId[]
e
s 00001/00001/00327
d D 1.2 88/04/20 10:05:32 shuki 2 1
c valcmp
e
s 00328/00000/00000
d D 1.1 88/04/14 13:52:42 shuki 1 0
c date and time created 88/04/14 13:52:42 by shuki
e
u
U
f e 0
t
T
I 1
/*
 * suedit - examine segy trace files
 */

#include <fcntl.h>
#include <stdio.h>
#include "../include/su.h"
/* #include "../include/hdrs.h" */
/* #include <strings.h> */
#include <fcntl.h>
/* #include <unistd.h> */

int xargc; char **xargv;
bool verbose=true;
I 3
char *SccsId[]="%W%\t%G%\n";

E 3
char *sdoc =
"suedit < diskfile						\n\
	Interactive program for inspection of segy diskfiles.	\n\
								\n\
	The following commands are recognized:			\n\
 	number	read in trace and print nonzero header words	\n\
	+	read in next trace and print header		\n\
	-	read in previous trace and print header		\n\
	s	print selected quantiles for trace in memory	\n\
	p [n1 [n2]]  plot sample n1 to n2			\n\
	g [tr1 tr2]  graph the current trace [traces tr1 to tr2]\n\
	f	plot the Fourier transform of the current trace	\n\
	dN	advance N traces and print the header		\n\
	! key=val  modify field in current header		\n\
	$	print the last trace				\n\
	?	print online documentation			\n\
	q	quit						\n\
								\n";

FILE *tty ,*fopen(), *freopen();
Sutrace tr;
I 4
Subhed bh;
E 4
int itr,*ai,j,nss;
#define LINELEN 201
#define NAMELEN 32
char cline[LINELEN],*fgets(),name[NAMELEN],tmpname[NAMELEN];
int infd;
int compar();
int nbh;

main(ac,av)
int ac;
char **av;
{
	int dtr=1;
D 4
	Subhed bh;
E 4

	xargc = ac ; xargv = av ;

	tty = fopen("/dev/tty","r");

	nbh = NTRHB;

	if(ac > 1) { /* place diskfile on standard input */
		close(0);
		if(-1 == (infd=suopen(av[1],O_RDWR))) {
		    if(-1 == (infd=suopen(av[1],O_RDONLY))) {
			perror(av[0]);
			err(__FILE__,__LINE__,"unable to open input file %s\n", av[1]);
			}
		    else
			warn(__FILE__,__LINE__,"file is read only.  Header modification will not be possible.");
		}
		strcpy(name,av[1]);
	} else {
		infd = input();
		strcpy(name,"STDIN");
	}

	apass(infd,-1);
	getbh(infd,&bh);

D 2
	fprintf(stdout,"Binary Heaser:\n");
E 2
I 2
	fprintf(stdout,"Binary Header:\n");
E 2
	bhpr(stdout,&bh);

	nss = bh.ns ;
	tr.data = (float*) malloc(sizeof(float)*nss);
	ai = ( int * ) malloc(sizeof(int)*nss);

	fprintf(stdout,"%d traces in %s\n",getntr(infd),name);

	itr = 0;
	if(gettra(infd,&tr,itr)) {
		fprintf(stdout,"TRACE %d:\n",itr);
		hdpr(stdout,&tr);
	} else err(__FILE__,__LINE__,"can't gettra %d\n",itr);

	fprintf(stdout,"> "); fflush(stdout);

	while(NULL !=  fgets(cline,LINELEN-1,tty))
	{
		if( *cline <= '9' && *cline >= '0' ) {
			itr = atoi(cline);
			if(gettra(infd,&tr,itr)) {
				fprintf(stdout,"TRACE %d:\n",itr);
				hdpr(stdout,&tr);
			} else warn(__FILE__,__LINE__,"can't gettra %d\n",itr);
		} else if ( *cline == 'q' ) {
			break ;
		} else if ( *cline == 'p' ) {
			plot() ;
		} else if ( *cline == 'g' )  {
			graph();
		} else if ( *cline == 'f' )  {
			ftplot();
		} else if ( *cline == '+' ) {
			itr++;
			if(gettra(infd,&tr,itr)) {
				fprintf(stdout,"TRACE %d:\n",itr);
				hdpr(stdout,&tr);
			} else warn(__FILE__,__LINE__,"can't gettra %d\n",itr);
			dtr = 1;
		} else if ( *cline == '-' ) {
			itr--;
			if(gettra(infd,&tr,itr)) {
				fprintf(stdout,"TRACE %d:\n",itr);
				hdpr(stdout,&tr);
			} else warn(__FILE__,__LINE__,"can't gettra %d\n",itr);
			dtr = -1;
		} else if ( (strlen(cline)==1) ) {
			itr+=dtr;
			if(gettra(infd,&tr,itr)) {
				fprintf(stdout,"TRACE %d:\n",itr);
				hdpr(stdout,&tr);
			} else warn(__FILE__,__LINE__,"can't gettra %d\n",itr);
		} else if ( *cline == 's' ) {
			qsort( (char*)(tr.data), nss, sizeof(float), compar );
/* 			sortfl(tr.data,ai,nss); */
/* 			conver(tr.data,ai,nss); */
			j = 0 ;
			fprintf(stdout," quant[ %d ] = %8.2e",j,tr.data[ai[j]]);
			j = nss / 20 ;
			fprintf(stdout," quant[ %d ] = %8.2e",j,tr.data[ai[j]]);
			j = nss/2 - j ;
			fprintf(stdout," quant[ %d ] = %8.2e",j,tr.data[ai[j]]);
			fprintf(stdout,"\n");
			j = nss - 1 - j ;
			fprintf(stdout," quant[ %d ] = %8.2e",j,tr.data[ai[j]]);
			j = nss -1 - nss/20 ;
			fprintf(stdout," quant[ %d ] = %8.2e",j,tr.data[ai[j]]);
			j = nss -1 ;
			fprintf(stdout," quant[ %d ] = %8.2e",j,tr.data[ai[j]]);
			fprintf(stdout,"\nmin is at sample %d, max at %d\n"
				,ai[0],ai[j]);
		} else if ( *cline == 'd' ) {
			dtr = atoi(cline+1);
			itr+=dtr;
			if(gettra(infd,&tr,itr)) {
				fprintf(stdout,"TRACE %d:\n",itr);
				hdpr(stdout,&tr);
			} else warn(__FILE__,__LINE__,"can't gettra %d\n",itr);
		} else if (*cline == '!' ) {
			keymod();
		} else if ( *cline == '$' ) {
			itr = getntr(infd)-1;
			if(gettra(infd,&tr,itr)) {
				fprintf(stdout,"TRACE %d:\n",itr);
				hdpr(stdout,&tr);
			} else warn(__FILE__,__LINE__,"can't gettra %d\n",itr);
		} else if ( *cline == '?' ) {
			fprintf(stdout,"%s\n",sdoc);
		} else {
			fprintf(stderr,"unknown key %s\n%s\n",cline,sdoc);
		}
		fprintf(stdout,"> ");
		fflush(stdout);
	}
	if(strlen(tmpname)) unlink(tmpname);
	exit(0);
}

static kchelp()
{
	fprintf(tty,"command error: format is \"! key=val\"\n");
}

extern char *index();

keymod()
{
	char *keyword, *keyval, *ptr;
	int ikey, nb, rc;
	value val;

	for(keyword=cline+1; (*keyword) == ' '; keyword++);

	if(((char *) NULL) == (keyval = index(keyword,'='))) {
		kchelp(); return;
	}
	for(ptr=keyval-1; (*ptr) == ' '; ptr--);
	(*(ptr+1)) = '\0';
	if(0 == (ikey = getindex(keyword))) {
		kchelp(); return;
	}

	keyval++;
	val.l = atol(keyval);
	puthval(&tr,ikey,&val);
	nb = nss*sizeof(float) + nbh;
	rc = lseek(0,(long) (-nb),1/*L_INCR*/);
	if(rc == -1) {
		perror(xargv[0]);
		fprintf(stdout,
		"lseek error. unable to update trace header on disk.\n");
		return;
	}
	rc = write(0,(char *) (&tr), nbh);
	if(rc < 0) {
		perror(xargv[0]);
		fprintf(stdout,
		"write error. unable to update trace header on disk.\n");
		return;
	}
	 if(rc != nbh) {
		fprintf(stdout,
		"write error. problem updating trace header on disk.\n");
	}
	lseek(0,(long) (nb-rc),1); /* resync ? */

	fprintf(stdout,"TRACE %d:\n",itr); hdpr(stdout,&tr);
}

graph()
{
	int i,i1,i2,n1,n2;
	char *mktemp(),comm[100];
	static fd=0;

	if(!fd) {	/* SHOULD BE WITH POPEN */
		sprintf(tmpname,"/tmp/snXXXXXX");
		mktemp(tmpname);
		fd = creat(tmpname,0666);
		if(fd == -1) err(__FILE__,__LINE__,"can't create %s\n",tmpname);
	}

	lseek(fd,0,0);

	i = sscanf(cline+1,"%d %d",&i1,&i2);

/* fprintf(stderr,"i=%d i1=%d i2=%d itr=%d\n",i,i1,i2,itr); */

	if(i<2) {
D 4
		write(fd,tr.data,tr.ns*4);
E 4
I 4
		write(fd,tr.data,bh.ns*4);
E 4
		i1 = 0; i2 = i1 + 1;
	} else {
		if(i1>i2) {
			for(i=i1;i>=i2;i--) {
				gettra(infd,&tr,&i);
D 4
				write(fd,tr.data,tr.ns*4);
E 4
I 4
				write(fd,tr.data,bh.ns*4);
E 4
			}
		} else {
			for(i=i1;i<=i2;i++) {
				gettra(infd,&tr,&i);
D 4
				write(fd,tr.data,tr.ns*4);
E 4
I 4
				write(fd,tr.data,bh.ns*4);
E 4
			}
		}
	}
D 4
	n1 = tr.ns;
E 4
I 4
	n1 = bh.ns;
E 4
	n2 = abs(i2-i1)+1;
	sprintf(comm,"graph in=%s title=tracr=%d n1=%d d1=%f n2=%d | tpen",
D 4
		tmpname,tr.tracr,n1,(float)tr.dt/1000000.,n2);
E 4
I 4
		tmpname,tr.tracr,n1,(float)bh.dt/1000000.,n2);
E 4
	fprintf(stderr,"%s\n",comm);
	scanf("%c",&n1);
	system(comm);
}

ftplot()
{
	char *mktemp(),comm[100];
	static fd=0;

	if(!fd) {
		sprintf(tmpname,"/tmp/snXXXXXX");
		mktemp(tmpname);
		fd = creat(tmpname,0666);
		if(fd == -1) err(__FILE__,__LINE__,"can't create %s\n",tmpname);
	}
	lseek(fd,0,0);
D 4
	write(fd,tr.data,tr.ns*4);
E 4
I 4
	write(fd,tr.data,bh.ns*4);
E 4

	sprintf(comm,"ftplot <%s title=tracr=%d nt=%d dt=%f | tube",
D 4
		tmpname,tr.tracr,tr.ns,(float)tr.dt/1000000.0);
E 4
I 4
		tmpname,tr.tracr,bh.ns,(float)bh.dt/1000000.0);
E 4
	system(comm);
}

plot()
{
	static p0,p1 ;
	float max,min,scal,val,jj;
	int ii ,is,ip,it;

	if ( p0 >= nss -1 ) p0 = 0 ;
	ii = sscanf(cline+1,"%d %d",&p0,&p1 );
	if(ii < 2) p1 = p0 + 19;
	if( p1 >= nss ) p1 = nss - 1;
	if( p1 < p0) return;
	max = min = tr.data[p0];
	for ( ii = p0 ; ii <= p1 ; ii++ )
	{
		val = tr.data[ii];
		if(min > val ) min = val;
		if(max < val ) max = val;
	}
	if( min != max ) scal = 60./(max-min);
		else scal = 0;
	for (ii = p0 ; ii <= p1 ; ii++) {
		fprintf(stdout,"%5d %11.4e",ii,tr.data[ii]);
		ip = 1 +( 0.5 + scal*(tr.data[ii] - min ) );
		it = ip >> 3;
		is = ip & 7;
		for(jj = 0 ; jj < it; jj++ ) 	fputc('\t',stdout);
		for(jj = 0 ; jj < is; jj++ ) 	fputc(' ',stdout);
		fputc('*',stdout);
		fputc('\n',stdout);
	}
	p0 = p1;
}

int compar(a,b)
float *a,*b;
{
	if(*a>*b)	return(1);
	if(*a<*b)	return(-1);
			return(0);
}
E 1
