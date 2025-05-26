/*--------------------------------------------------------------------*\
                             axis object code
\*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "par.h"

#include "ui_menu.h"
#include "ui_window.h"
#include "main.h"
#include "axis.h"

/*--------------------------------------------------------------------*\
   axis init from mandatory getpar n1=,o1=,d1=,label1= are length,
   origin ,sampling ,and name of first axis ,etc.
\*--------------------------------------------------------------------*/
Axis AxisInit(int iaxis ,int stride)
{
    Axis     axis;
    string   par;
    cwp_String label;

    {   extern int _alloc;

        axis = (Axis) malloc(sizeof(axis[0]));
        _alloc += (1) * sizeof(axis);
        if( axis == 0 ){
            err("cant allocate %d bytes for  axis; %d already allocated",
                sizeof(axis[0]) ,_alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n" ," axis" ,sizeof(axis[0]));
        }
    }

    axis->dir = iaxis;
    axis->stride = stride > 0 ? stride : 1;

    (void)sprintf(par ,"label%d" ,iaxis);
    if( getparstring (par ,&label) != 0 ){
        (void)strcpy(axis->label ,label );
    }else{
        (void)sprintf(axis->label ,"n%d" ,iaxis);
    }
        

    (void)sprintf(par ,"n%d" ,iaxis);
    if( getparint (par ,&axis->size) == 0 ){
        err("%s missing\n" ,par);
    }

    (void)sprintf(par ,"o%d" ,iaxis);
    if( getparfloat (par ,&axis->first) == 0 ){
        axis->first = 0.;
    }

    (void)sprintf(par ,"d%d" ,iaxis);
    if( getparfloat (par ,&axis->delta) == 0 ){
        axis->delta = 1.;
    }

    (void)sprintf(par ,"z%d" ,iaxis);
    if( getparfloat (par ,&axis->zoom) == 0 ){
        axis->zoom = 1.;
    }

    {   extern int _alloc;

        axis->values = (float *) malloc((axis->size) * sizeof(axis->values[0]));
        _alloc += (axis->size) * sizeof(axis->values);
        if( axis->values == 0 ){
            err("cant allocate %d bytes for  axis->values; %d already allocated",
                (axis->size) * sizeof(axis->values) ,_alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n" ," axis->values",
                          (axis->size) * sizeof(axis->values));
        }
    }

    {   extern int _alloc;

        axis->script = (string *) malloc((axis->size) * sizeof(axis->script[0]));
        _alloc += (axis->size) * sizeof(axis->script);
        if( axis->script == 0 ){
            err("cant allocate %d bytes for  axis->script; %d already allocated",
                (axis->size) * sizeof(axis->script) ,_alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n" ," axis->script",
                          (axis->size) * sizeof(axis->script));
        }
    }
    AxisSetScale(axis);
    AxisSetValues(axis);
    return (axis);
}

/* axis init from arguments ,superceded by getpar */
Axis AxisInit2(int dir 
              ,int stride 
              ,char *label 
              ,int size 
              ,float first 
              ,float delta
              ,float zoom ){

    Axis     axis;
    string   par;
    cwp_String cwp_label;

    {   extern int _alloc;

        axis = (Axis)malloc(sizeof(axis[0]) );

        _alloc += (1) * sizeof(axis[0]);
        if( axis == 0 ){
            err("cant allocate %d bytes for  axis; %d already allocated",
                (1) * sizeof(axis[0]) ,_alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n" ," axis" ,(1) * sizeof(axis[0]));
        }
    } 
    axis->dir = dir;
    axis->stride = stride > 0 ? stride : 1;

    (void)strcpy(axis->label ,label);
    (void)sprintf(par ,"label%d" ,dir);
    if( getparstring (par ,&cwp_label) != 0 ){
        (void)strcpy(axis->label ,cwp_label );
    }

    axis->size = size;
    (void)sprintf(par ,"n%d" ,dir);
    getparint (par ,&axis->size);

    axis->first = first;
    (void)sprintf(par ,"o%d" ,dir);
    getparfloat (par ,&axis->first);

    axis->delta = delta;
    (void)sprintf(par ,"d%d" ,dir);
    getparfloat (par ,&axis->delta);
    if( axis->delta == 0.0 ){
        axis->delta = 1.0;
    }

    axis->zoom = zoom;
    (void)sprintf(par ,"z%d" ,dir);
    getparfloat (par ,&axis->zoom);

    {   extern int _alloc;

        axis->script = (string *) malloc((axis->size) * sizeof(axis->script[0]));
        _alloc += (axis->size) * sizeof(axis->script[0]);
        if( axis->script == 0 ){
            err("cant allocate %d bytes for  axis->script; %d already allocated",
                (axis->size) * sizeof(axis->script[0]) ,_alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n" ," axis->script",
                          (axis->size) * sizeof(axis->script[0]));
        }
    
        axis->values = (float *) malloc((axis->size) * sizeof(axis->values[0]));
        _alloc += (axis->size) * sizeof(axis->values[0]);
        if( axis->values == 0 ){
            err("cant allocate %d bytes for  axis->values; %d already allocated",
                (axis->size) * sizeof(axis->values[0]) ,_alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n" ," axis->values",
                          (axis->size) * sizeof(axis->values[0]));
        }
    }
    
    AxisSetScale(axis);
    AxisSetValues(axis);
    return (axis);
}

/* compute axis scale */
void AxisSetScale(Axis axis)
{
    int      i;
    char     n[20];

    (void)sprintf(n ,"%g" ,axis->delta - (int) axis->delta);
    axis->prec = strlen(n) - 2;
    axis->prec = axis->prec > 0 ? axis->prec : 0;
    axis->prec = axis->prec < 10 ? axis->prec : 10;
    for( axis->scale = 1 ,i = 0; i < axis->prec; i++ ){
        axis->scale *= 10;
    }
}

/* return axis prec */
int AxisPrec(Axis axis)
{
    if( !axis ){
        return (0);
    }else{
        return (axis->prec);
    }
}

/* return axis scale */
int AxisScale(Axis axis)
{
    if( !axis ){
        return (0);
    }else{
        return (axis->scale);
    }
}

/* build a list of values for the axis */
void AxisSetValues(Axis axis)
{
    int      i;

    if( !axis ){
        return;
    }

    for( i = 0; i < axis->size; i++ ){
        axis->values[i] = axis->first + i * axis->delta;
        (void)sprintf(axis->script[i] ,"%s=%g" ,axis->label ,axis->values[i]);
    }
}

/* return sample size of axis */
int AxisSize(Axis axis)
{
    if( !axis ){
        return (0);
    }
    return (axis->size);
}

/* return axis delta */
float    AxisDelta(Axis axis)
{
    if( !axis ){
        return (0.0);
    }
    return (axis->delta);
}

/* return axis first value */
float    AxisFirst(Axis axis)
{
    if( !axis ){
        return (0.0);
    }
    return (axis->first);
}

/* return last value */
float    AxisLast(Axis axis)
{
    if( !axis ){
        return (0.0);
    }
    return (axis->first + (axis->size - 1) * axis->delta);
}

/* return axis zoom */
float    AxisZoom(Axis axis)
{
    if( !axis ){
        return (0.0);
    }
    return (axis->zoom);
}

/* return name of axis */
char    *AxisLabel(Axis axis)
{
    if( !axis ){
        return (0);
    }
    return (axis->label);
}

/* return stride of axis */
int AxisStride(Axis axis)
{
    if( !axis ){
        return (0);
    }
    return (axis->stride);
}

/* return value of axis at given index */
float    AxisValue(Axis axis ,int index)
{
    if( !axis || index < 0 || index >= axis->size ){
        return (0.0);
    }else{
        return (axis->values[index]);
    }
}

/* return scaled value */
int AxisScaledValue(Axis axis ,int index)
{
    if( !axis ){
        return (0);
    }
    if( index < 0 || index >= axis->size ){
        return (0);
    }
    return ((int) (axis->scale * AxisValue(axis ,index)));
}

/* return array of axis values */
float   *AxisValues(Axis axis)
{
    if( !axis ){
        return (0);
    }
    return (axis->values);
}

/* return index given value */
int AxisIndex(Axis axis ,float value)
{
    int      index;

    index = (value - axis->first) / axis->delta;
    if( index < 0 || index >= axis->size ){
        return (NO_INDEX);
    }else{
        return (index);
    }
}

/* return scaled index */
int AxisScaledIndex(Axis axis ,int value)
{
    float    fvalue;

    if( !axis ){
        return (NO_INDEX);
    }
    fvalue = value;
    fvalue = fvalue / axis->scale;
    return (AxisIndex(axis ,fvalue));
}


/* return axis direction */
int AxisDir(Axis axis)
{
    if( !axis ){
        return (NO_INDEX);
    }else{
        return (axis->dir);
    }
}

/* return axis script */
char    *AxisScript(Axis axis ,int index)
{
    if( !axis || index < 0 || index >= axis->size ){
        return ("");
    }else{
        return (axis->script[index]);
    }
}

/* print info about axis */
void AxisInfo(Axis axis)
{
    Message  message;

    if( !axis ){
        return;
    }
    (void)sprintf(message,
            "Axis: label%d=%s n%d=%d o%d=%g d%d=%g z%d=%g prec=%d scale=%d",
            axis->dir ,axis->label ,axis->dir ,axis->size ,axis->dir,
            axis->first ,axis->dir ,axis->delta ,axis->dir ,axis->zoom,
            axis->prec ,axis->scale);
    UIMessage(message);
}

/* save axis parameters */
void AxisSavePar(Axis axis)
{
    Message  message;

    if( !axis ){
        return;
    }
    (void)sprintf(message ,"Axis%d: label%d=\"%s\" n%d=%d o%d=%g d%d=%g z%d=%g",
            axis->dir,
            axis->dir ,axis->label,
            axis->dir ,axis->size,
            axis->dir ,axis->first,
            axis->dir ,axis->delta ,axis->dir ,axis->zoom);
    UISaveMessage(message);
}
