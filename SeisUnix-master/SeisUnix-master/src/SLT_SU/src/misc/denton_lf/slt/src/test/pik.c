/*====================================================================*\
                       pik list subroutines

   This code is complicated by the extensive use of global variables.

   Note that these routines are distinct from the pick routines
   whjich appear to be used for annotation of the frame positions.

   Reginald H. Beardsley                            rhb@acm.org

\*====================================================================*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "su.h"

#include "main.h"
#include "axis.h"
#include "color.h"
#include "draw.h"
#include "data.h"
#include "map.h"
#include "render.h"
#include "view.h"
#include "plane.h"
#include "pick.h"
#include "pik.h"
#include "ui_window.h"
#include "ui_menu.h"

/* main pik object */
PikList  pik = 0;
PikList  pikSet[NFILES];

static cwp_String pikfile[NFILES];
int      nfiles = 0;

/*--------------------------------------------------------------------*\
   Initialize startup pik objects specified by pick=file1,file2.
   There is no default pickfile. 
\*--------------------------------------------------------------------*/

void     PikInit(void)
{

    int      i;

    if ((nfiles = countparval("pick")) != 0) {
        getparstringarray("pick", pikfile);
    }

    for (i = 0; i < nfiles; i++) {

        {
            extern int _alloc;

            pikSet[i] = (PikList) malloc((1) * sizeof(pikSet[i][0]));
            _alloc += (1) * sizeof(pikSet[i][0]);
            if (pikSet[i] == 0)
                err
                    ("cant allocate %d bytes for  pikSet[i]; %d already allocated",
                     (1) * sizeof(pikSet[i][0]), _alloc);
            if (memwatch)
                (void) printf("malloc %s=%d\n", " pikSet[i]",
                              (1) * sizeof(pikSet[i][0]));
        };
        pik = pikSet[i];

        strcpy(pik->file, pikfile[i]);
        pik->npik = 0;
        pik->size = PIK_SIZE;
        pik->changed = 0;
        getparint("picksize", &pik->size);
        pik->nmax = NPIK;
        getparint("npick", &pik->nmax);

        {
            extern int _alloc;

            pik->pik = (Pik *) malloc((pik->nmax) * sizeof(pik->pik[0]));
            _alloc += (pik->nmax) * sizeof(pik->pik[0]);
            if (pik->pik == 0)
                err
                    ("cant allocate %d bytes for  pik->pik; %d already allocated",
                     (pik->nmax) * sizeof(pik->pik[0]), _alloc);
            if (memwatch)
                (void) printf("malloc %s=%d\n", " pik->pik",
                              (pik->nmax) * sizeof(pik->pik[0]));
        };
        pik->range = PIK_RANGE;
        getparint("pickrange", &pik->range);
        PikRead();
    }

    if (!nfiles) {
        nfiles++;

        {
            extern int _alloc;

            pikSet[0] = (PikList) malloc((1) * sizeof(pikSet[0][0]));
            _alloc += (1) * sizeof(pikSet[0][0]);
            if (pikSet[0] == 0)
                err
                    ("cant allocate %d bytes for  pikSet[0]; %d already allocated",
                     (1) * sizeof(pikSet[0][0]), _alloc);
            if (memwatch)
                (void) printf("malloc %s=%d\n", " pikSet[0]",
                              (1) * sizeof(pikSet[0][0]));
        };
        pik = pikSet[0];
        memset(pik->file, 0, sizeof(pik->file));
        pik->npik = 0;
        pik->size = PIK_SIZE;
        pik->changed = 0;
        getparint("picksize", &pik->size);
        pik->nmax = NPIK;
        getparint("npick", &pik->nmax);
        {
            extern int _alloc;

            pik->pik = (Pik *) malloc((pik->nmax) * sizeof(pik->pik[0]));
            _alloc += (pik->nmax) * sizeof(pik->pik[0]);
            if (pik->pik == 0)
                err
                    ("cant allocate %d bytes for  pik->pik; %d already allocated",
                     (pik->nmax) * sizeof(pik->pik[0]), _alloc);
            if (memwatch)
                (void) printf("malloc %s=%d\n", " pik->pik",
                              (pik->nmax) * sizeof(pik->pik[0]));
        };
        pik->range = PIK_RANGE;
        getparint("pickrange", &pik->range);
    }

    PikDraw(NO_INDEX, DRAW);

}

