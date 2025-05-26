#ifndef LOCALSYS_H	/*  if LOCALSYS_H is undefined, define it */

#define LOCALSYS_H

#ifdef __hpux		/* hp's cpp guarantees 'hpux' will be defined */

#define HAVEASYSTEM
#define	HPUXSYSTEM
#define	X11versionR4

#endif

#ifdef sun		/* sun's cpp guarantees 'sun' will be defined */

#define HAVEASYSTEM
#define	SUNSYSTEM
#define	X11versionR4

#endif

#ifdef _IBMR2		/* ibm's cpp guarantees '_IBMR2' will be defined */

#define HAVEASYSTEM
#define AIXSYSTEM
#define	X11versionR3

#endif
#ifdef CRAY		/* cray's cpp guarantees 'CRAY' will be defined */

#define HAVEASYSTEM
#define CRAYSYSTEM
#define	X11versionR4

#endif

#ifdef __convex__   /* convex's cpp guarantees '__convex__' will be defined */

#define HAVEASYSTEM
#define	CONVEXSYSTEM
#define	X11versionR4

#endif

#ifndef HAVEASYSTEM	/* if nothing has been defined by now, kill compiler
				by entering garbage */
	kill <- the -> compiler  :: NO SYSTEM
#endif

#endif
