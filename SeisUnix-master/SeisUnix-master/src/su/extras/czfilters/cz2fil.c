/* cz2fil - filter to produce data file for cz2
 *
 * Credits:
 *	CWP: Jack
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
 *
 * $Author: jkc $
 * $Source: /src/su/czfilters/RCS/cz2fil.c,v $
 * $Revision: 1.12 $ ; $Date: 88/05/21 23:40:31 $
*/

/*********************** self documentation **********************/
char *sdoc = "\
								\n\
CZ2FIL - filter to produce data file for cz2 on stdout		\n\
								\n\
cz2fil <segy_traces >data2_file v= dx=  [optional parameters]	\n\
								\n\
Required parameters:						\n\
	v			velocity picks			\n\
	dx			spatial sample rate		\n\
								\n\
Optional parameters:						\n\
	zv			depth picks for the v's		\n\
	bmin			minimum dip picks		\n\
	bmax			maximum dip picks		\n\
	zdip			depth picks for the dips	\n\
	final = INVTRACES	name of inverted trace file	\n\
	outdt = input dt	output sampling rate		\n\
	tmin=.05 * two_way	minimum output time		\n\
	tmax=.70 * two_way	maximum output time		\n\
	trmin = 1		first output trace number	\n\
	trmax = ntr		last output trace number	\n\
	spap = 0		spatial aliasing parameter	\n\
				0 <= spap <=1			\n\
				0 = allow no aliased data	\n\
								\n\
";
/*****************************************************************/

/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/czfilters/RCS/cz2fil.c,v $";
static char revid[] =
	"   $Revision: 1.12 $ ; $Date: 88/05/21 23:40:31 $";


#include "../include/cwp.h"
#include "../include/segy.h"

segy tr;


main(argc,argv)
int argc ; char **argv;
{
	float dt;		/* temporal sample rate		*/
	int tpts;		/* number of points on trace	*/
	float tfin;		/* maximum time on trace	*/
	int ntr;		/* number of traces		*/
	float outdt;		/* output sampling rate		*/
	float tmin;		/* minimum output time		*/
	float tmax;		/* maximum output time		*/
	float dx;		/* spatial sample rate		*/
	int trmin;		/* first output trace number	*/
	int trmax;		/* last output trace number	*/
	float spap;		/* spatial aliasing parameter	*/
	float *v;		/* velocity picks		*/
	int nv;			/* number of v's		*/
	float *zv;		/* depth picks for the v's	*/
	int nzv;		/* number of zv's		*/
	float *bmin;		/* minimum dip picks		*/
	int nbmin;		/* number of bmin's		*/
	float *bmax;		/* maximum dip picks		*/
	int nbmax;		/* number of bmax's		*/
	float *zdip;		/* depth picks for the dips	*/
	int nzdip;		/* number of zdip's		*/
	char *final;		/* inverted headerless traces	*/
	int maxpar;		/* maximum length input vector	*/
	float *ptr;		/* generic allocation pointer	*/
	int i;			/* counter			*/


	initgetpar(argc, argv); askdoc(1);


	/* Get information from the first header */
	ntr = gettra(STDIN, &tr, 0);
	dt = tr.dt*0.000001;
	tpts = tr.ns;
	tfin = (tpts - 1)*dt;

	MUSTFGETPAR("dx", &dx);

	/* Default parameters;	User-defined overrides */
	final = "INVTRACES";	sgetpar("final", &final);
	outdt = dt;		fgetpar("outdt", &outdt);
	tmin = 0.05*tfin;	fgetpar("tmin" , &tmin);
	tmax = 0.70*tfin;	fgetpar("tmax" , &tmax);
	trmin = 1;		igetpar("trmin", &trmin);
	trmax = ntr;		igetpar("trmax", &trmax);
	spap = 0.0;		fgetpar("spap" , &spap);

	/* Allocate space for velocity model and dip model */
	maxpar = maxgetpar();

	ptr   = vector(maxpar);
	nv    = fgetpar("v", ptr);
	v     = vector(nv);
	v     = ptr;

	ptr   = vector(maxpar);
	nzv   = fgetpar("zv", ptr);
	zv    = vector(nzv);
	zv    = ptr;

	ptr   = vector(maxpar);
	nzdip = fgetpar("zdip", ptr);
	zdip  = vector(nzdip);
	zdip  = ptr;

	ptr   = vector(maxpar);
	nbmin = fgetpar("bmin", ptr);
	bmin  = vector(nbmin);
	bmin  = ptr;

	ptr   = vector(maxpar);
	nbmax = fgetpar("bmax", ptr);
	bmax  = vector(nbmax);
	bmax  = ptr;


	/* Consistency checks */
	if (nzv && nzv != nv) {
		err("number of zv's must equal number of v's");
	}
	if (nzdip != nbmin || nzdip != nbmax) {
		err("must have same number of max, min angles as zdips");
	}


	/* Write out the cz2 data file */
	printf("%s\n", final);
	printf("%g\n", dx);
	printf("%d %d\n", trmin, trmax);
	printf("%g %g\n", tmin, tmax);
	printf("%g\n", outdt);
	printf("%g\n", v[0]);
	printf("%d\n", nv - 1);
	for (i = 1; i < nv; i++) {
		printf("%g %g\n", zv[i], v[i]);
	}
	printf("%d\n", nzdip);
	for (i = 0; i < nzdip; i++) {
		printf("%g %g %g\n", zdip[i], bmin[i], bmax[i]);
	}
	printf("%g\n", spap);
}
