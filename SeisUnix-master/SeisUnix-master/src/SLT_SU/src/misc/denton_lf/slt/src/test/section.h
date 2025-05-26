
#ifndef SECTION_H
#define SECTION_H


/* section.c */
void PlotFrontContour(void);
void PlotSideContour(void);
void PlotTopContour(void);
void PlotFrontWiggle(void);
void PlotSideWiggle(void);
void PlotTopWiggle(void);
void PlotDeepProfile(void);
void PlotAcrossProfile(void);
void PlotDownProfile(void);
void PrintFrontContour(void);
void PrintSideContour(void);
void PrintTopContour(void);
void PrintFrontWiggle(void);
void PrintSideWiggle(void);
void PrintTopWiggle(void);
void PrintDeepProfile(void);
void PrintAcrossProfile(void);
void PrintDownProfile(void);
void SaveFront(char *filename);
void SaveSide(char *filename);
void SaveTop(char *filename);
void SaveDeep(char *filename);
void SaveAcross(char *filename);
void SaveDown(char *filename);
void PlotPlane(int across, int down, int deep, char *program, char *file);
void PlotProfile(int across, int down, int deep, char *program, char *file);
void PlotExtractPlane(Map hmap, Map vmap, Map zmap, Map map4, Map map5, float *buffer, int n1, int n2, int hdir, int vdir);
void PlotExtractProfile(Map hmap, Map vmap, Map zmap, Map map4, Map map5, float *buffer, int n3, int zdir);

#endif
