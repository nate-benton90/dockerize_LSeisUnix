#include "sub.h"

/*
**	SEGY trace record structure table:  stolen from CWP code.
**
**	flength == -1 ==> variable array length.
*/
typedef struct {
    RBFieldType	ftype;
    char*	fname;
    int		foffset;
    int		flength;
    char*	finfo;
} RBTableEntry;

static RBTableEntry rbBinHdr[] = {
   {rbI32, "tracl",	0,	1, "trace sequence number within line"},
   {rbI32, "tracr",	4,	1, "trace sequence number within reel"},
   {rbI32, "fldr",	8,	1, "field record number"},
   {rbI32, "tracf",	12,	1, "trace number within field record"},
   {rbI32, "ep",	16,	1, "energy source point number"},
   {rbI32, "cdp",	20,	1, "CDP ensemble number"},
   {rbI32, "cdpt",	24,	1, "trace number within CDP ensemble"},
   {rbI16, "trid",	28,	1, "trace type identification code"},
   {rbI16, "nvs",	30,	1, "number of vertically summed traces"},
   {rbI16, "nhs",	32,	1, "number of horizontally summed traces"},
   {rbI16, "duse",	34,	1, "data use: production (1) or test (2)"},
   {rbI32, "offset",	36,	1, "distance from source to receiver group"},
   {rbI32, "gelev",	40,	1, "receiver group elevation from sea level"},
   {rbI32, "selev",	44,	1, "source elevation from sea level"},
   {rbI32, "sdepth",	48,	1, "source depth (positive)"},
   {rbI32, "gdel",	52,	1, "datum elevation at receiver group"},
   {rbI32, "sdel",	56,	1, "datum elevation at source"},
   {rbI32, "swdep",	60,	1, "water depth at source"},
   {rbI32, "gwdep",	64,	1, "water depth at receiver group"},
   {rbI16, "scalel",	68,	1, "scale factor for previous 7 entries"},
   {rbI16, "scalco",	70,	1, "scale factor for next 4 entries"},
   {rbI32, "sx",	72,	1, "X source coordinate"},
   {rbI32, "sy",	76,	1, "Y source coordinate"},
   {rbI32, "gx",	80,	1, "X group coordinate"},
   {rbI32, "gy",	84,	1, "Y source coordinate"},
   {rbI16, "counit",	88,	1, "coord units code for previous four entries"},
   {rbI16, "wevel",	90,	1, "weathering velocity"},
   {rbI16, "swevel",	92,	1, "subweathering velocity"},
   {rbI16, "sut",	94,	1, "uphole time at source"},
   {rbI16, "gut",	96,	1, "uphole time at receiver group"},
   {rbI16, "sstat",	98,	1, "source static correction"},
   {rbI16, "gstat",	100,	1, "group static correction"},
   {rbI16, "tstat",	102,	1, "total static applied"},
   {rbI16, "laga",	104,	1, "lag time A (ms)"},
   {rbI16, "lagb",	106,	1, "lag time B (ms)"},
   {rbI16, "delrt",	108,	1, "delay recording time (ms)"},
   {rbI16, "muts",	110,	1, "mute time--start"},
   {rbI16, "mute",	112,	1, "mute time--end"},
   {rbU16, "ns",	114,	1, "number of samples in this trace"},
   {rbU16, "dt",	116,	1, "sample interval, in micro-seconds"},
   {rbI16, "gain",	118,	1, "gain type of field instruments code"},
   {rbI16, "igc",	120,	1, "instrument gain constant"},
   {rbI16, "igi",	122,	1, "instrument early or initial gain"},
   {rbI16, "corr",	124,	1, "correlated 1 (no) or 2 (yes)"},
   {rbI16, "sfs",	126,	1, "sweep frequency at start"},
   {rbI16, "sfe",	128,	1, "sweep frequency at end"},
   {rbI16, "slen",	130,	1, "sweep length in ms"},
   {rbI16, "styp",	132,	1, "sweep type code"},
   {rbI16, "stas",	134,	1, "sweep trace length at start in ms"},
   {rbI16, "stae",	136,	1, "sweep trace length at end in ms"},
   {rbI16, "tatyp",	138,	1, "taper type: 1=linear, 2=cos^2, 3=other"},
   {rbI16, "afilf",	140,	1, "alias filter frequency if used"},
   {rbI16, "afils",	142,	1, "alias filter slope"},
   {rbI16, "nofilf",	144,	1, "notch filter frequency if used"},
   {rbI16, "nofils",	146,	1, "notch filter slope"},
   {rbI16, "lcf",	148,	1, "low cut frequency if used"},
   {rbI16, "hcf",	150,	1, "high cut frequncy if used"},
   {rbI16, "lcs",	152,	1, "low cut slope"},
   {rbI16, "hcs",	154,	1, "high cut slope"},
   {rbI16, "year",	156,	1, "year data recorded"},
   {rbI16, "day",	158,	1, "day of year"},
   {rbI16, "hour",	160,	1, "hour of day (24 hour clock)"},
   {rbI16, "minute",	162,	1, "minute of hour"},
   {rbI16, "sec",	164,	1, "second of minute"},
   {rbI16, "timbas",	166,	1, "time basis code local(1), GMT(2), other(3)"},
   {rbI16, "trwf",	168,	1, "trace weighting factor"},
   {rbI16, "grnors",	170,	1, "geophone group number of roll sw posn one"},
   {rbI16, "grnofr",	172,	1, "geophone group number of trace one (orig)"},
   {rbI16, "grnlof",	174,	1, "geophone group number of last trace(orig)"},
   {rbI16, "gaps",	176,	1, "gap size (total number of groups dropped)"},
   {rbI16, "otrav",	178,	1, "overtravel taper code"},
   {rbF32,  "d1",	180,	1, "CWP: sample spacing for non-seismic data"},
   {rbF32,  "f1",	184,	1, "CWP: first sample loc for non-seismic data"},
   {rbF32,  "d2",	188,	1, "sample spacing between traces"},
   {rbF32,  "f2",	192,	1, "first trace location"},
   {rbF32,  "ungpow",	196,	1, "negative of power used for compress"},
   {rbF32,  "unscale",	200,	1, "reciprocal of range scaling factor"},
   {rbI16,  "mark",	204,	1, "mark selected traces"},
   {rbAfI16,"extra",	206,	17, "unassigned"}
};

