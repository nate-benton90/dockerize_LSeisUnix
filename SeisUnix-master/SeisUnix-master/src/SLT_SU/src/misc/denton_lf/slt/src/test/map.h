
#ifndef MAP_H
#define MAP_H

/*
map axis

Remembers image size, windowing, frame selection and orientation
Uses axis definition
*/

#include "axis.h"

/* constants */

/* best fit tic spacing to pixels */
#define MAP_INTERP      4096
#define MAP_TIC 100

/* mapping index of data to and from image */
typedef unsigned int *Vec;

/* Map object */
typedef struct {
    Axis     axis;              /* current data axis */
    string   name;              /* name of map axis */
    int      size;              /* pixels long */
    int      first;             /* minimum data axis sample */
    int      last;              /* maximum data axis sample; can be less than 
                                   first */
    float    dtic;              /* tic spacing in axis value */
    float    tic0;              /* mimimum tic value */
    float    tic2;              /* maximum tic value */
    int      ntic;              /* number of tics */
    string   format;            /* printf format of tic label */
    int      frame1;            /* animation start */
    int      frame2;            /* animation end */
    int      dframe;            /* animation incrment */
    int      frame;             /* current animation frame */
    int      imap;              /* current map position */
    int      prev;              /* previous animation frame */
    int      save;              /* save frame */
    Vec      map;               /* data sample for each display sample */
    Vec      inv;               /* display sample for data sample */
    Vec      interp;            /* 16 bit interption for smooth mapping */
}       *Map, Map_;

/* map.c */
Map MapInit(Axis axis, char *name, int imap);
void MapSet(Map map, Axis axis, int size, int first, int last, int frame1, int frame2, int dframe);
int MapBound(int index, int bound1, int bound2);
void MapSetSize(Map map, int size);
void MapSetTics(Map map);
void MapSwap(Map a, Map b);
void MapFlip(Map map);
Axis MapAxis(Map map);
char *MapName(Map map);
Vec MapVec(Map map);
Vec MapInterp(Map map);
int MapSize(Map map);
int MapWindow(Map map);
int MapZoom(Map map);
int MapFirst(Map map);
int MapLast(Map map);
int MapLow(Map map);
int MapHigh(Map map);
int MapFrame(Map map);
int MapFrame1(Map map);
int MapNFrame(Map map);
int MapPrev(Map map);
int MapSave(Map map);
float MapTic0(Map map);
float MapTic2(Map map);
int MapNtic(Map map);
float MapDtic(Map map);
int MapMovie1(Map map);
int MapMovie2(Map map);
int MapDmovie(Map map);
char *MapFormat(Map map);
void MapSetFrame(Map map, int frame);
void MapSetFrameBounds(Map map, int frame1, int frame2);
void MapSetDmovie(Map map, int dframe);
void MapSetFrame1(Map map, int frame);
void MapNextFrame(Map map);
float MapValue(Map map, int index);
int MapIndex(Map map, float value);
int MapMap(Map map, int index);
int MapInverse(Map map, int index);
void MapInfo(Map map);
void MapSavePar(Map map);
void MapDump(Map map);
void MapSaveFrame(Map map);
void MapRestoreFrame(Map map);
#endif
