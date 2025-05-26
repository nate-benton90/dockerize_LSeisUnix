/*********************** self documentation **********************/
/*
 * commands.c - tcl/tk commands implemented in C
 *
 * NOTE: These subroutines were extracted from the source code of XPVM.
 * See:  XPVM (hhtp://www.netlib.edu/...)
 *
 */
/**************** end self doc ********************************/

/*
 * Original AUTHOR:  J. A. Khol PVM team
 * CSM AUTHOR:  Alejandro E. Murillo, Colorado School of Mines, 03/03/96
 *
*/

#include "dsu.h"

/* ARGSUSED */
int strip_label_cmd( clientData, itp, argc, argv )
ClientData clientData;
Tcl_Interp *itp;
int argc;
char **argv;
{
	char tmp[1024];

	char *result;
	char *ptr;
	char *str;

	int len;

	if ( argc != 2 )
	{
		Tcl_SetResult( itp, "usage: strip_label <str>", TCL_STATIC );

		return( TCL_ERROR );
	}

	str = argv[1];

	ptr = tmp;

	while ( *str != '\0' )
	{
		if ( *str == '-' || *str == ' ' || *str == '.' )
			*ptr++ = '_';

		else
			*ptr++ = *str;

		str++;
	}

	*ptr = '\0';

	result = copy_str( tmp );

	Tcl_SetResult( itp, result, TCL_DYNAMIC );

	return( TCL_OK );
}
