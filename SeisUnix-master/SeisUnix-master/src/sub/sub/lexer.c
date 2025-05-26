#include "sub.h"
#include "support.h"
#include "x_tab.h"

FILE    *fin;                   /* input file pointer */

char*	SourceText = NULL;

static int	avail		= 0;
static int	nextchar	= 0;
static int	chunksize	= 1024;

void addc(int c)
{
    if(SourceText == NULL) {
	avail = chunksize;
	SourceText = emalloc(avail);
    }
    if((nextchar + 1) >= avail) {
	avail += chunksize;
	SourceText = realloc(SourceText, avail);
    }
    SourceText[nextchar++] = c;
    SourceText[nextchar  ] = '\0';
}

static int pblevel = 0;

static int lgetc(void)
{
    int c;

    c = getc(fin);
    if(pblevel == 0) {
	if(c >= 0)
	    addc(c);
    } else
	--pblevel;
    return c;
}

static void lungetc(int c)
{
    ++pblevel;
    ungetc(c, fin);
}

static double getval(void)
{
    char t[1024];
    int i;
    int c;
    double strtod(const char *, char **);

    i = 0;
    for(;;) {
	c = lgetc();
	if( (c == '.') || isdigit(c) || (c == '+') || (c == '-')
	   || (c == 'e') || (c == 'E') || (c == 'd') || (c == 'D')) {
	    t[i++] = c;
	    continue;
	}
	t[i] = '\0';
	lungetc(c);
	break;
    }
    return strtod(t, NULL);
}
    

static int backslash(int c)	/* get next char with \'s interpreted */
      
{
        char *strchr(const char *, int);
	static char transtab[] = "b\bf\fn\nr\rt\t";
	if (c != '\\')
		return c;
        c = lgetc();
	if (islower(c) && strchr(transtab, c))
		return strchr(transtab, c)[1];
        return c;
}

static int follow(int expect, int ifyes, int ifno)     /* look ahead for >=, etc. */
{
	int c = lgetc();

	if (c == expect)
		return ifyes;
        lungetc(c);
	return ifno;
}

static int	c = EOF + 1;

void skip_to_eol(void)
{
	while (c != '\n' && c != EOF)
		c = lgetc();
	if (c == '\n')
		lineno++;
}

static void skip_to_star_slash(void)
{
    for(;;) {
	if((c = lgetc()) == '*') {
	    if((c = lgetc()) == '/')
		return;
	    if(c == '*')
		lungetc(c);
	}
	if(c == EOF)
	    return;
    }
}

int yylex(void)
{
	for(;;) {
		c = lgetc();
		if( (c == ' ') || (c == '\t') || (c == '\n') ){
			if(c == '\n')
				++lineno;
			continue;
		}
		if (c == EOF)
			return ENDPROGRAM;
		break;
	}
/*
**	comment conventions
*/
	if(c == '#') {		/* eat #-started comment */
		skip_to_eol();
		return yylex();
	}
	if(c == '/') {
	    c = lgetc();
	    if(c == '/') {	/* eat //-started comment */
		skip_to_eol();
		return yylex();
	    }
	    if(c == '*') {	/* eat normal comment */
		skip_to_star_slash();
		return yylex();
	    }
	    lungetc(c);	/* not a comment: fall through */
	    c = '/';
	}
/*
**	numbers starting with a '.' are not currently allowed
*/
	if (isdigit(c))  {
		lungetc(c);
		yylval.sym = installanon();
		yylval.sym->spk.u.val = getval();
		yylval.sym->ttype = NUMBER;
		yylval.sym->spk.type = Double;
		if(verbose) fprintf(stderr, "Token: number: %g\n",
				    yylval.sym->spk.u.val);
		return NUMBER;
	}

/*
**	symbols are converted to lc during input
*/
	if (isalpha(c)) {
		Symbol *s;
		char sbuf[100], *p =sbuf;
		do {
			if (p >= sbuf + sizeof(sbuf) -1 ) {
				*p = '\0';
				execerror("lexer.c: name too long \"%s\"",
					  sbuf);
			}
			*p++=tolower(c);
		} while ((c=lgetc()) != EOF && isalnum(c));
		lungetc(c);
		*p = '\0';
		if ((s=lookup(sbuf)) == (Symbol*) 0) {
			s = installlc(sbuf);
			s->ttype = VAR;
			s->spk = newTP();
		    }
		yylval.sym = s;
		if(verbose) fprintf(stderr, "Token: Symbol: \"%s\"\n", sbuf);
		return s->ttype;
	}

	if (c == '.') {
		RBFieldInfo* rbf;
		char sbuf[100], *p =sbuf;
		if((c = lgetc()) == EOF)
		    return '.';
		if(!isalpha(c)) {
		    lungetc(c);
		    return '.';
		}
		do {
			if (p >= sbuf + sizeof(sbuf) -1 ) {
				*p = '\0';
				execerror("lexer.c: name too long \"%s\"",
					  sbuf);
			}
			*p++=tolower(c);
		} while ((c=lgetc()) != EOF && isalnum(c));
		lungetc(c);
		*p = '\0';
		if (findRBFieldInfo(sbuf, &rbf) == 0)
		    execerror("unknown rb member name: \"%s\"", sbuf);
		yylval.rbf = rbf;
		if(verbose) fprintf(stderr, "Token: Member: \"%s\"\n", sbuf);
		return MEMBER;
	}


	if (c == '"') {
		char	sbuf[1025];
		char*	p;
		char*	emalloc(unsigned int n);
		for (p = sbuf; (c=lgetc()) != '"'; p++) {
			if (c == '\n' || c == EOF)
				execerror("missing end quote in \"%s\"",
					  sbuf);
			if (p >= sbuf + sizeof(sbuf) - 1) {
				*p = '\0';
				execerror("string too long: \"%s\"", sbuf);
			}
			*p = backslash(c);
		}
		*p = 0;
		yylval.sym = installanon();
		yylval.sym->ttype = STRING;
		yylval.sym->spk.u.str = (char *)emalloc(strlen(sbuf)+1);
		strcpy(yylval.sym->spk.u.str, sbuf);
		yylval.sym->spk.type = CharP;
		yylval.sym->spk.size = strlen(sbuf) + 1;
		if(verbose) fprintf(stderr, "Token: String: \"%s\"\n",
				    yylval.sym->spk.u.str);
		return STRING;
	}

	if(verbose) fprintf(stderr, "Token: character: %c\n", c);

	switch(c) {
	case '>':	return follow('=', GE, GT);
	case '<':	return follow('=', LE, LT);
	case '=':	return follow('=', EQ, '=');
	case '!':	return follow('=', NE, NOT);
	case '|':	return follow('|', OR, '|');
	case '&':	return follow('&', AND, '&');
	case '+':	return follow('+', PLUSPLUS, '+');
	case '-':	return follow('-', MINUSMINUS, '-');
	default:	return c;
	}
}
