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
#include <math.h>
#include "su.h"
#include "segy.h"
#include "header.h"
#include "bhpio_api.h"
#include "bhp_vel.h"

/*********************** self documentation **********************/
char *sdoc[] = {
" 					                        ",
" BHPTD - BHP Velocity Time/Depth Conversion                    ",
" 					                        ",
" BHPTD      converts seismic data traces                       ",
"            from time to depth and vice versa,                 ",
"            using velocities from a BHPIO cube dataset         ",
" 					                        ",
" bhptd < infile > outfile [optional parameters]                ",
" 					                        ",
" Required Parameters: none		                        ",
" 					                        ",
" Optional Parameters:			                        ",
" verbose=0         Use verbose=1 for long printout             ",
" velfile=vels      BHPIO filename of velocity volume           ",
" pathlist=vels.dat BHPIO pathlist for velocity volume          ",
" type=time-depth   Conversion type; use depth-time for depth to time",
"                                                               ",
" ntout=            Number of samples per trace in output         ",
"                   If not specified, ntout is set large enough to",
"                   map all samples in the first velocity trace   ",
" dt=0.004          Output time interval for depth-time conversion.",
" dz=20.0           Output depth interval for time-depth conversion.",
" 					                        ",
"                                                               ",
NULL};

/**************** end self doc ********************************/

/* Prototypes */

/* Globals */

