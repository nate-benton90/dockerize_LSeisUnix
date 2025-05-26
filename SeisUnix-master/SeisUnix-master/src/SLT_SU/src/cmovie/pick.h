
#ifndef PICK_H
#define PICK_H

/*--------------------------------------------------------------------*\
   pick object definition A pick object contains pick flags and the
   picks.
\*--------------------------------------------------------------------*/


#ifdef __convex__
#define NPICK       300
#define PICKNLIST   2000
#endif
#ifndef  __convex__
#define NPICK       200
#define PICKNLIST   500
#endif

/*-------------*/
/* Pick object */
/*-------------*/

typedef struct {
    int      iaxis[DATA_NAXIS];         /* orientation */
    int      index[DATA_NAXIS];         /* sample indices */
}       *PickPoint, PickPoint_;

/*---------------*/
/* line of picks */
/*---------------*/

typedef struct {
    int      iaxis[DATA_NAXIS];         /* orientation */
    int      npick;                     /* number of picks */
    int      index[NPICK][DATA_NAXIS];  /* data coordinates */
}       *PickLine;

/*------------*/
/* Prototypes */
/*------------*/

extern PickLine PickFind(
     int dir3
    ,int frame3
    ,int dir4
    ,int frame4
    ,int dir5
    ,int frame5
);

int PickAxis( void  );
int PickBetween( int *a ,int *b ,int *c  );
int PickCount( PickLine pickline  );
int PickDir( PickLine pickline  );
int PickFrame( int ipick  );
int PickIndex( PickLine pickline ,int ipick ,int idim  );
int PickSameDir( PickPoint pick1 ,PickPoint pick2  );
int PickSameFrame( PickPoint pick1 ,PickPoint pick2  );
int PickSharedDir( PickPoint pick1 ,PickPoint pick2  );
int PickSize( void  );

void PickAdd( int x ,int y  );
void PickClear( PickLine pickline  );
void PickClear0( void  );
void PickDecode(  int x ,int y ,PickPoint pick ,int print  );
void PickDecodeShadow( Shadow_ shadow ,int *index  );
void PickDelete( int x ,int y  );
void PickDraw( PickLine pickline ,int draw  );
void PickDrawAll( int draw  );
void PickInfo( void  );
void PickInit( void );
void PickInsert( int x ,int y  );
void PickListInfo( void  );
void PickRead( void  );
void PickReplace( int x ,int y  );
void PickSavePar( void  );
void PickSetFrame( int index  );
void PickWrite( void  );

#endif
