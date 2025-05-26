
#ifndef PIK_H
#define PIK_H

/*====================================================================*\
                          point pick object
\*====================================================================*/

#define NFILES          10
#define NPIK            300000
#define PIK_RANGE       1
#define PIK_SIZE        2
#define PIK_NEAR        3000
#define PIKOP_ADD       'a'
#define PIKOP_DELETE    'd'
#define PIKOP_MOVE      'm'

typedef short Pik[DATA_NAXIS];
typedef struct {
        Pik             *pik;           /* piks */
        string          file;           /* pik file name */
        int             npik;           /* number of piks */
        int             nmax;           /* maximum number of piks */
        int             range;          /* planes to display on */
        int             size;           /* display size in pixels */
        int             last_op;        /* last operation */
        int             last_item;      /* last item changed */
        int             changed;        /* pick set modified since read */
        } *PikList;

/*--------------------------------------------------------------------*\
                       Function prototypes
\*--------------------------------------------------------------------*/

/* pik.c */
void PikInit(void);
void PikClear(void);
void PikRead(void);
void PikWrite(char *filename, FILE *fd);
void PikDraw(int pik0, int draw);
void PikDrawEx(int x, int y, int draw);
void PikDrawBox(int x, int y, int size, int draw);
int PikNear(int x, int y);
int PikCoord(int ipik, Plane plane, int *x, int *y);
void PikQuery(int x, int y);
void PikAdd(int x, int y);
void PikEdge(int x, int y);
void PikDelete(int x, int y);
void PikMove(int x, int y);
void PikMoveEdge(int x, int y);
void PikUndo(void);
void PikInfo(void);
void PikSave(void);
int PikIncreaseSize(void);
int PikDecreaseSize(void);
int PikIncreaseRange(void);
int PikDecreaseRange(void);
#endif
