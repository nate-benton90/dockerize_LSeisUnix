/*
**	home for more built-in functions
*/
#include "sub.h"

static int bVector(FILE *fs)
{
    return fprintf(fs, "(");
}

static int eVector(FILE *fs)
{
    return fprintf(fs, ")");
}

int printItem(FILE *fs, TPackage tp)
{
    int c = 0;
    int i;
    switch(tp.type) {
      case TPackageP:
      case OTPackageP:
	c = bVector(fs);
	for(i = 0; i < tp.size; i++) {
	    c += printItem(fs, tp.u.tpa[i]);
	    if(i < (tp.size - 1))
		c += fprintf(fs, ", ");
	}
	c += eVector(fs);
	break;
      case Double:
	c += fprintf(fs, "%g", tp.u.val);
	break;
      case SFloatP:
	c += fprintf(fs, "%g", *tp.u.series);
	break;
      case CharP:
	c += fprintf(fs, "%s", tp.u.str);
	break;
      case FloatP:
      case OFloatP:
	c = bVector(fs);
	for(i = 0; i < tp.size; i++) {
	    c += fprintf(fs, "%g", tp.u.series[i]);
	    if(i < (tp.size - 1))
		c += fprintf(fs, ", ");
	}
	c += eVector(fs);
	break;
      default:
	c = fprintf(fs, "<%s>", stringForm(tp));
	break;
    }
    return c;
}

TPackage TPPrint(void)
{
    int i;
    double c = 0.0;
    for(i = 1; i <= nArgs(); i++)
	c += printItem(stderr, nthArg(i));
    return dTPackage(c);
}

TPackage TPPopen(void)
{
    char*	ps = asString(nthArg(1));
    char*	mode = asString(nthArg(2));
    TPackage	p;

    p.type = FileStar;
    if((p.u.fstar = popen(ps, mode)) == NULL)
	p = newTP();;
    return p;
}

TPackage TPPclose(void)
{
    FILE* stream = asFileStar(nthArg(1));

    return dTPackage((double) pclose(stream));
}

TPackage TPFopen(void)
{
    char*	ps = asString(nthArg(1));
    char*	mode = asString(nthArg(2));
    TPackage	p;

    p.type = FileStar;
    if((p.u.fstar = fopen(ps, mode)) == NULL)
	p = newTP();;
    return p;
}

TPackage TPFclose(void)
{
    FILE* stream = asFileStar(nthArg(1));
    return dTPackage((double) fclose(stream));
}

TPackage TPFflush(void)
{
    FILE* stream = asFileStar(nthArg(1));
    return dTPackage((double) fflush(stream));
}

TPackage TPFseek(void)
{
    FILE* stream = asFileStar(nthArg(1));
    long offs = asDouble(nthArg(2));
    int mode = asDouble(nthArg(3));
    switch(mode) {
      case 0: mode = SEEK_SET; break;
      case 1: mode = SEEK_CUR; break;
      case 2: mode = SEEK_END; break;
      default: execerror("TPFSeek: bad mode (%d) for offset %ld", mode, offs);
    }
    return dTPackage((double) fseek(stream, offs, mode));
}

TPackage TPFtell(void)
{
    FILE* stream = asFileStar(nthArg(1));
    return dTPackage((double) ftell(stream));
}

TPackage TPRewind(void)
{
    FILE* stream = asFileStar(nthArg(1));
    rewind(stream);
    return newTP();
}

#define SKIPBUFF	2048
TPackage TPFskip(void)
{
    FILE* stream = asFileStar(nthArg(1));
    int skip = asDouble(nthArg(2));
    char x[SKIPBUFF];
    int skiplen;

    while(skip > 0) {
	skiplen = skip;
	if(skiplen > SKIPBUFF)
	    skiplen = SKIPBUFF;
	fread(x, skiplen, 1, stream);
	skip -= skiplen;
    }
    return newTP();
}

TPackage TPFprint(void)
{
    FILE* stream = asFileStar(nthArg(1));
    int i;
    double c = 0.0;

    for(i = 2; i <= nArgs(); i++)
	c += fprintf(stream, "%s", stringForm(nthArg(i)));
    return dTPackage(c);
}

static TPackage addCharTP(TPackage tp, int ch)
{
    char* p;

    if(tp.type == Nothing) {
	tp.size = 1;
	tp.type = CharP;
	tp.u.str = (char*) malloc(1);
	tp.u.str[0] = '\0';
    }
    if(ch != '\0') {
	tp.size++;
	tp.u.str = realloc(tp.u.str, tp.size);
	p = tp.u.str + strlen(tp.u.str);
	*p++ = ch;
	*p = '\0';
    }
    return tp;
}

