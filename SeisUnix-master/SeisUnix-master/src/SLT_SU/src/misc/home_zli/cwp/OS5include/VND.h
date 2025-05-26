/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */


#include "par.h"
#define	VND_HOME ".VND"
/*********************** self documentation **********************/
/*************************************************************************
This is file VND.h.  

If you use this package of routines outside of SU, you may wish to 
operate independently of the SU defines and replace the include to par.h to by

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/errno.h>
#define	MIN(x,y) ((x) < (y) ? (x) : (y))

Define local file VND_HOME with list of default file systems for VND space.
Note that the default distribution name for VND_HOME is ".VND".  This file contains:
   line 1 of file: 	integer giving number of file systems	
   successive lines: 	directory path to each file system, one per line
Ideally, this variable would be set using a getenv("VND_HOME") function
but currently it is just hardcoded into the source code.

Note that machine dependent compilation options can substantially influence
how efficiently the VND routines run on a given machine.  See the
discussions in the file VND.h.

*************************************************************************/


/* MaxIOBuf is the maximum length of an Input/Output operation in bytes */
/* allowed on the current system.   If it is small, there may be no 	*/
/* advantage to using a large amount of memory for VND buffering so	*/
/* VND memory sizes are adjusted accordingly.  If you are on a system	*/ 
/* like a Cray PVP system which does a few large I/O operations much 	*/
/* more efficiently than many small ones, set this to a big number.	*/
/* Then, your VND routines will use larger buffers and be more 		*/
/* efficient.  You can tweak this number for better efficiency.  The	*/
/* default works well on LINUX boxes and small workstations.		*/
/* 									*/
/* #define MaxIOBuf 32768						*/

/* BytesPerSector equals the number of bytes per physical disk sector.	*/
/* If it is set to 1, then memory and i/o blocks for the VND routines	*/
/* are not rounded to to the next larger sector size so there is not	*/
/* any wasted space and files and memory buffers are as small as 	*/
/* possible.  On some machines, it is very important to work in blocks	*/ 
/* that are multiples of the sector size for efficiency reasons. 	*/ 
/* In that case, use							*/
/*									*/
/* #define BytesPerSector BUFSIZ					*/
/*									*/
/* Note that BUFSIZ is defined in the stdio.h header file.		*/ 

/**************** end self doc ********************************/

/* Define integer size and routine names for Fortran interface		*/

/* Use this on CONVEX only if want 8 byte word length for */
/* integers or floats.  Usually use the default. */
#ifdef CONVEXPD8
#define FORTINT long long int
#define BytesPerSector BUFSIZ
#define MaxIOBuf BUFSIZ*16

/* The IBM RS6000 workstations assume Fortran routines */
/* are all lower case if you are not using special compiler */
/* options as are standardly done in systems like ProMAX.  */ 
/* For use with ProMAX, don't specify the compiler option */
/* -DIBMRS6000 during your compiliation and all will work. */
#elif IBMRS6000
#define FORTINT long
#define vndop_ vndop
#define vndrw_ vndrw
#define vndcl_ vndcl
#define vndflush_ vndflush
#define v2dr2c_ v2dr2c
#define v2dc2r_ v2dc2r
#define v2dr1_ v2dr1
#define v2dr2_ v2dr2
#define v2dw1_ v2dw1
#define v2dw2_ v2dw2
#define BytesPerSector BUFSIZ
#define MaxIOBuf BUFSIZ*16

/*  The CRAY assumes Fortran routines are capitalized.  		*/
/*  Reals and ints are 8 bytes long.  Very long i/o's are best. 	*/
/*  New documentation suggests there is a better way to 		*/
/*  code Fortran string variables than done here for the Cray.  	*/
/*  This FORTRAN interface for the Cray has not been tested recently.  	*/
#elif CRAY
#define FORTINT int
#define vndop_ VNDOP
#define vndrw_ VNDRW
#define vndcl_ VNDCL
#define vndflush_ VNDFLUSH
#define v2dr2c_ V2DR2C
#define v2dc2r_ V2DC2R
#define v2dr1_ V2DR1
#define v2dr2_ V2DR2
#define v2dw1_ V2DW1
#define v2dw2_ V2DW2
#define BytesPerSector BUFSIZ
#define MaxIOBuf BUFSIZ*1024 

/*  This works pretty well as the default on most machines. */
#else
#define FORTINT long
#define BytesPerSector BUFSIZ
#define MaxIOBuf BUFSIZ*8

