#include <stdio.h>
static FILE *printer;
static enum {pipe, file} output;
static enum {off,on} moveflag,drawflag,colorflag;      
static char *out;
static unsigned short sx,sy;
static short int COLOR;

/*   internal fun to paint line in pen_draw  (stroke)*/
check()
{
	if (drawflag==on){
		fprintf(printer,"stroke\n");
		drawflag=off;
		}	
	moveflag=off;
}

pen_init()
{
	if(sgetpar("out",&out)) {
		if((printer=fopen(out,"w"))==NULL) err("cant fopen(%s)",out);
		output = file;
	} else {
		if((printer=popen("lpr","w"))==NULL) err("cant popen(lpr)");
		output = pipe;
	}

	fprintf(printer,"%%!\n");
	fprintf(printer,"20 20 translate\n");
	fprintf(printer,"/inch {72 mul } def\n");  /*size of 1 point=1/72 inch */
	/* Data range from 0 to 10000    */
	fprintf(printer,"/inchx {72 1220 div mul} def\n");  /* 1220=10000/8.2 inch  */
	fprintf(printer,"/inchy {72 1220 div mul} def\n");    /* page size  */
	fprintf(printer,"0.1 setlinewidth\n");
}

pen_vpause(i)
int i;
{
	sleep(i);
}
pen_erase()
{
	check();
	sx=sy=0;
	fprintf(printer,"showpage\n");
}

pen_text(scalefont,orient,string)
char *string;
short int orient,scalefont;
{
	check();
	colorflag=off;
	fprintf(printer,"gsave\n  %d rotate\n",orient);
	fprintf(printer,"/Times-Roman findfont %d scalefont setfont\n",scalefont);
	fprintf(printer,"(%s) show\n grestore\n",string);       
}

pen_Text(scalefont,orient,string)
char *string;
short int orient,scalefont;
{
	int or;
	check();
	colorflag=off;
	fprintf(printer,"gsave\n  %d rotate\n",orient);
	fprintf(printer,"/Times-Roman findfont %d scalefont setfont\n",scalefont);
	fprintf(printer,"(%s) stringwidth pop\n",string);       
	fprintf(printer,"2 div neg 0 rmoveto\n ");       
	fprintf(printer,"(%s) show\n grestore\n",string);       
	/*or=360-orient;
	fprintf(printer," %d rotate\n",or);  */
}

pen_move(xy)
unsigned short int xy[2];
{
	if (drawflag==on){
		fprintf(printer,"stroke\n");
		drawflag=off;
		}	
	moveflag=on;
	fprintf(printer,"%d inchx %d inchy moveto\n",xy[0],xy[1]);
}

pen_fat(fat)
short int fat;
{
	check();
	fprintf(printer,"%d inch setlinewidth\n",fat);
}

pen_col(col)
short int col;
{
	check();
	colorflag=on;
	COLOR=col;
}

pen_draw(xy)
unsigned short xy[2];
{
	if ( moveflag==off && drawflag==off ){
		fprintf(printer,"%d inchx %d inchy moveto\n",sx,sy);
	}
	fprintf(printer,"%d inchx %d inchy lineto\n ",xy[0],xy[1]);
	sx=xy[0];sy=xy[1];
	moveflag=off;
	drawflag=on;
	colorflag=off;
}

pen_area(lp,ua,va) 
short lp;
unsigned short *ua,*va;
{
	int i;
	check();
	if( colorflag==on ){
		fprintf(printer,"%d 9 div setgray\n",COLOR);   /*match 9 colors */
		}
	fprintf(printer,"%d inchx %d inchy moveto\n",ua[0],va[0]);   
	for(i=1;i<lp;i++)
		fprintf(printer,"%d inchx %d inchy lineto\n",ua[i],va[i]);
	fprintf(printer,"closepath\nfill \n");
	if( colorflag==on ){
		fprintf(printer,"0 setgray\n");   
		colorflag=off;
		}
}

pen_pldone()
{
	check();
	fprintf(printer,"showpage \nquit\n");
	if(output==pipe) pclose(printer);
	if(output==file) fclose(printer);
}
