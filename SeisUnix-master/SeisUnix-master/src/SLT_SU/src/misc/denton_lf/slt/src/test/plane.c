/*
code for plane object

The plane object records all planes drawn in an render
*/

#include <stdlib.h>
#include <stdio.h>

#include "par.h"

#include "main.h"
#include "axis.h"
#include "map.h"
#include "data.h"
#include "render.h"
#include "plane.h"

Plane    plane = 0, lastplane = 0, planelist = 0;

/* initialize planes */
Plane    PlaneInit(void)
{
    int      iplane;

    {   extern int _alloc;

	planelist = (Plane) malloc((NPLANE) * sizeof(planelist[0]));
	_alloc += (NPLANE) * sizeof(planelist[0]);
	if( planelist == 0 ){
	    err("cant allocate %d bytes for  planelist; %d already allocated",
		(NPLANE) * sizeof(planelist[0]), _alloc);
        }
	if( memwatch ){
	    (void) printf("malloc %s=%d\n", " planelist",
			  (NPLANE) * sizeof(planelist[0]));
        }
    }
    for( iplane = 0, plane = planelist; iplane < NPLANE; iplane++, plane++) {

	extern int _alloc;

	plane->attr = (RenderAttr) malloc((1) * sizeof(plane->attr[0]));
	_alloc += (1) * sizeof(plane->attr[0]);
	if( plane->attr == 0 ){
	    err("cant allocate %d bytes for  plane->attr; %d already allocated",
		(1) * sizeof(plane->attr[0]), _alloc);
        }
	if( memwatch ){
	    (void) printf("malloc %s=%d\n", " plane->attr",
			  (1) * sizeof(plane->attr[0]));
        }
    }

    PlaneReset();
    return (planelist);
}

/* find a plane given axis and frame */
Plane
PlaneFind(int dir3, int frame3, int dir4, int frame4, int dir5, int frame5)
{
    Plane    plane;

    for( plane = planelist; plane < planelist + NPLANE &&
	 plane->attr->orient != NO_INDEX; plane++) {
	if( AxisDir(MapAxis(plane->zmap)) == dir3 &&
	    (frame3 == NO_INDEX || plane->frame3 == frame3) &&
	    (dir4 == NO_INDEX || (AxisDir(MapAxis(plane->map4)) == dir4 &&
				  (frame4 == NO_INDEX
				   || plane->frame4 == frame4)
				  && AxisDir(MapAxis(plane->map5)) == dir5
				  && (frame5 == NO_INDEX
				      || plane->frame5 == frame5)))) {
	    return (plane);
	}
    }
    return (0);
}

/* empty the list */
void PlaneReset(void)
{
    for( plane = planelist; plane < planelist + NPLANE; plane++) {
	plane->attr->orient = NO_INDEX;
    }
    plane = planelist;
}

/* record a plane */
void PlaneSet(Map hmap, Map vmap, Map zmap, Map map4, Map map5, int h0, int v0,
	 int nh, int nv, Render render, int *margins, RenderAttr attr)
{
    plane->hmap = hmap;
    plane->vmap = vmap;
    plane->zmap = zmap;
    plane->map4 = map4;
    plane->map5 = map5;
    plane->frame3 = zmap->frame;
    plane->frame4 = map4->frame;
    plane->frame5 = map5->frame;
    plane->h0 = h0;
    plane->v0 = v0;
    plane->nh = nh;
    plane->nv = nv;
    plane->margins = margins;
    *(plane->attr) = *attr;
    lastplane = plane;
    plane++;
}

/* set frame */
void PlaneSetFrame(Plane plane, int frame3)
{
    if( !plane ){
	return;
    }
    plane->frame3 = frame3;
}

/* print list of planes */
void PlaneListInfo(void)
{
    Plane    plane;

    (void)printf("Plane list:\n");
    for( plane = planelist; plane < planelist + NPLANE &&
	 plane->attr->orient != NO_INDEX; plane++) {
	(void)printf
	    ("dir3=%d frame3=%d dir4=%d frame4=%d dir5=%d frame5=%d h0=%d v0=%d nh=%d nv=%d\n",
	     AxisDir(MapAxis(plane->zmap)), plane->frame3,
	     AxisDir(MapAxis(plane->map4)), plane->frame4,
	     AxisDir(MapAxis(plane->map5)), plane->frame5, plane->h0, plane->v0,
	     plane->nh, plane->nv);
    }
}

/* return plane dir */
int PlaneDir(Plane plane)
{
    if( !plane ){
	return (NO_INDEX);
    }
    return (AxisDir(MapAxis(plane->zmap)));
}

/* return plane frame */
int PlaneFrame(Plane plane)
{
    if( !plane ){
	return (NO_INDEX);
    }
    return (plane->frame3);
}

/* return plane rectangle */
void PlaneRect(int *h0, int *v0, int *nh, int *nv)
{
    *h0 = NO_INDEX;
    *v0 = NO_INDEX;
    *nh = NO_INDEX;
    *nv = NO_INDEX;
    if( !lastplane ){
	return;
    }
    *h0 = lastplane->h0;
    *v0 = lastplane->v0;
    *nh = lastplane->nh;
    *nv = lastplane->nv;
}

/* return plane skew */
int PlaneSkew(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->attr->skew);
}

/* return plane type */
int PlaneType(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->attr->orient);
}

/* return plane v0 */
int PlaneV0(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->v0);
}

/* return plane h0 */
int PlaneH0(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->h0);
}

/* return plane nv */
int PlaneNV(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->nv);
}

/* return plane nh */
int PlaneNH(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->nh);
}

/* return plane hmap */
Map      PlaneHmap(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->hmap);
}

/* return plane vmap */
Map      PlaneVmap(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->vmap);
}

/* return plane vmap */
Map      PlaneZmap(Plane plane)
{
    if( !plane ){
	return (0);
    }
    return (plane->zmap);
}
