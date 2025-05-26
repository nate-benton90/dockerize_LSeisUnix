/*
render object code
draws raster images into various buffers
draws axis and cross-line annotations
contains optimized subroutines for rapid raster rendering
*/

#include <unistd.h>
#include <stdio.h>

#include "par.h"

#include "main.h"
#include "axis.h"
#include "data.h"
#include "color.h"
#include "draw.h"
#include "map.h"
#include "movie.h"
#include "render.h"
#include "plane.h"
#include "view.h"
#include "pick.h"
#include "rargs.h"
#include "ui_menu.h"
#include "ui_window.h"

/* C or FORTRAN renderers */
#ifndef FORTRAN
#define RENDER_BASIC_HORZ       RenderBasicHorz CALLARGS
#define RENDER_BASIC_VERT       RenderBasicVert CALLARGS
#define RENDER_SHADOW_HORZ      RenderShadowHorz CALLARGS
#define RENDER_SHADOW_VERT      RenderShadowVert CALLARGS
#define RENDER_INTERP_HORZ      RenderInterpHorz CALLARGS
#define RENDER_INTERP_VERT      RenderInterpVert CALLARGS
#define RENDER_TRANSP_HORZ      RenderTranspHorz CALLARGS
#define RENDER_TRANSP_VERT      RenderTranspVert CALLARGS
#endif
#ifdef FORTRAN
#define RENDER_BASIC_HORZ       renderbasichorz_ FCALLARGS
#define RENDER_BASIC_VERT       renderbasicvert_ FCALLARGS
#define RENDER_SHADOW_HORZ      rendershadowhorz_ FCALLARGS
#define RENDER_SHADOW_VERT      rendershadowvert_ FCALLARGS
#define RENDER_INTERP_HORZ      renderinterphorz_ FCALLARGS
#define RENDER_INTERP_VERT      renderinterpvert_ FCALLARGS
#define RENDER_TRANSP_HORZ      rendertransphorz_ FCALLARGS
#define RENDER_TRANSP_VERT      rendertranspvert_ FCALLARGS
#endif
#define CORD3   AxisDir(MapAxis(zmap)),MapFrame(zmap),\
                AxisDir(MapAxis(map4)),MapFrame(map4),\
                AxisDir(MapAxis(map5)),MapFrame(map5)
#define CORD3P  AxisDir(MapAxis(zmap)),MapPrev(zmap),\
                AxisDir(MapAxis(map4)),MapFrame(map4),\
                AxisDir(MapAxis(map5)),MapFrame(map5)

/* initialize render object */
Render   RenderInit(void)
{
    Render   render;

    {    extern int _alloc;

        render = (Render) malloc((1) * sizeof(render[0]));
        _alloc += (1) * sizeof(render[0]);
        if (render == 0 ){
            err("cant allocate %d bytes for var; %d already allocated",
                (1) * sizeof(render[0]), _alloc);
        }
        if (memwatch ){
            (void) printf("malloc %s=%d\n", "var", (1) * sizeof(render[0]));
        }
    };
    render->wide = 0;
    render->hite = 0;
    render->interp = 0;
    render->polarity = RENDER_POLARITY;
    render->fence_transp = RENDER_FENCE_TRANSP;
    render->low_transp = RENDER_LOW_TRANSP;
    render->high_transp = RENDER_HIGH_TRANSP;
    render->vol_transp = RENDER_VOL_TRANSP;
    getparint("interpolate", &render->interp);
    getparint("fence_transp", &render->fence_transp);
    getparint("vol_transp", &render->vol_transp);
    render->image = 0;
    render->shadow = 0;
    render->zbuffer = 0;
    return (render);
}

/* adjust render size: reallocate buffers, adjust map axes */
void RenderSize(int wide, int hite)
{
    extern Render render;

    if (!render ){
        return;
    }
    /* reallocate buffers */
    if (wide != render->wide || hite != render->hite ){
        render->wide = wide;
        render->hite = hite;

        if (render->image ){
            free(render->image);
            render->image = 0;
            if (memwatch ){
                printf("free %s\n", "var");
            }
        };
        {   extern int _alloc;

            render->image =
                (Buffer) malloc((render->wide * render->hite) *
                                sizeof(render->image[0]));
            _alloc += (render->wide * render->hite) * sizeof(render->image[0]);
            if (render->image == 0 ){
                err("cant allocate %d bytes for var; %d already allocated",
                    (render->wide * render->hite) * sizeof(render->image[0]),
                    _alloc);
            }
            if (memwatch ){
                (void) printf("malloc %s=%d\n", "var",
                              (render->wide * render->hite) *
                              sizeof(render->image[0]));
            }
        };
        if (render->zbuffer ){
            free(render->zbuffer);
            render->zbuffer = 0;
            if (memwatch ){
                printf("free %s\n", "var");
            }
        };
        {
            extern int _alloc;

            render->zbuffer =
                (Zbuffer) malloc((render->wide * render->hite) *
                                 sizeof(render->zbuffer[0]));
            _alloc +=
                (render->wide * render->hite) * sizeof(render->zbuffer[0]);
            if (render->zbuffer == 0 ){
                err("cant allocate %d bytes for var; %d already allocated",
                    (render->wide * render->hite) * sizeof(render->zbuffer[0]),
                    _alloc);
            }
            if (memwatch ){
                (void) printf("malloc %s=%d\n", "var",
                              (render->wide * render->hite) *
                              sizeof(render->zbuffer[0]));
            }
        };
        if (render->shadow ){
            free(render->shadow);
            render->shadow = 0;
            if (memwatch ){
                printf("free %s\n", "var");
            }
        };
        {   extern int _alloc;

            render->shadow =
                (Shadow) malloc((render->wide * render->hite) *
                                sizeof(render->shadow[0]));
            _alloc += (render->wide * render->hite) * sizeof(render->shadow[0]);
            if (render->shadow == 0 ){
                err("cant allocate %d bytes for var; %d already allocated",
                    (render->wide * render->hite) * sizeof(render->shadow[0]),
                    _alloc);
            }
            if (memwatch ){
                (void) printf("malloc %s=%d\n", "var",
                              (render->wide * render->hite) *
                              sizeof(render->shadow[0]));
            }
        };
    }
    RenderClear();
}

/*--------------------------------------------------------------------*\
   extract plane in horizontal scan mode; given data, render buffers,
   three map attr->axes, origin, options to attr->image attr->skew,
   data, attr->axes, attr->shadow, and line
\*--------------------------------------------------------------------*/

