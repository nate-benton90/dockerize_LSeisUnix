#ifndef RARGS_H
#define RARGS_H

#define CALLARGS \
        /* unpack structure attributes into argument list */\
        (DataBuffer(data),render->image,render->shadow,render->zbuffer,\
        render->cmap,render->tmap,render->wide,render->hite,v0,h0,\
        (int)(render->fence_transp*DataValueSize(data)*.01),\
        MapSize(hmap),MapVec(hmap),MapInterp(hmap),AxisStride(MapAxis(hmap)),\
        MapSize(vmap),MapVec(vmap),MapInterp(vmap),AxisStride(MapAxis(vmap)),\
        MapSize(zmap),MapFrame(zmap),AxisStride(MapAxis(zmap)),\
        AxisDir(MapAxis(zmap)),MapInverse(zmap,MapFrame(zmap)),\
        MapFrame(map4),AxisStride(MapAxis(map4)),\
        MapFrame(map5),AxisStride(MapAxis(map5)),\
        attr->skew)

#define FCALLARGS \
        /* unpack structure attributes into argument list */\
        (data->buffer,render->image,render->shadow,render->zbuffer,\
        render->cmap,render->tmap,&render->wide,&render->hite,&v0,&h0,&transp,\
        &hmap->size,hmap->map,hmap->interp,&hmap->axis->stride,\
        &vmap->size,vmap->map,vmap->interp,&vmap->axis->stride,\
        &zmap->size,&zmap->frame,&zmap->axis->stride,\
        &zmap->axis->dir,&zmap->inv[zmap->frame],\
        &map4->frame,&map4->axis->stride,&map5->frame,&map5->axis->stride,\
        &attr->skew)

#define ARGS \
        /* unpacked argument list */\
        (data,image,shadow,zbuffer,map,tmap,wide,hite,v0,h0,fence_transp,\
        hsize,hmap,hinterp,hstride,vsize,vmap,vinterp,vstride,\
        zsize,zframe,zstride,zdir,zinv,frame4,stride4,frame5,stride5,\
        skew)\
        /* argument types */\
        Buffer          data;   /* data cube buffer */\
        Buffer          image;  /* render image buffer */\
        Shadow          shadow; /* render index shadow */\
        Zbuffer         zbuffer;        /* render zbuffer */\
        unsigned char   map[];  /* render opaque map */\
        unsigned char   tmap[]; /* render transparency map */\
        int             wide;   /* render buffer widths */\
        int             hite;   /* render buffer heights */\
        int             v0;     /* draw verticla origin */\
        int             h0;     /* draw horizontal origin */\
        int             fence_transp;   /* render transparency threshhold */\
        int             hsize;  /* horizontal axis map size */\
        Vec             hmap;   /* horizontal axis data2image map */\
        Vec             hinterp;        /* horizontal axis interpolation map */\
        int             hstride;        /* horizontal axis increment */\
        int             vsize;  /* vertical axis map length */\
        Vec             vmap;   /* vertical axis data2image map */\
        Vec             vinterp;        /* vertical axis interpolation map */\
        int             vstride;        /* vertical axis increment */\
        int             zsize;  /* depth axis map length */\
        int             zframe; /* depth axis frame */\
        int             zstride;        /* depth axis increment */\
        int             zdir;   /* depth axis direction */\
        int             zinv;   /* inverse mapping of depth frame */\
        int             frame4; /* frame of 4D axis */\
        int             stride4;        /* stride of 4D axis */\
        int             frame5; /* frame of 5D axis */\
        int             stride5;        /* stride of 5D axis */\
        int             skew;   /* draw skew */

#define hdata22 hdata
#define HARGS ARGS {\
        Buffer          vdata;  /* vertical data marker */\
        register Buffer hdata;  /* horizontal data pointer */\
        Buffer          vimage; /* vertical image pointer */\
        register Buffer himage; /* horizontal image pointer */\
        register Buffer eimage; /* end iamge pointer */\
        int             vindex; /* vertical data offset index */\
        register int    hindex; /* horizontal data offset index */\
        Shadow          vshadow;        /* vertical shadow pointer */\
        register Shadow hshadow;        /* horizontal shadow pointer */\
        register Shadow eshadow;        /* end shadow pointer */\
        Zbuffer         vzbuffer;       /* vertical zbuffer pointer */\
        register Zbuffer hzbuffer;      /* horizontal zbuffer pointer */\
        register Vec    hmap0;  /* horizontal map pointer */\
        register Vec    emap;   /* end map pointer */\
        register Buffer cmap;   /* render opaque map */\
        int             iv;     /* vertical counter */\
        register int    z;      /* depth */\
        register Vec    hinterp0;       /* vertical interpolation pointer */\
        register Buffer hdata11, hdata12, hdata21;\
        register int    hinterp1, hinterp2, vinterp1, vinterp2;\
        \
        vdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;\
        vimage = image + wide * v0 + h0 + vsize * skew;\
        vshadow = shadow + wide * v0 + h0 + vsize * skew;\
        eshadow = shadow + wide * hite;\
        vzbuffer = zbuffer + wide * v0 + h0 + vsize * skew;\
        vindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;\
        eimage = image + wide * hite;\
        emap = hmap + hsize;\
        cmap = map;

#define vdata22 vdata
#define VARGS ARGS { \
        register Buffer vdata;  /* vertical data marker */\
        Buffer          hdata;  /* horizontal data pointer */\
        register Buffer vimage; /* vertical image pointer */\
        Buffer           himage;        /* horizontal image pointer */\
        register Buffer eimage; /* end iamge pointer */\
        register int    vindex; /* vertical data offset index */\
        int             hindex; /* horizontal data offset index */\
        register Shadow vshadow;        /* vertical shadow pointer */\
        Shadow          hshadow;        /* horizontal shadow pointer */\
        register Shadow eshadow;        /* end shadow pointer */\
        register Zbuffer vzbuffer;      /* vertical zbuffer pointer */\
        Zbuffer         hzbuffer;       /* horizontal zbuffer pointer */\
        register Vec    vmap0;  /* horizontal map pointer */\
        register Vec    emap;   /* end map pointer */\
        register Buffer cmap;   /* render opaque map */\
        int             ih;     /* vertical counter */\
        register int    z;      /* depth */\
        register Vec    vinterp0;       /* horizontal interpolation pointer */\
        register int    vstep;  /* vertical data increment */\
        register Buffer vdata11, vdata12, vdata21;\
        register int    hinterp1, hinterp2, vinterp1, vinterp2;\
        \
        hdata = data + zframe * zstride + frame4 * stride4 + frame5 * stride5;\
        himage = image + wide * v0 + h0;\
        hshadow = shadow + wide * v0 + h0;\
        eshadow = shadow + wide * hite;\
        hzbuffer = zbuffer + wide * v0 + h0;\
        hindex = zframe * zstride + frame4 * stride4 + frame5 * stride5;\
        eimage = image + wide * hite;\
        emap = vmap + vsize;\
        vstep = wide;\
        cmap = map;

#endif
