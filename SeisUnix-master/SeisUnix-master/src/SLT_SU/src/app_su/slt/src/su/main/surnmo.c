
#include "su.h"
#include "cwp.h"
#include "segy.h"
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"

/*********************** self documentation ******************************/
string sdoc =
"                                                                       \n"
" NAME                                                                  \n"
"       RNmo -- Residual NMO transform                                  \n"
"               Input is an NMO corrected gather,                       \n"
"               Output is a Residual NMO corrected gather.              \n"
"                                                                       \n"
" SYNOPSIS                                                              \n"
"       RNmo < stdin > stdout [input parameters]                        \n"
"                                                                       \n"
"                                                                       \n"
" PARAMETERS:                                                           \n"
"                                                                       \n"
"  rmo_option=1    options for running RNmo                             \n"
"                  1 - scan and output rmo corrections in HANDVEL format \n"
"                  2 - scan and output rmo corrections in HANDVEL format \n"
"                       and rmo corrected gathers (for testing purpose)  \n" 
"                  3 - apply rmo corrections (input grid in vgrid format)\n"
"                       and output rmo corrected gathers                 \n"
"                                                                       \n"
"  amin=-150       negative max moveout time/depth (ms or meters)       \n"
"  amax=150        positive max moveout time/depth (ms or meters)       \n"
"  na=30           number of moveouts to try from a=amin to a=amax      \n"
"  ntri=25         halfwidth of triangle smoothing filter (in points)   \n"
"  sscale=1        if sscale=0, don't apply NMO stretch scaling         \n"
"                                                                       \n"
"  tracekey=tracr  segy key word defining trace number inline (vgrid)   \n"
"  linekey=fldr    segy key word defining line number (vgrid)           \n"
"  gatherkey=cdp   segy key word defining gather assemble               \n"
"                                                                       \n"
"  f_rmo=rmo.hvel  file for residual moveout corrections at maximum     \n"
"                   offset in HANDVEL format                            \n" 
"  f_semb=semb.su  file for optimal semblance traces                    \n"
"  maxoffset=      max. offset (in meters or feet) for computing        \n"
"                   residual moveout correction                         \n" 
"  nrmo=40         max. number of residual moveout corrections of each  \n"
"                   gather for output                                   \n" 
"                                                                       \n"
"                                                                       \n"
"  rmogrid=      file name of residual moveout correction grid          \n"
 "                (stored as (t,x,y) order with grid header             \n"
"                                                                       \n"
"  ocdp2=from_rmogrid   inline trace number of first rmo grid trace     \n"
"  dcdp2=from_rmogrid   inline trace number increment of rmo grid       \n"
"  oline3=from_rmogrid  line number of first rmo grid trace             \n"
"  dline3=from_rmogrid  line number increment of rmo grid               \n"
"                                                                       \n"
"  ntrgrid=from_rmogrid no. of time or depth samples per trace in rmogrid\n"
"  dtrgrid=from_rmogrid time or depth interval (in ms or m) of rmogrid  \n"
"  ftrgrid=from_rmogrid minimum time or depth (in ms or m) of rmogrid   \n"
"                                                                       \n"
"                                                                       \n"
" Keyword Residual NMO                                                  \n"
"                                                                       \n"
"                                                                       \n"
" AUTHOR:                                                               \n"
"       Wenlong Xu                                                      \n"
"                                                                       \n"
" DATE: Oct.9, 2000                                                     \n"
"                                                                       \n"
" REFERENCE:                                                            \n"
"                                                                       \n"
"\n";
/**************** end self doc *******************************************/


segychdr ch ;
segybhdr bh ;
segytrace tr, tr_out ;


void rnmo(float **cmp, int nt, int nx, float t0, float dt, float amin,
          float amax, int na, int ntri, int sscale, float *xx, float **flat,
          float *aaopt, float *sembmax, int rmo_option, int nrmo, int *nrmo1, 
          float *tnout, float *rnout) ;

void rnmo_apply(float **cmp, int nt, int nx, float t0, float dt, int sscale, 
          float *xx, float **flat, float *aaopt ) ;

/* Note: copied from supstack */
void readv(FILE *vgfp, int ntvgrid, int nx, int ny, int ix, int iy,
    float dtvgrid, float ftvgrid, int nt, float dt, float ft, float *vel);

