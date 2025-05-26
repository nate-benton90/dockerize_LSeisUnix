#include <math.h>

#include "su.h"
#include "segy.h"
#include "par.h"

void interpolate( 
    char   method 
   ,float* xin 
   ,float* yin 
   ,float* xout
   ,float* yout 
   ,int    nin
   ,int    nout
);

static const char ID[] = "Source file: \
   $RCSfile: supressure.c,v $ \
   $Revision: 1.41 $ \
   $Date: 2006/05/02 15:01:23 $ ";

char    *sdoc =
"\n SUPRESSURE - calculate pore pressures from interval velocity"
"\n"
"\n    supressure <input_velocity.su "
"\n"
"\nRequired parameters:"
"\n"
"\nI=        - Eaton's exponent "
"\n"
"\nvmax= - maximum velocity"
"\nvmin= - minimum velocity"
"\nkvp=  - normal compaction velocity trend exponent"
"\n"
"\nHG=   - hydrostatic gradient coefficient. If not set calculation "
"\n        uses approximation to depth dependent brine density"
"\n"
"\nrhomin=   - minimum density"
"\nrhomax=   - maximum density"
"\nkrho=     - normal compaction density trend exponent"
"\n"
"\n"
"\nOptional parameters:"
"\n"
"\nrho_gas=0.2 - gas density "
"\nvsalt=14500 - salt velocity "
#if 0
"\nexp=0       - =0  value = (max*min)/( (max-min)*exp(k*z)+min)"
"\n              =1  value = max - (max-min)*exp(k*z)"
#endif
"\n"
"\nzmin=0.0  - starting depth for calculation"
"\n"
"\nLG=0.0    - lithostatic gradient coefficient. Default is to integrate "
"\n            density trend function."
"\n"
"\ndt=       - defaults to value in dt/1000, specify float to override"
"\ndz=       - defaults to value in dt/1000, specify float to override"
"\n"
"\n  All input is expected in ft & ft/s except mudrock trend"
"\n"
"\nmethod=0  - calculation method "
"\n            =0 Eaton's method"
"\n            =1 normalized Eaton's method"
"\n"
"\nfrac=0    - fracture pressure calculation"
"\n            =0 Hubbert-Willis"
"\n            =1 Terzaghi"
"\n            =2 Matthews-Kelly-Constant-Bourgoyne"
"\n            =3 Fore-Beardsley"
"\n"
"\n The following are used if frac=0 or frac=1"
"\n"
"\nA=-1.1724 - Castagna mudrock coefficient"
"\nB=0.8621  - Castagna mudrock coefficient Vs = a + b * Vp (km/s)"
"\n"
"\n The following are used if frac=2"
"\n"
"\nMKCB_a=0.629   Constant-Bourgoyne 'a' term"
"\nMKCB_b=1.28e-4 Constant-Bourgoyne 'b' term"
"\n"
"\n The following are used if frac=3"
"\n"
"\nFBmin=      - minimum FP/OB"
"\nFBmax=      - maximum FP/OB"
"\nFBk=        - FB exponent"
"\n"
"\nppg=0     - output units 0=psi 1=ppg "
"\ntime=0    - vertical units 1=time 0=depth"
"\n"
"\n"
"\n At least one output volume must be specified."
"\n"
"\nep=   - name of effective pressure output volume"
"\nfp=   - name of fracture pressure output volume"
"\nop=   - name of over pressure output volume"
"\nob=   - name of overburden pressure output volume"
"\npp=   - name of pore pressure output volume"
"\ndv=   - name of differential velocity output volume"
"\nnh=   - name of normal hydrostatic pressure output volume"
"\nch=   - name of column height output volume"
"\npep=  - name of percent effective pressure output volume"
"\npop=  - name of percent over pressure output volume"
"\nmww=  - name of mud weight window output volume"
"\n"
"\nNotes:"
"\n This program calculates multiple output volumes from the input."
"\n"
"\n !!! You MUST set swdep in the header to the correct water depth !!!"
"\n"
"\n           !!! All input MUST be in ft & ft/s !!!"
"\n"
"\n Author: Reginald H. Beardsley "__DATE__" rhb@acm.org"
"\n";

