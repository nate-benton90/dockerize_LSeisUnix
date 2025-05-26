/*********************** self documentation **********************/
/*
 * graph_exec.c - Subroutines to execute a sequence of SU applications
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


#include "dsu.h"
#include "dsulib.h"

Tk_IdleProc *recv_event_proc();

/*
Tk_IdleProc *process_event_proc();
Tk_IdleProc *delete_host();
Tk_IdleProc *add_host();
*/

struct timeval  tm1, tm2;

int run_local()
{
  FILE *fp;

  /* Sequential execution of the graph */

  fprintf(stderr, "\nLocal execution: BEGIN\n");

  if ( MAIN_GRAPH -> task_list != (TASK)NULL) {

    if ( (fp = fopen("/tmp/DsuTmpExe", "w")) == NULL) {
      fprintf(stderr, "Can not open temp file to execute\n");
      return(TCL_ERROR);
    }

    /* Build the file or the string */

    print_shell(fp, MAIN_GRAPH -> task_list);

    fprintf(fp, "\n\n");

    fclose(fp);

    /* Call system to execute the sequence */

    system("sh /tmp/DsuTmpExe & \0");

  }

  fprintf(stderr, "\nLocal execution initiated on background\n");


}

int run_dst()
{
  TASK T;
  int	index = 0, root_tid;

  fprintf(stderr, "\nDistributed execution: BEGIN\n");

  gettimeofday(&tm1, (struct timezone*)0);

  CleanLog(); /* clean the directory ~/.dsulog */

  T = MAIN_GRAPH -> task_list;

  /*
      Starts appl's 
      DsuTids will contain appl's tid 
  */

/*
	RESET PREVIOUS EXECUTION NEEDED ?? at least xmovies, etc
*/
  initialize_hosts(); /* Update the list of host available */
  pvm_reset();

  if ( (root_tid = StartPvmTasks(T, &index)) < 0) {

    fprintf(stderr, "Error starting the applications. resetting PVM\n");
    fprintf(stderr, "\nRun distributed ENDED with errors\n");
    pvm_reset();

    return(TCL_ERROR);
  }

  /*
	Initiate the monitoring task
  */

  if (MAIN_INFO->ntasks_running)
    Tk_DoWhenIdle( (Tk_IdleProc *) recv_event_proc, (ClientData) NULL );

  /*
	Go ahead signal to node 0
  */

  SendInt(&index, 1, root_tid, MsgParent);
  
  fprintf(stderr, "\nDistribution completed. Monitoring task started\n\n");

  return(TCL_OK);

} /* End of run_dst */

/*
        The last parameter will be a null pointer.
        This is used when starting the applic. with PVM.
        Actually, argv[0] should be the application name
*/

char ** BuildArgv(TASK T)
{
  char **argv;
  char argt[1024];
  int  i, j;

  argv = (char **) malloc( (T->argc + 1 + 1 + 1)*(sizeof(char *)) );
  memcheck(argv, "Space for argv (on BuildArgv)");

 /* 1 + 1 + 1 indicates: name, options and null parameter */

  for (i = 0; i < (T -> argc + 1 + 1 + 1); i++) argv[i] = (char *) NULL;

  j = 0;
  argv[j] = copy_str(T -> name);

  for (i = 0; i < T -> S -> argc; i++) {
    if ( T -> argv[i] != (char *)NULL) {
      sprintf(argt, "%s=%s\0", T -> argvlabels[i], T->argv[i]);
      argv[++j] = copy_str(argt);
    }
  }

  /* Added 12/03/95 (AEM) */

  if (T -> options != (char *)NULL) {
    /* argv[++j] = (char *) malloc( (strlen(T -> options)) + 8); */
    sprintf(argt, "\"%s\"\0", T -> options);
    argv[++j] = copy_str(argt);
    /*
    printf("\t\tOptions: %s\n", argv[j]);
    */
  }

  return(argv);

}

int FreeArgv(char ** argv, int argc)
{
  
  int i;

  if (argv == (char **) NULL) return;

  for (i = 0; i < argc; i++)
    if (argv[i] != (char *) NULL) free(argv[i]);

  if (argv != (char **) NULL) free(argv);

}
	
int host_in_pvm(HOST H, char * name)
{

  if (H == (HOST) NULL) return(0);

  if (compare(H -> name, name) ) 
    if (H -> status == HOST_ON) 
      return(1);
    else
      return(0);

  return(host_in_pvm( H -> next, name));

}


/*
	This subroutine starts all the sub-branches and
	returns:
		its PVM tid
		-2 if there was error starting an application
		-3 If any application had a setup error
*/