int main (int argc, char **argv)
{

  char *type;        /* conversion type */
  char *velfile;     /* velocity filename */
  char *pathlist;    /* pathlist */

  int i;             /* loop count */
  int ntinv;         /* Number of samples SU vel input */
  int ntind;         /* Number of samples SU data input */
  int ntout;         /* Samples per trace - output */
  int verbose;       /* Debug print */
  int *keyvals;      /* keys for bhpread access */
  int *key_index;    /* key indices returned by open_bhpio_dataset */
  int nkeys;         /* nkeys returned by open_bhpio_dataset */
  int trcount;       /* trace count returned by bhpread */
  int data=1;        /* if data=0, convert vels only */

  float dt;          /* Time interval */
  float dz;          /* Depth interval */
  float *rt;         /* Output times for resample */
  float td;          /* Use for mapping to estimate ntout */
  float *times;      /* Output times for vel conversion */
  float *tin;        /* Mapped input times for vel conversion */
  float *depths;     /* Output depths for vel conversion */
  float *din;        /* Mapped input depths for vel conversion */
  float *trinr;      /* Resampled trin */
  float *troutr;     /* Resampled trout */
  float *vinr;       /* Resampled vin */

  segy vin;          /* Vel traces */
  segy trin,trout;   /* Seismic traces in and out */

  Value hval;        /* header value */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* check type */
  if(!getparstring("type",&type))
    type = "time-depth";
  if(strcmp(type,"depth-time") && strcmp(type,"time-depth") &&
     strcmp(type,"time-rms") && strcmp(type,"rms-time"))
    err("%s is illegal type\n",type);
  if(!strcmp(type,"time-rms") || !strcmp(type,"rms-time"))
    err("time-rms and rms-time not yet implemented\n");

  /* check ntout */
  if(!getparint("ntout",&ntout))
    ntout = -1;

  /* check dt and dz */
  if(!strcmp(type,"depth-time") && !getparfloat("dt",&dt))
    dt = 0.004;
  else if(!strcmp(type,"time-depth") && !getparfloat("dz",&dz))
    dz = 20.0;

  /* velfile, path */
  if(!getparstring("velfile",&velfile))
    velfile = "vels";
  /* if velfile=NULL, set vels only mode */
  if(!strcmp(velfile,"NULL")) {
    if(verbose)
      fprintf(stderr,"Setting velocity-only mode\n");
    data = 0;
  }
  if(!getparstring("pathlist",&pathlist))
    pathlist = "vels.dat";

  /* open vels datatset */
  if(data == 1) {
    key_index = open_bhpio_dataset(velfile,pathlist,&nkeys,verbose);
    /* alloc key vals */
    keyvals = calloc(nkeys,sizeof(int));
  }

  /* check first trace */
  if(!gettr(&trin))
    err("Can't get first data trace\n");
  ntind = trin.ns;
  /* get keyvals */
  if(data == 1) {
    for(i=0; i<nkeys; i++) {
      gethval(&trin,key_index[i],&hval);
      keyvals[i] = hval.i;
    }
  }

  if(verbose) {
    fprintf(stderr,"Conversion type is %s\n",type);
    if(data == 1) {
      fprintf(stderr,"Velocity filename is %s\n",velfile);
      fprintf(stderr,"Velocity file pathlist is %s\n",pathlist);
    }
    if(!strcmp(type,"depth-time"))
      fprintf(stderr,"Output time interval %f\n", dt);
    else
      fprintf(stderr,"Output depth interval %f\n", dz);
    fprintf(stderr,"Number of samples in input data = %d\n", ntind);
    if(!strcmp(type,"time-depth"))
      fprintf(stderr,"Input sample interval = %d\n",trin.dt);
    else
       fprintf(stderr,"Input depth interval = %d\n",trin.dt);
  }

  /* set interpolation */
  if(data == 1) {
    set_bhpio_binning_rule("match",verbose);
    set_bhpio_interp_flag("yes",verbose);
    /* get first vel trace */
    if((trcount = read_bhpio_trace(keyvals,&vin,verbose)) != 1) {
      fprintf(stderr,"Failed to read velocity trace for keys=");
      for(i=0; i<nkeys; i++)
        fprintf(stderr," %d ",keyvals[i]);
      fprintf(stderr,"\n");
      err("quitting\n");
    }
    ntinv = vin.ns;
  }
  /* copy trin to vin if no data */
  else {
    memcpy((void *)&vin,(const void *)&trin,HDRBYTES+4*trin.ns);
    ntinv = ntind;
  }


  /* If ntout wasn't set, use input or estimate */
  if(ntout == -1) {
    /* Map depths to time */
    if(!strcmp(type,"depth-time")) {
      td = 2.0 * 0.001 * vin.delrt / vin.data[0];
      for(i=1; i<ntinv; i++)
        td += 2.0 * 0.001 * vin.dt / vin.data[i-1];
      ntout = (td + dt) / dt + 1;
    }
    /* Map times to depth */
    else if(!strcmp(type,"time-depth")) {
      td = 0.5 * 0.001 * vin.delrt * vin.data[0];
      for(i=1; i<ntinv; i++)
        td += 0.5 * 0.000001 * vin.dt * vin.data[i-1];
      ntout = (td + dz) / dz + 1;
    }
    trout.dt = dt;
    trout.ns = ntout;
  }
  if(verbose) {
    fprintf(stderr,"td=%f\n",td);
    fprintf(stderr,"ntout set to %d\n", ntout);
  }

  /* alloc resampling buffers, build time/depth traces */
  vinr = calloc(M*ntinv,sizeof(float));
  i = MAX(M*ntinv,M*ntind);
  rt = calloc(MAX(i,ntout),sizeof(float));
  trinr = calloc(M*ntind,sizeof(float));
  troutr = calloc(M*ntout,sizeof(float));
  if(!strcmp(type,"depth-time")) {
    tin = calloc(M*ntinv,sizeof(float));
    times = calloc(M*ntout,sizeof(float));
    times[0] = 0.0;
    for(i=1; i<M*ntout; i++)
      times[i] = times[i-1] + (float)dt;
    if(verbose) {
      fprintf(stderr,"times\n");
      for(i=0; i<M*ntout; i++) {
        fprintf(stderr," %.4f ",times[i]);
        if(((i+1)%8) == 0)
          fprintf(stderr,"\n");
      }
      fprintf(stderr,"\n");
    }
  }
  else {
    din = calloc(M*ntinv,sizeof(float));
    depths = calloc(M*ntout,sizeof(float));
    depths[0] = 0.0;
    for(i=1; i<M*ntout; i++)
      depths[i] = depths[i-1] + ((float)dz / M);
    if(verbose) {
      fprintf(stderr,"depths\n");
      for(i=0; i<M*ntout; i++)
        fprintf(stderr," %f ",depths[i]);
      fprintf(stderr,"\n");
    }
  }

  /* Loop until done */
  for(;;) {
    /* Conversion */
    if(!strcmp(type,"depth-time"))
      dtot(verbose,&trin,trinr,&trout,troutr,&vin,vinr,tin,times,ntinv,ntind,ntout,
           vin.dt,dt,rt);
    else if(!strcmp(type,"time-depth"))
      ttod(verbose,&trin,trinr,&trout,troutr,&vin,vinr,din,depths,ntinv,ntind,ntout,
           0.000001*(float)vin.dt,dz,rt);
    else if(!strcmp(type,"rms-time"))
      rtot(verbose,&vin,&trout,ntout);
    else if(!strcmp(type,"time-rms"))
      ttor(verbose,&vin,&trout,ntout);
    puttr(&trout);
    if(!gettr(&trin))
      break;
    /* next veltrace */
    if(data == 1) {
      for(i=0; i<nkeys; i++) {
        gethval(&trin,key_index[i],&hval);
        keyvals[i] = hval.i;
      }
      if((trcount = read_bhpio_trace(keyvals,&vin,verbose)) != 1) {
        fprintf(stderr,"Failed to read velocity trace for keys=");
        for(i=0; i<nkeys; i++)
          fprintf(stderr," %d ",keyvals[i]);
        fprintf(stderr,"\n");
        err("quitting\n");
      }
    }
    else
      memcpy((void *)&vin,(const void *)&trin,HDRBYTES+4*trin.ns);
  }

  if(!strcmp(type,"depth-time")) {
    fprintf(stderr,"Last mapped time = %f\n", tin[M*ntinv-1]);
    fprintf(stderr,"Last output time = %f\n", times[M*ntout-1]);
  }
  else {
    fprintf(stderr,"Last mapped depth = %f\n", din[M*ntinv-1]);
    fprintf(stderr,"Last output depth = %f\n", depths[M*ntout-1]);
  }

  return EXIT_SUCCESS;

}
