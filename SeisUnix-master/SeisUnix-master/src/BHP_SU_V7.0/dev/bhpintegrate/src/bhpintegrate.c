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
* KEYWORDS:  $RCSfile: bhpintegrate.c,v $
*            $Revision: 1.5 $
*            $Date: 2004/03/26 19:35:47 $
*            $Author: ahmilb $
*
*------------------------------------------------------------------
*
* HISTORY:
*
* $Log: bhpintegrate.c,v $
* Revision 1.5  2004/03/26 19:35:47  ahmilb
* Change default mode to edge.
* Add type=abs, to allow for polarity changes.
*
* Revision 1.4  2002/01/30 16:01:01  ahmilb
* Change search parameter to 4-entry list to allow asymmetric searches.
*
* Revision 1.3  2001/09/13 18:21:07  ahmilb
* Move scalhdr function to libcube/src/interp_lib.c
*
* Revision 1.2  2001/02/06 03:04:27  ahglim
* corrected comment problem
*
*
*
******************************************************************/

#include "su.h"
#include "segy.h"
#include "header.h"
#include "bhp_hdr.h"
#include "bhp_interp.h"

/*********************** self documentation ******************************/
char *sdoc[] = {
" 									",
" BHPINTEGRATE - Calculate the area of a peak or trough                 ",
"                at a user-specified time                               ",
" 									",
" bhpintegrate < stdin > stdout [options]                               ",
"                                                                       ",
" Required Parameters:                                                  ",
"  None                                                                 ",
"        								",
" Optional parameters:   					        ",
" mode=edge     edge: Get P1,P2 from header, find Z                     ",
"               point: Get Z from header, find P1,P2                    ",
" Z=sut         Trace header containing peak/trough time(ms)            ",
" A=d2          Trace header in which to store calculated area          ",
" L=f1          Trace header in which to store width(ms) of peak/trough ",
" H=f2          Trace header in which to store amplitude at peak/trough ",
" P1=igc        Trace header in which to store point m1 (ms)            ",
" P2=igi        Trace header in which to store point m2 (ms)            ",
" P3=corr       Trace header in which to store peak/trough point (ms)   ",
" type=peak     peak, trough, or abs. Use abs to search for largest     ",
"               magnitude, irregardless of sign. This is useful if the  ",
"               event of interest has polarity reversals.               ",
"               type=abs is available for mode=edge only                ",
"               When type=abs is used, the top horizon is first searched",
"               for an absolute value peak. Then the bottom horizon is  ",
"               searched for a value of same sign as the top. The interior",
"               is then searched for a value of oppposite polarity.     ",
" search=3,3,3,3 Up to 4 search values, in samples, can be used         ",
"               For mode=edge, up to 4 values can be specified          ",
"               to search above P1, below P1, above P2, and below P2.   ",
"               If less than 4 values are specified, the last-specified ",
"               value is used for the unspecified values.               ",
"               For mode=point, up to 2 values can be specified for     ",
"               searching above Z and below Z. If only 1 value is       ",
"               specified, it is used above and below Z.                ",
"               To disable searching around P1, P2 and Z, specify all zeros.",
"               Search values are in samples.                           ",
" limit=25      Maximum distance to search, in samples, on each side of ",
"               peak/trough for an upturn or downturn. limit applies to ", 
"               mode=point only.                                        ",
" enforce=yes   Ensure that upturns are negative for type=peak, and that",
"               downturns are positive for type=trough, unless the limit",
"               number of samples have been reached on an edge search.  ",
"               enforce applies to mode=point only.                     ",
"                                                                       ",
" nval=-999.25 Z NULL value; set L, A to zero                           ",
"                               					",
"                               					",
NULL}; 

segy tr;

#define BACKWARD 0
#define FORWARD 1

/* Prototypes */
int findminmax(float *w, int val, char *type, int dir, int lim, int ns, char *enforce);
int findpktr(float *w, int val, int lim1, int lim2, int ns, int look, int *found);
int findh1h2(float *tr, int p1, int p2, float value, char *type, int dir, int look);
void trapzd(float *w, int m1, int m2, float dt, int verbose, int width, float *area);

