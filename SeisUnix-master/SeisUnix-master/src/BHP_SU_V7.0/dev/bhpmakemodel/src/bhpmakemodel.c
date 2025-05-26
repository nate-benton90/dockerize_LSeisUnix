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
*
* HISTORY:
*
******************************************************************/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" BHPMAKEMODEL -  Make BHP-SU Model Traces                              ",
" 									",
" bhpmakemodel combines depths from a BHPIO horizon dataset with",
"              velocity and other information in an ASCII file to build ",
"              a BHPIO horizon-based model dataset.                     ",
" bhpmakemodel < stdin avgvel=                             ",
" 									",
" Required Parameters: ",
"   avgvel=            velocity to use for depth-time conversion of top layer",
" Optional Parameters: ",
"   null=-999.25       null value. The horizon data may contain nulls   ",
"                      where no picks were made",
"   verbose=0          debug print",
"   shift1=0           amount to shift time model after depth-time conversion",
"   file=model.dat     ASCII file containing model velocity etc.",
"   tmodel=time_model.su      Output time model ",
"   dmodel=depth_model.su     Output depth model ",
"        ",
"   EXAMPLE MODEL FILE - NB: Layout is IMPORTANT",
" ",
"shift	enumber	lnumber	name		facies	N/G	vp	vs	rho	rho*vp",
"2000	3	1	top_shale	1	0.00	2836	1241	2.32	21576",
"2000	4	2	lam2		2	0.50	2707	1168	2.23	19752",
"2000	2	3	sand1		3	0.80	2644	1133	2.17	18799",
"2000	1	4	lam1		2	0.40	2730	1181	2.24	20091",
"2000	5	5	bot_shale	1	0.00	2836	1241	2.32	21576",
"2000	6	bottom							",
"					avg_vel =	9300			",
"                                                                       ",
"			vp	vs	rho				",
"		sand	8550	3650	2.13				",
"		shale	9300	4070	2.32				",
"                                                                       ",
"  The heading line is required, and the order of columns is required. The first ",
"  field(shift) is not used but must be input as a place-holder",
"                                                              ",
NULL}; 

