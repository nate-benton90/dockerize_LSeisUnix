/*
	DSU (AEM) modifications to segy.h
*/

#ifdef gettr
#undef gettr
#endif
#define gettr(x)	(*DsuGet) (stdin, (x))

#ifdef vgettr
#undef vgettr
#endif
#define vgettr(x)	(*DsuVGet)(stdin, (x))

#ifdef puttr
#undef puttr
#endif
#define puttr(x)	(*DsuPut) (stdout, (x))

int dsufgettr(FILE *fp, segy *tp);
int dsufvgettr(FILE *fp, segy *tp);
void dsufputtr(FILE *fp, segy *tp);
