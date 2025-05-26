
#include <stdio.h>
#include <stdlib.h>

#include "par.h"

#include "main.h"
#include "axis.h"
#include "map.h"

Buffer   edit_buffer = 0;
Vec      edit_vec = 0;
int      edit_size = 0;

/* extract frame of data, given frame */
int EditFrame(Buffer data, Map map1, Map map2, Map map3, int frame, float *dist)
{
    int      inc2, i2, len1, len2;
    int inc1;
    Buffer data1, edata;

    inc1 = AxisStride(MapAxis(map1));
    inc2 = AxisStride(MapAxis(map2));
    len1 = MapWindow(map1) - 1;
    len2 = MapWindow(map2) - 1;
    data +=
        frame * AxisStride(MapAxis(map3)) + MapLow(map1) * inc1 +
        MapLow(map2) * inc2;
    for( i2 = 0; i2 < len2; i2++, data += inc2 ){
        for( data1 = data, edata = data1 + inc1 * len1; data1 < edata;
             data1 += inc1 ){
            dist[*data1] += 1.;
        }
    }
    return (len1 * len2);
}

/* extract six frames of a box, given three map axes */
int EditBox(Buffer data, Map map1, Map map2, Map map3, float *dist)
{
    int      n = 0, i;

    for( i = 0; i < 256; i++)
        dist[i] = 0.;
    n += EditFrame(data, map1, map2, map3, MapLow(map3), dist);
    n += EditFrame(data, map1, map2, map3, MapHigh(map3), dist);
    n += EditFrame(data, map2, map3, map1, MapLow(map1), dist);
    n += EditFrame(data, map2, map3, map1, MapHigh(map1), dist);
    n += EditFrame(data, map3, map1, map2, MapLow(map2), dist);
    n += EditFrame(data, map3, map1, map2, MapHigh(map2), dist);
    return (n);
}

/* return data distribution and stats given buffer */
void EditStats(int n, float *dist, int *low, int *median, int *high)
{
    int      i;

    for( i = 0, *low = -1, *median = -1, *high = 0; i < 256; i++ ){
        if( i < 255 && dist[i + 1] > 0. ){
            *high = i + 1;
        }
        if( i < 255 ){
            dist[i + 1] += dist[i];
        }
        dist[i] /= n;
        if( dist[i] > 0. && *low == -1 ){
            *low = i;
        }
        if( dist[i] > .5 && *median == -1 ){
            *median = i;
        }
    }
}

/* extract sub volume given three map axes */
int EditCube(Buffer data, Map map1, Map map2, Map map3, float *dist)
{
    int      i2, i3, inc2, inc3, len1, len2, len3, i;
    int inc1;
    Buffer   data2;
    Buffer data1, edata;

    inc1 = AxisStride(MapAxis(map1));
    inc2 = AxisStride(MapAxis(map2));
    inc3 = AxisStride(MapAxis(map3));
    len1 = MapWindow(map1);
    len2 = MapWindow(map2);
    for( i = 0; i < 256; i++)
        dist[i] = 0.;
    len3 = MapWindow(map3);
    data += MapLow(map3) * inc3 + MapLow(map2) * inc2 + MapLow(map1) * inc1;
    for( i3 = 0; i3 < len3; i3++, data += inc3 ){
        for( i2 = 0, data2 = data; i2 < len2; i2++, data2 += inc2 ){
            for( data1 = data2, edata = data1 + inc1 * len1; data1 < edata;
                 data1 += inc1 ){
                dist[*data1] += 1.;
            }
        }
    }
    return (len1 * len2 * len3);
}

/* 3-D linear grade blemish repair; save backup */
void EditGrade(Buffer data, Map map1, Map map2, Map map3)
{
    Buffer   bufferp;
    Vec      vecp;
    Buffer datap;
    int      off, d1, d2, d3;
    int n1, n2, n3, e1, e2, e3, j1, j2, j3, i1, i2, i3;

    d1 = AxisStride(MapAxis(map1));
    d2 = AxisStride(MapAxis(map2));
    d3 = AxisStride(MapAxis(map3));
    n1 = MapWindow(map1);
    n2 = MapWindow(map2);
    n3 = MapWindow(map3);
    e1 = n1 * d1 - d1;
    e2 = n2 * d2 - d2;
    e3 = n3 * d3 - d3;
    off = MapLow(map1) * d1 + MapLow(map2) * d2 + MapLow(map3) * d3;
    datap = data + off;
    edit_size = (n1 - 2) * (n2 - 2) * (n3 - 2);
    if( edit_size < 1 ){
        return;
    }

    if( edit_buffer ){
        free(edit_buffer);
        edit_buffer = 0;
        if( memwatch ){
	    (void)printf("free %s\n", "edit_buffer");
        }
    }
    if( edit_vec ){
        free(edit_vec);
        edit_vec = 0;
        if( memwatch ){
	    (void)printf("free %s\n", "edit_vec");
        }
    }
    {
        extern int _alloc;
    
        edit_buffer = (Buffer) malloc((edit_size) * sizeof(edit_buffer[0]));
        _alloc += (edit_size) * sizeof(edit_buffer);
        if( edit_buffer == 0 ){
	    err("cant allocate %d bytes for  edit_buffer; %d already allocated",
	        (edit_size) * sizeof(edit_buffer), _alloc);
        }
        if( memwatch ){
	    (void) printf("malloc %s=%d\n", " edit_buffer",
		          (edit_size) * sizeof(edit_buffer[0]));
        }
    }
    {
        extern int _alloc;
    
        edit_vec = (Vec) malloc((edit_size) * sizeof(edit_vec[0]));
        _alloc += (edit_size) * sizeof(edit_vec);
        if( edit_vec == 0 ){
	    err("cant allocate %d bytes for  edit_vec; %d already allocated",
	        (edit_size) * sizeof(edit_vec[0]), _alloc);
        }
        if( memwatch ){
	    (void) printf("malloc %s=%d\n", " edit_vec",
		          (edit_size) * sizeof(edit_vec[0]));
        }
    }
    bufferp = edit_buffer;
    vecp = edit_vec;
    n1--;
    n2--;
    n3--;
    for( i1 = 1; i1 < n1; i1++ ){
        j1 = i1 * d1;
        for( i2 = 1; i2 < n2; i2++ ){
            j2 = i2 * d2;
            for( i3 = 1; i3 < n3; i3++ ){
                j3 = i3 * d3;
                *bufferp++ = datap[j1 + j2 + j3];
                *vecp++ = off + j1 + j2 + j3;
                datap[j1 + j2 + j3] = (
                                       ((n1 - i1) * (int) (datap[j2 + j3]) +
                                        i1 * (int) (datap[e1 + j2 + j3])) / n1 +
                                       ((n2 - i2) * (int) (datap[j1 + j3]) +
                                        i2 * (int) (datap[j1 + e2 + j3])) / n2 +
                                       ((n3 - i3) * (int) (datap[j1 + j2]) +
                                        i3 * (int) (datap[j1 + j2 + e3])) /
                                       n3) / 3;
            }
        }
    }
}

void EditUndo(Buffer data)
{
    Buffer bufferp, datap, endp;
    Vec vecp;
    int swap;

    for( bufferp = edit_buffer, vecp = edit_vec, datap = data, endp =
         edit_buffer + edit_size; bufferp < endp; bufferp++, vecp++ ){
        swap = *bufferp;
        *bufferp = datap[*vecp];
        datap[*vecp] = swap;
    }
}
