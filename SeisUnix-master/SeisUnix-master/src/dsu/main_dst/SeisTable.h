/*
*	SeisTable.h
*
*/

#include <sys/stat.h>
#include "extrap1.h"
/* 
	Etextrap STUFF
*/
void *GetAll(char * efile, int *size);
void sread(void* str, int size, int n, void *fp);
eTable *ezread (void *fp);
void pret(eTable *et);
