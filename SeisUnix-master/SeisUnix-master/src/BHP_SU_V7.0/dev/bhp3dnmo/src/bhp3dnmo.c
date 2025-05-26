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
/******************************************************************
**
*
* KEYWORDS:  $RCSfile: bhp3dnmo.c,v $
*            $Revision: 1.3 $
*            $Date: 2002/05/07 18:11:36 $
*            $Author: ahmilb $
*/

/* Copyright (c) Colorado School of Mines, 1999.*/
/* All rights reserved.                       */

#include "su.h"
#include "segy.h"

/* Prototypes */
FILE *getpar3d(segy *tr, int lindex, int *line, char *pardir, int verbose);
float *getlist(FILE *fp, char *name, int *nlist, int which);
void tnmovnmo(int *ntnmo, int *nvnmo, int *nanis1, int *nanis2);
float *getcdps(FILE *fp, int line, int lindex, int *ncdp);
float *getvnmo(FILE *fp, int *nvnmo);
float *gettnmo(segy *vel);

/* Globals */
segy vel;     /* for veltype=su */

/*********************** self documentation ******************************/
char *sdoc[] = {
"        					     ",
" BHP3DNMO - NMO for multiple lines, using user-specified arbitrary velocity ",
"            functions                                                       ",
" bhp3dnmo is a multi-line adaptation of the SU sunmo program.                       ",
"        					     ",
" bhp3dnmo <stdin >stdout [optional parameters]             ",
"        					     ",
" Optional Parameters:        			     ",
" veltype=par3d         Velocity file type. Use veltype=su for velocities",
"                       from seismic traces. It is assumed that velocity",
"                       traces are in correct line,cdp order, and that each ",
"                       trace has the same sampling interval and trace length",
"                       as the seismic data being corrected. Within a line,",
"                       velocity traces will be linearly interpolated for ",
"                       missing CDPs.                                        ",
" velfile=vels.su       Velocity traces if veltype=su.                       ",
" pardir=3dvels         Directory containing par file for each line, where   ",
"                       files are named line_'num'.par3d. 'num' corresponds  ",
"                       to 3D line number, which is matched against          ",
"                       lhdr value in trace data. Not used if veltype=su.    ", 
" lhdr=ep               Header used to locate par3d file containing correct  ",
"                       vnmo,tnmo,anis1,anis2, and cdp parameters. If        ",
"                       veltype=su, velocity trace header to use for line.   ",
" tnmo=0                NMO times corresponding to velocities in vnmo       ",
" vnmo=                 NMO velocities corresponding to times in tnmo       ",
" anis1=0               two anisotropy coefficients making up quartic term   ",
" anis2=0               in traveltime curve, corresponding to times in tnmo  ",
" cdp=                  CDPs for which vnmo & tnmo are specified (see Notes) ",
" smute=1.5             samples with NMO stretch exceeding smute are zeroed  ",
" lmute=25              length (in samples) of linear ramp for stretch mute  ",
" sscale=1              =1 to divide output samples by NMO stretch factor    ",
" invert=0              =1 to perform (approximate) inverse NMO               ",
" ixoffset=0            do not consider cross-line offset                    ",
"                       =1 read cross-line offset from trace header          ",
"        					     ",
" Notes:        				     ",
"        					     ",
" For constant-velocity NMO, specify only one vnmo=constant and omit tnmo.   ",
"        					     ",
" The anisotropy coefficients anis1, anis2 permit non-hyperbolicity due       ",
" to layering, mode conversion, or anisotropy. Default is isotropic NMO.     ",
"        					     ",
" For NMO with a velocity function of time only, specify the arrays       ",
"     vnmo=v1,v2,... tnmo=t1,t2,...      	     ",
" where v1 is the velocity at time t1, v2 is the velocity at time t2, ...    ",
" The times specified in the tnmo array must be monotonically increasing.    ",
" Linear interpolation and constant extrapolation of the specified velocities",
" is used to compute the velocities at times not specified.         ",
" The same holds for the anisotropy coefficients as a function of time only. ",
"        					     ",
" For NMO with a velocity function of time and CDP, specify the array       ",
"     cdp=cdp1,cdp2,...      			     ",
" and, for each CDP specified, specify the vnmo and tnmo arrays as described ",
" above. The first (vnmo,tnmo) pair corresponds to the first cdp, and so on. ",
" Linear interpolation and constant extrapolation of 1/velocity^2 is used    ",
" to compute velocities at CDPs not specified.             ",
" The same holds for the anisotropy coefficients as a function of time and   ",
" CDP.        					     ",
"        					     ",
" Moveout is defined by        			     ",
"        					     ",
"   1     anis1    					     ",
"  --- x^2 + ------------- x^4.        		     ",
"  v^2       1 + anis2 x^2      			     ",
"        					     ",
" Note: In general, the user should set the cdp parameter.  The default is   ",
"  to use tr.cdp from the first trace and assume only one cdp.          ",
" Caveat:        				     ",
" Nmo cannot handle negative moveout as in triplication caused by       ",
" anisotropy. But negative moveout happens necessarily for negative anis1 at ",
" sufficiently large offsets. Then the error-negative moveout- is printed.   ",
" Check anis1. An error (anis2 too small) is also printed if the       ",
" denominator of the quartic term becomes negative. Check anis2. These errors",
" are prompted even if they occur in traces which would not survive the       ",
" NMO-stretch threshold. Chop off enough far-offset traces (e.g. with suwind)",
" if anis1, anis2 are fine for near-offset traces.           ",
"        					     ",
" NMO interpolation error is less than 1% for frequencies less than 60% of   ",
" the Nyquist frequency.        		     ",
"        					     ",
" Exact inverse NMO is impossible, particularly for early times at large     ",
" offsets and for frequencies near Nyquist with large interpolation errors.  ",
NULL};

