/* gridheader.h - include file for un-scaled grid header for programmers
 *
 * Author: zhiming li
 */ 

#ifndef USGRIDHD_H
#define USGRIDHD_H

typedef struct {	/* unscaled grid header */

	float scale;	/* scale to be applied to all the header variables */
			/* below (typical value = 0.000001), before storing*/
			/* the grid header at the end of the grid file 	*/
			/* ignored at usghed structure */
			/* used only when converting usghed form/to ghed */

	int dtype;	/* grid data type */
			/* =1	-- 1-byte char numbers */
			/* =2	-- 2-byte short (integer*2) numbers */
			/* =4	-- 4-byte floating-point numbers    */
			/* =8	-- 8-byte complex numbers	    */

	int n1;		/* number of samples of the 1st axis of the grid */  

	int n2;		/* number of samples of the 2nd axis of the grid */  

	int n3;		/* number of samples of the 3rd axis of the grid */  

	int n4;		/* number of samples of the 4th axis of the grid */  

	int n5;		/* number of samples of the 5th axis of the grid */  

	float d1;	/* coordinate increment of the 1st axis of the grid */

	float d2;	/* coordinate increment of the 2nd axis of the grid */

	float d3;	/* coordinate increment of the 3rd axis of the grid */

	float d4;	/* coordinate increment of the 4th axis of the grid */

	float d5;	/* coordinate increment of the 5th axis of the grid */

	float o1;	/* starting coordinate of the 1st axis of the grid */

	float o2;	/* starting coordinate of the 2nd axis of the grid */

	float o3;	/* starting coordinate of the 3rd axis of the grid */

	float o4;	/* starting coordinate of the 4th axis of the grid */

	float o5;	/* starting coordinate of the 5th axis of the grid */

	float dcdp2;	/* cdp number increment along the 2nd axis 	*/ 

	float dline3;	/* line number increment along the 3rd axis 	*/ 

	float ocdp2;	/* cdp number at the begin of the 2nd axis	*/ 

	float oline3;	/* line number at the begin of the 3nd axis	*/ 
	
	float gmin;	/* minimum data value of the grid 		*/ 
			/* for dtype=1,2,4	may be negative 	*/ 
			/* for dtype=8		minimum absolute value (norm)*/ 

	float gmax;	/* maximum data value of the grid 		*/ 
			/* for dtype=1,2,4	may be negative 	*/ 
			/* for dtype=8		maximum absolute value (norm)*/ 

	int orient;     /* orientation of grid axes */
			/* 0= not defined;	*/
			/* 1= 1st: time/depth; 2nd: inline; 3rd: crossline */
			/* 2= 1st: time/depth; 2nd: crossline; 3rd: inline */
			/* 3= 1st: inline; 2nd: time/depth; 3rd: crossline */
			/* 4= 1st: inline; 2nd: crossline; 3rd: time/depth */
			/* 5= 1st: crossline; 2nd: time/depth; 3rd: inline */
			/* 6= 1st: crossline; 2nd: inline; 3rd: time/depth */
	int gtype;	/* grid type 	*/
			/* 0=not defined 	*/
			/* 1=time-rms velocity	*/
			/* 2=time-average velocity	*/
			/* 3=time-interval velocity	*/
			/* 4=depth grid	*/

} usghed;


#endif
