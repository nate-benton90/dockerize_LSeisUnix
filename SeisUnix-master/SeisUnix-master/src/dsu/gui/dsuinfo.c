/*********************** self documentation **********************/
/*
 * dsuinfo.c - Subroutines to handle a linked list whose elements
 *		contain information about SU applications.
 *
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

DSUINFO create_dsuinfo()
{
	DSUINFO tmp;

	tmp = (DSUINFO) malloc( sizeof( struct info_struct ) );
	memcheck( tmp, "info Structure" );

	tmp->appl_list = (DSUAPPL) NULL;
	tmp->host_list = (HOST) NULL;
	tmp->ntasks_running = 0;

	return( tmp );
}

DSUAPPL create_appl()
{
	DSUAPPL tmp;

	tmp = (DSUAPPL) malloc( sizeof( struct dsuappl_struct ) );
	memcheck( tmp, "dsuappl Structure" );

	tmp->name = (char *) NULL;
	tmp->helpFile = (char *) NULL;

	tmp->argc = 0;
	tmp->argv = (char **) NULL;
	tmp->argvlabels = (char **) NULL;

	tmp-> next = (DSUAPPL) NULL;

	return( tmp );
}

int add_to_appl_list(DSUAPPL T)
{

  if (MAIN_INFO->appl_list == (DSUAPPL) NULL)
    MAIN_INFO->appl_list = T;
  else
    MAIN_INFO->last_appl->next = T;

  MAIN_INFO->last_appl  = T;

}

int getArgvLabels(DSUAPPL S)
{
  char *pars[1024]; /* 1024 is the expected max. number of parameters */
  FILE *fp;
  int  i;
  char cmd[1024], *s1;

  if ( S != (DSUAPPL) NULL) {

    /* Open the file containing the parameters names and default
	values (SHOULD check home directory first) */

    sprintf(cmd, "%s/dsupars/%s\0", DSU_DIR, S -> helpFile);
    if ( (fp=fopen(cmd, "r")) == NULL) {
      fprintf(stderr, "Error opening parameter file: %s\n", cmd);
      return(TCL_ERROR);
    } 

    /* Read all the parameters in pars; update S-> argc */

    while ( fgets(cmd, 256, fp) != NULL ) {
      cmd [strlen(cmd) - 1] = '\0';
      pars[S -> argc++] = copy_str(cmd);
    }

    fclose(fp);

    /* Labels go to argvlabels. Default values go to argv */

    if ( S -> argc > 0 ) {
      S -> argvlabels = (char **)malloc(S -> argc*sizeof(char *));
      memcheck( S -> argvlabels, "Space for Arglabels" );
      S -> argv = (char **)malloc(S -> argc*sizeof(char *));
      memcheck( S -> argv, "Space for Argv" );
    }

    for (i = 0; i < S -> argc; i++) {

      s1 = (char *)index(pars[i], '=');

      if (s1 != NULL) {
        *s1++ = '\0';
        S -> argv[i] = copy_str(s1);
      }

      S -> argvlabels[i] = copy_str(pars[i]);
      free(pars[i]);

    }
  }
  return(TCL_OK);
}

                                            


int build_appl_list(char *fn)  /* fn is applinfo.list, for now */
{
  DSUAPPL S;
  FILE *fp;
  char line[256], *s1, *s2;


  if ( (fp = fopen(fn, "r")) == NULL ) {
    fprintf(stderr, "Error opening file (%s)\n", fn);
    return(-1);
  }

  /*
	A line in this file has the form:
	applname:[0-1]:source_code_dir
  */

  while ( fgets(line, 80, fp) != NULL)
    {
      if (line[0] == '#') continue;

      line [strlen(line) - 1] = '\0';

      S = create_appl();
      add_to_appl_list(S);

      if ( (s1 = (char *)strtok(line, ":")) == NULL)  {
	printf("Something wrong with this line: %s\n", line);
	continue;
      }

      S -> helpFile = copy_str(line);

      if ( (s2 = (char *)strchr(S -> helpFile, '.')) != NULL) *s2 = '\0';
      S -> name = copy_str(S -> helpFile);
      if (s2 != NULL) *s2 = '.';

      if ( (s1 = (char *)strtok(NULL, ":")) == NULL) {
	printf("Something wrong with this line: %s\n", line);
	continue;
      }

      S -> hostType = atoi(s1);

      if ( (s1 = (char *)strtok(NULL, ":")) == NULL) {
	printf("Something wrong with this line: %s\n", line);
	continue;
      }

      S -> sourceCode = copy_str(s1);

      getArgvLabels(S);
    }
}


/*
   This one is used to obtain the position of an argument
   in the array argvlabels[]
*/

int getlabelindex (DSUAPPL S, char *argvlabel)
{
  int i;

  for (i = 0; i < S -> argc; i++)
    if ( compare (argvlabel, S -> argvlabels[i]) )
      return(i);

  return(-1);
}

/*
   This one is used to update the value of an argument
   in the array argv[]
*/

int addArgvT (TASK T, char *argvlabel, char *value)
{
  int i;

  if ( value == NULL) {
    fprintf(stderr, "Warning: parameter %s ignored (assigning a null value)\n", 
	  	    argvlabel);
    return(-1);
  }

  for (i = 0; i < T -> S -> argc; i++)

    /* AEM 4/29/96 if ( compare (T -> S -> argvlabels[i], argvlabel) ) { */
    if ( compare (argvlabel, T -> S -> argvlabels[i]) ) {

      if (T -> argv[i] != (char *)NULL) {
	free(T -> argv[i]);
        T -> argv[i] = (char *) NULL;
      } else
	T -> argc++;

      T -> argv[i] = copy_str(value);
    
    }

  return(0);
}


/*
   This one is used to obtain a pointer to the 
   application with name: name
*/

DSUAPPL getThisAplInfo (char *name)
{

  DSUAPPL S;

  S =  MAIN_INFO -> appl_list;

  while (S != (DSUAPPL) NULL) {

    if ( compare (name, S->name) )
      return(S);
    S = S -> next;

  }

  return((DSUAPPL) NULL);
}
