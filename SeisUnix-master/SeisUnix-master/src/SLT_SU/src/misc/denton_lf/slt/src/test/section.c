/*
plot sections and profiles
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "par.h"

#include "map.h"
#include "section.h"
#include "main.h"
#include "axis.h"
#include "data.h"
#include "view.h"

void     PlotFrontContour(void)
{
    PlotPlane(AXIS_ACROSS, AXIS_DOWN, AXIS_DEEP, "xcontour", "");
}

void     PlotSideContour(void)
{
    PlotPlane(-AXIS_DEEP, AXIS_DOWN, AXIS_ACROSS, "xcontour", "");
}

void     PlotTopContour(void)
{
    PlotPlane(AXIS_ACROSS, AXIS_DEEP, AXIS_DOWN, "xcontour", "");
}

void     PlotFrontWiggle(void)
{
    PlotPlane(AXIS_ACROSS, AXIS_DOWN, AXIS_DEEP, "xwigb", "");
}

void     PlotSideWiggle(void)
{
    PlotPlane(-AXIS_DEEP, AXIS_DOWN, AXIS_ACROSS, "xwigb", "");
}

void     PlotTopWiggle(void)
{
    PlotPlane(AXIS_ACROSS, AXIS_DEEP, AXIS_DOWN, "xwigb", "");
}

void     PlotDeepProfile(void)
{
    PlotProfile(AXIS_ACROSS, AXIS_DOWN, AXIS_DEEP, "xgraph", "");
}

void     PlotAcrossProfile(void)
{
    PlotProfile(AXIS_DEEP, AXIS_DOWN, AXIS_ACROSS, "xgraph", "");
}

void     PlotDownProfile(void)
{
    PlotProfile(AXIS_ACROSS, AXIS_DEEP, AXIS_DOWN, "xgraph", "");
}

void     PrintFrontContour(void)
{
    PlotPlane(AXIS_ACROSS, AXIS_DOWN, AXIS_DEEP, "pscontour ", "");
}

void     PrintSideContour(void)
{
    PlotPlane(-AXIS_DEEP, AXIS_DOWN, AXIS_ACROSS, "pscontour ", "");
}

void     PrintTopContour(void)
{
    PlotPlane(AXIS_ACROSS, AXIS_DEEP, AXIS_DOWN, "pscontour ", "");
}

void     PrintFrontWiggle(void)
{
    PlotPlane(AXIS_ACROSS, AXIS_DOWN, AXIS_DEEP, "pswigb", "");
}

void     PrintSideWiggle(void)
{
    PlotPlane(-AXIS_DEEP, AXIS_DOWN, AXIS_ACROSS, "pswigb", "");
}

void     PrintTopWiggle(void)
{
    PlotPlane(AXIS_ACROSS, AXIS_DEEP, AXIS_DOWN, "pswigb", "");
}

void     PrintDeepProfile(void)
{
    PlotProfile(AXIS_ACROSS, AXIS_DOWN, AXIS_DEEP, "psgraph", "");
}

void     PrintAcrossProfile(void)
{
    PlotProfile(-AXIS_DEEP, AXIS_DOWN, AXIS_ACROSS, "psgraph", "");
}

void     PrintDownProfile(void)
{
    PlotProfile(AXIS_ACROSS, AXIS_DEEP, AXIS_DOWN, "psgraph", "");
}

void     SaveFront(char *filename)
{
    PlotPlane(AXIS_ACROSS, AXIS_DOWN, AXIS_DEEP, "save", filename);
}

void     SaveSide(char *filename)
{
    PlotPlane(-AXIS_DEEP, AXIS_DOWN, AXIS_ACROSS, "save", filename);
}

void     SaveTop(char *filename)
{
    PlotPlane(AXIS_ACROSS, AXIS_DEEP, AXIS_DOWN, "save", filename);
}

void     SaveDeep(char *filename)
{
    PlotProfile(AXIS_ACROSS, AXIS_DOWN, AXIS_DEEP, "save", filename);
}

void     SaveAcross(char *filename)
{
    PlotProfile(-AXIS_DEEP, AXIS_DOWN, AXIS_ACROSS, "save", filename);
}

void     SaveDown(char *filename)
{
    PlotProfile(AXIS_ACROSS, AXIS_DEEP, AXIS_DOWN, "save", filename);
}

void     PlotPlane(int across, int down, int deep, char *program, char *file)
{
    extern View view;
    extern Data data;
    float   *buffer, hfirst, hlast, vfirst, vlast;
    int      n1, n2, vdir, hdir, fd;
    char     filename[80], command[255];

    vdir = down > 0 ? 1 : -1;
    down = down > 0 ? down : -down;
    hdir = across > 0 ? 1 : -1;
    across = across > 0 ? across : -across;
    /* extract map plane */
    n1 = MapWindow(view->map[down]);
    n2 = MapWindow(view->map[across]);

    {
        extern int _alloc;

        buffer = (float *) malloc((n1 * n2) * sizeof(buffer[0]));
        _alloc += (n1 * n2) * sizeof(buffer[0]);
        if( buffer == 0 ){
            err("cant allocate %d bytes for var; %d already allocated",
                (n1 * n2) * sizeof(buffer[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", "var",
                          (n1 * n2) * sizeof(buffer[0]));}
    };

    PlotExtractPlane(view->map[across],
                     view->map[down],
                     view->map[deep],
                     view->map[4], view->map[5], buffer, n1, n2, hdir, vdir);
    if( !strcmp(program, "save") ){

/*
                sprintf (filename,
                        "%s.%s.%dx%d",
                        DataShortName(data),
                        AxisScript(MapAxis(view->map[deep]),
                        MapFrame(view->map[deep])),
                        n1,
                        n2);
*/
        strcpy(filename, file);
    } else {
        sprintf(filename, "/usr/tmp/cmovie%d", getpid());
    }
    write(fd = creat(filename, 0664), buffer, n1 * n2 * sizeof(buffer));
    if( !strcmp(program, "save") ){
        close(fd);
        return;
    }
    /* plot on screen */
    hfirst = MapValue(view->map[across], 1);
    hlast = MapValue(view->map[across], MapSize(view->map[across]));
    vfirst = MapValue(view->map[down], 1);
    vlast = MapValue(view->map[down], MapSize(view->map[down]));
    if( !strcmp(program, "xcontour") ){
        if( data->overlay_mode ){
        sprintf(command,
                "(%s < %s title=\"%s: %s\" n1=%d f1=%g d1=%g label1=\"%s\" n2=%d f2=%g d2=%g label2=\"%s\" dc=%f fc=%f nc=%d )&",
                program, filename, DataTitle(data),
                AxisScript(MapAxis(view->map[deep]), MapFrame(view->map[deep])),
                n1, vdir > 0 ? vfirst : vlast,
                vdir * (vlast - vfirst) / (n1 - 1),
                AxisLabel(MapAxis(view->map[down])), n2,
                hdir > 0 ? hfirst : hlast, hdir * (hlast - hfirst) / (n2 - 1),
                AxisLabel(MapAxis(view->map[across])) ,data->gh.dcdp2/data->gh.scale ,data->gh.o5/data->gh.scale ,irint(data->gh.d5/data->gh.scale));
        }else{
        sprintf(command,
                "(%s < %s title=\"%s: %s\" n1=%d f1=%g d1=%g label1=\"%s\" n2=%d f2=%g d2=%g label2=\"%s\")&",
                program, filename, DataTitle(data),
                AxisScript(MapAxis(view->map[deep]), MapFrame(view->map[deep])),
                n1, vdir > 0 ? vfirst : vlast,
                vdir * (vlast - vfirst) / (n1 - 1),
                AxisLabel(MapAxis(view->map[down])), n2,
                hdir > 0 ? hfirst : hlast, hdir * (hlast - hfirst) / (n2 - 1),
                AxisLabel(MapAxis(view->map[across])));
        }
    }
    if( !strcmp(program, "pscontour ") ){
        sprintf(command,
                "(%s < %s title=\"%s: %s\" n1=%d f1=%g d1=%g label1=\"%s\" n2=%d f2=%g d2=%g label2=\"%s\" | lpr)&",
                program, filename, DataTitle(data),
                AxisScript(MapAxis(view->map[deep]), MapFrame(view->map[deep])),
                n1, vdir > 0 ? vfirst : vlast,
                vdir * (vlast - vfirst) / (n1 - 1),
                AxisLabel(MapAxis(view->map[down])), n2,
                hdir > 0 ? hfirst : hlast, hdir * (hlast - hfirst) / (n2 - 1),
                AxisLabel(MapAxis(view->map[across])));
    }
    if( !strcmp(program, "xwigb") ){
        sprintf(command,
                "(%s < %s title=\"%s: %s\" n1=%d f1=%g d1=%g label1=\"%s\" n2=%d f2=%g d2=%g label2=\"%s\")&",
                program, filename, DataTitle(data),
                AxisScript(MapAxis(view->map[deep]), MapFrame(view->map[deep])),
                n1, vdir > 0 ? vfirst : vlast,
                vdir * (vlast - vfirst) / (n1 - 1),
                AxisLabel(MapAxis(view->map[down])), n2,
                hdir > 0 ? hfirst : hlast, hdir * (hlast - hfirst) / (n2 - 1),
                AxisLabel(MapAxis(view->map[across])));
    }
    if( !strcmp(program, "pswigb") ){
        sprintf(command,
                "(%s < %s title=\"%s: %s\" n1=%d f1=%g d1=%g label1=\"%s\" n2=%d f2=%g d2=%g label2=\"%s\" | lpr)&",
                program, filename, DataTitle(data),
                AxisScript(MapAxis(view->map[deep]), MapFrame(view->map[deep])),
                n1, vdir > 0 ? vfirst : vlast,
                vdir * (vlast - vfirst) / (n1 - 1),
                AxisLabel(MapAxis(view->map[down])), n2,
                hdir > 0 ? hfirst : hlast, hdir * (hlast - hfirst) / (n2 - 1),
                AxisLabel(MapAxis(view->map[across])));
    }
    system(command);
    sleep(5);
    unlink(filename);

    if( buffer ){
        free(buffer);
        buffer = 0;
        if( memwatch ){
            printf("free %s\n", "var");
        }
    };
}

void     PlotProfile(int across, int down, int deep, char *program, char *file)
{
    extern View view;
    extern Data data;
    float   *buffer, zfirst, zlast;
    int      n3, zdir, fd;
    string   filename;
    char     command[255];

    zdir = deep > 0 ? 1 : -1;
    deep = deep > 0 ? deep : -deep;
    /* extract map plane */
    n3 = MapWindow(view->map[deep]);

    {
        extern int _alloc;

        buffer = (float *) malloc((n3) * sizeof(buffer[0]));
        _alloc += (n3) * sizeof(buffer[0]);
        if( buffer == 0 ){
            err("cant allocate %d bytes for var; %d already allocated",
                (n3) * sizeof(buffer[0]), _alloc);
        }
        if( memwatch ){
            (void) printf("malloc %s=%d\n", "var", (n3) * sizeof(buffer[0]));
        }
    };

    PlotExtractProfile(view->map[across],
                       view->map[down],
                       view->map[deep],
                       view->map[4], view->map[5], buffer, n3, zdir);
    if( !strcmp(program, "save") ){

/*
                sprintf (filename,
                        "%s.%s.%s.%d",
                        DataShortName(data),
                        AxisScript(MapAxis(view->map[across]),MapFrame(view->map[across])),
                        AxisScript(MapAxis(view->map[down]),MapFrame(view->map[down])),
                        n3);
*/
        strcpy(filename, file);
    } else {
        sprintf(filename, "/usr/tmp/cmovie%d", getpid());
    }
    write(fd = creat(filename, 0664), buffer, n3 * sizeof(buffer));
    if( !strcmp(program, "save") ){
        close(fd);
        return;
    }
    /* plot on screen */
    zfirst = MapValue(view->map[deep], 1);
    zlast = MapValue(view->map[deep], MapSize(view->map[deep]));
    if( !strcmp(program, "xgraph") ){
        sprintf(command,
                "(%s < %s title=\"%s: %s %s\" n=%d f1=%g d1=%g label1=\"%s\" label2=\"%s\" width=400 height=600 style=seismic)&",
                program, filename, DataTitle(data),
                AxisScript(MapAxis(view->map[across]),
                           MapFrame(view->map[across])),
                AxisScript(MapAxis(view->map[down]), MapFrame(view->map[down])),
                n3, zdir > 0 ? zfirst : zlast,
                zdir * (zlast - zfirst) / (n3 - 1),
                AxisLabel(MapAxis(view->map[deep])),
                AxisLabel(data->axis[DATA_VALUE]));
    }
    if( !strcmp(program, "psgraph") ){
        sprintf(command,
                "(%s < %s title=\"%s: %s %s\" n=%d f1=%g d1=%g label1=\"%s\" label2=\"%s\" width=400 height=600 style=seismic | lpr)&",
                program, filename, DataTitle(data),
                AxisScript(MapAxis(view->map[across]),
                           MapFrame(view->map[across])),
                AxisScript(MapAxis(view->map[down]), MapFrame(view->map[down])),
                n3, zdir > 0 ? zfirst : zlast,
                zdir * (zlast - zfirst) / (n3 - 1),
                AxisLabel(MapAxis(view->map[deep])),
                AxisLabel(data->axis[DATA_VALUE]));
    }
    system(command);
    sleep(5);
    unlink(filename);

    if( buffer ){
        free(buffer);
        buffer = 0;
        if( memwatch ){
            printf("free %s\n", "var");
        }
    };
}

void     PlotExtractPlane(Map hmap, Map vmap, Map zmap, Map map4, Map map5,
                          float *buffer, int n1, int n2, int hdir, int vdir)
{
    extern Data data;
    Buffer   dp;
    register Buffer dp1, de;
    register float *map, *fp;
    int      i2, hinc;
    register int vinc;
    float val;

    dp = DataBuffer(data);
    if( hdir > 0 ){
        dp += MapFirst(hmap) * AxisStride(MapAxis(hmap));
    } else {
        dp += MapLast(hmap) * AxisStride(MapAxis(hmap));
    }
    if( vdir > 0 ){
        dp += MapFirst(vmap) * AxisStride(MapAxis(vmap));
    } else {
        dp += MapLast(vmap) * AxisStride(MapAxis(vmap));
    }
    dp += MapFrame(zmap) * AxisStride(MapAxis(zmap));
    dp += MapFrame(map4) * AxisStride(MapAxis(map4));
    dp += MapFrame(map5) * AxisStride(MapAxis(map5));
    map = AxisValues(DataAxis(data, DATA_VALUE));
    fp = buffer;
    hinc = AxisStride(MapAxis(hmap));
    hinc = MapFirst(hmap) < MapLast(hmap) ? hinc : -hinc;
    hinc = hdir > 0 ? hinc : -hinc;
    vinc = AxisStride(MapAxis(vmap));
    vinc = MapFirst(vmap) < MapLast(vmap) ? vinc : -vinc;
    vinc = vdir > 0 ? vinc : -vinc;

    for( i2 = 0; i2 < n2; i2++, dp += hinc ){

        for( dp1 = dp, de = dp1 + n1 * vinc; dp1 != de; dp1 += vinc ){

            if( data->overlay_mode ){
                val = *dp1/irint(data->gh.d4/data->gh.scale); 
                val *= data->gh.dcdp2 / data->gh.scale;
                val += data->gh.o5 / data->gh.scale;
                *fp++ = val;

            }else{
                *fp++ = map[*dp1 - DATA_VALUE_BASE];

            }
        }
    }
}

void     PlotExtractProfile(Map hmap, Map vmap, Map zmap, Map map4, Map map5,
                            float *buffer, int n3, int zdir)
{
    extern Data data;
    Buffer   dp;
    register float *map, *fp;
    register int i3, zinc;
    float val;

    dp = DataBuffer(data);
    dp += MapFrame(hmap) * AxisStride(MapAxis(hmap));
    dp += MapFrame(vmap) * AxisStride(MapAxis(vmap));
    if( zdir > 0 ){
        dp += MapFirst(zmap) * AxisStride(MapAxis(zmap));
    } else {
        dp += MapLast(zmap) * AxisStride(MapAxis(zmap));
    }
    dp += MapFrame(map4) * AxisStride(MapAxis(map4));
    dp += MapFrame(map5) * AxisStride(MapAxis(map5));
    map = AxisValues(DataAxis(data, DATA_VALUE));
    fp = buffer;
    zinc = AxisStride(MapAxis(zmap));
    zinc = MapFirst(zmap) < MapLast(zmap) ? zinc : -zinc;

    for( i3 = 0; i3 < n3; i3++, dp += zinc ){

        if( data->overlay_mode ){
            val = *dp/irint(data->gh.d4/data->gh.scale); 
            val *= data->gh.dcdp2 / data->gh.scale;
            val += data->gh.o5 / data->gh.scale;
            *fp++ = val;

        }else{
           *fp++ = map[*dp - DATA_VALUE_BASE];

        }
    }
}