main(int argc, char **argv)
{
    FILE   *rmofp ;
    FILE   *rmogrdfp ;
    FILE   *sembfp ;
    string f_rmo ;
    string rmogrid ;
    string f_semb ;

    int    rmo_option ;

    /* variable for handling the trace header parameters */
    String tracekey, linekey, gatherkey ;
    String trktype,  lnktype, gatherktype ;
    Value  trkval,   lnkval,  gatherkval ;
    int    indxtrk,  indxlnk, indxgatherk ;
    float  fx, dx, fy, dy, x, y ;
    int    i2, i3 ;

    int    traceno, lineno, cdplbl ;
    float  *tnout, *rnout ;
    int    ip, nrmo, nrmo1 ;

    /* variables for rnmo grid files */
    int    ntrgrid ;
    float  dtrgrid ;
    float  ftrgrid ;
    int    n1,n2,n3,n4,n5 ;
    float  o1,o2,o3,o4,o5,d1,d2,d3,d4,d5 ;
    float  scale, ocdp2, oline3, dcdp2, dline3 ;
    float  rmin, rmax ;
    int    dtype,ierr,orient,gtype ;
    ghed   gh ;

    int   nt, nx, maxnx ;
    int   it, ix ;
    float t0, dt ;

    float **cmp ;
    float **flat ;
    float *xx ;
    float *aaopt ;
    float *sembmax ;

    float maxoffset ;
    int   maxoffset_status ;  /* 0 = read from header of 1st gather, */ 
                              /* 1 = provided by user */
    int   icdp ;

    float amin, amax ;
    int   na ;
    int   ntri ;
    int   sscale;   /* if non-zero, apply NMO stretch scaling */

    char  *headers ;  /* trace header info. */
    int   cdp;        /* cdp from current input trace header */
    int   cdpprev;    /* cdp from previous input trace header */
    int   gottrace;   /* =1 if an input trace was read */
    int   done;       /* =1 if done with everything */
    
    
    /* hook up getpar */
    initargs(argc, argv) ;
    askdoc(0) ;
   
    /* read headers */
    gethdr(&ch, &bh) ;

    /* optional parameters */

    if( !getparint  ("rmo_option", &rmo_option) ) rmo_option = 1 ;

    if( !getparstring("gatherkey",&gatherkey) ) gatherkey = "cdp" ;
    if( !getparstring("tracekey",&tracekey) ) tracekey ="tracf" ;
    if( !getparstring("linekey",&linekey) ) linekey = "fldr" ;

    if( !getparint  ("maxnx", &maxnx) ) maxnx = 120 ;

    if( !getparfloat("maxoffset", &maxoffset) ) {
      maxoffset_status = 0 ;
    } else {
      maxoffset_status = 1 ;
      maxoffset *= 0.001 ;
    }
   
    if( !getparfloat("amin", &amin) ) amin =-150. ;
    if( !getparfloat("amax", &amax) ) amax = 150. ;

    amin = amin*0.001 ;
    amax = amax*0.001 ;

    if( !getparint  ("na", &na) ) na = 30 ;
    if( !getparint  ("ntri", &ntri) ) ntri = 25 ;
    if( !getparint("sscale", &sscale) ) sscale = 1 ;

    if( !getparint("nrmo", &nrmo) ) nrmo = 40 ;
    if( !getparstring("f_rmo",&f_rmo) ) f_rmo = "rmo.hvel" ;
    if( !getparstring("f_semb",&f_semb) ) f_semb = "semb.su" ;

    if( rmo_option == 1 || rmo_option == 2 ) {
      if( (rmofp=fopen(f_rmo, "w")) == NULL ) {
        fprintf (stderr, "%s: can not open %s \n", argv[0], f_rmo);
        exit(-1) ;
      }
/* uncomment for handle file size larger than 2GB */
      file2g(rmofp) ;

      if( (sembfp=fopen(f_semb, "w")) == NULL ) {
        fprintf (stderr, "%s: can not open %s \n", argv[0], f_semb);
        exit(-1) ;
      }
/* uncomment for handle file size larger than 2GB */
      file2g(sembfp) ;
    }

    if( rmo_option == 3 ) {
      if (!getparstring("rmogrid",&rmogrid) )
        err(" File rmogrid must be specified ") ;
      if( (rmogrdfp = fopen(rmogrid,"r"))==NULL )
        err("Input rmogrid file %s not found \n",rmogrid) ;
 
      /* obtain grid header info */
/* uncomment for handle file size larger than 2GB */
      file2g(rmogrdfp);
      ierr = fgetghdr(rmogrdfp, &gh);
      if(ierr==0) fromghdr(&gh,&scale,&dtype,&n1,&n2,&n3,&n4,&n5,
                          &d1,&d2,&d3,&d4,&d5,&o1,&o2,&o3,&o4,&o5,
                          &dcdp2,&dline3,&ocdp2,&oline3,
                          &rmin,&rmax,&orient,&gtype) ;
    }
    
    /* get parameters from the first trace */
    if( !gettr(&tr) ) err( "can't get first trace" );
    nt = tr.ns ;
    dt = (float)tr.dt / 1000000.0 ;
    t0 = tr.delrt / 1000.0 ;


    /* handle the trace keys */
    gatherktype = hdtype(gatherkey);
    trktype = hdtype(tracekey);
    lnktype = hdtype(linekey);

    indxgatherk = getindex(gatherkey);
    indxtrk = getindex(tracekey);
    indxlnk = getindex(linekey);

    if( rmo_option == 3 ) {
      if(!getparfloat("ocdp2",&fx) && ierr==0) fx = ocdp2;
      if(!getparfloat("dcdp2",&dx) && ierr==0) dx = dcdp2;
      if(!getparfloat("oline3",&fy) && ierr==0) fy = oline3;
      if(!getparfloat("dline3",&dy) && ierr==0) dy = dline3;

      if( !getparint("ntrgrid",&ntrgrid) ) {
        if(ierr==0) {
          ntrgrid = n1;
        } else {
          ntrgrid = nt;
        }
      }
      if( !getparfloat("dtrgrid",&dtrgrid) ) {
        if(ierr==0) {
          dtrgrid = d1 * 0.001;
        } else {
          dtrgrid = dt;
        }
      } else {
        dtrgrid = dtrgrid*0.001;
      }
      if( !getparfloat("ftrgrid",&ftrgrid) ) {
        if(ierr==0) {
          ftrgrid = o1 * 0.001;
        } else {
          ftrgrid = t0;
        }
      } else {
        ftrgrid = ftrgrid*0.001;
      }
    }


    /* trace number and line number */ 
    gethval(&tr,indxtrk,&trkval);
    traceno = vtoi(trktype,trkval);
    gethval(&tr,indxlnk,&lnkval);
    lineno = vtoi(lnktype,lnkval);

    if( rmo_option == 3 ) {
      x = (traceno - fx)/dx + 0.5;
      i2 = x;
      y = (lineno - fy)/dy + 0.5;
      i3 = y;
  
      if(i2<0) i2=0;
      if(i2>n2-1) i2=n2-1;
      if(i3<0) i3=0;
      if(i3>n3-1) i3=n3-1;
    }


    /* gather assemble identifier */
    gethval(&tr,indxgatherk,&gatherkval);
    cdp = vtoi(gatherktype,gatherkval);

    /* allocate memory */
    headers = (char*) emalloc(maxnx*HDRBYTES);
    cmp  = ealloc2float(nt, maxnx) ;
    flat = ealloc2float(nt, maxnx) ;
    aaopt = ealloc1float(nt) ;
    sembmax = ealloc1float(nt) ;
    xx   = ealloc1float(maxnx) ;

    tnout = ealloc1float(nrmo) ;
    rnout = ealloc1float(nrmo) ;


    /* output binary header */
    if( rmo_option == 1 || rmo_option == 2 ) {
      // fputhdr(sembfp, &ch, &bh) ;
    }
    if( rmo_option == 2 || rmo_option == 3 ) {
      // puthdr(&ch, &bh) ;
    }
  

    /* initialize flags */
    gottrace = 1 ;
    done = 0 ;

    /* remember previous cdp */
    cdpprev = cdp ;
    nx = 0 ;

    icdp = 0 ;
    /* loop all cdps */
    do {
       /* if got a trace */
       if( gottrace ) {

         /* determine offset and cdp */
         gethval(&tr,indxgatherk,&gatherkval);
         cdp = vtoi(gatherktype,gatherkval);

         if( cdp == cdpprev ) {

           /* get trace samples */

           /* save the trace header info. */
           bcopy( (char*)&tr, headers+nx*HDRBYTES, HDRBYTES ) ;
           /* for( it=0; it<nt; it++ ) cmp[nx][it] = tr.data[it] ; */
           bcopy( tr.data, cmp[nx], nt*sizeof(float) ) ;
           xx[nx] = tr.offset/1000.0 ;
           nx++ ;
         }
       }

       /* if cdp has changed or no more input traces */
       if( cdp != cdpprev || !gottrace ) {

         if( maxoffset_status == 0 ) {
           maxoffset = xx[nx-1] ;
           maxoffset_status = 1 ;  /* reset the status */
           fprintf(stderr,"Warning:\n") ;
           fprintf(stderr,"Max.offset of the 1st gather is used.  %.1f\n", 
                   maxoffset*1000.0) ;
         }

         /* perform Residual NMO correction */

         if( rmo_option == 1 || rmo_option == 2 ) {

           nrmo1 = nrmo ;
           for( ip=0; ip<nrmo; ip++ ) {
              tnout[ip] = -9.999 ; 
              rnout[ip] = 0.0 ;
           }

           rnmo(cmp, nt, nx, t0, dt, amin, amax, na, ntri, sscale, xx, flat,
                aaopt, sembmax, rmo_option, nrmo, &nrmo1, tnout, rnout) ;

           /* output the optimal semblance trace */
           bcopy( headers, (char*)&tr_out, HDRBYTES ) ;
           bcopy( sembmax, tr_out.data, nt*sizeof(float) ) ;
           tr_out.tracl = icdp + 1 ;
           fputtr(sembfp, &tr_out) ;


         } else if( rmo_option == 3 ) {

           /* prepare rmo correction function at the current cdp location */
           readv(rmogrdfp,ntrgrid,n2,n3,i2,i3,dtrgrid,ftrgrid,nt,dt,t0,aaopt);


           for( it=0; it<nt; it++ ) {
              aaopt[it] /= (maxoffset*maxoffset*1000.0) ; 
           }

           rnmo_apply(cmp, nt, nx, t0, dt, sscale, xx, flat, aaopt) ;
         }

         icdp++ ;

         /* output the NMO corrected gather */
         if( rmo_option == 2 || rmo_option == 3 ) {
           for( ix=0; ix<nx; ix++ ) {
              /* put trace header back */
              bcopy( headers+ix*HDRBYTES, (char*)&tr_out, HDRBYTES ) ;
              /* for( it=0; it<nt; it++ ) tr_out.data[it] = flat[ix][it] ; */
              bcopy( flat[ix], tr_out.data, nt*sizeof(float) ) ;
              puttr(&tr_out) ;
           }
         }
         
         /* output the residual correction at maximum offset in HANDVEL */

         if( rmo_option == 1 || rmo_option == 2 ) {
           cdplbl = lineno*10000 + traceno ;

           for( ip=0; ip<nrmo1; ip++ ) {
              tnout[ip] *= 1000.0 ;
              rnout[ip] *= maxoffset*maxoffset*1000.0 ;
           }

           printhvel(cdplbl, nrmo1, tnout, rnout, rmofp) ;
         }

         /* save the first trace of the next cdp */
         if( gottrace ) {
           /* start a new cdp gather, reset the offset counter */
           nx = 0 ;
           /* save the trace header info. */
           bcopy( (char*)&tr, headers+nx*HDRBYTES, HDRBYTES ) ;
           /* for( it=0; it<nt; it++ ) cmp[nx][it] = tr.data[it] ; */
           bcopy( tr.data, cmp[nx], nt*sizeof(float) ) ;
           xx[nx] = tr.offset/1000.0 ;
           nx++ ;

           /* update trace number and line number */
           gethval(&tr,indxtrk,&trkval);
           traceno = vtoi(trktype,trkval);
           gethval(&tr,indxlnk,&lnkval);
           lineno = vtoi(lnktype,lnkval);

           if( rmo_option == 3 ) {
             x = (traceno - fx)/dx + 0.5;
             i2 = x;
             y = (lineno - fy)/dy + 0.5;
             i3 = y;
  
             if(i2<0) i2=0;
             if(i2>n2-1) i2=n2-1;
             if(i3<0) i3=0;
             if(i3>n3-1) i3=n3-1;
           }
 
         }

         /* if no more input traces, break input trace loop */
         if( !gottrace )  break ;

         /* remember previous cdp and line number */
         cdpprev = cdp ;
       }

       /* get next trace (if there is one) */
       if( !gettr(&tr) ) gottrace = 0 ;

    } while( !done ) ;
   
    if( rmo_option == 1 || rmo_option == 2 ) {
      fclose(rmofp) ;
      fclose(sembfp) ;
    }

    /* free memory */
    free2float(cmp) ;
    free2float(flat) ;
    free1float(aaopt) ;
    free1float(sembmax) ;
    free1float(xx) ;
    free1float(tnout) ;
    free1float(rnout) ;
    free(headers) ;


    return EXIT_SUCCESS ;
}

 