void RenderHorz(Data data, Map hmap, Map vmap, Map zmap, Map map4, Map map5, int h0,
           int v0, Render render, int *margins, RenderAttr attr)
{
    int      x, y, transp;
    float    tic;
    string   label;

    if (!data || !render || !hmap || !hmap->map ){
        return;
    }
    attr->shadow = 1;
    /* clear previous pick */
    if (attr->pick && attr->mark == 0 ){
        PickDraw(PickFind(CORD3P), ERASE);
    }

    /* record plane */
    attr->orient = RENDER_HORZ;
    if (attr->image ){
        render->h0 = h0;
        render->v0 = v0;
        render->nh =
            MapSize(hmap) + attr->skew * (MapSize(vmap) + GAP) +
            attr->transp * MapSize(zmap);
        render->nh =
            render->wide - render->h0 <
            render->nh ? render->wide - render->h0 : render->nh;
        render->nv = MapSize(vmap) + attr->transp * MapSize(zmap);
        render->nv =
            render->hite - render->v0 <
            render->nv ? render->wide - render->v0 : render->nv;
    }
    if (attr->mark ){
        PlaneSet(hmap, vmap, zmap, map4, map5, render->h0, render->v0,
                 render->nh, render->nv, render, margins, attr);
    }else {
        PlaneSetFrame(PlaneFind(CORD3P), MapFrame(zmap));
    }

    /* extract images */
    if (attr->image ){
        /* update window */
        if (attr->zbuffer ){
            if (!strcmp(MapName(zmap), "DEEP") ){
                if (render->interp ){
                    RenderFrontFenceInterpHorz CALLARGS;

                }else{
                    RenderFrontFenceHorz CALLARGS;
                }
            }
            if (!strcmp(MapName(zmap), "DOWN") ){
                if (render->interp ){
                    RenderTopFenceInterpHorz CALLARGS;

                }else{
                    RenderTopFenceHorz CALLARGS;
                }
            }
        } else if (attr->transp ){
            transp = render->fence_transp * DataValueSize(data) * .01;
            RENDER_TRANSP_HORZ;
        } else if (render->interp ){
            RENDER_INTERP_HORZ;
        }else{
            RENDER_BASIC_HORZ;
        }
        if (attr->shadow && !attr->zbuffer ){
            RENDER_SHADOW_HORZ;
        }
    }

    /* draw crosslines */
    if (attr->lines ){
        /* erase horizontal */
        x = h0 + attr->skew * (MapSize(vmap) - MapInverse(vmap, MapPrev(vmap)));
        y = v0 + MapInverse(vmap, MapPrev(vmap));
        DrawLine(x, y, x + MapSize(hmap), y, ERASE);
        /* erase vertical */
        x = h0 + MapInverse(hmap, MapPrev(hmap));
        DrawLine(x + attr->skew * MapSize(vmap), v0, x, v0 + MapSize(vmap),
                 ERASE);
        /* draw horizontal */
        x = h0 + attr->skew * (MapSize(vmap) -
                               MapInverse(vmap, MapFrame(vmap)));
        y = v0 + MapInverse(vmap, MapFrame(vmap));
        DrawLine(x, y, x + MapSize(hmap), y, DRAW);
        /* draw vertical */
        x = h0 + MapInverse(hmap, MapFrame(hmap));
        DrawLine(x + attr->skew * MapSize(vmap), v0, x, v0 + MapSize(vmap),
                 DRAW);
    }

    /* draw box */
    if (attr->box ){
        DrawLine(h0 - 1 + attr->skew * MapSize(vmap), v0 - 1,
                 h0 + MapSize(hmap) + attr->skew * MapSize(vmap), v0 - 1, DRAW);
        DrawLine(h0 - 1, v0 + MapSize(vmap), h0 + MapSize(hmap),
                 v0 + MapSize(vmap), DRAW);
        DrawLine(h0 - 1 + attr->skew * (MapSize(vmap) + 1), v0 - 1, h0 - 1,
                 v0 + MapSize(vmap), DRAW);
        DrawLine(h0 + MapSize(hmap) + attr->skew * (MapSize(vmap) + 1), v0 - 1,
                 h0 + MapSize(hmap), v0 + MapSize(vmap), DRAW);
    }

    /* draw pick */
    if (attr->pick ){
        PickDraw(PickFind(CORD3), DRAW);
    }

    /* left tics */
    if (attr->axes & AXIS_LEFT ){
        for( tic = MapTic0(vmap); tic <= MapTic2(vmap) + .5 * MapDtic(vmap);
             tic += MapDtic(vmap) ){
            y = v0 + MapIndex(vmap, tic);
            x = h0 + attr->skew * (MapSize(vmap) - MapIndex(vmap, tic));
            sprintf(label, MapFormat(vmap), tic);
            DrawText(x - TIC_LENGTH - 1, y - 5, TEXT_V0 | TEXT_H100, label);
            DrawLine(x - TIC_LENGTH, y, x, y, DRAW);
        }
        /* label */
        if (MapNtic(vmap) == 1 ){

/*                      x = h0 - margins[MARGIN_LEFT] + attr->skew * MapSize(vmap) / 2;*/
            x = h0 - 5 + attr->skew * MapSize(vmap) / 2;
            y = v0 + MapSize(vmap) / 2;
        } else {
            tic = (MapTic0(vmap) + MapTic2(vmap) +
                   (MapNtic(vmap) % 2) * MapDtic(vmap)) / 2;
            y = v0 + MapIndex(vmap, tic);

/*
                        x = h0 + attr->skew * (MapSize(vmap) -
                                MapIndex(vmap,tic)) - margins[MARGIN_LEFT];
*/
            x = h0 - 5 + attr->skew * (MapSize(vmap) - MapIndex(vmap, tic));
        }
        DrawText(x, y, TEXT_V50 | TEXT_H100, AxisLabel(MapAxis(vmap)));
        /* border line */
        DrawLine(h0 - 1 + attr->skew * MapSize(vmap), v0, h0 - 1,
                 v0 + MapSize(vmap), DRAW);
    }

    /* base tics */
    if (attr->axes & AXIS_BASE ){
        for( tic = MapTic0(hmap); tic < MapTic2(hmap) + .5 * MapDtic(hmap);
             tic += MapDtic(hmap) ){
            x = h0 + MapIndex(hmap, tic);
            y = v0 + MapSize(vmap);
            sprintf(label, MapFormat(hmap), tic);
            DrawText(x - 10, y + TIC_LENGTH, TEXT_V0 | TEXT_H0, label);
            DrawLine(x, y, x, y + TIC_LENGTH, DRAW);
        }
        /* label */
        if (MapNtic(hmap) == 1 ){
            x = h0 + MapSize(hmap) / 2;
            y = v0 + margins[MARGIN_BASE] + MapSize(vmap);
        } else {
            tic = (MapTic0(hmap) + MapTic2(hmap) +
                   (MapNtic(hmap) % 2) * MapDtic(hmap)) / 2;
            x = h0 + MapIndex(hmap, tic);
            y = v0 + MapSize(vmap) + margins[MARGIN_BASE];
        }
        DrawText(x, y, TEXT_V100 | TEXT_H50, AxisLabel(MapAxis(hmap)));
        /* border line */
        DrawLine(h0, v0 + MapSize(vmap), h0 + MapSize(hmap),
                 v0 + MapSize(vmap), DRAW);
    }
}

