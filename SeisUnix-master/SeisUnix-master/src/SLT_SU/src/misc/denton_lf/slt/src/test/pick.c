/*--------------------------------------------------------------------*\
   pick object code pick object contains screen pick, map axis, view
   configuration and picking
\*--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "par.h"

#include "main.h"
#include "axis.h"
#include "data.h"
#include "color.h"
#include "draw.h"
#include "map.h"
#include "render.h"
#include "plane.h"
#include "view.h"
#include "pick.h"
#include "ui_menu.h"
#include "ui_window.h"

PickLine picklist[PICKNLIST];
PickLine lastpick = 0;
string   pickfile = "stdio";

#define PICKDIR( pick,axis )      pick iaxis[axis]
#define PICKFRAME0( pick,axis )   pick index[0][PICKDIR( pick,axis )]
#define PICKFRAME( pick,axis )    pick index[PICKDIR( pick,axis )]

/*--------------------------------------------------------------------*\
   initialize pick object
\*--------------------------------------------------------------------*/

void     PickInit(void)
{

    int      i;
    static int first = 1;

    for( i = 0; i < PICKNLIST; i++ ){

        if( !first ){

            if( picklist[i] ){
                free(picklist[i]);
                picklist[i] = 0;
                if( memwatch ){
                    printf("free %s\n", " picklist[i] ");
                }
            };
        }

        picklist[i] = 0;
    }

    PickRead();
    lastpick = 0;

    if( !first ){
        ViewDrawAll();
    }

    first = 0;
}

/*--------------------------------------------------------------------*\
   decode an x,y by shadow lookup
\*--------------------------------------------------------------------*/