int StartPvmTasks(TASK T, int *index)
{
  int i, info, myindex;
  char **ThisArgv;
  char *thehost;
  int  hostflag;

  if (T == (TASK)NULL ) return (-1);

  myindex = *index;
  *index += 1;

/*
  Set collect output off
  pvm_catchout(NULL);
*/

  /*
	Starts the sub-branches
  */

  for ( i = 0; i < MAX_LINK; i++) {

    T -> branch_tid[i] = StartPvmTasks(T -> next[i], index);

    if (T -> branch_tid[i] < -1) return (T -> branch_tid[i]);

  }

  /*
	Starts this application
  */

  fprintf(stderr,"\tStarting application # %d (%s)", myindex, T -> name);

  ThisArgv = BuildArgv(T);

  thehost = (char *)NULL;

  hostflag = 0;

  if ( T -> S -> hostType == LOCALHST ) {

        thehost = copy_str(HOST_NAME);
	hostflag = 1;

	/* 
        fprintf(stderr, "\t\tStarted locally in host (%s)\n", HOSTNAME);
        fflush(stderr);
	*/

  } else if ( T -> myhost != (char *)NULL) {
  
     if ( host_in_pvm(MAIN_INFO -> host_list, T -> myhost) )
     {
        thehost = copy_str(T -> myhost);
	hostflag = 1;
        fprintf(stderr, "\n\t\tStarting %s on host (%s)\n", 
			T -> name, T -> myhost);
     }
     else  
     {
        fprintf(stderr,
	  "\n\t\tRequested host (%s) NO in PVM. Using default\n", 
		   T -> myhost);
     }
  }

  info = pvm_spawn(T -> name, ThisArgv, hostflag,
            thehost, 1, &T -> mytid);

  if (thehost != (char *)NULL) free(thehost);
    
  if (info < 0) {  /* Notify the error and return the error */

    fprintf(stderr,"\tError Starting application %s\n", T -> name);
    FreeArgv(ThisArgv, T-> argc + 3);
    return(-2);

  }

  /*
	Increment the counter of tasks running
	Graphic applications don't count (suxmovie, etc)
  */

  if ( T -> S -> hostType != LOCALHST )
    MAIN_INFO->ntasks_running++;

  /*
	Let the  children know their father tid.
	Let this node know its children tids
  */

  for (i = 0; i < MAX_LINK; i++) 
    if (T -> next[i] != (TASK)NULL)
      SendInt(&T -> mytid, 1, T -> branch_tid[i], MsgParent);

  SendInt(T -> branch_tid, MAX_LINK, T -> mytid, MsgBranchs);

  /* Check if I/O must be redirected in this task */

  SendInt(&T -> srcfd, 1, T -> mytid, MsgWho);
  if ( T -> srcfd != 0) {
      SendStr(T -> srcfn, T -> mytid, MsgWho);
      fprintf(stderr, 
	 	"\n\t\tStandard input redirected in this application \n");
  }

  SendInt(&T -> dstfd, 1, T -> mytid, MsgWho);
  if ( T -> dstfd != 1) {
      SendStr(T -> dstfn, T -> mytid, MsgWho);
      fprintf(stderr, 
	 	"\n\t\tStandard output redirected in this application\n");
  }

  /*
	Wait for Ok from the application
  */

  if ( compare("Error", T -> logFile = RecvStr(T -> mytid, MsgFile)) )
	return(-3);

/*
    fprintf(stderr, "\tlogFile: (%s) \n", T -> logFile);
    fflush(stderr);
*/

  FreeArgv(ThisArgv, T-> argc + 3);

  fprintf(stderr, "\tReady \n");

  return(T -> mytid);
}

int runSeqC( clientData, itp, argc, argv )
ClientData clientData;
Tcl_Interp *itp;
int argc;
char **argv;
{

  if (MAIN_GRAPH -> task_list == (TASK)NULL) {
    Tcl_Eval( interp, "setMsg \"The sequence is empty !!!\"" );
    return(TCL_OK);
  }

  if ( compare(argv[1], "local") )
    run_local();
  else
    run_dst();

  return(TCL_OK);

}

/*
	Locate a task by its tid
*/

TASK get_task_tid( TASK T, int index )
{
  TASK T1;
  int  i;

  if ( T == (TASK)NULL) return( (TASK) NULL );

  if ( T->mytid == index ) return( T );

  for (i = 0; i < MAX_LINK; i++)
    if ( (T1 = get_task_tid(T -> next[i], index)) != (TASK) NULL) return(T1);

  return( (TASK) NULL );

} /* End of get_task_tid */


/* 
  This procedure will keep checking from messages from
	the applications.
*/

Tk_IdleProc *recv_event_proc( clientData )
ClientData clientData;
{
	TASK T;

	int msgtag;
	int len;
	int mid;
	int num;
	int tid;
	int src;
	int cc;

	/* Check for PVM Events */

  	if ( (mid = pvm_nrecv( -1, 99 )) > 0 ||
       	     (mid = pvm_nrecv( -1, 100 )) > 0 )
	{
	      PVMCKERR( pvm_bufinfo( mid, &len, &msgtag, &src ),
			"Error Checking Message Buffer", exit( -1 ) );

	      cc = pvm_upkint( &num, 1, 1 );

	      if ( cc < 0 )
		  pvm_perror( "Error Unpacking task msg Notify" );
		
	      if ( (T = get_task_tid(MAIN_GRAPH->task_list, src)) != (TASK)NULL )
	      {
		if ( msgtag == 99 ) /* A error has been detected */
		{
		  fprintf( stderr, "Task %s has abnormally ended\n",
					T -> name);
		  fprintf(stderr,"See log files. The execution is aborted\n");

		  pvm_reset();
		
		}

		else /* msgtag == 100 means succesfull completion */
		{
			fprintf( stderr, "\tTask <%s> has successfully ended\n",
					T -> name);
			if ( T -> S -> hostType != LOCALHST )
			    MAIN_INFO->ntasks_running--;
		}

		T -> mytid = -1;
	      }
     	      else 
   	      {
		fprintf (stderr, "Warning: Something weird is going on with PVM!!!\n");
		fprintf (stderr, "         Perhaps there are TOO MANY processes running \n");
		fprintf (stderr, "         In this machine.  kill  some sux* ASAP!!!\n");
		fflush(stderr);
	      } 

	} /* End of IF */

	if (MAIN_INFO->ntasks_running) {
	  Tk_Sleep(10);
	  Tk_DoWhenIdle( (Tk_IdleProc *) recv_event_proc, (ClientData) NULL );
	} else {
	  gettimeofday(&tm2, (struct timezone*)0);
	  fprintf(stderr, "\n\tDistributed execution completed\n");
	  fprintf(stderr, "\tTotal elapsed time <%d> Sec. \n\n", 
		tm2.tv_sec - tm1.tv_sec);
	}

}