/* 
 * Residual NMO scan and correction 
 */

#define  EPS 1.0e-12

void boxconv(int nb, int nx, float *xx, float *yy) ;
void triangle(int nr, int n12, float *uu, float *vv) ;

void vresamp(int np, float *ts, float *vs, int nt, float t0, float dt,
             float *v ) ;
 
void rnmo(float **cmp, int nt, int nx, float t0, float dt, float amin, 
          float amax, int na, int ntri, int sscale, float *xx, float **flat,
          float *aaopt, float *sembmax, int rmo_option, int nrmo, int *nrmo1, 
          float *tnout, float *rnout)
{

    int   it, ix, ia, it1, it2, it3 ;
    float df, trnmo, tnmo ;
    float sum1, sum2, semb ;
    float amp1, amp2 ;
    float aa, a0, da ;
    float a1, a2, a3 ; 
    float t1, slope ;

    float xmax, amin1, amax1 ;
    
    float *xx2, *tau ;
    float *temp1, *temp2 ;
    float **trnmo_buf ;
    float *atnz ;
    
    int   *ind_ex ;
    int   ip ;
    float *extrema ;
    float clip_semb ;

    xmax = MAX(ABS(xx[0]), ABS(xx[nx-1])) ;
    xmax = xmax*xmax ;
    amin1 = amin/xmax ;
    amax1 = amax/xmax ;

    df = 1./dt ;
    da = (amax1-amin1)/(float)na ;
    a0 = amin1 ;

    /* allocate temporary arrays */
    xx2 = ealloc1float(nx) ;
    tau = ealloc1float(nt) ;
    temp1 = ealloc1float(nt) ;
    temp2 = ealloc1float(nt) ;
    trnmo_buf = ealloc2float(nt, nx) ;
    atnz = ealloc1float(nt) ;

    ind_ex = ealloc1int(nt) ;
    extrema = ealloc1float(nt) ;
    
    for( ix=0; ix<nx; ix++ ) {
       xx[ix] = ABS(xx[ix]) ;
       xx2[ix] = xx[ix]*xx[ix] ;
    }

    for( it=0; it<nt; it++ ) {
       tau[it] = t0 + it*dt ;
    }
    
    /* initialize arrays for sorting semblance */
    for( it=0; it<nt; it++ ) {
       ind_ex[it] = it ;
       extrema[it] = -1.0 + EPS*franuni() ; 
    }

    /* 
     * Scan and search for optimum parameter of residual moveout equation
     * a near offset approximation
     *    Trnmo = Tnmo + aa*X^2
     */

    /* Loop over NMO time */
    for( it=0; it<nt; it++ ) {
       tnmo = tau[it] ;
       sum1 = 0 ;  
       sum2 = 0. ;
       for( ix=0; ix<nx; ix++ ) {
          a1 = cmp[ix][it] ;
          sum1 += a1 ;
          sum2 += a1*a1 ;
       }
       sembmax[it] = (sum1*sum1)/(sum2+EPS) ;
       aaopt[it] = 0. ;

       /* Loop over residual moveouts */
       for( ia=0; ia<=na; ia++ ) {

          aa = ia*da + a0 ;

          sum1 = 0. ;
          sum2 = 0. ;

          /* Loop over offset */
          for( ix=0; ix<nx; ix++ ) {
             trnmo = tnmo + aa*xx2[ix] ;
             it2 = (trnmo-t0)/dt + 0.5 ;
             if( it2 >= 1 && it2 < nt-1 ) {
               it1 = it2-1 ;
               it3 = it2+1 ;
               a1 = cmp[ix][it1] ; 
               a2 = 2.*cmp[ix][it2] ; 
               a3 = cmp[ix][it3] ;

               amp1 = 0.25*(a1 + a2 + a3) ;
               amp2 = amp1*amp1 ;
               sum1 += amp1 ;
               sum2 += amp2 ;
             }
          } /* end of ix loop */
          
          semb = (sum1*sum1)/(sum2+EPS) ;

          if( semb > sembmax[it] && ABS(sembmax[it]) > 1000.0*EPS ) {
            sembmax[it] = semb ;
            aaopt[it] = aa ;
          }
       } /* end of ia loop */
    } /* end of it loop */


    /* 
     * Smoothing by triangle integration with semblance weight
     */ 

    for( it=0; it<nt; it++ ) {
       temp1[it] = sembmax[it]*aaopt[it] ;
    }

    for( it=0; it<nt; it++ ) {
       temp2[it] = sembmax[it] ;
    }

    /* Smooth aaopt by triangle smoother */ 

    if( ntri > 1 ) {
      triangle( ntri, nt, temp1, temp1 ) ;
      triangle( ntri, nt, temp2, temp2 ) ;
    }

    for( it=0; it<nt; it++ ) {
       aaopt[it] = temp1[it]/(temp2[it]+2.0*EPS) ;
    }

    /* 
     * compute the semblance using the optimal moveout 
     */

    /* Loop over NMO time */
    for( it=0; it<nt; it++ ) {

       tnmo = tau[it] ;
       aa = aaopt[it] ;
       sembmax[it] = 0. ;

       sum1 = 0. ;
       sum2 = 0. ;

       /* Loop over offset */
       for( ix=0; ix<nx; ix++ ) {
          trnmo = tnmo + aa*xx2[ix] ;
          it2 = (trnmo-t0)/dt + 0.5 ;
          if( it2 >= 1 && it2 < nt-1 ) {
            it1 = it2-1 ;
            it3 = it2+1 ;
            a1 = cmp[ix][it1] ; 
            a2 = 2.*cmp[ix][it2] ;
            a3 = cmp[ix][it3] ;

            amp1 = 0.25*(a1 + a2 + a3) ;
            amp2 = amp1*amp1 ;
            sum1 += amp1 ;
            sum2 += amp2 ;
          }
       } /* end of ix loop */
       sembmax[it] = (sum1*sum1)/(sum2+EPS) ;

    } /* end of it loop */

    /* smooth sembmax using 9-point triangle smoother */
    triangle( 9, nt, sembmax, sembmax ) ;
    triangle( 3, nt, sembmax, sembmax ) ;

    /* 
     * sort out the first nrmo largest semblance values 
     */

    /* search for extrema along the semblance curve */
    for( it=3; it<nt-3; it++ ) {
       if( sembmax[it] > sembmax[it-1] &&
           sembmax[it] > sembmax[it-2] &&
           sembmax[it] > sembmax[it-3] &&
           sembmax[it] > sembmax[it+1] &&
           sembmax[it] > sembmax[it+2] &&
           sembmax[it] > sembmax[it+3] ) {
         extrema[it] = sembmax[it] ;
       }
    }
    
    /* sort an index array based on semblance */
    qkisort( nt, extrema, ind_ex ) ;
    

    /* save the nrmo largest extrema, set the rest to -1.0 */
    for( it=0; it<nt-nrmo; it++ ) {
       extrema[ind_ex[it]] = -1.0 ;
    }

    /* find a clip value for sembalance */
    clip_semb = extrema[ind_ex[NINT(0.98*nt-1.0)]] ;

    /* save the selected optimal residual movemouts */ 
    ip = 0 ;
    for( it=0; it<nt-1; it++ ) {
       if( extrema[it] > 0.10*clip_semb ) {
/*
         sembmax[it] = -1.0 ;
*/
         tnout[ip] = tau[it] ;
         rnout[ip] = aaopt[it] ;
         ip++ ;
       }
    }
    *nrmo1 = ip ;

    /* 
     * Apply optimal estimates of RNMO
     */

    if( rmo_option == 2 ) {

      vresamp(ip, tnout, rnout, nt, t0, dt, aaopt) ;
/*
    for( it=0; it<nt; it++ ) {
       sembmax[it] = aaopt[it] ;
    }
*/
      /* Zero output storage */
      for( ix=0; ix<nx; ix++ )
         for( it=0; it<nt; it++)
            flat[ix][it] = 0. ;

      /* Compute the residual moveouts */
      for( it=0; it<nt; it++ ) {
     
         tnmo = tau[it] ;
         aa = aaopt[it] ;

         for( ix=0; ix<nx; ix++ ) {
            if( ABS(cmp[ix][it]) > EPS ) {
              trnmo_buf[ix][it] = (tnmo + aa*xx2[ix])*df ;
            } else {
              trnmo_buf[ix][it] = tnmo*df ;
            }
         }
      }
   
      /* Perform residual moveout correction */
      for( ix=0; ix<nx; ix++ ) {

         /* Do rnmo via 8-point sinc interploation */
         ints8r(nt, 1.0, t0*df, cmp[ix], 0.0, 0.0, nt, trnmo_buf[ix], flat[ix]);
         if( sscale ) {
           /* Compute inverse of stretch facter */
           atnz[0] = trnmo_buf[ix][1] - trnmo_buf[ix][0] ;
           for( it=1; it<nt; it++ ) {
              atnz[it] = trnmo_buf[ix][it] - trnmo_buf[ix][it-1] ;
           }

           /* Scale by the NMO stretch factor */
           for( it=0; it<nt; it++ ) {
              flat[ix][it] *= atnz[it] ;
           }
         }
      }
    }

    /* free temp arrays */
    free1float(xx2) ;
    free1float(tau) ;
    free1float(temp1) ;
    free1float(temp2) ;
    free2float(trnmo_buf) ;
    free1float(atnz) ;
    free1int(ind_ex) ;
    free1float(extrema) ;
}

