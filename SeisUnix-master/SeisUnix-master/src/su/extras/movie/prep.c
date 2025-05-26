
#include "movie.h"

/*
 * Locals:
 */

#define ITEMS   7

#define SIZ	0
#define DEFSIZ	1
#define DIR3	2
#define	DIR2	3
#define DIR1	4
#define	RUN	5
#define QIT	6

static char *msg[] = {
	"Movie size","Enter",
	"Default size","Default size",
	"Direction 3","Processing",
	"Direction 2","Processing",
	"Direction 1","Processing",
	"Run movie","Error",
	"Quit","QUIT"
};

static int qt[] = {880, 840, 800, 760, 720, 680, 640};
static int qb[] = {860, 820, 780, 740, 700, 660, 620};

static char *ibuf;	/* Buffer for input */
static char *base;	/* Base of frame memory */
static int made=0;	/* Any areas allocated yet? */
static int isize;	/* Input buffer size */

			/* This direction already prepared */
static int prep[] = {0, 0, 0, 0};

			/* This direction size set */
static int set[] = {0, 0, 0, 0};

			/* Names for movie files */
static char *filnm[4] = {
	"",
	"/usr/tmp/MOVIE1",
	"/usr/tmp/MOVIE2",
	"/usr/tmp/MOVIE3"
};

/*
 * SPREAD3 - spread out movie in "3" direction
 *           Here, input frame number is same as output frame no.
 */

static void spread3()
{
	register int p;
	register char *ib, *ob, *nb, c;
	int q, i, k;

	lseek(ifd,0,0);

	if (pmag > 0) {

		/* P magnification > 0 */

		for(i=0;i<n3;++i) {
			if (read(ifd,ibuf,isize) != isize) {
				fprintf(stderr,"Read error.\n");
				exit(-1);
			}

			ib = ibuf;
			ob = base;

			q = 0;
			while (q < n2) {

				/* Byte duplication */

				p = n1;
				while (p--) {
					c = *ib++;
					switch(pmag) {
					case 25: *ob++ = c;
					case 24: *ob++ = c;
					case 23: *ob++ = c;
					case 22: *ob++ = c;
					case 21: *ob++ = c;
					case 20: *ob++ = c;
					case 19: *ob++ = c;
					case 18: *ob++ = c;
					case 17: *ob++ = c;
					case 16: *ob++ = c;
					case 15: *ob++ = c;
					case 14: *ob++ = c;
					case 13: *ob++ = c;
					case 12: *ob++ = c;
					case 11: *ob++ = c;
					case 10: *ob++ = c;
					case  9: *ob++ = c;
					case  8: *ob++ = c;
					case  7: *ob++ = c;
					case  6: *ob++ = c;
					case  5: *ob++ = c;
					case  4: *ob++ = c;
					case  3: *ob++ = c;
					case  2: *ob++ = c;
					case  1: *ob++ = c;
						break;
					default:
						printf(stderr,"Whoops (spread3)\n");
						exit(-1);
					}
				}

				++q;

				/* Row duplication */

				if (qmag > 1) {
					nb = ob - pframe;
					k = qmag-1;
					while (k--) {
						p = pframe;
						while (p--) *ob++ = *nb++;
					}
				} else if (qmag < 0) {
					q -= qmag+1;
					ib += n1 * (-qmag-1);
				}
			}

			write(mfd,base,framesize);
			frame[i].stat = DISK;
		}

	} else {

		/* P magnification < 0 */

		for(i=0;i<n3;++i) {
			if (read(ifd,ibuf,isize) != isize) {
				fprintf(stderr,"Read error.\n");
				exit(-1);
			}

			ib = ibuf;
			ob = base;

			q = 0;
			while (q < n2) {
				p = 0;
				while (p < n1) {
					c = ib[p];
					*ob++ = c;
					p -= pmag;
				}
				ib += n1;

				++q;
					
				/* Row duplication */

				if (qmag > 1) {
					nb = ob - pframe;
					p = (qmag-1) * pframe;
					while (p--) *ob++ = *nb++;
				} else if (qmag < 0) {
					q -= qmag+1;
					ib += n1 * (-qmag-1);
				}
			}

			write(mfd,base,framesize);
			frame[i].stat = DISK;
		}
	}

	return;
}

