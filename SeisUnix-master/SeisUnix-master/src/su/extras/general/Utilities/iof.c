/* iof.c - write on stderr from Fortran subroutines
 *
 * Caveat:
 *	These routines give a primitive write facility for debugging
 *	Fortran subroutines of C mains.  They are not meant for serious
 *	communications.
 *
 * Returns:
 *	void
 *
 * Synopsis:
 *	void wrtstr_(s)
 *	char *s;
 *
 *	void wrtflt_(f)
 *	float *f;
 *
 *	void wrtint_(d)
 *	int *d;
 *
 *	void die_()
 *
 * Example:
 *	call wrtstr('x:')
 *	call wrtflt(x)
 *	call die()
 *
 * Credits:
 *	Jack
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
 *
 * $Author: jkc $
 * $Source: /src/general/Util/RCS/iof.c,v $
 * $Revision: 1.5 $ ; $Date: 87/11/26 11:09:43 $
*/

/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/general/Util/RCS/iof.c,v $";
static char revid[] =
	"   $Revision: 1.5 $ ; $Date: 87/11/26 11:09:43 $";


#include <stdio.h>

void wrtstr_(s)
char *s;
{
	fprintf(stderr, "%s\n", s);
}

void wrtflt_(f)
float *f;
{
	fprintf(stderr, "%f\n", *f);
}

void wrtint_(d)
int *d;
{
	fprintf(stderr, "%d\n", *d);
}

void die_()
{
	exit(1);
}
