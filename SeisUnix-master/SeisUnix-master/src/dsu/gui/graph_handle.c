/*********************** self documentation **********************/
/*
 * graph_handle.c - Subroutines to handle the graph structure
 *
 * NOTE: Some  subroutines were extracted from the source code
	 of XPVM.
 * See:  XPVM (hhtp://www.netlib.edu/...)
 *
 */
/**************** end self doc ********************************/

/*
 *AUTHOR:  Alejandro E. Murillo, Colorado School of Mines, 03/03/96
 *
*/

#include "dsu.h"

/* 
	GRAPH type stuff
*/

GRAPH create_graph()
{
	GRAPH tmp;

	int i;


	tmp = (GRAPH) malloc( sizeof( struct graph_struct ) );
	memcheck( tmp, "Graph Structure" );

	tmp->type = -1;
	tmp->GRAPH_C = (char *) NULL;
	tmp->task_list = (TASK) NULL;
	tmp->actv_task = (TASK) NULL;
	tmp->currpos = (POSITION) NULL;

	return( tmp );
}

free_graph( ptr )
GRAPH *ptr;
{
	GRAPH N;

	int i;

	N = *ptr;

	N->type = -1;

	N->GRAPH_C = (char *) NULL;

	N->task_list = (TASK) NULL;
	N->currpos   = (POSITION) NULL;

	free( *ptr );

	*ptr = (GRAPH) NULL;
}

/*
	TASK STUFF
*/

TASK create_task()
{
	TASK tmp;

	int i;

	tmp = (TASK) malloc( sizeof( struct task_struct ) );
	memcheck( tmp, "Task Structure" );

	tmp->name     = (char *) NULL;
	tmp->myhost   = (char *) NULL;
	tmp->options   = (char *) NULL;
	tmp->helpFile = (char *) NULL;
	tmp->logFile  = (char *) NULL;
	tmp->loadid   = -1;
	tmp->mytid    = -1;
	tmp->in_pvm   = -1;
	for ( i=0 ; i < MAX_LINK ; i++ )
		tmp->branch_tid[i] = -1;


	tmp->argv 	= (char **) NULL;
	tmp->argvlabels = (char **) NULL;
	tmp->argc 	= 0;
	tmp->srcfd 	= 0;
	tmp->dstfd 	= 1;
	tmp->srcfn 	= (char *)NULL;
	tmp->dstfn 	= (char *)NULL;

	/* Drawing Fields */

	tmp->index  = -1;
	tmp->status = TASK_NOSETUP;

	tmp->color   = (char *) NULL;
	tmp->icon    = (GOBJ) NULL;

	for ( i=0 ; i < MAX_LINK ; i++ )
		tmp->links[i] = (GRAPHLINK) NULL;

	/* Link Fields */

	tmp->nbranches  = 0;
	tmp->angle     = 0;

	for ( i=0 ; i < MAX_LINK ; i++ )
		tmp->next[i] = (TASK) NULL;

	tmp->parent   = (TASK) NULL;

	return( tmp );
}


TASK create_task_nme(char *name)
{
  DSUAPPL S;
  TASK  T;
  int   i;

  /* Get this application (name) information */

  if ( (S = getThisAplInfo(name)) == (DSUAPPL)NULL) {
    fprintf(stderr, "\tThere is no INFO for this application: %s\n", name);
    return((TASK)NULL);
  }

  /* Update this task info using DSUAPPL info */

  T = create_task();
  T -> S = S;
  T -> name = copy_str(name);
  T -> helpFile = copy_str(S -> helpFile); 
  T -> argvlabels = S -> argvlabels; 

  if ( S -> argc > 0 ) {

    T -> argv = (char **) malloc(S -> argc * sizeof(char *));
    memcheck(T -> argv, "Space for argv");

    /* Initialize T -> argc according to the default values */

    for (i = 0; i < S -> argc; i++)
        T -> argv[i] = (char *)NULL;

  /* WORK ON  default value */

  }


  return( T );
}

free_task( TASK *ptr)
{
        TASK T;
	int  i;

        T = *ptr;

        if ( T->name != (char *)NULL )
                free( T->name );

        if ( T->logFile != (char *)NULL )
                free( T->logFile );

        if ( T->helpFile != (char *)NULL )
                free( T->helpFile );

        if ( T->myhost != (char *)NULL )
                free( T->myhost );

        if ( T->argv != (char **)NULL ) {
    	  for (i = 0; i < T -> S -> argc; i++)
            if (T -> argv[i] != (char *)NULL) free (T -> argv[i] );
          free(T->argv); 
	}

        T->status = -1;
   
	free( *ptr);
        *ptr = (TASK) NULL;
}

int next_link_avail(TASK T, int fork)
{
  int i;

  if (fork == 0) return (0);

  for (i = 0; i < MAX_LINK; i++)
    if (T -> next[i] == (TASK)NULL) return(i);

  return(0); /* All the links are used up */

} /* End of next_link_avail() */