/* extract panel in vertical scan line mode; permits vertical attr->skewing for side
of cubes */
void RenderVert(Data data, Map hmap, Map vmap, Map zmap, Map map4, Map map5, int h0,
           int v0, Render render, int *margins, RenderAttr attr)
{
    int      x, y, transp;
    float    tic;
    string   label;

    if (!data || !render || !hmap || !hmap->map ){
        return;
    }
    attr->shadow = 1;
    /* clear picks */
    if (attr->pick && attr->mark == 0 ){
        PickDraw(PickFind(CORD3P), ERASE);
    }

    /* record plane */
    attr->orient = RENDER_VERT;
    if (attr->image ){
        render->h0 = h0;
        render->v0 = v0 - attr->skew * (MapSize(hmap) + GAP);
        render->nh = MapSize(hmap) + attr->transp * MapSize(zmap);
        render->nv =
            MapSize(vmap) + attr->skew * (MapSize(hmap) + GAP) +
            attr->transp * MapSize(zmap);
        render->nh =
            render->wide - render->h0 <
            render->nh ? render->wide - render->h0 : render->nh;
        render->nv =
            render->hite - render->v0 <
            render->nv ? render->wide - render->v0 : render->nv;
    }
    if (attr->mark ){
        PlaneSet(hmap, vmap, zmap, map4, map5, render->h0, render->v0,
                 render->nh, render->nv, render, margins, attr);
    }else {
        PlaneSetFrame(PlaneFind(CORD3P), MapFrame(zmap));
    }

    /* extract image */
    if (attr->image ){
        /* update window */
        if (attr->zbuffer ){
            if (!strcmp(MapName(zmap), "ACROSS") ){
                if (render->interp ){
                    RenderSideFenceInterpVert CALLARGS;

                }else{
                    RenderSideFenceVert CALLARGS;
                }
            }
        } else if (attr->transp ){
            transp = render->fence_transp * DataValueSize(data) * .01;
            RENDER_TRANSP_VERT;
        } else if (render->interp ){
            RENDER_INTERP_VERT;
        }else{
            RENDER_BASIC_VERT;
        }
        if (attr->shadow && !attr->zbuffer ){
            RENDER_SHADOW_VERT;
        }
    }

    /* draw cross lines */
    if (attr->lines ){
        /* erase horizontal line */
        y = v0 + MapInverse(vmap, MapPrev(vmap));
        DrawLine(h0, y, h0 + MapSize(hmap), y - attr->skew * MapSize(hmap),
                 ERASE);
        /* erase vertical line */
        x = h0 + MapSize(hmap) - MapInverse(hmap, MapPrev(hmap));
        y = v0 - attr->skew * (x - h0);
        DrawLine(x, y, x, y + MapSize(vmap), ERASE);
        /* draw horizontal line */
        y = v0 + MapInverse(vmap, MapFrame(vmap));
        DrawLine(h0, y, h0 + MapSize(hmap), y - attr->skew * MapSize(hmap),
                 DRAW);
        /* draw vertical line */
        x = h0 + MapSize(hmap) - MapInverse(hmap, MapFrame(hmap));
        y = v0 - attr->skew * (x - h0);
        DrawLine(x, y, x, y + MapSize(vmap), DRAW);
    }

    /* draw box */
    if (attr->box ){
        DrawLine(h0 - 1, v0 - 1, h0 + MapSize(hmap),
                 v0 - 1 - attr->skew * (MapSize(hmap) + 1), DRAW);
        DrawLine(h0 - 1, v0 + MapSize(vmap) + 1, h0 + MapSize(hmap),
                 v0 + MapSize(vmap) + 1 - attr->skew * (MapSize(hmap) + 1),
                 DRAW);
        DrawLine(h0 - 1, v0, h0 - 1, v0 + MapSize(vmap), DRAW);
        DrawLine(h0 + MapSize(hmap), v0 + 1 - attr->skew * MapSize(hmap),
                 h0 + MapSize(hmap),
                 v0 + 1 + MapSize(vmap) - attr->skew * MapSize(hmap), DRAW);
    }

    /* draw picks */
    if (attr->pick ){
        PickDraw(PickFind(CORD3), DRAW);
    }

    /* left tics */
    if (attr->axes & AXIS_LEFT ){
        for( tic = MapTic0(vmap); tic <= MapTic2(vmap) + .5 * MapDtic(vmap);
             tic += MapDtic(vmap) ){
            y = v0 + MapIndex(vmap, tic);
            x = h0;
            sprintf(label, MapFormat(hmap), tic);
            DrawText(x - TIC_LENGTH, y - 5, TEXT_V0 | TEXT_H100, label);
            DrawLine(x - TIC_LENGTH, y, x, y, DRAW);
        }
        /* label */
        if (MapNtic(vmap) == 1 ){
            x = h0 - margins[MARGIN_LEFT];
            y = v0 + MapSize(vmap) / 2;
        } else {
            tic = (MapTic0(vmap) + MapTic2(vmap) +
                   (MapNtic(vmap) % 2) * MapDtic(vmap)) / 2;
            y = v0 + MapIndex(vmap, tic);
            x = h0 + -margins[MARGIN_LEFT];
        }
        DrawText(x, y, TEXT_V50 | TEXT_H0, AxisLabel(MapAxis(vmap)));
        /* border line */
        DrawLine(h0 - 1, v0, h0 - 1, v0 + MapSize(vmap), DRAW);
    }

    /* base tics */
    if (attr->axes & AXIS_BASE ){
        for( tic = MapTic0(hmap); tic <= MapTic2(hmap) + .5 * MapDtic(hmap);
             tic += MapDtic(hmap) ){
            x = h0 + MapSize(hmap) - MapIndex(hmap, tic);
            y = v0 + MapSize(vmap) - attr->skew * (MapSize(hmap) -
                                                   MapIndex(hmap, tic));
            sprintf(label, MapFormat(hmap), tic);
            if (attr->skew ){
                DrawText(x + TIC_LENGTH + 1, y, TEXT_V50 | TEXT_H0, label);
                DrawLine(x, y, x + TIC_LENGTH, y, DRAW);
            } else {
                DrawText(x - 10, y + TIC_LENGTH, TEXT_V0 | TEXT_H0, label);
                DrawLine(x, y, x, y + TIC_LENGTH, DRAW);
            }
        }
        /* label */
        if (MapNtic(hmap) == 1 ){
            x = h0 + MapSize(hmap) / 2;
            y = v0 + (attr->skew > 0 ? 0 : 1) * margins[MARGIN_BASE] +
                MapSize(vmap) - attr->skew * MapSize(hmap) / 2;
        } else {
            tic = (MapTic0(hmap) + MapTic2(hmap) +
                   (MapNtic(hmap) % 2) * MapDtic(hmap)) / 2;
            x = h0 + MapSize(hmap) - MapIndex(hmap, tic);
            y = v0 + MapSize(vmap) + (attr->skew > 0 ? 0 : 1) *
                margins[MARGIN_BASE] - attr->skew * (MapSize(hmap) -
                                                     MapIndex(hmap, tic));
        }
        if (attr->skew ){
            DrawText(x, y + TIC_LENGTH, TEXT_V0 | TEXT_H0,
                     AxisLabel(MapAxis(hmap)));
        }else{
            DrawText(x, y, TEXT_V100 | TEXT_H50, AxisLabel(MapAxis(hmap)));
        }
        /* border line */
        DrawLine(h0, v0 + MapSize(vmap), h0 + MapSize(hmap),
                 v0 + MapSize(vmap) - attr->skew * MapSize(hmap), DRAW);
    }
}

