/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"VELCONV - VELocity CONVersion\n"
"\n"
"velconv <infile >outfile intype= outtype= [optional parameters]\n"
"\n"
"Required Parameters:\n"
"intype=                input data type (see valid types below)\n"
"outtype=               output data type (see valid types below)\n"
"\n"
"Valid types for input and output data are:\n"
"vintt          interval velocity as a function of time\n"
"vrmst          RMS velocity as a function of time\n"
"vintz          velocity as a function of depth\n"
"zt             depth as a function of time\n"
"tz             time as a function of depth\n"
"\n"
"Optional Parameters:\n"
"nt=all                 number of time samples\n"
"dt=1.0                 time sampling interval\n"
"ft=0.0                 first time\n"
"nz=all                 number of depth samples\n"
"dz=1.0                 depth sampling interval\n"
"fz=0.0                 first depth\n"
"nx=all                 number of traces\n"
"\n"
"Example:  \"intype=vintz outtype=vrmst\" converts an interval velocity\n"
"          function of depth to an RMS velocity function of time.\n"
"\n"
"Notes:  nt, dt, and ft are used only for input and output functions\n"
"        of time; you need specify these only for vintt, vrmst, orzt.\n"
"        Likewise, nz, dz, and fz are used only for input and output\n"
"        functions of depth.\n"
"\n"
"AUTHOR:  Dave Hale, Colorado School of Mines, 07/07/89\n"
"\n";

#include "par.h"

void in_vintt(int nt, float dt, float ft, int nz, float dz, float fz,
	float vintt[], float zt[], float tz[]);
void in_vrmst(int nt, float dt, float ft, int nz, float dz, float fz,
	float vrmst[], float zt[], float tz[]);
void in_zt(int nt, float dt, float ft, int nz, float dz, float fz,
	float ztin[], float zt[], float tz[]);
void in_vintz(int nt, float dt, float ft, int nz, float dz, float fz,
	float vintz[], float zt[], float tz[]);
void in_tz(int nt, float dt, float ft, int nz, float dz, float fz,
	float tzin[], float zt[], float tz[]);
void out_vintt(int nt, float dt, float ft, int nz, float dz, float fz,
	float vintt[], float zt[], float tz[]);
void out_vrmst(int nt, float dt, float ft, int nz, float dz, float fz,
	float vrmst[], float zt[], float tz[]);
void out_zt(int nt, float dt, float ft, int nz, float dz, float fz,
	float ztout[], float zt[], float tz[]);
void out_vintz(int nt, float dt, float ft, int nz, float dz, float fz,
	float vintz[], float zt[], float tz[]);
void out_tz(int nt, float dt, float ft, int nz, float dz, float fz,
	float tzout[], float zt[], float tz[]);
void zttz(int nt, float dt, float ft, float zt[], float vft, float vlt, 
	int nz, float dz, float fz, float tz[]);
void tzzt(int nz, float dz, float fz, float tz[], float vfz, float vlz, 
	int nt, float dt, float ft, float zt[]);