/*
 * Apply residual NMO correction 
 */

void rnmo_apply(float **cmp, int nt, int nx, float t0, float dt, int sscale,
          float *xx, float **flat, float *aaopt )
{

    int   it, ix ;
    float df, trnmo, tnmo ;
    float aa ;
    
    float *xx2, *tau ;
    float **trnmo_buf ;
    float *atnz ;

    df = 1./dt ;

    /* allocate temporary arrays */
    xx2 = ealloc1float(nx) ;
    tau = ealloc1float(nt) ;
    trnmo_buf = ealloc2float(nt, nx) ;
    atnz = ealloc1float(nt) ;
    
    for( ix=0; ix<nx; ix++ ) {
       xx[ix] = ABS(xx[ix]) ;
       xx2[ix] = xx[ix]*xx[ix] ;
    }

    for( it=0; it<nt; it++ ) {
       tau[it] = t0 + it*dt ;
    }

    /* 
     * Apply optimal estimates of RNMO
     */

    /* Zero output storage */
    for( ix=0; ix<nx; ix++ )
       for( it=0; it<nt; it++)
          flat[ix][it] = 0. ;

    /* Compute the residual moveouts */
    for( it=0; it<nt; it++ ) {
     
       tnmo = tau[it] ;
       aa = aaopt[it] ;

       for( ix=0; ix<nx; ix++ ) {
          if( ABS(cmp[ix][it]) > EPS ) { 
            trnmo_buf[ix][it] = (tnmo + aa*xx2[ix])*df ;
          } else {
            trnmo_buf[ix][it] = tnmo*df ;
          }
       }
    }
   
    /* Perform residual moveout correction */
    for( ix=0; ix<nx; ix++ ) {

       /* Do rnmo via 8-point sinc interploation */
       ints8r(nt, 1.0, t0*df, cmp[ix], 0.0, 0.0, nt, trnmo_buf[ix], flat[ix]) ;

       if( sscale ) {
         /* Compute inverse of stretch facter */
         atnz[0] = trnmo_buf[ix][1] - trnmo_buf[ix][0] ;
         for( it=1; it<nt; it++ ) {
            atnz[it] = trnmo_buf[ix][it] - trnmo_buf[ix][it-1] ;
         }

         /* Scale by the NMO stretch factor */
         for( it=0; it<nt; it++ ) {
            flat[ix][it] *= atnz[it] ;
         }
       }
    }

    /* free temp arrays */
    free1float(xx2) ;
    free1float(tau) ;
    free2float(trnmo_buf) ;
    free1float(atnz) ;
} 