/*
 * SPREAD2 - spread out movie in "2" direction
 */

static void spread2()
{
	register char *nb, *ib, *ob, c;
	register int p;

	int q, fbase, t, f, inc1, inc2;
	char *inc3;

	if (qmag < 0) inc2 = pframe;
	else inc2 = pframe * qmag;

	/* Loop over frame block */

	for (fbase=0;fbase<nframe;fbase+=memframe) {

		inc1 = framesize - inc2;
		t = fbase+memframe-1;
		if (t > nframe-1) t = nframe-1;

#ifdef DEBUG
printf("(Processing frames %d thru %d)\n",fbase+1,t+1);
#endif
		/* Loop over Q in frame (input frames) */

		if (qmag > 0) lseek(ifd,0,0);

		q = n3-1;
		while (q >= 0) {
			
			if (qmag < 0) lseek(ifd,(n3-q-1)*isize,0);

			if (read(ifd,ibuf,isize) != isize) {
				fprintf(stderr,"Read error\n");
				exit(-1);
			}

			/* Loop over frames in block (input q) */

			ib = ibuf + fbase*n1;
			inc3 = base;

			for (f=fbase;f<=t;++f) {

				ob = inc3+inc1;

				/* Loop over P in frame */

				if (pmag > 0) {
					p = n1;
					while (p--) {
						c = *ib++;
						switch(pmag) {
						case 25: *ob++ = c;
						case 24: *ob++ = c;
						case 23: *ob++ = c;
						case 22: *ob++ = c;
						case 21: *ob++ = c;
						case 20: *ob++ = c;
						case 19: *ob++ = c;
						case 18: *ob++ = c;
						case 17: *ob++ = c;
						case 16: *ob++ = c;
						case 15: *ob++ = c;
						case 14: *ob++ = c;
						case 13: *ob++ = c;
						case 12: *ob++ = c;
						case 11: *ob++ = c;
						case 10: *ob++ = c;
						case  9: *ob++ = c;
						case  8: *ob++ = c;
						case  7: *ob++ = c;
						case  6: *ob++ = c;
						case  5: *ob++ = c;
						case  4: *ob++ = c;
						case  3: *ob++ = c;
						case  2: *ob++ = c;
						case  1: *ob++ = c;
							break;
						default:
							printf(stderr,"Whoops (spread3)\n");
							exit(-1);
						}
					}
				} else {
					p = 0;
					while (p < n1) {
						c = ib[p];
						*ob++ = c;
						p -= pmag;
					}
					ib += n1;
				}

				/* Row duplication */

				if (qmag > 1) {
					nb = ob - pframe;
					p = (qmag-1) * pframe;
					while (p--) *ob++ = *nb++;
				}

				inc3 += framesize;
			}

			/* Update q */

			if (qmag > 0) --q;
			else q += qmag;

			inc1 -= inc2;
		}

		/* Write completed block of frames to disk */

		write(mfd,base,(t-fbase+1)*framesize);

	}

	for (f=0;f<nframe;++f) frame[f].stat = DISK;

	return;
}

/*
 * SPREAD1 - spread out movie in "1" direction
 */

