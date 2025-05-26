#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "sub.h"

TPackage	TPSin(void);
TPackage	TPCos(void);
TPackage	TPAtan(void);
TPackage	TPLog(void);
TPackage	TPLog10(void);
TPackage	TPExp(void);
TPackage	TPSqrt(void);
TPackage	TPInteger(void);
TPackage	TPNInteger(void);
TPackage	TPAbs(void);

TPackage	TPStrToNum(void);
TPackage	TPDNumToStr(void);
TPackage	TPINumToStr(void);
TPackage	TPFlattened(void);

TPackage	object_size(void);
TPackage	bltin_exit(void);
TPackage	clocktime(void);
TPackage	urandom(void);

TPackage	fArg(void);

TPackage	TPPrint(void);
TPackage	TPPopen(void);
TPackage	TPPclose(void);
TPackage	TPFopen(void);
TPackage	TPFclose(void);
TPackage	TPFflush(void);
TPackage	TPFprint(void);

TPackage	TPFgets(void);
TPackage	TPGetRBChunk(FILE*);
TPackage	TPPutRB(void);
TPackage	TPGetRB(void);
TPackage	TPTmpFile(void);
TPackage	TPFseek(void);
TPackage	TPFtell(void);
TPackage	TPRewind(void);
TPackage	TPFskip(void);
TPackage	TPFGetBytes(void);
TPackage	TPFPutBytes(void);
TPackage	TPSystem(void);

TPackage	TPGetTokenLine(void);

TPackage 	traceback(void);

TPackage	TPGetRBChunk(FILE*);
TPackage	TPOutputRBChunk(void);

TPackage	TPMax(void);
TPackage	TPMin(void);
TPackage	TPSum(void);
TPackage	fVector(void);
TPackage	TPSequence(void);

TPackage	TPpfnext(void);
TPackage	TPpfbest(void);
TPackage	TPextend(void);
TPackage	TPfdft(void);
TPackage	TPidft(void);

TPackage	internals(void);

TPackage	TPisDoubleP(void);
TPackage	TPisStringP(void);
TPackage	TPisNothing(void);
TPackage	TPisArrayP(void);
TPackage	TPisfVectorP(void);

#endif

