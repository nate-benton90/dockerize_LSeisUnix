/* header in front of each compressed packet */
/* The distance between packets is "nbyte" */

#ifndef SUCOMP_H
#define SUCOMP_H

#define	SUCOMPID	 "sucomp"

#ifdef	AWARE
#define DATA_FORMAT	3
#define ENDIAN 		0
#define	LAMBDA		0.0
#endif

#if (LINUX == 1)

#define ENDIAN 		2

typedef struct {
	char	        id[8];		/* file type id */
	char	        key[12];	/* segmentation key */
	Value	        value;		/* key value (precise)*/
	int 	        pad1;
	unsigned long	ns;		/* trace length */
	unsigned long	ntrace;		/* number of traces */
	float	        compress;	/* compression factor */
	float	        mse;		/* mean square error of compression */
	long	        nbyte;		/* number of bytes */
	int	        pad2;
} comp_header;

#else


typedef struct {
	char	id[8];		/* file type id */
	char	key[12];	/* segmentation key */
	Value	value;		/* key value (precise)*/
	int	ns;		/* trace length */
	int	ntrace;		/* number of traces */
	float	compress;	/* compression factor */
	float	mse;		/* mean square error of compression */
	long    nbyte;		/* number of bytes */
} comp_header;

#endif

#endif
