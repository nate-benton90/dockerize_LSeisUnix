
/*
 * movie.h - includes for movie program
 */

#include <stdio.h>
#include <fcntl.h>
#include <math.h>

/* Movie area bound */

#define TOP	899
#define RIGHT	1141

/* Movie memory size 14Mb */

#define MSIZE	14680064

/* Colors */

#define BLUE	512
#define WHITE	513
#define BLACK	514

/* Frame stuff */

#define MAXFRAME	1000
#define MEM		1
#define DISK		0

typedef struct {
	char stat;
	char *adrs;
} FRAME;

extern FRAME frame[MAXFRAME];

/* Memory stuff */

#define INUSE	1
#define AVAIL	0

typedef struct {
	char stat;
	int who;
	char *adrs;
} MEMORY;

extern MEMORY mem[MAXFRAME];

/* Routines */

extern void
	movie(), onbut(), getbut();
extern int
	menu(), button(), checkbut();

/* Globals */

extern int ifd;			/* File descriptor for movie */
extern int mfd;			/* Movie file descriptor */
extern int fd[];		/* File descriptors for prepared movies */
extern int n1, n2, n3;		/* Dimensions of movie cube */
extern int curdir;		/* Current movie direction */
extern int pframe;		/* Horizontal # pixel in frame */
extern int qframe;		/* Vertical # pixel in frame */
extern int nframe;		/* Frames in movie */
extern int pmag;		/* P magnification */
extern int qmag;		/* Q magnification */
extern int framesize;		/* Size of frame in movie */
extern int memframe;		/* No. frames in memory */
extern int plen[];		/* # P in movie window */
extern int qlen[];		/* # Q in movie window */