#endif
typedef struct{
	long NumDim;		/* number of active dimensions 			*/
	long NumBytesPerNode;	/* number of bytes per node 			*/
	long NumBlocksPerPanel;	/* number of blocks per panel 			*/
	long NumFiles;		/* number of physical files used 		*/
	long NumPanels;		/* number of panels 				*/
	long NumOpenPanels;	/* number of panels open at one time 		*/
	long NumBytesMemBuf;	/* number of bytes in memory buffer 		*/
	long NumBytesPerBlock;  /* number of bytes per block 			*/
	long NumBytes1stDim;	/* number of bytes for IO in 1st dimension 	*/
	long NumNodesPerBlock;	/* number of nodes per block 			*/
	long NumBlocksPerFile;	/* number of blocks per physical file 		*/
	int CurrentFile;	/* index (zero based) of current open file 	*/
				/* -1 if not open 				*/
	char *Mode;		/* file open mode: "rb", "rb+", or "wb+"	*/
	FILE *FileDescriptor;	/* file descriptor for ANSI standard binary IO 	*/

   				/* The following have "physical file number" as */
				/* the first index 				*/

	int *LenFileNames;	/* length of each file name in characters 	*/
	char **FileNames;	/* list of file names for physical files 	*/
	long *FirstDataByte;	/* first data byte for each physical file 	*/
	long *LenFile;		/* length of each physical file in bytes 	*/

   				/*The following are arrays dimensioned NumDim.	*/
	long *N;		/* number of nodes for each dimension 		*/
	long *NNodesPerBlock;	/* array of nodes per block for each dimension 	*/
	long *NBlocks;		/* number of blocks in each dimension 		*/
	long *ByteIncr;		/* byte increment between values in each 	*/
				/* dimension 					*/
	long *BlockIncr;	/* block increment for each dimension 		*/

  				/* The following are arrays dimensioned 	*/
				/* noppanels, one for each panel to open 	*/
				/* simultaneously.				*/
  
	long *CurrentPanel;	/* current panel in memory (zero based) 	*/
	int *CurrentDimension;	/* current dimension, -1 if none in memory 	*/
	long *CurrentBlock;	/* starting block for current slice in memory 	*/
				/* for current dimension 			*/
	int *Modified;		/* 1 if current memory buffer has been modified */
				/* 0 if current memory not modified and can be	*/
				/*   thrown away without losing any information	*/

	char *w;		/* pointer to internal memory buffer used by VND*/
} VND;

VND *VNDop(int mode, long lwmax, int ndim, long *N, long npanels, 
		long nbytes, char *file, int ndir, char**dir, int noppanels);
void VNDrw(char iop, int iopanel,VND *vnd,int idim, 
		long *key, long ipanel, char *values,
		long start, long incr, long nvalues,
		int msgnum, char *msg);
void VNDcl(VND *vnd, int mode);
void VNDflush(VND *vnd);
char *VNDtempname(char *root);
int VNDGetDimensions(VND *vnd, long lwmax, int ich);
int VNDrwslice(VND *vnd, int iopanel, char mode, int idim, long iblock, long ipanel);
int VNDseek(VND *vnd, long ib);
void VNDdump(VND *vnd,FILE *fp);
void VNDlook(int *w, int lw);
void *VNDemalloc(size_t n,char *msg);
void VNDerr(char *msg);

/* additions 2/94 to the C interface */
void VNDr2c(VND *vnd);
void VNDc2r(VND *vnd);
VND *V2Dop(int mode, long lwmax, long nbytes, char *file, long n0, long n1);
void V2Dr0(VND *vnd,long key,char *buf,int msgnum);
void V2Dr1(VND *vnd,long key,char *buf,int msgnum);
void V2Dw0(VND *vnd,long key,char *buf,int msgnum);
void V2Dw1(VND *vnd,long key,char *buf,int msgnum);

/* additions 2/94 to the Fortran interface */
void vndr2c_(VND *vnd);
void vndc2r_(VND *vnd);
void v2dr1_(VND *vnd,FORTINT key,void *buf,FORTINT msgnum);
void v2dr2_(VND *vnd,FORTINT key,void *buf,FORTINT msgnum);
void v2dw1_(VND *vnd,FORTINT key,void *buf,FORTINT msgnum);
void v2dw2_(VND *vnd,FORTINT key,void *buf,FORTINT msgnum);

/* additions 9/94 to the C interface */
void VNDmemchk( void *p , char * msg);
void VNDfree( void *p, char *msg);
long VNDtotalmem();

#ifdef CRAY
void vndop_( VND **vnd, FORTINT *fort_mode, FORTINT *fort_lwmax, 
	FORTINT *fort_ndim, FORTINT *fort_N, FORTINT *fort_npanels,  
	FORTINT *fort_nbytes, char *fort_file, 
	FORTINT *fort_ndir, char *fort_dir, FORTINT *fort_noppanels);
void vndrw_(char *fort_iop, FORTINT *fort_iopanel,VND **vnd,
	FORTINT *fort_idim, FORTINT *fort_key, FORTINT *fort_ipanel,
	void *values, FORTINT *fort_start, FORTINT *fort_incr,
	FORTINT *fort_nvalues, FORTINT *fort_msgnum);
#else
void vndop_( VND **vnd, FORTINT *fort_mode, FORTINT *fort_lwmax, 
	FORTINT *fort_ndim, FORTINT *fort_N, FORTINT *fort_npanels,  
	FORTINT *fort_nbytes, char *fort_file, 
	FORTINT *fort_ndir, char *fort_dir, FORTINT *fort_noppanels, 
	int lfile, int ldir);
void vndrw_(char *fort_iop, FORTINT *fort_iopanel,VND **vnd,
	FORTINT *fort_idim, FORTINT *fort_key, FORTINT *fort_ipanel,
	void *values, FORTINT *fort_start, FORTINT *fort_incr,
	FORTINT *fort_nvalues, FORTINT *fort_msgnum, long liop);
#endif
void vndcl_(VND **vnd, FORTINT *fort_mode);
void vndflush_(VND **vnd);

#ifdef IBMRS6000
void *malloc();
void free();
#endif


