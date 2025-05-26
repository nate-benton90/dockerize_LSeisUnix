/*

LICENSE FOR BHP SU Suite of Programs

The following is the license that applies to the copy of the software hereby
provided to Licensee. BHP's Software Manager may be contacted at the following
address:

Colorado School of Mines
1500 Illinois Street
Golden, Colorado 80401
Attention: John Stockwell
e-mail: john@dix.mines.edu
Telephone: 303-273-3049

Copyright 2001 BHP Software. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software") to deal
in the Software, without restriction, except as hereinafter provided,
including without limitation the rights to use, copy, modify merge,
publish, and distribute the Software and to permit persons
to whom the Software is furnished to do so, provided that the above
copyright notice and this permission notice appear in all copies of the
Software and that both the above copyright notice and this permission
notice appear in supporting documentation. No charge may be made for
any redistribution of the Software, including modified or merged versions
of the Software. The complete source code must be included
in any distribution. For an executable file, complete source code means the
source code for all modules it contains.

Modified or merged versions of the Software must be provided to the Software
Manager, regardless of whether such modified or merged versions are
distributed to others.

THE SOFTWARE IS PROVIDED 'AS IS" WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE
COPYRIGHT HOLDER INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM OR
ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OF PERFORMANCE OF
THIS SOFTWARE.

The name of the copyright holder shall not be used in advertising or
otherwise to promote the use or other dealings in this Software, without
prior written consent of the copyright holder.

*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include "su.h"
#include "segy.h"
#include "bhp_hdr.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
" BHPBIN - assign inline/xline numbers to traces                   ",
"                                                                  ",
"  BHPBIN takes as parameters the x,y coordinates and associated   ",
"  inline,xline numbers for any three corner points of a 3D grid,  ",
"  calculates the inline/xline numbers of input traces from their  ",
"  x,y coordinates, and loads the inline/xline numbers to trace    ",
"  headers.                                                        ",
"                                                                  ",
" bhpbin c1=x1,y1,il1,xl1 c2=x2,y2,il2,xl2 c3=x3,y3,il3,xl3 ilhdr=fldr xlhdr=cdp xhdr=sx yhdr=sy",
"                                                                  ",
"                                                                  ",
" Required Parameters:                                             ",
"  none                                                            ",
" Optional Parameters:                                             ",
"    c1=1000,1000,1,1       Corner1 - xcoord,ycoord,iline,xline    ",
"    c2=1000,2000,1,200     Corner2 - xcoord,ycoord,iline,xline    ",
"    c3=2000,2000,200,200   Corner3 - xcoord,ycoord,iline,xline    ",
"    ilhdr=fldr             Header to use as inline                ",
"    xlhdr=cdp              Header to use as xline                 ",
"    xhdr=sx                Header to use as xcoord                ",
"    yhdr=sy                Header to use as ycoord                ",
"    verbose=0              For debug print                        ",
"                                                                  ",
NULL};

/* Globals */
segy tr;                                       /* Trace */
float iline_len, xline_len;                    /* Length of iline and xline */
float iline_x1, iline_x2, iline_y1, iline_y2;  /* iline end points */
float xline_x1, xline_x2, xline_y1, xline_y2;  /* xline end points */
int il1, il2, xl1, xl2;                        /* inline, xline limits */

/* Prototypes */
float distanceToEdge (float x1, float y1, float x2, float y2, float x, float y);
void set_lines(int line, float *c1, float *c2, float dist);