/* Credits:
 *  SEP: Shuki, Chuck Sword
 *  CWP: Shuki, Jack, Dave Hale, Bjoern Rommel
 *      Modified: 08/08/98 - Carlos E. Theodoro - option for lateral offset
 *
 * Technical Reference:
 *  The Common Depth Point Stack
 *  William A. Schneider
 *  Proc. IEEE, v. 72, n. 10, p. 1238-1254
 *  1984
 *
 * Trace header fields accessed: ns, dt, delrt, offset, cdp, sy
 */
/**************** end self doc *******************************************/

static void interpovv (int nt, int ncdp, float *cdp, 
  float **ovv, float **oa1, float **oa2, float cdpt, 
  float *ovvt, float *oa1t, float *oa2t);

segy tr;

int
main(int argc, char **argv)
{
  int nt;          /* number of time samples per trace */
  float dt;        /* time sampling interval */
  float ft;        /* time of first sample */
  int it;          /* time sample index */
  int ncdp;        /* number of cdps specified */
  float *cdp;      /* array[ncdp] of cdps */
  int icdp;        /* index into cdp array */
  int jcdp;        /* index into cdp array */
  int nvnmo;       /* number of vnmos specified */
  float *vnmo;     /* array[nvnmo] of vnmos */
  int ntnmo;       /* number of tnmos specified */
  float *tnmo;     /* array[ntnmo] of tnmos */
  float **ovv;     /* array[ncdp][nt] of sloth (1/velocity^2) functions */
  float *ovvt;     /* array[nt] of sloth for a particular trace */
  int nanis1;      /* number of anis1's specified */
  int nanis2;      /* number of anis2's specified */
  float *anis1;    /* array[nanis1] of anis1's */
  float *anis2;    /* array[nanis2] of anis2's */
  float **oa1;     /* array[ncdp][nt] of anis1 functions */
  float **oa2;     /* array[ncdp][nt] of anis2 functions */
  float *oa1t;     /* array[nt] of anis1 for a particular trace */
  float *oa2t;     /* array[nt] of anis2 for a particular trace */
  float smute;     /* zero samples with NMO stretch exceeding smute */
  float osmute;    /* 1/smute */
  int lmute;       /* length in samples of linear ramp for mute */
  int itmute=0;    /* zero samples with indices less than itmute */
  int sscale;      /* if non-zero, apply NMO stretch scaling */
  int invert;      /* if non-zero, do inverse NMO */
  float sy;        /* cross-line offset component */
  int ixoffset;    /* indes for cross-line offset component */
  long oldoffset;  /* offset of previous trace */
  long oldcdp;     /* cdp of previous trace */
  int newsloth;    /* if non-zero, new sloth function was computed */
  float tn;        /* NMO time (time after NMO correction) */
  float v;         /* velocity */
  float *qtn;      /* NMO-corrected trace q(tn) */
  float *ttn;      /* time t(tn) for NMO */
  float *atn;      /* amplitude a(tn) for NMO */
  float *qt;       /* inverse NMO-corrected trace q(t) */
  float *tnt;      /* time tn(t) for inverse NMO */
  float *at;       /* amplitude a(t) for inverse NMO */
  float acdp;      /* temporary used to sort cdp array */
  float *aovv;     /* temporary used to sort ovv array */
  float *aoa1;     /* temporary used to sort oa1 array */
  float *aoa2;     /* temporary used to sort oa2 array */
  float temp;      /* temporary float */
  float tsq;       /* temporary float */
  int i;           /* index used in loop */

  /* 3D */
  char *pardir;    /* directory containing lines of velocities */
  char *veltype;   /* default=par3d to use pardir, else =su to read vel traces */
  char *velfile;   /* velocities if veltype=su */
  char *lhdr;      /* header to use for line number */
  
  int line;        /* current line number */
  int trline;      /* line number from current trace */
  int lindex;      /* line header index */
  int verbose;     /* print */

  FILE *fp;        /* parfile or velfile */

  Value hval;      /* 3D line */

  /* hook up getpar */
  initargs(argc, argv);
  requestdoc(1);

  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* get information from the first header */
  if (!gettr(&tr)) err("can't get first trace");
  nt = tr.ns;
  dt = ((double) tr.dt)/1000000.0;
  ft = tr.delrt/1000.0;
  sy  = tr.sy;

  /* 3D */
  if(!getparstring("veltype",&veltype))
    veltype = "par3d";
  if(!strcmp(veltype,"su"))
    if(!getparstring("velfile",&velfile))
      velfile = "vels.su";
  if(!getparstring("pardir",&pardir))
    pardir = "3dvels";
  if(!getparstring("lhdr",&lhdr))
    lhdr = "ep";
  /* lhdr index */
  lindex = getindex(lhdr);
  gethval(&tr,lindex,&hval);
  trline = hval.i;
  line = trline;

  if(verbose) {
    fprintf(stderr,"VELTYPE: %s\n",veltype);
    if(!strcmp(veltype,"su"))
      fprintf(stderr,"VELFILE: %s\n", velfile);
    else
      fprintf(stderr,"PARDIR: %s\n", pardir);
  }

  /* get par3d file */
  if(!strcmp(veltype,"par3d"))
    fp = getpar3d(&tr,lindex,&trline,pardir,verbose);
  else
    if(!(fp = fopen(velfile,"r")))
      err("Unable to open seismic input %s\n", velfile);

  /* load cdp list */
  if(!strcmp(veltype,"par3d"))
    cdp = getlist(fp,"cdp",&ncdp,1);
  else
    cdp = getcdps(fp,trline,lindex,&ncdp);
  if(ncdp == 0)
    err("No velocities found for line %d\n",trline);

  /* get the tnmo and vnmo list for each cdp */
  ovv = ealloc2float(nt,ncdp);
  oa1 = ealloc2float(nt,ncdp);
  oa2 = ealloc2float(nt,ncdp);
  for (icdp=0; icdp<ncdp; ++icdp) {
    if(!strcmp(veltype,"par3d")) {
      rewind(fp);
      vnmo = getlist(fp,"vnmo",&nvnmo,icdp+1);
      rewind(fp);
      tnmo = getlist(fp,"tnmo",&ntnmo,icdp+1);
      rewind(fp);
      anis1 = getlist(fp,"anis1",&nanis1,icdp+1);
      rewind(fp);
      anis2 = getlist(fp,"anis2",&nanis2,icdp+1);
      tnmovnmo(&ntnmo,&nvnmo,&nanis1,&nanis2);
    }
    /* veltype=su */
    else {
      nanis1 = nanis2 = 0;
      vnmo = getvnmo(fp,&nvnmo);
      ntnmo = nvnmo;
      tnmo = gettnmo(&vel);
    }
    if(nanis1 == 0) {
      nanis1 = nvnmo;
      anis1 = calloc(nanis1,sizeof(float));
      for(i=0; i<nanis1; i++)
        anis1[i] = 0.0;
    }
    if(nanis2 == 0) {
      nanis2 = nvnmo;
      anis2 = calloc(nanis2,sizeof(float));
      for(i=0; i<nanis2; i++)
        anis2[i] = 0.0;
    }
    if(verbose)
      fprintf(stderr,"CDP: %f FIRST T,V: %f %f LAST T,V %f %f\n",
              cdp[icdp],tnmo[0],vnmo[0],tnmo[ntnmo-1],vnmo[nvnmo-1]);
    for (it=1; it<ntnmo; ++it)
      if (tnmo[it]<=tnmo[it-1])
        err("tnmo values must increase monotonically");
    for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
      intlin(ntnmo,tnmo,vnmo,vnmo[0],vnmo[nvnmo-1],1,&tn,&v);
      ovv[icdp][it] = 1.0/(v*v);
    }
    for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
      intlin(ntnmo,tnmo,anis1,anis1[0],anis1[nanis1-1],1,&tn,
             &oa1[icdp][it]);
    }
    for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
      intlin(ntnmo,tnmo,anis2,anis2[0],anis2[nanis2-1],1,&tn,
             &oa2[icdp][it]);
    }
    free1float(vnmo);
    free1float(tnmo);
    free1float(anis1);
    free1float(anis2);
  }

  /* sort (by insertion) sloth and anis functions by increasing cdp */
  for (jcdp=1; jcdp<ncdp; ++jcdp) {
    acdp = cdp[jcdp];
    aovv = ovv[jcdp];
    aoa1 = oa1[jcdp];
    aoa2 = oa2[jcdp];
    for (icdp=jcdp-1; icdp>=0 && cdp[icdp]>acdp; --icdp) {
      cdp[icdp+1] = cdp[icdp];
      ovv[icdp+1] = ovv[icdp];
      oa1[icdp+1] = oa1[icdp];
      oa2[icdp+1] = oa2[icdp];
    }
    cdp[icdp+1] = acdp;
    ovv[icdp+1] = aovv;
    oa1[icdp+1] = aoa1;
    oa2[icdp+1] = aoa2;
  }

  /* get other optional parameters */
  if (!getparfloat("smute",&smute)) smute = 1.5;
  if (!getparint("ixoffset",&ixoffset)) ixoffset=0;
  if(ixoffset==0) sy = 0.0;
  if (smute<=0.0) err("smute must be greater than 0.0");
  if (!getparint("lmute",&lmute)) lmute = 25;
  if (!getparint("sscale",&sscale)) sscale = 1;
  if (!getparint("invert",&invert)) invert = 0;

  /* allocate workspace */
  ovvt = ealloc1float(nt);
  oa1t = ealloc1float(nt);
  oa2t = ealloc1float(nt);
  ttn = ealloc1float(nt);
  atn = ealloc1float(nt);
  qtn = ealloc1float(nt);
  tnt = ealloc1float(nt);
  at = ealloc1float(nt);
  qt = ealloc1float(nt);

  /* interpolate sloth and anis function for first trace */
  interpovv(nt,ncdp,cdp,ovv,oa1,oa2,(float)tr.cdp,ovvt,oa1t,oa2t);

  /* set old cdp and old offset for first trace */
  oldcdp = tr.cdp;
  oldoffset = tr.offset-1;

  warn("sy = %f",sy);

  /* loop over traces */
  do {
    /* check line */
    gethval(&tr,lindex,&hval);
    trline = hval.i;
    if(trline != line) {
      if(verbose)
        fprintf(stderr,"New line: %d\n", trline);
      /* get par3d file */
      if(!strcmp(veltype,"par3d")) {
        fclose(fp);
        fp = getpar3d(&tr,lindex,&trline,pardir,verbose);
      }
      /* load cdp list */
      free1float(cdp);
      if(!strcmp(veltype,"par3d"))
        cdp = getlist(fp,"cdp",&ncdp,1);
      else
        cdp = getcdps(fp,trline,lindex,&ncdp);
      if(ncdp == 0)
        err("No velocities found for line %d\n",trline);
      free2float(ovv);
      ovv = ealloc2float(nt,ncdp);
      free2float(oa1);
      oa1 = ealloc2float(nt,ncdp);
      free2float(oa2);
      oa2 = ealloc2float(nt,ncdp);
      for(icdp=0; icdp<ncdp; icdp++) {
        if(!strcmp(veltype,"par3d")) {
          rewind(fp);
          vnmo = getlist(fp,"vnmo",&nvnmo,icdp+1);
          rewind(fp);
          tnmo = getlist(fp,"tnmo",&ntnmo,icdp+1);
          rewind(fp);
          anis1 = getlist(fp,"anis1",&nanis1,icdp+1);
          rewind(fp);
          anis2 = getlist(fp,"anis2",&nanis2,icdp+1);
          tnmovnmo(&ntnmo,&nvnmo,&nanis1,&nanis2);
        }
        /* veltype=su */
        else {
          nanis1 = nanis2 = 0;
          vnmo = getvnmo(fp,&nvnmo);
          ntnmo = nvnmo;
          tnmo = gettnmo(&vel);
        }
        if(verbose)
          fprintf(stderr,"CDP: %f FIRST T,V: %f %f LAST T,V %f %f\n",
                  cdp[icdp],tnmo[0],vnmo[0],tnmo[ntnmo-1],vnmo[nvnmo-1]);
        if(nanis1 == 0) {
          nanis1 = nvnmo;
          anis1 = calloc(nanis1,sizeof(float));
          for(i=0; i<nanis1; i++)
            anis1[i] = 0.0;
        }
        if(nanis2 == 0) {
          nanis2 = nvnmo;
          anis2 = calloc(nanis2,sizeof(float));
          for(i=0; i<nanis2; i++)
            anis2[i] = 0.0;
        }
        for (it=1; it<ntnmo; ++it)
          if (tnmo[it]<=tnmo[it-1])
            err("tnmo values must increase monotonically");
        for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
          intlin(ntnmo,tnmo,vnmo,vnmo[0],vnmo[nvnmo-1],1,&tn,&v);
          ovv[icdp][it] = 1.0/(v*v);
        }
        for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
          intlin(ntnmo,tnmo,anis1,anis1[0],anis1[nanis1-1],1,&tn,
                 &oa1[icdp][it]);
        }
        for (it=0,tn=ft; it<nt; ++it,tn+=dt) {
          intlin(ntnmo,tnmo,anis2,anis2[0],anis2[nanis2-1],1,&tn,
                 &oa2[icdp][it]);
        }
        free1float(tnmo);
        free1float(vnmo);
        free1float(anis1);
        free1float(anis2);
      }
      /* sort (by insertion) sloth and anis functions by increasing cdp */
      for (jcdp=1; jcdp<ncdp; ++jcdp) {
        acdp = cdp[jcdp];
        aovv = ovv[jcdp];
        aoa1 = oa1[jcdp];
        aoa2 = oa2[jcdp];
        for (icdp=jcdp-1; icdp>=0 && cdp[icdp]>acdp; --icdp) {
          cdp[icdp+1] = cdp[icdp];
          ovv[icdp+1] = ovv[icdp];
          oa1[icdp+1] = oa1[icdp];
          oa2[icdp+1] = oa2[icdp];
        }
        cdp[icdp+1] = acdp;
        ovv[icdp+1] = aovv;
        oa1[icdp+1] = aoa1;
        oa2[icdp+1] = aoa2;
      }
    }

    /* if necessary, compute new sloth and anis function */
    if (tr.cdp!=oldcdp && ncdp>1) {
      interpovv(nt,ncdp,cdp,ovv,oa1,oa2,(float)tr.cdp,
          ovvt,oa1t,oa2t);
      newsloth = 1;
    } else {
      newsloth = 0;
    }

    /* if sloth and anis function or offset has changed */
    if (newsloth || tr.offset!=oldoffset) {
      /* compute time t(tn) (normalized) */
      temp = ((float) tr.offset*(float) tr.offset + sy*sy)/(dt*dt);
      for (it=0,tn=ft/dt; it<nt; ++it,tn+=1.0) {
        tsq = temp*ovvt[it] + \
              oa1t[it]*temp*temp / (1.0+oa2t[it]*temp);
        if (tsq<0.0)
        	err("negative moveout; check anis1, "
        	    "anis2, or suwind far-offset "
        	    "traces");
        if ((1.0+oa2t[it]*temp)<=0.0)
        	err("anis2 negative and too small; "
        	    "check anis2, or suwind far-offset"
        	    " traces");
        ttn[it] = sqrt (tn*tn + tsq);
        }
      /* compute inverse of stretch factor a(tn) */
      atn[0] = ttn[1]-ttn[0];
      for (it=1; it<nt; ++it)
        atn[it] = ttn[it]-ttn[it-1];
      
      /* determine index of first sample to survive mute */
      osmute = 1.0/smute;
      for (it=0,itmute=0; it<nt && atn[it]<osmute; ++it)
        itmute++;
      
      /* if inverse NMO will be performed */
      if (invert) {
        			
        /* compute tn(t) from t(tn) */
        yxtoxy(nt-itmute,1.0,ft/dt+itmute,&ttn[itmute],
        	nt-itmute,1.0,ft/dt+itmute,
        	ft/dt-nt,ft/dt+nt,&tnt[itmute]);
      
        /* adjust mute time */
        itmute = 1.0+ttn[itmute]-ft/dt;
        itmute = MIN(nt-2,itmute);
        				
        /* compute a(t) */
        if (sscale) {
          for (it=itmute+1; it<nt; ++it)
            at[it] = tnt[it]-tnt[it-1];
            at[itmute] = at[itmute+1];
        }
      }
    }
    
    /* if forward (not inverse) nmo */
    if (!invert) {
  
      /* do nmo via 8-point sinc interpolation */
      ints8r(nt,1.0,ft/dt,tr.data,0.0,0.0,
        nt-itmute,&ttn[itmute],&qtn[itmute]);
      
      /* apply mute */
      for (it=0; it<itmute; ++it)
        qtn[it] = 0.0;
      
      /* apply linear ramp */
      for (it=itmute; it<itmute+lmute && it<nt; ++it)
        qtn[it] *= (float)(it-itmute+1)/(float)lmute;
      
      /* if specified, scale by the NMO stretch factor */
      if (sscale)
        for (it=itmute; it<nt; ++it)
          qtn[it] *= atn[it];
      
      /* copy NMO corrected trace to output trace */
      memcpy( (void *) tr.data,(const void *) qtn, nt*sizeof(float));
    
    /* else inverse nmo */
    } else {
  
      /* do inverse nmo via 8-point sinc interpolation */
      ints8r(nt,1.0,ft/dt,tr.data,0.0,0.0,
        nt-itmute,&tnt[itmute],&qt[itmute]);
      
      /* apply mute */
      for (it=0; it<itmute; ++it)
        qt[it] = 0.0;
      
      /* if specified, undo NMO stretch factor scaling */
      if (sscale)
        for (it=itmute; it<nt; ++it)
          qt[it] *= at[it];
      
      /* copy inverse NMO corrected trace to output trace */
      memcpy( (void *) tr.data,(const void *) qt,nt*sizeof(float));
    }

    /* write output trace */
    puttr(&tr);

    /* remember offset and cdp */
    oldoffset = tr.offset;
    oldcdp = tr.cdp;

    line = trline;

  } while (gettr(&tr));

  return EXIT_SUCCESS;
}


