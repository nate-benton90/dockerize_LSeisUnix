/*********************** self documentation **********************/
/*
 * dsupvm.c - subroutines to handle PVM from SU
 *
 * NOTE: Some of these subroutines were extracted from the source code
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
#include "pvm3.h"

Tk_IdleProc *delete_host();
Tk_IdleProc *add_host();

dump_host_info(HOST H)
{

  if (H == (HOST) NULL) return;

  if (H -> status == HOST_ON ) 
  {
	fprintf(stderr, "\nHOST -> name: (%s). Speed = %d\n", 
                H -> name, H -> speed);
/*
	fprintf(stderr, "\tHOST -> refname: (%s)\n", H -> refname);
	fprintf(stderr, "\tHOST -> alias: (%s)\n", H -> alias);
	fprintf(stderr, "\tHOST -> speed: (%d)\n", H -> speed);
*/
  } else
	fprintf(stderr, "\nHOST -> name: (%s) NO in the virtual machine\n", 
		H -> name);

  dump_host_info( H -> next);

}

pvm_init()
{
	char tmp[1024];

	char *av[4];
	int ac;

	int inum;
	int se;
	int cc;
	int i;

	/* Start PVM (if necessary) */

	av[0] = (char *) NULL;

	ac = 0;

	if ( HOSTFILE != NULL )
		av[ac++] = copy_str( HOSTFILE );

	pvm_setopt( PvmAutoErr, 0 );
	cc = pvm_start_pvmd( ac, av, FALSE );

	if ( cc < 0 )
	{
		if ( cc == PvmDupHost )
		{
			printf( "Connecting to PVMD already running... " );
			fflush( stdout );

		}

		else
		{
			pvm_perror( "Can't Start PVM" );

			exit( -1 );
		}
	}

	else
	{
		printf( "New PVMD started... " );
		fflush( stdout );

	}

	printf( " Ready \n");

	MYTID = pvm_mytid();  /* Set the tid of the GUI */

	read_hostfile();

	return( TCL_OK );
}

int pvm_reset()
{
  struct pvmtaskinfo *tip;

  int ntask;
  int num;
  int tid;
  int i;
  int mid, len, msgtag, src;


  Tcl_Eval( interp, "setMsg \"Resetting PVM...\"" );

  PVMCKERR( pvm_tasks( 0, &ntask, &tip ),
        "Error Obtaining Task Information", exit( -1 ) );

  for ( i=0 ; i < ntask ; i++ )
    {
      tid = tip[i].ti_tid;

      if ( tid != 0 && tid != MYTID )
                   pvm_kill( tid );
    }

  MAIN_INFO -> ntasks_running = 0;

  /* Check for pending error messages */

  while ( (mid = pvm_nrecv( -1, 99 )) > 0 ) {

    PVMCKERR( pvm_bufinfo( mid, &len, &msgtag, &src ),
             "Error Checking Message Buffer (reset)", exit( -1 ) );

    if ( pvm_upkint( &num, 1, 1 )  < 0 )
      pvm_perror( "Error Unpacking task msg Notify" );
    else
      fprintf(stderr, "Warning message (%d) from application (%d) received\n",
			num, src);

  }
  Tcl_Eval( interp, "setMsg \"PVM Reset Done.\"" );

  return( TCL_OK );

}

/* ARGSUSED */
int runPvmC( clientData, itp, argc, argv )
ClientData clientData;
Tcl_Interp *itp;
int argc;
char **argv;
{

  if ( compare(argv[1], "reset") )
    pvm_reset();
  else
    if ( compare(argv[1], "halt") )
      pvm_halt();
/*
    else
      if ( compare(argv[1], "add") )
        fprintf(stderr, "PVM add selected\n");
      else
        if ( compare(argv[1], "delete") )
          fprintf(stderr, "PVM delete selected\n");
*/

  return( TCL_OK );

}

/* Added 04/11/96 */

char *host_name( H, full )
HOST H;
int full;
{
        char str[1024];

        char *tmp;
	
	tmp = H->alias;

        if ( full && strcmp( H->name, tmp ) )
                sprintf( str, "%s (%s)", H->name, tmp );

        else
                sprintf( str, "%s", tmp );

        return( copy_str( str ) );
}

HOST create_host()
{
        HOST tmp;

        tmp = (HOST) malloc( sizeof( struct host_struct ) );
        memcheck( tmp, "Host Structure" );

        tmp->name = (char *) NULL;
        tmp->alias = (char *) NULL;
        tmp->refname = (char *) NULL;

	tmp->pvmd_tid = -1;
        tmp->status = HOST_OFF;
        tmp->speed = -1;

	tmp->next = (HOST) NULL;

        return( tmp );

}

