#include <stdio.h>
#include <math.h>

/* from /app/cwp-33 */

#include "su.h"
#include "segy.h"

/* from /app/SU */

#include "gridhd.h"

#include "vmerge.h"

char  *sdoc =
    "VMERGE - combine SU seismic file and vgrid velocity file (128 color version)\n"
    "for display by cmovie\n" 
    "\n"
    "vmerge seismic= velocity= [ options ] > mergefile\n"
    "\n"
    "seismic=        seismic file, default SU\n"
    "fold=1          fold of seismic data (default is stacked data)\n" 
    "stype=segy      format of seismic data: segy (SU), vgrid, float, byte\n"
    "velocity=       velocity grid, same size as seismic file, default vgrid format\n"
    "vtype=vgrid     format of velocity data: segy (SU), vgrid, float, byte\n"
    "sclip=          one or two values for clipping seismic data\n"
    "vclip=             specific values for clipping velocity data\n"

    "sbin=4          number of seismic amplitude bins\n"
    "nv= dv= v0=        bins for clip velocity data\n"
    "vrange=            another method of binning, suggest =3\n"
    "nan=-1e20  not-a-number in overlay file and output\n"
    "\nNotes: The input volumes must have the same dimensions and the product\n"
    "(nv+1)*sbins < 126\n"
;

#define N SU_NFLTS
#define SBIN    4
#define VBIN    31
#define OFFSET  1
#define MAXLEVEL 128
#define NAN     -1e20

