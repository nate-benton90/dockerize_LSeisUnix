/* percent - find 100 * ratio of arguments
 * Usage: percent x y
 *
 * This triviality is called by the rmaxdiff shell.  Surely, there
 * is a better way, but ...
 *
 * $Author: jkc $
 * $Source: /src/su/shell/RCS/percent.c,v $
 * $Revision: 1.2 $ ; $Date: 88/10/06 15:51:24 $
 */

#include <stdio.h>
#include <math.h>

main(argc, argv)
int argc;
char **argv;
{
	double x, y;

	x = atof(argv[1]);
	y = atof(argv[2]);
	printf("%g\n", y == 0.0 ? 0.0 : 100.0*x/y);
}
