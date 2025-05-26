/* header in front of each compressed packet */
/* The distance between packets is "nbyte" */

#ifndef SUCOMP_H
#define SUCOMP_H

#define	SUCOMPID	 "sucomp"

#ifdef	AWARE
#define DATA_FORMAT	3
#define ENDIAN 		0
#define	LAMBDA		0.0
#define	LINUX		1
#endif

typedef struct {
	char	id[8];		/* file type id */
	char	key[12];	/* segmentation key */
	Value	value;		/* key value (precise)*/
	int 	pad1;
	int	ns;		/* trace length */
	int	ntrace;		/* number of traces */
	float	compress;	/* compression factor */
	float	mse;		/* mean square error of compression */
	int	nbyte;		/* number of bytes */
	int	pad2;
} comp_header;

#endif
