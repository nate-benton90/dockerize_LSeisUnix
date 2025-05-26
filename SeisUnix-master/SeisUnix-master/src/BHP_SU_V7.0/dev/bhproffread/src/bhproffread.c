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
* KEYWORDS:  $RCSfile: bhproffread.c,v $
*            $Revision: 1.9 $
*            $Date: 2003/12/10 17:30:45 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhproffread.c,v $
* Revision 1.9  2003/12/10 17:30:45  ahmilb
* Use verbose=1 or verbose=2 for different amounts of debug print.
*
* Revision 1.8  2003/05/13 18:23:56  ahmilb
* Reduce volume of printout.
*
* Revision 1.7  2003/04/22 17:28:31  ahmilb
* Include libgen.h for all platforms.
*
* Revision 1.6  2002/01/30 16:10:33  ahmilb
* Change ifdef -- endif around libgen.h
*
* Revision 1.5  2001/09/14 20:17:03  ahmilb
* Change offsets back to 1,1 since row, column start with 0,0
* Set dt to 1000
*
* Revision 1.4  2001/06/19 14:14:01  ahmilb
* Fix initial offset value.
*
* Revision 1.3  2001/03/02 15:22:24  ahmilb
* Restrict ROF model format to binary only
*
* Revision 1.2  2001/02/23 19:50:24  ahhayg
* *** empty log message ***
*
* Revision 1.1  2001/02/07 19:55:45  ahglim
* added bhproffread, bhptemplate
* changed bhpio, bhpread, bhpwrite to handle ENDIAN issues
*
*
*
*
******************************************************************/

#include <stdio.h>
#include "readRoff.h"
#include "su.h"
#include "segy.h"
#include "par.h"
#include "header.h"
#include <libgen.h>

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
"  BHPROFFREAD - export reservoir properties from a ROFF model     ",
"                                                                  ",
"   bhproffread [optional parameters]                              ",
"                                                                  ",
"  Required Parameters: None                                       ",
"                                                                  ",
"  Optional Parameters:                                            ",
"    model=model.roff    Name of ROFF model, must be binary format ",
"    dir=/tmp            Directory to use for output traces        ",
"    keys=ep,cdp         Header keys to use for sort keys          ",
"    offsets=1,1         Initial value to assign to keys           ",
"                        Row and column number start with 0, so the",
"                        first primary,secondary keys default to 1,1",
"    scalars=1,1         Multipliers to apply to model node indices",
"                        to get resulting key values               ",
"                        The primary key value is assigned as      ",
"                        value = offset + factor * ix, where       ",
"                        ix is the model X node index              ",
"                        The secondary key value is assigned as    ",
"                        value = offset + factor * iy, where       ",
"                        iy is the model Y node index              ",
"    verbose=0           Use non-zero for detailed printout        ",
"                                                                  ",
"    All of the properties in the model are exported to a single SU",
"    dataset named 'model'.su  Each trace contains np*nz+nz+1      ",
"    samples, consisting of the first property value at each depth,",
"    followed by the secondary property value at each depth, etc., ", 
"    followed by the depths of each layer top, followed by the last",
"    bottom depth  The X,Y coordinates are stored in trace headers ",
"    gx,gy                                                         ",
"                                                                  ",
"                                                                  ",
"                                                                  ",
NULL};

/*  Prototypes */
int openRoff(char *fname, RoffInfo *roffinfo, char **pname, long *oProp,
             int *tProp, int *nProp, int verbose);

/* Globals */
#define FNULL -999.0
#define INULL -999
#define CNULL 255

