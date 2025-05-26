/* DSU Version */

#define DSU_VERSION "1.1.0 alpha"

/* Define Default Location of DSU Main Directory */

#define DSU_DEFAULT_DIR "/usr/local/cwp/src/dsu"

/* Define Default Location of Hostfile */

#define DEFAULT_HOSTFILE ".dsu_hosts"
#define HOST_OFF 0
#define HOST_ON  1

#ifndef PI
#define PI 3.14
#endif

/* Miscelaneous 9/19 */
#define LOCALHST 	2
#define DISTRHST 	4

/* Graph View Layout Types */
#define GRAPH_PIPE      0
#define GRAPH_TREE      1

/* Graph Link ID Constants */
#define MAIN_LINK     0
/*
#define LINK_NORTH    0
#define LINK_EAST     1
#define LINK_SOUTH    2
#define LINK_WEST     3
*/

/* See common.h when changing this !!! */
#define MAX_LINK      5  

/* Task Status Constants */
#define TASK_NOSETUP    0
#define TASK_SETUP   1
#define TASK_IGNORE  2

/* Graph Layout Constants */
#define ARRANGE_TOP   0
#define ARRANGE_BOT   1

/* True / False Constants */
#define FALSE    0
#define TRUE     1

/* Number of Unix Signals */
#define NUM_SIGNALS    31

/* Max Number of Coordinates for GOBJs */
#define MAX_GOBJ_COORDS    4

/* Handy Byte References */
#define NBYTES_1K       1024
#define NBYTES_1M    1048576

/* GOBJ Coordinate Identifiers */

#define X_COORD( _gobj )    ( (_gobj)->coords[0] )
#define Y_COORD( _gobj )    ( (_gobj)->coords[1] )

#define X1_COORD( _gobj )    ( (_gobj)->coords[0] )
#define Y1_COORD( _gobj )    ( (_gobj)->coords[1] )
#define X2_COORD( _gobj )    ( (_gobj)->coords[2] )
#define Y2_COORD( _gobj )    ( (_gobj)->coords[3] )

#define CREATE_GRAPH_LINK( _itp, _c, _gobj, _x1, _y1, _x2, _y2, _clr ) \
{\
        (_gobj) = create_gobj(); \
\
        X1_COORD( _gobj ) = (_x1); \
        Y1_COORD( _gobj ) = (_y1); \
\
        X2_COORD( _gobj ) = (_x2); \
        Y2_COORD( _gobj ) = (_y2); \
\
        (_gobj)->color = _clr; \
\
        sprintf( TMP_CMD, \
        "%s create line %d %d %d %d -capstyle round -fill %s -width 2", \
                _c, _x1, _y1, _x2, _y2, _clr ); \
\
        Tcl_Eval( _itp, TMP_CMD ); \
\
        (_gobj)->id = atoi( (_itp)->result ); \
\
        sprintf( TMP_CMD, \
        "%s lower %d", \
                _c, (_gobj)->id ); \
\
        Tcl_Eval( _itp, TMP_CMD ); \
\
}\
\

#define SET_COORDS( _itp, _canvas, _gobj ) \
{ \
        sprintf( TMP_CMD, "%s coords %d %d %d %d %d", \
                (_canvas), (_gobj)->id, \
                X1_COORD( _gobj ), Y1_COORD( _gobj ),  \
                X2_COORD( _gobj ), Y2_COORD( _gobj ) ); \
\
        Tcl_Eval( _itp, TMP_CMD ); \
} \
\

/* Delete Canvas Object Macro */

#define DELETE_GOBJ( _itp, _canvas, _gobj ) \
{ \
        sprintf( TMP_CMD, "%s delete %d", (_canvas), (_gobj)->id ); \
\
        Tcl_Eval( _itp, TMP_CMD ); \
\
        free_gobj( &(_gobj) ); \
} \
\


/* TCL Globals Constants & Macros */

#define TCL_GLOBAL_CHAR   0
#define TCL_GLOBAL_INT    1

#define MAKE_TCL_GLOBAL( _name, _type ) \
	( TMP_GLOBAL = create_tcl_global(), \
		TMP_GLOBAL->name = _name, \
		TMP_GLOBAL->type = _type, \
		TMP_GLOBAL ) \


#define GET_TCL_GLOBAL( _itp, _name ) \
	Tcl_GetVar( _itp, _name, TCL_GLOBAL_ONLY )

#define SET_TCL_GLOBAL( _itp, _name, _str ) \
	Tcl_SetVar( _itp, _name, _str, TCL_GLOBAL_ONLY )

#define REFRESH_GLOBAL( _glob ) \
{ \
	(_glob)->char_value = GET_TCL_GLOBAL( interp, (_glob)->name ); \
\
	if ( (_glob)->type == TCL_GLOBAL_INT ) \
		(_glob)->int_value = atoi( (_glob)->char_value ); \
} \
\

#define CHAR_GLOBVAL( _glob )    ( (_glob)->char_value )

#define INT_GLOBVAL( _glob )    ( (_glob)->int_value )

/* Create Canvas Rectangle Object Macro */

#define CREATE_RECT( _itp, _c, _gobj, _x1, _y1, _x2, _y2, _rc, _oc ) \
{ \
	(_gobj) = create_gobj(); \
\
	X1_COORD( _gobj ) = (_x1); \
	Y1_COORD( _gobj ) = (_y1); \
\
	X2_COORD( _gobj ) = (_x2); \
	Y2_COORD( _gobj ) = (_y2); \
\
	(_gobj)->color = _rc; \
\
	sprintf( TMP_CMD, \
	"%s create rectangle %d %d %d %d -fill \"%s\" -outline \"%s\" -tags node", \
		_c, _x1, _y1, _x2, _y2, _rc, _oc ); \
\
	Tcl_Eval( _itp, TMP_CMD ); \
\
	(_gobj)->id = atoi( (_itp)->result ); \
} \
\

/* Set Task Box Outline Color & Width */

#define SET_TASK_COLOR( _itp, _c, _task, _color, _width ) \
{ \
	sprintf( TMP_CMD, "%s itemconfigure %d -fill %s -width %d", \
		_c, (_task)->icon->id, _color, _width ); \
\
	Tcl_Eval( _itp, TMP_CMD ); \
} \
\

/* Create Graph View Link Line Macro */

/* Host Name Comparison Macro */

#define HOST_COMPARE( _host, _name ) \
\
	 ( !strcmp( (_host)->name, _name ) \
		 || !strcmp( (_host)->alias, _name ) \
		 || ( !strcmp( (_host)->name, (_host)->alias ) \
			 && compare( (_host)->alias, _name ) ) ) \
\

/* Hostname Check Macro */
#define LOCALHOST( _name ) \
\
        ( HOST_NAME != NULL && HOST_ALIAS != NULL \
                && ( !strcmp( HOST_NAME, _name ) \
                        || !strcmp( HOST_ALIAS, _name ) \
                        || ( !strcmp( HOST_NAME, HOST_ALIAS ) \
                                && compare( HOST_ALIAS, _name ) ) ) ) \
\


#define PVMCKERR( _cmd, _str, _handle ) \
{ \
        TMP_CC = _cmd; \
\
        if ( TMP_CC < 0 ) \
        { \
                pvm_perror( _str ); \
\
                _handle; \
        } \
} \
\

/* Group Task Check Macro */
#define GROUPTASK( _name ) \
        ( compare( "pvmg", _name ) || compare( "pvmcg", _name ) )