/* linearly interpolate/extrapolate sloth and anis between cdps */
static void interpovv (int nt, int ncdp, float *cdp, float **ovv, float **oa1, 
  float **oa2, float cdpt, float *ovvt, float *oa1t, float *oa2t)
{
  static int indx=0;
  int it;
  float a1,a2;

  /* if before first cdp, constant extrapolate */
  if (cdpt<=cdp[0]) {
    for (it=0; it<nt; ++it) {
      ovvt[it] = ovv[0][it];
      oa1t[it] = oa1[0][it];
      oa2t[it] = oa2[0][it];
          };
  
  /* else if beyond last cdp, constant extrapolate */
  } else if (cdpt>=cdp[ncdp-1]) {
    for (it=0; it<nt; ++it) {
      ovvt[it] = ovv[ncdp-1][it];
      oa1t[it] = oa1[ncdp-1][it];
      oa2t[it] = oa2[ncdp-1][it];
          };
  
  /* else, linearly interpolate */
  } else {
    xindex(ncdp,cdp,cdpt,&indx);
    a1 = (cdp[indx+1]-cdpt)/(cdp[indx+1]-cdp[indx]);
    a2 = (cdpt-cdp[indx])/(cdp[indx+1]-cdp[indx]);
    for (it=0; it<nt; ++it) {
      ovvt[it] = a1*ovv[indx][it]+a2*ovv[indx+1][it];
      oa1t[it] = a1*oa1[indx][it]+a2*oa1[indx+1][it];
      oa2t[it] = a1*oa2[indx][it]+a2*oa2[indx+1][it];
          };
  }
}

