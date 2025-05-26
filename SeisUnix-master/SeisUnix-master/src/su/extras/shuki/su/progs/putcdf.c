#include <stdio.h>
#include "../include/su.h"


putcdf(outfd,data,cdf,vel,i)
int outfd,i;
float cdf,vel;
float *data;
{
	int ix;
	static Sutrace tr;
	static bool first=true;

	if(first) {
		tr.data = (float*)malloc(i*sizeof(float));
		first = false;
	}
	tr.offset = vel;

		bcopy((char*)(data),
			(char*)tr.data,i*sizeof(float));
		tr.offset = vel;
		tr.cdp = cdf;
		puttr(outfd,&tr);
}