int main(int argc, char **argv)
{

  char *model;               /* ROFF model file */
  char *file;                /* Output file name = 'model.property.su' */
  char *dir=NULL;            /* Optional output directory */
  char dir_file[256];        /* Dir + file */
  char **property;           /* Up to 256 properties allowed */

  unsigned char *splitEnz;   /* splitEnz from file */
  unsigned char *cdata;      /* Buffer for reading 1 row of discrete properties */
  unsigned char *active;     /* active flags from file */

  float *cL;                 /* Corner lines */
  float *xct, *yct;          /* XYs at surface */
  float **traces;            /* Traces containing properties,depths for 1 row */
  float *zvals;              /* 1 row of corner point depths */
  float *fdata;              /* 1 row of floating properties */
  float xd, yd, dist;

  int nxp, nyp, nzp;         /* Number of nodes */
  int i, j, k, m;            /* Loop counters */
  int ix, iy, iz;            /* Model indices */
  int verbose;               /* Debug */
  int tProp[256];            /* Type of each property - byte or float */
  int nProp[256];            /* Number of data items, each property */
  int nzvals;                /* Number of zvals to load */
  int memalloc=0;            /* Number of bytes calloc'ed */
  int *idata;                /* 1 row of integer properties */
  int ncells=0;              /* Number of active cells */
  int offsets[2];            /* Header key offsets */
  int scalars[2];            /* Header key multipliers */
  int key_index[2];          /* Header key indices */
  int c1=0, c2=0;            /* Null counters */

  long oProp[256];           /* Byte offset of each property (up to 256) */
  long offset;               /* Offset into roff file */

  FILE *fp;                  /* Output file pointer */

  cwp_String keys[2];  /* SU header key names */
  cwp_String type[2];  /* SU header key types */

  Value hval;               /* Trace header value */

  RoffInfo roffinfo;         /* Structure to hold dimensions, etc */

  segy tr;                   /* Output trace */

  /* hook up getpar to handle the parameters */
  initargs(argc,argv);
  /* No stdin required */
  requestdoc(0);

  /* Debug */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* Model */
  if(!getparstring("model",&model))
    model = "model.roff";

  /* Properties - allow 256, 128-char each */
  property = calloc(256,sizeof(char *));
  for(i=0; i<256; i++)
    property[i] = calloc(128,sizeof(char));
  memalloc += 256 * 128;

  /* Dir */
  if(!getparstring("dir",&dir))
    dir = "/tmp";
  if(verbose > 0)
    printf("Output Directory %s\n", dir);

  /* Check keys */
  i = countparval("keys");
  if(i !=2 && i != 0)
    err("Specify 2 or 0 keys\n");
  if(!i) {
    keys[0] = "ep";
    keys[1] = "cdp";
  }
  else
    getparstringarray("keys",keys);
  type[0] = hdtype(keys[0]);
  type[1] = hdtype(keys[1]);

  /* Check offsets */
  i = countparval("offsets");
  if(i !=2 && i != 0)
    err("Specify 2 or 0 offsets\n");
  if(!i) {
    offsets[0] = 1;
    offsets[1] = 1;
  }
  else
    getparint("offsets",offsets);
    
  /* Check scalars */
  i = countparval("scalars");
  if(i !=2 && i != 0)
    err("Specify 2 or 0 scalars\n");
  if(!i) {
    scalars[0] = 1;
    scalars[1] = 1;
  }
  else
    getparint("scalars",scalars);
    
  key_index[0] = getindex(keys[0]);
  key_index[1] = getindex(keys[1]);

  if(verbose > 0) {
    printf("Key: %s, Type: %s, Key: %s, Type: %s\n", keys[0],type[0],keys[1],type[1]);
    printf("Offsets: %d, %d\n", offsets[0],offsets[1]);
    printf("Scalars: %d, %d\n", scalars[0],scalars[1]);
  }
  roffinfo.nx= 0;
  roffinfo.ny= 0;
  roffinfo.nz= 0;
  roffinfo.xo = 0;
  roffinfo.yo = 0;
  roffinfo.zo = 0;
  roffinfo.xscale = 1.0;
  roffinfo.yscale = 1.0;
  roffinfo.zscale = -1.0;
  roffinfo.ocL = 0;
  roffinfo.ncL = 0;
  roffinfo.ocP = 0;
  roffinfo.ncP = 0;
  roffinfo.osplitEnz = 0;
  roffinfo.nsplitEnz = 0;
  roffinfo.oactive = 0;
  roffinfo.nactive = 0;
  roffinfo.nProp = 0;

  i = openRoff(model,&roffinfo,property,oProp,tProp,nProp,verbose);
  if(i)
    err("Error %d in openRoff\n", i);

  if(verbose > 0) {
    printf("DIMS %d, %d, %d\n", roffinfo.nx,roffinfo.ny,roffinfo.nz);
    printf("OFFSETS: %f, %f, %f\n", roffinfo.xo,roffinfo.yo,roffinfo.zo);
    printf("SCALES: %f, %f, %f\n", roffinfo.xscale,roffinfo.yscale,roffinfo.zscale);
    printf("ocL,ncL %d, %d\n", roffinfo.ocL,roffinfo.ncL);
    printf("ocP,ncP %d, %d\n", roffinfo.ocP,roffinfo.ncP);
    printf("osplitEnz,nsplitEnz %d, %d\n", roffinfo.osplitEnz,roffinfo.nsplitEnz);
    printf("oactive,nactive %d, %d\n", roffinfo.oactive,roffinfo.nactive);
    printf("nProp %d\n", roffinfo.nProp);
    printf("MODEL: %s\n",model);
    printf("Properties:\n");
    for(i=0; i<roffinfo.nProp; i++)
      printf("NAME: %s, TYPE: %d, OFFSET: %d, NUM: %d\n",
             property[i],tProp[i],oProp[i],nProp[i]);
  }

  if(!roffinfo.nx || !roffinfo.nx || !roffinfo.nx)
    err("ROFF model dimensions missing or incomplete\n");

  /* Number of nodes */
  nxp = roffinfo.nx + 1;
  nyp = roffinfo.ny + 1;
  nzp = roffinfo.nz + 1;

  if(!roffinfo.ncL)
    err("ROFF model cornerLines missing\n");
  if(!roffinfo.ncP)
    err("ROFF model cornerPoint depths missing\n");
  if(!roffinfo.nProp)
    err("ROFF model has no properties\n");
  if(!roffinfo.nsplitEnz)
    warn("ROFF model splitEnz missing, assuming all ones\n");
  else
    if(roffinfo.nsplitEnz != nxp*nyp*nzp)
      err("Number of splitEnz does not match number of nodes\n");

  /* Corner lines for 1 row */
  cL = calloc(6*nyp,sizeof(float));
  memalloc += 6*nyp * sizeof(float);

  /* XYs for 1 row */
  xct = calloc(nxp*nyp,sizeof(float));
  yct = calloc(nxp*nyp,sizeof(float));
  memalloc += 2 * nxp * nyp * sizeof(float);

  /* All splitEnz */
  splitEnz = calloc(nxp*nyp*nzp,sizeof(char));
  memalloc += nxp * nyp * nzp;

  /* All active */
  active = calloc(roffinfo.nactive,sizeof(char));
  memalloc += roffinfo.nactive;

  /* zvals to hold max number of depths for 1 row */
  zvals = calloc(8*nzp*nyp,sizeof(float));
  memalloc += 8 * nzp * nyp * sizeof(float);

  /* Traces for 1 row */
  traces = calloc(nyp,sizeof(float *));
  for(i=0; i<nyp; i++)
    traces[i] = calloc(roffinfo.nProp*roffinfo.nz+nzp,sizeof(float));
  memalloc += nyp * (roffinfo.nProp * roffinfo.nz * sizeof(float) + nzp * sizeof(float));
  /* Continuous property */
  fdata = calloc(roffinfo.ny*roffinfo.nz,sizeof(float));
  memalloc += roffinfo.ny * roffinfo.nz * sizeof(float);
  idata = calloc(roffinfo.ny*roffinfo.nz,sizeof(int));
  memalloc += roffinfo.ny * roffinfo.nz * sizeof(int);
  /* Discrete property */
  cdata = calloc(roffinfo.ny*roffinfo.nz,sizeof(char));
  memalloc += roffinfo.ny * roffinfo.nz;

  /* Output file */
  file = calloc(256,sizeof(char));
  memalloc += 256;
  file = basename(model);
  if(dir) {
    strcpy(dir_file,dir);
    strcat(dir_file,"/");
    strcat(dir_file,file);
  }
  else {
    strcpy(dir_file,file);
  }
  strcat(dir_file,".su");
  if(verbose > 0)
    printf("Opening %s\n", dir_file);
  fp = efopen(dir_file,"w");
  tr.tracl = 0;
  tr.tracr = 0;
  tr.cdp = 0;
  tr.ns = roffinfo.nProp * roffinfo.nz + roffinfo.nz + 1;
  tr.dt = 4000;
  tr.scalco = -100;
  tr.counit = 1;
  tr.duse = 1;
  tr.trid = 1;
  tr.offset = 0;
  tr.delrt = 0;
  tr.sx = 0;
  tr.sy = 0;
  if(verbose > 0)
    printf("ns,dt %d %d\n", tr.ns, tr.dt);

  /* Get XYs */
  for(ix=0; ix<nxp; ix++) {
    offset = roffinfo.ocL + 6 * ix * nyp * sizeof(float);
    if(verbose > 1)
      printf("SEEK to %d for ROW %d of XYs\n", offset,ix);
    i = fseek(file_,offset,SEEK_SET);
    i = fread(cL,sizeof(float),6*nyp,file_);
    for(j=0; j<nyp; j++) {
      if(verbose > 1)
        printf("x,y,z before transformation and scaling: %f,%f,%f\n",cL[6*j+3],cL[6*j+4],cL[6*j+5]);
      xct[ix*nyp+j] = (cL[6*j+3] + roffinfo.xo) * roffinfo.xscale;
      yct[ix*nyp+j] = (cL[6*j+4] + roffinfo.yo) * roffinfo.yscale;
      if(verbose > 1) {
        printf("TOP: %f, %f, %f\n", xct[ix*nyp+j],yct[ix*nyp+j],
               (cL[6*j+5]+roffinfo.zo)*roffinfo.zscale);
        printf("BOT: %f, %f, %f\n",(cL[6*j]+roffinfo.xo)*roffinfo.xscale,
               (cL[6*j+1]+roffinfo.yo)*roffinfo.yscale,(cL[6*j+2]+roffinfo.zo)*roffinfo.zscale);
      }
    }
    if(verbose > 1)
      printf("\n");
  }
 
  /* Get splitEnz */
  if(roffinfo.nsplitEnz) {
    offset = roffinfo.osplitEnz;
    if(verbose > 1)
      printf("SEEK to %d\n", offset);
    i = fseek(file_,offset,SEEK_SET);
    i = fread(splitEnz,sizeof(char),roffinfo.nsplitEnz,file_);
    if(i != roffinfo.nsplitEnz)
      err("Error loading splitEnz\n");
  }
  else
    for(i=0; i<nxp*nyp*nzp; i++)
      splitEnz[i] = (unsigned char)1;

  /* Get active cell flags */
  offset = roffinfo.oactive;
  i = fseek(file_,offset,SEEK_SET);
  i = fread(active,sizeof(char),roffinfo.nactive,file_);
  if(i != roffinfo.nactive)
    err("Error loading active\n");

  /* Count active cells */
  for(i=0; i<roffinfo.nactive; i++)
    if((int)active[i] == 1)
      ncells++;
  if(verbose > 0)
    printf("%d active cells\n", ncells);

  /* Initialize offset to beginning of cornerPoint depths */
  offset = roffinfo.ocP;
  /* Load rows of properties and depths to traces */
  for(ix=0; ix<nxp; ix++) {
    if(verbose > 1)
      printf("ROW %d:\n", ix);
    /* Skip last row; edge gets same properties as prevoius row */
    if(ix < roffinfo.nx) {
      for(i=0; i<roffinfo.nProp; i++) {
        if(tProp[i] == ISKEYF) {
          if(verbose > 1)
            printf("SEEK property %d, row %d at %d\n", i,ix,
                   oProp[i]+(ix*roffinfo.ny*roffinfo.nz*sizeof(float)));
          j = fseek(file_,oProp[i]+(long)(ix*roffinfo.ny*roffinfo.nz*sizeof(float)),SEEK_SET);
          k = fread(fdata,sizeof(float),roffinfo.ny*roffinfo.nz,file_);
        }
        else if(tProp[i] == ISKEYI) {
          if(verbose > 1)
            printf("SEEK property %d, row %d at %d\n", i,ix,
                   oProp[i]+(ix*roffinfo.ny*roffinfo.nz*sizeof(int)));
          j = fseek(file_,oProp[i]+(long)(ix*roffinfo.ny*roffinfo.nz*sizeof(int)),SEEK_SET);
          k = fread(idata,sizeof(int),roffinfo.ny*roffinfo.nz,file_);
        }
        else if(tProp[i] == ISKEYU) {
          if(verbose > 1)
            printf("SEEK property %d, row %d at %d\n", i,ix,
                   oProp[i]+(ix*roffinfo.ny*roffinfo.nz*sizeof(char)));
          j = fseek(file_,oProp[i]+(long)(ix*roffinfo.ny*roffinfo.nz*sizeof(char)),SEEK_SET);
          k = fread(cdata,sizeof(char),roffinfo.ny*roffinfo.nz,file_);
        }
        for(j=0; j<roffinfo.ny; j++) {
          for(k=0; k<roffinfo.nz; k++) {
            if(tProp[i] == ISKEYU) {
              traces[j][i*roffinfo.nz+roffinfo.nz-k-1] = (float)cdata[j*roffinfo.nz+k];
              /*printf("Trace %d from data %d is %f\n", j,j*roffinfo.nz+k,(float)cdata[j*roffinfo.nz+k]);*/
              if(traces[j][i*roffinfo.nz+roffinfo.nz-k-1] == 255.)
                traces[j][i*roffinfo.nz+roffinfo.nz-k-1] = -999.;
            }
            else if(tProp[i] == ISKEYI) {
              traces[j][i*roffinfo.nz+roffinfo.nz-k-1] = (float)idata[j*roffinfo.nz+k];
              /*printf("Trace %d from data %d is %f\n", j,j*roffinfo.nz+k,(float)idata[j*roffinfo.nz+k]);*/
            }
            else if(tProp[i] == ISKEYF) {
              traces[j][i*roffinfo.nz+roffinfo.nz-k-1] = fdata[j*roffinfo.nz+k];
              /*printf("Trace %d from data %d is %f\n", j,j*roffinfo.nz+k,fdata[j*roffinfo.nz+k]);*/
            }
          }
        }
      }
    }
    /* Offset into splitEnz */
    j = ix * nyp * nzp;
    if(verbose > 1) {
      printf("SPLITS:\n");
      for(k=j; k<j+nyp*nzp; k++) {
        printf("%d ", splitEnz[k]);
        if(!((k+1)%8))
          printf("\n");
      }
      printf("\n");
    }
    /* Accumulate number of values to get */
    nzvals = 0;
    for(k=j; k<j+nyp*nzp; k++)
      nzvals += (int)splitEnz[k];
    if(verbose > 1)
      printf("NZVALS: %d, OFFSET: %d\n", nzvals,offset);
    i = fseek(file_,offset,SEEK_SET);
    i = fread(zvals,sizeof(float),nzvals,file_);
    /* Update offset */
    offset += nzvals * sizeof(float); 
    if(verbose > 1)
      printf("ZVALS:\n");
    for(k=0; k<nzvals; k++)
      zvals[k] = (zvals[k] + roffinfo.zo) * roffinfo.zscale;
    if(verbose > 1) {
      for(k=0; k<nzvals; k++) {
        printf("%f ", zvals[k]);
        if(!((k+1)%4))
          printf("\n");
      }
    }
    /* Offset into splitEnz */
    j = ix * nyp * nzp;
    /* Move cornerPoint depths to traces */
    iz = 0;
    for(i=0; i<nyp; i++) {
      for(m=roffinfo.nProp*roffinfo.nz+roffinfo.nz; m>=roffinfo.nProp*roffinfo.nz; m--) {
        if((int)splitEnz[j] == 1) {
          traces[i][m] = zvals[iz];
          j++;
          iz++;
        }
        else if((int)splitEnz[j] == 4) {
          /* If not last edge, take NE value */
          if(iy < roffinfo.ny)
            traces[i][m] = zvals[iz+3];
          /* Else take SE value */
          else
            traces[i][m] = zvals[iz+1];
          iz += 4;
          j++;
        }
        else
          err("splitEnz = %d not implemented\n", (int)splitEnz[j]);
      }
    }
    /* Pass row of traces */
    for(j=0; j<nyp; j++) {
      for(k=0; k<tr.ns; k++)
        tr.data[k] = traces[j][k];
      /* Edge trace property is same as previous trace */
      if(j == roffinfo.ny) {
        /*printf("Copy trace %d to trace %d\n", ix*nyp+j,ix*nyp+j+1);*/
        for(k=0; k<roffinfo.nProp*roffinfo.nz; k++)
          tr.data[k] = traces[j-1][k];
      }
      tr.gx = xct[ix*nyp+j] * 100;
      tr.gy = yct[ix*nyp+j] * 100;
      tr.tracl++;
      tr.tracr++;
      /* Set dt to 10 so bhpread will set units to meters */
      tr.dt = 10000;
      hval.i = offsets[0] + scalars[0] * ix;
      puthval(&tr,key_index[0],&hval);
      hval.i = offsets[1] + scalars[1] * j;
      puthval(&tr,key_index[1],&hval);
      for(k=0; k<roffinfo.nProp*roffinfo.nz; k++)
        if(tr.data[k] != -999)
          c1++;
        else
          c2++;
      fputtr(fp,&tr);
    }
  }

  efclose(fp);

  if(verbose > 0) {
    printf("MEMALLOC %d bytes\n", memalloc);
    printf("%d non-NULLS, %d NULLS\n", c1,c2);
  }

  return EXIT_SUCCESS;

}

