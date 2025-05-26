#ifndef BRECORD_H
#define BRECORD_H

#include "sub.h"
/*
**	data types we support.
**
**	rbAfI16 => fixed-length array of int 16
**	rbAvF32 => variable-length array of float 32.
*/
typedef enum {
    rbByte,
    rbI16,
    rbU16,
    rbI32,
    rbF32,
    rbF64,
    rbAsciiZn,
    rbAsciiBn,
    rbAfI16,
    rbAvF32
} RBFieldType;
/*
**	record types in the stream.  must include rbNothing so we
**	can signal EOF to the caller.
*/
typedef enum {
    rbNothing,
    rbTraceRecord
} RBRecordType;
/*
**	translation status (network/local)
*/
typedef enum {
    network_order,
    local_order
} Ordering;
/*
**	rbptr points to a private data type (hidden in brecord.c)
*/
typedef struct {
    RBRecordType	rbrecordtype;
    int			rblen;
    Ordering		tstatus;
    void*		rbptr;
} RBlock;

typedef struct {
    char* 				rbfieldname;
    RBFieldType				rbfieldtype;
    int					rbfieldoffset;
    int					rbfieldlength;
    iSet				rbiset;
} RBFieldInfo;

void		initRBPackage();
TPackage	nameRBPackage();
TPackage	infoRBPackage();

RBlock		getRBlock(FILE*);
void		putRBlock(FILE*, RBlock*);

void		freeRBlock(RBlock);
int		findRBFieldInfo(const char* name, RBFieldInfo** rbi);
int		getRBRange(RBlock*, RBFieldInfo*);
TPackage	getRBValue(RBlock*, RBFieldInfo*);
int		putRBValue(RBlock*, RBFieldInfo*, TPackage*);

#endif


