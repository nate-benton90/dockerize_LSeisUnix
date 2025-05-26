h56572
s 00000/00000/00031
d D 1.2 88/11/15 14:19:44 shuki 2 1
c 
e
s 00031/00000/00000
d D 1.1 88/11/06 09:55:41 shuki 1 0
c date and time created 88/11/06 09:55:41 by shuki
e
u
U
t
T
I 1
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
E 1
