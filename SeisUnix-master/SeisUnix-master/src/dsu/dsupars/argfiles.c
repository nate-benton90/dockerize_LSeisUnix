/* Distributed Seismic Unix (dsu): buildpars.c */

#include "argfiles.h"

#define TRUE 1
#define FALSE 0

static FILE *mystdin;
static FILE *mystdout;

int compare( x, y )
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

TOKEN GetThisToken(char *word, TOKEN ThisToken)
{
  TOKEN 	token;

  token = GetToken(word);  /* Get next token */

  while ( (token != ThisToken) &&
          (token != T_SEMI) &&
	  (token != T_EOF)  &&
	  (token != T_NL) )
	    token = GetToken(word);  /* Get next token */

  return(token);
	  
}


int main(char **argv, int argc)
{
  TOKEN 	token;
  char		word[256], ArgName[256], ArgValue[256];


  mystdin = stdin;
  mystdout = stdout;

  do {
  
    switch(token = GetToken(word)) {

      case T_WORD:
	
	if (  compare("getparstring", word) ||
	      compare("getparint", word)    ||
	      compare("getparfloat", word)    ) {


	  if ( GetThisToken(ArgName, T_WORD) != T_WORD )
	    continue; /* Ignore this possible parameter */

	  if ( (GetThisToken(ArgValue, T_EQUAL)) == T_EQUAL )
	    {
	      if ( (GetThisToken(ArgValue, T_WORD)) != T_WORD )
	        strcpy(ArgValue, "void");
	    }
	  else
	      strcpy(ArgValue, "void");

          fprintf(mystdout, "%s=%s\n", ArgName, ArgValue);
	    
        } /* End if */

	break;  	/* T_WORD */

      case T_EOF:   
		/* fprintf(stderr, "\tEOF found \n"); */
		continue;
    }
  } while (token != T_EOF);

  fclose(mystdin);
  fclose(mystdout);

}/* End of main program() */


static TOKEN GetToken(char *word)
{
  enum { NEUTRAL, GTGT, INQUOTE, INWORD, COMMENT, MACRO} 
	state = NEUTRAL, prev_state;

  int  c, i;
  char *w, *s1, macro_name[200];

  w = word;
  state = NEUTRAL;
  while ( (c = fgetc(mystdin)) != EOF) {
    switch(state) {
      case NEUTRAL:
        switch(c) {
   
	  case '=' : return(T_EQUAL);
	  case ';' : return(T_SEMI);
	  case '\n' : return(T_NL);

	  case '\\':  fgetc(mystdin); /* special characters not in words ignored */
          case ' ' :
          case '\t' : continue;

	  default  : state = INWORD;
		     if (!isalnum(c)) return (T_DELIM);
	             *w++ = c;
		     continue;
        }

      case INWORD:
        switch(c) {
	  default : 
		     if ( !isalnum(c) && (c != '_') ) {
			 if ( c == '=') ungetc(c, mystdin);
			 *w = '\0';
			 return(T_WORD);
		     }
	 	     *w++ = c;
		     continue;
	}
      } /* States switch */
    } /* End of WHILE */
    return(T_EOF);
} /* End of GetToken() */	