/* 
 * Convole with triangle
 */

void triangle(int nr, int n12, float *uu, float *vv)  
{
/* 
 * input:  nr -- rectangle width (points) (triangle base twice as wide)
 *         uu[i2], i2=0, n12-1 -- a vector of data
 * output: vv[i2], i2=0, n12-1 -- may be on top of uu 
 */
    int   i, np, nq ;
    float *pp, *qq, *tt ;
    
    /* allocate memory */
    pp = ealloc1float(n12+nr-1) ; 
    qq = ealloc1float(n12+nr+nr-2) ; 
    tt = ealloc1float(n12) ; 

    for( i=0; i<n12; i++ ) qq[i] = uu[i] ;
    if( n12 == 1 ) {
      for( i=0; i<n12; i++ ) tt[i] = qq[i] ;
    } else {
      boxconv( nr, n12, qq, pp) ;
      np = nr + n12 - 1 ;
      boxconv( nr, np, pp, qq ) ;
      nq = nr + np - 1 ;
      for( i=0; i<n12; i++ ) tt[i] = qq[i+nr-1] ;

      /* fold back near end */
      for( i=0; i<nr-1; i++ ) tt[i] = tt[i] + qq[nr-i-2] ;

      /* fold back far end */
      for( i=0; i<nr-1; i++ ) tt[n12-i-1] = tt[n12-i-1] + qq[n12+(nr-1)+i] ;
    }

    for( i=0; i<n12; i++ ) vv[i] = tt[i] ;

    /* free memory */
    free1float(pp) ;
    free1float(qq) ;
    free1float(tt) ;
}

