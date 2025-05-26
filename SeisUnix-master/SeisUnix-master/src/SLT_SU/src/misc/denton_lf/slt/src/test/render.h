
#ifndef RENDER_H
#define RENDER_H

/*
render object definition

The render has several zoom axes, drawing buffer, zbuffer and shadow buffer
Independent of graphics calls
Uses axis, data, and zoom definitions
*/

#include "map.h"
#include "data.h"

/* contants */
#define	RENDER_INTERP	16777216
#define	NALPHA		256
#define	RENDER_POLARITY		1
#define	RENDER_FENCE_TRANSP	1
#define RENDER_LOW_TRANSP	0
#define RENDER_HIGH_TRANSP	100
#define	RENDER_VOL_TRANSP	100

/* a shadow contains index information about an render: drawing side + data index */
typedef unsigned int* Shadow;
typedef unsigned int Shadow_;
/* Zbuffer records depth of object in depth axis samples */
typedef unsigned int* Zbuffer;
typedef unsigned int Zbuffer_;
/* Render object */
typedef struct {
	int	wide;	/* drawing surface dimensions */
	int	hite;
	int	h0, v0, nh, nv;	/* window last drawn */
	int	interp;		/* interpolation flag */
	int	polarity;	/* color mapping polarity */
	int	fence_transp;	/* fence transparency (0-100) */
	int	low_transp;	/* low opacity value */
	int	high_transp;	/* low opacity value */
	int	vol_transp;	/* opacity gradient */
	float	alpha[NALPHA];	/* alpha channel for transparency */
	unsigned char	cmap[256];	/* data -> color map */
	unsigned char	tmap[65536];	/* transparency map */
	Buffer	image;	/* render pixel buffer */
	Shadow	shadow;	/* render shadow buffer */
	Zbuffer	zbuffer;	/* zbuffer to record render depth */
	} *Render;

/* render attribute list */
#define	RENDER_HORZ	1
#define	RENDER_VERT	2
#define	RENDER_DEEP	3
typedef struct {
	int	orient;	/* RENDER_HORZ, RENDER_VERT, RENDER_DEEP */
	int	image;	/* draw image */
	int	shadow;	/* draw shadow */
	int	zbuffer;	/* draw zbuffer */
	int	box;	/* draw bounding box */
	int	axes;	/* draw axes */
	int	lines;	/* draw cross lines */
	int	pick;	/* draw picks */
	int	skew;	/* draw skewed */
	int	transp;	/* draw transparent */
	int	mark;	/* record the frame in plane list */
	} *RenderAttr;

/* render.c */
Render RenderInit(void);
void RenderSize(int wide, int hite);
void RenderHorz(Data data, Map hmap, Map vmap, Map zmap, Map map4, Map map5, int h0, int v0, Render render, int *margins, RenderAttr attr);
void RenderVert(Data data, Map hmap, Map vmap, Map zmap, Map map4, Map map5, int h0, int v0, Render render, int *margins, RenderAttr attr);
void RenderDeep(Data data, Map hmap, Map vmap, Map zmap, Map map4, Map map5, int h0, int v0, Render render, int *margins, RenderAttr attr);
int RenderBufferValue(Render render, int x, int y);
int RenderShadowValue(Render render, int x, int y);
Buffer RenderBuffer(Render render);
void RenderDraw(void);
void RenderInfo(Render render);
void RenderSavePar(void);
void RenderLine(Render render, int x0, int y0, int x1, int y1, int color);
void RenderSetInterp(int mode);
void RenderTogglePolarity(void);
void RenderToggleInterp(void);
void RenderSetFenceTransp(int transparency);
void RenderSetLow(int low);
void RenderSetHigh(int high);
void RenderSetGradient(int vol_transp);
void RenderClear(void);
void RenderMap(Render render);
void RenderMapDump(void);
void RenderImageDump(void);
void RenderRect(int *h0, int *v0, int *nh, int *nv);
void RenderBasicHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderBasicVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderShadowHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderShadowVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderInterpHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderInterpVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderTranspHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderTranspVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderFrontFenceHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderTopFenceHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderSideFenceVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderFrontFenceInterpHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderTopFenceInterpHorz(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
void RenderSideFenceInterpVert(Buffer data, Buffer image, Shadow shadow, Zbuffer zbuffer, unsigned char *map, unsigned char *tmap, int wide, int hite, int v0, int h0, int fence_transp, int hsize, Vec hmap, Vec hinterp, int hstride, int vsize, Vec vmap, Vec vinterp, int vstride, int zsize, int zframe, int zstride, int zdir, int zinv, int frame4, int stride4, int frame5, int stride5, int skew);
#endif
