

#include "movie.h"

#define	GREY2	"/src/graphics/movie/grey2.ct"

/*
 * Locals:
 */

static short int curs[16] = {
	0x0100, 0x0100, 0x0100, 0x0100,
	0x0100, 0x0100, 0x0380, 0xfefe,
	0x0380, 0x0100, 0x0100, 0x0100,
	0x0100, 0x0100, 0x0100, 0x0000
};

/*
 * Getpar interface
 */

extern int xargc;
extern char **xargv;

/*
 * MAIN - movie driver
 *
 * Parameters:
 *    argc	- arg count
 *    argv	- arg strings
 */

main(argc,argv)
int argc;
char *argv[];
{

	FILE *fp;
	char *mfilnam;	/* Movie file name */
	char *cfilnam;
	int r,g,b;
	register int i;		/* Counter */

	xargc = argc;
	xargv = argv;

	/* Get input info */

	if (!sgetpar("in",&mfilnam) || !igetpar("n1",&n1) ||
	    !igetpar("n2",&n2)) {
		fprintf(stderr,"%s n1= n2= n3= ct= in=\n",argv[0]);
		exit(-1);
	}

	n3 = 1;
	igetpar("n3",&n3);
	if (!sgetpar("ct",&cfilnam)) cfilnam = GREY2;

	if (n1 < 1 || n1 > 2000 || n2 < 1 || n2 > 2000 ||
	    n3 < 1 || n3 > 2000) {
		fprintf(stderr,"Bad n1 or n2 or n3: %d %d %d\n",
			n1,n2,n3);
		exit(-1);
	}


	ifd = open(mfilnam,O_RDONLY,0777);

	if (ifd < 0) {
		fprintf(stderr,"Cant open %s.\n",mfilnam);
		exit(-1);
	}

	fp = fopen(cfilnam,"r");

	if (fp == NULL) {
		fprintf(stderr,"Can't open color table %s\n",cfilnam);
		exit(-1);
	}

	/* OK, ready to roll. Set up for graphics. */

	mgiasngp(0,0);
	mgifb(1,3);
	mgiclearpln(2,-1,0);

	/* Load font for menus */

	mgifetchgf(0,"7x9_bold");

	/* Assign movie window */

	mgidefw(3);

	/* Load color table. Planes 0-7 reserved for image */

	for (i=0;i<256;++i) {
		fscanf(fp,"%d %d %d",&r,&g,&b);
		mgicm(i,(r<<16)|(g<<8)|b);
	}

	fclose(fp);

	mgicm(BLUE,0x7f7f7f);
	mgicm(WHITE,0xffffff);
	mgicm(BLACK,0x000000);

	/* Background for run */

	mgiv(2);
	mgihue(BLUE);
	mgibox(0,0,1151,909);

	/* Set up cursor */

	mgiloadcurs(7,7,01777,curs);
	mgicursmode(07);

	/* Set up for button interrupts */

	mgibuttonint(button);

	/* Movie loop */

	while (menu()) movie();

	/* All done */

	mgicm(0,0);
	mgideagp();

	exit(0);
}