read_hostfile()
{
	HOST H;

	FILE *fp;

	char globline[1024];
	char hostline[1024];
	char reftmp[1024];
	char msg[1024];
	char tmp[1024];

	int done;
	int i, j;

	/* Initialize Global Options Line */

	strcpy( globline, "" );

	/* Get list of hosts */

	fp = fopen( HOSTFILE, "r" );

	if ( fp != NULL )
	{
	  done = 0;

	  do {
		i = 0;

		while ( (hostline[i] = getc( fp )) != (char) EOF
			&& hostline[i] != '\n' )
		{
			i++;
		}

		if ( hostline[i] != '\n' )
			done++;
		else
		{
			hostline[i] = '\0';

			if ( hostline[0] != '#' && strcmp( hostline, "" ) )
			{
				if ( hostline[0] == '*' )
					sprintf( globline, " %s", hostline + 1 );

				else
				{
					if ( hostline[0] == '&' )
					{
						sprintf( reftmp, "%s%s",
							hostline + 1, globline );
	
					}
		
					else
					{
						sprintf( reftmp, "%s%s",
							hostline, globline );
	
					}
		
					sscanf( reftmp, "%s", tmp );
	
					if ( MAIN_INFO->host_list == (HOST)NULL )
					{
					  H = MAIN_INFO->host_list = create_host();
		
					}
		
					else
					{
					  H->next = create_host();
		
					  H = H->next;
					}
		
					H->name = copy_str( tmp );
		
					H->refname = copy_str( reftmp );
		
					H->alias = host_alias_str( H->name );

					H->status = HOST_OFF;
	
					/* dump_host_info(H); */
				}
			}
		}
	  } while ( !done );

	  /* dump__host_info(MAIN_INFO->host_list); */
	  fclose( fp );
	}

	else
	{
		sprintf( tmp, "Error Opening Hostfile \"%s\"", HOSTFILE );

		sprintf( msg, "setMsg { %s }", tmp );

		Tcl_Eval( interp, msg );

	}
}

initialize_hosts()
{
	HOST H;

	HOST last;

	struct pvmhostinfo *HP;

	struct pvmhostinfo *hostp;

	char cmd[1024];
	char tmp[1024];
	char tmp2[1024];

	int found;
	int narch;
	int nhost;
	int host;
	int i, j;

	/* Get Existing Configuration */

	PVMCKERR( pvm_config( &nhost, &narch, &hostp ),
		"Error Checking Existing Configuration", exit( -1 ) );

	/* Set up Host Delete Notify's */

	/* AEM
	for ( i=0 ; i < nhost ; i++ )
		pvm_notify( PvmHostDelete, 100, 1, &(hostp[i].hi_tid) );
	*/

	/* Check Existing List Against Host File */

	for ( i=0 ; i < nhost ; i++ )
	{
		HP = &(hostp[i]);

		/* Update Host List */

		H = MAIN_INFO->host_list;

		last = (HOST) NULL;

		found = 0;

		while ( H != (HOST)NULL && !found )
		{
			if ( HOST_COMPARE( H, HP->hi_name ) )
			{
				found++;
			}
			else
			{
				last = H;

				H = H->next;
			}
		}

		/* Host Not Found, Add to Hosts List */

		if ( !found )

		{

			if ( last != (HOST)NULL )
				H = last->next = create_host();
			
			else
			{
				H = MAIN_INFO->host_list = create_host();

				/* NHOSTS = 0; */
			}

			H->name = copy_str( HP->hi_name );

			H->alias = host_alias_str( H->name );

			H->refname = copy_str( HP->hi_name );

		}

		H->pvmd_tid = HP->hi_tid;

		H->speed = HP->hi_speed;

		H->status = HOST_ON;

	}
	/* dump_host_info(MAIN_INFO->host_list); */
}

