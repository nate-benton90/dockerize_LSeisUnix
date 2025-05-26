/*
axis object definition

Three axes define a cube dataset
*/


#ifndef AXIS_H
#define AXIS_H

#include "grunge.h"

/* Axis object */
typedef struct {
    string   label;             /* name */
    int      size;              /* length */
    float    first;             /* first value */
    float    delta;             /* sampling rate */
    float    zoom;              /* relative zoom */
    int      dir;               /* direction */
    int      stride;            /* array stride */
    int      prec;              /* slider precision */
    int      scale;             /* slider scaling: 10**scale */
    float   *values;            /* tabulated values */
    string  *script;            /* label for each element */
}       *Axis;


/* axis.c */
Axis AxisInit(int iaxis, int stride);
Axis AxisInit2(int dir, int stride, char *label, int size, float first, float delta, float zoom);
void AxisSetScale(Axis axis);
int AxisPrec(Axis axis);
int AxisScale(Axis axis);
void AxisSetValues(Axis axis);
int AxisSize(Axis axis);
float AxisDelta(Axis axis);
float AxisFirst(Axis axis);
float AxisLast(Axis axis);
float AxisZoom(Axis axis);
char *AxisLabel(Axis axis);
int AxisStride(Axis axis);
float AxisValue(Axis axis, int index);
int AxisScaledValue(Axis axis, int index);
float *AxisValues(Axis axis);
int AxisIndex(Axis axis, float value);
int AxisScaledIndex(Axis axis, int value);
int AxisDir(Axis axis);
char *AxisScript(Axis axis, int index);
void AxisInfo(Axis axis);
void AxisSavePar(Axis axis);
#endif
