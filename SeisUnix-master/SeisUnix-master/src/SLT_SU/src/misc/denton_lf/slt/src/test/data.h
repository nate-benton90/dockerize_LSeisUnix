/*
data object definition

Three axes, global information, plus data buffer
Uses axis definitions
*/

#ifndef DATA_H
#define DATA_H

#define DATA_NAXIS	6
#define	DATA_VALUE	0
#define	DATA_AXIS0	0
#define	DATA_AXIS1	1
#define	DATA_AXIS2	2
#define	DATA_AXIS3	3
#define	DATA_AXIS4	4
#define	DATA_AXIS5	5
#define	DATA_HEAD	60
#define	DATA_HEAD0	100
#define	DATA_HEAD1	800
#define	DATA_TSIZE	4096
#define	DATA_ESIZE	1
#define	DATA_SEGY	0
#define	DATA_CENT_MIN	0.
#define DATA_LOW	1.
#define	DATA_HIGH	255.
#define	DATA_CENT	99.
#define	DATA_CENT_LOW	(100 - data->cent)
#define	DATA_CENT_HIGH	data->cent
#define	DATA_CENT_MAX	100.
#define	DATA_MIN_GPOW	0.1
#define	DATA_MAX_GPOW	10.0
#define	DATA_TPOW	0.0
#define	DATA_VALUE_SIZE	127
#define DATA_VALUE_BASE 1

/* Data object */
#include "gridhd.h"
typedef struct {
	string	title;	        /* dataset name */
	string	file;	        /* input file name */
	int	esize;	        /* element size in bytes */
	int	segy;	        /* segy format or not */
	int	vgrid;	        /* vgrid format or not */
        int     overlay_mode;   /* 1 = overlay mode */
	int	hbytes;	        /* header bytes to discard; 0 default, 3600 segy=1 */
	string	script;	/* script file name */
	Axis	axis[DATA_NAXIS];	/* axes */
	int	transp;		/* transpose axis */
	unsigned int	size;	/* number of samples */
	float	tpow;	/* time power correction */
	float	gpow;	/* gpow of data samples */
	float	low;	/* low clip value of data samples */
	float	high;	/* high clip value of data samples */
	float	min;	/* maximum value */
	float	max;	/* maximum value */
	float	cent;	/* clip percentile */
	float	half;	/* half value */
	int	value_base;	/* minimum value */
	int	value_size;	/* maximum value */
	Buffer	buffer;	/* samples */
	float	histogram[256];	/* histogram of data samples per color */
	int	hist_low;	/* low index of histogram */
	int	hist_high;	/* high index of histogram */
	ghed	gh;	/* vgrid header */
	} *Data;


/* data.c */
Data DataInit(void);
void DataGetpar(Data data);
int DataGridHeader(Data data, int fd);
void DataLoad(void);
void DataLoadByte(Data data);
void DataLoadFloat(Data data);
float DataCent(float *x, int n, float p);
char *DataLabel(Data data);
char *DataTitle(Data data);
char *DataFile(Data data);
char *DataShortName(Data data);
Buffer DataBuffer(Data data);
Axis DataAxis(Data data, int iaxis);
int DataSize(Data data);
int DataMaxDim(Data data);
float DataValue(Data data, int value);
int DataValueSize(Data data);
int DataValueBase(Data data);
float DataHigh(void);
float DataLow(void);
int DataHistogram(int i);
void DataComputeHistogram(Data data);
void DataInfo(Data data);
void DataValueInfo(Data data);
void DataSavePar(Data data);
void DataDumpBytes(Data data, char *file, int fd);
void DataDumpHeader(Data data, char *file, int datafd, int esize);
void DataDumpFloats(Data data, char *file, int fd);
void Data2Float(Buffer dbuf, float *fbuf, int n, float *min, float *max);
int irint(float x);

#endif