/*--------------------------------------------------------------------*\
   Clear all items in the current pik list (pik) and redraw the screen.
\*--------------------------------------------------------------------*/

void     PikClear(void)
{

    PikDraw(NO_INDEX, ERASE);
    pik->npik = 0;
    pik->changed = 0;
    PikDraw(NO_INDEX, DRAW);

}

/*--------------------------------------------------------------------*\
   Read one pick file. 
\*--------------------------------------------------------------------*/

void     PikRead(void)
{

    int      iaxis;
    int      ipik;
    FILE    *fd;
    float    value[DATA_NAXIS+1];
    char     line[256];
    char*    ptr;

    int cnt;
    int i;

    extern Data data;
    Message  message;

    if (!pik) {
        return;
    }

    if ((fd = fopen(pik->file, "r")) == NULL) {
        sprintf(message, "can't open pick=%s file for reading", pik->file);
        UIMessage(message);
        return;
    }

/*--------------------------------------------------------------------*\
   The format being parsed here is a bit odd.  The input consists of
   a line "n1 n2 n3 n4 n5 val" however the data is stored in the
   value array in the order: val n1 n2 n3 n4 n5.

   The intent here is to allow 3 to 6 column input with the last 
   column being the data value.  The value is optional.
\*--------------------------------------------------------------------*/

    while (pik->npik < pik->nmax && !feof(fd) ){

      memset( value ,0 ,sizeof(value) );
      memset( line  ,0 ,sizeof(line)  );

      if( fgets( line ,sizeof(line) ,fd ) && line[0] != '#' ){

           ptr = line;
           cnt = 0;
           i = 1;
   
           while( *ptr ){
     
              strtok( ptr ," \t\n" );
     
              if( ptr && strspn( ptr ," \t\n") != strlen( ptr ) ){
                 sscanf( ptr ,"%f" ,&value[i] );
                 cnt++;
              }
     
              ptr += strlen(ptr) + 1;
              i++;
     
           }
  
          if( cnt > 2 ){

             for (iaxis = 1; iaxis < DATA_NAXIS; iaxis++) {
                 pik->pik[pik->npik][iaxis] =
                     AxisIndex(DataAxis(data, iaxis), value[iaxis]);
             }
          }

/*--------------------------------------------------------------------*\
   Allow specifying coordinates w/o the field value to facilitate
   data import from other sources.
\*--------------------------------------------------------------------*/

          if( cnt > 3 ){
             pik->pik[pik->npik][0] = 
                  AxisIndex(DataAxis(data, 0), value[cnt] );

          }else{
             pik->pik[pik->npik][0] = 0;

          }
  
          pik->npik++;
      }

    }

    fclose(fd);

    sprintf(message, "%d picks read from file %s", pik->npik, pik->file);
    UIMessage(message);

/*--------------------------------------------------------------------*\
   Note: this cruft belongs in a class constructor, not here. Since
   we're not really using C++, the overhead has to be tolerated.
\*--------------------------------------------------------------------*/

    for (ipik = pik->npik; ipik < pik->nmax; ipik++) {
        pik->pik[ipik][0] = NO_INDEX;
    }

}

/*--------------------------------------------------------------------*\
   write the current pick file
\*--------------------------------------------------------------------*/

void     PikWrite(char *filename, FILE * fd)
{

    int      ipik, count = 0;
    extern Data data;
    Message  message;

    if (!pik || !pik->npik) {
        return;
    }

    strcpy(pik->file, filename);

    for (ipik = 0, count = 0; ipik < pik->npik; ipik++) {

        if (pik->pik[ipik][0] != NO_INDEX) {
            count++;
            fprintf(fd, "%10g %10g %10g %10g %10g %10g\n",
                    AxisValue(DataAxis(data, 1), pik->pik[ipik][1]),
                    AxisValue(DataAxis(data, 2), pik->pik[ipik][2]),
                    AxisValue(DataAxis(data, 3), pik->pik[ipik][3]),
                    AxisValue(DataAxis(data, 4), pik->pik[ipik][4]),
                    AxisValue(DataAxis(data, 5), pik->pik[ipik][5]),
                    AxisValue(DataAxis(data, 0), pik->pik[ipik][0]));
        }
    }

    fclose(fd);
    sprintf(message, "%d picks written to %s", count, pik->file);
    UIMessage(message);
}