static RBTableEntry rbTrace[] = {
   {rbAvF32, "trace",	240,	-1,	"trace samples"}
};

typedef struct {
    RBTableEntry*	tabptr;
    char*		tabname;
    int			tabentries;
} TabTabEntry;
TabTabEntry tabtab[] = {
    {&rbBinHdr[0], "binary header", sizeof(rbBinHdr)/sizeof(RBTableEntry)},
    {&rbTrace[0], "trace data", sizeof(rbTrace)/sizeof(RBTableEntry)},
};
static tabtablen = sizeof(tabtab)/sizeof(TabTabEntry);

static iSet nulli = {-1, -1};

#define HEADERLENGTH	(240)
#define NSOFFSET	(114)
/*
**	internal: length of the trace (from ns)
*/
static int traceLength(void* rb)
{
    unsigned short* ns;
    ns = (unsigned short*)(rb + NSOFFSET);
    return (*ns);
}
/*
**	internal: changes the value stored in ns and
**		alter the trace length accordingly.  any newly
**		created trace samples are intitialized to zero.
**	returns the length of the new chunk if a change occurred
**		and 0 otherwise.
*/
static int setTraceLength(void** rb, int newsamps)
{
    unsigned short* ns;
    int oldsamps;
    int newlength;
    int i;
    float* fp;

    ns = (unsigned short*)(*rb + NSOFFSET);
    oldsamps = *ns;
    newlength = HEADERLENGTH + newsamps * sizeof(float);
    if(oldsamps == newsamps)
	return newlength;
    *rb = realloc(*rb, newlength);
    ns = (unsigned short*)(*rb + NSOFFSET);
    *ns = newsamps;
    fp = (float*) (*rb + HEADERLENGTH);
    for(i = oldsamps; i < newsamps; i++)
	fp[i] = 0.0;
    return newlength;
}

void initRBPackage()
{
    return;
}

TPackage nameRBPackage()
{
    static char* name = "segy trace record stream";
    return cTPackage(name);
}

TPackage infoRBPackage()
{
    static char* info [] = {
	0
    };

    int i;
    TPackage t;
    TPackage* n;
    for(i = 0; info[i] != 0; i++) {
	n = accessArrayTP(&t, i);
	*n = cTPackage(info[i]);
    }
    return t;
}

RBlock getRBlock(FILE* rbs)
{
    static void* hbuff = 0;
    void* fbuff;
    RBlock rb;
    int r;
    int nsamp;
    int tlength;

    if(hbuff == 0)
	hbuff = malloc(HEADERLENGTH);
    r = fread(hbuff, HEADERLENGTH, 1, rbs);
    if(r <= 0) {
	rb.rbrecordtype = rbNothing;
	if(feof(rbs))
	    return rb;
	execerror("r/s header input error");
    }

    nsamp = traceLength(hbuff);
    if(nsamp <= 0)
	execerror("r/s input: nsamp = %d", nsamp);

    tlength = HEADERLENGTH + nsamp * sizeof(float);
    fbuff = malloc(tlength);
    memcpy(fbuff, hbuff, HEADERLENGTH);

    r = fread(fbuff + HEADERLENGTH, nsamp * sizeof(float), 1, rbs);
    if(r <= 0)
	execerror("r/s trace input error");

    rb.rbrecordtype = rbTraceRecord;
    rb.rblen = tlength;
    rb.tstatus = network_order;
    rb.rbptr = fbuff;

    return rb;
}