int main(int argc, char **argv)
{

  Value hZ;             /* Header values */
  Value hL;             /* Header values */
  Value hA;             /* Header values */
  Value hH;             /* Header values */
  Value hP1;             /* Header values */
  Value hP2;             /* Header values */
  Value hP3;             /* Header values */

  float Aval;           /* Computed area */
  float Lval;           /* Peak/trough length(ms) */
  float Hval;           /* Peak/trough amplitude */
  float nval;           /* Z NULL value */
  float avg;            /* Average amplitude of end points */
  float temp;           /* temp holder for header values */
  float halfmax;        /* 1/2 max amplitude */

  int i, j;             /* Counter */
  int verbose;          /* Debug */
  int indexA, indexL, indexZ, indexH; /* Header indices */
  int indexP1, indexP2, indexP3;
  int Zval;             /* Peak/trough time(ms) */
  int P1val;            /* Point 1 */
  int P2val;            /* Point 2 */
  int P3val;            /* Point 3 */
  int m1, m2, m3;       /* edge and target samples */
  int dt;               /* Sample interval(ms) */
  int limit;            /* Search limit, in samples */
  int *search;          /* 4 search values */
  int *s;               /* adjusted search values for a given trace */
  int h1, h2;           /* width-half-max indices */
  int found=0;          /* returned from findpktr, 
                           found=0 --> requested value not found
                           found=1 --> found maximum,
                           found=-1, --> found minimum
                           found=100 --> found algebraic maximum,
                           found=-100 --> found algebraic minimum */
  int look;             /* passed to findpktr,
                           look=1 --> look for maximum (ignore sign),
                           look=-1 --> look for minimum (ignore sign),
                           look=0 --> look for absolute maximum,
                           look=100 --> look for maximum positive,
                           look=-100 --> look for maximum negative */

  char *Z, *A, *L, *H;     /* Headers to use */
  char *P1, *P2, *P3;      /* Headers to use */
  char *type;              /* Peak or Trough */
  char *mode;              /* point or edge */
  char *enforce;           /* yes or no */

  cwp_String typeA, typeL, typeZ, typeH; /* header types */
  cwp_String typeP1, typeP2, typeP3;

  /* Initialize */
  initargs(argc, argv);
  requestdoc(1);

  /* debug option */
  if(!getparint("verbose",&verbose))
    verbose = 0;

  /* Get headers to use */
  if(!getparstring("Z",&Z))
    Z = "sut";
  if(!getparstring("H",&H))
    H = "f2";
  if(!getparstring("L",&L))
    L = "f1";
  if(!getparstring("A",&A))
    A = "d2";
  if(!getparstring("P1",&P1))
    P1 = "igc";
  if(!getparstring("P2",&P2))
    P2 = "igi";
  if(!getparstring("P3",&P3))
    P3 = "corr";

  /* Z NULL value */
  if(!getparfloat("nval",&nval))
    nval = -999.25;

  /* point or edge mode */
  if(!getparstring("mode",&mode))
    mode = "edge";

  /* Search for peak or trough */
  if(!getparstring("type",&type))
    type = "peak";

  /* min/max search limits */
  search = calloc(4,sizeof(int));
  s = calloc(4,sizeof(int));
  i = countparval("search");
  if(i > 4)
    err("Maximum of 4 search values allowed\n");
  else if(i == 0) {
    for(i=0; i<4; i++)
      search[i] = 3;
  }
  else {
    getparint("search",search);
    for(j=i; j<4; j++)
      search[j] = search[i-1];
  }

  /* edges search limit */
  if(!getparint("limit",&limit))
    limit = 25;

  /* Enforce pos/neg upturn/downturn? */
  if(!getparstring("enforce",&enforce))
    enforce = "yes";
    
  /* get types and indexes corresponding to the keys */
  typeA = hdtype(A);
  indexA = getindex(A);
  typeL = hdtype(L);
  indexL = getindex(L);
  typeZ = hdtype(Z);
  indexZ = getindex(Z);
  typeH = hdtype(H);
  indexH = getindex(H);
  typeP1 = hdtype(P1);
  indexP1 = getindex(P1);
  typeP2 = hdtype(P2);
  indexP2 = getindex(P2);
  typeP3 = hdtype(P3);
  indexP3 = getindex(P3);

  if(verbose) {
    fprintf(stderr,"A Name: %s, A Type: %s, A Index: %d\n", A,typeA,indexA);
    fprintf(stderr,"L Name: %s, L Type: %s, L Index: %d\n", L,typeL,indexL);
    fprintf(stderr,"Z Name: %s, Z Type: %s, Z Index: %d\n", Z,typeZ,indexZ);
    fprintf(stderr,"H Name: %s, H Type: %s, H Index: %d\n", H,typeH,indexH);
    fprintf(stderr,"P1 Name: %s, P1 Type: %s, P1 Index: %d\n", P1,typeP1,indexP1);
    fprintf(stderr,"P2 Name: %s, P2 Type: %s, P2 Index: %d\n", P2,typeP2,indexP2);
    fprintf(stderr,"P3 Name: %s, P3 Type: %s, P3 Index: %d\n", P3,typeP3,indexP3);
    fprintf(stderr,"Mode: %s\n", mode);
    fprintf(stderr,"Type of search: %s\n", type);
    fprintf(stderr,"Peak/trough search limits: %d %d %d %d\n",search[0],search[1],search[2],search[3]);
    if(!strcmp(mode,"point"))
      fprintf(stderr,"Edge search limit: %d (samples)\n", limit);
    if(!strcmp(enforce,"yes") && !strcmp(type,"peak"))
      fprintf(stderr,"Enforce negative upturns at edges, unless limit is reached\n");
    if(!strcmp(enforce,"yes") && !strcmp(type,"trough"))
      fprintf(stderr,"Enforce positive downturns at edges, unless limit is reached\n");
    if(!strcmp(enforce,"no") && !strcmp(type,"peak"))
      fprintf(stderr,"Don't check sign of upturns\n");
    if(!strcmp(enforce,"no") && !strcmp(type,"trough"))
      fprintf(stderr,"Don't check sign of downturns\n");
  }

  /* mode=point, type=abs not allowed */
  if(!strcmp(mode,"point") && !strcmp(type,"abs"))
    err("mode=point not allowed if type=abs\n");

  /* loop over traces */
  if(!gettr(&tr))
    err("Cant get first trace\n");

  /* Sample interval in ms */
  dt = tr.dt / 1000;

  do {
    /* Initialize to invalid values */
    Aval = nval;
    Hval = nval;
    P1val = nval;
    P2val = nval;
    P3val = nval;
    Lval = nval;
    Zval = -1;
    m1 = -1;
    m2 = tr.ns + 1;
    m3 = -1;
    if(!strcmp(mode,"edge")) {
      /* Get P1, P2 from header, find Z */
      gethval(&tr,indexP1,&hP1);
      m1 = vtoi(typeP1,hP1);
      gethval(&tr,indexP2,&hP2);
      m2 = vtoi(typeP2,hP2);
      /* Make sure m1 and m2 are inside trace, and p2 > p1+1 */
      if(m1 >= tr.delrt &&  m1 <= tr.delrt + tr.ns * dt &&
         m2 >= tr.delrt &&  m2 <= tr.delrt + tr.ns * dt &&
         m1 + 1 < m2) {
        /* convert to samples */
        m1 = (m1 - tr.delrt) / dt;
        m2 = (m2 - tr.delrt) / dt;
        /* Check for overlap in search ranges */
        s[0] = search[0];
        s[1] = search[1];
        s[2] = search[2];
        s[3] = search[3];
        while(m1 + s[1] >= m2 - s[2]) {
          if(s[1] >= s[2])
            s[1]--;
          else
            s[2]--;
        }
        if(verbose) {
          if(search[1] != s[1])
            fprintf(stderr,"Adjusted second search value: %d\n",s[1]);
          if(search[2] != s[2])
            fprintf(stderr,"Adjusted third search value: %d\n",s[2]);
        }
        /* code for peak or trough */
        if(!strcmp(type,"peak") || !strcmp(type,"trough")) {
          /* find minimum on edge if target is peak */
          if(!strcmp(type,"peak"))
            look = -1;
          /* find maximum on edge if target is trough */
          else if(!strcmp(type,"trough"))
            look = 1;
          m1 = findpktr(tr.data,m1,s[0],s[1],tr.ns,look,&found);
          if(verbose > 1)
            fprintf(stderr,"look=%d, found=%d, m1=%d\n",look,found,m1);
          /* findpktr will never fail for look=1 or look=-1 */
          /* calibrate bottom edge if value found is same as look */
          if(found == look) {
            m2 = findpktr(tr.data,m2,s[2],s[3],tr.ns,look,&found);
            if(verbose > 1)
              fprintf(stderr,"look=%d, found=%d, m2=%d\n",look,found,m2);
            if(m1 >= 0 && m2 < tr.ns && m1 + 1 < m2 && found == look) {
              /* now find maximum for peak or minimum for trough */
              m3 = m1;
              if(!strcmp(type,"peak")) {
                for(i=m1+1; i<=m2; i++) {
                  if(tr.data[i] > tr.data[m3])
                    m3 = i;
                }
              }
              else if(!strcmp(type,"trough")) {
                for(i=m1+1; i<=m2; i++) {
                  if(tr.data[i] < tr.data[m3])
                    m3 = i;
                }
              }
            }
            if(verbose > 1)
              fprintf(stderr,"m3=%d\n",m3);
            if(m3 > m1 && m3 < m2)
              Zval = m3;
          }
        }
        /* code for abs */
        else if(!strcmp(type,"abs")) {
          /* look for absolute maximum on edges */
          look = 0;
          m1 = findpktr(tr.data,m1,s[0],s[1],tr.ns,look,&found);
          if(verbose > 1)
            fprintf(stderr,"look=%d, found=%d, m1=%d\n",look,found,m1);
          /* look for bottom edge to match what was found at top */
          /* findpktr returns 100 or -100 if look=0 */
          look = found;
          m2 = findpktr(tr.data,m2,s[2],s[3],tr.ns,look,&found);
          if(verbose > 1)
            fprintf(stderr,"look=%d, found=%d, m2=%d\n",look,found,m2);
          /* if found same at bottom */
          if(found == look) {
            /* find opposite target */
            m3 = m1;
            if(found == -100) {
              for(i=m1+1; i<=m2; i++) {
                if(tr.data[i] > tr.data[m3] && tr.data[i] > 0)
                  m3 = i;
              }
            }
            else if(found == 100) {
              for(i=m1+1; i<=m2; i++) {
                if(tr.data[i] < tr.data[m3] && tr.data[i] < 0)
                  m3 = i;
              }
            }
            if(m3 > m1 && m3 < m2)
              Zval = m3;
          }
        }
      }
    }
    /* code for mode=point */
    else if(!strcmp(mode,"point")) {
      gethval(&tr,indexZ,&hZ);
      /*  Use i as Zval until validated */
      i = vtoi(typeZ,hZ);
      if(i > tr.delrt && i < tr.delrt + tr.ns * dt) {
        i = (i - tr.delrt) / dt;
        s[0] = search[0];
        s[1] = search[1];
        /* set look according to type */
        if(!strcmp(type,"peak"))
          look = 100;
        else if(!strcmp(type,"trough"))
          look = -100;
        /* Make sure we are on an actual peak/trough */
        i = findpktr(tr.data,i,s[0],s[1],tr.ns,look,&found);
        if(verbose > 1)
          fprintf(stderr,"look=%d, found=%d, i=%d\n",look,found,i);
        if(look == found) {
          /* Check limits again */
          if(i > 0 && i < tr.ns - 1) {
            Zval = i;
            /* Find maxima or minima */
            m1 = findminmax(tr.data,Zval,type,BACKWARD,limit,tr.ns,enforce);
            m2 = findminmax(tr.data,Zval,type,FORWARD,limit,tr.ns,enforce);
          }
        }
      }
    }
    /* If Zval, m1, and m2 are valid, calculate area */
    if(m1 >= 0 && m2 <= tr.ns && Zval > 0) {
      /* Calculate amplitude */
      avg = (tr.data[m1] + tr.data[m2]) / 2.;
      Hval = tr.data[Zval] - avg;
      /* Find area */
      Aval = 0.0;
      /* use full width for area calculation */
      Lval = (m2 - m1) * dt;
      trapzd(tr.data,m1,m2,(float)dt,verbose,Lval,&Aval);
      /* Re-calc width as "width-half-max" */
      halfmax = tr.data[Zval] - 0.5 * Hval;
      if(!strcmp(type,"peak"))
        look = 100;
      else if(!strcmp(type,"trough"))
        look = -100;
      else if(!strcmp(type,"abs"))
        look = -look;
      h1 = findh1h2(tr.data,m1,Zval,halfmax,type,FORWARD,look);
      h2 = findh1h2(tr.data,m2,Zval,halfmax,type,BACKWARD,look);
      Lval = (h2 - h1) * dt;
      P3val = Zval * dt + tr.delrt;
      P1val = m1 * dt + tr.delrt;
      P2val = m2 * dt + tr.delrt;
    }

    /* Scale as necessary and store in header */
    temp = Lval;
    scalhdr(&tr,L,&temp,STORE);
    setval1(typeL,&hL,(double)temp);
    puthval(&tr,indexL,&hL);

    scalhdr(&tr,A,&Aval,STORE);
    setval1(typeA,&hA,(double)Aval);
    puthval(&tr,indexA,&hA);

    scalhdr(&tr,H,&Hval,STORE);
    setval1(typeH,&hH,(double)Hval);
    puthval(&tr,indexH,&hH);

    temp = P1val;
    scalhdr(&tr,P1,&temp,STORE);
    setval1(typeP1,&hP1,(double)temp);
    puthval(&tr,indexP1,&hP1);

    temp = P2val;
    scalhdr(&tr,P2,&temp,STORE);
    setval1(typeP2,&hP2,(double)temp);
    puthval(&tr,indexP2,&hP2);

    temp = P3val;
    scalhdr(&tr,P3,&temp,STORE);
    setval1(typeP3,&hP3,(double)temp);
    puthval(&tr,indexP3,&hP3);
 
    puttr(&tr);
  } while(gettr(&tr));

  return EXIT_SUCCESS;
}