int openRoff(char *fname, RoffInfo *roffinfo, char **pname, long *oProp,
             int *tProp, int *nProp, int verbose)

{

  int error=0;
  int itype, nd;
  int *id;

  char **cd;
  char stype[STL];
  char stag[STL];
  
  unsigned char* ud;

  float *fd;

  double *dd;

  initRoffRead();

  if(verbose > 0)
    printf("Opening %s\n", fname);
  error = openFile(fname,"rb");
  if(error)
    return error;
  if(!binary_) {
    err("ROFF model file must be binary format");
    return -99;
  }

  if(binary_ && verbose > 1)
    printf("At byte %d: , file is binary\n",ftell(file_));

  for(;;) {

    if((error = readNextItem(&itype,stype,&cd,&id,&ud,&fd,&dd,&nd)))
      return error;

    if( itype==ISTAG ) strcpy(stag,stype);

    if(verbose > 1)
      printf("At byte %d: itype,stag,stype,nd %d, %s, %s, %d\n",ftell(file_),itype,stag,stype,nd);

    if(!strcmp(stag,"eof")) {
      if(verbose > 0)
        printf("EOF at %d bytes\n", ftell(file_));
      rewind(file_);
      break;
    }

    if(itype  == ISKEYI && !strcmp(stype,"nX"))
      roffinfo->nx = id[0];
    if(itype  == ISKEYI && !strcmp(stype,"nY"))
      roffinfo->ny = id[0];
    if(itype  == ISKEYI && !strcmp(stype,"nZ"))
      roffinfo->nz = id[0];

    if(itype == ISKEYF && !strcmp(stype,"xoffset"))
      roffinfo->xo = fd[0];
    if(itype == ISKEYF && !strcmp(stype,"yoffset"))
      roffinfo->yo = fd[0];
    if(itype == ISKEYF && !strcmp(stype,"zoffset"))
      roffinfo->zo = fd[0];

    if(itype == ISKEYF && !strcmp(stype,"xscale"))
      roffinfo->xscale = fd[0];
    if(itype == ISKEYF && !strcmp(stype,"yscale"))
      roffinfo->yscale = fd[0];
    if(itype == ISKEYF && !strcmp(stype,"zscale"))
      roffinfo->zscale = fd[0];

    if(itype == ISKEYF && !strcmp(stag,"cornerLines") && !strcmp(stype,"data")) {
      roffinfo->ncL = nd;
      roffinfo->ocL = ftell(file_) - roffinfo->ncL * sizeof(float);
    }

    if(itype == ISKEYU && !strcmp(stag,"zvalues") && !strcmp(stype,"splitEnz")) {
      roffinfo->nsplitEnz = nd;
      roffinfo->osplitEnz = ftell(file_) - roffinfo->nsplitEnz;
    }

    if(itype == ISKEYF && !strcmp(stag,"zvalues") && !strcmp(stype,"data")) {
      roffinfo->ncP = nd;
      roffinfo->ocP = ftell(file_) - roffinfo->ncP * sizeof(float);
    }

    if(itype == ISKEYB && !strcmp(stag,"active") && !strcmp(stype,"data")) {
      roffinfo->nactive = nd;
      roffinfo->oactive = ftell(file_) - roffinfo->nactive;
    }

    if(itype == ISKEYC && !strcmp(stag,"parameter") && !strcmp(stype,"name"))
      strcpy(pname[roffinfo->nProp],cd[0]);
 
    if((itype == ISKEYF || itype == ISKEYU || itype == ISKEYI)
      && !strcmp(stag,"parameter") && !strcmp(stype,"data")) {
      tProp[roffinfo->nProp] = itype;
      nProp[roffinfo->nProp] = nd;
      if(itype == ISKEYF)
        oProp[roffinfo->nProp] = ftell(file_) - nd * sizeof(float);
      else if(itype == ISKEYI)
        oProp[roffinfo->nProp] = ftell(file_) - nd * sizeof(int);
      else if(itype == ISKEYU)
        oProp[roffinfo->nProp] = ftell(file_) - nd * sizeof(char);
      roffinfo->nProp++;
    }
  }

  return error;

}

