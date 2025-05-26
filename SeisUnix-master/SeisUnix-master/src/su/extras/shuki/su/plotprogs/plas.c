#include <stdio.h>
#define SCALE 0.001
#define L 80
char **xargv,*sdoc="[in= ]\n";
char *SccsId="@(#)plas.c	1.2 5/29/88\n";
char *gname();
int xargc;
enum {false,true} verbose=true;
main(ac,av)
int ac;
char **av;
{
	int size,orient;
	char line[L],str[L],*in,*out,*buf;
	float x,y;

/* 	char *malloc(),*realloc(); */
	float *xp,*yp;
	int lp,color,fat,nlp=0,i;

	FILE *infd;

	xargc = ac; xargv = av;

	if (xargc>1) if (*xargv[1]=='-') selfdoc();

	/* OPEN INPUT FILE */
	if(sgetpar("in",&in)) {	/* in= specified */
		infd = fopen(line,"r");
		if(infd==NULL) err(__FILE__,__LINE__,"can't open %s\n",line);
	} else if (xargc>1) {		/* Maybe xargv[1] */
		infd = fopen(xargv[1],"r");
		if(infd==NULL) {
			err(__FILE__,__LINE__,"can't open %s\n",xargv[1]);
		}
	} else {
		infd = stdin;
		buf = gname(0);
		fprintf(stderr,"infile: %s\n",buf);
	}

	/* OUTPUT FILE */
	if(sgetpar("out",&out)) {	/* out= specified */
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
				sscanf(line,"%*s %f %f %d %d %*s",
						&x,&y,&size,&orient);
				str_cat(str,line);
				text(SCALE*x,SCALE*y,size,orient,str);
				break;
			case 'T':
				sscanf(line,"%*s %f %f %d %d %*s",
						&x,&y,&size,&orient);
				str_cat(str,line);
				Text(SCALE*x,SCALE*y,size,orient,str);
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
				break;
		}
	}
	exit(0);
}
str_cat(s,t)  /* copy from the 5th string in t to s  */
char *s,*t;
{
        int i,j;
        for(i=0,j=0;j<=3;i++)
		if((t[i]==' '||t[i]=='\t')&&t[i+1]!=' '&&t[i+1]!='\t')
                        j++; 
        j=0 ;
        while((s[j++]=t[i++])!='\0')   /* copy end of t  */
        ;
}
