
#include "movie.h"

#define ITEMS   4

#define DIR	0
#define STEP	1
#define	RUN	2
#define QIT	3

/*
 * Locals:
 */

static char *msg[] = {
	"Forward","Reverse",
	"Step","Step",
	"Run","Stop",
	"Exit","EXIT"
};

static int qt[] = {880, 840, 800, 760};
static int qb[] = {860, 820, 780, 740};

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
	if (m) {
		mgihue(BLACK);
		mgibox(50,qb[i],250,qt[i]);
		mgihue(WHITE);
		mgigfs(60,qb[i]+2,0,msg[2*i+w]);
	} else {
		mgihue(WHITE);
		mgibox(50,qb[i],250,qt[i]);
		mgihue(BLACK);
		mgigfs(60,qb[i]+2,0,msg[2*i+w]);
	}

	return;
}

/*
 * PUT - put frame number
 *
 * Parameters:
 *    fr	- frame number
 */

static void put(fr)
int fr;
{

	char buf[32];

	if (fr < 0) {
		mgihue(BLUE);
		mgibox(300,80,500,100);
		return;
	}

	sprintf(buf,"Frame: %d",fr+1);
	mgihue(WHITE);
	mgibox(300,80,500,100);
	mgihue(BLACK);
	mgigfs(310,82,0,buf);

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
 * GETFRAME - get a frame from disk if required
 *
 * Parameters:
 *    i		- frame number
 */

static void getframe(fn)
int fn;
{

	static int wh = 0;
	int l = 0;

#ifdef DEBUG
printf("(Get %d)\n",fn);
#endif
	if (frame[fn].stat == MEM) return;

	/* Find a free slot */

	while (l < memframe) {
		if (mem[wh].stat == AVAIL) {
			mem[wh].stat = INUSE;
			mem[wh].who = fn;
			frame[fn].stat = MEM;
			frame[fn].adrs = mem[wh].adrs;
			++wh; if (wh >= memframe) wh = 0;
			lseek(mfd,fn*framesize,0);
			if (read(mfd,frame[fn].adrs,framesize)!=framesize) {
				fprintf(stderr,"Bad frame read.\n");
				exit(-1);
			}
			return;
		}
		++wh; if (wh >= memframe) wh = 0;
		++l;
	}

	/* No free slots. Make one */

	frame[mem[wh].who].stat = DISK;
	mem[wh].who = fn;
	frame[fn].stat = MEM;
	frame[fn].adrs = mem[wh].adrs;
	++wh; if (wh >= memframe) wh = 0;
	lseek(mfd,fn*framesize,0);
	if (read(mfd,frame[fn].adrs,framesize)!=framesize) {
		fprintf(stderr,"Bad frame read.\n");
		exit(-1);
	}
	return;
}

/*
 * MOVIE - run movie
 */

void movie()
{

	int p, q, msk, p1, p2, q1, q2;
	int fr = 0;
	int fb = 1;
	int moviedir = 1;

	/* Movie position */

	p1 = (plen[curdir] - pframe) >> 1;
	q1 = (qlen[curdir] - qframe) >> 1;
	p2 = p1 + pframe - 1;
	q2 = q1 + qframe - 1;

	/* Clear old menu and write new one */

	mgihue(BLUE);
	mgibox(0,0,300,909);

	if (moviedir == 1) mesg(DIR,0,0);
	else mesg(DIR,0,1);

	mesg(STEP,0,0);

	mesg(RUN,0,0);

	mesg(QIT,0,0);

	/* Clear drawing area */

	mgiclearpln(3,-1,0);

	getframe(fr);
	mgiv(3);
	mgifb(fb+6,3-fb);
	mgiimage(frame[fr].adrs,p1,q1,p2,q2);
	fb = 3-fb;
	mgifb(fb+6,3);
	mgiv(2);
	put(fr);

	/* Command loop */

	for(;;) {
		getbut(&p,&q,&msk);
		switch(item(p,q)) {
		case DIR:
			moviedir = -moviedir;
			if (moviedir == 1) mesg(DIR,0,0);
			else mesg(DIR,0,1);
			break;
		case STEP:
			mesg(STEP,1,0);
			fr += moviedir;
			if (fr >= nframe) fr=0;
			else if (fr < 0) fr = nframe-1;
			getframe(fr);
			mgiv(3);
			mgifb(fb+6,3-fb);
			mgiimage(frame[fr].adrs,p1,q1,p2,q2);
			fb = 3-fb;
			mgifb(fb+6,3);
			mgiv(2);
			put(fr);
			mesg(STEP,0,0);
			break;
		case RUN:
			mesg(RUN,1,0);
			put(-1);
			mgiv(3);
			onbut();
			do {
				fr += moviedir;
				if (fr >= nframe) fr=0;
				else if (fr < 0) fr = nframe-1;
				getframe(fr);
				mgifb(fb+6,3-fb);
				mgiimage(frame[fr].adrs,p1,q1,p2,q2);
				mgisyncrb(1);
				fb = 3-fb;
			} while (!checkbut());
			mgifb(fb+6,3);
			mgiv(2);
			put(fr);
			mesg(RUN,0,0);
			break;
		case QIT:
			mesg(QIT,1,1);
			put(-1);
			return;
		default:
			break;
		}
	}
}
