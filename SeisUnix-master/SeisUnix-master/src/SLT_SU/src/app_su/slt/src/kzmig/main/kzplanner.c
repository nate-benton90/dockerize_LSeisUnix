/* KZMIG planing program */
#include "par.h"

char *sdoc = 
"KZPLANNER - KZMIG Planning to output parameters for kzvelo, rayt3d and kzmig\n"
"\n"
"kzplanner [parameters] 		\n" 
"\n"
"Required parameters:					\n"
"offmax=    maximum offset to migrate	\n"
"feather=   maximum feathering distance of input data \n"
"           (maximum crossline distance from source or receive to midpoint \n"
"minli=     minimum line number of input data \n"
"maxli=     maximum line number of input data \n"
"minti=     minimum trace number of input data \n"
"maxti=     maximum trace number of input data \n"
"minlo=     minimum line number of migration output \n"
"maxlo=     maximum line number of migration output \n"
"minto=     minimum trace number of migration output \n"
"maxto=     maximum trace number of migration output \n"
"Optional Parameters: \n"
"ds=12.5    distance of two inline traces (trace number difference of 1) \n"  
"dl=20      distance of two lines (line number difference of 1) \n"
"apers=5000 inline migration aperture (m or ft) \n"
"aperl=5000 crossline migration aperture (m or ft) \n"
"minzo=0    minimum depth of migration output \n"
"maxzo=8000 maximum depth of migration output \n"
"dzo=10     depth interval of migration output \n"
"ntvel=6    trace number increment of velocity grid \n"
"nlvel=4    line number increment of velocity grid \n"
"dzvel=50   depth interval of velocity grid (m or ft) \n"
"dztt=50    depth interval of travel time grid (m or ft) \n"
"nttt=10    trace number increment of travel time grid  \n"
"nltt=6     line number increment of travel time grid  \n"
"nlray=10   line number increment of ray-tracing source grid \n"
"ntray=16   trace number increment of ray-tracing source grid \n"
"\n"
"Example: \n"
" kzplanner offmax=10000 feather=3280.8 \\ \n"
" minli=5001 maxli=7065 minti=2950 maxti=5500 \\ \n"
" minlo=5899 maxlo=6179 minto=4100 maxto=5050 \\ \n"
" ds=41.01 dl=65.616 apers=16404 aperl=16404 \\ \n"
" minzo=5000 maxzo=28000 dzo=30 ntvel=6 nlvel=4 dzvel=200 \\ \n"
" dztt=200 nttt=10 nltt=20 nlray=10 ntray=16 \n"
"\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	4/27/2000   \n"		    
;