int findminmax(float *w, int val, char *type, int dir, int lim, int ns, char *enforce)
{

  int i;
  int index;
  int start, end;

  index = -1;

  /* Set start, end of window and constrain to trace limits */
  start = val - lim;
  if(start < 0)
    start = 0;
  end = val + lim;
  if(end > ns)
    end = ns - 1;
  if(dir) {
    for(i=val; i<=end; i++) { 
      /* Search forward for an upturn, must be negative if enforce=yes */
      if(!strcmp(type,"peak")) {
        if(w[i] < w[i+1] ) {
          if((!strcmp(enforce,"yes") && w[i] < 0) || !strcmp(enforce,"no")) {
            index = i;
            break;
          }
        }
      }
      /* Search forward for a downturn, must be positive if enforce=yes */
      else {
        if(w[i] > w[i+1] ) {
          if((!strcmp(enforce,"yes") && w[i] >= 0) || !strcmp(enforce,"no")) {
            index = i;
            break;
          }
        }
      }
    }
    /* If min/max not found, set to limit */
    if(index < 0)
      index = end;
  }
  else {
    for(i=val; i>=start; i--) {
      /* Search backward for an upturn, must be negative if enforce=yes */
      if(!strcmp(type,"peak")) {
        if(w[i] < w[i-1] ) {
          if((!strcmp(enforce,"yes") && w[i] < 0) || !strcmp(enforce,"no")) {
            index = i;
            break;
          }
        }
      }
      /* Search backward for a downturn, must be positive if enforce=yes */
      else {
        if(w[i] > w[i-1] ) {
          if((!strcmp(enforce,"yes") && w[i] >= 0) || !strcmp(enforce,"no")) {
            index = i;
            break;
          }
        }
      }
    }
    /* If min/max not found, set to limit */
    if(index < 0)
      index = start;
  }
  return index;
}