FILE *getpar3d(segy *tr, int lindex, int *line, char *pardir, int verbose)
{

  char parfile[256];
  char string[32];

  FILE *fp;

  /* Check 1st line in pardir */
  strcpy(parfile,pardir);
  strcat(parfile,"/line_");
  sprintf(string,"%d",*line);
  strcat(parfile,string);
  strcat(parfile,".par3d");
  if(verbose)
    fprintf(stderr,"Opening %s\n", parfile);
  if(!(fp = fopen(parfile,"r")))
    err("Can't open %s\n", parfile);

  return fp;

}

float *getlist(FILE *fp, char *name, int *n, int which)
{

  char record[4096];
  char *item;

  float *list;

  int i;
  int count=1;

  /* get next list with name='name' from par3d file */
  *n = 0;
  while(fgets(record,4096,fp) != NULL) {
    if(strncmp(record,name,strlen(name)) == 0 && count == which) {
      /* count commas */
      for(i=0,*n=0; record[i]!=0; i++) {
        if(record[i] == ',')
          (*n)++;
      }
      (*n)++;
      list = calloc(*n,sizeof(float));
      item = strtok(record,"=");
      for(i=0; i<*n; i++)  {
        item = strtok(NULL,",");
        list[i] = atof(item);
      }
      return list;
    }
    else if(strncmp(record,name,strlen(name)) == 0)
      count++;
  }

  return 0;

}
void tnmovnmo(int *ntnmo, int *nvnmo, int *nanis1, int *nanis2)
{

  if (*nvnmo != *ntnmo)
    err("number of vnmo and tnmo values must be equal");
  if (*nanis1 != *nvnmo && *nanis1 != 0)
    err("number of vnmo and anis1 values must be equal");
  if (*nanis2 != *nvnmo && *nanis2 != 0)
    err("number of vnmo and anis2 values must be equal");

}

