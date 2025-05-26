h52041
s 00061/00054/00077
d D 1.7 88/09/06 09:32:09 tamar 8 6
c text,color,draw
e
s 00061/00054/00077
d R 1.7 88/08/31 12:30:53 tamar 7 6
c Text,stroke,color
e
s 00033/00015/00098
d D 1.6 88/08/22 12:59:37 tamar 6 5
c option to change parameter along a trace
e
s 00019/00003/00094
d D 1.5 88/08/18 15:22:55 tamar 5 4
c change in draw
e
s 00021/00014/00076
d D 1.4 88/08/16 16:51:43 tamar 4 3
c working version (file output)
e
s 00005/00004/00085
d D 1.3 88/08/10 11:08:11 tamar 3 2
c 
e
s 00002/00002/00087
d D 1.2 88/08/09 11:52:50 tamar 2 1
c squaring the page(inch)
e
s 00089/00000/00000
d D 1.1 88/08/09 11:12:25 tamar 1 0
c date and time created 88/08/09 11:12:25 by tamar
e
u
U
f e 0
t
T
I 1
#include <stdio.h>
D 5

E 5
D 8
#define FP(x) fprintf(printer,x)
#define FP2(x,y) fprintf(printer,x,y)
#define FP3(x,y,z) fprintf(printer,x,y,z)
E 8
D 4
FILE *printer;
E 4
I 4
static FILE *printer;
static enum {pipe, file} output;
I 6
D 8
static enum {off,on} posind,index;    
E 8
I 8
static enum {off,on} moveflag,drawflag,colorflag;      
E 8
E 6
static char out[64];
I 5
D 6
static char index;
E 6
I 6
D 8
static char opt,w='w',s='s';
E 8
I 8
static unsigned short sx,sy;
static short int COLOR;
E 8
E 6
E 5
E 4

