#include <stdio.h>
#include <osfcn.h>
#include <string.h>
#include "velplane.h"
#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "su.h"

char * sdoc = 
"hfl2grd - transform an hfile or ifile to a grid \n"
"\n"
"hfl2grd [parameters] < hfile > grid	\n"
"\n"
"Required Parameters:				\n"
"\tnz=(number of depth samples)       	\n"
"\tnx=(number of lateral samples)       \n"
"OR\n"
"\tdz=(depth sampling rate)       	\n"
"\tdx=(lateral sampling rate)       \n"
"\n"
"Optional Parameters:				\n"
"\thfile=in=\t(horizon file)				\n"
"\n"
"\toz=0.\t(origin of depth samples)       	\n"
"\tox=xmin\t(origin of lateral samples)       \n"
"\n"
"\tdz=(zmax-zmin)/nz\t(depth sampling interval)       	\n"
"\tdx=(xmax-xmin)/nx\t(lateral sampling interval)       \n"
"   OR\n"
"\tnz=(zmax-zmin)/dz\t(number of depth samples)       	\n"
"\tnx=(xmax-xmin)/dx\t(number of lateral samples)       \n"
"\n"
"\tsmx=0\t(length of smooth window in x direction)       \n"
"\tsmz=0\t(length of smooth window in z direction)       \n"
"\n"
"\tout=\toutput velgrid\n"
"\n"
"BUGS\n"
"\n"
"If an ifile is specified the horizon number 1 should be the top horizon\n"
"\n"
"AUTHOR:		J.C. Dulac	06/10/92	\n"
;

/* modified history	*/
/* 	Zhiming Li,  8-25-92	added grid header */

main(int argc,char *argv[]) {
      initargs(argc,argv);
      askdoc(1);

      char * velFile ;
      if( !getparstring("hfile",&velFile) && !getparstring("in",&velFile) ) {
          velFile=nil;
      }

      // get velPlane
      DVelPlane * velPlane = new DVelPlane(velFile) ;
      if( !velPlane-> OK() ) err("Error reading input HFILE") ;

      float xmin, xmax, zmax, zmin;
      int cdpmin, cdpmax ;
      velPlane-> XExtrems(xmin,xmax,cdpmin,cdpmax) ;
      velPlane-> ZExtrems(zmin,zmax) ;

      int nz, nx ;
      float ox, dx, oz, dz ;
      if( !getparfloat("ox",&ox) ) { ox = xmin ; }
      if( !getparfloat("oz",&oz) ) { oz = 0. ; }
       
      if( !getparint("nz",&nz) ) { 
         if( !getparfloat("dz",&dz) ) { 
             err("nz or dz needed") ; 
         } else {
             nz = int((zmax-oz)/dz + 0.5) ;
	     fprintf(stderr,"nz=%d\n",nz) ;
         }
      } else if( !getparfloat("dz",&dz) ) { 
           dz = (zmax-oz)/nz ; 
      }

      if( !getparint("nx",&nx) ) { 
         if( !getparfloat("dx",&dx) ) { 
             err("nx or dx needed") ; 
         } else {
             nx = int((xmax-ox)/dx + 0.5) ;
	     fprintf(stderr,"nx=%d\n",nx) ;
         }
      } else if( !getparfloat("dx",&dx) ) { 
           dx = (xmax-ox)/nx ; 
      }

      // get output file (if any write to standard output)
      char * out ;
      if( !getparstring("out",&out) ) { out = nil ; }

      // get smooth factor
      int smx, smz ;
      if( !getparint("smx",&smx) ) { smx = 0 ; }
      if( !getparint("smz",&smz) ) { smz = 0 ; }

      // get ocdp and dcdp 
      float dcdp, ocdp;
      dcdp = (float)(cdpmax-cdpmin)/(xmax-xmin)*dx;
      ocdp = (float)cdpmin + (float)(cdpmax-cdpmin)/(xmax-xmin)*(ox-xmin);

      // compute grid
      velPlane-> Grid(nz,oz,dz,nx,ox,dx,smz,smx) ;

      // output grid
      velPlane-> GetVelGrid()-> Write(out) ;

      // Write grid header 
      char * rhdr = out ;
      char * hdrgrid ;
      if( rhdr ) {
         hdrgrid = new char[strlen(rhdr)+9+1+1] ;
         char * dot = strrchr(rhdr,'.') ;
         if( dot ) {
	    strncpy(hdrgrid,rhdr,dot-rhdr+1);
	    hdrgrid[dot-rhdr] = '\0' ;
	    strcat(hdrgrid,".hdr") ;
	 } else {
	    strcpy(hdrgrid,rhdr);
	    strcat(hdrgrid,"_vgrd.hdr") ;
         }
      } else {
	 hdrgrid = strdup("velgrid.hdr") ;
      }

      FILE * fhdr = fopen(hdrgrid,"w") ;
      fprintf(fhdr,"n1=%d o1=%g d1=%g\n",nz,oz,dz) ;
      fprintf(fhdr,"n2=%d o2=%g d2=%g\n",nx,ox,dx) ;
      fclose(fhdr) ;

      // append header to the end of grid  
      FILE *gfp;
      ghed gh;
      float o3=0.,o4=0.,o5=0.,d3=0.,d4=0.,d5=0.;
      int n3=1,n4=1,n5=1,dtype=4; 
      float scale=1.e-6,dline3=0.,oline3=0.;
      float vmin, vmax;

      if (out==nil) {
		gfp = stdout;
      } else {
		gfp = fopen(out,"r+");
      }
      // get max and min values of velocity grid 
      velPlane->GetVelGrid()->MinMax(vmin, vmax);

      // update grid header
      toghdr(&gh,&scale,&dtype,&nz,&nx,&n3,&n4,&n5,
		&dz,&dx,&d3,&d4,&d5,&oz,&ox,&o3,&o4,&o5,
		&dcdp,&dline3,&ocdp,&oline3,
		&vmin,&vmax);
      fputghdr(gfp,&gh);

      return 0 ;
}