int main(int argc, char **argv)
{
    String   stype = "segy";
    String   vtype = "vgrid";
    String   seismic = "";
    String   velocity = "";

    FILE *sfp, *vfp, *outfp;
    int      nsclip;
    int      nvclip;
    int      ns;
    int      n1;
    int      n2;
    int      nv;
    int      i;
    int      sbin = SBIN;
    int      vbin = VBIN;
    int      voffset;
    int      soffset = OFFSET;
    int      n3 = 1;
    int      fold = 1;

    long     ssize;
    long     vsize;

    float    sclip[128];
    float    vclip[128];
    float    v[N];
    float    vtmp[N];
    float    s[N];
    float    low;
    float    high;
    float    dv = 0;
    float    ddv = 1;
    float    d1 = 1;
    float    d2 = 1;
    float    d3 = 1;
    float    o1 = 0;
    float    o2 = 0;
    float    o3 = 0;
    float    vrange = 0;
    float    nan = NAN;
    float    *sp;
    float    *vp;
    float    *se;
    float    slow;
    float    shigh;
    float    sscale;

    int traceCount = 0;

    unsigned char o[N];
    unsigned char *op;

    ghed     gh;

    initargs(argc, argv);
    askdoc(1);

    /*-------------------------------*/
    /* get required input parameters */
    /*-------------------------------*/

    if( !getparstring("seismic", &seismic) ) {
	err("seismic= missing");
    }else {
    	sfp = efopen(seismic, "r");
       if( (getparstring("stype", &stype) != 0 ) && ( !strcmp(stype, "SU") || !strcmp(stype, "su") ) ){
           stype = "segy";
       }
    }

    if( !getparstring("velocity", &velocity)){
        err("velocity= missing");
    }else {
    	vfp = efopen(velocity, "r");
        if( (getparstring("vtype", &vtype) != 0) && ( !strcmp(vtype, "SU") || !strcmp(vtype, "su") ) ){
        vtype = "segy";
        }
    }

    /*-------------------------------*/
    /* get optional input parameters */
    /*-------------------------------*/

    getparfloat("d1", &d1);
    getparfloat("d2", &d2);
    getparfloat("d3", &d3);

    getparfloat("o1", &o1);
    getparfloat("o2", &o2);
    getparfloat("o3", &o3);

    getparfloat("nan", &nan);

    nsclip = getparfloat("sclip", sclip);
    nvclip = getparfloat("vclip", vclip);
    if( nvclip > (vbin - 1)){
        err("#vclip <= %d", vbin - 1);
    }

    vbin = nvclip;

    getparint("sbin", &sbin);
    getparint("vbin", &vbin);

    getparfloat("vrange", &vrange);
    if( getparfloat("v0", vclip) 
     && getparfloat("dv", &dv)
     && getparint("nv", &vbin) ){
        for( i = 1; i < vbin; i++){
            vclip[i] = vclip[i - 1] + dv;
        }

        nvclip = vbin;
    }

    getparint( "fold" ,&fold );

    if( sbin * (vbin + 1) > MAXLEVEL){
        err("sbin=%d * vbin=%d > MAXLEVEL=%d", sbin, vbin, MAXLEVEL);
    }

    voffset = sbin * soffset;

    /*--------------------*/
    /* compare file sizes */
    /*--------------------*/


    findsize(sfp, stype, &ssize, &ns);
    findsize(vfp, vtype, &vsize, &nv);
    nv=ns;

    ns = ns < nv ? ns : nv;
    nv = ns < nv ? ns : nv;

    if( ssize != vsize*fold ){
        err("sample sizes %s=%d does not equal %s=%d fold=%d"
            ,seismic ,ssize ,velocity ,vsize ,fold);
    }


    fprintf( stderr ,"seismic=%s stype=%s velocity=%s vtype=%s\n" 
            ,seismic ,stype ,velocity ,vtype);

    fprintf( stderr ,"ns=%d n1=%d n2=%d o1=%g o2=%g d1=%g d2=%g o3=%g\n" 
            ,ssize ,n1 = ns ,n2 = ssize / ns ,o1 ,o2 ,d1 ,d2 ,o3);


    /*------------------*/
    /* find data ranges */
    /*------------------*/

    if( nsclip == 1 ){
        sclip[1] = sclip[0];
        sclip[0] = -sclip[0];

    }else if( !nsclip){
        findclip(sfp, stype, ssize, sclip, sclip + 1, 95.0, nan);

    }

    if( !nvclip ){
        findclip(vfp, vtype, vsize, &low, &high, 100., nan);
        nvclip = findbin(vclip, vbin, low, high);
        dv     = vclip[1] - vclip[0];
    }

    if( vrange > 0 ){

        if( nvclip == 2 ){
            low  = vclip[0];
            high = vclip[1];
        }

        nvclip = findbin3(vclip, vbin, low, high, vrange);
        dv     = vclip[1] - vclip[0];
        ddv    = (vclip[2] - vclip[1]) / dv;
    }

    vbin   = nvclip + 1;
    slow   = sclip[0];
    shigh  = sclip[1];
    sscale =  (sbin-1) / (shigh - slow);

    /*-----------------------------------*/
    /* report the data binning to stderr */
    /*-----------------------------------*/

    fprintf( stderr ,"sbin=%d sclip=%g,%g vbin=%d dv=%g ddv=%g vclip=" 
            ,sbin ,slow ,shigh ,vbin ,dv ,ddv);

    for( i = 0; i < nvclip - 1; i++ ){
        fprintf(stderr ,"%d," ,(int) vclip[i]);
    }

    fprintf(stderr ,"%d\n" ,(int) vclip[nvclip - 1]);

    /*------------------------*/
    /* set the file positions */
    /*------------------------*/

    if( !strcmp(stype, "segy") ){
        fseeko(sfp, (long long)3600, SEEK_SET);

    }else{
        fseeko(sfp, (long long)0, SEEK_SET);

    }

    if( !strcmp(vtype, "segy") || !strcmp(vtype, "SU") ){
        fseeko(vfp, (long long)3600, SEEK_SET);

    }else{
        fseeko(vfp, (long long)0, SEEK_SET);

    }

    /*-----------------------*/
    /* read & merge the data */
    /*-----------------------*/

    fprintf(stderr," before mergeing \n");

    traceCount=0;
    while( (ns = readvec(sfp, stype, s, ns)) > 0 ){

        if( !(traceCount % fold ) ){
           nv = readvec(vfp, vtype, v, nv);
        }
        memcpy( vtmp ,v ,nv*sizeof(float) );

        traceCount++;

        for( sp = s, se = s + ns, vp = vtmp, op = o; sp < se; sp++, vp++, op++ ){
            *sp = *sp > slow ? *sp : slow;
            *sp = *sp < shigh ? *sp : shigh;
            *sp -= slow;
            *sp *= sscale;
            if( *vp == nan)
                *vp = nvclip;
            else{ 

                for( i = 0; (i < nvclip) && (*vp > vclip[i]+1.0);)
                    i++;
                *vp = i;
            }

            *op = (int)( rint(*sp ) + (*vp) * voffset);

            /*-----------------------------------*/
            /* scale for compatibility w/ cmovie */
            /*-----------------------------------*/

            *op *= 2;
            
        }

        write(1, o, ns);
    }

    /*---------------------------------*/
    /* write the output grid structure */
    /*---------------------------------*/

    if( !strcmp(vtype, "vgrid") ){
        fseeko(vfp, (long long)-100, SEEK_END);
        fread(&gh,1,sizeof(gh),vfp);
        gh.dtype = gh.scale;

    }else if( !strcmp(stype, "vgrid") ){
        fseeko(sfp, (long long)-100, SEEK_END);
        fread(&gh,1, sizeof(gh),sfp);
        gh.dtype = gh.scale;

    }else{ 

        gh.scale = .000001;
        gh.dtype = gh.scale;
        gh.n1 = n1 * gh.scale;
        gh.n2 = n2 * gh.scale;
        gh.n3 = n3 * gh.scale;
        gh.d1 = d1 * gh.scale;
        gh.d2 = d2 * gh.scale;
        gh.d3 = d3 * gh.scale;
        gh.o1 = o1 * gh.scale;
        gh.o2 = o2 * gh.scale;
        gh.o3 = o3 * gh.scale;
    }


    if( fold > 1 ){
       gh.n2 = fold * gh.n2;

    }

    gh.n4 = gh.scale;
    gh.n5 = gh.scale;
    gh.d4 = sbin * gh.scale;
    gh.d5 = vbin * gh.scale;
    gh.o4 = shigh * gh.scale;;
    gh.o5 = vclip[0] * gh.scale;
    gh.dcdp2 = dv * gh.scale;
    gh.dline3 = ddv * gh.scale;
    gh.oline3 = 0.;
    gh.ocdp2 = 0.;
    gh.gtype=5 * gh.scale;

    write(1, &gh, sizeof(gh));

    return( 0 );
}