main(int argc, char* argv[] ){

   segytrace vi;   /* input observed seismic velocity */

   segytrace nh;   /* normal hydrostatic pressure     */
   segytrace ep;   /* effective pressure              */
   segytrace fp;   /* fracture pressure               */
   segytrace op;   /* overpressure                    */
   segytrace ob;   /* overburdenpressure              */
   segytrace pp;   /* pore pressure                   */
   segytrace dv;   /* differential velocity           */
   segytrace ch;   /* column height                   */
   segytrace pep;  /* percent effective  pressure     */
   segytrace pop;  /* percent over  pressure          */
   segytrace mww;  /* mud weight window               */

   String epname  = 0;
   String fpname  = 0;
   String opname  = 0;
   String obname  = 0;
   String ppname  = 0;
   String nhname  = 0;
   String dvname  = 0;
   String chname  = 0;
   String pepname = 0;
   String popname = 0;
   String mwwname = 0;

   FILE* vifp = stdin;

   FILE* epfp  = 0;
   FILE* fpfp  = 0;
   FILE* opfp  = 0;
   FILE* obfp  = 0;
   FILE* ppfp  = 0;
   FILE* nhfp  = 0;
   FILE* dvfp  = 0;
   FILE* chfp  = 0;
   FILE* pepfp = 0;
   FILE* popfp = 0;
   FILE* mwwfp = 0;

   float Vmin;
   float Vmax;
   float OB;

   float MKCB_a = 0.629;
   float MKCB_b = 1.28e-4;
   float Rms;

   float A=-1.1724;
   float B=0.8621;

   float Rho_a = -0.0261;
   float Rho_b =  0.373;
   float Rho_c =  1.458;

   float I;
   float Kvp;

   float Zmin=0.0;

   float Krho;
   float Rho_min;
   float Rho_max;

   float FTmin=1.12;
   float FTmax=0.917;
   float FTk=-0.013;

   float T;
   float T0=70.0;
   float dT=0.0145;
   float S=50000.0;

   float dTout=0.0;

   float FBmin;
   float FBmax;
   float FBk;

   float LG=0.0;
   float HG=0.0;
   float Vp;
   float Vs;
   float sigma;

   float P_a0 =  4.7971767E-08;
   float P_b0 = -1.1742946E-10;
   float P_c0 = -2.3712071E-13;
   float P_a1 =  1.0040964E-02;
   float P_b1 = -2.4035580E-06;
   float P_c1 =  6.7821539E-09;
   float P_a2 =  9.7948082E-02;
 
   float a;
   float b;
   float c;

   float Vnct;
   float Vobs;
   float Vsalt=14500;
   float Rho;
   float Rho_gas = 0.2;
   float WD;
   float Z;
   float dZout = 0.0;

   int i;
   int j = 0;
   int frac = 0;
   int method = 0;

   char InterpType[2] = { 'l',0 };

   int time = 0;
   int ppg = 0;

   float Tin [1024];

   float Tout[1024];
   float Dout[1024];
   int Nout;

   int dbg = -1;

   initargs(argc, argv);
   askdoc(1);

   /*---------------------*/
   /* optional parameters */
   /*---------------------*/

   getparfloat( "dt" ,&dTout );
   getparfloat( "dz" ,&dZout );
   getparfloat( "LG" ,&LG );
   getparfloat( "HG" ,&HG );
   getparfloat( "rho_gas" ,&Rho_gas );
   getparfloat( "zmin" ,&Zmin );
   getparfloat( "MKCB_a" ,&MKCB_a );
   getparfloat( "MKCB_b" ,&MKCB_b );
   getparfloat( "vsalt" ,&Vsalt );

   getparint( "method" ,&method );

   getparint( "time" ,&time );
   getparint( "ppg" ,&ppg );

   if( method <= 1 ){

      if( !getparfloat( "I" ,&I ) ){
         err( "I not specified" );
      }

   }else{
      err( "method invalid" );
   }

   getparint( "frac" ,&frac );

   if( (frac < 0) || (frac > 3) ){
      err( "frac invalid" );

   }else if( frac == 3 ){

      if( !getparfloat( "FBmin" ,&FBmin ) ){
         err( "FBmin not specified" );
      }

      if( !getparfloat( "FBmax" ,&FBmax ) ){
         err( "FBmax not specified" );
      }

      if( !getparfloat( "FBk" ,&FBk ) ){
         err( "FBk not specified" );
      }

   }

   /*---------------------*/
   /* required parameters */
   /*---------------------*/

   if( !getparfloat( "kvp" ,&Kvp ) ){
      err( "kvp not specified" );
   }

   if( !getparfloat( "vmin" ,&Vmin ) ){
      err( "vmin not specified" );
   }

   if( !getparfloat( "vmax" ,&Vmax ) ){
      err( "vmax not specified" );
   }

   if( !getparfloat( "krho" ,&Krho ) ){
      err( "krho not specified" );
   }

   if( !getparfloat( "rhomin" ,&Rho_min ) ){
      err( "rhomin not specified" );
   }

   if( !getparfloat( "rhomax" ,&Rho_max ) ){
      err( "rhomax not specified" );
   }

   /*----------------------*/
   /* get output filenames */
   /*----------------------*/

   getparstring( "ep"  ,&epname  );
   getparstring( "fp"  ,&fpname  );
   getparstring( "ob"  ,&obname  );
   getparstring( "op"  ,&opname  );
   getparstring( "pp"  ,&ppname  );
   getparstring( "dv"  ,&dvname  );
   getparstring( "nh"  ,&nhname  );
   getparstring( "ch"  ,&chname  );
   getparstring( "pep" ,&pepname );
   getparstring( "pop" ,&popname );
   getparstring( "mww" ,&mwwname );

   /*-------------------*/
   /* open output files */
   /*-------------------*/

   if( epname && !(epfp=fopen( epname ,"w" )) ){
      err( "unable to open ep" );
   }

   if( fpname && !(fpfp=fopen( fpname ,"w" )) ){
      err( "unable to open fp" );

   }

   if( opname && !(opfp=fopen( opname ,"w" )) ){
      err( "unable to open op" );
   }

   if( ppname && !(ppfp=fopen( ppname ,"w" )) ){
      err( "unable to open pp" );
     
   }

   if( obname && !(obfp=fopen( obname ,"w" )) ){
      err( "unable to open ob" );
      
   }

   if( dvname && !(dvfp=fopen( dvname ,"w" )) ){
      err( "unable to open dv" );

   }

   if( nhname && !(nhfp=fopen( nhname ,"w" )) ){
      err( "unable to open nh" );

   }

   if( chname && !(chfp=fopen( chname ,"w" )) ){
      err( "unable to open ch" );

   }

   if( pepname && !(pepfp=fopen( pepname ,"w" )) ){
      err( "unable to open pep" );

   }

   if( popname && !(popfp=fopen( popname ,"w" )) ){
      err( "unable to open pop" );

   }

   if( mwwname && !(mwwfp=fopen( mwwname ,"w" )) ){
      err( "unable to open mww" );

   }

   /*------------------------------------*/
   /* make sure at least one output file */
   /*------------------------------------*/

   if( !epfp  && !fpfp && !mwwfp && !nhfp
    && !pepfp && !obfp && !ppfp && !chfp
    && !opfp  && !dvfp && !popfp ){
      err( "no output specified" );
   }

   /*-----------------------*/
   /* get first input trace */
   /*-----------------------*/

   if( !fgettr( vifp ,&vi ) ){
      err( "Unable to read first trace!" );
   }

   /*-------------------------*/
   /* override dT if required */
   /*-------------------------*/

   if( time && dTout == 0.0 ){
      dTout = vi.dt*1.0e-6;

   }else if( !time && dZout == 0.0 ){
      dZout = vi.dt*1.0e-3;
      
   }

   /*----------------------------*/
   /* loop over all input traces */
   /*----------------------------*/

   do {

      /*---------------------------*/
      /* initialize output headers */
      /*---------------------------*/

      if( !time ){

         if(  dZout*1000 > 65535 ){
            vi.dt = (unsigned short)dZout;
         }else{
            vi.dt = (unsigned short)(dZout*1000);
         }

      }else{

         if( dTout*1e6 > 65535 ){
            vi.dt = (unsigned short)(dTout*1e3);
         }else{
            vi.dt = (unsigned short)(dTout*1e6);
         }
      }

      if( epfp ){
         memcpy( &ep  ,&vi ,240 );
      }

      if( fpfp ){
         memcpy( &fp  ,&vi ,240 );
      }

      if( opfp ){
         memcpy( &op  ,&vi ,240 );
      }

      if( obfp ){
         memcpy( &ob  ,&vi ,240 );
      }

      if( ppfp ){
         memcpy( &pp  ,&vi ,240 );
      }

      if( nhfp ){
         memcpy( &nh  ,&vi ,240 );
      }

      if( chfp ){
         memcpy( &ch  ,&vi ,240 );
      }

      if( dvfp ){
         memcpy( &dv  ,&vi ,240 );
      }

      if( pepfp ){
         memcpy( &pep ,&vi ,240 );
      }

      if( popfp ){
         memcpy( &pop ,&vi ,240 );
      }

      if( mwwfp ){
         memcpy( &mww ,&vi ,240 );
      }

      WD = vi.swdep;

      Z = 0.0;

      j++;

      /*---------------*/
      /* process input */
      /*---------------*/

      for( i=0; i<vi.ns; i++ ){

         if( i == dbg ){
            printf( "break\n" );
         }

         Vobs = vi.data[i];

         if( time ){
            dZout = 0.5 * Vobs * dTout;
         }

         if( Z < Zmin || Z < WD ){
            Vnct = Vobs;
            
         }else{
#if 0
            Vnct = (Vmin*Vmax)/((Vmax-Vmin)*exp(-Kvp*(Z-WD)) + Vmin);
#else
            Vnct = Vmax - (Vmax - Vmin)*exp(-Kvp*(Z-WD));
#endif

         }

         Vp = Vobs *0.3048e-3;
         Vs = A + B *Vp;

         sigma = (Vp*Vp - 2*Vs*Vs) / (2*Vp*Vp - 2*Vs*Vs);

         /*---------------------------------------*/
         /* calculate normal hydrostatic gradient */
         /*---------------------------------------*/

         if( HG ){

            nh.data[i] = HG*Z;
  
         }else{

            nh.data[i] = (P_a0+P_b0*T+P_c0*S)*Z*Z*.3048*.3048 
                + (P_a1+P_b1*T+P_c1*S)*Z*.3048 
                + P_a2;

            /*--------------------------*/
            /* convert from MPa to psia */
            /*--------------------------*/

            nh.data[i] *= 145.0378;

         }

         /*-----------------------------------------*/
         /* compute vertical stress from overburden */
         /*-----------------------------------------*/

         if( Z <= WD ){

            ob.data[i] = nh.data[i];

         }else{

            if( LG ){

               /*---------------------------------*/
               /* compute using constant gradient */
               /*---------------------------------*/

               ob.data[i] = WD*HG + LG*(Z-WD);

            }else{

               /*----------------------------------------*/
               /* compute using integrated density trend */
               /*----------------------------------------*/

               if( Vobs >= Vsalt && Vobs > Vnct ){
                  Rho = 2.12;

               }else{
#if 0
                  Rho = Rho_max - (Rho_max-Rho_min)*exp(-Krho*(Z-WD));
#else
                  Rho = (Rho_max*Rho_min)
                      / ( (Rho_max-Rho_min)*exp(-Krho*(Z-WD))+Rho_min );
#endif

               }

               ob.data[i] = ob.data[i-1] + 0.43353 * Rho * dZout;

            }

         }

         /*---------------------------------*/
         /* account for water depth effects */
         /*---------------------------------*/

         if( Z < WD ){
            c = 0.0;
            Vnct = Vmin;

         }else{
            c = (ob.data[i] - nh.data[i]);

         }

         dv.data[i] = Vnct - Vobs;

         /*------------------------------*/
         /* calculate effective pressure */
         /*------------------------------*/

         if( method == 1 ){

            /*---------------------------*/
            /* normalized Eaton's method */
            /*---------------------------*/

            a = Vobs - Vmin;
            b = Vnct - Vmin;

            if( Vnct > Vmin && Vobs > Vmin ){
               ep.data[i] = c*pow( a / b ,I );

            }else{
               ep.data[i] = 0.0;

            }

         }else{

            /*----------------*/
            /* Eaton's method */
            /*----------------*/

            ep.data[i] = c*pow( Vobs / Vnct ,I );

         }

         /*--------------------------*/
         /* bound effective pressure */
         /*--------------------------*/

         ep.data[i] = ep.data[i] > c ? c : ep.data[i];

         /*-------------------------*/
         /* calculate over pressure */
         /*-------------------------*/

         if( Vobs > Vmin ){
            op.data[i] = c - ep.data[i];

         }else{
            op.data[i] = 0.0;

         }

         /*-------------------------*/
         /* calculate pore pressure */
         /*-------------------------*/

         if( Vobs > Vmin ){


            pp.data[i] = nh.data[i] + op.data[i];
            

         }else{
            pp.data[i] = 0.0;

         }

         if( pp.data[i] > ob.data[i] ){
            pp.data[i] = ob.data[i];
         }

         /*--------------------------------*/
         /* calculate pressure percentages */
         /*--------------------------------*/

         if( (ep.data[i] + op.data[i] ) != 0.0 ){

            pep.data[i] = (100.0 * ep.data[i]) 
                       / (ep.data[i] + op.data[i] );

            pop.data[i] = (100.0 * op.data[i]) 
                       / (ep.data[i] + op.data[i] );

         }else{
            pep.data[i] = 100.0;
            pop.data[i] =   0.0;

         }

/*--------------------------------------------------------------------*\
   calculate fracture pressure based on various models.
\*--------------------------------------------------------------------*/

         if( Vobs > Vmin && Z > WD && Vobs < Vsalt ){

            if( frac == 0 ){

               /* Hubbert-Willis */

               fp.data[i] = ep.data[i]*sigma/(1-sigma) + pp.data[i];

            }else if( frac == 1){

               /* Terzaghi */

               fp.data[i] = 2*ep.data[i]*sigma/(1-sigma) + pp.data[i];

            }else if( frac == 2){

               /* Matthews-Kelly-Constant-Bourgoyne */

               Rms = 1.0 - MKCB_a*exp(-MKCB_b*(Z-WD));
               fp.data[i] = Rms*ep.data[i] + pp.data[i];

            }else if( frac == 3){
#if 0
               /* Fore-Beardsley */
               fp.data[i] = ob.data[i]*(FBmin*FBmax) 
                           / ((FBmax-FBmin)*exp(-FBk*(Z-WD))+FBmin);
#else
               fp.data[i] = nh.data[i] 
                          + c*(FBmax-(FBmax-FBmin)*exp(-FBk*(Z-WD)));
#endif
            }else{

               /* default to no answer*/
               fp.data[i] = ob.data[i];

            }


         }

         if( Vobs >= Vsalt ){

            fp.data[i]  = 0.0;
            pp.data[i]  = 0.0;
            op.data[i]  = 0.0;
            ch.data[i]  = 0.0;

            pop.data[i] = 0.0;
            pep.data[i] = 100.0;

            ep.data[i]  = ob.data[i] - nh.data[i];

         }else if( Vobs < Vsalt && fp.data[i] > 0.0 ){
            ch.data[i] = (fp.data[i] - pp.data[i]) 
                        / (0.43353 * (Rho - Rho_gas ));

         }else{
            ch.data[i] = 0.0;

         }

         if( ch.data[i] > 8000.0 ){
            ch.data[i] = 8000.0;
         }

         mww.data[i] = fp.data[i] - pp.data[i];

         if( ep.data[i]  != ep.data[i]
          || fp.data[i]  != fp.data[i]
          || op.data[i]  != op.data[i]
          || ob.data[i]  != ob.data[i]
          || pp.data[i]  != pp.data[i]
          || nh.data[i]  != nh.data[i]
          || ch.data[i]  != ch.data[i]
          || dv.data[i]  != dv.data[i] 
          || mww.data[i] != mww.data[i]
          || pep.data[i] != pep.data[i]
          || pop.data[i] != pop.data[i] ){
            fprintf( stderr ,"NaN trace=%d sample=%d\n" ,j ,i );
         }

         Z += dZout;
      }

/*--------------------------------------------------------------------*\
            Convert from PSI to PPG if requested
\*--------------------------------------------------------------------*/

      if( ppg ){

         Z = 0;

         for( i=1; i<vi.ns; i++ ){

            if( time ){
               Z += vi.data[i] * 0.5 * dTout;

            }else{
               Z += dZout;
            }

            nh.data[i]  /= Z*0.052;
            ep.data[i]  /= Z*0.052;
            op.data[i]  /= Z*0.052;
            pp.data[i]  /= Z*0.052;
            ob.data[i]  /= Z*0.052;
            fp.data[i]  /= Z*0.052;
            mww.data[i] /= Z*0.052;

         }

      }

      /*----------------*/
      /* output results */
      /*----------------*/

      if( epfp ){
         fputtr(epfp, &ep);
      }

      if( fpfp ){
         fputtr(fpfp, &fp);
      }

      if( pepfp ){
         fputtr(pepfp, &pep);
      }

      if( popfp ){
         fputtr(popfp, &pop);
      }

      if( ppfp ){
         fputtr(ppfp, &pp);
      }

      if( opfp ){
         fputtr(opfp, &op);
      }

      if( obfp ){
         fputtr(obfp, &ob);
      }

      if( dvfp ){
         fputtr(dvfp, &dv);
      }

      if( nhfp ){
         fputtr(nhfp, &nh);
      }

      if( chfp ){
         fputtr(chfp, &ch);
      }

      if( mwwfp ){
         fputtr(mwwfp, &mww);
      }

   } while (fgettr(vifp, &vi));

   return 0;

}

void interpolate( char   method 
                 ,float* xin 
                 ,float* yin 
                 ,float* xout
                 ,float* yout 
                 ,int    nin
                 ,int    nout
                ){

   float yind[4096][4];

   /*----------------------*/
   /* linear interpolation */
   /*----------------------*/

   if( method == 'l' ){
      intlin(nin,xin,yin,yin[0],yin[nin-1],nout,xout,yout);

   /*-------------------------*/
   /* monotonic interpolation */
   /*-------------------------*/

   }else if( method == 'm' ){
      cmonot(nin,xin,yin,yind);
      intcub(0,nin,xin,yind,nout,xout,yout);

   /*---------------------*/
   /* Akima interpolation */
   /*---------------------*/

   }else if( method == 'a' ){
      cakima(nin,xin,yin,yind);
      intcub(0,nin,xin,yind,nout,xout,yout);

   /*----------------------------*/
   /* cubic spline interpolation */
   /*----------------------------*/

   }else if( method == 's' ){
      csplin(nin,xin,yin,yind);
      intcub(0,nin,xin,yind,nout,xout,yout);

   /*--------------------------*/
   /* unknown method specified */
   /*--------------------------*/

   }else{
      err("%s is an unknown interpolation method!\n",method);
   }

}	
