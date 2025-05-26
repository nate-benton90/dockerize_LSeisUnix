/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"UNISAM:  UNIformly SAMple a function y(x) specified as x,y pairs\n"
"\n"
"unisam xin= yin= nout= [optional parameters] >binaryfile\n"
"\n"
"Required Parameters:\n"
"xin             array of x values (number of xin = number of yin)\n"
"yin             array of y values (number of yin = number of xin)\n"
"nout            number of y values output to binary file\n"
"\n"
"Optional Parameters:\n"
"dxout=1.0       output x sampling interval\n"
"fxout=0.0       output first x\n"
"method=linear   =linear for linear interpolation (continuous y)\n"
"                =mono for monotonic cubic interpolation (continuous y')\n"
"                =akima for Akima's cubic interpolation (continuous y')\n"
"                =spline for cubic spline interpolation (continuous y'')\n"
"\n"
"AUTHOR:  Dave Hale, Colorado School of Mines, 07/07/89\n"
"\n";

#include "par.h"

main(int argc, char **argv)
{
    int nin,nout,iout;
    float dxout,fxout,*xin,*yin,(*yind)[4],*xout,*yout;
    char *method="linear";
    FILE *outfp=stdout;

    /* hook up getpar */
    initargs(argc,argv);
    askdoc(0);

    /* get parameters */
    xin = alloc1float(countparval("xin"));
    yin = alloc1float(countparval("xin"));
    if ((nin=getparfloat("xin",xin))==0)
        err("Must specify xin!");
    if (getparfloat("yin",yin)!=nin) 
        err("Number of yins must equal number of xins!");
    if (!getparint("nout",&nout))
        err("Must specify nout!");
    dxout = 1.0;  getparfloat("dxout",&dxout);
    fxout = 0.0;  getparfloat("fxout",&fxout);
    getparstring("method",&method);

    /* allocate space for output */
    xout = ealloc1float(nout);
    yout = ealloc1float(nout);

    /* compute uniformly sampled xs */
    for (iout=0; iout<nout; iout++)
        xout[iout] = fxout+iout*dxout;

    /* if linear interpolation or only one input sample */
    if (method[0]=='l' || nin==1) {
        intlin(nin,xin,yin,yin[0],yin[nin-1],nout,xout,yout);

    /* else, if monotonic interpolation */
    } else if (method[0]=='m') {
        yind = (float (*)[4])ealloc1float(nin*4);
        cmonot(nin,xin,yin,yind);
        intcub(0,nin,xin,yind,nout,xout,yout);

    /* else, if Akima interpolation */
    } else if (method[0]=='a') {
        yind = (float (*)[4])ealloc1float(nin*4);;
        cakima(nin,xin,yin,yind);
        intcub(0,nin,xin,yind,nout,xout,yout);

    /* else, if cubic spline interpolation */
    } else if (method[0]=='s') {
        yind = (float (*)[4])ealloc1float(nin*4);;
        csplin(nin,xin,yin,yind);
        intcub(0,nin,xin,yind,nout,xout,yout);

    /* else, if unknown method specified */
    } else {
        err("%s is an unknown interpolation method!\n",method);
    }

    /* output */
    efwrite(yout,sizeof(float),nout,outfp);
}
