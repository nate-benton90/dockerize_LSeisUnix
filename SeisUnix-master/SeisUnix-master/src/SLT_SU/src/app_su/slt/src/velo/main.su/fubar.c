/* LMKTZCONV program */
#include "velo.h"
#include "usgrid.h"
#include "par.h"

char    *sdoc =
      "LMKTZCONV - Landmark time-depth convertion program			\n"
      "\n"
      "lmktzconv <landmark.in.file vgrid= [parameters] >landmark.out.file	\n"
      "\n"
      "Required parameters:							\n"
      "landmark.in.file=  name of Landmark input file (time or depth)		\n"
      "landmark.out.file= name of Landmark output file (depth or time)	\n"
      "vgrid=             name of velocity grid 				\n"
      "Optional parameters:							\n"
      "xlpos=2            column position of landmark crossline number	\n"
      "slpos=1            column position of landmark line number 		\n"
      "tzpos=5            column position of landmark time (in ms) or depth 	\n"
      "maxp=250000        maximum number of rows in the landmark pick file	\n"
      "vgtype=1           velocity grid type (1=interval 0=time-average) 	\n"
      "torz=0             type of input landmark picks 			\n"
      "                   (0=time   output will be depth			\n"
      "                    1=depth  output will be time (ms) )	\n"
      "vtorz=0            velocity in time or depth (0=time 1=depth)			\n"
      "o1=                minimum time in ms of velocity grid 		\n"
      "d1=                time sampling interval in ms of velocity grid	\n"
      "ocdp2=             minimum crossline number of velocity grid 		\n"
      "dcdp2=             crossline number increment of velocity grid		\n"
      "oline3=            minimum line number of velocity grid		\n"
      "dline3=            line number increment of velocity grid 		\n"
      "\n"
      "                   The above six parameters default to grid header	\n"
      "                   of input vgrid; use gridheader to print or update	\n"
      "                   the input velocity grid if needed; when supplied	\n"
      "                   they will overwrite the values in the velocity grid	\n"
      "NOTES:						 			\n"
      "\n"

      "AUTHOR:		Zhiming Li,       ,	11/24/94   		\n";

void     bilint_(int *n1, int *nx, int *ny, float *x0, float *y0,
		 float *dx, float *dy, float *x, float *y, float *vs,

		 float *v);

void     lin1dn_(float *xin, float *yin, int *nin, float *xo, float *yo,
		 int *nout, int *indx, float *dydx0, float *dydxn);