int main(int argc, char **argv)
{

  char *ilhdr;    /* Output iline header */
  char *xlhdr;    /* Output xline header */
  char *xhdr;     /* Input x header */
  char *yhdr;     /* Input y header */

  int verbose;                 /* debug printout */
  int i, j;                    /* Loop counters */
  int indx, indy;              /* Header indices - x,y */
  int indil, indxl;            /* Header indices - il,xl */
  int iline, xline;            /* il,xl to header */
  int c1c2=0, c1c3=0, c2c3=0;  /* Set to 1 if a line */

  float c1[4], c2[4], c3[4];                     /* Corners - x,y,il,xl */
  float x, y;                                    /* x,y from header */
  float minx, maxx, miny, maxy;                  /* Min, max X,Y */
  float c1c2d, c1c3d, c2c3d;                     /* Distances between pairs of points */
  float disti, distx;                            /* distance from x,y to iline, xline */
  float frac;                                    /* Fractional distance along iline or xline */

  cwp_String xtype, ytype, iltype, xltype;   /* Header types - x,y il,xl */

  Value hval;  /* Header value */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  if(getparfloat("c1",c1)) {
    if((countparval("c1")) != 4)
      err("c1 must contain 4 items: x,y,il,xl\n");
  }
  else {
    c1[0] = 1000.;
    c1[1] = 1000.;
    c1[2] = 1;
    c1[3] = 1;
  }

  if(getparfloat("c2",c2)) {
    if(countparval("c2") != 4)
      err("c2 must contain 4 items: x,y,il,xl\n");
  }
  else {
    c2[0] = 1000.;
    c2[1] = 2000.;
    c2[2] = 1;
    c2[3] = 200;
  }

  if(getparfloat("c3",c3)) {
    if(countparval("c3") != 4)
      err("c3 must contain 4 items: x,y,il,xl\n");
  }
  else {
    c3[0] = 2000.;
    c3[1] = 2000.;
    c3[2] = 200;
    c3[3] = 200;
  }

  if(!getparstring("ilhdr",&ilhdr))
    ilhdr = "fldr";
  if(!getparstring("xlhdr",&xlhdr))
    xlhdr = "cdp";
  if(!getparstring("xhdr",&xhdr))
    xhdr = "sx";
  if(!getparstring("yhdr",&yhdr))
    yhdr = "sy";

  if(verbose) {
    fprintf(stderr,"Corner 1: X: %f  Y: %f\n", c1[0],c1[1]);
    fprintf(stderr,"         IL: %f XL: %f\n", c1[2],c1[3]);
    fprintf(stderr,"Corner 2: X: %f  Y: %f\n", c2[0],c2[1]);
    fprintf(stderr,"         IL: %f XL: %f\n", c2[2],c2[3]);
    fprintf(stderr,"Corner 3: X: %f  Y: %f\n", c3[0],c3[1]);
    fprintf(stderr,"         IL: %f XL: %f\n", c3[2],c3[3]);
    fprintf(stderr,"Headers: IL: %s  XL: %s\n", ilhdr,xlhdr);
    fprintf(stderr,"Headers: X: %s  Y: %s\n", xhdr,yhdr);
  }

  /* Compute distance of each possible line - c1 to c2, c1 to c3, and c2 to c3 */
  c1c2d = sqrt( ((c2[0] - c1[0])*(c2[0] - c1[0])) + ((c2[1] - c1[1])*(c2[1] - c1[1])) );
  c1c3d = sqrt( ((c3[0] - c1[0])*(c3[0] - c1[0])) + ((c3[1] - c1[1])*(c3[1] - c1[1])) );
  c2c3d = sqrt( ((c3[0] - c2[0])*(c3[0] - c2[0])) + ((c3[1] - c2[1])*(c3[1] - c2[1])) );

  /* The two shortest distances are the connected lines */
  if(c1c2d > c1c3d && c1c2d > c2c3d) {
    c1c3 = 1;
    c2c3 = 1;
  }
  else if(c1c3d > c1c2d && c1c3d > c2c3d) {
    c1c2 = 1;
    c2c3 = 1;
  }
  else if(c2c3d > c1c2d && c2c3d > c1c3d) {
    c1c2 = 1;
    c1c3 = 1;
  }

  if(verbose) {
    fprintf(stderr,"C1 to C2 = %f\n", c1c2d);
    fprintf(stderr,"C1 to C3 = %f\n", c1c3d);
    fprintf(stderr,"C2 to C3 = %f\n", c2c3d);
    if(c1c2 && c2c3)
      fprintf(stderr,"Lines are c1 --> c2 and c2 --> c3\n");
    if(c1c2 && c1c3)
      fprintf(stderr,"Lines are c1 --> c2 and c1 --> c3\n");
    if(c2c3 && c1c3)
      fprintf(stderr,"Lines are c2 --> c3 and c1 --> c3\n");
  }

  /* Determine iline, xline and set end points */
  set_lines(c1c2,c1,c2,c1c2d);
  set_lines(c1c3,c1,c3,c1c3d);
  set_lines(c2c3,c2,c3,c2c3d);

  /* Find ending il, xl */
  if(c1[2] != il1)
    il2 = c1[2];
  else if(c2[2] != il1)
    il2 = c2[2];
  else if(c3[2] != il1)
    il2 = c3[2];
  if(c1[3] != xl1)
    xl2 = c1[3];
  else if(c2[3] != xl1)
    xl2 = c2[3];
  else if(c3[3] != xl1)
    xl2 = c3[3];

  if(verbose) {
    fprintf(stderr,"ILINE LEN = %f, XLINE_LEN = %f\n", iline_len,xline_len);
    fprintf(stderr,"ILINE END POINTS: %f, %f  --> %f, %f\n", iline_x1,iline_y1,iline_x2,iline_y2);
    fprintf(stderr,"XLINE END POINTS: %f, %f  --> %f, %f\n", xline_x1,xline_y1,xline_x2,xline_y2);
    fprintf(stderr,"IL1,IL2 %d, %d\n", il1,il2);
    fprintf(stderr,"XL1,XL2 %d, %d\n", xl1,xl2);
  }

  /* Header indices, etc */
  indx = getindex(xhdr);
  indy = getindex(yhdr);
  indil = getindex(ilhdr);
  indxl = getindex(xlhdr);
  xtype = hdtype(xhdr);  
  ytype = hdtype(yhdr);  
  iltype = hdtype(ilhdr);  
  xltype = hdtype(xlhdr);  

  if(!gettr(&tr))
    err("Can't get first trace");
  i = 9000;
  do {
    gethval(&tr,indx,&hval);
    x = vtof(xtype,hval);
    scalhdr(&tr,xhdr,&x,LOAD);
    gethval(&tr,indy,&hval);
    y = vtof(ytype,hval);
    scalhdr(&tr,yhdr,&y,LOAD);
/*
    x = 16250;
    y = i;
    i+=100;
*/
    disti = distanceToEdge(iline_x1,iline_y1,iline_x2,iline_y2,x,y);
    frac = disti / xline_len;
    iline = il1 + frac * (il2 - il1);
    distx = distanceToEdge(xline_x1,xline_y1,xline_x2,xline_y2,x,y);
    frac = distx / iline_len;
    xline = xl1 + frac * (xl2 - xl1);
    if(verbose)
      fprintf(stderr,"x,y,disti,distx,iline,xline %f, %f, %f, %f, %d, %d\n",
              x,y,disti,distx,iline,xline);
    hval.i = iline;
    puthval(&tr,indil,&hval);
    hval.i = xline;
    puthval(&tr,indxl,&hval);
    puttr(&tr);
  } while(gettr(&tr));

  return EXIT_SUCCESS;

}


