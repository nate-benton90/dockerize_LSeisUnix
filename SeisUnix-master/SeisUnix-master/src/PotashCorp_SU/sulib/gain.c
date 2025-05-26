#include <stdio.h>
#include <math.h>
#include "suhdr.h"

void do_agc(float *data, int iwagc, int nt)
/*
Simple agc routine
inputs:
	data - array
	iwagc - length of agc window in samples
	nt - length of the array
*/

{
        register int i;
        register float val;
        register float sum;
        register int nwin;
        register float rms;
	float *agcdata;


	agcdata=ealloc1float(nt);

        /* compute initial window for first datum */
        sum = 0.0;
        for (i = 0; i < iwagc+1; ++i) {
                val = data[i];
                sum += val*val;
        }
        nwin = 2*iwagc+1;
        rms = sum/nwin;
        agcdata[0] = (rms <= 0.0) ? 0.0 : data[0]/sqrt(rms);

	/* ramping on */
        for (i = 1; i <= iwagc; ++i) {
                val = data[i+iwagc];
                sum += val*val;
                ++nwin;
                rms = sum/nwin;
                agcdata[i] = (rms <= 0.0) ? 0.0 : data[i]/sqrt(rms);
        }

        /* middle range -- full rms window */
        for (i = iwagc + 1; i <= nt-1-iwagc; ++i) {
                val = data[i+iwagc];
                sum += val*val;
                val = data[i-iwagc];
                sum -= val*val; /* rounding could make sum negative! */
                rms = sum/nwin;
                agcdata[i] = (rms <= 0.0) ? 0.0 : data[i]/sqrt(rms);
        }

        /* ramping off */
        for (i = nt - iwagc; i <= nt-1; ++i) {
                val = data[i-iwagc];
                sum -= val*val; /* rounding could make sum negative! */
                --nwin;
                rms = sum/nwin;
                agcdata[i] = (rms <= 0.0) ? 0.0 : data[i]/sqrt(rms);
        }
        
	/* copy data back into trace */
        memcpy( (void *) data, (const void *) agcdata, nt*FSIZE);

        return;
}	

void do_agc_scale(float *data, int iwagc,float *scale, int nt)
{
 
 /* the same as above except the data is not gained,
    but the gain values returned in array scale */
        register int i;
        register float val;
        register float sum;
        register int nwin;
        register float rms;


        /* compute initial window for first datum */
        sum = 0.0;
        for (i = 0; i < iwagc+1; ++i) {
                val = data[i];
                sum += val*val;
        }
        nwin = 2*iwagc+1;
        rms = sum/nwin;
        scale[0] = (rms <= 0.0) ? 0.0 : sqrt(rms);

	/* ramping on */
        for (i = 1; i <= iwagc; ++i) {
                val = data[i+iwagc];
                sum += val*val;
                ++nwin;
                rms = sum/nwin;
                scale[i] = (rms <= 0.0) ? 0.0 : sqrt(rms);
        }

        /* middle range -- full rms window */
        for (i = iwagc + 1; i <= nt-1-iwagc; ++i) {
                val = data[i+iwagc];
                sum += val*val;
                val = data[i-iwagc];
                sum -= val*val; /* rounding could make sum negative! */
                rms = sum/nwin;
                scale[i] = (rms <= 0.0) ? 0.0 : sqrt(rms);
        }

        /* ramping off */
        for (i = nt - iwagc; i <= nt-1; ++i) {
                val = data[i-iwagc];
                sum -= val*val; /* rounding could make sum negative! */
                --nwin;
                rms = sum/nwin;
                scale[i] = (rms <= 0.0) ? 0.0 : sqrt(rms);
        }

        return;
}	
