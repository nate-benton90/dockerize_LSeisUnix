#include "su.h"

float xmcor(long sx, long gx, short scalco) {
/* input:
    	sx	----	x coordinate of source
    	gx	----	x coordinate of geophone 
	scalco  ----    scale factor (in segy trace) for sx and gx
			(+- 10, 100, 1000, 10000)
   return:
        xmcor   ----    x coordinate of midpoint
   author: 
	Zhiming Li		      		7/92
*/
	float mx;
 	mx = (sx+gx)/2.; 
	if(scalco>1) {
		mx = mx * scalco; 
	} else if(scalco<0) {
		mx = mx / (-scalco); 
	}
	return mx;
}
