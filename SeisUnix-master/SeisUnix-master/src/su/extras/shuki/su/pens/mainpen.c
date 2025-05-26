#include <stdio.h>

/*
 * tpen - vplot command interpreter for tektronix
 */

#define MAXPOLY 256
#define NTEXT 64

int xargc;
char **xargv;
char *sdoc="SDOC\n";
char *SccsId="8/9/88  @(#)mainpen.c	1.3\n";
typedef enum {false,true} bool;
bool verbose=true;

main(argc,argv)
int argc;
char *argv[];
{
	FILE *vpf;
	char sbp[NTEXT];
	unsigned short xy[2];
	short int j,size,orient,lp;
	int code;
	register int i;
	unsigned short ua[MAXPOLY],va[MAXPOLY];
	char *sbuf;
	bool first=true;

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
	} else if(sgetpar("in",&sbuf)) {
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
				for(i=0;i<NTEXT&&(sbp[i]=getc(vpf));i++);
				sbp[i] = 0;
				pen_Text(size,orient,sbp);
				break;
			
			case 't':
				fread(&size,2,1,vpf);
				fread(&orient,2,1,vpf);
				for(i=0;i<NTEXT&&(sbp[i]=getc(vpf));i++);
				sbp[i] = 0;
				pen_text(size,orient,sbp);
				break;
			
			case 'a':
				fread(&lp,2,1,vpf);
/* 				if(lp>MAXPOLY)
				  fprintf(stderr,
				  "Can't fill a polygon with than %d corners (maximum is %d)\n",
				  lp,MAXPOLY); */
				for (i=0;i<lp;++i) {
					pfread(xy,2,2,vpf);
					ua[i] = xy[0] ;
					va[i] = xy[1] ;
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
