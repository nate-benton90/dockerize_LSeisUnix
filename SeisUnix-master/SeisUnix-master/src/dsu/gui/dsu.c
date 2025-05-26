/*********************** self documentation **********************/
/*
 * dsu.c - Distributed Seismic Unix main program
 *
 * Usage:   dsu
 *
 * NOTE: portions of this program were extracted from the source
	 code for XPVM. 
 *
 * OTHER FUNCTIONS IN THIS FILE:
 
        program_init()
        tcl_init()
        pvm_init()
        window_init()

 *  
 */
/**************** end self doc ********************************/

/*
 * AUTHOR:  Alejandro E. Murillo, Colorado School of Mines, 03/03/96
 *
*/

/*

Copyright statement:
Copyright (c) Colorado School of Mines, 1995,1996
All rights reserved.

*/
#define DO_GLOBALS

#include "dsu.h"

Tk_IdleProc *recv_event_proc();

/* Command Routines */

int initialize_tcl_globals_proc();
int quit_proc();
int halt_proc();
int printGraphInfo();
int mkNodeC();
int delNodeC();
int moveNodeC();
int getParsC();
int saveParsC();
int loadGraphC();
int saveGraphC();
int runSeqC();
int runPvmC();
int do_add_delC();
int setafieldC();
int getafieldC();
int getsrcdirC();
int show_task_help();
int set_active_task();
int strip_label_cmd();


/* MAIN */

int main( argc, argv )
int argc;
char **argv;
{
	char **mung_argv;
	int mung_argc;
	int i;

	/* Mung Argv to Circumvent TK Stupidity */

	mung_argc = argc + 1;

	mung_argv = (char **) malloc( (unsigned) mung_argc
		* sizeof( char * ) );
	memcheck( mung_argv, "Munged Argv List" );

	mung_argv[0] = argv[0];

	mung_argv[1] = copy_str( "-" );

	for ( i=2 ; i <= mung_argc ; i++ )
		mung_argv[i] = argv[ i - 1 ];

	/* Pass to TK */

	Tk_Main( mung_argc, mung_argv, Tcl_AppInit );

	return( 0 );
}

Tcl_AppInit( itp )
Tcl_Interp *itp;
{
	/* Save Interpreter */

	interp = itp;

	/* Get Main Window */

	Top = Tk_MainWindow( interp );

	if ( Top == NULL )
		return( TCL_ERROR );

	Disp = Tk_Display( Top );

/* Initialize Program Constants */

	if ( read_args() == TCL_ERROR )
		return( TCL_ERROR );

	/* Initialize Program Constants & Structs */

	if ( program_init() == TCL_ERROR )
		return( TCL_ERROR );

	/* Initialize Modules */

	if ( tcl_init() == TCL_ERROR )
		return( TCL_ERROR );

	if ( pvm_init() == TCL_ERROR )
		return( TCL_ERROR );

	
	if ( window_init() == TCL_ERROR )
		return( TCL_ERROR );

	/* Set Up User-Specific Startup File */

	tcl_RcFileName = "~/.dsutclrc";

        initialize_hosts();
	
	/* Return to tkMain to Start Tk_MainLoop() */

	return( TCL_OK );
}

read_args() /* Dummy so far */
{
	char tmp[1024];

	char *argv_str;

	return( TCL_OK );
}

usage() /* Dummy so far */
{
	printf( "\nusage:  dsu [ hostfile ] [ -N name ] " );
	printf( "[ -Hv ]\n\n" );

	printf( "where:\n" );
	printf( "------\n" );

	printf( "hostfile   = Alternate DSU Hostfile (~/.dsu_hosts)\n" );

	printf( "-N name\t   = Use \"name\" as Local Network Hostname\n" );

	printf( "-H\t   = Print This Help Information\n" );

	printf( "-v\t   = Verbose Operation\n" );

	printf( "\n" );

	exit( 0 );
}