main (int argc, char **argv)
{
	int nt,nz,nin,nout,nx,ix;
	float dt,ft,dz,fz,*din,*dout,*zt,*tz;
	char *intype="",*outtype="";
	FILE *infp=stdin,*outfp=stdout;

	/* hook up getpar to handle the parameters */
	initargs(argc,argv);
	askdoc(2);

	/* get required parameters */
	if (!getparstring("intype",&intype)) err("Must specify intype!\n");
	if (!getparstring("outtype",&outtype)) err("Must specify outtype!\n");

	/* get optional parameters */
	if (!getparint("nt",&nt)) {
		if (getparint("nz",&nz)) {
			nt = nz;
		} else {
			if (fseek(infp,0L,2)==-1)
				err("input file size unknown; specify nt or nz\n");
			nt = eftell(infp)/sizeof(float);
		}
	}
	if (!getparfloat("dt",&dt)) dt = 1.0;
	if (!getparfloat("ft",&ft)) ft = 0.0;
	if (!getparint("nz",&nz)) {
		if (getparint("nt",&nt)) {
			nz = nt;
		} else {
			if (fseek(infp,0L,2)==-1)
				err("input file size unknown; specify nt or nz\n");
			nz = eftell(infp)/sizeof(float);
		}
	}
	if (!getparfloat("dz",&dz)) dz = 1.0;
	if (!getparfloat("fz",&fz)) fz = 0.0;

	/* determine number of samples per input and output trace */
	if (
		STREQ(intype,"vintt") || 
		STREQ(intype,"vrmst") || 
		STREQ(intype,"zt") ) {
		nin = nt;
	} else if (
		STREQ(intype,"vintz") || 
		STREQ(intype,"tz") ) {
		nin = nz;
	} else {
		err("invalid intype=%s!\n",intype);
	}
	if (
		STREQ(outtype,"vintt") || 
		STREQ(outtype,"vrmst") || 
		STREQ(outtype,"zt") ) {
		nout = nt;
	} else if (
		STREQ(outtype,"vintz") || 
		STREQ(outtype,"tz") ) {
		nout = nz;
	} else {
		err("invalid outtype=%s!\n",outtype);
	}

	/* determine number of traces to process */
	if (!getparint("nx",&nx)) nx = -1;

	/* allocate space */
	tz = ealloc1float(nz);
	zt = ealloc1float(nt);
	din = ealloc1float(nin);
	dout = ealloc1float(nout);

	/* set input file pointer to beginning of file */
	efseek(infp,0L,0);

	/* loop over traces */
	for (ix=0; ix<nx || nx<0; ix++) {

		/* read input data */
		if (efread(din,sizeof(float),nin,infp)!=nin) break;

		/* convert input data to zt and tz */
		if (STREQ(intype,"vintt"))
			in_vintt(nt,dt,ft,nz,dz,fz,din,zt,tz);
		else if (STREQ(intype,"vrmst"))
			in_vrmst(nt,dt,ft,nz,dz,fz,din,zt,tz);
		else if (STREQ(intype,"zt"))
			in_zt(nt,dt,ft,nz,dz,fz,din,zt,tz);
		else if (STREQ(intype,"vintz"))
			in_vintz(nt,dt,ft,nz,dz,fz,din,zt,tz);
		else if (STREQ(intype,"tz"))
			in_tz(nt,dt,ft,nz,dz,fz,din,zt,tz);
		else
			err("invalid intype=%s!\n",intype);

		/* convert zt and tz to output data */
		if (STREQ(outtype,"vintt"))
			out_vintt(nt,dt,ft,nz,dz,fz,zt,tz,dout);
		else if (STREQ(outtype,"vrmst"))
			out_vrmst(nt,dt,ft,nz,dz,fz,zt,tz,dout);
		else if (STREQ(outtype,"zt"))
			out_zt(nt,dt,ft,nz,dz,fz,zt,tz,dout);
		else if (STREQ(outtype,"vintz"))
			out_vintz(nt,dt,ft,nz,dz,fz,zt,tz,dout);
		else if (STREQ(outtype,"tz"))
			out_tz(nt,dt,ft,nz,dz,fz,zt,tz,dout);
		else
			err("invalid outtype=%s!\n",outtype);

		/* write output data */
		efwrite(dout,sizeof(float),nout,outfp);
	}
}

/* compute z(t) and t(z) from input vint(t) */
void in_vintt(int nt, float dt, float ft, int nz, float dz, float fz,
	float vintt[], float zt[], float tz[])
{
	int it;
	float vft,vlt;

	zt[0] = 0.5*ft*vintt[0];
	for (it=1; it<nt; it++)
		zt[it] = zt[it-1]+0.5*dt*vintt[it-1];
	vft = vintt[0];
	vlt = vintt[nt-1];
	zttz(nt,dt,ft,zt,vft,vlt,nz,dz,fz,tz);
}

/* compute z(t) and t(z) from input vrms(t) */
void in_vrmst(int nt, float dt, float ft, int nz, float dz, float fz,
	float vrmst[], float zt[], float tz[])
{
	int it;
	float t,vft,vlt,vtinys,vintts,vintt;

	vintt = vrmst[0];
	vtinys = 0.00001*vintt*vintt;
	zt[0] = 0.5*ft*vintt;
	for (it=1,t=ft+dt; it<nt; it++,t+=dt) {
		vintts = (t*vrmst[it]*vrmst[it] -
			(t-dt)*vrmst[it-1]*vrmst[it-1])/dt;
		vintt = sqrt(MAX(vintts,vtinys));
		zt[it] = zt[it-1]+0.5*dt*vintt;
	}
	vft = vrmst[0];
	vlt = vintt;
	zttz(nt,dt,ft,zt,vft,vlt,nz,dz,fz,tz);
}

/* compute z(t) and t(z) from input z(t) */
void in_zt(int nt, float dt, float ft, int nz, float dz, float fz,
	float ztin[], float zt[], float tz[])
{
	int it;
	float vft,vlt;

	for (it=0; it<nt; it++)
		zt[it] = ztin[it];
	vft = 2.0*(zt[1]-zt[0])/dt;
	vlt = 2.0*(zt[nt-1]-zt[nt-2])/dt;
	zttz(nt,dt,ft,zt,vft,vlt,nz,dz,fz,tz);
}

