/* velocity grid to HANDVEL card conversion */

#include "velo.h"
#include "ghdr.h"
#include "gridhd.h"
#include "par.h"


char *sdoc = 
"VGRID2HVEL - convert velocity grid to DISCO HANDVEL or Landmark AVF cards          \n"
"\n"
"vgrid2hvel [parameters] < vgrid  >hvel-cards               \n" 
"\n"
"Required parameters:                           \n"
"vgrid=                Name of velocity grid                \n"
"hvel-cards=           Name of dataset to output HANDVEL cards      \n"
"\n"
"Optional parameters:                           \n"
"outfmt=hvel               Output format.  outfmt=avf produces Landmark format\n"
"\n"
"If outfmt=avf, the following parameters are required\n"
"x1=           x coordinate of 1st corner of the 3D master grid \n"
"y1=           y coordinate of 1st corner of the 3D master grid \n"
"trace1=       trace number of 1st corner of the 3D master grid \n"
"line1=        line number of 1st corner of the 3D master grid \n"
"x2=           x coordinate of 2nd corner of the 3D master grid \n"
"y2=           y coordinate of 2nd corner of the 3D master grid \n"
"trace2=       trace number of 2nd corner of the 3D master grid \n"
"line2=        line number of 2nd corner of the 3D master grid \n"
"x3=           x coordinate of 3rd corner of the 3D master grid \n"
"y3=           y coordinate of 3rd corner of the 3D master grid \n"
"trace3=       trace number of 3rd corner of the 3D master grid \n"
"line3=        line number of 3rd corner of the 3D master grid \n"
"ftype=        Landmark velocity function type\n" 
"lunits=       Landmark length units\n"
"datum=0       Optional datum\n"
"\n"
"fcdpvgrid=from_header   First trace number of velocity grid        \n"
"dcdpvgrid=from_header   Trace number increment of velocity grid    \n"
"ncdpvgrid=from_header   Number of trace per line of velocity grid  \n"
"flinevgrid=from_header  First line number of velocity grid         \n"
"dlinevgrid=from_header  line number increment of velocity grid     \n"
"nlinevgrid=from_header  Number of lines of velocity grid   \n"
"ftvgrid=from_header     First Time/depth (in ms/m or ft) of velocity grid\n"
"dtvgrid=from_header     Time/depth sampling interval (in ms/m or ft)   \n"
"                         of velocity grid \n"
"ntvgrid=from_header     Number of time/depth samples of velocity grid  \n"
"hcdptype=0              Output HANDVEL cdp type (0=cdp 1=cdplbl)       \n" 
"                        the following 5 parameters are ignored when  \n"
"                        hcdptype=1  \n"
"cdp1=1                  first cdp Number of 3D master grid \n"
"trace1=1                first trace (crossline) number of 3D master grid\n"
"line1=1                 first line number of 3D master grid    \n"
"dline=1                 line number increment of 3D master grid    \n"
"ncdpline=ncdpvgrid      Number of cdp per line in 3D master grid   \n"
"                        (cdp number starts with cdp1 and increases by 1  \n"
"                        in 3D master grid)  \n"
"fcdphvel=fcdpvgrid      First trace number to output HANDVEL cards \n"
"dcdphvel=dcdpvgrid      Trace number increment to output HANDVEL cards \n"
"ncdphvel=1              Number of traces per line to output HANDVEL cards\n"
"flinehvel=flinevgrid    First line number to output HANDVEL cards  \n"
"dlinehvel=dlinevgrid    Line number increment to output HANDVEL cards  \n"
"nlinehvel=1             Number of lines to output HANDVEL cards\n"
"fthvel=ftvgrid          First time/depth (in ms/m or ft) to output HANDVEL\n" 
"dthvel=10*dtvgrid       Time/depth interval (in ms/m or ft) to output \n"
"                         HANDVEL\n" 
"nthvel=ntvgrid/10       Number of times/depths to output HVEL      \n"
"template=               template name of desired output (t,cdp) locations \n"
"                        in hanvel cards format.                        \n"
"                        (When this parameter is supplied and when number \n"
"                        of cdps in template is more than 1, output hanvel \n"
"                        will be the same (t,cdp) as those defined in the \n"
"                        template, (fcdphvel, dcdphvel, ncdphvel,   \n"
"                        flinehvel, dlinehvel, nlinehvel,       \n"
"                        fthvel, dthvel, nthvel) will be ignored.       \n"
"                        When this parameter is supplied and when number \n"
"                        of cdps in the template is 1, output hanvel \n"
"                        will be the same (t) as those defined in the \n"
"                        (fthvel, dthvel) will be ignored.)     \n"
"                        nthvel will be the maximum number of t-v pairs to \n"
"                        output per cdp. It defaults to 4096.       \n"
"nvfmax=4096             maximum number of velocity functions in template \n"
"ntvmax=256              maximum number of t-v pairs per velocity functions  \n"
"                        in template   \n"
"ivtype=0                Input velocity grid type (0=rms; 1=avg; 2=int) \n" 
"ovtype=0                Output HANDVEL velocity type (0=rms; 1=avg; 2=int)\n" 
"ittype=0                Input velocity grid time/depth type    \n"
"                        (0=time 1=depth)\n" 
"ottype=0                Output HANDVEL time/depth type (0=time 1=depth \n" 
"wbgrid=                 Name of the water bottom time horizon grid \n"
"                        if given, an extra t-v pair will be output at \n"
"                        the input horizon time \n"
"stgrid=                 Name of the salt top time horizon grid \n"
"                        if given, an extra t-v pair will be output at \n"
"                        the input horizon time \n"
"sbgrid=                 Name of the salt base time horizon grid \n"
"                        if given, an extra t-v pair will be output at \n"
"                        the input horizon time \n"
"nulltime=0              time value in the horizon grid to indicate no \n"
"                        horizon present at this grid location \n"
"\n"
"Notes:                                 \n"
" 1. This program can be used to make velocity contour plot using Disco \n"
"    plotting program.                          \n" 
" 2. When input velocity grid is a nonstandard grid file, i.e., without \n"
"    header, 9 grid parameters of input velocity grid must be supplied. \n"
" 3. The grid geometry (trace,line) position of wbgrid, stgrid and sbgrid \n"
"    must be the same as those of the velocity grid \n"
" 4. The three corners can be at the output 3D grid or at the output 3D \n"
"    master grid. First corner is always minimum trace and minimum line. \n"
"    Second corner is always maximum trace and minimum line. \n"
"    Third corner is always minimum trace and maximum line. \n"

