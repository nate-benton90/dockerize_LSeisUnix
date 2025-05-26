h56277
s 00008/00006/00103
d D 1.8 88/11/15 14:08:45 shuki 8 7
c 
e
s 00003/00003/00106
d D 1.7 88/08/24 16:01:03 rafi 7 6
c improve str_cat to be more jeneral
e
s 00015/00002/00094
d D 1.6 88/08/24 14:22:19 evgeny 6 5
c alowed to write text that composed of several strings
e
s 00001/00001/00095
d D 1.5 88/08/23 09:48:26 rafi 5 4
c correct case "t"
e
s 00001/00001/00095
d D 1.4 88/08/09 11:00:07 rafi 4 3
c 
e
s 00039/00001/00057
d D 1.3 88/08/09 10:10:19 tamar 3 2
c case :a c f T
e
s 00001/00000/00057
d D 1.2 88/05/29 07:23:33 shuki 2 1
c SccsId
e
s 00057/00000/00000
d D 1.1 88/04/14 13:57:00 shuki 1 0
c date and time created 88/04/14 13:57:00 by shuki
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
#define SCALE 0.001
#define L 80
char **xargv,*sdoc="[in= ]\n";
I 2
D 3
char *SccsId="%W% %G%\n";
E 3
I 3
char *SccsId="@(#)plas.c	1.2 5/29/88\n";
I 8
char *gname();
E 8
E 3
E 2
int xargc;
enum {false,true} verbose=true;
main(ac,av)
int ac;
char **av;
{
	int size,orient;
D 8
	char line[L],str[L];
E 8
I 8
	char line[L],str[L],*in,*out,*buf;
E 8
	float x,y;
I 3

D 8
	char *malloc(),*realloc();
E 8
I 8
/* 	char *malloc(),*realloc(); */
E 8
	float *xp,*yp;
	int lp,color,fat,nlp=0,i;

E 3
	FILE *infd;

	xargc = ac; xargv = av;

D 4
	if (*xargv[1]=='-') selfdoc();
E 4
I 4
	if (xargc>1) if (*xargv[1]=='-') selfdoc();
E 4

	/* OPEN INPUT FILE */
D 8
	if(sgetpar("in",line)) {	/* in= specified */
E 8
I 8
	if(sgetpar("in",&in)) {	/* in= specified */
E 8
		infd = fopen(line,"r");
		if(infd==NULL) err(__FILE__,__LINE__,"can't open %s\n",line);
	} else if (xargc>1) {		/* Maybe xargv[1] */
		infd = fopen(xargv[1],"r");
		if(infd==NULL) {
			err(__FILE__,__LINE__,"can't open %s\n",xargv[1]);
		}
	} else {
		infd = stdin;
D 8
		gname(0,line);
		fprintf(stderr,"infile: %s\n",line);
E 8
I 8
		buf = gname(0);
		fprintf(stderr,"infile: %s\n",buf);
E 8
	}

	/* OUTPUT FILE */
D 8
	if(sgetpar("out",line)) {	/* out= specified */
E 8
I 8
	if(sgetpar("out",&out)) {	/* out= specified */
E 8
		setfn(line);
	}

	while(fgets(line,L,infd)!=NULL) {
		switch(*line) {
			case 'm':
				sscanf(line,"%*s %f %f",&x,&y);
				move(SCALE*x,SCALE*y);
				break;
			case 'd':
				sscanf(line,"%*s %f %f",&x,&y);
				draw(SCALE*x,SCALE*y);
				break;
			case 't':
D 6
				sscanf(line,"%*s %f %f %d %d %s",
E 6
I 6
				sscanf(line,"%*s %f %f %d %d %*s",
E 6
D 7
						&x,&y,&size,&orient,str);
E 7
I 7
						&x,&y,&size,&orient);
E 7
I 6
				str_cat(str,line);
E 6
				text(SCALE*x,SCALE*y,size,orient,str);
I 3
				break;
			case 'T':
D 6
				sscanf(line,"%*s %f %f %d %d %s",
E 6
I 6
				sscanf(line,"%*s %f %f %d %d %*s",
E 6
D 7
						&x,&y,&size,&orient,str);
E 7
I 7
						&x,&y,&size,&orient);
E 7
I 6
				str_cat(str,line);
E 6
D 5
				text(SCALE*x,SCALE*y,size,orient,str);
E 5
I 5
				Text(SCALE*x,SCALE*y,size,orient,str);
E 5
				break;
			case 'a':
				sscanf(line,"%*s %d",&lp);
				if(nlp==0){
					xp=(float *)malloc(lp*sizeof(float));
					yp=(float *)malloc(lp*sizeof(float));
					nlp=lp;
				}
				else if(nlp<lp){
					xp=(float *)realloc(lp*sizeof(float));
					yp=(float *)realloc(lp*sizeof(float));
					nlp=lp;
				}
				for(i=0;i<lp;i++){
					fgets(line,L,infd);
					sscanf(line,"%f %f",&xp[i],&yp[i]);
					xp[i]*=SCALE;
					yp[i]*=SCALE;
				}
				area(xp,yp,lp);
				break;
			case 'c':
				sscanf(line,"%*s %d",&color);
				setcol(color);
				break;
			case 'f':
				sscanf(line,"%*s %d",&fat);
				setfat(fat);
E 3
				break;
		}
	}
I 8
	exit(0);
E 8
I 6
}
str_cat(s,t)  /* copy from the 5th string in t to s  */
char *s,*t;
{
        int i,j;
        for(i=0,j=0;j<=3;i++)
D 7
                if(t[i]==' ')
E 7
I 7
		if((t[i]==' '||t[i]=='\t')&&t[i+1]!=' '&&t[i+1]!='\t')
E 7
                        j++; 
        j=0 ;
        while((s[j++]=t[i++])!='\0')   /* copy end of t  */
        ;
E 6
}
E 1
