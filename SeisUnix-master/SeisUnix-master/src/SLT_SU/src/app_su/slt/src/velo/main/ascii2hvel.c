/* velocity card format conversion */

#include "velo.h"
#include "usu.h"


char    *sdoc =
"ASCII2HVEL - convert ascii velocity cards to HVEL cards                        \n"
"\n"
"ascii2hvel [parameters] <ascii-cards >hvel-cards                               \n"
"\n"
"Required parameters:                                                   \n"
"none                                                                   \n"
"\n"
"Optional parameters:                                                   \n"
"mintrace=-99999        minimum trace number of velocity cards to output \n"
"maxtrace=99999         maximum trace number of velocity cards to output \n"
"minline=-99999         minimum line number of velocity cards to output \n"
"maxline=99999          maximum line number of velocity cards to output \n"
"linepos=1              column position of line number \n"
"tracepos=2             column position of trace number \n"
"timepos=5              column position of time (ms) \n"
"velopos=6              column position of velocity \n"
"\n"
"Notes:                                                                 \n"
"1. This program converts ASCII velocity (10 column per row) cards \n"
"   to disco HANDVEL (cdplbl) format    \n"
"\n"
"AUTHOR:                Zhiming Li,       ,     7/28/1999               \n";

main(int argc, char **argv)
{
   char     cbuf[1024];
   float    velo, time, trace, line;
   float    fbuf[10];
   FILE    *infp = stdin, *outfp = stdout;
   float   *times, *vrms;
   int      mintrace, maxtrace, minline, maxline;
   int      n1 = 8192;
   int      iline, itrace, iline0, itrace0, cdplbl;
   int      nvt, nxin, i;
   int      linepos, tracepos, timepos, velopos;

   /* get parameters */
   initargs(argc, argv);
   askdoc(1);


   if (!getparint("linepos", &linepos))
      linepos = 1;
   if (!getparint("tracepos", &tracepos))
      tracepos = 2;
   if (!getparint("timepos", &timepos))
      timepos = 5;
   if (!getparint("velopos", &velopos))
      velopos = 6;
   linepos -= 1;
   tracepos -= 1;
   timepos -= 1;
   velopos -= 1;

   if (!getparint("mintrace", &mintrace))
      mintrace = -99999;
   if (!getparint("maxtrace", &maxtrace))
      maxtrace = 99999;
   if (!getparint("minline", &minline))
      minline = -99999;
   if (!getparint("maxline", &maxline))
      maxline = 99999;

/* memory allocation */
   times = (float *) malloc(n1 * sizeof(int));
   vrms = (float *) malloc(n1 * sizeof(int));

   nxin = 0;
   nvt = 0;

/* read input velf file */
   iline0 = -99999;
   itrace0 = -99999;
   while( !feof(infp) ){

      fgets(cbuf, sizeof(cbuf), stdin);
      sscanf(cbuf, "%f%f%f%f%f%f%f%f%f%f",
             &fbuf[0], &fbuf[1], &fbuf[2], &fbuf[3], &fbuf[4]
             , &fbuf[5], &fbuf[6], &fbuf[7], &fbuf[8], &fbuf[9]);

      line = fbuf[linepos];
      trace = fbuf[tracepos];
      time = fbuf[timepos];
      velo = fbuf[velopos];

      iline = line;
      itrace = trace;

      if (nvt > 0 && (iline != iline0 || itrace != itrace0)) {
         cdplbl = iline0 * 10000 + itrace0;
         printhvel(cdplbl, nvt, times, vrms, outfp);
         fprintf(stderr,
                 "VELF2HVEL at cdplbl=%d line=%d trace=%d nvt=%d\n",
                 cdplbl, iline0, itrace0, nvt);
         nvt = 0;
         nxin = nxin + 1;
      }

      iline0 = iline;
      itrace0 = itrace;
      if (iline >= minline && iline <= maxline && itrace >= mintrace
          && itrace <= maxtrace) {
         times[nvt] = time;
         vrms[nvt] = velo;
         nvt = nvt + 1;
      }

   }

   if (nvt > 0) {
      nxin = nxin + 1;
      cdplbl = iline0 * 10000 + itrace0;
      printhvel(cdplbl, nvt, times, vrms, outfp);

      fprintf(stderr,
              "VELF2HVEL at cdplbl=%d line=%d trace=%d nvt=%d\n",
              cdplbl, iline0, itrace0, nvt);
   }

   fprintf(stderr, "\n");
   fprintf(stderr, "ASCII2HVEL conversion done for %d locations \n",
           nxin);
   free(times);
   free(vrms);
   free(cbuf);
}
