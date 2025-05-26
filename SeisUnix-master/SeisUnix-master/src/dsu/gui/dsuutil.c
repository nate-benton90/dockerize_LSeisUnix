/*********************** self documentation **********************/
/*
 * dsuutil.c - tcl/tk and general purpose utility programs implemented 
		in C
 *
 * NOTE: These subroutines were extracted from the source code
	 of XPVM.
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

TCL_GLOBAL create_tcl_global()
{
	TCL_GLOBAL tmp;

	int i;

	tmp = (TCL_GLOBAL) malloc( sizeof( struct tcl_glob_struct ) );
	memcheck( tmp, "TCL Global Structure" );

	tmp->name = (char *) NULL;

	tmp->type = -1;

	tmp->char_value = (char *) NULL;

	tmp->int_value = -1;

	return( tmp );
}

free_tcl_global( ptr )
TCL_GLOBAL *ptr;
{
	TCL_GLOBAL G;

	G = *ptr;

	G->name = (char *) NULL;

	G->type = -1;

	G->char_value = (char *) NULL;

	G->int_value = -1;

	free( G );

	*ptr = (TCL_GLOBAL) NULL;
}

char *date_str()
{
	char tmp[255];

	char *result;

	time_t t;

	time( &t );

	sprintf( tmp, "%s", ctime( &t ) );

	result = tmp;

	while ( *result != '\n' && *result != '\0' )
		result++;
	
	*result = '\0';

	result = copy_str( tmp );

	return( result );
}

char *host_alias_str( name )
char *name;
{
	char *ptr;
	char *tmp;

	char c;

	if ( name == NULL )
		return( (char *) NULL );

	ptr = name;

	while ( *ptr != '\0' && *ptr != '.' )
		ptr++;

	if ( *ptr == '.' )
	{
		c = *ptr;

		*ptr = '\0';

		tmp = copy_str( name );

		*ptr = c;
	}

	else
		tmp = copy_str( name );
	
	return( tmp );
}

char *copy_str( str )
char *str;
{
	char *tmp;

	tmp = (char *) malloc( (unsigned) (strlen(str) + 1)
		* sizeof(char) );
	memcheck(tmp,"Copy String");

	strcpy( tmp, str );

	return( tmp );
}

filecheck( fp, name )
FILE *fp;
char *name;
{
	char msg[1024];
	char tmp[1024];

	if ( fp == NULL )
	{
		sprintf( tmp, "Error Opening File \"%s\"", name );

		sprintf( msg, "setMsg { %s }", tmp );

		Tcl_Eval( interp, msg );

		fprintf( stderr, "\n%s\n\n", tmp );

		return( FALSE );
	}

	return( TRUE );
}

memcheck( ptr, name )
char *ptr;
char *name;
{
	if ( ( ptr == NULL ) || ( ptr == (char *)NULL) )
	{
		fprintf( stderr, "\nError Allocating Memory for \"%s\"\n\n",
			name );
		fflush(stderr);

		error_exit();
	}
}

compare( x, y )
char *x, *y;
{
	while ( *x != '\0' )
	{
		if ( *x != *y )
			return( FALSE );

		x++; y++;
	}

	return( TRUE );
}

GOBJ create_gobj()
{
        GOBJ tmp;

        int i;

        tmp = (GOBJ) malloc( sizeof( struct gobj_struct ) );
        memcheck( tmp, "Graphical Object Structure" );

        tmp->id = -1;

        for ( i=0 ; i < MAX_GOBJ_COORDS ; i++ )
                tmp->coords[i] = -1;

        tmp->color = (char *) NULL;

        return( tmp );
}

free_gobj( ptr )
GOBJ *ptr;
{
        GOBJ G;

        int i;

        G = *ptr;

        G->id = -1;

        for ( i=0 ; i < MAX_GOBJ_COORDS ; i++ )
                G->coords[i] = -1;

        G->color = (char *) NULL;

        free( G );

        *ptr = (GOBJ) NULL;
}

error_exit()
{
	int cc;

	if ( MYTID > 0 )
	{
		cc = pvm_getinst( "dsu", MYTID );

		if ( cc >= 0 )
		{
			cc = pvm_lvgroup( "dsu" );

			if ( cc < 0 )
			{
				printf( "\nError Leaving DSU Group, cc=%d\n\n",
					cc );
			}
		}
	}

	exit( -1 );
}