void initRoffRead()
{
  int i;

/*--Status flags and counts-----------------------------------------------------*/

  fileIsOpen_    =   0;
  inTag_         =   0;
  file_          =NULL;
  binary_        =   1;
  nvalues_       =   0;
  strcpy(cvalues_,"undef");
  nwritten_      =   0;
 
/*--Buffer arrays---------------------------------------------------------------*/

  ncbuf_ = 0;
  cbuf_  = 0;
  ibuf_  = 0;
  fbuf_  = 0;
  dbuf_  = 0;
  ubuf_  = 0;

/*--Set up error values---------------------------------------------------------*/

  lastError_ = 0;
  strcpy(lastErrorMessage_,"No errors");

  for(i=0;i<MERRORS;i++)
  {
    errorString_[i]=(char*) malloc(STL*sizeof(char));
    errorFlag_  [i]=i;
  }
 
  strcpy(errorString_[ 0],"No errors"                                                   );
  strcpy(errorString_[ 1],"File is already open, call closeFile to close"               );
  strcpy(errorString_[ 2],"String too long for buffer (CBUFF)"                          );
  strcpy(errorString_[ 3],"Unable to recognise mode, should be r,w,a,rb,wb or ab"       );
  strcpy(errorString_[ 4],"Unable to open file"                                         );
  strcpy(errorString_[ 5],"File is not yet open, cannot close"                          );
  strcpy(errorString_[ 6],"Unable to close file"                                        );
  strcpy(errorString_[ 7],"File name is too long: limit is 127 characters"              );
  strcpy(errorString_[ 8],"Tag is already open"                                         );
  strcpy(errorString_[ 9],"Unable to write tag value"                                   );
  strcpy(errorString_[10],"Tag is not open"                                             );
  strcpy(errorString_[11],"Unable to write endtag key"                                  );
  strcpy(errorString_[12],"Keys with no associated data must start with a"              );
  strcpy(errorString_[13],"Unable to write keyword"                                     );
  strcpy(errorString_[14],"Key with associated string data must start with s"           );
  strcpy(errorString_[15],"Unable to write keyword and associated string"               );
  strcpy(errorString_[16],"Key with associated double data must start with d"           );
  strcpy(errorString_[17],"Unable to write keyword and associated double value"         );
  strcpy(errorString_[18],"Wrong initial letter for key with associated float data"     );
  strcpy(errorString_[19],"Unable to write keyword and associated float data"           );
  strcpy(errorString_[20],"Key with associated boolean data must start with b"          );
  strcpy(errorString_[21],"Unable to write keyword and associated boolean data"         );
  strcpy(errorString_[22],"Key with associated int data must start with i,j,k,l,m or n" );
  strcpy(errorString_[23],"Unable to write keyword and associated int data"             );
  strcpy(errorString_[24],"Type of values written does not agree with tag"              );
  strcpy(errorString_[25],"Number of values written does not agree with tag"            );
  strcpy(errorString_[26],"Unable to write float array"                                 );
  strcpy(errorString_[27],"Unable to read  float array"                                 );
  strcpy(errorString_[28],"Unable to write comment"                                     );

  nErrors_=29;

}

