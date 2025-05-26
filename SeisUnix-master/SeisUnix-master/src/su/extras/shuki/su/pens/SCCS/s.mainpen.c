h54659
s 00014/00014/00127
d D 1.4 88/11/15 14:07:30 shuki 5 4
c 
e
s 00004/00004/00137
d D 1.3 88/08/09 10:44:27 tamar 4 2
c change pen_area
e
s 00002/00002/00139
d R 1.3 88/08/09 09:03:19 tamar 3 2
c change pen_area
e
s 00001/00000/00140
d D 1.2 88/05/29 07:01:19 shuki 2 1
c SccsId
e
s 00140/00000/00000
d D 1.1 88/04/14 13:59:29 shuki 1 0
c date and time created 88/04/14 13:59:29 by shuki
e
u
U
f e 0
t
T
I 1
#include <stdio.h>

/*
 * tpen - vplot command interpreter for tektronix
 */

#define MAXPOLY 256
I 5
#define NTEXT 64
E 5

int xargc;
char **xargv;
char *sdoc="SDOC\n";
I 2
D 4
char *SccsId="%G%  %W%\n";
E 4
I 4
char *SccsId="8/9/88  @(#)mainpen.c	1.3\n";
E 4
E 2
D 5
enum {false,true} verbose=true;
E 5
I 5
typedef enum {false,true} bool;
bool verbose=true;
E 5

main(argc,argv)
int argc;
char *argv[];
{
	FILE *vpf;
D 5
	char *sbp;
E 5
I 5
	char sbp[NTEXT];
E 5
	unsigned short xy[2];
	short int j,size,orient,lp;
D 5
	int code, c;
E 5
I 5
	int code;
E 5
	register int i;
D 4
	float ua[MAXPOLY],va[MAXPOLY];
E 4
I 4
	unsigned short ua[MAXPOLY],va[MAXPOLY];
E 4
D 5
	char sbuf[128];
	enum {false,true} first=true;
E 5
I 5
	char *sbuf;
	bool first=true;
E 5

	xargc = argc; xargv = argv;
/*
	if (argc > 2) {
		printf("Usage: %s vplot_command_file\n",argv[0]);
		exit(0);
	}

	if (argc == 1)
		vpf = stdin;
	else {
		if ((vpf = fopen(argv[1],"r")) == NULL) {
			printf("%s : Cant open %s \n",argv[0],argv[1]);
			exit(1);
		}
	}
*/
	if (!isatty(0)) {
		vpf = stdin;
D 5
	} else if(sgetpar("in",sbuf)) {
E 5
I 5
	} else if(sgetpar("in",&sbuf)) {
E 5
		vpf = fopen(sbuf,"r");
		if(vpf==NULL) err(__FILE__,__LINE__,"can't fopen in=%s\n",sbuf);
	} else if(xargc>1) {
		vpf = fopen(xargv[1],"r");
		if(vpf==NULL) err(__FILE__,__LINE__,"can't fopen %s\n",xargv[1]);
	} else {
		err(__FILE__,__LINE__,"vplot_command_file\n");
	}

	while ((code = getc(vpf)) != EOF) {

		/*
		 * pen_init is INSIDE the main loop to make
		 * sure the screen is clear when plotting starts
		 * (important when piping to the pen program)
		 */
		if(first==true) {
			pen_init();
			first = false;
		}

		switch(code) {

			case 's':
				fread(&j,2,1,vpf);
				pen_vpause(j);

			case 'e':
				pen_erase();
				break;

			case 'm':
				fread(xy,2,2,vpf);
				pen_move(xy);
				break;

			case 'd':
				fread(xy,2,2,vpf);
				pen_draw(xy);
				break;

			case 'c':
				fread(&j,2,1,vpf);
				pen_col(j);
				break;

			case 'f':
				fread(&j,2,1,vpf);
				pen_fat(j);
				break;
			
			case 'T':
				fread(&size,2,1,vpf);
				fread(&orient,2,1,vpf);
D 5
				sbp = sbuf;
				while((c = getc(vpf))) *sbp++ = (char) c;
				*sbp++ = 0;
				pen_Text(size,orient,sbuf);
E 5
I 5
				for(i=0;i<NTEXT&&(sbp[i]=getc(vpf));i++);
				sbp[i] = 0;
				pen_Text(size,orient,sbp);
E 5
				break;
			
			case 't':
				fread(&size,2,1,vpf);
				fread(&orient,2,1,vpf);
D 5
				sbp = sbuf;
				while((c = getc(vpf))) *sbp++ = (char) c;
				*sbp++ = 0;
				pen_text(size,orient,sbuf);
E 5
I 5
				for(i=0;i<NTEXT&&(sbp[i]=getc(vpf));i++);
				sbp[i] = 0;
				pen_text(size,orient,sbp);
E 5
				break;
			
			case 'a':
				fread(&lp,2,1,vpf);
/* 				if(lp>MAXPOLY)
				  fprintf(stderr,
				  "Can't fill a polygon with than %d corners (maximum is %d)\n",
				  lp,MAXPOLY); */
				for (i=0;i<lp;++i) {
					pfread(xy,2,2,vpf);
D 4
					ua[i] = (float) xy[0] / 1000.0;
					va[i] = (float) xy[1] / 1000.0;
E 4
I 4
					ua[i] = xy[0] ;
					va[i] = xy[1] ;
E 4
				}
				if(lp<MAXPOLY) pen_area(lp,ua,va);
				break;

			default:
				pen_pldone();
				err(__FILE__,__LINE__,"Illegal code: %c\n",(char)(code&0177));
		}
	}

	pen_pldone();
	fflush(stdout);

	exit(0);
}
E 1