void putRBlock(FILE* rbs, RBlock* rb)
{
    int r;

    r = fwrite(rb->rbptr, rb->rblen, 1, rbs);
    if(r <= 0) {
	execerror("r/s write error (length = %d)", rb->rblen);
    }
}

void freeRBlock(RBlock rb)
{
    free(rb.rbptr);
}
/*
**	stuff *rbi with a pointer to a malloc'd value
**	(that is allocated at compile time and will never
**	be freed).
*/
int findRBFieldInfo(const char* name, RBFieldInfo** rbi)
{
    TabTabEntry    rbtbl;
    RBTableEntry   rb;
    int ent;
    int tab;
    RBFieldInfo*   p;

    for(tab = 0; tab < tabtablen; ++tab) {
	rbtbl = tabtab[tab];
	for(ent = 0; ent < rbtbl.tabentries; ent++) {
	    if(strcasecmp(name, rbtbl.tabptr[ent].fname) == 0) {
		rb = rbtbl.tabptr[ent];
		p = (RBFieldInfo*) emalloc(sizeof(RBFieldInfo));
		*rbi = p;
		p->rbfieldname = emalloc(strlen(rb.fname) + 1);
		strcpy(p->rbfieldname, rb.fname);
		p->rbfieldtype = rb.ftype;
		p->rbfieldoffset = rb.foffset;
		p->rbfieldlength = rb.flength;
		p->rbiset = nulli;
		return 1;
	    }
	}
    }
    return 0;
}
/*
**	return the number of elements in a member.
*/
int getRBRange(RBlock* rbl, RBFieldInfo* rbi)
{
    switch(rbi->rbfieldtype) {
      case rbI16:
      case rbU16:
      case rbI32:
      case rbF32:
	return 1;
	break;
      case rbAvF32:
	return traceLength(rbl->rbptr);
	break;
      case rbAfI16:
	return rbi->rbfieldlength;
      default:
	execerror("getRBValue: invalid field type %d for field <%s>",
		  rbi->rbfieldtype, rbi->rbfieldname);
	break;
    }
    return -1;
}
/*
**	return a TP containing the value of the indicated field.
**	This function is used only to read values.  The returned TPs
**	cannot be reliably used to change value in the RB.
*/
TPackage getRBValue(RBlock* rbl, RBFieldInfo* rbi)
{
    TPackage tp;
    short* sp;
    unsigned short* usp;
    long* lp;
    float* fp;
    int i;
    int l;
    int u;
    int o;
    int nelem;

    switch(rbi->rbfieldtype) {
      case rbI16:
	tp.type = Double;
	tp.size = 1;
	sp = (short*) ((char*)(rbl->rbptr + rbi->rbfieldoffset));
	tp.u.val = (double) (*sp);
	break;
      case rbU16:
	tp.type = Double;
	tp.size = 1;
	usp = (unsigned short*) ((char*)(rbl->rbptr + rbi->rbfieldoffset));
	tp.u.val = (double) (*usp);
	break;
      case rbI32:
	tp.type = Double;
	tp.size = 1;
	lp = (long*) ((char*)(rbl->rbptr + rbi->rbfieldoffset));
	tp.u.val = (double) (*lp);
	break;
      case rbF32:
	tp.type = Double;
	tp.size = 1;
	fp = (float*) ((char*)(rbl->rbptr + rbi->rbfieldoffset));
	tp.u.val = (double) (*fp);
	break;
      case rbAvF32:
	tp.type = FloatP;
	nelem = traceLength(rbl->rbptr);
	l = rbi->rbiset.l;
	if(l < 0) l = 0;
	u = rbi->rbiset.u;
	if(u < 0) u = nelem - 1;
	if((l > u) || (l >= nelem) || (u >= nelem))
	    execerror("getRBValue: range [%d:%d] not [0:%d]", l, u, nelem);
	tp.size = u - l + 1;
	o = rbi->rbfieldoffset + l * sizeof(float);
	tp.u.series = (float*) emalloc(sizeof(float) * tp.size);
	memcpy(tp.u.series, rbl->rbptr + o, tp.size * sizeof(float));
	break;
      case rbAfI16:
	tp.type = FloatP;
	l = rbi->rbiset.l;
	if(l < 0) l = 0;
	u = rbi->rbiset.u;
	if(u < 0) u = rbi->rbfieldlength - 1;
	tp.size = u - l + 1;
	o = rbi->rbfieldoffset + l * sizeof(short);
	tp.u.series = (float*) emalloc(sizeof(float) * tp.size);
	sp = (short*) ((char*)rbl->rbptr + o);
        for(i = 0; i < tp.size; i++)
	    tp.u.series[i] = sp[i];
	break;
      default:
	execerror("getRBValue: invalid field type %d for field <%s>",
		  rbi->rbfieldtype, rbi->rbfieldname);
    }
    return tp;
}