/*--------------------------------------------------------------------*\
   draw pick pik0 or all picks ( pik0 = NO_INDEX ) on all planes.
   Note that the current pick list selected must be saved and
   restored after the draw operation. This is ugly, but done as a
   defensive measure since the scope of "pik" is large and unknown.
\*--------------------------------------------------------------------*/

void     PikDraw(int pik0, int draw)
{

    Plane    plane;

    extern Data data;
    extern Plane planelist;

    int      dir;
    int      frame;
    int      hskew;
    int      ipik;
    int      pik1;
    int      pik2;
    int      range1;
    int      range2;
    int      vskew;
    int      x0;
    int      x;
    int      y0;
    int      y;
    int      i;

    PikList  currentPik;

    currentPik = pik;

    for (i = 0; i < nfiles; i++) {

        if (pik0 == NO_INDEX) {
            pik = pikSet[i];
        }

        if (!pik ){
            return;

        }else if (!pik->npik) {
            continue;
        }

        if (pik0 < pik->npik) {

/*--------------------------------------------------------------------*\
   Loop over all the planes currently shown on the screen. This
   varies from one to three depending on the display mode being
   used.  Picks are displayed as X's on the frame on which they were
   picked and as small boxes on adjacent frames.  The range field
   specifies the number of frames in which a box is shown.
\*--------------------------------------------------------------------*/

            for (plane = planelist; plane < planelist + NPLANE &&
                 plane->attr->orient != NO_INDEX; plane++) {


                dir    = PlaneDir(plane);
                frame  = PlaneFrame(plane);
                range1 = frame - pik->range;
                range2 = frame + pik->range;

                range1 = range1 > 0 ? range1 : 0;
                range2 =
                    range2 <
                    AxisSize(DataAxis(data, dir)) -
                    1 ? range2 : AxisSize(DataAxis(data, dir)) - 1;

/*--------------------------------------------------------------------*\
   Set bounds for the pick display loop.  This may be either a single
   pick or all the picks in a picklist.  For each pick, check to see 
   if the current frame and range values are such that the pick should
   be drawn.
\*--------------------------------------------------------------------*/

                if (pik0 == NO_INDEX) {
                    pik1 = 0;
                    pik2 = pik->npik - 1;

                } else {
                    pik1 = pik0;
                    pik2 = pik0;
                }

                for (ipik = pik1; ipik <= pik2; ipik++) {
                    if (pik->pik[ipik][0] != NO_INDEX
                        && pik->pik[ipik][dir] >= range1
                        && pik->pik[ipik][dir] <= range2) {

/*--------------------------------------------------------------------*\
   Calculate the screen coordinates for the pick from the data
   coordinates and whether the image is skewed.
\*--------------------------------------------------------------------*/

                        hskew = 0;
                        vskew = 0;
                        if (PlaneType(plane) == RENDER_HORZ) {
                            hskew = PlaneSkew(plane);

                        } else {
                            vskew = PlaneSkew(plane);
                        }

                        x = MapInverse(PlaneHmap(plane)
                                       ,
                                       pik->pik[ipik][AxisDir
                                                      (MapAxis
                                                       (PlaneHmap(plane)))]);
                        if (PlaneType(plane) == RENDER_VERT) {
                            x = MapSize(PlaneHmap(plane)) - x;
                        }

                        y = MapInverse(PlaneVmap(plane),
                                       pik->pik[ipik][AxisDir
                                                      (MapAxis
                                                       (PlaneVmap(plane)))]);

/*--------------------------------------------------------------------*\
   If the pick is drawable, draw an X or box as appropriate.
\*--------------------------------------------------------------------*/

                        if (x != NO_INDEX && y != NO_INDEX) {
                            x0 =
                                x + PlaneH0(plane) +
                                hskew * (MapSize(PlaneVmap(plane)) - y);
                            y0 =
                                y + PlaneV0(plane) + vskew * (PlaneNH(plane) -
                                                              x);
                            if (pik->pik[ipik][dir] == frame) {
                                PikDrawEx(x0, y0, draw);

                            } else if (pik->pik[ipik][dir] > frame) {
                                PikDrawBox(x0, y0,
                                           pik->range - pik->pik[ipik][dir] +
                                           frame, draw);

                            } else {
                                PikDrawBox(x0, y0,
                                           pik->range - frame +
                                           pik->pik[ipik][dir], draw);
                            }
                        }
                    }
                }
            }
        }
    }

    pik = currentPik;
}