int main(int argc, char **argv)
{

  char *file;	        /* ascii model properties */
  char string[16];
  char record[512];     /* record from model file */
  char *tmodel;         /* time model output */
  char *dmodel;         /* time model output */

  int verbose;          /* Debug */
  int nlayers;          /* layer count from model info file */
  int nevents;          /* event count from model info file */
  int i, j, i1, i2;
  int i3, i4;
  int iz;               /* zero-based sample index of start of depths in model traces */
  int nprop=6;          /* number of properties in model (hard-code for now) */
  int first=1;          /* first trace flag */

  float f1, f2, f3, f4, f5;
  float null;           /* null event value */
  float mind, maxd;     /* min, max depths in model */
  float mint, maxt;     /* min, max times in model */
  float *savetr;        /* save last no-null input trace */
  float scale;          /* depth-time scale factor */
  float shift1;         /* shift applied after shift from layer table */
  float avgvel;         /* conversion velocity */
  float dthick, tthick; /* depth, time thicknesses */

  segy tr;              /* horizon input */
  segy time_trace;      /* time model output */
  segy depth_trace;     /* depth model output */

  FILE *infp;           /* model file */
  FILE *tfp;            /* time model file */
  FILE *dfp;            /* depth model file */

  typedef struct {      /* layer info structure */
    int shift;          /* time shift to apply */
    int evnum;          /* event number */
    int laynum;         /* layer number */
    char layname[16];   /* layer name */
    int facies;         /* facies index */
    float ng;           /* net-to-gross */
    float vp;           /* velocity */
    float vs;           /* shear velocity */
    float rho;          /* density */
    float ai;           /* impedence */
  } layer_info;

  layer_info *layer_table;

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* debug option */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* null */
  if(!getparfloat("null",&null))
    null = -999.25;

  /* scale */
  if(!getparfloat("scale",&scale))
    scale = 1.;

  /* shift1 */
  if(!getparfloat("shift1",&shift1))
    shift1 = 0;

  /* avgvel */
  if(!getparfloat("avgvel",&avgvel))
    err("Average velocity must be input\n");

  /* ascii layer info file */
  if(!getparstring("file",&file))
    file = "model.dat";

  /* time model output */
  if(!getparstring("tmodel",&tmodel))
    tmodel = "time_model.su";

  /* depth model output */
  if(!getparstring("dmodel",&dmodel))
    dmodel = "depth_model.su";

  /* open files */
  if((infp = fopen(file,"r")) == NULL)
    err("Cannot open %s\n",file);
  if((tfp = fopen(tmodel,"w")) == NULL)
    err("Cannot open %s\n",tmodel);
  if((dfp = fopen(dmodel,"w")) == NULL)
    err("Cannot open %s\n",dmodel);

  if(verbose) {
    fprintf(stderr,"ASCII Layer Info File: %s\n",file);
    fprintf(stderr,"Time Model File: %s\n",tmodel);
    fprintf(stderr,"Depth Model File: %s\n",dmodel);
  }

  mind = mint = FLT_MAX;
  maxd = maxt = FLT_MIN;
  nlayers = 0;
  /* first record is heading */
  if(fgets(record,512,infp) == NULL)
    err("No data in %s\n",file);
  /* model file columns are: time-shift,event-number,layer-number,layer-name,facies,N/G,vp,vs,rho,AI */
  while(fgets(record,512,infp) != NULL) {
    if(strstr(record,"bottom") != NULL)
      break;
    /* skip blank records */
    if(!strcmp(record,""))
      continue;
    nlayers++;
  }
  nevents = nlayers + 1;
  savetr = calloc(nevents,sizeof(float));
  rewind(infp);
  if(verbose)
    fprintf(stderr,"%d layers in %s\n", nlayers,file);

  layer_table = calloc(nevents,sizeof(layer_info));
  /* Load layer info */
  fgets(record,512,infp);
  for(i=0; i<nevents; i++) {
    fgets(record,512,infp);
    if(strstr(record,"bottom") != NULL) {
      sscanf(record,"%d %d %s",&i1,&i2,string);
      layer_table[i].evnum = i2;
      layer_table[i].laynum = 0;
      strcpy(layer_table[i].layname,"bottom");
      continue;
    }
    else {
      sscanf(record,"%d %d %d %s %d %f %f %f %f %f",&i1,&i2,&i3,string,&i4,&f1,&f2,&f3,&f4,&f5);
      if(i3 < 1 || i3 > nlayers)
        err("Layer number %d is out of range\n",i3);
      layer_table[i].shift = i1;
      layer_table[i].evnum = i2;
      layer_table[i].laynum = i3;
      strcpy(layer_table[i].layname,string);
      layer_table[i].facies = i4;
      layer_table[i].ng = f1;
      layer_table[i].vp = f2;
      layer_table[i].vs = f3;
      layer_table[i].rho = f4;
      layer_table[i].ai = f5;
      /*if(verbose) {
        fprintf(stderr,"time shift=%d, event number=%d, layer number=%d, layer name=%s\n",i1,i2,i3,string);
        fprintf(stderr,"facies=%d, NG=%f, vp=%f, vs=%f, rho=%f, AI=%f\n",i4,f1,f2,f3,f4,f5);
      }*/
    }
  }
  fclose(infp);

  if(verbose) {
    fprintf(stderr,"Layer Info Table\n");
    for(i=0; i<nlayers+1; i++)
      fprintf(stderr,"Shift: %d, Enum: %d, Lnum: %d, Lname: %s, Facies: %d, NG: %f, VP: %f, VS: %f, RHO: %f, AI: %f\n",
              layer_table[i].shift,layer_table[i].evnum,layer_table[i].laynum,
              layer_table[i].layname,layer_table[i].facies,layer_table[i].ng,layer_table[i].vp,
              layer_table[i].vs,layer_table[i].rho,layer_table[i].ai);
    fprintf(stderr,"POST-SHIFT = %f MS\n",shift1);
  }

  /* check first trace */
  if(!gettr(&tr))
    err("BHPMAKEMODEL: Can't get first trace\n");

  /* set output indices, etc */
  iz = nprop * nlayers;
  if(verbose)
    fprintf(stderr,"IZ=%d, NS=%d\n",iz,nprop*nlayers+nlayers+1);

  /* loop over traces */
  do {
    /* put events in layer-number order in output */
    for(i=0; i<tr.ns; i++)
      time_trace.data[iz+i] = null;
    for(i=0; i<tr.ns; i++) {
      /* see if event is in table, if so get laynum, zero is bottom model event */
      i1 = null;
      for(j=0; j<nevents; j++) {
        if(layer_table[j].evnum == i + 1) {
          i1 = layer_table[j].laynum;
          /* bottom */
          if(i1 == 0)
            i1 = nevents;
          break;
        }
      }
      time_trace.data[iz+i1-1] = depth_trace.data[iz+i1-1] = tr.data[i];
    }

    /* first trace cannot be null */
    if(first) {
      if(time_trace.data[iz] == null)
        err("First trace cannot have null top event\n");
      else
        first = 0;
    }

    /* if trace has non-null top event, save it to replace null trace */
    if(time_trace.data[iz] != null)
      for(j=0; j<nevents; j++)
        savetr[j] = time_trace.data[iz+j];

    /* if top event null use save trace */
    if(time_trace.data[iz] == null)
      for(i=0; i<nevents; i++)
        time_trace.data[i+iz] = depth_trace.data[i+iz] = savetr[i];

    /* make sure first layer top is not null */
    if(depth_trace.data[iz] == null) {
      fprintf(stderr,"null in first sample for cdp=%d\n",tr.cdp);
      err("First event cannot be NULL\n");
    }

    /* replace null with previous layer depth, and constrain depth to >= previous depth */
    for(i=iz+1; i<iz+nevents; i++)
      if(time_trace.data[i] == null || time_trace.data[i] < time_trace.data[i-1])
        time_trace.data[i] = depth_trace.data[i] = time_trace.data[i-1];

    /* update min, max depths */
    if(depth_trace.data[iz] < mind)
      mind = depth_trace.data[iz];
    if(depth_trace.data[iz+nlayers] > maxd)
      maxd = depth_trace.data[iz+nlayers];

    /* fill in property values - order is facies,ng,vp,vs,rho,ai */
    for(i1=0; i1<nevents; i1++) {
      i2 = layer_table[i1].laynum;
      /* skip zero laynum */
      if(i2 == 0)
        continue;
      i2--;
      time_trace.data[i2] = depth_trace.data[i2] = layer_table[i1].facies;
      time_trace.data[nlayers+i2] = depth_trace.data[nlayers+i2] = layer_table[i1].ng;
      time_trace.data[2*nlayers+i2] = depth_trace.data[2*nlayers+i2] = layer_table[i1].vp;
      time_trace.data[3*nlayers+i2] = depth_trace.data[3*nlayers+i2] = layer_table[i1].vs;
      time_trace.data[4*nlayers+i2] = depth_trace.data[4*nlayers+i2] = layer_table[i1].rho;
      time_trace.data[5*nlayers+i2] = depth_trace.data[5*nlayers+i2] = layer_table[i1].ai;
    }

    /* depth-time conversion using layer thickness to time thickness */
    /* use avgvel for first time */
    time_trace.data[iz] = 2 * time_trace.data[iz] / avgvel;
    /*for(i=iz; i<iz+nlayers+1; i++)
      fprintf(stderr,"depth=%f\n",depth_trace.data[i]);*/
    for(i=iz+1,i2=0; i<iz+nlayers+1; i++,i2++) {
      dthick = depth_trace.data[i] - depth_trace.data[i-1];
      tthick = 2. * dthick / depth_trace.data[2*nlayers+i2];
      time_trace.data[i] = time_trace.data[i-1] + tthick;
      /*fprintf(stderr,"dthick=%f, tthick=%f, vel=%f, time=%f\n",
              dthick,tthick,depth_trace.data[2*nlayers+i2],time_trace.data[i]+.25);*/
    }

    /* shifts in seconds */
    for(i=iz,j=0; i<iz+nlayers+1; i++,j++)
      time_trace.data[i] += 0.001 * shift1;

    /* update mint, maxt */
    if(time_trace.data[iz] < mint)
      mint = time_trace.data[iz];
    if(time_trace.data[iz+nlayers] > maxt)
      maxt = time_trace.data[iz+nlayers];

    /* convert model times to mills */
    for(i=iz,j=0; i<iz+nlayers+1; i++,j++)
      time_trace.data[i] *= 1000.0;

    memcpy((void *)&time_trace,(const void *)&tr,HDRBYTES);
    memcpy((void *)&depth_trace,(const void *)&tr,HDRBYTES);
    time_trace.ns = depth_trace.ns = nprop * nlayers + nlayers + 1;  
    time_trace.dt = depth_trace.dt = 1000;
    fputtr(tfp,&time_trace);
    fputtr(dfp,&depth_trace);
  } while (gettr(&tr));

  if(verbose) {
    fprintf(stderr,"Min Depth in Model(meters) = %f, Max Depth in Model(meters) = %f\n",mind,maxd);
    fprintf(stderr,"Min Time in Model(seconds) = %f, Max Time in Model(seconds) = %f\n",mint,maxt);
  }

  fclose(tfp);
  fclose(dfp);
  return EXIT_SUCCESS;

}