/*==============================================================================
  ==============================================================================
  Destructor 
  ==============================================================================
  ==============================================================================*/

void release()
{

/*--Free the error strings-----------------------------------------------------*/

  int i;
  for(i=0;i<MERRORS;i++)
  {
    free(errorString_[i]);
  }

/*--Free the buffer arrays-----------------------------------------------------*/

  if( cbuf_ )
  {
    for(i=0;i<ncbuf_;i++)
    {
      if( cbuf_[i] )
      {
        free(cbuf_[i]);cbuf_[i]=0;
      }
    }
    free(cbuf_);cbuf_=0;
  }
  if( dbuf_ ) free(dbuf_);
  if( fbuf_ ) free(fbuf_);
  if( ibuf_ ) free(ibuf_);
  if( ubuf_ ) free(ubuf_);

}

/*==============================================================================
  ==============================================================================
  Methods for opening and closing files
  ==============================================================================
  ==============================================================================*/

/*==============================================================================
  Open file
  ==============================================================================
  fname - input - name of file to be opened 
  mode  - input - mode of file to be opened - one of r,w,a,rb,wb,ab
  ==============================================================================*/

int openFile(char* fname,char* mode)
{
  int error=0;
  int isr,isw,isa,isrb,iswb,isab;

/*------------------------------------------------------------------------------
  Check that file is not already open
  ------------------------------------------------------------------------------*/

  if( fileIsOpen_ )
  {
    error=setError(1);
    return error;
  }

/*------------------------------------------------------------------------------
  Check that the mode flag is valid and set binary switch
  ------------------------------------------------------------------------------*/

  isr =compare(mode,"r" );
  isw =compare(mode,"w" );
  isa =compare(mode,"a" );

  isrb=compare(mode,"rb");
  iswb=compare(mode,"wb");
  isab=compare(mode,"ab");

  if( !isr && !isw && !isa && !isrb && !iswb && !isab )
  {
    error=setError(3);
    return error;
  }
  else
  {
     if( isrb || iswb || isab ) binary_=1;
     if( isr  || isw  || isa  ) binary_=0;
  }

/*------------------------------------------------------------------------------
  Open and store FILE* pointer in file_
  ------------------------------------------------------------------------------*/

  if( strlen(fname)>STL-5 )
  {
    error=setError(7);
    return error;
  }

  strncpy(fname_,fname,STL);
  
  if( isr  ) file_=fopen(fname_,"r" );              /* Open for ascii  read   */
  if( isw  ) file_=fopen(fname_,"w" );              /* Open for ascii  write  */
  if( isa  ) file_=fopen(fname_,"a" );              /* Open for ascii  append */
  if( isrb ) file_=fopen(fname_,"rb");              /* Open for binary read   */
  if( iswb ) file_=fopen(fname_,"wb");              /* Open for binary write  */
  if( isab ) file_=fopen(fname_,"ab");              /* Open for binary append */

  if( file_==NULL )
  {
    error=setError(3);
  }
  else				 
  { 
    int binary;
    readIsBinary(&binary_);
    fileIsOpen_=1;
    error=0;
  }
  
  return error;
}

