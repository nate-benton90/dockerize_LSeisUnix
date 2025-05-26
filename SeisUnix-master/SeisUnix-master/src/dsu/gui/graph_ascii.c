/*********************** self documentation **********************/
/*
 * graph_ascii.c - Subroutines to save/load a graph representing a 
		   sequence of SU applications in a plain ASCII file.
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
#include "graph_ascii.h"

static FILE *mystdin;
  

int load_from_file(GRAPH G, char *filename)
{
  TOKEN 	token;
  int		i; 
  TASK	 	T;
  HASH		HashTable;

  InitHash(&HashTable, 16);

  printf("\nReading graph (new_format) from file:\n\t(%s)\n", filename);

  if ( (mystdin = fopen(filename,"r")) == NULL) {
    fprintf(stderr, "Error opening the file: %s\n", filename);
    return(T_ERROR);
  }

  T = create_task(); /* Temporary task */
  T -> argv = (char **) malloc(MAXARGS*sizeof(char *));
  memcheck( T -> argv, "No more space for argv (loading)");
  T -> argvlabels = (char **) malloc(MAXARGS*sizeof(char *));
  memcheck( T -> argvlabels, "No more space for argvlabels (loading)");

  for (i = 0; i < MAXARGS; i++) {
    T -> argv[i] = (char *)NULL;
    T -> argvlabels[i] = (char *)NULL;
  }

  do {

    CleanTask(T);
    token = GetNode(T, &HashTable);

    switch (token) {

      case T_ERROR: 
	  	  fprintf(stderr, 
		      "\tIncorrect I/O redirection (%s)\n", T -> argv[0]);
	  	  fprintf(stderr, "\tCheck for an extra <, > or >>\n");
		  break;
      case T_BAR: 
		/* printf("\tNode ended with TBAR (%s)\n", T->argv[0]); */

        	if (update_graph(G, T) == -1) token = T_ERROR;
		
		break;

      case T_AMP:  
      case T_SEMI: 
      case T_NL:   
		/* printf("\tNode ended with End of command (%s)\n", T->argv[0]); */

		if (T -> argc > 0 ) {

        	  if ( compare(T -> argv[0], "set") )  	break;
        	  if ( compare(T -> argv[0], "echo") )  break;
        	  if ( compare(T -> argv[0], "exit") )  break;

        	  if (strchr(T -> argv[0], '=') != NULL ) { /* Assignm. */
		    do_assignment(T, &HashTable);
		    break;
		  } 

	    	  if (update_graph(G, T) == -1) {
		    token = T_ERROR;
		    break;
		  }

		  if (token != T_NL) token = T_DONE;

		}
		break;

      case T_EOF:   
		token = T_DONE;
		continue;
    }

  } while ( (token != T_ERROR) && (token != T_DONE) );

  fclose(mystdin);
   
  /* Clean up the temporary task T */

  T -> argc = MAXARGS;
  CleanTask(T); 

  if (T -> argv != (char **)NULL) {
    free(T -> argv);
    T -> argv = (char **) NULL;
  }

  if (T -> argvlabels != (char **)NULL) {
    free(T -> argvlabels);
    T -> argvlabels = (char **) NULL;
  }

  free_task(&T);

  return(token);

}/* End of load_from_file */

TASK get_task_loadid( TASK T, int id )
{
  int  i;
  TASK T1;

  if ( id == -1) return( (TASK) NULL ); /* MUST BE OUT !!! */
  if ( T == (TASK)NULL) return( (TASK) NULL );

  if ( T->loadid == id ) return( T );

  for (i = 0; i < MAX_LINK; i++)
    if ((T1 = get_task_loadid(T->next[i], id)) != (TASK) NULL) return(T1);

  return( (TASK) NULL );

} /* End of get_task_loadid */


/*
        ADD a node to a graph structure at after node parent
        (it might be a new branch)
*/