float *getcdps(FILE *fp, int line, int lindex, int *ncdp)
{

  float temp[4096];     /* temp cdp array */
  float *cdp;           /* return pointer */

  int i;

  static long curpos=0; /* current location in velfile */

  Value hval;           /* header */

  *ncdp = 0;

  /* save current file position */
  curpos = ftell(fp);

  /* read vel traces for all CDPs in line */
  for(;;) {
    if(!fgettr(fp,&vel))
      break;
    gethval(&vel,lindex,&hval);
    if(line != hval.i)
      break;
    else {
      temp[*ncdp] = vel.cdp;
      (*ncdp)++;
      if(*ncdp == 4096)
        err("temp overflow\n");
    }
  }

  /* move temp to cdp list */
  cdp = calloc(*ncdp,sizeof(float));
  for(i=0; i<*ncdp; i++)
    cdp[i] = temp[i];

  /* reset curpos */
  if(fseek(fp,curpos,SEEK_SET))
    err("Error from fseek\n");

  return cdp;

}

float *getvnmo(FILE *fp, int *nvnmo)
{

  float *vnmo;

  int i;

  fgettr(fp,&vel);

  *nvnmo = vel.ns;

  vnmo = calloc(*nvnmo,sizeof(float));

  for(i=0; i<*nvnmo; i++)
    vnmo[i] = vel.data[i];

  return vnmo;

}

float *gettnmo(segy *vel)
{
  float *tnmo;
  float dt;

  int i;

  tnmo = calloc(vel->ns,sizeof(float));

  dt = vel->dt * 0.000001;

  tnmo[0] = vel->delrt;
  for(i=1; i<vel->ns; i++)
    tnmo[i] = tnmo[i-1] + dt;

  return tnmo;

}