static TPackage nextToken(FILE *stream, char *separators)
{
    TPackage tk;
    int next;
    int inquote;

    for(;;) {
	next = getc(stream);
	if((next == '\n') || (next == EOF) || !isspace(next)) {
	    ungetc(next, stream);
	    break;
	}
    }
    tk = newTP();
    inquote = 0;
    for(;;) {
	next = getc(stream);
	if((next == EOF) || (next == '\n')) {
	    if(tk.type != Nothing)
		ungetc(next, stream);
	    break;
	}
	if(inquote) {
	    if(next == '"')
		break;
	    tk = addCharTP(tk, next);
	    continue;
	}
	if(next == '"') {
	    inquote = 1;
	    tk = addCharTP(tk, '\0');
	    continue;
	}
	if(strchr(separators, next) != 0)
	    break;
	tk = addCharTP(tk,next);
    }
    return tk;
}

TPackage TPFGetBytes(void)
{
    FILE* stream = asFileStar(nthArg(1));
    int bytes = asDouble(nthArg(2));
    char* b;
    int readin;
    TPackage t;
    int i;

    b = emalloc(bytes);
    readin = fread(b, 1, bytes, stream);
    if(readin > 0) {
	accessArrayTP(&t, readin);
	for(i = 0; i < readin; i++)
	    t.u.tpa[i] = dTPackage((double)b[i]);
    }
    free(b);
    return t;
}

TPackage TPFPutBytes(void)
{
    FILE* stream = asFileStar(nthArg(1));
    char* b;
    int readin;
    TPackage t = nthArg(2);
    int i;

    if(t.type == Nothing)
	return newTP();
    if(t.type != TPackageP)
	execerror("TPFPutBytes: argument not an array: type %s",
		  typeName(t));
    readin = t.size;
    if(readin < 1)
	return newTP();
    b = emalloc(readin);
    for(i = 0; i < readin; i++)
	    b[i] = asDouble(t.u.tpa[i]);
    fwrite(b, readin, 1, stream);
    free(b);
    return newTP();
}

TPackage TPGetTokenLine(void)
{
    TPackage t, r;
    FILE* stream = asFileStar(nthArg(1));
    char* separators;

    separators = " \\t,;";
/*
**	use stringForm() so we don't have to free() it
*/
    if(nArgs() == 2)
	separators = stringForm(nthArg(2));
    t = newTP();
    for(;;) {
	r = nextToken(stream, separators);
	if(r.type == Nothing)
	    break;
	if(t.type == Nothing) {
	    t.size = 1;
	    t.type = TPackageP;
	    t.u.tpa = (TPackage*) malloc(sizeof(TPackage));
	    t.u.tpa[0] = r;
	} else {
	    t.size++;
	    t.u.tpa = (TPackage*) realloc(t.u.tpa,
					  t.size * sizeof(TPackage));
	    t.u.tpa[t.size - 1] = r;
	}
    }
    return t;
}

static int   slen	= 0;
static int   deltas	= 1024;
static char* sbuff	= 0;

TPackage TPFgets(void)
{
    FILE* stream = stdin;
    TPackage t = newTP();
    char* cb;
    int cl;
    char* rv;

    if(nArgs() > 1)
	stream = asFileStar(nthArg(1));
    if(sbuff == 0) {
	slen += deltas;
	sbuff = emalloc(slen);
    }
    cb = sbuff;
    cl = slen;
    while(1) {
	rv = fgets(cb, cl, stream);
	if(rv == 0)
	    return newTP();
	if(strlen(cb) < (cl - 1))
	    break;
	if(cb[cl - 2] == '\n')
	    break;
	sbuff = realloc(sbuff, slen + deltas);
	cb = sbuff + strlen(sbuff);
	cl = deltas + 1;
	slen += deltas;
    }
    t.type = CharP;
    t.u.str = sbuff;
    t.size = strlen(sbuff);
    sbuff = 0;
    slen = 0;
    return t;
}

TPackage TPTmpFile(void)
{
    TPackage t = newTP();
    t.type = FileStar;
    t.u.fstar = tmpfile();
    return t;
}

TPackage TPPutRB(void)
{
    TPackage t;
    FILE* stream;
    int nextarg;

    stream = stdout;
    nextarg = 1;
    t = nthArg(nextarg);
    if(t.type == FileStar) {
	stream = asFileStar(nthArg(1));
	if(nArgs() < 2)
	    execerror("TPOutputRBChunk: FILE* not followed by anything");
	++nextarg;
    }
    for(; nextarg <= nArgs(); nextarg++) {
	t = nthArg(nextarg);
	if(t.type != RBlockP)
	    execerror("TPOutputRBChunk: argument %d not r/b type: %s",
		      nextarg, typeName(t));
	putRBlock(stream, (RBlock *) t.u.chunk);
    }
    return dTPackage(0.0);
}

TPackage TPGetRBChunk(FILE* f)
{
    TPackage t;
    RBlock rl;
    RBlock* rbp;
    t.type = RBlockP;
    rl = getRBlock(f);
    if(rl.rbrecordtype == rbNothing)
	return newTP();
    rbp = (RBlock*) emalloc(sizeof(RBlock));
    *rbp = rl;
    t.u.chunk = rbp;
    return t;
}

TPackage TPGetRB(void)
{
    FILE* stream;

    stream = stdin;
    if(nArgs() > 1)
	stream = asFileStar(nthArg(1));
    return TPGetRBChunk(stream);
}