int add_temp_graph(GRAPH G, TASK T, int fork, int parent)
{

  TASK active;
  int link;

  if ( G -> task_list == (TASK)NULL) { /* Empty list */

    G->task_list = T;

    link = 0;

  } else {

    /* added to be able to insert after any node */

    if ( (active = get_task_loadid (G -> task_list, parent)) == (TASK)NULL)
      active = G -> actv_task;

    link = next_link_avail(active, fork);

    if (active -> next[link] == (TASK)NULL) active -> nbranches++;

    T -> next[0] = active -> next[link];

    T -> parent = active;

    active -> next[link] = T;

    if ( T -> next[0] != (TASK)NULL)
      T -> next[0] -> parent = T;

  }

}

int   update_graph(GRAPH G, TASK T1)
{
  TASK T;
  int i, parent;
  char *name, *thost, *nn, *np;

  /* 
    Name has the form: apl_name@host_name:nn,np
  */

  name = copy_str(T1 -> argv[0]);

  if ( (np = strrchr(name, ',')) != (char *)NULL) *np++ = '\0';
  if ( (nn = strrchr(name, ':')) != (char *)NULL) *nn++ = '\0';
  if ( (thost = strrchr(name, '@')) != (char *)NULL) *thost++ = '\0';

  if ( (T = create_task_nme(name) ) == (TASK)NULL) {
    fprintf(stderr,"\tError adding (%s). Info not found\n", name);
    return(-1);
  }

  parent = -1;
  if (np != (char *)NULL) parent = atoi(np);
  if (nn != (char *)NULL) T -> loadid = atoi(nn);
  if ( (thost != (char *)NULL) )
    if (!compare(thost, "\0")) 
      T -> myhost = copy_str(thost);

  /*
	Load the arguments: use nn as temporary char *
  */

  for (i = 1; i < T1 -> argc; i++) {

    if (T1 -> argv[i][0] == '-' )

	T -> options = copy_str(T1 -> argv[i]);

    else {

        if ( (nn = (char *)index(T1 -> argv[i], '=')) != NULL ) 
	  *nn++ = '\0';
        addArgvT(T, T1 -> argv[i], nn);

    } /* else */

  }

  T -> status = TASK_SETUP;
  
  if (T1 -> srcfd != 0 ) {  /* input redirected here */
    T -> srcfd = -1;
    T -> srcfn = copy_str(T1 -> srcfn);
  }

  if (T1 -> dstfd != 1 ) {  /* output redirected here */
    T -> dstfd = -1;
    T -> dstfn = copy_str(T1 -> dstfn);
  }

  add_temp_graph(G, T, 1, parent); /* Add to TEMP_G graph */

  G -> actv_task = T; /* This is missing in add_task_graph
			 for compatibility with the drawing 
			 routines !!! */

  return(0);

} /* End of update_graph */

void   add_this_task(TASK T, int fork)
{

  TASK next[MAX_LINK];
  int i, link;
  int add_task_graph(GRAPH, TASK, int );

  if (T == (TASK)NULL) return;

  for (i = 0; i < MAX_LINK; i++) {
    next[i] = T -> next[i];
    T -> next[i] = (TASK)NULL;
    T -> nbranches = 0;
    T -> parent = (TASK)NULL;
  }

  link = add_task_graph(MAIN_GRAPH, T, fork);
  draw_graph_task(T, link);
  createParsWinC(T);

  for (i = 0; i < MAX_LINK; i++) {
    add_this_task(next[i], 1);
    set_the_active(T);
  }

}


int   draw_loaded_graph(GRAPH G)
{
  
  add_this_task(G -> task_list, 1);

} /* End draw_loaded_graph() */

/*
	save_task: print the information of a single task
	Used by save_the_graph
*/

