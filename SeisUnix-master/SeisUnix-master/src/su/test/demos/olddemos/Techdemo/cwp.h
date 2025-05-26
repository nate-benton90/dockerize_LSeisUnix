/* cwp.h - include file for CWP SU programs
 *
 * $Author: jkc $
 * $Source: /src/su/include/RCS/cwp.h,v $
 * $Revision: 2.4 $ ; $Date: 88/11/17 20:42:53 $
 */

#ifndef CWP_H
#define CWP_H

#include "cwpdefs.h"
#include "suportdefs.h"


/* DECLARATIONS */
int xargc; char **xargv;	/* for getpars	*/


/* DEFINES */
#define gettr(x)	fgettr(STDIN, (x))
#define puttr(x)	fputtr(STDOUT, (x))
#define gettr1(x)	fgettr1(STDIN, (x))
#define puttr1(x)	fputtr1(STDOUT, (x))
#define gettr2(x)	fgettr2(STDIN, (x))
#define puttr2(x)	fputtr2(STDOUT, (x))
#define MUSTSGETPAR(x,y) if(!sgetpar(x,y)) err("need %s=",x)
#define MUSTHGETPAR(x,y) if(!hgetpar(x,y)) err("need %s=",x)
#define MUSTUGETPAR(x,y) if(!ugetpar(x,y)) err("need %s=",x)
#define MUSTLGETPAR(x,y) if(!lgetpar(x,y)) err("need %s=",x)
#define MUSTVGETPAR(x,y) if(!vgetpar(x,y)) err("need %s=",x)
#define MUSTIGETPAR(x,y) if(!igetpar(x,y)) err("need %s=",x)
#define MUSTPGETPAR(x,y) if(!pgetpar(x,y)) err("need %s=",x)
#define MUSTFGETPAR(x,y) if(!fgetpar(x,y)) err("need %s=",x)
#define MUSTZGETPAR(x,y) if(!zgetpar(x,y)) err("need %s=",x)


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


/* EXTERNS */
extern void syserr(), err(), warn(), selfdoc(), askdoc(), initgetpar();
extern void fputtr(), fputtr1(), fputtr2();
extern void puthval(), gethval(), puthdval(), gethdval(), rew();
extern void printfval(), fprintfval(), scanfval(), printftype();
extern uint maxgetpar();
extern short atohe();
extern ushort atoue();
extern int atoie();
extern uint atope();
extern long atole();
extern ulong atove();
extern int hgetpar(), ugetpar(), igetpar(), pgetpar(), lgetpar(), vgetpar();
extern int fgetpar(), zgetpar(), sgetpar(), gettra(), syswarn(), getindex();
extern int fgettr(), fgettr1(), fgettr2();
extern int pread(), pfread();
extern int vtoi(), valcmp(), ivmax(), ivmin(), ivabsmx();
extern int ivsum(), ivl1(), ivl2(), ivprod();
extern long vtol();
extern float vtof();
extern double vtoz();
extern char *getkey(), *hdtype(), *statprint(), *getname();
extern filetype statfil();

#endif