/*--------------------------------------------------------------------*\
  Read a pair of data vectors from the two input files.
\*--------------------------------------------------------------------*/

static int readvec(FILE *fp, char *type, float *x, int n)
{
    int      nread;

    if( !strcmp(type, "segy") ){
        fseeko(fp, (long long)240, SEEK_CUR);
        if( fread(x, 4, n,fp) != n ){
            return (0);

        }else{
            return (n);

        }

    }else if( !strcmp(type, "vgrid") ){

        if( (nread = fread(x,4,n,fp)) == n ){
            return (n);

        }else{
            return (nread - 25);

        }

    }else if( !strcmp(type, "float") || !strcmp(type, "floats") ){
        return (fread(x,4,n,fp));

    }else if( !strcmp(type, "byte") || !strcmp(type, "bytes") ){
        return (fread(x,1,n,fp));

    }

    return (0);
}

/*--------------------------------------------------------------------*\
   Determine the total size of the input dataset.
\*--------------------------------------------------------------------*/

static void findsize(FILE *fp, char *type, long *size, int *ns)
{
    segy     t1;

    *ns = N;

    if( !strcmp(type, "segy") ){

        fseeko(fp, (long long)3600, SEEK_SET);
        fread(&t1,1,240,fp);

        fseeko(fp, (long long)0, SEEK_END);

        *ns = ftello(fp) / (t1.ns * 4 + 240);
        *size = t1.ns * *ns;
        *ns = t1.ns;

    }else if( !strcmp(type, "vgrid") ){
        fseeko(fp, (long long)0, SEEK_END);
        *size = (ftello(fp) - 100) / 4;

    }else if( !strcmp(type, "float") || !strcmp(type, "floats") ){
        fseeko(fp, (long long)0, SEEK_END);
        *size = ftello(fp) / 4;

    }else if( !strcmp(type, "byte") || !strcmp(type, "bytes") ){
        fseeko(fp, (long long)0, SEEK_END);
        *size = ftello(fp);

    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

static void findclip(FILE *fp, char *type, long size, float *low, float *high,
                  float perc, float nan)
{
#define NSORT 10000000

    int      nsort;
    int      dsort;
    int      i;
    int      n;
    float   *buf;
    unsigned char cbuf[N];
    segy     tr;

    if( !strcmp(type, "byte") || !strcmp(type, "bytes") ){
        return;

    }

    nsort = size < NSORT ? size : NSORT;
    nsort = nsort > (int) (.05 * size) ? nsort : (int) (.05 * size);
    dsort = ceil((float) size / (float) nsort);
    buf   = (float *) malloc((nsort + N) * 4);

    if( !strcmp(type, "segy") ){
        fseeko(fp, (long long)3600, SEEK_SET);
        fread(&tr,1,240,fp);
        nsort = 0;

        while( fread(tr.data,1,tr.ns * 4 + 240,fp) > 0 ){

            for( i = 0; i < tr.ns; i += dsort ){
                if( tr.data[i] != nan ){
                    buf[nsort++] = tr.data[i];
                }
            }
        }

    }else if( !strcmp(type, "vgrid") ){
        fseeko(fp, (long long)0, SEEK_SET);
        nsort = 0;

        while( (n = fread(tr.data,1,N * 4,fp) / 4) > 0 ){

            for( i = 0; i < n; i += dsort, nsort++ ){
                buf[nsort] = tr.data[i];
            }
        }
        nsort -= 25;

    }else if( !strcmp(type, "float") || !strcmp(type, "floats") ){
        fseeko(fp, (long long)0, SEEK_SET);
        nsort = 0;
        while( (n = fread(tr.data,1, N * 4,fp) / 4) > 0 ){

            for( i = 0; i < n; i += dsort, nsort++ ){
                buf[nsort] = tr.data[i];
            }
        }

    }else if( !strcmp(type, "float") || !strcmp(type, "floats") ){
        fseeko(fp, (long long)0, SEEK_SET);
        nsort = 0;
        while( (n = fread(cbuf,1,N,fp)) > 0 ){

            for( i = 0; i < n; i += dsort, nsort++ ){
                buf[nsort] = cbuf[i];
            }
        }
    }

    perc = perc < 100. ? perc : 100.;
    *low = cent(buf, nsort, 100. - perc);
    *high = cent(buf, nsort, perc);
    free(buf);
}

/*--------------------------------------------------------------------*\
  percentile subroutine based on Canales, SEP-10
  this routine changes data order, so sort a copy

  p - percentile <0.,99.999999999999>
  x - data
  n - vector length
 \*--------------------------------------------------------------------*/

static float cent(float *x, int n, float p ){

    int      q;
    register float *i, *j, ak;
    float   *low, *hi, buf, *k;

    p = p < 99.999 ? p : 99.999;
    p = p > 0.0 ? p : 0.0;
    q = (p * n) / 100.;

    if( q == n)
        q--;

    for( low = x, hi = x + n - 1, k = x + q; low < hi; ){
        ak = *k;
        i = low;
        j = hi;
        do{
            while( *i < ak)
                i++;
            while( *j > ak)
                j--;
            if( i <= j ){
                buf = *i;
                *i++ = *j;
                *j-- = buf;
            }
        }while( i <= j);
        if( j < k)
            low = i;
        if( k < i)
            hi = j;
    }
    return (*k);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

static int findbin(float *bin, int nbin, float low, float high)
{
    float    interval;
    int      nbin1;

    interval = (high - low) / nbin;

    interval = pow(10., rint(log10(interval)));

    /* fprintf (stderr,"interval=%g\n",interval); */

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.2 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.25 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.3 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.4 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.5 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.6 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 0.75 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 1.0 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 2.0 * interval)) > 0 ){
        return (nbin1);
    }

    if( (nbin1 = findbin2(bin, nbin, low, high, 5.0 * interval)) > 0 ){
        return (nbin1);
    }

    return (0);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

static int findbin2(float *bin, int nbin, float low, float high, float interval)
{
    int      ilow, ihigh, i;

    ilow = ceil(low / interval);
    ihigh = floor(high / interval);

#if 1

    fprintf (stderr ,"interval=%g ilow=%d ihigh=%d nbin=%d\n"
                ,interval,ilow,ihigh,ihigh-ilow+2); 

#endif

    if( ihigh - ilow + 2 > nbin)
        return (0);

    for( i = 0; ilow <= ihigh; i++, ilow++ ){
        bin[i] = ilow * interval;
    }
    return (i);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

static int findbin3(float *bin, int nbin, float low, float high, float range)
{
    int      i;
    float    ratio, length, interval;

    ratio    = pow(range, 1. / (nbin - 3));
    interval = 1.0;
    length   = 0.0;

    for( i = 0; i < nbin - 2; i++ ){
        interval *= ratio;
        length   += interval;
    }

    interval = (high - low) / length;

    fprintf( stderr 
            ,"range=%g ratio=%g length=%g low=%g high=%g interval=%g-%g " 
            ,range ,ratio ,length ,low ,high ,interval ,interval * range);

    bin[0] = low;

    for (i = 1; i < nbin - 1; i++ ){
        interval *= ratio;
        bin[i] = bin[i - 1] + interval;

    }

    fprintf(stderr, "bin1=%g bin$=%g\n", bin[0], bin[nbin - 2]);

    return (nbin - 1);
}
