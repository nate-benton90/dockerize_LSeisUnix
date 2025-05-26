/* cz1fil - filter to produce data file for cz1
 *
 * Credits:
 *	CWP: Jack
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
 *
 * $Author: jkc $
 * $Source: /src/su/czfilters/RCS/cz1fil.c,v $
 * $Revision: 1.9 $ ; $Date: 88/05/21 05:22:36 $
*/

/*********************** self documentation **********************/
char *sdoc = "\
								\n\
CZ1FIL - filter to produce data file for cz1 on stdout		\n\
								\n\
cz1fil <segy_traces >data1_file [optional parameters]		\n\
								\n\
Required parameters:						\n\
	none							\n\
								\n\
Optional parameters: (nyquist calculated internally)		\n\
	f1 = 0.10*(nyquist)	left  low  corner frequency (Hz)\n\
	f2 = 0.15*(nyquist)	left  high corner frequency (Hz)\n\
	f3 = 0.45*(nyquist)	right low  corner frequency (Hz)\n\
	f4 = 0.50*(nyquist)	right high corner frequency (Hz)\n\
	traces = TRACES		name of headerless data	file	\n\
								\n\
";
/*****************************************************************/

/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/czfilters/RCS/cz1fil.c,v $";
static char revid[] =
	"   $Revision: 1.9 $ ; $Date: 88/05/21 05:22:36 $";


#include "../include/cwp.h"
#include "../include/segy.h"

segy tr;


main(argc,argv)
int argc ; char **argv;
{
	float dt;		/* time sample rate		*/
	float f1;		/* low end of frequency band	*/
	float f2;		/* end of upward taper		*/
	float f3;		/* start of downward taper	*/
	float f4;		/* high end of frequency band	*/
	int ntr;		/* number of traces		*/
	float nyq;		/* nyquist frequency		*/
	float tinit;		/* minimum time (0.0)		*/
	float tfin;		/* maximum time			*/
	float tskip;		/* obsolete parameter (0.0)	*/
	int tpts;		/* number of time points	*/
	char *traces;		/* stripped time trace file	*/


	initgetpar(argc, argv); askdoc(1);


	tinit = 0.0;
	tskip = 0.0;

	/* Get information from the first header */
	ntr = gettra(STDIN, &tr, 0);
	dt = tr.dt*0.000001;
	nyq = 0.5/dt;
	tpts = tr.ns;
	tfin = (tpts - 1)*dt;


	/* Default parameters;	User-defined overrides */
	traces = "TRACES";	sgetpar("traces", &traces);
	f1 = 0.10 * nyq;	fgetpar("f1", &f1);
	f2 = 0.15 * nyq;	fgetpar("f2", &f2);
	f3 = 0.45 * nyq;	fgetpar("f3", &f3);
	f4 = 0.49 * nyq;	fgetpar("f4", &f4);


	/* Write out the cz1 data file */
	printf("%s\n", traces);
	printf("%d\n", ntr);
	printf("%g %g\n", tinit, tfin);
	printf("%g\n", tskip);
	printf("%g\n", dt);
	printf("%g %g %g %g\n", f1, f2, f3, f4);
}
