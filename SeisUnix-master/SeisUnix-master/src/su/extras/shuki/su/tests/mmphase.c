/*	SUBROUTINE:	mmphase			*/

/* DESTINATION:					*/
/* min/max-phase wavelet calculation		*/
/*			 t			*/
/*          wlet(t)=(rad) * cos(2pi*freq*t) 	*/

/* ENTRANCE:					*/
/*	rad	abs(rad) >1 max-phase walelet	*/
/*			 <1 min			*/ 
/*	freq	signal frequency (hertz)	*/
/*	dt	sampling interval(sec)		*/
/*	len	wavelet length (sec)		*/

/* RESULT:					*/
/*	wlet	min/max-wavelet 		*/

#include <math.h>
/* #define pi 3.14159265 */
mmphase(rad,freq,dt,len,wlet)
float rad,freq,dt,len,*wlet;
{
	int ns,is;
	float r1,r2;

	ns = len/dt + 1.5;
	r1 = 2.0 * M_PI * freq * dt;
	wlet[0] = 1.0;
	for(is=1,r2=rad;is<ns;is++,r2 *= rad)
		wlet[is] = r2*cos(is*r1);
}
