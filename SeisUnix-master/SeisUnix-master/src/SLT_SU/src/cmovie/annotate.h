/*
annotation line segment object
*/

#ifndef MOVIE_ANNO
#define	ANNO_NFILE	256
#define	ANNO_NSEG	100000

typedef struct {
    short    id;		/* group id */
    short    dir;		/* axial direction */
    short    line;		/* file line number */
    short    a[DATA_NAXIS];	/* point a */
    short    b[DATA_NAXIS];	/* point b */
}       *AnnotateSeg, AnnotateSeg_;

typedef struct {
    string   filename[ANNO_NFILE];	/* filename */
    int      nfile;		/* number of active ids */
    int      nseg;		/* total number of segments */
    AnnotateSeg_ seg[ANNO_NSEG];	/* segments */
    int      vis[ANNO_NFILE];	/* file visibility */
    int     *seg0[DATA_NAXIS];	/* segment starting point */
    int     *segn[DATA_NAXIS];	/* number of segments per frame */
}       *Annotate;

#ifdef __cplusplus	/* if C++, specify external linkage to C functions */
extern   "C" {
#endif
    Annotate AnnotateCurrent(void);
    void     AnnotateSetCurrent(Annotate annotate);
    Annotate AnnotateInit(void);
    void     AnnotateDraw(Annotate annotate, int draw);
    int      AnnotateCompare(AnnotateSeg a, AnnotateSeg b);
    void     AnnotateRead(Annotate annotate, char *filename);
    char    *AnnotateFilename(Annotate annotate);
    void     AnnotateInfo(Annotate annotate);
#ifdef __cplusplus		/* if C++, specify external linkage to C
				   functions */
}
#endif
#define MOVIE_ANNO
#endif