int   save_a_task(FILE *fp,  TASK T, int index, int parent)
{
  int i;

  if (T -> myhost != (char *)NULL)
    fprintf(fp, "\n%s@%s:%d,%d ", T->name, T->myhost, index, parent);
  else
    fprintf(fp, "\n%s:%d,%d ", T->name, index, parent);

  if ( T -> srcfn != (char *)NULL) fprintf(fp, " < %s ", T -> srcfn);
  if ( T -> dstfn != (char *)NULL) fprintf(fp, " > %s ", T -> dstfn);

  fprintf(fp, "\\\n");

  for (i = 0; i < (T -> S -> argc); i++)
    if ( (T -> argv[i] != (char *)NULL) && (!compare("void", T -> argv[i]) ) )
      fprintf(fp, "\t%s=\"%s\"\\\n", T->argvlabels[i], T->argv[i]);

  if ( T -> options != (char *)NULL) fprintf(fp, "\t\"%s\"\\\n", T -> options);

  return;
}

int   save_graph(FILE *fp,  TASK T, int *index, int parent)
{
  int i, me;

  if (T == (TASK) NULL) return;

  *index = *index + 1;
  me = *index;

  save_a_task(fp, T, me, parent);

  for ( i = 0; i < MAX_LINK; i++) 
    if (T -> next[i] != (TASK) NULL) {
      fprintf(fp, " |  \n"); /* End of node indicator (|) */
      save_graph(fp, T -> next[i], index, me);
    }

} /* End of save_graph() */

int   print_one_task(FILE *fp,  TASK T)
{
  int i;

  fprintf(fp, "\n%s ", T->name);

  if ( T -> srcfn != (char *)NULL) fprintf(fp, " < %s ", T -> srcfn);
  if ( T -> dstfn != (char *)NULL) fprintf(fp, " > %s ", T -> dstfn);

  fprintf(fp, "\\\n");

  for (i = 0; i < (T -> S -> argc); i++)
    if ( (T -> argv[i] != (char *)NULL) && (!compare("void", T -> argv[i]) ) )
      fprintf(fp, "\t%s=\"%s\"\\\n", T->argvlabels[i], T->argv[i]);

  return;
}

int   print_shell(FILE *fp,  TASK T)
{
  int i;

  if (T == (TASK)NULL) return;

  print_one_task(fp, T);

  if (T -> next[0] != (TASK)NULL) {
    fprintf(fp, " |  \n"); /* Pipe indicator */
    print_shell(fp, T -> next[0]);
  } 

  return;
}

int   asg1(char * argument, HASH * HashTable)
{
  char *s1, *s2, *s3;

  s1 = copy_str(argument);
  s2 = strtok(s1, "=");
  s3 = strtok(NULL, "\0");
  /* printf("\tInserting %s = (%s)\n", s2, s3); */
  HashInsert(HashTable, s2, s3);
  free(s1);
 
} /* End of asg1() */

int   do_assignment(TASK T, HASH *HashTable)
{
  int i;

  for (i = 0 ; i < (T -> argc); i++)
    asg1(T -> argv[i], HashTable);
}