main(int argc, char **argv)
{
   FILE    *infp = stdin, *outfp = stdout, *vfp;
   string   vgrid;

   int      maxp = 250000, xlpos = 2, slpos = 1, tzpos = 5, torz =
	 0, vtorz = 0;
   float    ocdp2, dcdp2, oline3, dline3, o1, d1;
   int      vgtype = 1;
   float    xl, sl;
   usghed   usgh;
   int      n1, n2, n3, i1;

   int      ierr;
   float   *vs, *tv, *zv, *v;
   float   *fbuf;
   char    *cbuf;
   int      one = 1, indx, nc, nf;
   float    tmp;
   float    to, zo, dtdzn, dtdz0;
   float    dzdt0, dzdtn;

   /* initialization */
   initargs(argc, argv);
   askdoc(1);

   /* get input parameters */
   if (!getparstring("vgrid", &vgrid))
      err("vgrid missing");
   vfp = efopen(vgrid, "r");
   ierr = fgetusghdr(vfp, &usgh);
   if (ierr != 0)
      err("error get grid header ");


   if (!getparfloat("o1", &o1))
      o1 = usgh.o1;
   if (!getparfloat("d1", &d1))
      d1 = usgh.d1;
   if (!getparfloat("ocdp2", &ocdp2))
      ocdp2 = usgh.ocdp2;
   if (!getparfloat("dcdp2", &dcdp2))
      dcdp2 = usgh.dcdp2;
   if (!getparfloat("oline3", &oline3))
      oline3 = usgh.oline3;
   if (!getparfloat("dline3", &dline3))
      dline3 = usgh.dline3;
   if (dcdp2 == 0)
      err("dcdp2 equals 0");
   if (dline3 == 0)
      err("dline3 equals 0");

   if (!getparint("vgtype", &vgtype))
      vgtype = 1;

   if (!getparint("xlpos", &xlpos))
      xlpos = 2;
   xlpos -= 1;
   if (!getparint("slpos", &slpos))
      slpos = 1;
   slpos -= 1;
   if (!getparint("tzpos", &tzpos))
      tzpos = 5;
   tzpos -= 1;
   if (!getparint("maxp", &maxp))
      maxp = 250000;
   if (!getparint("torz", &torz))
      torz = 0;
   if (!getparint("vtorz", &vtorz))
      vtorz = 0;
   o1 = o1;
   d1 = d1;

   n1 = usgh.n1;
   n2 = usgh.n2;
   n3 = usgh.n3;
   nc = 200;
   nf = 10;

   /* memory allocations */
   vs = (float *) emalloc(n1 * n2 * n3 * sizeof(float));
   v = (float *) emalloc(n1 * sizeof(float));
   tv = (float *) emalloc(n1 * sizeof(float));
   zv = (float *) emalloc(n1 * sizeof(float));
   fbuf = (float *) malloc(nf * sizeof(float));
   cbuf = (char *) emalloc(nc * sizeof(char));

   for (i1 = 0; i1 < n1; i1++)
      tv[i1] = o1 + i1 * d1;
   for (i1 = 0; i1 < n1; i1++)
      zv[i1] = o1 + i1 * d1;
   efseek(vfp, 0, 0);
   efread(vs, sizeof(float), n1 * n2 * n3, vfp);

   fgets(cbuf, nc, infp);
   do {
      sscanf(cbuf, "%f %f %f %f %f",
	     &fbuf[0], &fbuf[1], &fbuf[2], &fbuf[3], &fbuf[4]);
      xl = fbuf[xlpos];
      sl = fbuf[slpos];

      bilint_(&n1, &n2, &n3, &ocdp2, &oline3, &dcdp2, &dline3, &xl, &sl,
	      vs, v);

      if (torz == 0) {
	 to = fbuf[tzpos];
	 if (vgtype == 1) {
	    if (vtorz == 0) {
	       zv[0] = o1 * v[0] * 0.0005;
	       for (i1 = 1; i1 < n1; i1++)
		  zv[i1] =
			zv[i1 - 1] + (v[i1 - 1] + v[i1]) * d1 * 0.00025;
	    } else if (vtorz == 1) {
	       for (i1 = 0; i1 < n1; i1++)
		  zv[i1] = o1 + i1 * d1;
	       tv[0] = o1 / v[0] * 2000.;
	       for (i1 = 1; i1 < n1; i1++)
		  tv[i1] =
			tv[i1 - 1] + d1 / (v[i1 - 1] + v[i1]) * 4000.;
	    }
	 } else {
	    for (i1 = 0; i1 < n1; i1++)
	       zv[i1] = (o1 + i1 * d1) * v[i1] * 0.0005;
	 }
	 dzdt0 = (zv[1] - zv[0]) / (tv[1] - tv[0]);
	 dzdtn = (zv[n1 - 1] - zv[n1 - 2]) / (tv[n1 - 1] - tv[n1 - 2]);
	 lin1dn_(tv, zv, &n1, &to, &zo, &one, &indx, &dzdt0, &dzdtn);
	 fbuf[tzpos] = zo;
      } else {
	 zo = fbuf[tzpos];
	 if (vgtype == 1) {
	    if (vtorz == 1) {
	       tv[0] = o1 / v[0] * 2000.;
	       for (i1 = 1; i1 < n1; i1++)
		  tv[i1] =
			tv[i1 - 1] + d1 / (v[i1 - 1] + v[i1]) * 4000.;
	    } else if (vtorz == 0) {
	       for (i1 = 0; i1 < n1; i1++)
		  tv[i1] = o1 + d1 * i1;
	       zv[0] = o1 * v[0] * 0.0005;
	       for (i1 = 1; i1 < n1; i1++)
		  zv[i1] =
			zv[i1 - 1] + (v[i1 - 1] + v[i1]) * d1 * 0.00025;
	    }
	 } else {
	    for (i1 = 0; i1 < n1; i1++)
	       tv[i1] = o1 + d1 * i1;
	    for (i1 = 0; i1 < n1; i1++)
	       zv[i1] = (o1 + i1 * d1) * v[i1] * 0.0005;
	 }
	 dtdz0 = (tv[1] - tv[0]) / (zv[1] - zv[0]);
	 dtdzn = (tv[n1 - 1] - tv[n1 - 2]) / (zv[n1 - 1] - zv[n1 - 2]);
	 lin1dn_(zv, tv, &n1, &zo, &to, &one, &indx, &dtdz0, &dtdzn);
	 fbuf[tzpos] = to;
      }


      fprintf(outfp,
	      "                    %10.2f%10.2f%12.2f%12.2f%12.4f   \n",
	      fbuf[0], fbuf[1], fbuf[2], fbuf[3], fbuf[4]);
      bzero(cbuf, nc);
   } while (fgets(cbuf, nc, infp));

   free(cbuf);
   free(fbuf);
   free(vs);
   free(v);
   free(tv);
   free(zv);

   exit(0);
}
