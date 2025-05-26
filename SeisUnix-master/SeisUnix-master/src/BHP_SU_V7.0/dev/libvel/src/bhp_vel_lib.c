
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
#include "su.h"
#include "segy.h"
#include "header.h"
#include "bhp_vel.h"

void dtot(int verbose, segy *trin, float *trinr, segy *trout, float *troutr, segy *vin, float *vinr,
          float *tin, float *times, int ntinv, int ntind, int ntout, float dz, float dt, float *rt)
{

  /* time/depth intervals are assumed to be in seconds/meters/feet */
  /* delrt is assumed to be in milliseconds for time, or millimeters/feet for depth */

  int i;

  /* Oversample */
  resamp(vin->data,vinr,dz,dz/M,ntinv,M*ntinv,rt);
  resamp(trin->data,trinr,dz,dz/M,ntind,M*ntind,rt);

  /* First sample is same as input */
  troutr[0] = trinr[0];
      
  /* Map depths to time */
  tin[0] = 2.0 * 0.001 * vin->delrt / vin->data[0];
  for(i=1; i<M*ntinv; i++)
    tin[i] = tin[i-1] + 2.0 * ((dz/M) / vinr[i-1]);

  /* Copy headers, set dt, ns */
  memcpy((void *)trout,(const void *)trin,HDRBYTES);
  trout->dt = dt * 1000000.;
  trout->ns = ntout;

  /* Interpolate */
  int4(M*ntind,tin,trinr,trinr[0],trinr[M*ntind-1],M*ntout,times,troutr);

  /* Undersample */
  resamp(troutr,trout->data,dt/M,dt,M*ntout,ntout,rt);

}

void ttod(int verbose, segy *trin, float *trinr, segy *trout, float *troutr, segy *vin, float *vinr,
          float *din, float *depths, int ntinv, int ntind, int ntout, float dt, float dz, float *rt)
{
  
  /* time/depth intervals are assumed to be in seconds/meters/feet */
  /* delrt is assumed to be in milliseconds for time, or millimeters/feet for depth */

  int i;

  float vt, vavg;
 
  resamp(vin->data,vinr,dt,dt/M,ntinv,M*ntinv,rt);
  resamp(trin->data,trinr,dt,dt/M,ntind,M*ntind,rt);

  /* First sample is same as input */
  troutr[0] = trinr[0];

  /* Map times to depth */
  din[0] = 0.5 * 0.001 * vin->delrt * vin->data[0];
  for(i=1; i<M*ntinv; i++)
    din[i] = din[i-1] + 0.5 * (dt / M) * vinr[i-1];

  /* Copy headers, set dz, ns */
  memcpy((void *)trout,(const void *)trin,HDRBYTES);
  trout->dt = (int)dz * 1000;
  trout->ns = ntout;

  /* Interpolate */
  int4(M*ntind,din,trinr,trinr[0],trinr[M*ntind-1],M*ntout,depths,troutr);

  /* Undersample */
  resamp(troutr,trout->data,dz/M,dz,M*ntout,ntout,rt);

}

void ttor(int verbose, segy *vin, segy *vout, int ntout)
{

  int i;

  float t;
  float sum;
  float dt;

  dt = vin->dt * 0.001;
  sum = vin->delrt * vin->data[0] * vin->data[0];
  vout->data[0] = vin->data[0];
  for(i=1,t=vin->delrt+dt; i<vin->ns; i++,t+=dt) {
    sum += dt * vin->data[i] * vin->data[i-1];
    vout->data[i] = sqrt(sum/t);
  }

  /* Copy headers, set dt, ns */
  memcpy((void *)&vout,(const void *)&vin,HDRBYTES);
  vout->dt = vin->dt;
  vout->ns = ntout;

}

void rtot(int verbose, segy *vin, segy *vout, int ntout)
{

  int i;

  float dt, t, vt;

  dt = vin->dt * 0.001;
  vout->data[0] = vin->data[0];
  vt = 0.00001 * vout->data[0] * vout->data[0];
  for(i=1,t=vin->delrt+dt; i<vin->ns; i++,t+=dt) {
    vout->data[i] = (t * vin->data[i] * vin->data[i] - (t - dt) * vin->data[i-1] * vin->data[i-1]) / dt;
    vout->data[i] = sqrt(MAX(vout->data[i],vt));
  }

  /* Copy headers, set dt, ns */
  memcpy((void *)&vout,(const void *)&vin,HDRBYTES);
  vout->dt = vin->dt;
  vout->ns = ntout;

}