/*--------------------------------------------------------------------*\
   draw an "x"
\*--------------------------------------------------------------------*/

void     PikDrawEx(int x, int y, int draw)
{

    if (!pik) {
        return;
    }

    DrawLine(x + pik->size, y + pik->size, x - pik->size, y - pik->size, draw);
    DrawLine(x - pik->size, y + pik->size, x + pik->size, y - pik->size, draw);
}

/*--------------------------------------------------------------------*\
   draw a box of specified size
\*--------------------------------------------------------------------*/

void     PikDrawBox(int x, int y, int size, int draw)
{

    if (!pik) {
        return;
    }

    DrawLine(x + size, y + size, x - size, y + size, draw);
    DrawLine(x + size, y - size, x - size, y - size, draw);
    DrawLine(x + size, y + size, x + size, y - size, draw);
    DrawLine(x - size, y + size, x - size, y - size, draw);
}

/*--------------------------------------------------------------------*\
   find nearest visible pick on mouse pick plane
\*--------------------------------------------------------------------*/

int      PikNear(int x, int y)
{

    PickPoint_ pick;
    Plane    plane;

    extern Data data;

    int      dir1;
    int      dir2;
    int      dir;
    int      distance;
    int      frame;
    int      inear;
    int      ipik;
    int      near;
    int      range1;
    int      range2;
    int      x1;
    int      y1;

    if (!pik || !pik->npik) {
        return (NO_INDEX);
    }

    PickDecode(x, y, &pick, 1);
    plane =
        PlaneFind(pick.iaxis[AXIS_DEEP], pick.index[pick.iaxis[AXIS_DEEP]], -1,
                  -1, -1, -1);
    dir = PlaneDir(plane);
    frame = PlaneFrame(plane);
    range1 = frame - pik->range;
    range2 = frame + pik->range;
    range1 = range1 > 0 ? range1 : 0;
    range2 =
        range2 <
        AxisSize(DataAxis(data, dir)) -
        1 ? range2 : AxisSize(DataAxis(data, dir)) - 1;
    near = 100000000;
    inear = NO_INDEX;
    dir1 = pick.iaxis[1];
    dir2 = pick.iaxis[2];

    for (ipik = 0; ipik < pik->npik; ipik++) {

        if (pik->pik[ipik][0] != NO_INDEX && pik->pik[ipik][dir] >= range1
            && pik->pik[ipik][dir] <= range2) {
            distance =
                (pik->pik[ipik][dir1] -
                 pick.index[dir1]) * (pik->pik[ipik][dir1] - pick.index[dir1])
                + (pik->pik[ipik][dir2] -
                   pick.index[dir2]) * (pik->pik[ipik][dir2] -
                                        pick.index[dir2]);
            if (distance < near) {
                near = distance;
                inear = ipik;
            }
        }
    }

    if (!PikCoord(inear, plane, &x1, &y1)) {
        return (NO_INDEX);
    }

    if ((x - x1) * (x - x1) + (y - y1) * (y - y1) > PIK_NEAR) {
        return (NO_INDEX);
    }

    return (inear);
}

/*--------------------------------------------------------------------*\
    Return the pick coordinates ???
\*--------------------------------------------------------------------*/