/* extract plane along pick line */
void RenderDeep(Data data, Map hmap, Map vmap, Map zmap, Map map4, Map map5, int h0,
           int v0, Render render, int *margins, RenderAttr attr)
{
    Map_     hmap1, vmap1, zmap1;
    extern PickLine lastpick;
    int      ipick, idim, rmap;
    float    arclen = 0;
    float  darc, dlen, dseg, seglen[NPICK], interp;
    extern double sqrt(double);

    if (!lastpick || PickCount(lastpick) < 2 ){
        return; /* no active pick */
    }
    /* compute new map maps based on supplied maps */
    /* hmap is zmap resized into zmap */
    /* vmap is replaced by pick map */
    /* zmap is inactive */
    hmap1 = *zmap;
    vmap1 = *vmap;
    zmap1 = *zmap;

    if (vmap1.map ){
        free(vmap1.map);
        vmap1.map = 0;
        if (memwatch ){
            printf("free %s\n", "var");
        }
    };
    {   extern int _alloc;

        vmap1.map = (Vec) malloc((MapSize(&vmap1)) * sizeof(vmap1.map[0]));
        _alloc += (MapSize(&vmap1)) * sizeof(vmap1.map[0]);
        if (vmap1.map == 0 ){
            err("cant allocate %d bytes for var; %d already allocated",
                (MapSize(&vmap1)) * sizeof(vmap1.map[0]), _alloc);
        }
        if (memwatch ){
            (void) printf("malloc %s=%d\n", "var",
                          (MapSize(&vmap1)) * sizeof(vmap1.map[0]));
        }
    };
    hmap1.map = 0;
    MapSetSize(&hmap1, MapSize(&zmap1));
    zmap1.frame = 0;

    /* compute pick segment length in samples */
    for( ipick = 1; ipick < PickCount(lastpick) - 1; ipick++ ){
        for( seglen[ipick] = 0, idim = 0; idim < 3; idim++ ){
            seglen[ipick] += (PickIndex(lastpick, ipick, idim) -
                              PickIndex(lastpick, ipick + 1, idim)) *
                (PickIndex(lastpick, ipick, idim) -
                 PickIndex(lastpick, ipick - 1, idim));
        }
        seglen[ipick] = sqrt(seglen[ipick]);
        arclen += seglen[ipick];
    }
    /* compute interpolation axis */
    dlen = arclen / MapSize(&vmap1);
    for( darc = 0, dseg = 0, rmap = 0, ipick = 0; rmap < MapSize(&vmap1);
         rmap++, dseg += dlen ){
        /* check to see if in new segment of arc */
        if (dseg >= darc ){
            darc += seglen[ipick];
            ipick++;
        }
        /* interption along current arc segment */
        interp = (darc - dseg) / seglen[ipick - 1];
        vmap1.map[rmap] = 0;
        /* add up interptional data offsets of two pick dimensions */
        for( idim = 1; idim < 4; idim++ ){
            /* skip perpendicular dimension */
            if (idim != PickDir(lastpick) ){
                vmap1.map[rmap] += (int) (interp *
                                          PickIndex(lastpick, ipick, idim) +
                                          (1. - interp) * PickIndex(lastpick,
                                                                    ipick - 1,
                                                                    idim)) *
                    AxisStride(DataAxis(data, idim));
            }
        }
    }
    /* plot it */
    attr->orient = RENDER_DEEP;

    RenderHorz(data, &hmap1, &vmap1, &zmap1, map4, map5, h0, v0, render,
               margins, attr);

/*--------------------------------------------------------------------*\

int RenderHorz(
     Data data 
    ,Map hmap 
    ,Map vmap 
    ,Map zmap 
    ,Map map4 
    ,Map map5 
    ,int h0 
    ,int v0 
    ,Render render 
    ,int margins[] 
    ,RenderAttr attr
);
\*--------------------------------------------------------------------*/

/*      (attr)  0,0,1,AXIS_BASE,1,1,1,0,margins*/
}

/* return buffer value */
int RenderBufferValue(Render render, int x, int y)
{
    if (!render ){
        return (0);
    }

    return (render->image[render->wide * y + x]);
}

/* return shadow value */
int RenderShadowValue(Render render, int x, int y)
{
    if (!render ){
        return (NO_INDEX);
    }

    if (x < 0 || x >= render->wide || y < 0 || y >= render->hite ){
        return (NO_INDEX);
    }
    return (render->shadow[render->wide * y + x]);
}

/* return buffer */
Buffer   RenderBuffer(Render render)
{

    if (!render ){
        return (0);
    }
    return (render->image);
}

/* draw render */
void RenderDraw(void)
{
    extern Render render;

    if (!render || UIFirst() ){
        return;
    }
    DrawMask(254);
    if (MovieRun() ){
        DrawImage(render, render->h0, render->v0, render->nh, render->nv);
    } else {
        DrawImage(render, 0, 0, render->wide, render->hite);
    }
}

/* print render information */
void RenderInfo(Render render)
{
    Message  message;

    if (!render ){
        return;
    }
    sprintf(message,
            "Render: wide=%d hite=%d interpolate=%d fence_transp=%d low_transp=%d high_transp=%d vol_transp=%d",
            render->wide, render->hite, render->interp, render->fence_transp,
            render->low_transp, render->high_transp, render->vol_transp);
    UIMessage(message);
}

/* save render parameters */
void RenderSavePar(void)
{
    Message  message;
    extern Render render;

    if (!render ){
        return;
    }
    sprintf(message,
            "Render: wide=%d hite=%d interpolate=%d fence_transp=%d low_transp=%d high_transp=%d vol_transp=%d",
            render->wide, render->hite, render->interp, render->fence_transp,
            render->low_transp, render->high_transp, render->vol_transp);
    UISaveMessage(message);
}

/* rasterize overlay line; OBSELETE */
void RenderLine(Render render, int x0, int y0, int x1, int y1, int color)
{
    register Buffer bp, be;
    register int inc;

    if (!render ){
        return;
    }
    if (y0 == y1 ){     /* horzontal line */
        bp = render->image + render->wide * y0 + (x0 < x1 ? x0 : x1);
        be = render->image + render->wide * y0 + (x0 > x1 ? x0 : x1);
        inc = 1;
    } else if (x0 == x1 ){      /* vertical line */
        bp = render->image + render->wide * (y0 < y1 ? y0 : y1) + x0;
        be = render->image + render->wide * (y0 > y1 ? y0 : y1) + x0;
        inc = render->wide;
    } else {    /* diagonal line */
        if (y1 < y0 ){
            inc = y0;
            y0 = y1;
            y1 = inc;
            inc = x0;
            x0 = x1;
            x1 = inc;
        }
        bp = render->image + render->wide * y0 + x0;
        be = render->image + render->wide * y1 + x1;
        inc = render->wide + (x1 > x0 ? 1 : -1);
    }
    if (!color ){       /* erase */
        for( ; bp < be; bp += inc ){
            *bp &= 254;
        }
    } else {    /* draw */
        for( ; bp < be; bp += inc ){
            *bp |= 1;
        }
    }
}

void RenderSetInterp(int mode)
{
    extern Render render;

    if (!render ){
        return;
    }
    render->interp = mode;
}

/* set color mapping polarity */
void RenderTogglePolarity(void)
{
    extern Render render;

    if (!render ){
        return;
    }
    render->polarity *= -1;
    RenderMap(render);
    ViewDrawAll();
}

/* toggle interpolation state */
void RenderToggleInterp(void)
{
    extern Render render;

    if (!render ){
        return;
    }
    render->interp = 1 - render->interp;
}

/* set fence transparency cutoff value */
void RenderSetFenceTransp(int transparency)
{
    extern Render render;

    if (!render ){
        return;
    }
    render->fence_transp = transparency;
}

/* set volume transp low */
void RenderSetLow(int low)
{
    extern Render render;

    if (!render ){
        return;
    }
    render->low_transp = low;
    RenderMap(render);
}

/* set volume transp high */
void RenderSetHigh(int high)
{
    extern Render render;

    if (!render ){
        return;
    }
    render->high_transp = high;
    RenderMap(render);
}

/* set volume transp vol_transp */
void RenderSetGradient(int vol_transp)
{
    extern Render render;

    if (!render ){
        return;
    }
    render->vol_transp = vol_transp;
    RenderMap(render);
}

/* clean render buffers */
void RenderClear(void)
{
    extern Render render;
    register Shadow sp, se;
    register back;

    back = ColorBackground();






    {register byte bp, be; for( bp=(byte) render->image , be=bp+(  render->wide * render->hite )*sizeof( render->image [0]); bp<be;) *bp++ =   back ;} ;
    {register byte bp, be; for( bp=(byte) render->zbuffer , be=bp+(  render->wide * render->hite )*sizeof( render->zbuffer [0]); bp<be;) *bp++ =   0 ;} ;
    if (!MovieRun() ){
        for( sp = render->shadow, se = sp + render->wide * render->hite;
             sp < se; ){
            *sp++ = NO_INDEX;
        }
    }
}