/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */
/* Modified to do 4-point Lagrange interpolation for interior points */
/* by Robert Miller, GSA, Inc., February, 2001 */

/*****************************************************************************
INT4  - evaluate y(x) via linear interpolation of y(x[0]), y(x[1]), ...
******************************************************************************
Function Prototype:
void int4 (int nin, float xin[], float yin[], float yinl, float yinr,
	int nout, float xout[], float yout[]);
Input:
nin		length of xin and yin arrays
xin		array[nin] of monotonically increasing or decreasing x values
yin		array[nin] of input y(x) values
yinl		value used to extraplate y(x) to left of input yin values
yinr		value used to extraplate y(x) to right of input yin values
nout		length of xout and yout arrays
xout		array[nout] of x values at which to evaluate y(x)
Output:
yout		array[nout] of linearly interpolated y(x) values
******************************************************************************
Notes:
xin values must be monotonically increasing or decreasing.

Extrapolation of the function y(x) for xout values outside the range
spanned by the xin values in performed as follows:

	For monotonically increasing xin values,
		yout=yinl if xout<xin[0], and yout=yinr if xout>xin[nin-1].

	For monotonically decreasing xin values, 
		yout=yinl if xout>xin[0], and yout=yinr if xout<xin[nin-1].

If nin==1, then the monotonically increasing case is used.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
void int4(int nin, float xin[], float yin[], float yinl, float yinr, 
          int nout, float xout[], float yout[])
{
	static int idx;
	int jout;
        int i, j;
        int four=4;

        float term1, term2;
	float x;
        float dy;

	/* if input x values are monotonically increasing, then */
	if (xin[0]<=xin[nin-1]) {
		for (jout=0; jout<nout; jout++) {
			x = xout[jout];
			if (x<xin[0])
				yout[jout] = yinl;
			else if (x>xin[nin-1])
				yout[jout] = yinr;
			else if (x==xin[nin-1] || nin==1)
				yout[jout] = yin[nin-1];
			else if((x >= xin[0] && x <= xin[1]) ||
                                (x >= xin[nin-2] && x <= xin[nin-1])) {
			        xindex(nin,xin,x,&idx);
				yout[jout] = yin[idx]+(x-xin[idx])
					*(yin[idx+1]-yin[idx])
					/(xin[idx+1]-xin[idx]);
			}
                        else {
			  xindex(nin,xin,x,&idx);
                          yout[jout] = 0.0;
                          for(i=idx-1; i<=idx+2; i++) {
                            term1 = yin[i];
                            term2 = 1.0;
                            for(j=idx-1; j<=idx+2; j++) {
                              if(j != i) {
                                term1 *= (x - xin[j]);
                                term2 *= (xin[i] - xin[j]);
                              }
                            }
                            yout[jout] += (term1 / term2);
                          }
                        }
		}
	
	/* else, if input x values are monotonically decreasing, then */
	} else {
          err("Code for decreasing values not yet implemented\n");
	}
}
void resamp(float *in, float *out, float dtin, float dtout, int ntin, int ntout, float *rt)
{
  int i;
  int mult;
  int dti, dto;

  float tmin;
  float tv;

  dti = dtin * 1000.;
  dto = dtout * 1000.;

  for(i=0,tv=0.0; i<ntout; i++,tv+=dtout)
    rt[i] = tv;

  /* Distribute input samples? */
  if(!(dto%dti)) {
    mult = dto / dti;
    for(i=0; i<ntout,i*mult<ntin; i++)
      out[i] = in[i*mult];
  }
  /* sinc interpolate new data */
  else
    ints8r(ntin,dtin,0,in,0.0,0.0,ntout,rt,out);

  if(!(dti%dto)) {
    mult = dti / dto;
    for(i=0; i<ntin,i*mult<ntout; i++)
      out[i*mult] = in[i];
  }

}