static TOKEN GetToken(char *word, HASH *HashTable)
{
  enum { NEUTRAL, GTGT, INQUOTE, INWORD, COMMENT, MACRO} 
	state = NEUTRAL, prev_state;
  int  c, i;
  char *w, *s1, macro_name[512];
  int  quote_flag = 0;

  w = word;
  while ( (c = fgetc(mystdin)) != EOF) {
    switch(state) {
      case NEUTRAL:
        switch(c) {
   
	  case ';' : return(T_SEMI);
	  case '&' : return(T_AMP);
	  case '|' : return(T_BAR);
	  case '<' : return(T_LT);

	  case '\n' : return(T_NL);

	  case '\\':  fgetc(mystdin); /* special chars not in words ignored */
          case ' ' :
          case '\t' : continue;

	  case '>' : state = GTGT;
		     continue;
	  case '"' : state = INQUOTE;
		     continue;
	  case '$' : state = MACRO;
	  	     prev_state = INWORD;
		     s1 = macro_name;
		     if ( (c = fgetc(mystdin)) != '{' ) 
			ungetc(c, mystdin); /* Allow for ${macro} */
		     continue;
	  case '#' : state = COMMENT;
		     continue;
	  default  : state = INWORD;
	             *w++ = c;
		     continue;
        }
      case MACRO:  	/* Accept cases like: ${pepe=  (no closing '}') */
 	switch(c) {
	  case '|' :  
	  case ';' :
	  case '&' : 
	  case '<' :
          case ' ' :
          case '\t':
	  case '>' :
	  case '\n':
	  case '$':
	  case '/':
	  case '=':
	  case '"':
	  case '}':
		if (c != '}') ungetc(c, mystdin); /* Macro: ${macro} */
		*s1 = '\0'; /* End of macro */ 
		if ((s1 = LookUp(HashTable, macro_name)) == NULL) {
		  s1 = macro_name;
		  *w++ = '$';
		}
		while (*s1 != '\0') *w++ = *s1++;
	  	state = prev_state;
		continue;
	  default : 
		*s1++ = c;
		continue;
	}
      case COMMENT:
        if ( c == EOF) return(T_EOF);
        if ( c == '\n') 
	  state = NEUTRAL;
	continue;
      case GTGT:
        if ( c == '>') return(T_GTGT);
        ungetc(c, mystdin);
        return(T_GT);

      case INQUOTE:
 	switch (c) {
	  case '$' : state = MACRO;
	  	     prev_state = INQUOTE;
		     s1 = macro_name;
		     if ( (c = fgetc(mystdin)) != '{' ) 
			ungetc(c, mystdin); /* Allow for ${macro} */
		     continue;
	  case '\\':  *w++ = fgetc(mystdin);
		      continue;
	  case '"':   *w = '\0';
		      return(T_WORD);
	  default :   *w++ = c;
		      continue;
	}
      case INWORD:
        switch(c) {
	  case '$' : state = MACRO;
	  	     prev_state = INWORD;
		     s1 = macro_name;
		     if ( (c = fgetc(mystdin)) != '{' ) 
			ungetc(c, mystdin); /* Allow for ${macro} */
		     continue;
	  case '\\':  *w = fgetc(mystdin);
		      if (*w != '\n') w++;
		      continue;
	  case '|' :  
	  case ';' :
	  case '&' : 
	  case '<' :
          case ' ' :
          case '\t' :
	  case '>' :
	  case '\n' :
		     if (!quote_flag) {   /* Quote inside a word */
		       ungetc(c, mystdin);
		       *w = '\0';
		       return(T_WORD);
		     } else {
	 	       *w++ = c;
		       continue;
		     }
	  case '"' :
		     quote_flag = ++quote_flag % 2; continue;
	  default : 
	 	     *w++ = c;
		     continue;
	}
      } /* States switch */
    } /* End of WHILE */
    return(T_EOF);
} /* End of GetToken() */	


static TOKEN GetNode(TASK T, HASH *HashTable)
{
  TOKEN  token;
  char   word[1024];

  while(1)
    switch(token = GetToken(word, HashTable)) {
      case T_WORD:
	if (T->argc == MAXARGS) {
	  fprintf(stderr, "\tToo many arguments\n");
	  break;
	}
	T -> argv[T->argc] = copy_str(word);
	T -> argc++;
	continue;
      case T_LT:
    	if (GetToken(word, HashTable) != T_WORD) { /* it may be $1, $2, etc */
	  fprintf(stderr, "\tIllegal input redirection\n");
	  return(T_ERROR);
	}
	fprintf(stderr, "\tInput redirection (%s)\n", word);
        T -> srcfn = copy_str(word);
	T -> srcfd = -1;
	continue;
      case T_GT:
      case T_GTGT:
    	if (GetToken(word, HashTable) != T_WORD) { /* it may be $1, $2, etc */
	  fprintf(stderr, "\tIllegal output redirection\n");
	  return(T_ERROR);
	}
	fprintf(stderr, "\tOutput redirection (%s)\n", word);
	T -> dstfn = copy_str(word);
	T -> dstfd = -1;
	continue;
      case T_NL:
	if (T -> argc == 0) continue;   /* Ignore blank lines */
					/* or EOL after a T_BAR */
      case T_BAR:
      case T_AMP:
      case T_SEMI:
      case T_EOF:
	T -> argv[T -> argc] = (char *)NULL;
	return(token);

    } /* end of switch */

} /* End of GetNode() */