do_add_delC( clientData, itp, argc, argv )
ClientData clientData;
Tcl_Interp *itp;
int argc;
char **argv;
{

  HOST H, last;
  char tmp[4096];
  char *refname;

  int found;

  refname = copy_str( argv[1] );

  sscanf( refname, "%s", tmp );

  /* Look for Host in Host List (D-Oh!) */

  H = MAIN_INFO->host_list;

  last = (HOST) NULL;

  found = 0;

  while ( H != (HOST)NULL && !found )
  {
       if ( HOST_COMPARE( H, tmp ) )

          found++;

       else
       {
          last = H;

          H = H->next;
       }
  }
                                                
  if ( !found  )
  {
    if ( compare( argv[2] , "delete" ) )
    {  
      sprintf( tmp, "setMsg \"Host %s no in the virtual machine\"", refname);
      Tcl_Eval( interp, tmp );
      return(TCL_OK);
    }

    if ( last != (HOST)NULL )
    {
      H = last->next = create_host();
    }
    else
    {
      H = MAIN_INFO->host_list = create_host();

    }

    H->name = copy_str( tmp );

    H->alias = host_alias_str( H->name );

    H->refname = copy_str( H->name );

  }
  else
  {
    if ( compare( argv[2] , "add" ) && (H -> status == HOST_ON) ) 
    {  
      sprintf( tmp, "setMsg \"Host %s already in the virtual machine\"", refname);
      Tcl_Eval( interp, tmp );
      return(TCL_OK);
    }
    if ( compare( argv[2] , "delete" ) && (H -> status == HOST_OFF) ) 
    {  
      sprintf(tmp,"setMsg \"Host %s already Out of the virtual machine\"",refname);
      Tcl_Eval( interp, tmp );
      return(TCL_OK);
    }
  }
  
  

  if ( compare( argv[2] , "add" ) )
    Tk_DoWhenIdle( (Tk_IdleProc *) add_host, (ClientData) H );
  else
    Tk_DoWhenIdle( (Tk_IdleProc *) delete_host, (ClientData) H );

   return( TCL_OK );
}

Tk_IdleProc *add_host( clientData )
ClientData clientData;
{
	HOST H;

	struct pvmhostinfo *HP;

	struct pvmhostinfo *hostp;

	char result[4096];

	int found;
	int narch;
	int nhost;
	int infos;
	int cc;
	int i;

	H = (HOST) clientData;

	sprintf( result, "setMsg \"Adding Host %s...\"",
		host_name( H, TRUE ) );

	Tcl_Eval( interp, result );

	/* printf( "Host Add\n" ); */

	cc = pvm_addhosts( &(H->refname), 1, &infos );

	if ( cc < 0 || infos < 0 )
	{
		/* pvm_perror( H->refname ); */

		sprintf( result, "setMsg \"Error Adding Host %s.\"",
			host_name( H, TRUE ) );

		Tcl_Eval( interp, result );

	}

	else
	{
		/* Update Configuration Info */

		PVMCKERR( pvm_config( &nhost, &narch, &hostp ),
			"Error Checking Configuration", exit( -1 ) );

		found = 0;

		for ( i=0 ; i < nhost && !found ; i++ )
		{
			HP = &(hostp[i]);

			if ( HOST_COMPARE( H, HP->hi_name ) )
			{
				if ( strlen( HP->hi_name ) > strlen( H->name ) )
				{
					free( H->name );

					H->name = copy_str( HP->hi_name );
				}

				H->pvmd_tid = HP->hi_tid;

				H->speed = HP->hi_speed;

				found++;
			}
		}

		/* Mark Host as Added */

		H->status = HOST_ON;

		sprintf( result, "setMsg \"Host %s successfully added.\"",
			host_name( H, TRUE ) );

		Tcl_Eval( interp, result );
	}
}

Tk_IdleProc *delete_host( clientData )
ClientData clientData;
{
	HOST H;

	struct pvmhostinfo *HP;

	struct pvmhostinfo *hostp;

	char result[4096];
	char tmp[1024];

	char *tmpptr;

	char *strcat();

	int found;
	int infos;
	int narch;
	int nhost;
	int cc;
	int i;

	H = (HOST) clientData;

	if ( LOCALHOST( H->name ) )
	{
		sprintf( result,"setMsg \"The local host cannot be deleted, %s.\"",
			host_name( H, TRUE ) );

		Tcl_Eval( interp, result );

	}

	else
	{
		sprintf( result, "setMsg \"Deleting Host %s...\"",
			host_name( H, TRUE ) );
	
		Tcl_Eval( interp, result );
	
		/* printf( "Host Delete\n" ); */

		sscanf( H->refname, "%s", tmp );

		tmpptr = tmp;

		cc = pvm_delhosts( &tmpptr, 1, &infos );

		if ( cc < 0 || infos < 0 )
		{
			/* pvm_perror( H->name ); */
	
			sprintf( result, "setMsg \"Error Deleting Host %s.\"",
				host_name( H, TRUE ) );

			Tcl_Eval( interp, result );

		}
	
		else
		{
			/* Mark Host as Deleted */

			H->status = HOST_OFF;

			sprintf( result, "setMsg \"Host %s Deleted.\"",
				host_name( H, TRUE ) );
	
			Tcl_Eval( interp, result );
		}
	}
}