/*==============================================================================
  Close file
  ==============================================================================*/

int closeFile()
{

  int error=0,ioss;

/*------------------------------------------------------------------------------
  Check that file is open
  ------------------------------------------------------------------------------*/

  if( !fileIsOpen_ )
  {
    error=setError(4);
    return error;
  }

/*------------------------------------------------------------------------------
  Close the file
  ------------------------------------------------------------------------------*/

  ioss=fclose(file_);
  if( ioss )
  {
    error=setError(5);
  }

  return error;
}

/*==============================================================================
  Read next item
  ==============================================================================
  itype - output - flag identifying the next item
  stype - output - string identifying the next item
  cd    - output - location of character data
  id    - output - location of integer   data
  ud    - output - location of byte      data
  fd    - output - location of float     data
  dd    - output - location of double    data
  nd    - output - number of data values read
  ==============================================================================*/

int readNextItem( int*         itype
                 ,char*        stype
                 ,char***         cd
			     ,int**           id
                 ,unsigned char** ud
                 ,float**         fd
                 ,double**        dd
                 ,int*            nd )
{
  int  error,ns,i;
  char c;
  char keyword[STL];
  
/*------------------------------------------------------------------------------
  Initialise output arguments
  ------------------------------------------------------------------------------*/

  strncpy(stype  ," ",STL);
  strncpy(keyword," ",STL);
  
/*------------------------------------------------------------------------------
  Find start of next thing on file
  ------------------------------------------------------------------------------*/

  error=readtoNonDelim(&c);
  if( error )
  { 
    goto exit;
  }

/*------------------------------------------------------------------------------
  Read first character
  ------------------------------------------------------------------------------*/

  stype[0]=c;ns=1;

/*---Special case of comment----------------------------------------------------*/

  if( c=='#' )
  {
    *itype = ISCOMMENT;
    
    if(binary_) 
    {
      error=readtoChar('\0',stype,&ns,STL);
    }
    else 
    {
      error=readtoChar('#' ,stype,&ns,STL);
    }
    goto exit;
  }

/*------------------------------------------------------------------------------
  Read the remainder
  ------------------------------------------------------------------------------*/

  error=readtoDelim(stype,&ns,STL);

/*--Case of tag-----------------------------------------------------------------*/
 
  if( compare(stype,"tag") )
  {
    inTag_=1;
    *itype =ISTAG;
    getArgS(stype,STL);
    nvalues_=0;
    goto exit;
  }

/*--Case of endtag--------------------------------------------------------------*/

  if( compare(stype,"endtag") )
  {
    inTag_=0;
    *itype =ISENDTAG;
    goto exit;
  }
  
/*------------------------------------------------------------------------------
  If in a tag, anything else is a keyword
  ------------------------------------------------------------------------------*/

  if( inTag_ )
  {
    int is_array = 0;
    *nd = 1;
    if(compare(stype,str_array))
    {
      is_array = 1;
      error=readtoNonDelim(&c);
      if( error ) goto exit;
      stype[0] = c;
      ns=1;
      error=readtoDelim(stype,&ns,STL);
    }
    
    error=readtoNonDelim(&c);
    if( error ) goto exit;

    strcpy(cvalues_, stype);
    
    keyword[0] = c; ns=1;
    error=readtoDelim(keyword,&ns,STL);
    
    if(isThisVoid(stype))
      *itype=ISKEYA;
    else if(isThisBool(stype))
      *itype=ISKEYB;
    else if(isThisByte(stype))
      *itype=ISKEYU;
    else if(isThisInt(stype))
      *itype=ISKEYI;
    else if(isThisFloat(stype))
      *itype=ISKEYF;
    else if(isThisDouble(stype))
      *itype=ISKEYD;
    else if(isThisChar(stype))
      *itype=ISKEYC;
    else 
    {
      error = -1;
      goto exit;
    }
 
    strcpy(stype,keyword);

    if(is_array) 
    {
      getArgI(nd);
    }

    nvalues_ = *nd;
    
    switch(*itype) 
    {
      case ISKEYA: /* no associated data */
        nd = 0;
        break;
      case ISKEYB: /* boolean data */
        if( ubuf_ ) free(ubuf_);
        ubuf_ = malloc(nvalues_*sizeof(unsigned char));
        *ud    = ubuf_;
        error = readDataB(ubuf_,nvalues_);
        break;
      case ISKEYU:
        if(ubuf_) free(ubuf_);
        ubuf_ = malloc(nvalues_*sizeof(unsigned char));
        *ud = ubuf_;
        error = readDataU(ubuf_, nvalues_);
        break;
      case ISKEYC: /* character data */
        if( cbuf_ ) 
        {
          for(i=0;i<ncbuf_;i++) 
          {
	        if( cbuf_[i] ) 
            {
              free(cbuf_[i]);cbuf_[i]=0;
            }
          }
          free(cbuf_);cbuf_=0;
        }

        ncbuf_=nvalues_;
        cbuf_ =malloc(ncbuf_*sizeof(char*));
        for(i=0;i<ncbuf_;i++) cbuf_[i]=0;

        error=readDataC(cbuf_,nvalues_);
        *cd   =cbuf_;
        break;
      case ISKEYD: /* double data */
        if( dbuf_ ) free(dbuf_);
        dbuf_=malloc(nvalues_*sizeof(double));
        error=readDataD(dbuf_,nvalues_);
        *dd   =dbuf_;
        break;
      case ISKEYI: /* int data */
        if( ibuf_ ) free(ibuf_);
        ibuf_=malloc(nvalues_*sizeof(int));
        error=readDataI(ibuf_,nvalues_);
        *id   =ibuf_;
        break;
      case ISKEYF: /* float data */
        if( fbuf_ ) free(fbuf_);
        fbuf_=malloc(nvalues_*sizeof(float));
        error=readDataF(fbuf_,nvalues_);
        *fd   =fbuf_;
        break;
      default:
        error = -1;
    }
  }
  else
  {
    error = -1;
  }
  
  exit:return error;
}
 