/*
	ADD to the graph structure
	(it might be a new branch)
*/

int add_task_graph(GRAPH G, TASK T, int fork)
{

  TASK active;
  int link;

/*
  printf("\tAdding (fork = %d) %s to the graph ... ", fork, T -> name);
*/

  if ( G -> task_list == (TASK)NULL) { /* Empty list */

    G->task_list = T;

    link = 0;

  } else {

    active = G -> actv_task;

    link = next_link_avail(active, fork);

    if (active -> next[link] == (TASK)NULL) active -> nbranches++;

    T -> next[0] = active -> next[link];

    T -> parent = active;

    active -> next[link] = T;

    if ( T -> next[0] != (TASK)NULL)
      T -> next[0] -> parent = T;

  }

/*
  printf(" Ready \n");
*/

  return(link);

} /* End of add_task_graph() */

/*
	Debugging option
*/

int   dump_branch_info(FILE *fp,  TASK T)
{

  int i;

  fprintf(fp, "\nTASK_NAME: (%s)\n", T->name);
  fprintf(fp, "Help_file: (%s)\n", T->helpFile);
  if ( T->logFile != (char *)NULL) fprintf(fp, "Log_file: (%s)\n", T->logFile);

  fflush(stdout);

  if (T -> parent != (TASK)NULL)
    fprintf(fp, "\tPARENT:%s\n", T -> parent -> name);

/*
  fprintf(fp, "\tNumber of branches= %d\n", T -> nbranches);
*/
  for (i = 0; i < MAX_LINK; i++)
    if (T -> next[i] != (TASK)NULL)
      fprintf(fp, "\tNEXT[%d]: %s\n", i, T -> next[i] -> name);

  if (T -> srcfn != (char *)NULL)
    fprintf(fp, "\tSTDINP:%s\n", T -> srcfn);
  if (T -> dstfn != (char *)NULL)
    fprintf(fp, "\tSTDOUT: %s\n", T -> dstfn);
  if (T -> options != (char *)NULL)
    fprintf(fp, "\tOPTIONS: %s\n", T -> options);
  if (T -> myhost != (char *)NULL)
    fprintf(fp, "\tMYHOST: %s\n", T -> myhost);

  fprintf(fp, "\tNumber of arguments = %d\n", T -> argc);
  for (i = 0; i < T -> S -> argc; i++)
    if ( (T -> argv[i] != (char *)NULL) )
      fprintf(fp, "\t\t%s=%s\\\n", T->argvlabels[i], T->argv[i]);

/*
  fprintf(fp, "\tArguments window = %s\n", T -> parWin);
  fprintf(fp, "\tBitmap id = %d\n", T -> icon -> id);
  fprintf(fp, "\tLabel id  = %d\n", T -> nme -> id);
  if (T -> links[0] != (GRAPHLINK) NULL)
    fprintf(fp, "\tLink id   = %d\n", T -> links[0] -> link -> id);
*/

   
  for (i = 0; i < MAX_LINK; i++)
    if (T -> next[i] != (TASK) NULL)
      dump_branch_info(fp, T -> next[i]);

  return;
}

int CleanTask(TASK T)
{
  int i;

  if ( T != (TASK)NULL) {

    for (i = 0; i < T -> argc; i++)  {

      if (T -> argv[i] != (char *)NULL) {
        free(T -> argv[i]);
	T -> argv[i] = (char *)NULL;
      }

      if (T -> argvlabels[i] != (char *)NULL) {
        free(T -> argvlabels[i]);
	T -> argvlabels[i] = (char *)NULL;
      }

    }

    if (T -> helpFile != (char *)NULL) {
	 free(T -> helpFile);
	 T -> helpFile = (char *)NULL;
    }
    T -> argc = 0;

    if (T -> srcfn != (char *)NULL)  {
	free(T -> srcfn);
	T -> srcfn = (char *)NULL;
    }
    T -> srcfd = 0;

    if (T -> dstfn != (char *)NULL)  {
	free(T -> dstfn);
	T -> dstfn = (char *)NULL;
    }
    T -> dstfd = 1;
    for ( i = 0; i < MAX_LINK; i++)
      T -> next[i] = (TASK)NULL;

  }

} /* End of CleanTask() */

int DestroyTask(TASK T)
{
  int i;

  CleanTask(T);

  if (T -> argv != (char **)NULL) {
     free(T -> argv);
     T -> argv = (char **)NULL;
  }

  free_task(&T);

  return(0);

}

int remove_branch (TASK T)
{
  int i;

  if ( T == (TASK)NULL) return;

  for (i = 0; i < MAX_LINK; i++)
    remove_branch(T -> next[i]);

  DestroyTask(T);

  return;

} /* End remove_branch() */