static void spread1()
{

	register char *ib, *ob, *nb, c;

	int fbase, f, t, p, q, l;
	int inc1, inc2;
	char *inc3;

	if (pmag > 0) inc1 = pmag;
	else inc1 = 1;

	/* Loop over frame block */

	for (fbase=0;fbase<nframe;fbase+=memframe) {

		t = fbase+memframe-1;
		if (t > nframe-1) t = nframe-1;

#ifdef DEBUG
printf("(Processing frames %d thru %d)\n",fbase+1,t+1);
#endif
		/* Loop over P in frame (input frames) */

		if (pmag > 0) {
			lseek(ifd,0,0);

			inc2 = pframe - inc1;

			for (p=n3-1;p>=0;--p) {
			
				if (read(ifd,ibuf,isize) != isize) {
					fprintf(stderr,"Read error\n");
					exit(-1);
				}

				/* Loop over frames in block (input P) */

				inc3 = base;

				for (f=fbase;f<=t;++f) {
					ib = ibuf+f;
					ob = inc3 + inc2;

					q = 0;
					while (q<n2) {
						nb = ob;
						c = *ib;
						switch(pmag) {
						case 25: *nb++ = c;
						case 24: *nb++ = c;
						case 23: *nb++ = c;
						case 22: *nb++ = c;
						case 21: *nb++ = c;
						case 20: *nb++ = c;
						case 19: *nb++ = c;
						case 18: *nb++ = c;
						case 17: *nb++ = c;
						case 16: *nb++ = c;
						case 15: *nb++ = c;
						case 14: *nb++ = c;
						case 13: *nb++ = c;
						case 12: *nb++ = c;
						case 11: *nb++ = c;
						case 10: *nb++ = c;
						case  9: *nb++ = c;
						case  8: *nb++ = c;
						case  7: *nb++ = c;
						case  6: *nb++ = c;
						case  5: *nb++ = c;
						case  4: *nb++ = c;
						case  3: *nb++ = c;
						case  2: *nb++ = c;
						case  1: *nb++ = c;
							break;
						default:
							fprintf(stderr,"Whoops (spread1)\n");
						}

						ob += pframe;
						++q;

						if (qmag > 1) {
							ib += n1;
							l = qmag-1;
							while (l--) {
							   nb = ob;
							   switch(pmag) {
							   case 25: *nb++ = c;
							   case 24: *nb++ = c;
							   case 23: *nb++ = c;
							   case 22: *nb++ = c;
							   case 21: *nb++ = c;
							   case 20: *nb++ = c;
							   case 19: *nb++ = c;
							   case 18: *nb++ = c;
							   case 17: *nb++ = c;
							   case 16: *nb++ = c;
							   case 15: *nb++ = c;
							   case 14: *nb++ = c;
							   case 13: *nb++ = c;
							   case 12: *nb++ = c;
							   case 11: *nb++ = c;
							   case 10: *nb++ = c;
							   case  9: *nb++ = c;
							   case  8: *nb++ = c;
							   case  7: *nb++ = c;
							   case  6: *nb++ = c;
							   case  5: *nb++ = c;
							   case  4: *nb++ = c;
							   case  3: *nb++ = c;
							   case  2: *nb++ = c;
							   case  1: *nb++ = c;
								   break;
							   default:
								   fprintf(stderr,"Whoops\n");
								   exit(-1);
							   }
							   ob += pframe;
							}
						} else if (qmag == 1) {
							ib += n1;
						} else if (qmag < 0) {
							q -= qmag+1;
							ib += n1 * (-qmag-1);
						}
					}

					inc3 += framesize;
				}

				inc2 -= inc1;
			}
		} else {

			inc2 = pframe - inc1;

			for (p=n3-1;p>=0;p+=pmag) {
				lseek(ifd,(n3-1-p)*isize,0);
				if (read(ifd,ibuf,isize) != isize) {
					fprintf(stderr,"Read error.\n");
					exit(-1);
				}

				/* Loop over frames in block (input P) */

				inc3 = base;

				for (f=fbase;f<=t;++f) {
					ib = ibuf+f;
					ob = inc3 + inc2;

					q = 0;
					while (q<n2) {
						c = *ib;
						*ob = c;
						ob += pframe;
						++q;
						if (qmag > 1) {
							ib += n1;
							l = qmag-1;
							while (l--) {
								*ob = c;
								ob += pframe;
							}
						} else if (qmag == 1) {
							ib += n1;
						} else if (qmag < 0) {
							q -= qmag+1;
							ib += n1 * (-qmag-1);
						}
					}

					inc3 += framesize;
				}

			inc2 -= inc1;
			}
		}

		/* Write completed block of frames to disk */

		write(mfd,base,(t-fbase+1)*framesize);

	}

	for (f=0;f<nframe;++f) frame[f].stat = DISK;

	return;
}



/*
 * MESG - display menu item
 *
 * Parameters:
 *    i		- item number
 *    m		- mode
 *    w		- which message
 */