void trapzd(float *w, int m1, int m2, float dt, int verbose, int width, float *area)

{

  int j;

  float avg;

  *area = 0.0;
  for(j=m1+1; j<m2; j++)
    *area += 2 * w[j];
  *area += w[m1];
  *area += w[m2];
  *area *= 0.5;
  *area *= dt;
  avg = 0.5 * (w[m1] + w[m2]);
  *area -= (avg * width);

}
int findpktr(float *w, int val, int lim1, int lim2, int ns, int look, int *found)
{

  float newval;

  int i;
  int index;
  int start, end;

  /* Set start and end of window and adjust if outside trace */
  start = val - lim1;
  end = val + lim2;
  if(start < 0)
    start = 0;
  if(end > ns)
    end = ns - 1;
  /* initialize index */
  index = start;

  /* look=100, search for max pos */
  if(look == 100) {
    newval = -FLT_MAX;
    for(i=start; i<=end; i++) {
      if(w[i] > newval && w[i] >= 0) {
        newval = w[i];
        index = i;
      }
    }
  }
  /* look=-100, search for max neg */
  else if(look == -100) {
    newval = FLT_MAX;
    for(i=start; i<=end; i++) {
      if(w[i] < newval && w[i] < 0) {
        newval = w[i];
        index = i;
      }
    }
  }
  /* look=1, search for max (ignore sign) */
  else if(look == 1) {
    newval = w[start];
    for(i=start; i<=end; i++) {
      if(w[i] > newval) {
        newval = w[i];
        index = i;
      }
    }
  }
  /* look=-1, search for min (ignore sign) */
  else if(look == -1) {
    newval = w[start];
    for(i=start; i<=end; i++) {
      if(w[i] < newval) {
        newval = w[i];
        index = i;
      }
    }
  }
  /* look=0, search for abs max */
  else if(look == 0) {
    newval = w[start];
    for(i=start; i<=end; i++) {
      if(ABS(w[i]) > ABS(newval)) {
        newval = w[i];
        index = i;
      }
    }
  }

  /* if newval still illegal, request not found */
  if(newval == FLT_MAX || newval == -FLT_MAX)
    *found = 0;
  else if(look == 1)
    *found = 1;
  else if(look == -1)
    *found = -1;
  else if(newval >= 0)
    *found = 100;
  else if(newval < 0)
    *found = -100;

  return index;
}