/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */


/*********************** self documentation **********************/
/*****************************************************************************
DTE - Distance to Edge

distanceToEdge - return distance to edge from specified (x,y) coordinates

******************************************************************************
Function Prototype:
float distanceToEdge (float x1, float y1, float x2, float y2, float x, float y);
******************************************************************************
distanceToEdge:
Input:
x1,x2,y1,y2     edge to which distance is to be computed
x		x-coordinate
y		y-coordinate

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 09/11/90
******************************************************************************/
/**************** end self doc ********************************/


float distanceToEdge (float x1, float y1, float x2, float y2, float x, float y)
/*****************************************************************************
distanceToEdge - return distance to edge from specified (x,y) coordinates
******************************************************************************
Input:
x1,x2,y1,y2     edge to which distance is to be computed
x		x-coordinate
y		y-coordinate

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 09/11/90
******************************************************************************/
{
	float x1mx,y1my,x2mx1,y2my1,t,xfac,yfac;

	/* compute distance */
	x1mx = x1-x;  y1my = y1-y;
	x2mx1 = x2-x1;  y2my1 = y2-y1;
	t = -(x1mx*x2mx1+y1my*y2my1)/(x2mx1*x2mx1+y2my1*y2my1);
	if (t<0.0) t = 0.0;
	if (t>1.0) t = 1.0;
	xfac = x1mx+t*x2mx1;
	yfac = y1my+t*y2my1;
	return sqrt(xfac*xfac+yfac*yfac);
}

void set_lines(int line, float *c1, float *c2, float dist)

{

  if(line && c1[2] == c2[2]) {
    iline_len = dist;
    iline_x1 = c1[0];
    iline_x2 = c2[0];
    iline_y1 = c1[1];
    iline_y2 = c2[1];
    il1 = c1[2];      /* Base iline */
  }
  if(line && c1[3] == c2[3]) {
    xline_len = dist;
    xline_x1 = c1[0];
    xline_x2 = c2[0];
    xline_y1 = c1[1];
    xline_y2 = c2[1];
    xl1 = c1[3];       /* Base xline */
  }

}