static void mesg(i,m,w)
int i,m,w;
{
	if (m == 1) {
		mgihue(BLACK);
		mgibox(50,qb[i],250,qt[i]);
		mgihue(WHITE);
		mgigfs(60,qb[i]+2,0,msg[2*i+w]);
	} else if (m == 0) {
		mgihue(WHITE);
		mgibox(50,qb[i],250,qt[i]);
		mgihue(BLACK);
		mgigfs(60,qb[i]+2,0,msg[2*i+w]);
	} else {
		mgihue(BLUE);
		mgibox(50,qb[i],250,qt[i]);
	}

	return;
}

/*
 * ITEM - determine item selected
 *
 * Parameters:
 *    p, q	- point on screen
 */

static int item(p,q)
int p,q;
{
	int i;

	if (p < 50 || p > 250) return(-1);

	for (i=0;i<ITEMS;++i)
		if (q >= qb[i] && q <= qt[i]) return(i);

	return(-1);
}


/*
 * DODIR - handle direction
 *
 * Parameter:
 *    dir	- direction
 */

static void dodir(dir)
int dir;
{

	int p, q, msk;
	register int i;

	/* Set size */

	if (!set[dir]) {
		mesg(SIZ,0,0);
		mesg(DEFSIZ,0,0);
		while (!set[dir]) {
			getbut(&p,&q,&msk);
			switch(item(p,q)) {
			case DEFSIZ:
				switch(dir) {
				case 1:
					plen[1] = n3+10;
					qlen[1] = n2+10;
					break;
				case 2:
					plen[2] = n1+10;
					qlen[2] = n3+10;
					break;
				case 3:
					plen[3] = n1+10;
					qlen[3] = n2+10;
					break;
				}
				if (plen[dir] < 170) plen[dir] = 170;
				if (plen[dir] > 800) plen[dir] = 800;
				if (qlen[dir] < 170) qlen[dir] = 170;
				if (qlen[dir] > 800) qlen[dir] = 800;
				mesg(DEFSIZ,1,0);
				mesg(SIZ,-1,0);
				set[dir] = 1;
				break;
			case SIZ:
				mesg(SIZ,1,1);
				getbut(&p,&q,&msk);
				plen[dir] = RIGHT - p +1;
				qlen[dir] = TOP - q +1;

				if (plen[dir] < 50 || plen[dir] > 800 ||
				    qlen[dir] < 50 || qlen[dir] > 800) {
					mesg(SIZ,0,0);
					break;
				}

				mesg(SIZ,1,0);
				mesg(DEFSIZ,-1,0);
				set[dir] = 2;
				break;
			default:
				break;
			}
		}
	}
	
	mgihue(BLUE);
	mgibox(300,100,1151,909);
	p = ((RIGHT-300-plen[dir]) >> 1) + 300;
	q = ((TOP-100-qlen[dir]) >> 1) + 100;
	mgipw(3,2,p,q,p+plen[dir]-1,q+qlen[dir]-1);
	mgiclearpln(3,-1,0);

	switch(curdir) {
	case 0:
		break;
	case 1:
		mesg(DIR1,0,0);
		break;
	case 2:
		mesg(DIR2,0,0);
		break;
	case 3:
		mesg(DIR3,0,0);
		break;
	}

	curdir = dir;

	switch(curdir) {
	case 1:
		mesg(DIR1,1,1);
		break;
	case 2:
		mesg(DIR2,1,1);
		break;
	case 3:
		mesg(DIR3,1,1);
		break;
	}

	/* Deallocate old area */

	if (made) {
		free(base);
		free(ibuf);
		made = 0;
	}

	/* Prepare stuff for new area and frames */

	switch (curdir) {
	case 1:
		if (plen[1] >= n3) pmag = plen[1] / n3;
		else pmag = -(n3+plen[1]-1) / plen[1];
		if (pmag > 25) pmag = 25;
		if (qlen[1] >= n2) qmag = qlen[1] / n2;
		else qmag = -(n2+qlen[1]-1) / qlen[1];
		pframe = (pmag > 0) ? pmag * n3 : -(n3-pmag-1) / pmag;
		qframe = (qmag > 0) ? qmag * n2 : -(n2-qmag-1) / qmag;
		nframe = n1;
		break;
	case 2:
		if (plen[2] >= n1) pmag = plen[2] / n1;
		else pmag = -(n1+plen[2]-1) / plen[2];
		if (pmag > 25) pmag = 25;
		if (qlen[2] >= n3) qmag = qlen[2] / n3;
		else qmag = -(n3+qlen[2]-1) / qlen[2];
		pframe = (pmag > 0) ? pmag * n1 : -(n1-pmag-1) / pmag;
		qframe = (qmag > 0) ? qmag * n3 : -(n3-qmag-1) / qmag;
		nframe = n2;
		break;
	case 3:
		if (plen[3] >= n1) pmag = plen[3] / n1;
		else pmag = -(n1+plen[3]-1) / plen[3];
		if (pmag > 25) pmag = 25;
		if (qlen[3] >= n2) qmag = qlen[3] / n2;
		else qmag = -(n2+qlen[3]-1) / qlen[3];
		pframe = (pmag > 0) ? pmag * n1 : -(n1-pmag-1) / pmag;
		qframe = (qmag > 0) ? qmag * n2 : -(n2-qmag-1) / qmag;
		nframe = n3;
		break;
	}

	framesize = pframe*qframe;
	memframe = MSIZE / framesize;
	if (memframe > nframe) memframe = nframe;

	if (nframe > MAXFRAME) {
		fprintf(stderr,"Too many frames. (alloc)\n");
		exit(-1);
	}

	isize = n1 * n2;
	ibuf = (char *) malloc(isize);
	if (!ibuf) {
		fprintf(stderr,"Cant allocate input buffer\n");
		exit(-1);
	}

	/* Make movie area */

	base = (char *) malloc(framesize*memframe);

	if (!base) {
		fprintf(stderr,"Cant allocate movie memory\n");
		exit(-1);
	}

	/* Set memory */

	for (i=0;i<memframe;++i) {
		mem[i].stat = AVAIL;
		mem[i].adrs = base + i*framesize;
	}

	made = 1;

	/* Prepare movie if necessary */

	if (prep[curdir]) {
		mfd = fd[curdir];
		for (i=0;i<nframe;++i) frame[i].stat = DISK;
	} else {

		/* Open movie file (Think about O_CTG) */

		unlink(filnm[curdir]);
		mfd = open(filnm[curdir],O_RDWR|O_CREAT,0700);

		if (mfd < 0) {
			fprintf(stderr,"Cant open movie file %s\n",filnm[dir]);
			exit(-1);
		}

		fd[curdir] = mfd;

		switch(dir) {
		case 1:
			spread1();
			break;
		case 2:
			spread2();
			break;
		case 3:
			spread3();
			break;
		}

		prep[curdir] = 1;
	}

	/* Final message */

	switch(curdir) {
	case 1:
		mesg(DIR1,1,0);
		break;
	case 2:
		mesg(DIR2,1,0);
		break;
	case 3:
		mesg(DIR3,1,0);
		break;
	}

	return;
}
	
	
/*
 * MENU - input parameters for movie
 *        button version
 */

int menu()
{

	int p,q,msk;

	/* Set up menu */

	mgihue(BLUE);
	mgibox(0,0,300,909);

	if (curdir == 3) mesg(DIR3,1,0);
	else mesg(DIR3,0,0);

	if (curdir == 2) mesg(DIR2,1,0);
	else mesg(DIR2,0,0);

	if (curdir == 1) mesg(DIR1,1,0);
	else mesg(DIR1,0,0);

	mesg(RUN,0,0);
	mesg(QIT,0,0);

	/* Get something from button */

	for (;;) {
		getbut(&p,&q,&msk);
	
		switch(item(p,q)) {
		case DIR1:
			dodir(1);
			break;
		case DIR2:
			dodir(2);
			break;
		case DIR3:
			dodir(3);
			break;
		case RUN:
			if (!curdir || !set[curdir]) {
				mesg(RUN,1,1);
				sleep(1);
				mesg(RUN,0,0);
			} else {
				mesg(RUN,1,0);
				return(1);
			}
			break;
		case QIT:
			mesg(QIT,1,1);
			return(0);
		default:
			break;
		}
	
	}
}