I 5
D 6
check()
E 6
I 6
D 8
check(opt)
E 8
I 8
/*   internal fun to paint line in pen_draw  (stroke)*/
check()
E 8
E 6
{
D 6
	if (index=='d'){
		FP("stroke\n");
		index=NULL;
	}
E 6
I 6
D 8
	if (index==on){
		switch(opt){
			case 's':
				FP("stroke\n");
				index=off;
				break;
			case 'w':
				posind=on;
				FP("stroke\n");
				index=off;
			 	break;	
E 8
I 8
	if (drawflag==on){
		fprintf(printer,"stroke\n");
		drawflag=off;
E 8
		}	
D 8
	}else if(opt=='s')
		posind=off;
	
E 8
I 8
	moveflag=off;
E 8
E 6
}
I 8

E 8
E 5
pen_init()
{
D 4
	if((printer=popen("lpr","w"))==NULL){
	fprintf(stderr,"cant open PRINTER");
	exit(-1);
E 4
I 4
	if(sgetpar("out",out)) {
		if((printer=fopen(out,"w"))==NULL) err("cant fopen(%s)",out);
		output = file;
	} else {
		if((printer=popen("lpr","w"))==NULL) err("cant popen(lpr)");
		output = pipe;
E 4
	}
I 4

E 4
D 8
	FP("%%!\n");
	FP("20 20 translate\n");
D 3
	FP("/inch {72 mul 20 sub} def\n");
	FP("/inchx {72 1220 div mul 20 sub} def\n");  /*for 8.2 / 11.7 inch  */
D 2
	FP("/inchy {72 855 div mul 20 sub} def\n");    /* page size  */
E 2
I 2
	FP("/inchy {72 1220 div mul 20 sub} def\n");    /* page size  */
E 3
I 3
	FP("/inch {72 mul } def\n");
E 8
I 8
	fprintf(printer,"%%!\n");
	fprintf(printer,"20 20 translate\n");
	fprintf(printer,"/inch {72 mul } def\n");  /*size of 1 point=1/72 inch */
E 8
D 4
	FP("/inchx {72 1220 div mul } def\n");  /*for 8.2 / 11.7 inch  */
	FP("/inchy {72 1220 div mul } def\n");    /* page size  */
E 4
I 4
	/* Data range from 0 to 10000    */
D 8
	FP("/inchx {72 1220 div mul} def\n");  /* 1220=10000/8.2 inch  */
	FP("/inchy {72 1220 div mul} def\n");    /* page size  */
I 5
	FP("0.1 setlinewidth\n");
E 8
I 8
	fprintf(printer,"/inchx {72 1220 div mul} def\n");  /* 1220=10000/8.2 inch  */
	fprintf(printer,"/inchy {72 1220 div mul} def\n");    /* page size  */
	fprintf(printer,"0.1 setlinewidth\n");
E 8
E 5
E 4
E 3
E 2
}

pen_vpause(i)
int i;
{
	sleep(i);
}
pen_erase()
{
I 5
D 6
	check();
E 6
I 6
D 8
	check(s);
E 6
E 5
	FP("showpage\n");
E 8
I 8
	check();
	sx=sy=0;
	fprintf(printer,"showpage\n");
E 8
}

pen_text(scalefont,orient,string)
char *string;
short int orient,scalefont;
{
I 5
D 6
	check();
E 6
I 6
D 8
	check(w);
E 6
E 5
	FP2("%d rotate\n",orient);
	FP2("/Times-Roman findfont %d scalefont setfont\n",scalefont);
D 4
	FP2("(%s) show\nnewpath\n",string);       
E 4
I 4
	FP2("(%s) show\n",string);       
E 8
I 8
	check();
	colorflag=off;
	fprintf(printer,"gsave\n %d rotate\n",orient);
	fprintf(printer,"/Times-Roman findfont %d scalefont setfont\n",scalefont);
	fprintf(printer,"(%s) show\n grestore\n",string);       
E 8
E 4
}

pen_Text(scalefont,orient,string)
char *string;
short int orient,scalefont;
{
I 5
D 6
	check();
E 6
I 6
D 8
	check(w);
E 6
E 5
D 4
/*	FP2("/Times-Roman findfont %d scalefont setfont\n",scalefont);
	FP2("(%s) show\n",string);         */
E 4
I 4
	FP2("%d rotate\n",orient);
	FP2("/Times-Roman findfont %d scalefont setfont\n",scalefont);
	FP2("(%s) show\n",string);       
E 8
I 8
	check();
	colorflag=off;
	fprintf(printer,"gsave\n %d rotate\n",orient);
	fprintf(printer,"/Times-Roman findfont %d scalefont setfont\n",scalefont);
	fprintf(printer,"(%s) stringwidth pop\n",string);       
	fprintf(printer,"2 div neg 0 rmoveto\n ");       
	fprintf(printer,"(%s) show\n grestore\n",string);       
E 8
E 4
}

pen_move(xy)
unsigned short int xy[2];
{
I 5
D 6
	check();
E 6
I 6
D 8
	check(s);
E 6
E 5
	FP3("%u inchx %u inchy moveto\n",xy[0],xy[1]);
E 8
I 8
	if (drawflag==on){
		fprintf(printer,"stroke\n");
		drawflag=off;
		}	
	moveflag=on;
	fprintf(printer,"%d inchx %d inchy moveto\n",xy[0],xy[1]);
E 8
}

pen_fat(fat)
short int fat;
{
I 5
D 6
	check();
E 6
I 6
D 8
	check(w);
E 6
E 5
	FP2("%d inch setlinewidth\n",fat);
E 8
I 8
	check();
	fprintf(printer,"%d inch setlinewidth\n",fat);
E 8
}

pen_col(col)
short int col;
{
I 5
D 6
	check();
E 6
I 6
D 8
	check(w);
E 6
E 5
D 3
	FP2("%d 9 div setgrey\n",col);
E 3
I 3
D 4
	FP2("%d 9 div setgray\n",col);
E 4
I 4
	FP2("%d 9 div setgray\n",col);    /* match 9 colors */
E 8
I 8
	check();
	colorflag=on;
	COLOR=col;
E 8
E 4
E 3
}

pen_draw(xy)
unsigned short xy[2];
{
D 5
	FP3("%d inchx %d inchy lineto\nstroke \n",xy[0],xy[1]);
	FP3("%d inchx %d inchy moveto\n",xy[0],xy[1]);
E 5
I 5
D 6
	index='d';
E 6
I 6
D 8
	unsigned short sx,sy;
	index=on;
	if (posind==on){
		FP3("%u inchx %u inchy moveto\n",sx,sy);
		posind=off;
E 8
I 8
	if ( moveflag==off && drawflag==off ){
		fprintf(printer,"%d inchx %d inchy moveto\n",sx,sy);
E 8
	}
E 6
D 8
	FP3("%d inchx %d inchy lineto\n ",xy[0],xy[1]);
E 8
I 8
	fprintf(printer,"%d inchx %d inchy lineto\n ",xy[0],xy[1]);
E 8
I 6
	sx=xy[0];sy=xy[1];
I 8
	moveflag=off;
	drawflag=on;
	colorflag=off;
E 8
E 6
E 5
}

pen_area(lp,ua,va) 
short lp;
unsigned short *ua,*va;
{
	int i;
I 5
D 6
	check();
E 6
I 6
D 8
	check(w);
E 6
E 5
I 3
D 4
	FP("newpath \n");
E 4
E 3
	FP3("%d inchx %d inchy moveto\n",ua[0],va[0]);   
E 8
I 8
	check();
	if( colorflag==on ){
		fprintf(printer,"%d 9 div setgray\n",COLOR);   /*match 9 colors */
		}
	fprintf(printer,"%d inchx %d inchy moveto\n",ua[0],va[0]);   
E 8
D 4
	for(i=1;i<lp;i++){
E 4
I 4
	for(i=1;i<lp;i++)
E 4
D 2
		FP3("%d inchx,%d inchy lineto\n",ua[i],va[i]);
E 2
I 2
D 8
		FP3("%d inchx %d inchy lineto\n",ua[i],va[i]);
E 2
D 4
	}
E 4
	FP("closepath\nfill \n");
E 8
I 8
		fprintf(printer,"%d inchx %d inchy lineto\n",ua[i],va[i]);
	fprintf(printer,"closepath\nfill \n");
	if( colorflag==on ){
		fprintf(printer,"0 setgray\n");   
		colorflag=off;
		}
E 8
}

pen_pldone()
{
I 5
D 6
	check();
E 6
I 6
D 8
	check(s);
E 6
E 5
	FP("showpage \nquit\n");
E 8
I 8
	check();
	fprintf(printer,"showpage \nquit\n");
E 8
D 4
	pclose(printer);
E 4
I 4
	if(output==pipe) pclose(printer);
	if(output==file) fclose(printer);
E 4
}
E 1