main(int argc, char **argv)
{

float ds, dl, apers, aperl, offmax, feather;
int minli, maxli, minti, maxti, minlo, maxlo, minto, maxto;
float minzo,maxzo,dzo;
int ntvel,nlvel; 
float dzvel, dztt;
int nttt, nltt, ntray, nlray;

float tmp;
int dstrace, dsline;
int smins,smaxs,lmins,lmaxs;
int sminv,smaxv,lminv,lmaxv;
int smint,smaxt,lmint,lmaxt;
int nzo,nss,nls,nst,nlt,nsv,nlv,nzv,smin;
int timin,timax,limin,limax,nzt;
float zmint;

/* initialization */
initargs(argc,argv);
askdoc(1);

/* get required parameters */
if (!getparfloat("offmax", &offmax)) err(" offmax missing \n");
if (!getparfloat("feather", &feather)) err(" feather missing \n");
if (!getparint("minli", &minli)) err(" minli missing \n");
if (!getparint("maxli", &maxli)) err(" maxli missing \n");
if (!getparint("minti", &minti)) err(" minti missing \n");
if (!getparint("maxti", &maxti)) err(" maxti missing \n");
if (!getparint("minlo", &minlo)) err(" minlo missing \n");
if (!getparint("maxlo", &maxlo)) err(" maxlo missing \n");
if (!getparint("minto", &minto)) err(" minto missing \n");
if (!getparint("maxto", &maxto)) err(" maxto missing \n");

/* optional parameters */
if (!getparfloat("ds", &ds)) ds = 12.5;
if (!getparfloat("dl", &dl)) dl = 20.;
if (!getparfloat("apers", &apers)) apers = 5000.;
if (!getparfloat("aperl", &aperl)) aperl = 5000.;
if (!getparfloat("minzo", &minzo)) minzo = 0.;
if (!getparfloat("maxzo", &maxzo)) maxzo = 0.;
if (!getparfloat("dzo", &dzo)) dzo = 10.;
if (!getparint("ntvel", &ntvel)) ntvel = 6;
if (!getparint("nlvel", &nlvel)) nlvel = 4;
if (!getparfloat("dzvel", &dzvel)) dzvel = 50.;
if (!getparfloat("dztt", &dztt)) dztt = 50.;
if (!getparint("nttt", &nttt)) nttt = 10;
if (!getparint("nltt", &nltt)) nltt = 6;
if (!getparint("nlray", &nlray)) nlray = 10;
if (!getparint("ntray", &ntray)) ntray = 16;

tmp = (apers+offmax/2.)/ds + 0.5;
dstrace = tmp;
tmp = (aperl+feather)/dl + 0.5;
dsline = tmp;

/* limits of input source and receive locations */
tmp = offmax/2./ds+0.5; 
timin = minti - (int)tmp;
timax = maxti + (int)tmp;
tmp = feather/dl+0.5; 
limin = minli - (int)tmp;
limax = maxli + (int)tmp;


/* output travel time grid limit */
tmp = (float)(maxto-minto)/nttt + 1.9999;
nst = tmp;
smint=minto;
smaxt=minto+(nst-1)*nttt;
tmp = (float)(maxlo-minlo)/nltt + 1.9999;
nlt = tmp;
lmint=minlo;
lmaxt=minlo+(nlt-1)*nltt;
zmint = minzo;
tmp = (maxzo-minzo)/dztt + 1.9999;
nzt = tmp; 


/* raytracing source limit */
smins = smint - dstrace;
if(smins<timin) smins = timin;
smaxs = smaxt + dstrace;
if(smaxs>timax) smaxs = timax;
lmins = lmint - dsline;
if(lmins<limin) lmins = limin;
lmaxs = lmaxt + dsline;
if(lmaxs>limax) lmaxs = limax;

tmp = (float)(smaxs-smins)/ntray + 1.9999;
nss = (int) tmp; 
tmp = (float)(lmaxs-lmins)/nlray + 1.9999;
nls = (int) tmp; 

smaxs = smins + (nss-1)*ntray;
lmaxs = lmins + (nls-1)*nlray;


/* velocity grid limit */
tmp = (float)(smaxs-smins)/ntvel + 1.999;
nsv = (int) tmp;
tmp = (float)(lmaxs-lmins)/nlvel + 1.999;
nlv = (int) tmp;
tmp = maxzo/dzvel + 1.9999;
nzv = (int) tmp;


/* ouput migration limit */
tmp = (maxzo-minzo)/dzo + 1.9999;
nzo = (int) tmp;


fprintf(stderr," ===  KZPLANNER OUTPUT === \n");
fprintf(stderr," =========================  \n");
fprintf(stderr,"  \n");

fprintf(stderr," ===  KZVELO SETUP === \n");
fprintf(stderr," number of depth samples of velocity output=%d \n", nzv);
fprintf(stderr," starting depth of velocity output =0 \n");
fprintf(stderr," depth sampling interval of velocity output =%g \n",dzvel);
fprintf(stderr," number of traces per line of velocity output=%d \n", nsv+2);
fprintf(stderr," starting trace number of velocity output=%d \n", smins-ntvel);
fprintf(stderr," trace number increment of velocity output=%d \n", ntvel);
fprintf(stderr," number of lines of velocity output=%d \n", nlv+2);
fprintf(stderr," starting line number of velocity output=%d \n", lmins-nlvel);
fprintf(stderr," line number increment of velocity output=%d \n", nlvel);
fprintf(stderr," \n");


fprintf(stderr," ===  RAYT3D SETUP === \n");
fprintf(stderr," first trace number to output travel time=%d \n", smint);
fprintf(stderr," trace number increment to output travel time=%d \n", nttt);
fprintf(stderr," number of traces per line to output travel time=%d \n", nst);
fprintf(stderr," first line number to output travel time=%d \n", lmint);
fprintf(stderr," line number increment to output travel time=%d \n", nltt);
fprintf(stderr," number of lines to output travel time=%d \n", nlt);
fprintf(stderr," minimum depth to output travel time=%g \n", minzo);
fprintf(stderr," depth interval to output travel time=%g \n", dztt);
fprintf(stderr," number of depth samples to output travel time=%d \n", nzt);
fprintf(stderr," trace number to first raytracing surface source=%d \n", smins);
fprintf(stderr," trace number increment of raytracing surface source=%d \n", ntray);
fprintf(stderr," number of traces per line of raytracing surface source=%d \n", nss);
fprintf(stderr," line number to first raytracing surface source=%d \n", lmins);
fprintf(stderr," line number increment of raytracing surface source=%d \n", nlray);
fprintf(stderr," number of lines of raytracing surface source=%d \n", nls);
fprintf(stderr," inline raytracing distance from source=%g \n", dstrace*ds);
fprintf(stderr," crossline raytracing distance from source=%g \n", dsline*dl);
fprintf(stderr," \n");

fprintf(stderr," ===  KZMIG SETUP === \n");
fprintf(stderr," depth of first output sample=%g \n", minzo);
fprintf(stderr," depth interval to output =%g \n", dzo);
fprintf(stderr," number of depth samples to output=%d \n", nzo);
fprintf(stderr," inline aperture of migration=%g \n", apers);
fprintf(stderr," crossline aperture of migration=%g \n", aperl);


return 0;

}
