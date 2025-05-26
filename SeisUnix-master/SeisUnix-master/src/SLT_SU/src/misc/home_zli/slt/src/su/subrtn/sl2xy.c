#include "usu.h"
#include "par.h"
/* from s and l coordinates, compute x and y positions */ 
/* author: z. li	      		9/94 	*/
/*
	| y
	|   .l	      * (x4,y4)
	|    .     .    .
	|     .  .       .         . s 
       (x3,y3) *	  .      .   
	|	 .          .  . 
	|	  .          * (x2,y2)
	|	   .       .
	|	     .  .
	|	      * (x1,y1)
	|
	|    
	|
	|--------------------------------- x

given (x,y) and (s,l) of 3 corners of a rantangular region, this routine
compute (x,y) position of an input (s,l) point 

	x1,y1		smallest s and smallest l position
	x2,y2		largest s and smallest l position
	x3,y3		smallest s and largest l position
	s		shotpoint position within a 3d line
	l		line position 
*/

void sl2xy(float s1, float l1, float x1, float y1, 
	   float s2, float l2, float x2, float y2, 
	   float s3, float l3, float x3, float y3, 
	   float s,  float l,  float *x, float *y) {

	static int ifirst=1;
	static double dxds,dxdl,dyds,dydl;

	if(ifirst==1) {
		/* error checking */
		if(l1!=l2) err("l1 not the same as l2");
		if(s1!=s3) err("s1 not the same as s3");

		ifirst = 0;
		dxds = (x2-x1)/(s2-s1);
		dxdl = (x3-x1)/(l3-l1);
		dyds = (y2-y1)/(s2-s1);
		dydl = (y3-y1)/(l3-l1);
	}

	*x =  x1 + (s-s1)*dxds+(l-l1)*dxdl;
	*y =  y1 + (s-s1)*dyds+(l-l1)*dydl; 

}