int findh1h2(float *tr, int p1, int p2, float value, char *type, int dir, int look)
{

  int i, s;

  s = p2;

  /* Searching forward, type=peak or type=abs and look=100 */
  if(dir && (!strcmp(type,"peak") || (!strcmp(type,"abs") && look == 100))) {
    for(i=p1; i<=p2; i++) {
      if(tr[i] >= value) {
        s = i;
        break;
      }
    }
  }
  /* Searching forward, type=trough or type=abs and look=-100 */
  else if(dir && (!strcmp(type,"trough") || (!strcmp(type,"abs") && look == -100))) {
    for(i=p1; i<=p2; i++) {
      if(tr[i] <= value) {
        s = i;
        break;
      }
    }
  }
  /* Searching backward, type=peak or type=abs and look=100 */
  else if(!dir && (!strcmp(type,"peak") || (!strcmp(type,"abs") && look == 100))) {
    for(i=p1; i>=p2; i--) {
      if(tr[i] >= value) {
        s = i;
        break;
      }
    }
  }
  /* Searching backward, type=trough or type=abs and look=-100 */
  else if(!dir && (!strcmp(type,"trough") || (!strcmp(type,"abs") && look == -100))) {
    for(i=p1; i>=p2; i--) {
      if(tr[i] <= value) {
        s = i;
        break;
      }
    }
  }

  return s;

}