void boxconv(int nb, int nx, float *xx, float *yy) 
{
/* 
 * input:  nx, xx[i], i=0, nx-1    the data
 *         nb -- the box length
 * output: yy[i], i=0, nx+nb-2     smoothed data
 */
    int   ny, i ;
    float *bb ;
    
    /* allocate memory */
    bb = ealloc1float(nx+nb) ; 

    if( nb < 1 || nb > nx ) err("error in boxconv's input") ;
    ny = nx + nb - 1 ;
    for( i=0; i<ny; i++ ) bb[i] = 0. ;

    /* make B(Z) = X(Z)/(1-Z) */ 
    bb[0] = xx[0] ;
    for( i=1; i<nx; i++ ) bb[i] = bb[i-1] + xx[i] ; 
    for( i=nx; i<ny; i++ ) bb[i] = bb[i-1] ;
    for( i=0; i<nb; i++ ) yy[i] = bb[i] ;  
    /* make Y(Z) = B(Z)*(1-Z**nb) */ 
    for( i=nb; i<ny; i++ ) yy[i] = bb[i] - bb[i-nb] ;
    for( i=0; i<ny; i++ ) yy[i] = yy[i] / nb ;
    
    /* free memory */
    free1float(bb) ;
}

/* 
 * read and interpolate velocity function from an Vnmo grid file 
 */