/* build lookup map for render compositing */
void RenderMap(Render render)
{
    int      cbase, csize, cback, cmark, im, id, value, dbase, dsize, id1, im1;
    double   pow(double, double), power;
    extern Data data;

    cback = ColorBackground();
    cbase = ColorBase();
    csize = ColorSize();
    cmark = ColorMark();
    dbase = DataValueBase(data);
    dsize = DataValueSize(data);
    /* opaque color map */
    for( id = 0; id < dsize; id++ ){
        value = (id * csize) / dsize;
        if (render->polarity < 0 ){
            value = csize - value - 1;
        }
        /* leave odd colors free for overlay */
        render->cmap[id + dbase] = cbase + (value / 2) * 2;
    }
    for( id = 0; id < dbase; id++ ){
        render->cmap[id] = cback;
    }
    for( id = dbase + dsize; id < 128; id++ ){
        render->cmap[id] = cback;
    }
    for( id = 128; id < 256; id++ ){
        render->cmap[id] = cmark;
    }
    /* marked data, i.e. > dsize, revert to maximum value */
    for( value = ((cbase + csize) / 2) * 2 + 2; id < 256; id++ ){
        render->cmap[id] = value;
    }
    if (render->vol_transp >= 100 ){
        for( im = 0; im < 256; im++ ){
            im1 = im > cbase ? im : cbase;
            for( id = 0; id < 256; id++ ){
                id1 = render->cmap[id];
                render->tmap[(im << 8) + id] = im1 > id1 ? im1 : id1;
            }
        }
        return;
    } else {
        power = render->vol_transp / 11.0 + 0.5;
        for( id = 0; id < dsize - 1; id++ ){
            render->alpha[id] = pow((1. * id) / dsize, power);
            render->alpha[id] =
                .01 * (render->low_transp +
                       (render->high_transp -
                        render->low_transp) * render->alpha[id]);
        }
        for( ; id < 256; id++ ){
            render->alpha[id] = 1.0;
        }
        /* special case of zero values */
        for( id = 0; id <= dsize; id++ ){
            value = render->cmap[(int) (render->alpha[id] * id)];
            render->tmap[id] = value > cbase ? value : cbase;
        }
        value = ((cbase + csize) / 2) * 2;
        for( ; id < 256; id++ ){
            render->tmap[id] = value;
        }
        for( im = 0; im < 256; im++ ){{
            render->tmap[im << 8] = im > cbase ? im : cbase;
        }
        }
        for( im = 0; im < 256; im++ ){
            im1 = im > cbase ? im : cbase;
            for( id = 0; id < 256; id++ ){
                id1 = render->cmap[id];
                value = (1 - render->alpha[id]) * im1 + render->alpha[id] * id1;
                render->tmap[(im << 8) + id] = value > cbase ? value : cbase;
            }
        }
    }
}

/* write render maps to files for debug purposes */
void RenderMapDump(void)
{
    extern Render render;
    int      fd;

    fd = creat("render.map.256x8", 0664);
    write(fd, render->cmap, 256);
    close(fd);

    fd = creat("render.map.256x256x8", 0664);
    write(fd, render->tmap, 65536);
    close(fd);

    UIMessage("render maps dumped");
}

/* write render images to files for debug purposes */
void RenderImageDump(void)
{
    extern Render render;
    string   filename;
    int      fd;

    sprintf(filename, "render.image.%dx%dx8", render->wide, render->hite);
    fd = creat(filename, 0664);
    write(fd, render->image,
          render->wide * render->hite * sizeof(render->image[0]));
    close(fd);

    sprintf(filename, "render.shadow.%dx%dx32", render->wide, render->hite);
    fd = creat(filename, 0664);
    write(fd, render->shadow,
          render->wide * render->hite * sizeof(render->shadow[0]));
    close(fd);

    sprintf(filename, "render.zbuffer.%dx%dx16", render->wide, render->hite);
    fd = creat(filename, 0664);
    write(fd, render->zbuffer,
          render->wide * render->hite * sizeof(render->zbuffer[0]));
    close(fd);

    UIMessage("render images dumped");
}

/* return last region rendered */
void RenderRect(int *h0, int *v0, int *nh, int *nv)
{
    extern Render render;

    *h0 = NO_INDEX;
    *v0 = NO_INDEX;
    *nh = NO_INDEX;
    *nv = NO_INDEX;
    if (!render ){
        return;
    }
    *h0 = render->h0;
    *v0 = render->v0;
    *nh = render->nh;
    *nv = render->nv;
}

/* optimized rendering routines */

void RenderBasicHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                unsigned char *map, unsigned char *tmap, int wide, int hite,
                int v0, int h0, int fence_transp, int hsize, Vec hmap,
                Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                int vstride, int zsize, int zframe, int zstride, int zdir,
                int zinv, int frame4, int stride4, int frame5, int stride5,
                int skew)
{
    Buffer   vdata;
    register Buffer hdata;
    Buffer   vimage;
    register Buffer himage;
    register Buffer eimage;
    register Vec hmap0;
    register Vec emap;
    register Buffer cmap;
    int      iv;

    if( ! vmap ){
       return;
    }
    vdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    vimage = image + wide * v0 + h0 + vsize * skew;
    eimage = image + wide * hite;
    emap = hmap + hsize;
    cmap = map;
    for( iv = 0; iv < vsize; iv++, vmap++ ){
        hdata = vdata + *vmap;
        for( hmap0 = hmap, himage = vimage; hmap0 < emap && himage < eimage; ){
            *himage++ = cmap[hdata[*hmap0++]];
        }
        vimage += wide - skew;
    }
}

void RenderBasicVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                unsigned char *map, unsigned char *tmap, int wide, int hite,
                int v0, int h0, int fence_transp, int hsize, Vec hmap,
                Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                int vstride, int zsize, int zframe, int zstride, int zdir,
                int zinv, int frame4, int stride4, int frame5, int stride5,
                int skew)
{
    register Buffer vdata;
    Buffer   hdata;
    register Buffer vimage;
    Buffer   himage;
    register Buffer eimage;
    register Vec vmap0;
    register Vec emap;
    register Buffer cmap;
    int      ih;
    register int vstep;

    hdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    himage = image + wide * v0 + h0;
    eimage = image + wide * hite;
    emap = vmap + vsize;
    vstep = wide;
    cmap = map;
    for( ih = 0, hmap += hsize - 1; ih < hsize; ih++, hmap-- ){
        vdata = hdata + *hmap;
        for( vmap0 = vmap, vimage = himage;
             vmap0 < emap && vimage < eimage; vimage += vstep ){
            *vimage = cmap[vdata[*vmap0++]];
        }
        himage += 1 - skew * wide;
    }
}

void RenderShadowHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                 unsigned char *map, unsigned char *tmap, int wide, int hite,
                 int v0, int h0, int fence_transp, int hsize, Vec hmap,
                 Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                 int vstride, int zsize, int zframe, int zstride, int zdir,
                 int zinv, int frame4, int stride4, int frame5, int stride5,
                 int skew)
{
    int      vindex;
    register int hindex;
    Shadow   vshadow;
    register Shadow hshadow;
    register Shadow eshadow;
    register Vec hmap0;
    register Vec emap;
    int      iv;


    if( ! vmap ){
       return;
    }

    vshadow = shadow + wide * v0 + h0 + vsize * skew;
    eshadow = shadow + wide * hite;
    vindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    emap = hmap + hsize;
    for( iv = 0; iv < vsize; iv++, vmap++ ){
        hindex = vindex + *vmap;
        for( hmap0 = hmap, hshadow = vshadow;
             hmap0 < emap && hshadow < eshadow; ){
            *hshadow++ = (hindex + *hmap0++);
        }
        vshadow += wide - skew;
    }
}

void RenderShadowVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                 unsigned char *map, unsigned char *tmap, int wide, int hite,
                 int v0, int h0, int fence_transp, int hsize, Vec hmap,
                 Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                 int vstride, int zsize, int zframe, int zstride, int zdir,
                 int zinv, int frame4, int stride4, int frame5, int stride5,
                 int skew)
{
    register Buffer vdata;
    Buffer   hdata;
    register Buffer vimage;
    Buffer   himage;
    register Buffer eimage;
    register int vindex;
    int      hindex;
    register Shadow vshadow;
    Shadow   hshadow;
    register Shadow eshadow;
    register Zbuffer vzbuffer;
    Zbuffer  hzbuffer;
    register Vec vmap0;
    register Vec emap;
    register Buffer cmap;
    int      ih;
    register int z;
    register Vec vinterp0;
    register int vstep;

    hdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    himage = image + wide * v0 + h0;
    hshadow = shadow + wide * v0 + h0;
    eshadow = shadow + wide * hite;
    hzbuffer = zbuffer + wide * v0 + h0;
    hindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = vmap + vsize;
    vstep = wide;
    cmap = map;
    for( ih = 0, hmap += hsize - 1; ih < hsize; ih++, hmap-- ){
        vindex = hindex + *hmap;
        for( vmap0 = vmap, vshadow = hshadow;
             vmap0 < emap && vshadow < eshadow; vshadow += vstep ){
            *vshadow = (vindex + *vmap0++);
        }
        hshadow += 1 - skew * wide;
    }
}

void RenderInterpHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                 unsigned char *map, unsigned char *tmap, int wide, int hite,
                 int v0, int h0, int fence_transp, int hsize, Vec hmap,
                 Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                 int vstride, int zsize, int zframe, int zstride, int zdir,
                 int zinv, int frame4, int stride4, int frame5, int stride5,
                 int skew)
{
    Buffer   vdata;
    register Buffer hdata;
    Buffer   vimage;
    register Buffer himage;
    register Buffer eimage;
    int      vindex;
    Shadow   vshadow;
    register Shadow eshadow;
    Zbuffer  vzbuffer;
    register Vec hmap0;
    register Vec emap;
    register Buffer cmap;
    int      iv;
    register Vec hinterp0;
    register Buffer hdata11, hdata12, hdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    vdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    vimage = image + wide * v0 + h0 + vsize * skew;
    vshadow = shadow + wide * v0 + h0 + vsize * skew;
    eshadow = shadow + wide * hite;
    vzbuffer = zbuffer + wide * v0 + h0 + vsize * skew;
    vindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = hmap + hsize;
    cmap = map;
    for( iv = 0; iv < vsize; iv++, vmap++ ){
        hdata = vdata + *vmap;
        hdata21 = hdata + hstride;
        hdata12 = hdata + vstride;
        hdata11 = hdata + hstride + vstride;
        vinterp1 = *vinterp++;
        vinterp2 = (MAP_INTERP - vinterp1);
        for( hmap0 = hmap, himage = vimage, hinterp0 = hinterp;
             hmap0 < emap && himage < eimage; hmap0++ ){
            hinterp1 = *hinterp0++;
            hinterp2 = (MAP_INTERP - hinterp1);
            *himage++ = cmap[
                             ((int) hdata[*hmap0] * vinterp2 * hinterp2
                              + (int) hdata21[*hmap0] * vinterp2 * hinterp1
                              + (int) hdata12[*hmap0] * vinterp1 * hinterp2
                              +
                              (int) hdata11[*hmap0] * vinterp1 * hinterp1) /
                             RENDER_INTERP];
        }
        vimage += wide - skew;
    }
}

void RenderInterpVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                 unsigned char *map, unsigned char *tmap, int wide, int hite,
                 int v0, int h0, int fence_transp, int hsize, Vec hmap,
                 Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                 int vstride, int zsize, int zframe, int zstride, int zdir,
                 int zinv, int frame4, int stride4, int frame5, int stride5,
                 int skew)
{
    register Buffer vdata;
    Buffer   hdata;
    register Buffer vimage;
    Buffer   himage;
    register Buffer eimage;
    register int vindex;
    int      hindex;
    register Shadow vshadow;
    Shadow   hshadow;
    register Shadow eshadow;
    register Zbuffer vzbuffer;
    Zbuffer  hzbuffer;
    register Vec vmap0;
    register Vec emap;
    register Buffer cmap;
    int      ih;
    register int z;
    register Vec vinterp0;
    register int vstep;
    register Buffer vdata11, vdata12, vdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    hdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    himage = image + wide * v0 + h0;
    hshadow = shadow + wide * v0 + h0;
    eshadow = shadow + wide * hite;
    hzbuffer = zbuffer + wide * v0 + h0;
    hindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = vmap + vsize;
    vstep = wide;
    cmap = map;
    for( ih = 0, hmap += hsize - 1, hinterp += hsize - 1; ih < hsize;
         ih++, hmap-- ){
        vdata = hdata + *hmap;
        vdata21 = vdata + vstride;
        vdata12 = vdata + hstride;
        vdata11 = vdata + vstride + hstride;
        hinterp1 = *hinterp--;
        hinterp2 = (MAP_INTERP - hinterp1);
        for( vmap0 = vmap, vimage = himage, vinterp0 = vinterp;
             vmap0 < emap && vimage < eimage; vimage += vstep, vmap0++ ){
            vinterp1 = *vinterp0++;
            vinterp2 = (MAP_INTERP - vinterp1);
            *vimage = cmap[
                           ((int) vdata[*vmap0] * hinterp2 * vinterp2
                            + (int) vdata21[*vmap0] * hinterp2 * vinterp1
                            + (int) vdata12[*vmap0] * hinterp1 * vinterp2
                            +
                            (int) vdata11[*vmap0] * hinterp1 * vinterp1) /
                           RENDER_INTERP];
        }
        himage += 1 - skew * wide;
    }
}

void RenderTranspHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                 unsigned char *map, unsigned char *tmap, int wide, int hite,
                 int v0, int h0, int fence_transp, int hsize, Vec hmap,
                 Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                 int vstride, int zsize, int zframe, int zstride, int zdir,
                 int zinv, int frame4, int stride4, int frame5, int stride5,
                 int skew)
{
    Buffer   vdata;
    register Buffer hdata;
    Buffer   vimage;
    register Buffer himage;
    register Buffer eimage;
    int      vindex;
    register int hindex;
    Shadow   vshadow;
    register Shadow hshadow;
    register Shadow eshadow;
    Zbuffer  vzbuffer;
    register Zbuffer hzbuffer;
    register Vec hmap0;
    register Vec emap;
    register Buffer cmap;
    int      iv;
    register int z;
    register Vec hinterp0;
    register Buffer hdata11, hdata12, hdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    vdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    vimage = image + wide * v0 + h0 + vsize * skew;
    vshadow = shadow + wide * v0 + h0 + vsize * skew;
    eshadow = shadow + wide * hite;
    vzbuffer = zbuffer + wide * v0 + h0 + vsize * skew;
    vindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = hmap + hsize;
    cmap = map;
    for( iv = 0; iv < vsize; iv++, vmap++ ){
        hdata = vdata + *vmap;
        hindex = vindex + *vmap;
        for( hmap0 = hmap, himage = vimage, hshadow = vshadow;
             hmap0 < emap && himage < eimage; hshadow++ ){
            if (hdata[*hmap0] > *himage ){
                *hshadow = hindex + *hmap0;
            }
            *himage++ = *(tmap + (*himage << 8) + hdata[*hmap0++]);
        }
        vimage += wide - skew;
        vshadow += wide - skew;
    }
}

void RenderTranspVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                 unsigned char *map, unsigned char *tmap, int wide, int hite,
                 int v0, int h0, int fence_transp, int hsize, Vec hmap,
                 Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                 int vstride, int zsize, int zframe, int zstride, int zdir,
                 int zinv, int frame4, int stride4, int frame5, int stride5,
                 int skew)
{
    register Buffer vdata;
    Buffer   hdata;
    register Buffer vimage;
    Buffer   himage;
    register Buffer eimage;
    register int vindex;
    int      hindex;
    register Shadow vshadow;
    Shadow   hshadow;
    register Shadow eshadow;
    register Zbuffer vzbuffer;
    Zbuffer  hzbuffer;
    register Vec vmap0;
    register Vec emap;
    register Buffer cmap;
    int      ih;
    register int z;
    register Vec vinterp0;
    register int vstep;
    register Buffer vdata11, vdata12, vdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    hdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    himage = image + wide * v0 + h0;
    hshadow = shadow + wide * v0 + h0;
    eshadow = shadow + wide * hite;
    hzbuffer = zbuffer + wide * v0 + h0;
    hindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = vmap + vsize;
    vstep = wide;
    cmap = map;
    for( ih = 0, hmap += hsize - 1; ih < hsize; ih++, hmap-- ){
        vdata = hdata + *hmap;
        vindex = hindex + *hmap;
        for( vmap0 = vmap, vimage = himage, vshadow = hshadow;
             vmap0 < emap && vimage < eimage; vimage += vstep, vshadow += vstep ){
            if (vdata[*vmap0] > *vimage ){
                *vshadow = vindex + *vmap0;
            }
            *vimage = *(tmap + (*vimage << 8) + vdata[*vmap0++]);
        }
        himage += 1 - skew * wide;
        hshadow += 1 - skew * wide;
    }
}

void RenderFrontFenceHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                     unsigned char *map, unsigned char *tmap, int wide,
                     int hite, int v0, int h0, int fence_transp, int hsize,
                     Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap,
                     Vec vinterp, int vstride, int zsize, int zframe,
                     int zstride, int zdir, int zinv, int frame4, int stride4,
                     int frame5, int stride5, int skew)
{
    Buffer   vdata;
    register Buffer hdata;
    Buffer   vimage;
    register Buffer himage;
    register Buffer eimage;
    int      vindex;
    register int hindex;
    Shadow   vshadow;
    register Shadow hshadow;
    register Shadow eshadow;
    Zbuffer  vzbuffer;
    register Zbuffer hzbuffer;
    register Vec hmap0;
    register Vec emap;
    register Buffer cmap;
    int      iv;
    register int z;
    register Vec hinterp0;
    register Buffer hdata11, hdata12, hdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    vdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    vimage = image + wide * v0 + h0 + vsize * skew;
    vshadow = shadow + wide * v0 + h0 + vsize * skew;
    eshadow = shadow + wide * hite;
    vzbuffer = zbuffer + wide * v0 + h0 + vsize * skew;
    vindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = hmap + hsize;
    cmap = map;
    z = zinv;
    for( iv = 0; iv < vsize; iv++, vmap++ ){
        hdata = vdata + *vmap;
        hindex = vindex + *vmap;
        for( hmap0 = hmap, himage = vimage, hshadow = vshadow, hzbuffer =
             vzbuffer; hmap0 < emap && himage < eimage;
             hmap0++, himage++, hshadow++, hzbuffer++ ){
            if (z > (int) *hzbuffer && (int) hdata[*hmap0] > fence_transp ){
                *himage = cmap[hdata[*hmap0]];
                *hshadow = (hindex + *hmap0);
                *hzbuffer = z;
            }
        }
        vimage += wide - skew;
        vshadow += wide - skew;
        vzbuffer += wide - skew;
    }
}

void RenderTopFenceHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                   unsigned char *map, unsigned char *tmap, int wide, int hite,
                   int v0, int h0, int fence_transp, int hsize, Vec hmap,
                   Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                   int vstride, int zsize, int zframe, int zstride, int zdir,
                   int zinv, int frame4, int stride4, int frame5, int stride5,
                   int skew)
{
    Buffer   vdata;
    register Buffer hdata;
    Buffer   vimage;
    register Buffer himage;
    register Buffer eimage;
    int      vindex;
    register int hindex;
    Shadow   vshadow;
    register Shadow hshadow;
    register Shadow eshadow;
    Zbuffer  vzbuffer;
    register Zbuffer hzbuffer;
    register Vec hmap0;
    register Vec emap;
    register Buffer cmap;
    int      iv;
    register int z;
    register Vec hinterp0;
    register Buffer hdata11, hdata12, hdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    vdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    vimage = image + wide * v0 + h0 + vsize * skew;
    vshadow = shadow + wide * v0 + h0 + vsize * skew;
    eshadow = shadow + wide * hite;
    vzbuffer = zbuffer + wide * v0 + h0 + vsize * skew;
    vindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = hmap + hsize;
    cmap = map;
    for( iv = 0; iv < vsize; iv++, vmap++ ){
        hdata = vdata + *vmap;
        hindex = vindex + *vmap;
        z = iv;
        for( hmap0 = hmap, himage = vimage, hshadow = vshadow, hzbuffer =
             vzbuffer; hmap0 < emap && himage < eimage;
             hmap0++, himage++, hshadow++, hzbuffer++ ){
            if (z > (int) *hzbuffer && (int) hdata[*hmap0] > fence_transp ){
                *himage = cmap[hdata[*hmap0]];
                *hshadow = (hindex + *hmap0);
                *hzbuffer = z;
            }
        }
        vimage += wide - skew;
        vshadow += wide - skew;
        vzbuffer += wide - skew;
    }
}

void RenderSideFenceVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer,
                    unsigned char *map, unsigned char *tmap, int wide, int hite,
                    int v0, int h0, int fence_transp, int hsize, Vec hmap,
                    Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp,
                    int vstride, int zsize, int zframe, int zstride, int zdir,
                    int zinv, int frame4, int stride4, int frame5, int stride5,
                    int skew)
{
    register Buffer vdata;
    Buffer   hdata;
    register Buffer vimage;
    Buffer   himage;
    register Buffer eimage;
    register int vindex;
    int      hindex;
    register Shadow vshadow;
    Shadow   hshadow;
    register Shadow eshadow;
    register Zbuffer vzbuffer;
    Zbuffer  hzbuffer;
    register Vec vmap0;
    register Vec emap;
    register Buffer cmap;
    int      ih;
    register int z;
    register Vec vinterp0;
    register int vstep;
    register Buffer vdata11, vdata12, vdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    hdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    himage = image + wide * v0 + h0;
    hshadow = shadow + wide * v0 + h0;
    eshadow = shadow + wide * hite;
    hzbuffer = zbuffer + wide * v0 + h0;
    hindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = vmap + vsize;
    vstep = wide;
    cmap = map;
    for( ih = 0, hmap += hsize - 1; ih < hsize; ih++, hmap-- ){
        vdata = hdata + *hmap;
        vindex = hindex + *hmap;
        z = hsize - ih - 1;
        for( vmap0 = vmap, vimage = himage, vshadow = hshadow, vzbuffer =
             hzbuffer; vmap0 < emap && vimage < eimage;
             vmap0++, vimage += vstep, vshadow += vstep, vzbuffer += vstep ){
            if (z > (int) *vzbuffer && (int) vdata[*vmap0] > fence_transp ){
                *vimage = cmap[vdata[*vmap0]];
                *vshadow = (vindex + *vmap0);
                *vzbuffer = z;
            }
        }
        himage += 1 - skew * wide;
        hshadow += 1 - skew * wide;
        hzbuffer += 1 - skew * wide;
    }
}