program_init()
{
	struct passwd *pw;

	char hname[1024];
	char tmp[2048];

	char *getenv();

	char *dsu_dir;
	char *stripped;
	char *uppered;
	char *pvstr;
	char *home;

	int entry_exit;
	int release;
	int version;
	int fmt;
	int i;


	/* Get Hostname */

	if ( HOST_NAME == NULL )
	{
		if ( gethostname( hname, 1024 ) == -1 )
		{
			perror( "Getting Hostname" );

			HOST_NAME = (char *) NULL;
		}

		else
			HOST_NAME = copy_str( hname );
	}

	HOST_ALIAS = host_alias_str( HOST_NAME );

	/* Get Home Directory */

	home = getenv("HOME");

	if ( home == NULL )
	{
		if ( (pw = getpwuid( getuid() )) != NULL )
			home = pw->pw_dir;

		else
			home = "/";

		HOME_DIR = copy_str( home );
	}

	else
		HOME_DIR = home;

	/* Get DSU (AEM) Main Directory */

	dsu_dir = getenv("CWPROOT");

	if ( dsu_dir == NULL )
	{
		sprintf( tmp, "%s", DSU_DEFAULT_DIR );

		if ( tmp[0] != '/' )
			sprintf( tmp, "%s/%s", HOME_DIR, DSU_DEFAULT_DIR );

		DSU_DIR = copy_str( tmp );
	}

	else
	{
		sprintf( tmp, "%s/src/dsu", dsu_dir );
		DSU_DIR = copy_str( tmp );
	}

	/* Check Hostfile */

	if ( HOSTFILE == NULL )
	{
		sprintf( tmp, "%s", DEFAULT_HOSTFILE );

		if ( tmp[0] != '/' )
			sprintf( tmp, "%s/%s", HOME_DIR, DEFAULT_HOSTFILE );
		
		HOSTFILE = copy_str( tmp );
	}


	/* Main graph */

	MAIN_GRAPH = create_graph();

	MAIN_GRAPH->type = GRAPH_PIPE;

/* Collect information about  SU applications and hosts */

	MAIN_INFO = create_dsuinfo();

/* Collect information about SU applications (AEM) */

	sprintf(tmp, "%s/gui/applinfo.list\0", DSU_DIR);
	stripped = copy_str(tmp);
	build_appl_list(stripped);

	return( TCL_OK );
}

