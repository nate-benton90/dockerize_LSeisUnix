
#ifndef REGION_H
#define REGION_H
/*
region picking object
*/
#define MARK(cell)      ((cell) |= 0x80 ,mark_count++ )
#define UNMARK(cell)    ((cell) &= 0x7F ,mark_count-- )
#define ISMARK(cell)    ((cell) &  0x80 )
#define NOTMARK(cell)   (!(((cell) & 0x80)))
#define INRANGE(cell)   (NOTMARK(cell) && (cell)>=min && (cell)<=max)
#define OUTRANGE(cell)  (((cell)&0x7F)<min || ((cell)&0x7F)>max)
#define MARK_FACE       1
#define MARK_EDGE       2
#define MARK_CORNER     4
#define REGION_NLIST    100000

typedef struct {
    int      live;              /* regions enabled */
    int      seed[4];           /* seed index */
    int      value;             /* data value at seed */
    int      bound[2];          /* range bounds */
    int      neighborhood;      /* neighborhood mode: face, edge, corner */
    Shadow   list;              /* list of marked points */
    Shadow   index;             /* list of saved point indexes */
    Buffer   save;              /* value of marked points */
    int      nlist;             /* list size */
    int      size;              /* count of marked points */
}       *REgion;

extern REgion region;

/* Region used by X11/Intrinsic.h */

/* region.c */
void RegionInit(void);
void RegionSetLive(int live);
void RegionSetBound(int index, int ibound);
void RegionSetNeighborhood(int mode);
void RegionSetSeed(int *seed);
int RegionBound(int ibound);
int RegionSize(void);
void RegionMark0(void);
void RegionFill(int *seed, int bound1, int bound2, int value);
void RegionClear(void);
void RegionSetValue(int bound1, int bound2);
void RegionRestoreValue(void);
void RegionInfo(void);
int RegionNeighbors(void);
void RegionSavePar(void);
int RegionMark(Buffer data, int size1, int size2, int size3, int seed1, int seed2, int seed3, int min, int max, int neighborhood, Shadow list, int nlist);
void RegionMarkBorder(Buffer data, int size1, int size2, int size3);
void RegionUnMarkBorder(Buffer data, int size1, int size2, int size3, int Min, int Max);
void RegionDump(void);

#endif
