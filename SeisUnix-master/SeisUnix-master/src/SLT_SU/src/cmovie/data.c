/*
data object code
data object consists of axes, buffers, and dataset info
four input data formats recognized:
    FORMAT  CONDITIONS      ACTIONS
    vgrid   DataGridHeader()==1 DataLoadFloat() or DataLoadByte()
    segy    else segy=1     DataGetpar(), DataLoadFloat()
    floats  else float=1        DataGetpar(), DataLoadFloat()
    bytes   else            DataGetpar(), DataLoadByte()
*/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "par.h"

#include "main.h"
#include "axis.h"
#include "data.h"
#include "color.h"
#include "draw.h"
#include "ui_menu.h"
#include "ui_window.h"

/* initialize dataset from getpar */
Data     DataInit(void)
{
    Data     data;
    Axis     axis;
    extern int infd;
    int      i;
    FILE    *fd;

    cwp_String parString;

    {
        extern int _alloc;

        data = (Data) malloc((size_t) sizeof(data[0]));
        _alloc += sizeof(data[0]);
        if( data == 0 ){
            err("cant allocate %d bytes for  data; %d already allocated",
                sizeof(data[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " data", sizeof(data[0]));
        }
    };
    /* fetch grid parameters from tail record or pars */
    data->vgrid = -1;
    data->overlay_mode  = 0;

    getparint("vgrid", &data->vgrid);
    if( data->vgrid != 0 ){
        if( DataGridHeader(data, infd) == 0 ){
            DataGetpar(data);
        }
    } else {
        DataGetpar(data);
    }
    lseek64(infd, 0L, 0);
    strcpy(data->title, "stdin");
    if( getparstring("title", &parString) == 0 ){
        if( getparstring("in", &parString ) ){
            strcpy( data->title ,parString );
        }
    }else{
        strcpy( data->title ,parString );
    }

    if( getparstring("in", &parString ) ){
        strcpy( data->file ,parString );
    }
 
    /* get external script file */

    if( getparstring("script", &parString) ){
        strcpy( data->script ,parString );
        fd = fopen(data->script, "r");
        if( fd == NULL ){
            UIMessage("cant open script file");
        } else {
            int n = AxisSize(data->axis[DATA_AXIS3]);
            for( i = 0; i < n &&
                 fgets(AxisScript(data->axis[DATA_AXIS3], i), sizeof(string),
                       fd) != NULL; i++ ){
                AxisScript(data->axis[DATA_AXIS3],
                           i)[strlen(AxisScript(data->axis[DATA_AXIS3], i)) -
                              1] = '\0';
            }
        }
        fclose(fd);
    }
    data->tpow = 0.;
    getparfloat("tpow", &data->tpow);
    data->gpow = 1.;
    getparfloat("gpow", &data->gpow);
    data->transp = 0;
    getparint("transp", &data->transp);
    /* perform transpose by swapping axes */
    if( data->transp ){

        {
            extern int _alloc;

            axis = (Axis) malloc((size_t)sizeof(axis[0]));
            _alloc += sizeof(axis[0]);
            if( axis == 0 ){
                err("cant allocate %d bytes for  axis; %d already allocated",
                    sizeof(axis[0]), _alloc);
            }
            if( memwatch ){
                (void) printf("malloc %s=%d\n", " axis", sizeof(axis[0]));
            }
        };
        axis[0] = data->axis[DATA_AXIS1][0];
        data->axis[DATA_AXIS1][0] = data->axis[DATA_AXIS2][0];
        data->axis[DATA_AXIS2][0] = axis[0];
        if( axis ){
            free(axis);
            axis = 0;
            if( memwatch ){
                printf("free %s\n", "axis");
            }
        };
    }
    data->value_base = DATA_VALUE_BASE;
    data->value_size = DATA_VALUE_SIZE;
    {
        extern int _alloc;

        data->buffer = (Buffer) malloc((size_t) (data->size) * sizeof(data->buffer[0]));
        _alloc += (data->size) * sizeof(data->buffer[0]);
        if( data->buffer == 0 ){
            err
                ("cant allocate %d bytes for  data->buffer; %d already allocated",
                 (data->size) * sizeof(data->buffer[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " data->buffer",
                          (data->size) * sizeof(data->buffer[0]));
        }
    };
    return (data);
}

/* fetch grid parameters from getpar */
void DataGetpar(Data data)
{
    int      iaxis;
    string   label;
    cwp_String   parString;

    data->segy = DATA_SEGY;
    getparint("segy", &data->segy);
    data->esize = 0;
    getparint("esize", &data->esize);
    if( data->segy ){
        data->esize = 4;
    } else if( data->esize == 0 ){
        data->esize = 1;
    }
    data->size = 1;
    if( data->segy ){
        data->hbytes = 3600;
    } else {
        data->hbytes = 0;
    }
    getparint("hbytes", &data->hbytes);
    for( iaxis = DATA_AXIS1; iaxis <= DATA_AXIS3; iaxis++ ){

        data->axis[iaxis] = AxisInit(iaxis, data->size);
        data->size *= AxisSize(data->axis[iaxis]);
    }
    data->axis[DATA_AXIS4] =
        AxisInit2(DATA_AXIS4, data->size, "n4", 1, 0., 1., 1.);
    data->size *= AxisSize(data->axis[DATA_AXIS4]);
    data->axis[DATA_AXIS5] =
        AxisInit2(DATA_AXIS5, data->size, "n5", 1, 0., 1., 1.);
    data->size *= AxisSize(data->axis[DATA_AXIS5]);
    data->max = DATA_HIGH;
    getparfloat("max", &data->max);
    data->high = data->max;
    getparfloat("high", &data->high);
    getparfloat("pclip", &data->high);
    getparfloat("clip", &data->high);
    if( data->high != DATA_HIGH ){
        data->min = -data->high;
        data->low = data->min;
    } else {
        data->min = DATA_LOW;
        data->low = data->min;
    }
    getparfloat("low", &data->low);
    getparfloat("nclip", &data->low);
    getparfloat("min", &data->min);
    data->cent = DATA_CENT;
    getparfloat("perc", &data->cent);
    getparfloat("cent", &data->cent);
    strcpy(label, "samples");
    if( getparstring("value", &parString) ){
       strcpy(label ,parString );
    }
    data->axis[DATA_VALUE] = AxisInit2(DATA_VALUE ,1 ,label 
                            ,DATA_VALUE_SIZE ,data->low 
                            ,(data->high - data->low) / (DATA_VALUE_SIZE-1) 
                            ,1.0);
}

/* fetch grid parameters from tail of vgrid format */
DataGridHeader(Data data, int fd)
{
    int      size, n1, n2, n3, n4, n5, size1;
    string label;
    cwp_String   parString;

    size = lseek64(fd, -(long long)sizeof(data->gh), 2);
    if( read(fd, &data->gh, sizeof(data->gh)) < sizeof(data->gh) ){
        lseek64(fd, 0L, 0);
        data->gh.dtype = 0.;
        return (0);
    }

#ifdef DBG_GRD

    {  FILE* fp;
     
       if( (fp = fopen( "gridhd.dat" ,"w" ) ) ){
          fwrite( &(data->gh) ,1 ,sizeof(data->gh) ,fp );
          fclose( fp );
       }
    }
#endif
       
    if( data->gh.scale == 0.0 ){
        data->gh.dtype = 0.;
        return (0);
    }
    size1 = irint(data->gh.n1 / data->gh.scale)
        * irint(data->gh.n2 / data->gh.scale)
        * irint(data->gh.n3 / data->gh.scale)
        * irint(data->gh.n4 / data->gh.scale)
        * irint(data->gh.n5 / data->gh.scale)
        * irint(data->gh.dtype / data->gh.scale);
    if( size != size1 ){
        lseek64(fd, 0L, 0);
        data->gh.dtype = 0.;
        return (0);
    }
    data->vgrid = 1;
    if( irint( data->gh.gtype /data->gh.scale ) == 5 ){
       data->overlay_mode = 1;
    }
    strcpy(label, "time");

    if( getparstring("label1", &parString ) ){
        strcpy( label ,parString );
    }
    n1 = irint(data->gh.n1 / data->gh.scale);
    data->axis[DATA_AXIS1] =
        AxisInit2(DATA_AXIS1, 1, label, n1, data->gh.o1 / data->gh.scale,
                  data->gh.d1 / data->gh.scale, 1.);
    strcpy(label, "cdp");
    if( getparstring("label2", &parString ) ){
        strcpy( label ,parString );
    }
    n2 = irint(data->gh.n2 / data->gh.scale);
    data->axis[DATA_AXIS2] = AxisInit2(DATA_AXIS2, n1, label,
                                       n2, (data->gh.o2 / data->gh.scale),
                                       (data->gh.d2 / data->gh.scale), 1.);
    strcpy(label, "line");
    if( getparstring("label3", &parString ) ){
        strcpy( label ,parString );
    }
    n3 = irint(data->gh.n3 / data->gh.scale);
    getparint("n3", &n3);
    data->axis[DATA_AXIS3] = AxisInit2(DATA_AXIS3, n1 * n2, label,
                                       n3, (data->gh.o3 / data->gh.scale),
                                       (data->gh.d3 / data->gh.scale), 1.);
    strcpy(label, "n4");
    if( getparstring("label4", &parString ) ){
        strcpy( label ,parString );
    }
    n4 = irint(data->gh.n4 / data->gh.scale);
    getparint("n4", &n4);
    data->axis[DATA_AXIS4] = AxisInit2(DATA_AXIS4, n1 * n2 * n3, label,
                                       n4, (data->gh.o4 / data->gh.scale),
                                       (data->gh.d4 / data->gh.scale), 1.);
    strcpy(label, "n5");
    if( getparstring("label5", &parString ) ){
        strcpy( label ,parString );
    }
    n5 = irint(data->gh.n5 / data->gh.scale);
    getparint("n5", &n5);
    data->axis[DATA_AXIS5] = AxisInit2(DATA_AXIS5, n1 * n2 * n3 * n4, label,
                                       n5, (data->gh.o5 / data->gh.scale),
                                       (data->gh.d5 / data->gh.scale), 1.);
    strcpy(label, "amplitude");
    if( getparstring("amplitude", &parString ) ){
        strcpy( label ,parString );
    }
    data->low = data->gh.gmin / data->gh.scale;
    data->high = data->gh.gmax / data->gh.scale;
    getparfloat("high", &data->high);
    getparfloat("pclip", &data->high);
    getparfloat("clip", &data->high);
    getparfloat("low", &data->low);
    getparfloat("nclip", &data->low);
    data->axis[DATA_VALUE] = AxisInit2(DATA_VALUE, 1, label,
                                       DATA_VALUE_SIZE, data->low,
                                       (data->high -
                                        data->low) / (DATA_VALUE_SIZE - 1), 1.);
    data->esize = irint(data->gh.dtype / data->gh.scale);
    data->size = n1 * n2 * n3 * n4 * n5;
    data->max = data->high;
    data->min = data->low;
    return (1);
}

/* decide which data reading routine to use */
void DataLoad(void)
{
    extern Data data;

    switch (data->esize ){
       case 4:
        DataLoadFloat(data);
        break;
       case 1:
        DataLoadByte(data);
        break;
    }
    UIMessage("data loaded");
}

/* load byte data; compress to 7 bits- 0-128 */
void DataLoadByte(Data data)
{
    extern int infd;
    unsigned char  table[256];
    int      i;
    unsigned char* bp;
    unsigned char* be;

    int n;

/*--------------------------------------------------------------------*\
   Seek to the beginning of the file before trying to read.  While
   the previous code worked under Solaris 2.6, under Linux the read(2)
   of byte data failed.  Apparently because of differing filesystem
   semantics elsewhere in the code.
\*--------------------------------------------------------------------*/

    n=lseek64( infd ,(long long)0 ,SEEK_SET );

    if( n < 0 ){
       perror("DataLoadByte lseek64() " );
    }

    UIMessage("loading byte data ...");

    if( data->hbytes ){
        read(infd, data->buffer, data->hbytes);

#ifdef DBG_HDR
       {  FILE* fp;
     
          if( (fp = fopen( "header.dat" ,"w" ) ) ){
             fwrite( data->buffer ,1 ,data->hbytes ,fp );
             fclose( fp );
          }
       }
#endif
       
    }

    n=read(infd, data->buffer, data->size);

    if( n == 0 ){
       perror("DataLoadByte read() " );

    }

#ifdef DBG_DAT
    {  FILE* fp;
     
       if( (fp = fopen( "data.dat" ,"w" ) ) ){
          fwrite( data->buffer ,1 ,data->size ,fp );
          fclose( fp );
       }
    }
#endif
       

    for( i = 0; i < 256; i++ ){
        table[i] = i / 2;
    }

    table[1] = 1;

    for( bp = data->buffer, be = bp + data->size; bp < be; bp++ ){
        *bp = table[*bp];

    }

    DataComputeHistogram(data);

}

void DataLoadFloat(Data data)
{
    int      i1, i2, i3, n1, n2, n3, size, head;
    float   *buf1, *buf2, DataCent(float *x, int n, float p), *trace, *tgain;
    float scale, bias, *bp1, *bp2, *fp, *fe, *gp;
    int datum;
    byte dp;
    extern int infd;
    int n;

    if( data->segy ){
        UIMessage("loading segy data ...");

    } else {
        UIMessage("loading float data ...");
    }

    n1 = AxisSize(data->axis[DATA_AXIS1]);
    n2 = AxisSize(data->axis[DATA_AXIS2]);
    n3 = AxisSize(data->axis[DATA_AXIS3])
        * AxisSize(data->axis[DATA_AXIS4])
        * AxisSize(data->axis[DATA_AXIS5]);

    if( data->segy ){
        head = 60;

    } else {
        head = 0;
    }
/*--------------------------------------------------------------------*\
   Allocate a set of work buffers which will be freed before leaving
   this function.  This was a pseudo C++ NEW in original code using
   C preprocessor.  It was made explicit so that a debugger can
   actually stop within the code to check it.
\*--------------------------------------------------------------------*/

    {   extern int _alloc;

        buf1 = (float *) malloc((size_t) (n1 * n2) * sizeof(buf1[0]));
        _alloc += (n1 * n2) * sizeof(buf1[0]);
        if( buf1 == 0 ){
            err("cant allocate %d bytes for  buf1; %d already allocated",
                (n1 * n2) * sizeof(buf1[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " buf1",
                          (n1 * n2) * sizeof(buf1[0]));
        };
    }

    {   extern int _alloc;

        buf2 = (float *) malloc((size_t) (n1 * n2) * sizeof(buf2[0]));
        _alloc += (n1 * n2) * sizeof(buf2[0]);
        if( buf2 == 0 ){
            err("cant allocate %d bytes for  buf2; %d already allocated",
                (n1 * n2) * sizeof(buf2[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " buf2",
                          (n1 * n2) * sizeof(buf2[0]));
        }
     }

     {  extern int _alloc;

        tgain = (float *) malloc((size_t) (n1) * sizeof(tgain[0]));
        _alloc += (n1) * sizeof(tgain[0]);
        if( tgain == 0 ){
            err("cant allocate %d bytes for  tgain; %d already allocated",
                (n1) * sizeof(tgain[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " tgain", (n1) * sizeof(tgain[0]));
        }
     }

    size = (n1 + head) > DATA_HEAD1 ? (n1 + head) : DATA_HEAD1;
    size = size > data->hbytes ? size : data->hbytes;

    {   extern int _alloc;

        trace = (float *) malloc((size_t) (size) * sizeof(trace[0]));
        _alloc += (size) * sizeof(trace[0]);
        if( trace == 0 ){
            err("cant allocate %d bytes for  trace; %d already allocated",
                (size) * sizeof(trace[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " trace",
                          (size) * sizeof(trace[0]));
        }
     }

    bp1 = buf1;
    bp2 = buf2;

    /*------------------*/
    /* time gain vector */
    /*------------------*/

    for( i1 = 0; i1 < n1 - 1; i1++ ){
        tgain[i1] = AxisValue(data->axis[DATA_AXIS1], i1 + 1);
        tgain[i1] = tgain[i1] != 0.0 ? pow(tgain[i1], data->tpow) : 1.0;
    }
    tgain[n1 - 1] = tgain[n1 - 2];

    /*---------------------------------------------------*/
    /* remember first panel in buf1; copy for percentile */
    /*---------------------------------------------------*/

    n=lseek64( infd ,0L ,0 );

    if( data->segy ){
        read(infd, trace, DATA_HEAD0 * sizeof(trace[0]));
        read(infd, trace, DATA_HEAD1 * sizeof(trace[0]));
    } else if( data->hbytes ){
        read(infd, trace, data->hbytes);
    }

/*--------------------------------------------------------------------*\
   Read the data from disk and apply time variant gain if requested.
   Make a 2nd copy to pas to the histogram routine.
\*--------------------------------------------------------------------*/

    for( i2 = 0; i2 < n2; i2++ ){

        n=read(infd, trace, (n1 + head) * sizeof(trace[0]));

        for( fp = trace + head, fe = fp + n1, gp = tgain; fp < fe;
             fp++, bp1++, bp2++, gp++ ){
            *bp1 = *fp * *gp;
            *bp2 = *bp1;
        }
    }
/*--------------------------------------------------------------------*\
   Not clear what this is all about.  The call to free() is especially
   mysterious.

   DataCent calculates the values at the desired percentiles of the
   input data ranges.
\*--------------------------------------------------------------------*/

    if( data->low == DATA_LOW && data->high == DATA_HIGH ){
        string   label;

        data->min = DataCent(buf2, n1 * n2, DATA_CENT_MIN);
        data->low = DataCent(buf2, n1 * n2, DATA_CENT_LOW);
        data->high = DataCent(buf2, n1 * n2, DATA_CENT_HIGH);
        data->max = DataCent(buf2, n1 * n2, DATA_CENT_MAX);
        strcpy(label, AxisLabel(data->axis[DATA_VALUE]));

        if( data->axis[DATA_VALUE] ){
            free(data->axis[DATA_VALUE]);
            data->axis[DATA_VALUE] = 0;
            if( memwatch ){
                printf("free %s\n", "data->axis[DATA_VALUE]");
            }
        }

        data->axis[DATA_VALUE] =
            AxisInit2(DATA_VALUE, 1, label, DATA_VALUE_SIZE + 1, data->low,
                      (data->high - data->low) / DATA_VALUE_SIZE, 1.0);
    }

    if( data->low == 0.0 && data->high == 0. ){
        err("first panel appears to be all zeros; set clip");
    }
    if( data->high > data->low ){
        bias = -data->low;
        scale = DATA_VALUE_SIZE / (data->high - data->low);
    } else {
        bias = 0.;
        scale = 0.5 * DATA_VALUE_SIZE / data->high;
    }

/*--------------------------------------------------------------------*\
   This appears to be where the input data actually gets scaled and 
   copied into the data buffer used to hold the byte volume.

   The first panel is processed, then the remaining panels are read
   and processed.
\*--------------------------------------------------------------------*/

    dp = data->buffer;

    for( fp = buf1, fe = buf1 + n1 * n2; fp < fe; fp++, dp++ ){
        datum = (*fp + bias) * scale;
        datum = datum > 0 ? datum : 0;
        datum = datum < DATA_VALUE_SIZE ? datum : DATA_VALUE_SIZE - 1;
        datum += DATA_VALUE_BASE;
        *dp = datum;
    }


    /*------------------------*/
    /* convert rest of traces */
    /*------------------------*/

    for( i3 = 1; i3 < n3; i3++ ){
        for( i2 = 0; i2 < n2; i2++ ){
            if( read(infd, trace, (n1 + head) * sizeof(trace[0])) < 0 ){
                break;
            }
            for( fp = trace + head, fe = fp + n1, gp = tgain; fp < fe;
                 fp++, dp++, gp++ ){
                datum = (*fp * *gp + bias) * scale;
                datum = datum > 0 ? datum : 0;
                datum = datum < DATA_VALUE_SIZE ? datum : DATA_VALUE_SIZE - 1;
                datum += DATA_VALUE_BASE;
                *dp = datum;
            }
        }
    }

    DataComputeHistogram(data);

/*--------------------------------------------------------------------*\
   Free work buffers
\*--------------------------------------------------------------------*/

    if( buf1 ){
        free(buf1);
        buf1 = 0;
        if( memwatch ){
            printf("free %s\n", "buf1");
        }
    }

    if( buf2 ){
        free(buf2);
        buf2 = 0;
        if( memwatch ){
            printf("free %s\n", "buf2");
        }
    }

    if( trace ){
        free(trace);
        trace = 0;
        if( memwatch ){
            printf("free %s\n", "trace");
        }
    }

    if( tgain ){
        free(tgain);
        tgain = 0;
        if( memwatch ){
            printf("free %s\n", "tgain");
        }
    }
}

/*--------------------------------------------------------------------*\
   percentile subroutine based on Canales, SEP-10

    p - percentile <0.,99.999999999999>
    x - data
    n - vector length

    this routine changes data order, so sort a copy
\*--------------------------------------------------------------------*/

static int DataCentCmp( float* e1 ,float* e2 ){

   if( *e1 < *e2 ){
      return -1;
   }else if( *e1 > *e2 ){
      return 1;
   }else{
      return 0;
   }

}

float    DataCent(float *x, int n, float p)
{
    int      q;

    q = 0.01*p*n;

    if( q == n ){
       q--;
    }

    qsort( x ,n ,sizeof(float) ,DataCentCmp );

    return( x[q] );
}

/* return long data name */
char    *DataLabel(Data data)
{
    static Message message;

    if( !data ){
        return (0);
    }
    sprintf(message, "%s: %dx%dx%dx%dx%d=%d samples ", data->title,
            AxisSize(data->axis[DATA_AXIS1]),
            AxisSize(data->axis[DATA_AXIS2]),
            AxisSize(data->axis[DATA_AXIS3]),
            AxisSize(data->axis[DATA_AXIS4]),
            AxisSize(data->axis[DATA_AXIS5]), data->size);
    return (message);
}

/* return short data name */
char    *DataTitle(Data data)
{
    if( !data ){
        return (0);
    }
    return (data->title);
}

/* return data file */
char    *DataFile(Data data)
{
    if( !data ){
        return (0);
    }
    return (data->file);
}

/* return short name of dataset from filename */
char    *DataShortName(Data data)
{
    static string name;
    char    *c;

    /* truncate leading pathname */
    if( (c = strrchr(data->file, '/')) != NULL ){
        strcpy(name, c + 1);
    } else {
        strcpy(name, data->file);
    }
    /* truncate trailing tail */
    if( (c = strchr(name, '.')) != NULL ){
        *c = '\0';
    }
    return (name);
}

/* return data buffer */
Buffer   DataBuffer(Data data)
{
    if( !data ){
        return (0);
    }
    return (data->buffer);
}

/* return axis reference */
Axis     DataAxis(Data data, int iaxis)
{
    if( !data || iaxis > DATA_NAXIS ){
        return (0);
    }
    return (data->axis[iaxis]);
}

/* return data size */
int DataSize(Data data)
{
    return (data->size);
}

/* return longest dimension */
int DataMaxDim(Data data)
{
    int      max, iaxis;

    if( !data ){
        return (0);
    }
    for( iaxis = 0, max = 1; iaxis < DATA_NAXIS - 1; iaxis++ ){
        max =
            AxisSize(data->axis[iaxis + 1]) >
            max ? AxisSize(data->axis[iaxis + 1]) : max;
    }
    return (max);
}

/* return data amplitude */
float    DataValue(Data data, int value)
{
    return (data->low +
            ((data->high - data->low) * ((value & 0x7F) - data->value_base)) /
            DATA_VALUE_SIZE);
}

/* return data value size */
int DataValueSize(Data data)
{
    if( !data ){
        return (0);
    } else {
        return (data->value_size);
    }
}

/* return base data value */
int DataValueBase(Data data)
{
    if( !data ){
        return (0);
    } else {
        return (data->value_base);
    }
}

/* return data high clip */
float    DataHigh(void)
{
    extern Data data;

    if( !data ){
        return (0.0);
    }
    return (data->high);
}

/* return data low clip */
float    DataLow(void)
{
    extern Data data;

    if( !data ){
        return (0.0);
    }
    return (data->low);
}

/* return integer percentage of histogram */
int DataHistogram(int i)
{
    extern Data data;

    if( !data || i < 0 || i > 255 ){
        return (0);
    }
    return ((int) (100 * data->histogram[i]));
}

void DataComputeHistogram(Data data)
{
    register Buffer bp, be;
    int      i;
    float    max;

    /*----------------------------*/
    /* clear the histogram buffer */

    /*----------------------------*/

    for( i = 0; i < 256; i++ ){
        data->histogram[i] = 0;
    }

    /*------------------------------*/
    /* calculate total for each bin */

    /*------------------------------*/

    for( bp = data->buffer, be = bp + data->size; bp < be; bp++ ){

        data->histogram[*bp] += 1.0;
    }

    /*------------------------------*/
    /* rescale totals to percentage */

    /*------------------------------*/

    max = 0;

    for( i = 0; i < ColorSize(); i++ ){
        max = data->histogram[i] > max ? data->histogram[i] : max;
    }

    for( i = 0; i < ColorSize(); i++ ){
        data->histogram[i] /= max;
    }
}

/* print data information */
void DataInfo(Data data)
{
    Message  message;

    sprintf(message,
            "Data: %s: in=%s %dx%dx%dx%dx%d=%d esize=%d segy=%d hbytes=%d script=%s",
            data->title, data->file, AxisSize(data->axis[DATA_AXIS1]),
            AxisSize(data->axis[DATA_AXIS2]), AxisSize(data->axis[DATA_AXIS3]),
            AxisSize(data->axis[DATA_AXIS4]), AxisSize(data->axis[DATA_AXIS5]),
            data->size, data->esize, data->segy, data->hbytes, data->script);
    UIMessage(message);
}

/* data value parameters */
void DataValueInfo(Data data)
{
    Message  message;

    sprintf(message,
            "Value: min=%g low=%g high=%g max=%g tpow=%g gpow=%g base=%d size=%d",
            data->min, data->low, data->high, data->max, data->tpow, data->gpow,
            data->value_base, data->value_size);
    UIMessage(message);
}

/* save data parameters */
void DataSavePar(Data data)
{
    int      iaxis;
    Message  message;

    if( !data ){
        return;
    }
    sprintf(message,
            "Data: %s: in=%s %dx%dx%dx%dx%d=%d esize=%d segy=%d hbytes=%d script=%s",
            data->title, data->file, AxisSize(data->axis[DATA_AXIS1]),
            AxisSize(data->axis[DATA_AXIS2]), AxisSize(data->axis[DATA_AXIS3]),
            AxisSize(data->axis[DATA_AXIS4]), AxisSize(data->axis[DATA_AXIS5]),
            data->size, data->esize, data->segy, data->hbytes, data->script);
    UISaveMessage(message);
    sprintf(message,
            "Value: min=%g low=%g high=%g max=%g tpow=%g gpow=%g base=%d size=%d",
            data->min, data->low, data->high, data->max, data->tpow, data->gpow,
            data->value_base, data->value_size);
    UISaveMessage(message);
    for( iaxis = DATA_AXIS1; iaxis <= DATA_AXIS5; iaxis++ ){
        AxisSavePar(data->axis[iaxis]);
    }
}

/* dump data bytes */
void DataDumpBytes(Data data, char *file, int fd)
{
    register byte bp, be;

    DrawWatch(1);
    for( bp = data->buffer, be = bp + data->size; bp < be; ){
        *bp++ <<= 1;
    }
    write(fd, data->buffer, data->size);
    for( bp = data->buffer, be = bp + data->size; bp < be; ){
        *bp++ >>= 1;
    }
    DataDumpHeader(data, file, fd, 1);
    DrawWatch(0);
}

/* dump data vgrid header */
void DataDumpHeader(Data data, char *file, int datafd, int esize)
{
    Message  message;
    char     parfile[80];
    extern FILE *savefd;

    if( data->gh.dtype == 0. ){ /* build a header */
        data->gh.scale = 1.;
        data->gh.n1 = AxisSize(data->axis[DATA_AXIS1]) * data->gh.scale;
        data->gh.n2 = AxisSize(data->axis[DATA_AXIS2]) * data->gh.scale;
        data->gh.n3 = AxisSize(data->axis[DATA_AXIS3]) * data->gh.scale;
        data->gh.n4 = AxisSize(data->axis[DATA_AXIS4]) * data->gh.scale;
        data->gh.n5 = AxisSize(data->axis[DATA_AXIS5]) * data->gh.scale;
        data->gh.d1 = AxisDelta(data->axis[DATA_AXIS1]) * data->gh.scale;
        data->gh.d2 = AxisDelta(data->axis[DATA_AXIS2]) * data->gh.scale;
        data->gh.d3 = AxisDelta(data->axis[DATA_AXIS3]) * data->gh.scale;
        data->gh.d4 = AxisDelta(data->axis[DATA_AXIS4]) * data->gh.scale;
        data->gh.d5 = AxisDelta(data->axis[DATA_AXIS5]) * data->gh.scale;
        data->gh.o1 = AxisFirst(data->axis[DATA_AXIS1]) * data->gh.scale;
        data->gh.o2 = AxisFirst(data->axis[DATA_AXIS2]) * data->gh.scale;
        data->gh.o3 = AxisFirst(data->axis[DATA_AXIS3]) * data->gh.scale;
        data->gh.o4 = AxisFirst(data->axis[DATA_AXIS4]) * data->gh.scale;
        data->gh.o5 = AxisFirst(data->axis[DATA_AXIS5]) * data->gh.scale;
        data->gh.dcdp2 = 1.;
        data->gh.dline3 = 1.;
        data->gh.ocdp2 = 1.;
        data->gh.oline3 = 1.;
        data->gh.gmin = data->low * data->gh.scale;
        data->gh.gmax = data->high * data->gh.scale;
    }
    data->gh.dtype = esize * data->gh.scale;
    write(datafd, &data->gh, sizeof(data->gh));
    close(datafd);
    sprintf(parfile, "%s.H", file);
    if( (savefd = fopen(parfile, "w")) == NULL ){
       UIMessage("cant create vgrid parfile");
    }
    DataSavePar(data);
    sprintf(message, "Byte data dump into %s", parfile);
    UIMessage(message);
    fclose(savefd);
}

/* dump data floats */
void DataDumpFloats(Data data, char *file, int fd)
{
    int      nn, n3, i;
    Buffer   dbuf;
    float   *fbuf, min, max;

    DrawWatch(1);
    nn = AxisSize(data->axis[DATA_AXIS1]) * AxisSize(data->axis[DATA_AXIS2]);
    n3 = data->size / nn;

    {
        extern int _alloc;

        fbuf = (float *) malloc((size_t) (nn) * sizeof(fbuf[0]));
        _alloc += (nn) * sizeof(fbuf[0]);
        if( fbuf == 0 ){
            err("cant allocate %d bytes for  fbuf; %d already allocated",
                (nn) * sizeof(fbuf[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", " fbuf", (nn) * sizeof(fbuf[0]));
        }
    };

    /* convert and dump a plane at a time */
    min = data->high;
    max = data->low;
    for( i = 0, dbuf = data->buffer; i < n3; i++, dbuf += nn ){
        Data2Float(dbuf, fbuf, nn, &min, &max);
        write(fd, fbuf, nn * sizeof(fbuf[0]));
    }
    data->gh.gmin = min * data->gh.scale;
    data->gh.gmax = max * data->gh.scale;
    DataDumpHeader(data, file, fd, 4);

    if( fbuf ){
        free(fbuf);
        fbuf = 0;
        if( memwatch ){
            printf("free %s\n", "fbuf");
        }
    };
    DrawWatch(0);
}

void Data2Float(Buffer dbuf, float *fbuf, int n, float *min, float *max)
{
    register Buffer bp, be;
    register float *fp, *mp, Min, Max;
    extern Data data;

    Min = *min;
    Max = *max;
    /* convert */
    for( mp = AxisValues(data->axis[DATA_VALUE]) - DATA_VALUE_BASE, bp =
         dbuf, fp = fbuf, be = dbuf + n; bp < be; bp++, fp++ ){
        *fp = mp[*bp];
        Max = Max > *fp ? Max : *fp;
        Min = Min < *fp ? Min : *fp;
    }
    *min = Min;
    *max = Max;
}

int      irint(float x)
{
    int      i;

    i = x;
    return ((x - i) < (1 - (x - i)) ? i : i + 1);
}