void     PickDecode(int x, int y, PickPoint pick, int print)
{

    extern Render render;
    extern View view;
    extern Data data;

    Buffer   buffer;
    Message  message;
    Shadow_  shadow1;
    Shadow_  shadow;

    int      diff;
    int      i;
    int      iaxis;
    int      idiff;
    int      index1[DATA_NAXIS];
    int      isame;
    int      jaxis;
    float    val;

    /*--------------*/
    /* no pick made */
    /*--------------*/

    if( !render || !view || !data ){
        return;
    }

    /*-----------------*/
    /* no invalid pick */
    /*-----------------*/

    for( iaxis = 0; iaxis < DATA_NAXIS; iaxis++ ){
        pick->iaxis[iaxis] = NO_INDEX;
        pick->index[iaxis] = NO_INDEX;
    }

/*--------------------------------------------------------------------*\
   decode directions; using neighboring horizontal & vertical pixels
\*--------------------------------------------------------------------*/

    shadow = RenderShadowValue(render, x, y);

    if( (int) shadow == NO_INDEX ){
        return;
    }

    PickDecodeShadow(shadow, pick->index);

    for( i = 1;; i++ ){
        shadow1 = RenderShadowValue(render, x + i, y + i);

        if( (int) shadow1 == NO_INDEX ){
            break;
        }

        if( shadow1 == shadow ){
            continue;
        }

        PickDecodeShadow(shadow1, index1);

        for( jaxis = 1, diff = 0; jaxis < DATA_NAXIS; jaxis++ ){
            diff += (index1[jaxis] != pick->index[jaxis]);
        }

        if( diff == 2 ){
            break;
        }
    }

    if( (int) shadow1 == NO_INDEX || shadow1 == shadow || diff < 2 ){
        for( i = -1;; i-- ){

            shadow1 = RenderShadowValue(render, x + i, y + i);

            if( (int) shadow1 == NO_INDEX ){
                break;
            }

            if( shadow1 == shadow ){
                continue;
            }

            PickDecodeShadow(shadow1, index1);

            for( jaxis = 1, diff = 0; jaxis < DATA_NAXIS; jaxis++ ){
                diff += (index1[jaxis] != pick->index[jaxis]);
            }

            if( diff == 2 ){
                break;
            }
        }
    }

    for( iaxis = 1, isame = diff + 1, idiff = 1; iaxis < DATA_NAXIS; iaxis++ ){
        if( pick->index[iaxis] != index1[iaxis] ){
            pick->iaxis[idiff++] = iaxis;
        } else {
            pick->iaxis[isame++] = iaxis;
        }

    }

    pick->iaxis[AXIS_COLOR] = DATA_VALUE;

/*------------------------------- dead code --------------------------*\

        if(  diff != 2 ){
                printf ( "%d %d %d %d\n",shadow,i,shadow1,diff );
                for(  i=0; i<DATA_NAXIS; i++ ) printf ( "%7d: ",i ); printf ( "\n" );
                for(  i=0; i<DATA_NAXIS; i++ ) printf ( "%8d ",pick->iaxis[i] ); printf ( "\n" );
                for(  i=0; i<DATA_NAXIS; i++ ) printf ( "%8d ",pick->index[i] ); printf ( "\n" );
                for(  i=0; i<DATA_NAXIS; i++ ) printf ( "%8d ",index1[i] ); printf ( "\n" );
                printf ( "\n" );
                return;
                }
\*------------------------------ dead code ---------------------------*/

    buffer = DataBuffer(data);

    if( data->overlay_mode ){
        pick->index[DATA_VALUE] = buffer[shadow] ;
        val = buffer[shadow]/irint(data->gh.d4/data->gh.scale); 
        val *= data->gh.dcdp2 / data->gh.scale;
        val += data->gh.o5 / data->gh.scale;

    }else{
        pick->index[DATA_VALUE] = buffer[shadow] ;
        val =  AxisValue(DataAxis(data, DATA_VALUE), pick->index[DATA_VALUE]);

    }

    /*-----------------------*/
    /* print pick in message */
    /*-----------------------*/

    if( print ){

        sprintf(message,
                "dir=%s frame=%d,%d,%d %s=%g %s=%g %s=%g %s=%g %s=%g %s=%g",
                AxisLabel(DataAxis(data, pick->iaxis[AXIS_DEEP])),
                pick->index[pick->iaxis[AXIS_DEEP]],
                pick->index[pick->iaxis[AXIS_4D]],
                pick->index[pick->iaxis[AXIS_5D]],
                AxisLabel(DataAxis(data, DATA_AXIS1)),
                AxisValue(DataAxis(data, DATA_AXIS1), pick->index[DATA_AXIS1]),
                AxisLabel(DataAxis(data, DATA_AXIS2)),
                AxisValue(DataAxis(data, DATA_AXIS2), pick->index[DATA_AXIS2]),
                AxisLabel(DataAxis(data, DATA_AXIS3)),
                AxisValue(DataAxis(data, DATA_AXIS3), pick->index[DATA_AXIS3]),
                AxisLabel(DataAxis(data, DATA_AXIS4)),
                AxisValue(DataAxis(data, DATA_AXIS4), pick->index[DATA_AXIS4]),
                AxisLabel(DataAxis(data, DATA_AXIS5)),
                AxisValue(DataAxis(data, DATA_AXIS5), pick->index[DATA_AXIS5]),
                AxisLabel(DataAxis(data, DATA_VALUE)), val );
        UIMessage(message);
    }

    return;
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void     PickDecodeShadow(Shadow_ shadow, int *index)
{

    extern Data data;
    int      iaxis;

    for( iaxis = DATA_NAXIS - 1; iaxis > 0; iaxis-- ){
        index[iaxis] = shadow / AxisStride(DataAxis(data, iaxis));
        shadow = shadow % AxisStride(DataAxis(data, iaxis));
    }
}

/*--------------------------------------------------------------------*\
   find a pick list
\*--------------------------------------------------------------------*/

PickLine PickFind(int dir3, int frame3, int dir4, int frame4, int dir5,
                  int frame5)
{

    int      iset;

    for( iset = 0; iset < PICKNLIST; iset++ ){
        if( picklist[iset]
            && dir3 == PICKDIR(picklist[iset]->, AXIS_DEEP)
            && frame3 == PICKFRAME0(picklist[iset]->, AXIS_DEEP)
            && (dir4 == NO_INDEX || (dir4 == PICKDIR(picklist[iset]->, AXIS_4D)
                                     && frame4 == PICKFRAME0(picklist[iset]->,
                                                             AXIS_4D)
                                     && dir5 == PICKDIR(picklist[iset]->,
                                                        AXIS_5D)
                                     && frame5 == PICKFRAME0(picklist[iset]->,
                                                             AXIS_5D))) ){
            return (lastpick = picklist[iset]);
        }
    }
    return (lastpick = 0);
}

/*--------------------------------------------------------------------*\
   return size of pick list
\*--------------------------------------------------------------------*/

int      PickSize(void)
{

    int      iset;
    int      nset;

    for( iset = 0, nset = 0; iset < PICKNLIST; iset++ ){

        if( picklist[iset] ){
            nset++;
        }

    }
    return (nset);
}

/*--------------------------------------------------------------------*\
   return pick axis
\*--------------------------------------------------------------------*/

int      PickAxis(void)
{

    int      iset;

    for( iset = 0; iset < PICKNLIST; iset++ ){

        if( picklist[iset] ){
            return (PICKDIR(picklist[iset]->, AXIS_DEEP));
        }

    }
    return (NO_INDEX);
}

/*--------------------------------------------------------------------*\
   return pick direction
\*--------------------------------------------------------------------*/

int      PickDir(PickLine pickline)
{

    if( !pickline ){
        return (NO_INDEX);

    } else {
        return (PICKDIR(pickline->, AXIS_DEEP));

    }
}

/*--------------------------------------------------------------------*\
   return pick count
\*--------------------------------------------------------------------*/

int      PickCount(PickLine pickline)
{

    if( !pickline ){
        return (NO_INDEX);

    } else {
        return (pickline->npick);

    }
}

/*--------------------------------------------------------------------*\
   return pick sample ipick and idim
\*--------------------------------------------------------------------*/

int      PickIndex(PickLine pickline, int ipick, int idim)
{

    if( !pickline ){
        return (NO_INDEX);
    }

    if( ipick < 0 || ipick >= NPICK ){
        return (NO_INDEX);
    }

    if( idim < 0 || idim >= DATA_NAXIS ){
        return (NO_INDEX);
    }

    return (pickline->index[ipick][idim]);
}

/*--------------------------------------------------------------------*\
   return pick list frame
\*--------------------------------------------------------------------*/

int      PickFrame(int ipick)
{

    int      iset;

    for( iset = 0; iset < PICKNLIST; iset++ ){

        if( picklist[iset] ){
            ipick--;
        }

        if( ipick < 0 ){
            return (PICKFRAME0(picklist[iset]->, AXIS_DEEP));
        }

    }

    return (NO_INDEX);
}

/*--------------------------------------------------------------------*\
   print information about pick object
\*--------------------------------------------------------------------*/

void     PickInfo(void)
{

    Message  message;
    PickLine pickline = 0;
    int      iset;

    pickline = lastpick;

    if( pickline == 0 ){

        for( iset = 0; iset < PICKNLIST; iset++ ){

            if( picklist[iset] ){
                pickline = picklist[iset];
            }
        }
    }

    if( pickline ){
        sprintf(message, "Pick: nset=%d dir=%d frame=%d npick=%d pickfile=%s",
                PickSize()
                , PICKDIR(pickline->, AXIS_DEEP)
                , PICKFRAME0(pickline->, AXIS_DEEP)
                , pickline->npick, pickfile);

    } else {
        sprintf(message, "Pick: nset=0 pickfile=%s", pickfile);

    }

    UIMessage(message);
}

/*--------------------------------------------------------------------*\
   add valid pick to current set
\*--------------------------------------------------------------------*/

void     PickAdd(int x, int y)
{

    int      iset;
    int      iaxis;
    PickPoint_ pick;
    PickLine pickline;

    /*-------------*/
    /* recover set */
    /*-------------*/

    PickDecode(x, y, &pick, 1);

    if( PICKDIR(pick., AXIS_DEEP) == NO_INDEX ){
        return;
    }

    /*---------------*/
    /* find pick set */
    /*---------------*/

    if( 
        (pickline =
         PickFind(PICKDIR(pick., AXIS_DEEP), PICKFRAME(pick., AXIS_DEEP),
                  PICKDIR(pick., AXIS_4D), PICKFRAME(pick., AXIS_4D),
                  PICKDIR(pick., AXIS_5D), PICKFRAME(pick., AXIS_5D))) == 0 ){

        for( iset = 0; iset < PICKNLIST; iset++ ){

            if( picklist[iset] == 0 ){
                {
                    extern int _alloc;

                    picklist[iset] =
                        (PickLine) malloc((1) * sizeof(picklist[iset][0]));
                    _alloc += (1) * sizeof(picklist[iset][0]);
                    if( picklist[iset] == 0 ){
                        err
                            ("cant allocate %d bytes for picklist[iset] ; %d already allocated",
                             (1) * sizeof(picklist[iset][0]), _alloc);
                    }
                    if( memwatch ){
                        (void) printf("malloc %s=%d\n", "picklist[iset] ",
                                      (1) * sizeof(picklist[iset][0]));
                    }
                };
                pickline = picklist[iset];
                pickline->npick = 0;

                for( iaxis = 0; iaxis < DATA_NAXIS; iaxis++ ){
                    pickline->iaxis[iaxis] = pick.iaxis[iaxis];
                    pickline->index[0][iaxis] = pick.index[iaxis];
                }

                break;
            }
        }

        /*--------------------*/
        /* no more allocation */
        /*--------------------*/

        if( iset == PICKNLIST ){
            return;
        }
    }
    if( pickline->npick == NPICK ){
        return;
    }

    for( iaxis = 0; iaxis < DATA_NAXIS; iaxis++ ){
        pickline->index[pickline->npick][iaxis] = pick.index[iaxis];
    }

    pickline->npick++;
    PickDraw(pickline, DRAW);
}

/*--------------------------------------------------------------------*\
   add valid pick to current set
\*--------------------------------------------------------------------*/

void     PickInsert(int x, int y)
{

    int      i;
    int      span;
    int      metric;
    int      near;
    int      inear;
    int      iset;
    int      iaxis;
    int      insert;
    PickPoint_ pick;
    PickLine pickline;

    /*-------------*/
    /* recover set */
    /*-------------*/

    PickDecode(x, y, &pick, 1);

    if( PICKDIR(pick., AXIS_DEEP) == NO_INDEX ){
        return;
    }

    /*---------------*/
    /* find pick set */
    /*---------------*/

    if( 
        (pickline =
         PickFind(PICKDIR(pick., AXIS_DEEP), PICKFRAME(pick., AXIS_DEEP),
                  PICKDIR(pick., AXIS_4D), PICKFRAME(pick., AXIS_4D),
                  PICKDIR(pick., AXIS_5D), PICKFRAME(pick., AXIS_5D))) == 0 ){

        for( iset = 0; iset < PICKNLIST; iset++ ){

            if( picklist[iset] == 0 ){

                {
                    extern int _alloc;

                    picklist[iset] =
                        (PickLine) malloc((1) * sizeof(picklist[iset][0]));
                    _alloc += (1) * sizeof(picklist[iset][0]);
                    if( picklist[iset] == 0 ){
                        err
                            ("cant allocate %d bytes for picklist[iset] ; %d already allocated",
                             (1) * sizeof(picklist[iset][0]), _alloc);
                    }
                    if( memwatch ){
                        (void) printf("malloc %s=%d\n", "picklist[iset] ",
                                      (1) * sizeof(picklist[iset][0]));
                    }
                };
                pickline = picklist[iset];
                pickline->npick = 0;

                for( iaxis = 0; iaxis < DATA_NAXIS; iaxis++ ){
                    pickline->iaxis[iaxis] = pick.iaxis[iaxis];
                    pickline->index[0][iaxis] = pick.index[iaxis];
                }

                break;
            }
        }

        /*--------------------*/
        /* no more allocation */
        /*--------------------*/

        if( iset == PICKNLIST ){
            return;
        }
    }

    if( pickline->npick == NPICK ){
        return;
    }

    /*-------------------*/
    /* find insert point */
    /*-------------------*/

    /*--------------------------------------------*/
    /* if one or less existing points ,add to end */
    /*--------------------------------------------*/

    if( pickline->npick <= 1 ){
        insert = pickline->npick;

    /*-----------------------------------------------*/
    /* base insertion point on nearest existing pick */
    /*-----------------------------------------------*/

    } else {

        PickDraw(pickline, ERASE);
        near = 1000000;
        inear = 0;

        for( i = 0; i < pickline->npick; i++ ){

            metric = 0;

            for( iaxis = 1; iaxis < DATA_NAXIS; iaxis++ ){
                span = (pickline->index[i][iaxis] - pick.index[iaxis]);
                metric += span * span;
            }

            if( metric < near ){
                near = metric;
                inear = i;
            }
        }

        /*------------------------------*/
        /* closest point is first point */
        /*------------------------------*/

        if( inear == 0 ){

            /*----------------------*/
            /* between second point */
            /*----------------------*/

            if( PickBetween(pick.index, pickline->index[0], pickline->index[1]) ){
                insert = 1;

            /*-----------------------*/
            /* or before first point */
            /*-----------------------*/

            } else {
                insert = 0;
            }

        /*-----------------------------*/
            /* closest point is last point */

        /*-----------------------------*/

        } else if( inear >= pickline->npick - 1 ){

            /*----------------------------*/
            /* between next to last point */
            /*----------------------------*/

            if( PickBetween(pick.index, pickline->index[pickline->npick - 1]
                            , pickline->index[pickline->npick - 2]) ){
                insert = pickline->npick - 1;

            /*------------------*/
            /* after last point */
            /*------------------*/

            } else {
                insert = pickline->npick;
            }

        /*---------------------------------*/
        /* closest point in middle of line */
        /*---------------------------------*/

        } else {

            /*------------------------*/
            /* between previous point */
            /*------------------------*/

            if( PickBetween
                (pick.index, pickline->index[inear],
                 pickline->index[inear - 1]) ){
                insert = inear;

            /*--------------------------*/
            /* between subsequent point */
            /*--------------------------*/

            } else {
                insert = inear + 1;
            }
        }
    }

    /*------------------------------------*/
    /* shift points after insertion point */
    /*------------------------------------*/

    for( i = pickline->npick; i > insert; i-- ){

        for( iaxis = 0; iaxis < DATA_NAXIS; iaxis++ ){
            pickline->index[i][iaxis] = pickline->index[i - 1][iaxis];
        }
    }

    /*--------*/
    /* insert */
    /*--------*/

    for( iaxis = 0; iaxis < DATA_NAXIS; iaxis++ ){
        pickline->index[insert][iaxis] = pick.index[iaxis];
    }

    pickline->npick++;

    /*--------*/
    /* redraw */
    /*--------*/

    PickDraw(pickline, DRAW);
}

/*--------------------------------------------------------------------*\
   replace nearest pick
\*--------------------------------------------------------------------*/

void     PickReplace(int x, int y)
{

    PickLine pickline;
    int      i;
    int      j;
    int      span;
    int      metric;
    int      near;
    int      inear;
    PickPoint_ pick;

    /*-------------*/
    /* recover set */
    /*-------------*/

    PickDecode(x, y, &pick, 1);

    if( PICKDIR(pick., AXIS_DEEP) == NO_INDEX ){
        return;
    }

    if( 
        (pickline =
         PickFind(PICKDIR(pick., AXIS_DEEP), PICKFRAME(pick., AXIS_DEEP),
                  PICKDIR(pick., AXIS_4D), PICKFRAME(pick., AXIS_4D),
                  PICKDIR(pick., AXIS_5D), PICKFRAME(pick., AXIS_5D))) == 0 ){
        return;
    }

    if( pickline->npick == 0 ){
        return;
    }

    PickDraw(pickline, ERASE);
    near = 1000000;
    inear = 0;

    for( i = 0; i < pickline->npick; i++ ){
        metric = 0;

        for( j = 1; j < DATA_NAXIS; j++ ){
            span = (pickline->index[i][j] - pick.index[j]);
            metric += span * span;
        }

        if( metric < near ){
            near = metric;
            inear = i;
        }
    }

    for( j = 0; j < DATA_NAXIS; j++ ){
        pickline->index[inear][j] = pick.index[j];
    }

    PickDraw(pickline, DRAW);
}

/*--------------------------------------------------------------------*\
   delete nearest pick
\*--------------------------------------------------------------------*/

void     PickDelete(int x, int y)
{

    PickLine pickline;
    int      i;
    int      j;
    int      span;
    int      metric;
    int      near;
    int      inear;
    PickPoint_ pick;

    /*-------------*/
    /* recover set */
    /*-------------*/

    PickDecode(x, y, &pick, 1);

    if( PICKDIR(pick., AXIS_DEEP) == NO_INDEX ){
        return;
    }

    if( 
        (pickline =
         PickFind(PICKDIR(pick., AXIS_DEEP), PICKFRAME(pick., AXIS_DEEP),
                  PICKDIR(pick., AXIS_4D), PICKFRAME(pick., AXIS_4D),
                  PICKDIR(pick., AXIS_5D), PICKFRAME(pick., AXIS_5D))) == 0 ){
        return;
    }

    if( pickline->npick < 2 ){
        PickClear(pickline);
        return;
    }

    PickDraw(pickline, ERASE);
    near = 1000000;
    inear = 0;

    for( i = 0; i < pickline->npick; i++ ){
        metric = 0;

        for( j = 1; j < DATA_NAXIS; j++ ){
            span = (pickline->index[i][j] - pick.index[j]);
            metric += span * span;
        }

        if( metric < near ){
            near = metric;
            inear = i;
        }
    }

    for( i = inear; i < pickline->npick - 1; i++ ){

        for( j = 0; j < DATA_NAXIS; j++ ){
            pickline->index[i][j] = pickline->index[i + 1][j];
        }
    }

    pickline->npick--;
    PickDraw(pickline, DRAW);
}

/*--------------------------------------------------------------------*\
   end pick enable
\*--------------------------------------------------------------------*/

void     PickDraw(PickLine pickline, int draw)
{

    Plane    plane;
    int      ipick;
    int      x;
    int      y;
    int      x0 = -1;
    int      y0 = -1;
    int      x1;
    int      y1;
    int      x2;
    int      y2;
    int      hskew = 0;
    int      vskew = 0;

    if( !pickline ){
        return;
    }

    if( 
        (plane =
         PlaneFind(PICKDIR(pickline->, AXIS_DEEP),
                   PICKFRAME0(pickline->, AXIS_DEEP), PICKDIR(pickline->,
                                                              AXIS_4D),
                   PICKFRAME0(pickline->, AXIS_4D), PICKDIR(pickline->,
                                                            AXIS_5D),
                   PICKFRAME0(pickline->, AXIS_5D))) == 0 ){
        return;
    }

    if( PlaneType(plane) == RENDER_HORZ ){
        hskew = PlaneSkew(plane);

    } else {
        vskew = PlaneSkew(plane);
    }

    for( ipick = 0; ipick < pickline->npick; ipick++ ){
        x = MapInverse(PlaneHmap(plane),
                       pickline->index[ipick][AxisDir
                                              (MapAxis(PlaneHmap(plane)))]);
        if( PlaneType(plane) == RENDER_VERT ){
            x = MapSize(PlaneHmap(plane)) - x;
        }
        y = MapInverse(PlaneVmap(plane),
                       pickline->index[ipick][AxisDir
                                              (MapAxis(PlaneVmap(plane)))]);
        if( x != NO_INDEX && y != NO_INDEX ){
            x1 = x + PlaneH0(plane) + hskew * (MapSize(PlaneVmap(plane)) - y);
            y1 = y + PlaneV0(plane) + vskew * (PlaneNH(plane) - x);
            if( !ipick ){
                DrawLine(x1 - 1, y1 - 1, x1, y1, draw);

            } else if( x0 != NO_INDEX && y0 != NO_INDEX ){
                x2 = x0 + PlaneH0(plane) + hskew *
                    (MapSize(PlaneVmap(plane)) - y0);
                y2 = y0 + PlaneV0(plane) + vskew * (PlaneNH(plane) - x0);
                DrawLine(x1, y1, x2, y2, draw);
            }
        }
        x0 = x;
        y0 = y;
    }
}

/*--------------------------------------------------------------------*\
   draw all pick sets
\*--------------------------------------------------------------------*/

void     PickDrawAll(int draw)
{

    int      iset;

    for( iset = 0; iset < PICKNLIST; iset++ ){

        if( picklist[iset] ){
            PickDraw(picklist[iset], draw);
        }
    }
}

/*--------------------------------------------------------------------*\
   read pick file
\*--------------------------------------------------------------------*/

void     PickRead(void)
{

    FILE    *fd;
    FILE    *fopen(const char *, const char *);

    extern Data data;
    string   dummy;
    string   line;
    int      iset = 0;
    int      i;
    float    value[DATA_NAXIS];
    Message  message;
    cwp_String str;

    if( !data ){
        return;
    }

    if( getparstring ("annotate", &str) == 0 ){
        return;
    }

    strcpy( pickfile ,str );

    if( (fd = fopen(pickfile, "r")) == NULL ){
        UIMessage("cant open pick file");
        return;
    }

    fgets(line, sizeof(line), fd);

/*--------------------------- dead code ------------------------------*\

    int      n1;
    int      n2;
    int      n3;
    int      n4;
    int      n5;

    sscanf ( line,"%s %d %d %d %d %d",dummy,&n1,&n2,&n3,&n4,&n5 );
    if(  n1 != AxisSize( DataAxis( data,DATA_AXIS1 ) )
     || n2 != AxisSize( DataAxis( data,DATA_AXIS2 ) ) 
     || n3 != AxisSize( DataAxis( data,DATA_AXIS3 ) ) 
     || n4 != AxisSize( DataAxis( data,DATA_AXIS4 ) ) 
     || n5 != AxisSize( DataAxis( data,DATA_AXIS5 ) ) ){
            UIMessage ( "pick file wrong shape" );
            return;
    }

\*--------------------------- dead code ------------------------------*/

    fgets(line, sizeof(line), fd);

    while (fgets(line, sizeof(line), fd) != NULL ){

        if( !strncmp(line, "Pick#", 5) ){
            iset++;

            {
                extern int _alloc;

                picklist[iset] =
                    (PickLine) malloc((1) * sizeof(picklist[iset][0]));
                    _alloc += (1) * sizeof(picklist[iset][0]);
                if( picklist[iset] == 0 ){
                    err
                        ("cant allocate %d bytes for picklist[iset] ; %d already allocated",
                         (1) * sizeof(picklist[iset][0]), _alloc);
                }
                if( memwatch ){
                    (void) printf("malloc %s=%d\n", "picklist[iset] ",
                                  (1) * sizeof(picklist[iset][0]));
                }
            };
            sscanf(line, "%s %s %s %d %d %d %d %d %d", dummy, dummy, dummy,
                   &PICKDIR(picklist[iset]->, AXIS_DOWN),
                   &PICKDIR(picklist[iset]->, AXIS_ACROSS),
                   &PICKDIR(picklist[iset]->, AXIS_DEEP),
                   &PICKDIR(picklist[iset]->, AXIS_4D),
                   &PICKDIR(picklist[iset]->, AXIS_5D),
                   &PICKDIR(picklist[iset]->, AXIS_5D));
            picklist[iset]->npick = 0;

        } else {

            if( picklist[iset]->npick < NPICK ){
                sscanf(line, "%f %f %f %f %f %f",
                       &value[DATA_AXIS1],
                       &value[DATA_AXIS2],
                       &value[DATA_AXIS3],
                       &value[DATA_AXIS4],
                       &value[DATA_AXIS5], &value[DATA_VALUE]);

                for( i = 1; i < DATA_NAXIS; i++ ){
                    picklist[iset]->index[picklist[iset]->npick][i] =
                        AxisIndex(DataAxis(data, i), value[i]);
                }

                picklist[iset]->index[picklist[iset]->npick][DATA_VALUE] =
                    value[DATA_VALUE];
                picklist[iset]->npick++;
            }
        }
    }

    fclose(fd);
    sprintf(message, "%d picklines read from %s", iset, pickfile);
    UIMessage(message);
}

#ifdef UNUSED_CODE

/*--------------------------------------------------------------------*\
   write pick file
\*--------------------------------------------------------------------*/

void     PickWrite(void)
{

    FILE    *fd;
    Message  message;

    extern Data data;
    extern FILE *outstream;

    int      ipick;
    int      isave;
    int      iset;

    if( picklist[0] == 0 ){
        return;
    }

    if( !strcmp(pickfile, "stdio") ){
        fd = outstream;

    } else {
        fd = fopen(pickfile, "w");
    }

    fseek(fd, 0L, SEEK_SET);
    fprintf(fd, "Picks for dataset: %s\n", DataTitle(data));

/*------------------------ dead code ---------------------------------*\

        fprintf ( fd,"Dimensions: %d %d %d %d %d\n",
                AxisSize( DataAxis( data,DATA_AXIS1 ) ),
                AxisSize( DataAxis( data,DATA_AXIS2 ) ),
                AxisSize( DataAxis( data,DATA_AXIS3 ) ),
                AxisSize( DataAxis( data,DATA_AXIS4 ) ),
                AxisSize( DataAxis( data,DATA_AXIS5 ) ) );

\*------------------------ dead code ---------------------------------*/

    fprintf(fd, "%9s: %9s: %9s: %9s: %9s: %9s:\n",
            AxisLabel(DataAxis(data, DATA_AXIS1)),
            AxisLabel(DataAxis(data, DATA_AXIS2)),
            AxisLabel(DataAxis(data, DATA_AXIS3)),
            AxisLabel(DataAxis(data, DATA_AXIS4)),
            AxisLabel(DataAxis(data, DATA_AXIS5)),
            AxisLabel(DataAxis(data, DATA_AXIS0)));

    for( iset = 0, isave = 0; iset < PICKNLIST; iset++ ){

        if( picklist[iset] && picklist[iset]->npick > 0 ){
            fprintf(fd, "Pick# %d axis# %d %d %d %d %d %s\n",
                    isave++,
                    PICKDIR(picklist[iset]->, AXIS_DOWN),
                    PICKDIR(picklist[iset]->, AXIS_ACROSS),
                    PICKDIR(picklist[iset]->, AXIS_DEEP),
                    PICKDIR(picklist[iset]->, AXIS_4D),
                    PICKDIR(picklist[iset]->, AXIS_5D),
                    AxisScript(DataAxis
                               (data, PICKDIR(picklist[iset]->, AXIS_DEEP)),
                               picklist[iset]->index[AXIS_DEEP]));

            for( ipick = 0; ipick < picklist[iset]->npick; ipick++ ){
                fprintf(fd, "%10g %10g %10g %10g %10g %10g\n",
                        AxisValue(DataAxis(data, DATA_AXIS1),
                                  picklist[iset]->index[ipick][DATA_AXIS1]),
                        AxisValue(DataAxis(data, DATA_AXIS2),
                                  picklist[iset]->index[ipick][DATA_AXIS2]),
                        AxisValue(DataAxis(data, DATA_AXIS3),
                                  picklist[iset]->index[ipick][DATA_AXIS3]),
                        AxisValue(DataAxis(data, DATA_AXIS4),
                                  picklist[iset]->index[ipick][DATA_AXIS4]),
                        AxisValue(DataAxis(data, DATA_AXIS5),
                                  picklist[iset]->index[ipick][DATA_AXIS5]),
                        (float) picklist[iset]->index[ipick][DATA_VALUE]);
            }
        }
    }
    fclose(fd);
    sprintf(message, "%d pick lines save in %s", isave, pickfile);
    UIMessage(message);
}

#endif

/*--------------------------------------------------------------------*\
   remove set from pick list
\*--------------------------------------------------------------------*/

void     PickClear(PickLine pickline)
{

    int      iset;

    if( !pickline ){
        return;
    }

    PickDraw(pickline, ERASE);

    for( iset = 0; iset < PICKNLIST; iset++ ){

        if( pickline == picklist[iset] ){

            if( picklist[iset] ){
                free(picklist[iset]);
                picklist[iset] = 0;
                if( memwatch ){
                    printf("free %s\n", " picklist[iset] ");
                }
            };
            picklist[iset] = 0;
            return;
        }
    }
    lastpick = 0;
}

/*--------------------------------------------------------------------*\
   clear current pick
\*--------------------------------------------------------------------*/

void     PickClear0(void)
{

    PickClear(lastpick);
}

/*---------------------- unreferenced code ---------------------------*\
   set pick frame

void PickSetFrame( int index ){

    PickLine pickline;
    int      iset;

    if(  index < 0 || index > PickSize(  ) ){
        return;
    }

    for(  iset = 0; iset < PICKNLIST; iset++ ){

        if(  picklist[iset] ){
            index--;
        }

        if(  index < 0 ){
            pickline = picklist[iset];
            break;
        }
    }
    if(  iset == PICKNLIST ){
        return;
    }

    ViewSetMovie( PICKDIR( pickline-> ,AXIS_DEEP ) );
    ViewSetFrame( pickline->index[AXIS_DEEP] ,FRAME_ACTUAL );

    PickDraw( pickline ,DRAW );

}
\*---------------------- unreferenced code ---------------------------*/

/*--------------------------------------------------------------------*\
   print a list of picks
\*--------------------------------------------------------------------*/

void     PickListInfo(void)
{

    int      iset;

    printf("Pick list:\n");

    for( iset = -1; iset < PICKNLIST; iset++ ){

        if( picklist[iset] ){
            printf("dir=%d frame=%d\n",
                   PICKDIR(picklist[iset]->, AXIS_DEEP),
                   PICKFRAME0(picklist[iset]->, AXIS_DEEP));
        }
    }
}

/*--------------------------------------------------------------------*\
   save pick parameters
\*--------------------------------------------------------------------*/

void     PickSavePar(void)
{

    Message  message;
    extern PickLine lastpick;

    if( !lastpick ){
        return;
    }

    sprintf(message, "Pick: nset=%d pickdir=%d pickframe=%d npick=%d pick=%s",
            PickSize()
            , PICKDIR(lastpick->, AXIS_DEEP)
            , PICKFRAME0(lastpick->, AXIS_DEEP)
            , lastpick->npick, pickfile);

    UISaveMessage(message);
}

/*--------------------------------------------------------------------*\
   returns two picks are in same frame
\*--------------------------------------------------------------------*/

int      PickSameFrame(PickPoint pick1, PickPoint pick2)
{

    return ((pick1->iaxis[AXIS_DOWN] == pick2->iaxis[AXIS_DOWN]) &&
            (pick1->iaxis[AXIS_ACROSS] == pick2->iaxis[AXIS_ACROSS]) &&
            (pick1->iaxis[AXIS_DEEP] == pick2->iaxis[AXIS_DEEP]) &&
            (pick1->iaxis[AXIS_4D] == pick2->iaxis[AXIS_4D]) &&
            (pick1->iaxis[AXIS_5D] == pick2->iaxis[AXIS_5D]) &&
            (pick1->index[pick1->iaxis[AXIS_DEEP]] ==
             pick2->index[pick2->iaxis[AXIS_DEEP]])
            && (pick1->index[pick1->iaxis[AXIS_4D]] ==
                pick2->index[pick2->iaxis[AXIS_4D]])
            && (pick1->index[pick1->iaxis[AXIS_5D]] ==
                pick2->index[pick2->iaxis[AXIS_5D]]));
}

/*--------------------------------------------------------------------*\
   returns two picks are same direction
\*--------------------------------------------------------------------*/

int      PickSameDir(PickPoint pick1, PickPoint pick2)
{

    return ((pick1->iaxis[AXIS_DEEP] == pick2->iaxis[AXIS_DEEP]) &&
            (pick1->iaxis[AXIS_4D] == pick2->iaxis[AXIS_4D]) &&
            (pick1->iaxis[AXIS_5D] == pick2->iaxis[AXIS_5D]));
}

/*--------------------------------------------------------------------*\
   returns direction of shared coordinate: 0 ,pick->iaxis[1] or
   pick->iaxis[2]
\*--------------------------------------------------------------------*/

int      PickSharedDir(PickPoint pick1, PickPoint pick2)
{

    return (PickSameFrame(pick1, pick2) *
            (((pick1->index[pick1->iaxis[AXIS_DOWN]] ==
               pick2->index[pick2->iaxis[AXIS_DOWN]]) *
              pick1->iaxis[AXIS_DOWN]) +
             ((pick1->index[pick1->iaxis[AXIS_ACROSS]] ==
               pick2->index[pick2->iaxis[AXIS_ACROSS]]) *
              pick1->iaxis[AXIS_ACROSS])));
}

/*--------------------------------------------------------------------*\
   return non-zero if a is between b and c; angle a-b-c < 90; a is
   nearer to b
\*--------------------------------------------------------------------*/
int      PickBetween(int *a, int *b, int *c)
{
    int      i;
    int      ra = 0;
    int      rb = 0;
    int      rc = 0;

    for( i = 0; i < DATA_NAXIS; i++ ){
        ra += (b[i] - c[i]) * (b[i] - c[i]);
        rb += (a[i] - c[i]) * (a[i] - c[i]);
        rc += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return (rb < (ra + rc));
}