void RenderFrontFenceInterpHorz(Buffer data, Buffer image, Shadow shadow,
                           Zbuffer zbuffer, unsigned char *map,
                           unsigned char *tmap, int wide, int hite, int v0,
                           int h0, int fence_transp, int hsize, Vec hmap,
                           Vec hinterp, int hstride, int vsize, Vec vmap,
                           Vec vinterp, int vstride, int zsize, int zframe,
                           int zstride, int zdir, int zinv, int frame4,
                           int stride4, int frame5, int stride5, int skew)
{
    Buffer   vdata;
    register Buffer hdata;
    Buffer   vimage;
    register Buffer himage;
    register Buffer eimage;
    int      vindex;
    register int hindex;
    Shadow   vshadow;
    register Shadow hshadow;
    register Shadow eshadow;
    Zbuffer  vzbuffer;
    register Zbuffer hzbuffer;
    register Vec hmap0;
    register Vec emap;
    register Buffer cmap;
    int      iv;
    register int z;
    register Vec hinterp0;
    register Buffer hdata11, hdata12, hdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    vdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    vimage = image + wide * v0 + h0 + vsize * skew;
    vshadow = shadow + wide * v0 + h0 + vsize * skew;
    eshadow = shadow + wide * hite;
    vzbuffer = zbuffer + wide * v0 + h0 + vsize * skew;
    vindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = hmap + hsize;
    cmap = map;
    z = zinv;
    for( iv = 0; iv < vsize; iv++, vmap++ ){
        hdata = vdata + *vmap;
        hdata21 = hdata + hstride;
        hdata12 = hdata + vstride;
        hdata11 = hdata + hstride + vstride;
        vinterp1 = *vinterp++;
        vinterp2 = (MAP_INTERP - vinterp1);
        hindex = vindex + *vmap;
        for( hmap0 = hmap, himage = vimage, hshadow = vshadow, hzbuffer =
             vzbuffer, hinterp0 = hinterp; hmap0 < emap && himage < eimage;
             hmap0++, himage++, hshadow++, hzbuffer++, hinterp0++ ){
            if (z > (int) *hzbuffer && (int) hdata[*hmap0] > fence_transp ){
                hinterp1 = *hinterp0;
                hinterp2 = (MAP_INTERP - hinterp1);
                *himage = cmap[
                               ((int) hdata[*hmap0] * vinterp2 * hinterp2
                                + (int) hdata21[*hmap0] * vinterp2 * hinterp1
                                + (int) hdata12[*hmap0] * vinterp1 * hinterp2
                                +
                                (int) hdata11[*hmap0] * vinterp1 * hinterp1) /
                               RENDER_INTERP];
                *hshadow = (hindex + *hmap0);
                *hzbuffer = z;
            }
        }
        vimage += wide - skew;
        vshadow += wide - skew;
        vzbuffer += wide - skew;
    }
}

void RenderTopFenceInterpHorz(Buffer data, Buffer image, Shadow shadow,
                         Zbuffer zbuffer, unsigned char *map,
                         unsigned char *tmap, int wide, int hite, int v0,
                         int h0, int fence_transp, int hsize, Vec hmap,
                         Vec hinterp, int hstride, int vsize, Vec vmap,
                         Vec vinterp, int vstride, int zsize, int zframe,
                         int zstride, int zdir, int zinv, int frame4,
                         int stride4, int frame5, int stride5, int skew)
{
    Buffer   vdata;
    register Buffer hdata;
    Buffer   vimage;
    register Buffer himage;
    register Buffer eimage;
    int      vindex;
    register int hindex;
    Shadow   vshadow;
    register Shadow hshadow;
    register Shadow eshadow;
    Zbuffer  vzbuffer;
    register Zbuffer hzbuffer;
    register Vec hmap0;
    register Vec emap;
    register Buffer cmap;
    int      iv;
    register int z;
    register Vec hinterp0;
    register Buffer hdata11, hdata12, hdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    vdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    vimage = image + wide * v0 + h0 + vsize * skew;
    vshadow = shadow + wide * v0 + h0 + vsize * skew;
    eshadow = shadow + wide * hite;
    vzbuffer = zbuffer + wide * v0 + h0 + vsize * skew;
    vindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = hmap + hsize;
    cmap = map;
    for( iv = 0; iv < vsize; iv++, vmap++ ){
        hdata = vdata + *vmap;
        hdata21 = hdata + hstride;
        hdata12 = hdata + vstride;
        hdata11 = hdata + hstride + vstride;
        vinterp1 = *vinterp++;
        vinterp2 = (MAP_INTERP - vinterp1);
        hindex = vindex + *vmap;
        z = iv;
        for( hmap0 = hmap, himage = vimage, hshadow = vshadow, hzbuffer =
             vzbuffer, hinterp0 = hinterp; hmap0 < emap && himage < eimage;
             hmap0++, himage++, hshadow++, hzbuffer++, hinterp0++ ){
            if (z > (int) *hzbuffer && (int) hdata[*hmap0] > fence_transp ){
                hinterp1 = *hinterp0;
                hinterp2 = (MAP_INTERP - hinterp1);
                *himage = cmap[
                               ((int) hdata[*hmap0] * vinterp2 * hinterp2
                                + (int) hdata21[*hmap0] * vinterp2 * hinterp1
                                + (int) hdata12[*hmap0] * vinterp1 * hinterp2
                                +
                                (int) hdata11[*hmap0] * vinterp1 * hinterp1) /
                               RENDER_INTERP];
                *hshadow = (hindex + *hmap0);
                *hzbuffer = z;
            }
        }
        vimage += wide - skew;
        vshadow += wide - skew;
        vzbuffer += wide - skew;
    }
}

void RenderSideFenceInterpVert(Buffer data, Buffer image, Shadow shadow,
                          Zbuffer zbuffer, unsigned char *map,
                          unsigned char *tmap, int wide, int hite, int v0,
                          int h0, int fence_transp, int hsize, Vec hmap,
                          Vec hinterp, int hstride, int vsize, Vec vmap,
                          Vec vinterp, int vstride, int zsize, int zframe,
                          int zstride, int zdir, int zinv, int frame4,
                          int stride4, int frame5, int stride5, int skew)
{
    register Buffer vdata;
    Buffer   hdata;
    register Buffer vimage;
    Buffer   himage;
    register Buffer eimage;
    register int vindex;
    int      hindex;
    register Shadow vshadow;
    Shadow   hshadow;
    register Shadow eshadow;
    register Zbuffer vzbuffer;
    Zbuffer  hzbuffer;
    register Vec vmap0;
    register Vec emap;
    register Buffer cmap;
    int      ih;
    register int z;
    register Vec vinterp0;
    register int vstep;
    register Buffer vdata11, vdata12, vdata21;
    register int hinterp1, hinterp2, vinterp1, vinterp2;

    hdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;
    himage = image + wide * v0 + h0;
    hshadow = shadow + wide * v0 + h0;
    eshadow = shadow + wide * hite;
    hzbuffer = zbuffer + wide * v0 + h0;
    hindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;
    eimage = image + wide * hite;
    emap = vmap + vsize;
    vstep = wide;
    cmap = map;
    for( ih = 0, hmap += hsize - 1, hinterp += hsize - 1; ih < hsize;
         ih++, hmap-- ){
        vdata = hdata + *hmap;
        vdata21 = vdata + vstride;
        vdata12 = vdata + hstride;
        vdata11 = vdata + vstride + hstride;
        hinterp1 = *hinterp--;
        hinterp2 = (MAP_INTERP - hinterp1);
        vindex = hindex + *hmap;
        z = hsize - ih - 1;
        for( vmap0 = vmap, vimage = himage, vshadow = hshadow, vzbuffer =
             hzbuffer, vinterp0 = vinterp; vmap0 < emap && vimage < eimage;
             vmap0++, vimage += vstep, vshadow += vstep, vzbuffer +=
             vstep, vinterp0++ ){
            if (z > (int) *vzbuffer && (int) vdata[*vmap0] > fence_transp ){
                vinterp1 = *vinterp0;
                vinterp2 = (MAP_INTERP - vinterp1);
                *vimage = cmap[
                               ((int) vdata[*vmap0] * hinterp2 * vinterp2
                                + (int) vdata21[*vmap0] * hinterp2 * vinterp1
                                + (int) vdata12[*vmap0] * hinterp1 * vinterp2
                                +
                                (int) vdata11[*vmap0] * hinterp1 * vinterp1) /
                               RENDER_INTERP];
                *vshadow = (vindex + *vmap0);
                *vzbuffer = z;
            }
        }
        himage += 1 - skew * wide;
        hshadow += 1 - skew * wide;
        hzbuffer += 1 - skew * wide;
    }
}