int      PikCoord(int ipik, Plane plane, int *x, int *y)
{

    int      hskew = 0;
    int      vskew = 0;
    int      x1;
    int      y1;

    *x = NO_INDEX;
    *y = NO_INDEX;

    if (PlaneType(plane) == RENDER_HORZ) {
        hskew = PlaneSkew(plane);

    } else {
        vskew = PlaneSkew(plane);
    }

    x1 =
        MapInverse(PlaneHmap(plane),
                   pik->pik[ipik][AxisDir(MapAxis(PlaneHmap(plane)))]);
    if (PlaneType(plane) == RENDER_VERT) {
        x1 = MapSize(PlaneHmap(plane)) - x1;
    }

    y1 =
        MapInverse(PlaneVmap(plane),
                   pik->pik[ipik][AxisDir(MapAxis(PlaneVmap(plane)))]);
    if (x1 == NO_INDEX || y1 == NO_INDEX) {
        return (0);
    }

    *x = x1 + PlaneH0(plane) + hskew * (MapSize(PlaneVmap(plane)) - y1);
    *y = y1 + PlaneV0(plane) + vskew * (PlaneNH(plane) - x1);
    return (1);
}

/*--------------------------------------------------------------------*\
   inquire about pick nearest to mouse pick
\*--------------------------------------------------------------------*/

void     PikQuery(int x, int y)
{

    int      inear;
    extern Data data;
    Message  message;

    inear = PikNear(x, y);
    if (inear != NO_INDEX) {
        sprintf(message, "Pik%d= %s=%g %s=%g %s=%g %s%g %s=%g %s=%g", inear,
                AxisLabel(DataAxis(data, 1)), AxisValue(DataAxis(data, 1),
                                                        pik->pik[inear][1]),
                AxisLabel(DataAxis(data, 2)), AxisValue(DataAxis(data, 2),
                                                        pik->pik[inear][2]),
                AxisLabel(DataAxis(data, 3)), AxisValue(DataAxis(data, 3),
                                                        pik->pik[inear][3]),
                AxisLabel(DataAxis(data, 4)), AxisValue(DataAxis(data, 4),
                                                        pik->pik[inear][4]),
                AxisLabel(DataAxis(data, 5)), AxisValue(DataAxis(data, 5),
                                                        pik->pik[inear][5]),
                AxisLabel(DataAxis(data, 0)), AxisValue(DataAxis(data, 0),
                                                        pik->pik[inear][0]));
        UIMessage(message);
    }
}

/*--------------------------------------------------------------------*\
   add a pick at mouse pick location
\*--------------------------------------------------------------------*/

void     PikAdd(int x, int y)
{

    int      iaxis;
    PickPoint_ pick;

    if (!pik) {
        return;
    }

    PickDecode(x, y, &pick, 1);

    for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
        pik->pik[pik->npik][iaxis] = pick.index[iaxis];
    }
    pik->npik = pik->npik < pik->nmax - 1 ? pik->npik + 1 : pik->nmax - 1;
    PikDraw(pik->npik - 1, DRAW);
    pik->last_op = PIKOP_ADD;
    pik->last_item = pik->npik - 1;
    pik->changed = 1;
}

/*--------------------------------------------------------------------*\
   add a pick at X edge nearest mouse location
\*--------------------------------------------------------------------*/

void     PikEdge(int x, int y)
{

    int      iaxis;
    PickPoint_ pick;
    extern Data data;

    if (!pik) {
        return;
    }

    PickDecode(x, y, &pick, 1);

    if (pick.iaxis[1] == 1 && pick.iaxis[2] == 2) {
        if (pick.index[2] < data->axis[2]->size / 2) {
            pick.index[2] = 0;
        } else {
            pick.index[2] = data->axis[2]->size - 1;
        }

    } else if (pick.iaxis[1] == 1 && pick.iaxis[2] == 3) {
        if (pick.index[3] < data->axis[3]->size / 2) {
            pick.index[3] = 0;
        } else {
            pick.index[3] = data->axis[3]->size - 1;
        }

    } else if (pick.iaxis[1] == 2 && pick.iaxis[2] == 3) {
        if (pick.index[2] < data->axis[2]->size / 2) {
            pick.index[2] = 0;
        } else {
            pick.index[2] = data->axis[2]->size - 1;
        }

    }

    for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
        pik->pik[pik->npik][iaxis] = pick.index[iaxis];
    }

    pik->npik = pik->npik < pik->nmax - 1 ? pik->npik + 1 : pik->nmax - 1;
    PikDraw(pik->npik - 1, DRAW);
    pik->last_op = PIKOP_ADD;
    pik->last_item = pik->npik - 1;
    pik->changed = 1;
}

/*--------------------------------------------------------------------*\
   delete nearest pick to mouse pick location
\*--------------------------------------------------------------------*/

void     PikDelete(int x, int y)
{

    int      inear, ipik, iaxis;

    if (!pik || !pik->npik) {
        return;
    }

    inear = PikNear(x, y);

    if (inear == NO_INDEX) {
        return;
    }

    PikDraw(inear, ERASE);

    for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
        pik->pik[pik->npik][iaxis] = pik->pik[inear][iaxis];
    }

    for (ipik = inear; ipik < pik->npik; ipik++) {

        for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
            pik->pik[ipik][iaxis] = pik->pik[ipik + 1][iaxis];
        }
    }
    pik->npik = pik->npik > 0 ? pik->npik - 1 : 0;
    pik->pik[pik->npik][0] = NO_INDEX;
    pik->last_op = PIKOP_DELETE;
    pik->last_item = pik->npik;
    pik->changed = 1;
}

/*--------------------------------------------------------------------*\
   move nearest pick to mouse pick location
\*--------------------------------------------------------------------*/

void     PikMove(int x, int y)
{

    int      inear;
    int      iaxis;
    PickPoint_ pick;

    if (!pik || !pik->npik) {
        return;
    }

    inear = PikNear(x, y);
    if (inear == NO_INDEX) {
        return;
    }

    PikDraw(inear, ERASE);
    PickDecode(x, y, &pick, 1);

    for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
        pik->pik[pik->npik][iaxis] = pik->pik[inear][iaxis];
    }

    for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
        pik->pik[inear][iaxis] = pick.index[iaxis];
    }

    PikDraw(inear, DRAW);
    pik->last_op = PIKOP_MOVE;
    pik->last_item = inear;
    pik->changed = 1;
}

/*--------------------------------------------------------------------*\
   move nearest pick to nearest edge

   This is a fragile hack because of the data model being used for
   piks.  The whole internal data model needs a serious rexamination.
\*--------------------------------------------------------------------*/

void     PikMoveEdge(int x, int y)
{

    int      inear;
    int      iaxis;
    PickPoint_ pick;
    extern Data data;

    if (!pik || !pik->npik) {
        return;
    }

    inear = PikNear(x, y);
    if (inear == NO_INDEX) {
        return;
    }

    PikDraw(inear, ERASE);
    PickDecode(x, y, &pick, 1);

    if (pick.iaxis[1] == 1 && pick.iaxis[2] == 2) {
        if (pick.index[2] < data->axis[2]->size / 2) {
            pick.index[2] = 0;
        } else {
            pick.index[2] = data->axis[2]->size - 1;
        }
        pick.index[3] = pik->pik[inear][3];

    } else if (pick.iaxis[1] == 1 && pick.iaxis[2] == 3) {
        if (pick.index[3] < data->axis[3]->size / 2) {
            pick.index[3] = 0;
        } else {
            pick.index[3] = data->axis[3]->size - 1;
        }
        pick.index[2] = pik->pik[inear][2];

    } else if (pick.iaxis[1] == 2 && pick.iaxis[2] == 3) {
        if (pick.index[2] < data->axis[2]->size / 2) {
            pick.index[2] = 0;
        } else {
            pick.index[2] = data->axis[2]->size - 1;
        }
        pick.index[3] = pik->pik[inear][3];

    }

    pick.index[0] = pik->pik[inear][0];
    pick.index[1] = pik->pik[inear][1];
    pick.index[4] = pik->pik[inear][4];
    pick.index[5] = pik->pik[inear][5];

    for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
        pik->pik[pik->npik][iaxis] = pik->pik[inear][iaxis];
    }

    for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
        pik->pik[inear][iaxis] = pick.index[iaxis];
    }

    PikDraw(inear, DRAW);
    pik->last_op = PIKOP_MOVE;
    pik->last_item = inear;
    pik->changed = 1;
}

/*--------------------------------------------------------------------*\
   undo a pick modification operation
\*--------------------------------------------------------------------*/

void     PikUndo(void)
{

    int      iaxis;
    int      save;

    switch (pik->last_op) {

       case 'a':
        PikDraw(pik->npik - 1, ERASE);
        pik->npik = pik->npik > 0 ? pik->npik - 1 : 0;
        pik->pik[pik->npik][0] = NO_INDEX;
        pik->last_op = PIKOP_DELETE;
        pik->last_item = pik->npik;
        break;

       case 'd':
        pik->pik[pik->npik][0] = 0;
        pik->npik = pik->npik < pik->nmax - 1 ? pik->npik + 1 : pik->nmax - 1;
        PikDraw(pik->npik, DRAW);
        pik->last_op = PIKOP_ADD;
        pik->last_item = pik->npik - 1;
        break;

       case 'm':
        PikDraw(pik->last_item, ERASE);

        for (iaxis = 0; iaxis < DATA_NAXIS; iaxis++) {
            save = pik->pik[pik->last_item][iaxis];
            pik->pik[pik->last_item][iaxis] = pik->pik[pik->npik][iaxis];
            pik->pik[pik->npik][iaxis] = save;
        }

        PikDraw(pik->last_item, DRAW);
        break;

       case 'u':
        printf("no undo\n");
        break;
    }
}

/*--------------------------------------------------------------------*\
   print attributes of pick list
\*--------------------------------------------------------------------*/

void     PikInfo(void)
{

    Message  message;

    sprintf(message, "Pik: file=%s npick=%d nmax=%d range=%d size=%d",
            pik->file, pik->npik, pik->nmax, pik->range, pik->size);
    UIMessage(message);
}

/*--------------------------------------------------------------------*\
   save parameters
\*--------------------------------------------------------------------*/

void     PikSave(void)
{

    Message  message;

    sprintf(message, "Pik: pick=%s npick=%d pickrange=%d picksize=%d",
            pik->file, pik->npik, pik->range, pik->size);
    UISaveMessage(message);
}

/*--------------------------------------------------------------------*\
   Increment the size of the pick markers
\*--------------------------------------------------------------------*/

int      PikIncreaseSize(void)
{

    if (!pik) {
        return 0;
    }

    PikDraw(NO_INDEX, ERASE);
    pik->size++;
    PikDraw(NO_INDEX, DRAW);
    PikInfo();
    return 0;
}

/*--------------------------------------------------------------------*\
   Decrement the size of the pick markers
\*--------------------------------------------------------------------*/

int      PikDecreaseSize(void)
{

    if (!pik) {
        return 0;
    }

    if (pik->size < 2) {
        return 0;
    }

    PikDraw(NO_INDEX, ERASE);
    pik->size--;
    PikDraw(NO_INDEX, DRAW);
    PikInfo();
    return 0;
}

/*--------------------------------------------------------------------*\
   Increment the number of frames over which a pick is shown by a box
\*--------------------------------------------------------------------*/

int      PikIncreaseRange(void)
{

    if (!pik) {
        return 0;
    }

    PikDraw(NO_INDEX, ERASE);
    pik->range++;
    PikDraw(NO_INDEX, DRAW);
    PikInfo();
    return 0;
}

/*--------------------------------------------------------------------*\
   Decrement the number of frames over which a pick is shown by a box
\*--------------------------------------------------------------------*/

int      PikDecreaseRange(void)
{

    if (!pik) {
        return 0;
    }

    if (pik->range == 0) {
        return 0;
    }

    PikDraw(NO_INDEX, ERASE);
    pik->range--;
    PikDraw(NO_INDEX, DRAW);
    PikInfo();
    return 0;
}
