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
#include "par.h"
#include "su.h"
#include "segy.h"
#include "bhp_hdr.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                                  ",
"                                                                  ",
" bhpnavmerge < input p190=file [optional parameters] > output     ",
"                                                                  ",
"                                                                  ",
" Required Parameters:                                             ",
"  p190=path          Path and filename of P190 data               ",
"                                                                  ",
" Optional Parameters:                                             ",
"    verbose=0          For debug print                            ",
"    maxerrors=0        Number of errors to allow before quitting, ",
"                        where an error is defined as having an    ",
"                        input trace for which no matching nav     ",
"                        record exists                             ",
NULL};

char rec[80];           /* One ASCII record */
char string[80];        /* String for rec->variable transfer */
#define MOVEI(a,b,c) strncpy(string,&rec[a],b); string[b] = '\0'; c = atoi(string)
#define MOVEF(a,b,c) strncpy(string,&rec[a],b); string[b] = '\0'; c = atof(string)

segy tr;

int main(int argc, char **argv)
{

  char *path;             /* Path to P190 file */
  char ns[2], ew[2];      /* Hemispheres */
  char recid;             /* Record type */
  char line[61];          /* Line name  - first S record[2:13] + H0203 record[33:80] */

  FILE *fp;               /* p190 file */

  float convert;          /* Metric conversion factor grid units */
  float height;           /* Metric conversion factor height units */
  float lats, lons;       /* Latitude, longitude - seconds */
  float east, north;      /* Easting, Northing of grid origin */
  float *sx, *sy;         /* Source X,Y's */
  float **gx, **gy;       /* Receiver X,Y's */
  float *swdep;           /* Water depth */
  float **gelev;          /* Streamer depth */
  float offset;           /* Source-receiver offset (from sx,sy--->gx,gy) */
  
  double temp;            /* Needed for setval1 */ 

  int i, j;               /* Loop counter */
  int latd, latm;         /* latitude - degrees, minutes */
  int lond, lonm;         /* longitude - degrees, minutes */
  int *shot;              /* Shot-point numbers */
  int *day;               /* Julian day of year */
  int *hour;              /* Hour */
  int *min;               /* Minute */
  int *sec;               /* Second */
  int nsrec=0;            /* Number of source records in p190 file */
  int nums=0;             /* Number of source records found so far */
  int ngrec;              /* Number of R records required per S record */
  int numg;               /* Number of R records found so far for current S record */
  int numchan;            /* Number of channels found so far for current shot */ 
  int nchan;              /* Number of receivers per group */
  int **chan;             /* Channel number */
  int ndxyear;            /* Header index - year recorded */
  int ndxday;             /* Header index - day of year */
  int ndxhour;            /* Header index  - hour of day */
  int ndxminute;          /* Header index  - minute of hour */
  int ndxsec;             /* Header index  - second of minute */
  int ndxtimbas;          /* Header index  - time basis 1=local,2=GMT,3=other */
  int ndxoffset;          /* Header index  - source-receiver offset */
  int ndxswdep;           /* Header index  - water depth at source */
  int ndxgelev;           /* Header index  - receiver group elevation from sea level */
  int ndxsx;              /* Header index  - source X */
  int ndxsy;              /* Header index  - source Y */
  int ndxgx;              /* Header index  - receiver X */
  int ndxgy;              /* Header index  - receiver Y */
  int ndxep;              /* Header index  - Energy source point number */
  int ndxtracf;           /* Header index  - trace number within FFID */
  int ndxscalel;          /* Header index  - Scale factor for elevations */
  int ndxscalco;          /* Header index  - Scale factor for coordinates */
  int ndxcounit;          /* Header index  - Coordinate units */
  int tday;               /* Day from current trace */
  int thour, tmin, tsec;  /* Time from current trace */
  int ttracf;             /* Channel from current trace */
  int verbose;            /* Debug print */
  int maxerrors=0;        /* Number of mis-matches to allow */
  int errors=0;           /* Error count */
  int merge=0;            /* Merge count */
  int found;              /* Flag for match found */
  /*int ndxfldr;*/            /* Header index  - FFID */
  /*int fldr;*/               /* Trace header - FFID */
  
  /*cwp_String typefldr, typetracf;*/               /* Header types */
  cwp_String typetracf;                         /* Header types */
  cwp_String typesx, typesy, typegx, typegy;    /* Header types */
  cwp_String typeoffset, typegelev;             /* Header types */
  cwp_String typeswdep;                         /* Header types */
  cwp_String typeday, typehour;                 /* Header types */
  cwp_String typemin, typesec;                  /* Header types */
  cwp_String typescalco, typecounit;            /* Header types */
  cwp_String typescalel;                        /* Header types */
  cwp_String typeep;                            /* Header types */

  Value val;              /* Header value */

  /* Hook up getpar to handle the parameters */
  initargs(argc,argv);
  requestdoc(1);

  /* Parameters */

  /* p190 path */
  if(!getparstring("p190",&path))
    err("P190 file name is required\n");
  fprintf(stderr,"P190 file --> %s\n", path);

  /* Debug print */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* Max errors */
  getparint("maxerrors",&maxerrors);
  fprintf(stderr,"%d errors will be allowed\n", maxerrors);

  /* Get first trace */
  if (!gettr(&tr))
    err("Can't get first trace\n");

  /* Open P190 file */
  fp = efopen(path,"r");
  /* Count S records */
  while(fgets(rec,81,fp) != NULL)
    if(!strncmp(rec,"S",1))
      nsrec++;
  rewind(fp);
  fprintf(stderr,"%d Source Records in P190 file\n", nsrec);

  /* Get header indices  and types */
/*
  ndxfldr = getindex("fldr");
  typefldr = hdtype("fldr");
*/
  ndxtracf = getindex("tracf");
  typetracf = hdtype("tracf");
  ndxep = getindex("ep");
  typeep = hdtype("ep");
  ndxoffset = getindex("offset");
  typeoffset = hdtype("offset");
  ndxswdep = getindex("swdep");
  typeswdep = hdtype("swdep");
  ndxgelev = getindex("gelev");
  typegelev = hdtype("gelev");
  ndxsx = getindex("sx");
  typesx = hdtype("sx");
  ndxsy = getindex("sy");
  typesy = hdtype("sy");
  ndxgx = getindex("gx");
  typegx = hdtype("gx");
  ndxgy = getindex("gy");
  typegy = hdtype("gy");
  ndxyear = getindex("year");
  ndxday = getindex("day");
  typeday = hdtype("day");
  ndxhour = getindex("hour");
  typehour = hdtype("hour");
  ndxminute = getindex("minute");
  typemin = hdtype("minute");
  ndxsec = getindex("sec");
  typesec = hdtype("sec");
  ndxtimbas = getindex("timbas");
  typescalel = hdtype("scalel");
  ndxscalel = getindex("scalel");
  typescalco = hdtype("scalco");
  ndxscalco = getindex("scalco");
  typecounit = hdtype("counit");
  ndxcounit = getindex("counit");

  /* Allocate buffers to hold p190 fields being merged into trace headers */
  hour = calloc(nsrec,sizeof(int));
  min = calloc(nsrec,sizeof(int));
  sec = calloc(nsrec,sizeof(int));
  day = calloc(nsrec,sizeof(int));
  shot = calloc(nsrec,sizeof(int));
  sx = calloc(nsrec,sizeof(float));
  sy = calloc(nsrec,sizeof(float));
  swdep = calloc(nsrec,sizeof(float));
  gx = calloc(nsrec,sizeof(float *));
  gy = calloc(nsrec,sizeof(float *));
  gelev = calloc(nsrec,sizeof(float *));
  chan = calloc(nsrec,sizeof(int *));

  /* Scan P190 file to get info needed from header records */
  while(fgets(rec,81,fp) != NULL) {
    if(!strncmp(rec,"H0203 ",6)) {
      sscanf(&rec[32],"%s\n",line);
      /*strcpy(line,&rec[32]);*/
      fprintf(stderr,"Line Prefix: %s\n", line);
      continue;
    }
    else if(!strncmp(rec,"H0800 ",6)) {
      if(verbose)
        fprintf(stderr,"%s\n", rec);
      continue;
    }
    else if(!strncmp(rec,"H1000 ",6)) {
      if(verbose)
        fprintf(stderr,"Time Units: %s\n", rec);
      continue;
    }
    else if(!strncmp(rec,"H1100 ",6)) {
      MOVEI(32,4,nchan);
      fprintf(stderr,"Number of Channels: %d\n", nchan);
      for(i=0; i<nsrec; i++) {
        gx[i] = calloc(nchan,sizeof(float));
        gy[i] = calloc(nchan,sizeof(float));
        gelev[i] = calloc(nchan,sizeof(float));
        chan[i] = calloc(nchan,sizeof(int));
      }
      /* Number of R records required per S */
      ngrec = (nchan + 2) / 3;
      if(verbose)
        fprintf(stderr,"Number of R records per S record: %d\n", ngrec);
      continue;
    }
    else if(!strncmp(rec,"H1700 ",6)) {
      if(verbose)
        fprintf(stderr,"Vertical Datum: %s\n", &rec[32]);
      continue;
    }
    else if(!strncmp(rec,"H2000 ",6)) {
      MOVEF(57,15,convert);
      if(verbose)
        fprintf(stderr,"Metric Conversion Factor for Grid Units: %f\n", convert);
      continue;
    }
    else if(!strncmp(rec,"H2001 ",6)) {
      MOVEF(57,15,height);
      if(verbose)
        fprintf(stderr,"Metric Conversion Factor for Height Units: %f\n", height);
      continue;
    }
    else if(!strncmp(rec,"H2002 ",6)) {
      if(verbose)
        fprintf(stderr,"Angular Units %s\n", &rec[33]);
      continue;
    }
    else if(!strncmp(rec,"H2301 ",6)) {
      if(verbose)
        fprintf(stderr,"Grid Origin\n");
      MOVEI(32,3,latd);
      MOVEI(35,2,latm);
      MOVEF(37,6,lats);
      strncpy(ns,&rec[43],1);
      ns[1] = '\0';
      MOVEI(44,3,lond);
      MOVEI(47,2,lonm);
      MOVEF(49,6,lons);
      strncpy(ew,&rec[55],1);
      ew[1] = '\0';
      if(verbose) {
        fprintf(stderr,"Latitude: Degrees: %d\n", latd);
        fprintf(stderr,"          Minutes: %d\n", latm);
        fprintf(stderr,"          Seconds: %f\n", lats);
        fprintf(stderr,"          Hemisphere: %s\n", ns);
        fprintf(stderr,"Longitude: Degrees: %d\n", lond);
        fprintf(stderr,"           Minutes: %d\n", lonm);
        fprintf(stderr,"           Seconds: %f\n", lons);
        fprintf(stderr,"           Hemisphere: %s\n", ew);
      }
    }
    else if(!strncmp(rec,"H2302 ",6)) {
      MOVEF(32,11,east);
      MOVEF(44,11,north);
      if(verbose) {
        fprintf(stderr,"Grid Coordinates at Origin\n");
        fprintf(stderr,"    Easting: %f\n", east);
        fprintf(stderr,"    Northing: %f\n", north);
      }
    }
    /*else if(!strncmp(rec,"X",1)) {*/
    else if(!strncmp(rec,"S",1)) {
      if(!nums) {
        i = strlen(line);
        strncpy(&line[i],&rec[1],12);
        line[i+12] = '\0';
        fprintf(stderr,"LINE: %s\n", line);
      }
      MOVEI(19,6,shot[nums]);
      MOVEF(46,9,sx[nums]);
      MOVEF(55,9,sy[nums]);
      MOVEF(64,6,swdep[nums]);
      MOVEI(70,3,day[nums]);
      MOVEI(73,2,hour[nums]);
      MOVEI(75,2,min[nums]);
      MOVEI(77,2,sec[nums]);
      /* Get R records for this source */
      numg = 0;
      numchan = 0;
      /* Loop until all R records for this source are found */
      for(;;) {
        if(fgets(rec,81,fp) != NULL) {
          if(!strncmp(rec,"S",1))
            err("S record out of sequence\n");
          if(!strncmp(rec,"R",1)) {
            numg++;
            MOVEI(1,4,chan[nums][numchan]);
            MOVEF(5,9,gx[nums][numchan]);
            MOVEF(14,9,gy[nums][numchan]);
            MOVEF(23,4,gelev[nums][numchan]);
            numchan++;
            /* Last channel for this shot? */
            if(numchan == nchan)
              break;
            MOVEI(27,4,chan[nums][numchan]);
            MOVEF(31,9,gx[nums][numchan]);
            MOVEF(40,9,gy[nums][numchan]);
            MOVEF(49,4,gelev[nums][numchan]);
            numchan++;
            /* Last channel for this shot? */
            if(numchan == nchan)
              break;
            MOVEI(53,4,chan[nums][numchan]);
            MOVEF(57,9,gx[nums][numchan]);
            MOVEF(66,9,gy[nums][numchan]);
            MOVEF(75,4,gelev[nums][numchan]);
            numchan++;
            /* Last channel for this shot? */
            if(numchan == nchan)
              break;
          }
          /* Next record */
          continue;
        }
        /* Premature EOF */
        err("Unexpected EOF in P190 file\n");
      }
      nums++;
    }
  } /* End of while !=NULL */
  fprintf(stderr,"%d shots loaded\n", nums);

  /* Main loop over traces */
  do {
    /* Find p190 source with date and time matching current trace */
    gethval(&tr,ndxday,&val);
    tday = vtoi(typeday,val);
    gethval(&tr,ndxhour,&val);
    thour = vtoi(typehour,val);
    gethval(&tr,ndxminute,&val);
    tmin = vtoi(typemin,val);
    gethval(&tr,ndxsec,&val);
    tsec = vtoi(typesec,val);
    gethval(&tr,ndxtracf,&val);
    ttracf = vtoi(typetracf,val);
    found = 0;
    for(i=0; i<nsrec; i++) {
      if(tday == day[i] && thour == hour[i] &&
        tmin == min[i] && tsec == sec[i]) {
        /* Find matching receiver */
        for(j=0; j<nchan; j++) {
          if(ttracf == chan[i][j]) {
            found = 1;
            merge++;
            temp = -1.;
            setval1(typescalel,&val,temp);
            puthval(&tr,ndxscalel,&val);
            temp = -1.;
            setval1(typescalco,&val,temp);
            puthval(&tr,ndxscalco,&val);
            temp = 1.;
            setval1(typecounit,&val,temp);
            puthval(&tr,ndxcounit,&val);
            temp = sx[i] * 10.;
            setval1(typesx,&val,temp);
            puthval(&tr,ndxsx,&val);
            temp = sy[i] * 10.;
            setval1(typesy,&val,temp);
            puthval(&tr,ndxsy,&val);
            temp = gx[i][j] * 10.;
            setval1(typegx,&val,temp);
            puthval(&tr,ndxgx,&val);
            temp = gy[i][j] * 10.;
            setval1(typegy,&val,temp);
            puthval(&tr,ndxgy,&val);
            temp = swdep[i];
            setval1(typeswdep,&val,temp);
            puthval(&tr,ndxswdep,&val);
            temp = gelev[i][j];
            setval1(typegelev,&val,temp);
            puthval(&tr,ndxgelev,&val);
            temp = shot[i];
            setval1(typeep,&val,temp);
            puthval(&tr,ndxep,&val);
/*
            temp = chan[i][j];
            setval1(typetracf,&val,temp);
            puthval(&tr,ndxtracf,&val);
            temp = fldr[i][j];
            setval1(typefldr,&val,temp);
            puthval(&tr,ndxfldr,&val);
*/
            offset = ((sx[i] - gx[i][j]) * (sx[i] - gx[i][j])) +
                     ((sy[i] - gy[i][j]) * (sy[i] - gy[i][j]));
            offset = sqrt(offset);
            temp = offset;
            setval1(typeoffset,&val,temp);
            puthval(&tr,ndxoffset,&val);
            puttr(&tr);
            break;
          }
        }
      }
    }
    if(!found) {
      errors++;
      if(errors > maxerrors)
        err("Maximum alowable errors exceeded\n");
    }
      
  } while (gettr(&tr));

  fprintf(stderr,"%d records merged\n", merge);
  return EXIT_SUCCESS;

}