/* compute z(t) and t(z) from input vint(z) */
void in_vintz(int nt, float dt, float ft, int nz, float dz, float fz,
	float vintz[], float zt[], float tz[])
{
	int iz;
	float vfz,vlz;

	tz[0] = 2.0*fz/vintz[0];
	for (iz=1; iz<nz; iz++)
		tz[iz] = tz[iz-1]+2.0*dz/vintz[iz-1];
	vfz = vintz[0];
	vlz = vintz[nz-1];
	tzzt(nz,dz,fz,tz,vfz,vlz,nt,dt,ft,zt);
}

/* compute z(t) and t(z) from input t(z) */
void in_tz(int nt, float dt, float ft, int nz, float dz, float fz,
	float tzin[], float zt[], float tz[])
{
	int iz;
	float vfz,vlz;

	for (iz=0; iz<nz; iz++)
		tz[iz] = tzin[iz];
	vfz = 2.0*dz/(tz[1]-tz[0]);
	vlz = 2.0*dz/(tz[nz-1]-tz[nz-2]);
	tzzt(nz,dz,fz,tz,vfz,vlz,nt,dt,ft,zt);
}

/* compute output vint(t) from z(t) and t(z) */
void out_vintt(int nt, float dt, float ft, int nz, float dz, float fz,
	float zt[], float tz[], float vintt[])
{
	int it;

	for (it=0; it<nt-1; it++)
		vintt[it] = 2.0*(zt[it+1]-zt[it])/dt;
	vintt[nt-1] = vintt[nt-2];
}

/* compute output vrms(t) from z(t) and t(z) */
void out_vrmst(int nt, float dt, float ft, int nz, float dz, float fz,
	float zt[], float tz[], float vrmst[])
{
	int it;
	float vintt,vrmsts,sum,t;

	vintt = 2.0*(zt[1]-zt[0])/dt;
	sum = ft*vintt*vintt;
	vrmst[0] = vintt;
	for (it=1,t=ft+dt; it<nt; it++,t+=dt) {
		vintt = 2.0*(zt[it]-zt[it-1])/dt;
		sum += dt*vintt*vintt;
		vrmst[it] = sqrt(sum/t);
	}
}

/* compute output z(t) from z(t) and t(z) */
void out_zt(int nt, float dt, float ft, int nz, float dz, float fz,
	float zt[], float tz[], float ztout[])
{
	int it;

	for (it=0; it<nt; it++)
		ztout[it] = zt[it];
}

/* compute output vint(z) from z(t) and t(z) */
void out_vintz(int nt, float dt, float ft, int nz, float dz, float fz,
	float zt[], float tz[], float vintz[])
{
	int iz;

	for (iz=0; iz<nz-1; iz++)
		vintz[iz] = 2.0*dz/(tz[iz+1]-tz[iz]);
	vintz[nz-1] = vintz[nz-2];
}

/* compute output t(z) from z(t) and t(z) */
void out_tz(int nt, float dt, float ft, int nz, float dz, float fz,
	float zt[], float tz[], float tzout[])
{
	int iz;

	for (iz=0; iz<nz; iz++)
		tzout[iz] = tz[iz];
}

/* compute t(z) from z(t) */
void zttz(int nt, float dt, float ft, float zt[], float vft, float vlt, 
	int nz, float dz, float fz, float tz[])
{
	int iz;
	float z,lt=ft+(nt-1)*dt,lz=fz+(nz-1)*dz;

	yxtoxy(nt,dt,ft,zt,nz,dz,fz,0.0,0.0,tz);
	for (iz=0,z=fz; z<=zt[0]; iz++,z+=dz)
		tz[iz] = 2.0*z/vft;
	for (iz=nz-1,z=lz; z>=zt[nt-1]; iz--,z-=dz)
		tz[iz] = lt+2.0*(z-zt[nt-1])/vlt;
}

/* compute z(t) from t(z) */
void tzzt(int nz, float dz, float fz, float tz[], float vfz, float vlz, 
	int nt, float dt, float ft, float zt[])
{
	int it;
	float t,lt=ft+(nt-1)*dt,lz=fz+(nz-1)*dz;

	yxtoxy(nz,dz,fz,tz,nt,dt,ft,0.0,0.0,zt);
	for (it=0,t=ft; t<=tz[0]; it++,t+=dt)
		zt[it] = 0.5*t*vfz;
	for (it=nt-1,t=lt; t>=tz[nz-1]; it--,t-=dt)
		zt[it] = lz+0.5*(t-tz[nz-1])*vlz;
}
