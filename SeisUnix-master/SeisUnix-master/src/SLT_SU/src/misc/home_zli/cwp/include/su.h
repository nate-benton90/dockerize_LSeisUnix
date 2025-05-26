/* su.h - include file for SU programs
 *
 * $Author: jkc $
 * $Source: /usr/local/src/su/include/RCS/su.h,v $
 * $Revision: 1.15 $ ; $Date: 90/12/22 18:36:53 $
 */

#ifndef SU_H
#define SU_H

#include "par.h"
#include "segy.h"
#include "suport.h"
#include "header.h"

/* TYPEDEFS */

typedef char *String;
typedef union { /* storage for arbitrary type */
        char s[8];
        short h;
        unsigned short u;
        long l;
        unsigned long v;
        int i;
        unsigned int p;
        float f;
        double d;
} Value;


/* DEFINES */
#define gettr(x)	fgettr(stdin, (x))
#define puttr(x)	fputtr(stdout, (x))
#define gettra(x, y)    fgettra(stdin, (x), (y))
#define gethdr(x, y)    fgethdr(stdin, (x), (y))
#define puthdr(x, y)    fputhdr(stdout, (x), (y))
#define NALLOC  (524288)
#define NFALLOC (NALLOC/FSIZE)
#define NIALLOC (NALLOC/ISIZE)
#define NDALLOC (NALLOC/DSIZE)
#define LOWBYTE(w) ((w) & 0xFF)
#define HIGHBYTE(w) LOWBYTE((w) >>8)
#define LOWWORD(w) ((w) & 0xFFFF)
#define HIGHWORD(w) LOWWORD((w) >>16)
#define ISNEGCHAR(c) ((c) & 0x80)
#define SIGNEXTEND(c) (~0xFF | (int) (c))


/*	READ_OK  - read  permission for access(2)
 *	WRITE_OK - write permission for access(2)
 *	EXEC_OK  - exec  permission for access(2)
 *	FILE_OK  - file  existence  for access(2)
 *	Note: these are changed from the usual defines in file.h
 *	      because this include exists on some machines and
 *	      not others, often overlaps fcntl.h, etc.  Lint is
 *            happier with a fresh start.
 *	Note: Post-ANSI sometimes R_OK in unistd.h (this isn't
 *	      an ANSI file).
 */
#define		READ_OK		4
#define		WRITE_OK	2
#define		EXEC_OK		1
#define		FILE_OK		0
#define		EXIT_SUCCESS	0
#define		EXIT_FAILURE	1
#define		SEEK_SET	0
#define		SEEK_CUR	1


/* The following refer to the trid field in segy.h		*/
/* CHARPACK represents byte packed seismic data from supack1	*/
#define		CHARPACK	101
/* SHORTPACK represents 2 byte packed seismic data from supack2	*/
#define		SHORTPACK	102
/* TREAL represents real time traces ( see trid in segy.h)	*/
#define		TREAL		1
/* TCMPLX represents complex time traces (see trid in segy.h)	*/
#define		TCMPLX		13
/* TAMPH represents time domain data in amplitude/phase form	*/
#define		TAMPH		15
/* FPACK represents packed frequency domain data (see segy.h)	*/
#define		FPACK		12
/* FUNPACKNYQ represents complex frequency domain data (segy.h)	*/
#define		FUNPACKNYQ	11
/* FCMPLX represents complex frequency domain data (see segy.h)	*/
#define		FCMPLX		10
/* FAMPH represents freq domain data in amplitude/phase form	*/
#define		FAMPH		14
/* REALPART represents the real part of a trace to Nyquist	*/
#define		REALPART	16
/* IMAGPART represents the real part of a trace to Nyquist	*/
#define		IMAGPART	17
/* AMPLITUDE represents the amplitude of a trace to Nyquist	*/
#define		AMPLITUDE	18
/* PHASE represents the phase of a trace to Nyquist		*/
#define		PHASE		19
/* KT represents wavenumber-time domain data 			*/
#define		KT		21
/* KOMEGA represents wavenumber-frequency domain data		*/
#define		KOMEGA		22
/* ENVELOPE represents the envelope of the complex time trace	*/
#define		ENVELOPE	23
/* INSTPHASE represents the phase of the complex time trace	*/
#define		INSTPHASE	24
/* INSTPHASE represents the frequency of the complex time trace */
#define         INSTFREQ        25


/* FUNCTION PROTOTYPES */
#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

int fgettr(FILE *fp, segy *tp);
void fputtr(FILE *fp, segy *tp);
int fgettra(FILE *fp, segy *tp, int itr);
void fgethdr(FILE *fp, segychdr *chdr, segybhdr *bhdr);
void fputhdr(FILE *fp, segychdr *chdr, segybhdr *bhdr);
int auxgettr(FILE *fp, segy *tp);
void auxputtr(FILE *fp, segy *tp);
void auxgethdr(FILE *fp, segychdr *chdr, segybhdr *bhdr);
void auxputhdr(FILE *fp, segychdr *chdr, segybhdr *bhdr);


/* hdrpkge */
void gethval(segy *tr, int index, Value *valp);
void puthval(segy *tr, int index, Value *valp);
void gethdval(segy *tr, char *key, Value *valp);
void puthdval(segy *tr, char *key, Value *valp);
char *hdtype(char *key);
char *getkey(int index);
int getindex(char *key);

/* bhdrpkge */
void getbhval(segybhdr *bh, int index, Value *valp);
void putbhval(segybhdr *bh, int index, Value *valp);
void getbhdval(segybhdr *bh, char *key, Value *valp);
void putbhdval(segybhdr *bh, char *key, Value *valp);
char *bhdtype(char *key);
char *getbkey(int index);
int getbindex(char *key);

/* valpkge */
int vtoi(register String type, Value val);
long vtol(register String type, Value val);
float vtof(register String type, Value val);
double vtod(register String type, Value val);
int valcmp(register String type, Value val1, Value val2);
void printfval(register String type, Value val);
void fprintfval(FILE *stream, register String type, Value val);
void scanfval(register String type, Value *valp);
void printheader(segy *tp);
void atoval(String type, String keyval, Value *valp);
void getparval(String name, String type, int n, Value *valp);
Value valtoabs(String type, Value val);

void tabplot(segy *tp, int itmin, int itmax);

#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif
