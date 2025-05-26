

#ifndef PLANE_H
#define PLANE_H

/*--------------------------------------------------------------------*\
   plane object definition

   A plane is a component of an image
\*--------------------------------------------------------------------*/

/* constants */
#define NPLANE          2500

/* Plane object */
typedef struct {
    Map      hmap;              /* five map axes */
    Map      vmap;
    Map      zmap;
    Map      map4;
    Map      map5;
    int      frame3;            /* three frames of a plane */
    int      frame4;            /* three frames of a plane */
    int      frame5;            /* three frames of a plane */
    int      h0;                /* panel origin */
    int      v0;                /* panel origin */
    int      nh;                /* panel dimensions */
    int      nv;                /* panel dimensions */
    int     *margins;
    RenderAttr attr;            /* render attributes */
}       *Plane;

extern Plane plane;
extern Plane lastplane;
extern Plane planelist;

/* plane.c */
Plane PlaneInit(void);
Plane PlaneFind(int dir3, int frame3, int dir4, int frame4, int dir5, int frame5);
void PlaneReset(void);
void PlaneSet(Map hmap, Map vmap, Map zmap, Map map4, Map map5, int h0, int v0, int nh, int nv, Render render, int *margins, RenderAttr attr);
void PlaneSetFrame(Plane plane, int frame3);
void PlaneListInfo(void);
int PlaneDir(Plane plane);
int PlaneFrame(Plane plane);
void PlaneRect(int *h0, int *v0, int *nh, int *nv);
int PlaneSkew(Plane plane);
int PlaneType(Plane plane);
int PlaneV0(Plane plane);
int PlaneH0(Plane plane);
int PlaneNV(Plane plane);
int PlaneNH(Plane plane);
Map PlaneHmap(Plane plane);
Map PlaneVmap(Plane plane);
Map PlaneZmap(Plane plane);

#endif