/*===============================================================================
  Read a set of characters terminated by a general delimiter
  ===============================================================================
  s  - output - delimited string
  ms - input  - maximum length of string (including terminator)
  ===============================================================================*/
 
int getArgS(char* s,int ms)
{
  int  error,ns;
  char c;

/*--Find start of next thing on file--------------------------------------------*/

  error=readtoNonDelim(&c);if( error ) goto exit;

  s[0]=c;ns=1;
  error=readtoDelim(s,&ns,ms);
  s[ns]='\0';
   
  exit:return error;
}

/*===============================================================================
  Read an integer  
  ===============================================================================
  ia - output - integer value
  ===============================================================================*/

int getArgI(int* ia)
{
  int error=0; 

  *ia=0;

  if( binary_ )
  {
    fread(ia,4,1,file_);
  }
  else
  {
    fscanf(file_,"%d",ia);
  }
  return error;
}

/*==============================================================================
  Read a double quote delimuted string
  ==============================================================================
  s  - output - string
  ns - output - length of string         (not including terminator)
  ms - input  - maximum length of string (    including terminator)
  ==============================================================================*/

int getString(char* s,int* ns,int ms)
{
  int  error=0,n;

  s[0] = '\0';
  
  if(binary_) 
  {
    error=readtoChar('\0',s,ns,ms);
  }
  else 
  {
    char c;

    error=readtoNonDelim(&c);
    if( error )
    { 
      goto exit;
    }

/*--Read it into string-----------------------------------------------------------*/

    if(c != '\"') 
    {
      s[0] = '\0';
      return 29;
    }

    *ns=0;
    error=readtoChar('\"',s,ns,ms);
    n=*ns;
    if( n>0 && s[n-1] == '\"') 
    {
      n--;
    }
    s[n] = '\0';
    *ns=n+1;
  }

  exit:return error;
}

/*================================================================================
  Read file until a non-delimiter character is found, and return it
  ================================================================================
  c - output - non-delimiter
  ================================================================================*/

int readtoNonDelim(char* c)
{
  int nread,error=0;

  *c=' ';
  while( *c==' ' || *c=='\n' || *c=='\0')
  {
    nread=fread(c,1,1,file_);
    if( nread!=1 ) 
    {
      error=1;
      break;
    }
  } 
  return error;
}


/*=================================================================================
  Read through input file to a given character
  =================================================================================
  c  - input  - character to be read to 
  s  - output - data read
  ns - output - number of items read     (not including terminator)
  ms - input  - maximum length of string (    including terminator)
  =================================================================================*/

int readtoChar(char c,char* s,int* ns,int ms)
{
  int nread,error=0;
  char cr=' ';
  int n;
  
  n=*ns;
  while( cr!=c && n<ms-1 )
  {
    nread=fread(&cr,1,1,file_);
    if( nread!=1 ) 
    {
      error=1;
      break;
    }
    else
    {
      s[n]=cr;
      n++;
    }
  }
  s[n]='\0';
  *ns=n;
  return error;
}

/*================================================================================
  Read file until a delimiter character is found
  ================================================================================
  s  - output - string read
  ns - output - number of items read     (not including terminator)
  ms - input  - maximum length of string (    including terminator)
  ================================================================================*/

int readtoDelim(char* s,int* ns,int ms)
{
  int nread,error=0,n;
  char c='X';

  n=*ns;
  while( !error && n<ms-1 )
  {
    nread=fread(&c,1,1,file_);
    if( nread!=1 ) 
    {
      error=1;
    }
    else
    {
      if( c==' ' || c=='\n' || c=='\0') break;
      s[n]=c;
      n++;
    }
  }
  s[n]='\0';
  *ns=n;

  return error;
}

 
/*==============================================================================
  ==============================================================================
  The next section contains members for reading bulk data
  ==============================================================================
  ==============================================================================*/

/*==============================================================================
  Read an array of booleans (as integers)
  ==============================================================================*/

int readDataB(unsigned char* data,int number)
{
  int error;
  
  error=readDataU(data,number);

  return error;
}

/*==============================================================================
  Read an array of character strings
  ==============================================================================*/

int readDataC(char** data,int number)
{

  int i,error=0,iret=0,ns;
  char cbuf[CBUFF];
  
  for(i=0;i<number;i++)
  {
    ns=0;
    iret=getString(cbuf,&ns,CBUFF);
    if( ns>CBUFF-2 )
    {
      error=setError(2);
    }
/*
    data[i]=malloc((ns+1)*sizeof(char));
    strncpy(data[i],cbuf,ns+1);
*/
    data[i]=malloc((ns)*sizeof(char));
    strncpy(data[i],cbuf,ns);
  }

  if( iret<0 )
  {
    error=setError(28);
  }
 
  return error;
}