"\n"
"AUTHOR:         J. Cignoli, Zhiming Li,       ,    25-aug-1993     \n";

void inserttime(float *ts, int *np, float tgrid, float nulltime);

main(int argc, char **argv)
{
    FILE *infp=stdin,*outfp=stdout;

    int ncdpvgrid,ntvgrid,ncdphvel,nthvel;
    float fcdpvgrid,dcdpvgrid,ftvgrid,dtvgrid,tmp;
    float fcdphvel,dcdphvel,fthvel,dthvel;
    float flinevgrid, dlinevgrid;
    float flinehvel, dlinehvel;
    int nlinevgrid,cdp1,line1,dline,ncdpline, nlinehvel;
    int ivtype,ovtype;
    int ittype,ottype;
    int trace1, hcdptype;

    float *tin, *tout, *vin, *vout;
    float o2, o3; 
    int ocdp;
    int it, i2vgrid, i3vgrid; 
    int i2, i3;

    ghed gh;
    int ierr;
    long long lpos;

    char *template;
    FILE *tmpfp;
    int *cdp, *nps, ncdp=0, icdp, nvfmax, ntvmax;
    float *ts, *vs;
    int io2, io3;

    char* outfmt="";
    char* ftype="";
    char* lunits="";

    char *wbgrid, *stgrid, *sbgrid;
    float nulltime;
    float *twb, *tst, *tsb, *tout0;
    int nthvel0;
    FILE *wbfp, *stfp, *sbfp;
    
    float lmk_x1;
    float lmk_x2;
    float lmk_x3;

    float lmk_y1;
    float lmk_y2;
    float lmk_y3;

    float lmk_trace1;
    float lmk_trace2;
    float lmk_trace3;

    float lmk_line1;
    float lmk_line2;
    float lmk_line3;
 
    double xout;
    double yout;
    float datum = 0.0;

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);

        if (!getparstring( "outfmt" ,&outfmt)) {
           outfmt="hvel";
        }

    if( !strcmp( outfmt ,"avf" ) ){

	if (!getparfloat("datum",&datum)) {
           datum=0.0;
        }

	if (!getparfloat("x1",&lmk_x1)) err("must specify x1");
	if (!getparfloat("x2",&lmk_x2)) err("must specify x2");
	if (!getparfloat("x3",&lmk_x3)) err("must specify x3");
	if (!getparfloat("y1",&lmk_y1)) err("must specify y1");
	if (!getparfloat("y2",&lmk_y2)) err("must specify y2");
	if (!getparfloat("y3",&lmk_y3)) err("must specify y3");
	if (!getparfloat("line1",&lmk_line1)) err("must specify line1");
	if (!getparfloat("line2",&lmk_line2)) err("must specify line2");
	if (!getparfloat("line3",&lmk_line3)) err("must specify line3");
	if (!getparfloat("trace1",&lmk_trace1)) err("must specify trace1");
	if (!getparfloat("trace2",&lmk_trace2)) err("must specify trace2");
	if (!getparfloat("trace3",&lmk_trace3)) err("must specify trace3");

        if (!getparstring( "ftype" ,&ftype )) err( "must specify ftype" );
        if (!getparstring( "lunits" ,&lunits )) err( "must specify lunits" );

        fprintf( outfp ,"# Produced by vgrid2hvel\n" );
        fprintf( outfp ,"# FUNCTION_TYPE = %s\n" ,ftype );
        fprintf( outfp ,"# LINEAR_UNITS = %s\n" ,lunits );
        fprintf( outfp ,"# DATUM = %f\n" ,datum );

     }


    ierr = fgetghdr(infp,&gh);
    if (!getparfloat("fcdpvgrid",&fcdpvgrid)) { 
        if(ierr==0) {
            getgval(&gh,"ocdp2",&fcdpvgrid);
        } else {
            err(" fcdpvgrid missing ");
        }  
    }
    if (!getparfloat("dcdpvgrid",&dcdpvgrid)) { 
        if(ierr==0) {
            getgval(&gh,"dcdp2",&dcdpvgrid);
        } else {
            err(" dcdpvgrid missing ");
        }  
    }
    if (!getparint("ncdpvgrid",&ncdpvgrid)) { 
        if(ierr==0) {
            getgval(&gh,"n2",&tmp);
            ncdpvgrid = (int)tmp;
        } else {
            err(" ncdpvgrid missing ");
        }  
    }
    if (!getparfloat("flinevgrid",&flinevgrid)) { 
        if(ierr==0) {
            getgval(&gh,"oline3",&flinevgrid);
        } else {
            err(" flinevgrid missing ");
        }  
    }
    if (!getparfloat("dlinevgrid",&dlinevgrid)) { 
        if(ierr==0) {
            getgval(&gh,"dline3",&dlinevgrid);
        } else {
            err(" dlinevgrid missing ");
        }  
    }
    if (!getparint("nlinevgrid",&nlinevgrid)) { 
        if(ierr==0) {
            getgval(&gh,"n3",&tmp);
            nlinevgrid = (int)tmp;
        } else {
            err(" ncdpvgrid missing ");
        }  
    }
    if (!getparfloat("ftvgrid",&ftvgrid)) { 
        if(ierr==0) {
            getgval(&gh,"o1",&ftvgrid);
        } else {
            err(" ftvgrid missing ");
        }  
    }
    if (!getparfloat("dtvgrid",&dtvgrid)) { 
        if(ierr==0) {
            getgval(&gh,"d1",&dtvgrid);
        } else {
            err(" dtvgrid missing ");
        }  
    }
    if (!getparint("ntvgrid",&ntvgrid)) { 
        if(ierr==0) {
            getgval(&gh,"n1",&tmp);
            ntvgrid = (int)tmp;
        } else {
            err(" ntvgrid missing ");
        }  
    }
    if (!getparint("ncdpline",&ncdpline)) ncdpline = ncdpvgrid;
    if (!getparint("cdp1",&cdp1)) cdp1 = 1;
    if (!getparint("trace1",&trace1)) trace1 = 1;
    if (!getparint("line1",&line1)) line1 = 1;
    if (!getparint("dline",&dline)) dline = 1;

    if (!getparfloat("fcdphvel",&fcdphvel)) fcdphvel = fcdpvgrid;
    if (!getparfloat("dcdphvel",&dcdphvel)) dcdphvel = dcdpvgrid;
    if (!getparint("ncdphvel",&ncdphvel)) ncdphvel = 1;
    if (!getparfloat("flinehvel",&flinehvel)) flinehvel = flinevgrid;
    if (!getparfloat("dlinehvel",&dlinehvel)) dlinehvel = dlinevgrid;
    if (!getparint("nlinehvel",&nlinehvel)) nlinehvel = 1;
    if (!getparfloat("fthvel",&fthvel)) fthvel = ftvgrid;
    if (!getparfloat("dthvel",&dthvel)) dthvel = 10. * dtvgrid;
    if (!getparint("nthvel",&nthvel)) nthvel = ntvgrid/10;

    if (!getparint("ivtype",&ivtype)) ivtype=0;
    if (!getparint("ovtype",&ovtype)) ovtype=0;
    if (!getparint("ittype",&ittype)) ittype=0;
    if (!getparint("ottype",&ottype)) ottype=0;
    if (!getparint("hcdptype",&hcdptype)) hcdptype=0;

    if(getparstring("template",&template)) {
                tmpfp = efopen(template,"r");
                if (!getparint("nvfmax",&nvfmax)) nvfmax = 4096;
                if (!getparint("ntvmax",&ntvmax)) ntvmax = 256;
        if (!getparint("nthvel",&nthvel)) nthvel= 4096; 

        /* arrays used to store all VS3D card's x,y,time and velocity */
        cdp = (int*)emalloc(nvfmax*sizeof(int));
        nps = (int*)emalloc(nvfmax*sizeof(int));
        ts = (float*) emalloc(nvfmax*ntvmax*sizeof(float));
        vs = (float*) emalloc(nvfmax*ntvmax*sizeof(float));
        ncdp = 0;
        bzero(nps,nvfmax*sizeof(int));
        hvelread(tmpfp,cdp,ts,vs,&ncdp,nps,ntvmax,nvfmax);
    }

    if(!getparfloat("nulltime",&nulltime)) nulltime = 0.0;
    twb = (float*) malloc(ncdpvgrid*nlinevgrid*sizeof(float));
    tst = (float*) malloc(ncdpvgrid*nlinevgrid*sizeof(float));
    tsb = (float*) malloc(ncdpvgrid*nlinevgrid*sizeof(float));
    for(it=0;it<ncdpvgrid*nlinevgrid;it++) {
        twb[it] = nulltime;
        tst[it] = nulltime;
        tsb[it] = nulltime;
    }
    if(getparstring("wbgrid",&wbgrid)) {
        wbfp = efopen(wbgrid,"r");
        efread(twb,sizeof(float),ncdpvgrid*nlinevgrid,wbfp);
    }
    if(getparstring("stgrid",&stgrid)) {
        stfp = efopen(stgrid,"r");
        efread(tst,sizeof(float),ncdpvgrid*nlinevgrid,stfp);
    }
    if(getparstring("sbgrid",&sbgrid)) {
        sbfp = efopen(sbgrid,"r");
        efread(tsb,sizeof(float),ncdpvgrid*nlinevgrid,sbfp);
    }
    
    ierr = fgetghdr(infp,&gh);

    nthvel0 = nthvel;
    tin = (float*)malloc(ntvgrid*sizeof(float));
    vin = (float*)malloc(ntvgrid*sizeof(float));
    tout0 = (float*)malloc(nthvel*sizeof(float));
    tout = (float*)malloc((nthvel+3)*sizeof(float));
    vout = (float*)malloc((nthvel+3)*sizeof(float));

    for(it=0;it<ntvgrid;it++) tin[it] = ftvgrid + it*dtvgrid;

    if(ncdp==1) {
        nthvel = nps[0];
        for(it=0;it<nthvel;it++) tout[it] = ts[it];
    } else if(ncdp==0) {
        for(it=0;it<nthvel;it++) tout0[it] = fthvel + it*dthvel;
    }

    if(ncdp>1) {
        for(icdp=0;icdp<ncdp;icdp++) {
            ocdp = cdp[icdp];
            if(hcdptype==0) {
                io3 = (ocdp-cdp1)/ncdpline*dline + line1;
                /* io2 = ocdp - (io3-line1)/dline*ncdpline + trace1 - 1; */
                io2 = ocdp - (io3-line1)/dline*ncdpline;
            } else {
                tmp = ocdp/10000;
                io3 = tmp;
                tmp = ocdp - io3*10000 + 0.5;
                io2 = tmp;
            }
            tmp = (io3 - flinevgrid)/dlinevgrid + 0.5;
            i3vgrid = (int) tmp;
            tmp = (io2 - fcdpvgrid)/dcdpvgrid + 0.5;
            i2vgrid = (int) tmp;
            if(i3vgrid<0) i3vgrid=0;
            if(i3vgrid>=nlinevgrid) i3vgrid=nlinevgrid-1;
            if(i2vgrid<0) i2vgrid=0;
            if(i2vgrid>=ncdpvgrid)  i2vgrid=ncdpvgrid-1;
            lpos = i2vgrid+i3vgrid*ncdpvgrid;
            lpos = lpos * ntvgrid*sizeof(float);
            fseek64(infp,lpos,0);
            fread(vin,sizeof(float),ntvgrid,infp);

            nthvel = nps[icdp];
            for(it=0;it<nthvel;it++) tout[it] = ts[it+icdp*ntvmax];
            /* insert times of wbgrid, stgrid, sbgrid */
            inserttime(tout,&nthvel,twb[i2vgrid+i3vgrid*ncdpvgrid],nulltime);
            inserttime(tout,&nthvel,tst[i2vgrid+i3vgrid*ncdpvgrid],nulltime);
            inserttime(tout,&nthvel,tsb[i2vgrid+i3vgrid*ncdpvgrid],nulltime);

            /* time/depth conversion if needed */
            vconvert(tin,vin,ntvgrid,ivtype,ittype,
                     tout,vout,nthvel,ovtype,ottype);

        /*
        for(it=0;it<ntvgrid;it++) 
            fprintf(stderr,"tin=%f vin=%f \n",tin[it],vin[it]);
        for(it=0;it<nthvel;it++) 
            fprintf(stderr,"tout=%f vout=%f \n",tout[it],vout[it]);
        */

        if( !strcmp( outfmt ,"hvel" )) {
           printhvel(ocdp,nthvel,tout,vout,outfp);

        }else if( !strcmp( outfmt ,"avf" )){

		   sl2xydb(lmk_trace1,lmk_line1,lmk_x1,lmk_y1
                  ,lmk_trace2,lmk_line2,lmk_x2,lmk_y2
			  ,lmk_trace3,lmk_line3,lmk_x3,lmk_y3
                  ,(double)io2,(double)io3,&xout,&yout);
           if( vout[1] > 0.0 ){
              printavf(ocdp,xout,yout,nthvel,tout,vout,outfp);
           }


        }else{
            fprintf( stderr ,"Unknown output format: %s\n" ,outfmt );
            exit(-1);
        }
        if(hcdptype==0) { 
fprintf(stderr,"Output at trace=%d line=%d cdp=%d i2vg=%d i3vg=%d\n",
                io2,io3,ocdp,i2vgrid+1,i3vgrid+1);
        } else {
fprintf(stderr,"Output at trace=%d line=%d cdplbl=%d i2vg=%d i3vg=%d\n",
                io2,io3,ocdp,i2vgrid+1,i3vgrid+1);
        }
        }
    } else {
        for(i3=0;i3<nlinehvel;i3++) {
            o3 = flinehvel+i3*dlinehvel;
            io3 = o3;
            tmp = (o3 - flinevgrid)/dlinevgrid + 0.5;
            i3vgrid = (int) tmp;
            if(i3vgrid<0) i3vgrid=0;
            if(i3vgrid>=nlinevgrid) i3vgrid=nlinevgrid-1;
            for(i2=0;i2<ncdphvel;i2++) {
                o2 = fcdphvel+i2*dcdphvel;
                io2 = o2;
                tmp = (o2 - fcdpvgrid)/dcdpvgrid + 0.5;
                i2vgrid = (int) tmp;
                if(i2vgrid<0) i2vgrid=0;
                if(i2vgrid>=ncdpvgrid)  i2vgrid=ncdpvgrid-1;

                if(hcdptype==0) {
                    tmp = o2 - trace1 + (o3-line1)/dline*ncdpline + cdp1 + 0.5; 
                    ocdp = tmp;
                } else {
                    ocdp = io3*10000 + io2;
                }
                lpos = i2vgrid+i3vgrid*ncdpvgrid;
                lpos = lpos * ntvgrid*sizeof(float);
                fseek64(infp,lpos,0);
                efread(vin,sizeof(float),ntvgrid,infp);

                nthvel = nthvel0;
                for(it=0;it<nthvel;it++) tout[it] = tout0[it];
                /* insert times of wbgrid, stgrid, sbgrid */
                inserttime(tout,&nthvel,
                    twb[i2vgrid+i3vgrid*ncdpvgrid],nulltime);
                inserttime(tout,&nthvel,
                    tst[i2vgrid+i3vgrid*ncdpvgrid],nulltime);
                inserttime(tout,&nthvel,
                    tsb[i2vgrid+i3vgrid*ncdpvgrid],nulltime);
                /* time/depth conversion if needed */
                vconvert(tin,vin,ntvgrid,ivtype,ittype,
                        tout,vout,nthvel,ovtype,ottype);

                if( !strcmp( outfmt ,"hvel" )) {
                   printhvel(ocdp,nthvel,tout,vout,outfp);

                }else if( !strcmp( outfmt ,"avf" )){

		   sl2xydb(lmk_trace1,lmk_line1,lmk_x1,lmk_y1
                          ,lmk_trace2,lmk_line2,lmk_x2,lmk_y2
			  ,lmk_trace3,lmk_line3,lmk_x3,lmk_y3
                          ,(double)io2,(double)io3,&xout,&yout);
                   if( vout[1] > 0.0 ){
                      printavf(ocdp,xout,yout,nthvel,tout,vout,outfp);
                   }


                }else{
                    fprintf( stderr ,"Unknown output format: %s\n" ,outfmt );
                    exit(-1);
                }
                fprintf(stderr
                ,"Output at trace=%d line=%d cdp=%d i2vg=%d i3vg=%d\n"
                ,io2,io3,ocdp,i2vgrid+1,i3vgrid+1);
            }
        }
    }
    
    free(tout0);
    free(tout);
    free(vout);
    free(tin);
    free(vin);
    free(twb);
    free(tst);
    free(tsb);
}

void inserttime(float *ts, int *np, float tgrid, float nulltime) { 

    int i, j;
    int ns;

    ns = *np;

    if(tgrid==nulltime) return;

    if(tgrid<ts[0]) {
        for(j=ns;j>0;j--) ts[j] = ts[j-1];
        ts[0] = tgrid;
        *np += 1;
    } else if(tgrid > ts[ns-1]) {
        ts[ns] = tgrid;
        *np += 1;
    } else {
        for(i=0;i<ns-1;i++) {
            if((tgrid>ts[i])&&(tgrid<ts[i+1])) {
                for(j=ns;j>i+1;j--) ts[j] = ts[j-1];
                ts[i+1] = tgrid;
                *np +=  1;
                break;
            }
        }
    }
}