void	InsertArgv(HASH *HashTable, char *argv[], int argc)
{
  char s1[3];
  int  i;
  
  for (i = 1; i < argc; i++) {
   sprintf(s1, "%1d\0", i);
   HashInsert(HashTable, s1, argv[i]);
  }
} /* end of InsertArgv() */

/*
	Read the file
	draw the graph
	Makes the correponding error checking
        argv[1] : filename
	argv[2] : format (0 -- Shell script, 1 -- new format)
*/

int loadGraphC( clientData, itp, argc, argv )
ClientData clientData;
Tcl_Interp *itp;
int argc;
char **argv;
{

  GRAPH G;
  char *fn, msg[1024], tmp[1024];;
  
  fn = copy_str(argv[1]);
  Tcl_Eval( interp, msg );
  G = create_graph();

  if ( load_from_file(G, fn) != T_ERROR ) {
    printf("\tDrawing the graph ... ");
    draw_loaded_graph(G);
    printf(" Ready\n");
    printf("Load succesfully Completed \n\n");
    sprintf( tmp, "Load succesfully completed (\"%s\")", fn);
  } else {
    remove_branch(G -> task_list);
    printf("\nLoad NO COMPLETED (check for errors in the input file) \n\n");
    sprintf( tmp, "Error loading from file \"%s\"", fn);
  }

  sprintf( msg, "setMsg { %s }", tmp );
  Tcl_Eval( interp, msg );

  if ( fn != (char *) NULL) free(fn);
  free_graph(&G);

  return(TCL_OK);
}

int saveGraphC( clientData, itp, argc, argv )
ClientData clientData;
Tcl_Interp *itp;
int argc;
char **argv;
{

  char *fn, msg[1024], tmp[1024];
  FILE *fp;
  time_t  tm1;
  int index;
  
  if (MAIN_GRAPH -> task_list == (TASK)NULL) {
    Tcl_Eval( interp, "setMsg \"The sequence is empty !!!\"" );
    fprintf(stderr, "\n\tSequence is empty !!!\n");
    return(TCL_OK);
  }

  fn = copy_str(argv[1]);

  if ( (fp = fopen(fn,"w")) == NULL) {
    fprintf(stderr, "Error opening the file (for writing): %s\n", fn);
    sprintf( tmp, "Error opening the file (for writing)\"%s\"", fn);
    sprintf( msg, "setMsg { %s }", tmp );
    Tcl_Eval( interp, msg );
    return(TCL_ERROR);
  }

  fprintf(stderr, "\nSaving the current sequence on file:\n\t%s\n", fn);

  time(&tm1);
  fprintf(fp, "# This sequence was written by dsu: %s\n\n", ctime(&tm1));

  index = 0;
  save_graph(fp, MAIN_GRAPH -> task_list, &index, -1);
  
  fprintf(fp,"\n\n exit \n\n");
  
  fclose(fp);

  fprintf(stderr, "Save completed\n\n");

  sprintf( tmp, "Sequence saved in file \"%s\"", fn);
  sprintf( msg, "setMsg { %s }", tmp );
  Tcl_Eval( interp, msg );

  if ( fn != (char *) NULL) free(fn);

  return(TCL_OK);

}