void readv(FILE *vgfp, int ntvgrid, int nx, int ny, int ix, int iy,
    float dtvgrid, float ftvgrid, int nt, float dt, float ft, float *vel) {

    float *vread, ratio, t, ftr;
    int icdp, it, itread;
    float tmp;
    long lpos;

    vread = (float *) malloc(ntvgrid*sizeof(float));

    icdp = ix + iy*nx;

    lpos = icdp;
    lpos = lpos*ntvgrid*sizeof(float);

    fseek(vgfp,lpos,0);
    fread(vread,sizeof(float),ntvgrid,vgfp);

    ratio = dt/dtvgrid;
    ftr = (ft - ftvgrid)/dtvgrid;

    for(it=0;it<nt;it++) {
        t = it*ratio + ftr;
        itread = t;
        if(itread < 0) {
            tmp = vread[0];
        } else if(itread >= ntvgrid-1) {
            tmp = vread[ntvgrid-1];
        } else {
            tmp = vread[itread] + (t-itread)*
                (vread[itread+1]-vread[itread]);
        }
        vel[it] = tmp;
    }

    free(vread);
}

/* 
 * Re-interpolate a (t,v) function to every sample point
 */ 

void vresamp(int np, float *ts, float *vs, int nt, float t0, float dt,
             float *v )
{
    int it, j, j1 ;
    float tmax, tout, fraction ;
    
    tmax = (nt-1)*dt + t0 ;

    j1 = 0 ;
    it = 0 ;
    tout = t0 ; 

    do {
       for( j=j1; j<np; j++ ) {
          if( ts[j] >= tout) {
            if( ts[j] == tout) {
              v[it] = vs[j] ;
            } else {
              if( tout < ts[0] ) {
                v[it] = vs[0] ;
              } else {
                fraction = (ts[j] - tout) / (ts[j] - ts[j-1]) ;
                v[it] = fraction*vs[j-1] + (1.-fraction)*vs[j] ;
              }
            }
            j1 = j ;
            break ;
          } else {
            fraction = (tmax - tout) / (tmax - ts[np-1]) ;
            v[it] = fraction*vs[np-1] ;
          }
       }
       it++ ;
       tout = tout + dt ;
    } while( tout <= tmax ) ; 
} 