int putRBValue(RBlock* rbl, RBFieldInfo* rbi, TPackage* tp)
{
    short* sp;
    unsigned short* usp;
    long* lp;
    float* fp;
    int i;
    int l;
    int u;
    int nelem;
    enum {rawa, suba, vala} astate;

    switch(rbi->rbfieldtype) {
      case rbI16:
	if(tp->type != Double)
	    execerror("putRBValue: scalar assignment from type <%s>",
		      typeName(*tp));
	sp = (short*) ((char*)(rbl->rbptr + rbi->rbfieldoffset));
	*sp = tp->u.val;
	break;
      case rbU16:
	if(tp->type != Double)
	    execerror("putRBValue: scalar assignment from type <%s>",
		      typeName(*tp));
	usp = (unsigned short*) ((char*)(rbl->rbptr + rbi->rbfieldoffset));
	*usp = tp->u.val;
	break;
      case rbI32:
	if(tp->type != Double)
	    execerror("putRBValue: scalar assignment from type <%s>",
		      typeName(*tp));
	lp = (long*) ((char*)(rbl->rbptr + rbi->rbfieldoffset));
	*lp = tp->u.val;
	break;
      case rbF32:
	if(tp->type != Double)
	    execerror("putRBValue: scalar assignment from type <%s>",
		      typeName(*tp));
	fp = (float*) ((char*)(rbl->rbptr + rbi->rbfieldoffset));
	*fp = tp->u.val;
	break;
      case rbAvF32:
	nelem = traceLength(rbl->rbptr);
	l = rbi->rbiset.l;
	u = rbi->rbiset.u;
	if((l == u) && (l >= 0))
	    astate = vala;
	if((l == u) && (l == -1))
	    astate = rawa;
	if(l != u)
	    astate = suba;
	if(l < 0) l = 0;
	if(u < 0) u = nelem - 1;
	if(u >= nelem) 
	    rbl->rblen = setTraceLength(&rbl->rbptr, u + 1);
	nelem = traceLength(rbl->rbptr);
	fp = rbl->rbptr + rbi->rbfieldoffset;
	switch(tp->type) {
	  case Double:
	    for(i = l; i <= u; i++)
		fp[i] = tp->u.val;
	    break;
	  case FloatP:
	  case OFloatP:
	    if(astate == rawa) {
		rbl->rblen = setTraceLength(&rbl->rbptr, tp->size);
		nelem = traceLength(rbl->rbptr);
		u = nelem - 1;
		l = 0;
	    }
	    if((l + tp->size) > nelem)
		execerror("putRBValue: can't extend a qualified array.");
	    fp = rbl->rbptr + rbi->rbfieldoffset;
	    for(i = l; i < (l + tp->size); i++)
		fp[i] = tp->u.series[i - l];
	    for(i = l + tp->size; i <= u; i++)
		fp[i] = 0.0;
	    break;
	  default:
	    execerror("putRBValue: can't assign from <%s> in rbAvF32",
		      typeName(*tp));
	}
	break;
      case rbAfI16:
	l = rbi->rbiset.l;
	u = rbi->rbiset.u;
	if((l == u) && (l >= 0))
	    astate = vala;
	if((l == u) && (l == -1))
	    astate = rawa;
	if(l != u)
	    astate = suba;
	if(l < 0) l = 0;
	if(u < 0) u = rbi->rbfieldlength - 1;
	sp = (short*) ((char*)rbl->rbptr + rbi->rbfieldoffset);
	switch(tp->type) {
	  case Double:
	    for(i = l; i <= u; i++)
		sp[i] = (short) tp->u.val;
	    break;
	  case FloatP:
	  case OFloatP:
	    if((l + tp->size) > nelem)
		if(astate != rawa)
		    execerror("putRBValue: can't extend a fixed array.");
	    sp = rbl->rbptr + rbi->rbfieldoffset;
	    for(i = l; i < (l + tp->size); i++)
		sp[i] = (short) tp->u.series[i - l];
	    for(i = l + tp->size; i <= u; i++)
		sp[i] = 0;
	    break;
	  default:
	    execerror("putRBValue: can't assign from <%s> in rbAfI16",
		      typeName(*tp));
	}
	break;
      default:
	execerror("getRBValue: invalid field type %d for field <%s>",
		  rbi->rbfieldtype, rbi->rbfieldname);
    }
    return 0;
}