tcl_init()
{
	/* Initialize TCL / TK */

	if ( Tcl_Init( interp ) == TCL_ERROR )
		return( TCL_ERROR );

	if ( Tk_Init( interp ) == TCL_ERROR )
		return( TCL_ERROR );

	/* Create TCL Commands for Action Routines */

	/* Main Console Commands */

	Tcl_CreateCommand( interp, "setafieldC", setafieldC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "getafieldC", getafieldC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "getsrcdirC", getsrcdirC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "runSeqC", runSeqC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "runPvmC", runPvmC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "do_add_delC", do_add_delC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "loadGraphC", loadGraphC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "saveGraphC", saveGraphC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "getParsC", getParsC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "saveParsC", saveParsC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "printGraphInfo", printGraphInfo,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "delNodeC", delNodeC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "mkNodeC", mkNodeC,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "show_task_help", show_task_help,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "set_active_task", set_active_task,
		(ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "moveNodeC", moveNodeC,
		(ClientData) NULL, (vfp) NULL );



	/* Menu & Miscellaneous Commands */

	Tcl_CreateCommand( interp, "initialize_tcl_globals",
		initialize_tcl_globals_proc, (ClientData) NULL, (vfp) NULL );

	Tcl_CreateCommand( interp, "strip_label_cmd",
		strip_label_cmd, (ClientData) NULL, (vfp) NULL );


	return( TCL_OK );
}

window_init()
{
	static char fname[1024];

	FILE *fptest;

	/* Set up GLOBAL structs */

	define_tcl_globals(); /* CINTHISFILE */

	/* Create Interface - TCL Script */

	fptest = fopen( "dsumain.tcl", "r" );

	if ( fptest != NULL )
	{
		strcpy( fname, "dsumain.tcl" );

		fclose( fptest );
	}

	else
		sprintf( fname, "%s/gui/dsumain.tcl", DSU_DIR );

	if ( Tcl_EvalFile( interp, fname ) == TCL_ERROR )
		return( TCL_ERROR );


	/* Set Up Event Polling Proc */

/*
	Tk_DoWhenIdle( (Tk_IdleProc *) recv_event_proc, (ClientData) NULL );
*/

	return( TCL_OK );
}

define_tcl_globals()
{
	/* Drawing Constants */

	FRAME_BORDER = MAKE_TCL_GLOBAL( "frame_border", TCL_GLOBAL_INT );

	FRAME_OFFSET = MAKE_TCL_GLOBAL( "FRAME_OFFSET", TCL_GLOBAL_INT );

	BORDER_SPACE = MAKE_TCL_GLOBAL( "border_space", TCL_GLOBAL_INT );

	ROW_HEIGHT = MAKE_TCL_GLOBAL( "row_height", TCL_GLOBAL_INT );

	DEPTH = MAKE_TCL_GLOBAL( "depth", TCL_GLOBAL_INT );

	/* Graph Constants */

	GRAPH_CHEIGHT = MAKE_TCL_GLOBAL( "graph_cheight", TCL_GLOBAL_INT );

	GRAPH_HHEIGHT = MAKE_TCL_GLOBAL( "graph_hheight", TCL_GLOBAL_INT );

	GRAPH_CWIDTH = MAKE_TCL_GLOBAL( "graph_cwidth", TCL_GLOBAL_INT );

	GRAPH_HWIDTH = MAKE_TCL_GLOBAL( "graph_hwidth", TCL_GLOBAL_INT );

	GRAPH_SCROLL = MAKE_TCL_GLOBAL( "graph_scroll", TCL_GLOBAL_INT );

	GRAPH_FLASH = MAKE_TCL_GLOBAL( "graph_flash", TCL_GLOBAL_INT );

	GRAPH_SPACE = MAKE_TCL_GLOBAL( "graph_space", TCL_GLOBAL_INT );

	GRAPH_XVIEW = MAKE_TCL_GLOBAL( "graph_xview", TCL_GLOBAL_INT );

	GRAPH_YVIEW = MAKE_TCL_GLOBAL( "graph_yview", TCL_GLOBAL_INT );

	GRAPH_SIZE = MAKE_TCL_GLOBAL( "graph_size", TCL_GLOBAL_INT );


	/* Colors */

	TASK_ACTIVE_COLOR = MAKE_TCL_GLOBAL( "task_active_color",
		TCL_GLOBAL_CHAR );

	TASK_SETUP_COLOR = MAKE_TCL_GLOBAL( "task_setup_color",
		TCL_GLOBAL_CHAR );

	TASK_NOSETUP_COLOR = MAKE_TCL_GLOBAL( "task_nosetup_color",
		TCL_GLOBAL_CHAR );

	GRAPH_FG_COLOR = MAKE_TCL_GLOBAL( "graph_fg_color", TCL_GLOBAL_CHAR );

	FG_COLOR = MAKE_TCL_GLOBAL( "fg_color", TCL_GLOBAL_CHAR );

	/* Others (AEM) */

	MYRESULT = MAKE_TCL_GLOBAL( "myresult", TCL_GLOBAL_CHAR );


}

/* ARGSUSED */
int initialize_tcl_globals_proc( clientData, itp, argc, argv )
ClientData clientData;
Tcl_Interp *itp;
int argc;
char **argv;
{
	/* Window Names */

	GRAPH_C = GET_TCL_GLOBAL( interp, "GRAPH_C" );
	GRAPH_SBH = GET_TCL_GLOBAL( interp, "GRAPH_SBH" );
	GRAPH_SBV = GET_TCL_GLOBAL( interp, "GRAPH_SBV" );

	MAIN_GRAPH->GRAPH_C = GRAPH_C;

	return( TCL_OK );
}