/*==============================================================================
  Read an array of ints
  ==============================================================================*/

int readDataI(int* data,int number)
{

  int i,error=0,iret;
  int rvalue;

  if(  binary_ )
  {
    iret=breadi4(data,number);
  }

  if( !binary_ )
  {
    i=0;
    while(i<number)
    {
      iret=fscanf(file_,"%d",&rvalue);
      data[i]=rvalue;
      i++;
    }
  }

  if( iret<0 )
  {
   error=setError(28);
  }
 
  return error;
}

int readDataU(unsigned char* data,int number) {
  
  int i,error=0,iret;
  int rvalue;

  if(  binary_ )
  {
    iret=breadi1(data,number);
  }

  if( !binary_ )
  {
    i=0;
    while(i<number)
    {
      iret=fscanf(file_,"%d",&rvalue);
      data[i]=rvalue;
      i++;
    }
  }

  if( iret<0 )
  {
   error=setError(28);
  }
 
  return error;
}


/*==============================================================================
  Read an array of floats
  ==============================================================================*/

int readDataF(float* data,int number)
{

  int i,error=0,iret;
  float rvalue;

  if(  binary_ )
  {
    iret=breadf4(data,number);
  }

  if( !binary_ )
  {
    i=0;
    while(i<number)
    {
      iret=fscanf(file_,"%f",&rvalue);
      data[i]=rvalue;
      i++;
    }
  }

  if( iret<0 )
  {
   error=setError(28);
  }
 
  return error;
}

/*==============================================================================
  Read an array of doubles
  ==============================================================================*/

int readDataD(double* data,int number)
{

  int i,error=0,iret;
  float rvalue;

  if(  binary_ )
  {
    iret=breadd8(data,number);
  }

  if( !binary_ )
  {
    i=0;
    while(i<number)
    {
      iret=fscanf(file_,"%f",&rvalue);
      data[i]=rvalue;
      i++;
    }
  }

  if( iret<0 )
  {
   error=setError(28);
  }
 
  return error;
}

 
/*===========================================================================
  breadi1 - reads 1 byte from a file
  ===========================================================================*/
int breadi1(unsigned char* uvals,int nitem) {
  int nread, error=0;
  nread=fread(uvals,1,nitem,file_);
  if( nread!=nitem ) error=-1;

  return error;  
}

/*===========================================================================
  breadi4 - reads 4 byte words from a file
  ===========================================================================*/

int breadi4(int* ivals,int nitem )
{
  int nread,error=0;

  nread=fread(ivals,4,nitem,file_);
  if( nread!=nitem ) error=-1;

  return error;
}

/*===================================================================
  creadf4 - reads 4 byte words from a file
  ===================================================================*/

int breadf4(float* fvals,int nitem )
{
  int nread,error=0;

  nread=fread(fvals,4,nitem,file_);
  if( nread!=nitem ) error=-1;
  
  return error;
}

/*===================================================================
  creadf8 - reads 8 byte words from a file
  ===================================================================*/

int breadd8(double* dvals,int nitem )
{
  int nread,error=0;

  nread=fread(dvals,8,nitem,file_);
  if( nread!=nitem ) error=-1;
  
  return error;
}


int isThisVoid  (char* name) 
{
  return compare(name, str_void);
}

/*==============================================================================
  Check if a given mnemonic corresponds to an int
  ==============================================================================*/

int isThisInt(char* name)
{
  return compare(name,str_int);
}

/*==============================================================================
  Check if a given mnemonic corresponds to a bool
  ==============================================================================*/

int isThisBool(char* name)
{
  return compare(name, str_bool);
}

/*==============================================================================
  Check if a given mnemonic corresponds to a char
  ==============================================================================*/

int isThisChar(char* name)
{
  return compare(name, str_char);
}


/*==============================================================================
  Check if a given mnemonic corresponds to a double
  ==============================================================================*/

int isThisDouble(char* name)
{
  return compare(name, str_double);
}

/*==============================================================================
  Check if a given mnemonic corresponds to a float
  ==============================================================================*/

int isThisFloat(char* name)
{
  return compare(name, str_float);
}

/*==============================================================================
  Check if a given mnemonic corresponds to a byte
  ==============================================================================*/

int isThisByte  (char* name)
{
  return compare(name, str_byte);
}

/*==============================================================================
  Set last error and last error string 
  ==============================================================================*/
 
int setError(int ierror)
{
  lastError_=ierror;
  if( ierror>-1 && ierror<nErrors_ )
  {
    strcpy(lastErrorMessage_,errorString_[ierror]);
  }
  return ierror;
}

/*==============================================================================
  Get last recorded error
  ==============================================================================*/

int getLastError( )
{
  return lastError_;
}

/*==============================================================================
  Get last recorded error message
  ==============================================================================*/

char* getLastErrorMessage()
{
  return lastErrorMessage_;
}

/*==============================================================================
  Get error message corresponding to error flag
  ==============================================================================*/

char* getErrorMessage(int ierror)
{
  if( ierror>-1 && ierror<nErrors_ )
  {
    return errorString_[ierror];
  }
  else
  {
    return errorString_[0];
  }
}

/*==============================================================================
  Routine to do case-independent comparisons
  ==============================================================================*/

int compare(char* a,char* b)
{
  int i,ia,ib;
  int theSame=1;
  for(i=0;i<STL;i++)
  {
    if( a[i]=='\0' || b[i]=='\0' ) break;
    ia=(int) a[i];
    ib=(int) b[i];
    ia=tolower(ia);
    ib=tolower(ib);
    if( ia!=ib )
    {
      theSame=0;
      break;
    }
  }
  if(a[i] != b[i])
    theSame = 0;
  
  return theSame;
}
  
int readIsBinary(int* binary)
{
  char buf[9];
  int error=0;
  int nread = fread(buf,1,8,file_);
  buf[8] = '\0';
  if(compare(buf, str_initasc))
    *binary = 0;
  else if(compare(buf, str_initbin))
    *binary = 1;
  else
    error = -1;
  
  return error;
}
