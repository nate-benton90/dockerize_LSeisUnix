/* wgc4.h - include file for WGC code-4 traces
 *
 * declarations for:
 *	typedef struct {} wgctrace - the trace header and data 
 * Author: zhiming li
 */ 

#ifndef WGC4_H
#define WGC4_H

#define	WGCHBYTES	400	/* Bytes in the WGC trace header */
#define WGC_NFLTS	16384	/* Arbitrary limit on data array size	*/

typedef struct {	/* wgc - trace identification header */

	short fmati2;	/* wgc format code (byte 1-2) */

	short sndxi2;	/* sample start index (byte 3-4) */

	long idnti4;	/* ident number (byte 5-8) */

	short itrci2;	/* trace number (byte 9-10) */

	short cdfni2;	/* cdf number at midpoint (byte 11-12) */

	short stkwi2;	/* stackword (byte 13-14) */

	short elesi2;	/* elevation at source location (byte 15-16) */

	long xcdsi4;	/* x-coordinate at source location (byte 17-20) */

	long ycdsi4;	/* y-coordinate at source location (byte 21-24) */

	short stcsi2;	/* static correction at source location (byte 25-26) */

	short eledi2;	/* elevation at detector location (byte 27-28) */

	long xcddi4;	/* x-coordinate at detector location (byte 29-32) */
			/* or post-stack x-coordinate at centroid location */
	
	long ycddi4;	/* y-coordinate at detector location (byte 33-36) */
			/* or post-stack y-coordinate at centroid location */

	short stcdi2;	/* static correction at detector location (byte 37-38)*/

	short srddi2;	/* source to detector distance (byte 39-40)*/
			/* or central angle or velocity 	*/

	float trbfr4;	/* trace balance factor (byte 41-44)	*/

	float stwfr4;	/* stack weight factor (byte 45-48) 	*/
			/* or change of rms in percent	*/
	
	short fxgvi2;   /* fixed gain value (byte 49-50) 	*/

	short polci2;	/* polarity code (byte 51-52)	*/

	short tmfsi2;	/* time of first sample (byte 53-54)	*/

	short undefn;	/* undefined (byte 55-56)	*/

	short shpti2;	/* shotpoint number (byte 57-58)	*/

	short cdsci2;	/* cdf datum static correction (byte 59-60)	*/

	short stnsi2;	/* station number at source location (byte 61-62) */

	short stndi2;	/* station number at detector location (byte 63-64) */

	short elemi2;	/* elevation at midpoint location (byte 65-66) */

	short stnmi2;	/* station number at midpoint location (byte 67-68) */

	short strti2;	/* start time (byte 69-70) */

	short cblni2;	/* cable number (byte 71-72) */

	short ivfci2;	/* input velocity function code (byte 73-74) */

	short lsmbi2;	/* local status for muliple boat operations */
			/* (byte 75-76) */

	short rsmbi2;	/* remote status for muliple boat operations */
			/* (byte 77-78) */

	short cdsai2;	/* cdf datum static correction after application */
			/* (byte 79-80) */

	long frlni4;	/* field reel number or field shot identifier */
			/* (byte 81-84) */

	short fflni2;	/* field file number (byte 85-86) */

	short fchni2;	/* field channel number (byte 87-88) */

	short linei2;	/* line number within 3-d survey (byte 89-90) */

	short mutei2;	/* mute time (byte 91-92) */

	float pn3dr4;	/* shotpoint number within 3-d line (byte 93-96) */

	short podmi2;	/* perpendicular offset distance from midpoint */
			/* to profile (byte 97-98) */

	short ppoai2;	/* perpendicular offset azimuth (byte 99-100) */

	short edsli2;	/* elevation of datum at source location */
			/* (byte 101-102) */

	short eddli2;	/* elevation of datum at detector location */
			/* (byte 103-104) */

	short edmli2;	/* elevation of datum at midpoint location */
			/* (byte 105-106) */

	short ebwsi2;	/* elevation at base of weathering */
			/* at source location (byte 107-108) */

	short ebwdi2;	/* elevation at base of weathering */
			/* at detecor location (byte 109-110) */

	short ebwmi2;	/* elevation at base of weathering */
			/* at midpoint location (byte 111-112) */

	short ebssi2;	/* elevation at base of subweathering */
			/* at source location (byte 113-114) */

	short ebsdi2;	/* elevation at base of subweathering */
			/* at detecor location (byte 115-116) */

	short ebsmi2;	/* elevation at base of subweathering */
			/* at midpoint location (byte 117-118) */

	short drvsi2;	/* datum (replacement) velocity at */
			/* at source location (byte 119-120) */

	short drvdi2;	/* datum (replacement) velocity at */
			/* at detector location (byte 121-122) */

	short drvmi2;	/* datum (replacement) velocity at */
			/* at midpoint location (byte 123-124) */

	short vwlsi2;	/* velocity in weathering layer at */
			/* at source location (byte 125-126) */

	short vwldi2;	/* velocity in weathering layer at */
			/* at detector location (byte 127-128) */

	short vwlmi2;	/* velocity in weathering layer at */
			/* at midpoint location (byte 129-130) */

	short vslsi2;	/* velocity in subweathering layer at */
			/* at source location (byte 131-132) */

	short vsldi2;	/* velocity in subweathering layer at */
			/* at detector location (byte 133-134) */

	short vslmi2;	/* velocity in subweathering layer at */
			/* at midpoint location (byte 135-136) */

	short dpsri2;	/* depth of source at source location */
			/* (byte 137-138) */

	short dpdt;	/* depth of detector (byte 139-140) */

	short lenci2;	/* length of charge (byte 141-142) */

	short utsli2;	/* uphole time at source location (byte 143-144) */

	short utdli2;	/* uphole time at detector location (byte 145-146) */

	short utmli2;	/* uphole time at midpoint location (byte 147-148) */

	float fscsr4;	/* field static correction at source location */
			/* (byte 149-152) */

	float fscdr4;	/* field static correction at detector location */
			/* (byte 153-156) */

	float fscmr4;	/* field static correction at midpoint location */
			/* (byte 157-160) */
	
	float rscsr4;	/* residual static correction at source location */
			/* (byte 161-164) */

	float rscdr4;	/* residual static correction at detector location */
			/* (byte 165-168) */

	float rscmr4;	/* residual static correction at midpoint location */
			/* (byte 169-172) */

	float wtdsr4;	/* water depth or water bottom reflection time */
			/* at source location (byte 173-176) */
			/* positive=depth   negative=time */

	float wtddr4;	/* water depth or water bottom reflection time */
			/* at detector location (byte 177-180) */
			/* positive=depth   negative=time */

	float wtdmr4;	/* water depth or water bottom reflection time */
			/* at midpoint location (byte 181-184) */
			/* positive=depth   negative=time */

	float dfsmr4;	/* distance along profile from first survey station */
			/* to midpoint location (byte 185-188) */
			/* or refined first arrival time */

	float dfssr4;	/* distance along profile from first survey station */
			/* to source location (byte 189-192) */
			/* or first break time */

	float dsldr4;	/* distance along profile from source */
			/* location to detector location (byte 193-196) */
			/* positive - detector precedes source */
			/* negative - detector follows source */

	long snsli4;	/* station number at source location (byte 197-200) */

	long sndli4;	/* station number at detector location (byte 201-204) */

	long snmli4;	/* station number at midpoint location (byte 205-208) */

	long cdnsi4;	/* cdf number at source location (byte 209-212) */

	long cdndi4;	/* cdf number at detector location (byte 213-216) */

	long cdnmi4;	/* cdf number at midpoint location (byte 217-220) */

	float ssddr4;	/* signed source to detector distance (byte 221-224) */
			/* positive - detector precedes source */
			/* negative - detector follows source */

	double xcslr8;	/* x-coordinate at source location (byte 225-232)  */

	double xcdlr8;	/* x-coordinate at detector location (byte 232-240)  */

	double xcmlr8;	/* x-coordinate at midpoint location (byte 241-248)  */
			/* or poststack x-coordinate at centroid location */ 

	double ycslr8;	/* y-coordinate at source location (byte 249-256)  */

	double ycdlr8;	/* y-coordinate at detector location (byte 257-264)  */

	double ycmlr8;	/* y-coordinate at midpoint location (byte 265-272)  */
			/* or poststack y-coordinate at centroid location */ 

	double xcelr8;	/* x-coordinate at cell center (byte 273-280)  */
	
	double ycelr8;	/* y-coordinate at cell center (byte 281-288)  */
	
	float srt1r4;	/* primary 3-d sort index (byte 289-292)  */
	
	float srt2r4;	/* secondary 3-d sort index (byte 293-296)  */
	
	float az3dr4;	/* 3-d azimuth (degree) (byte 297-300)  */
	
	long ttypi4;	/* data trace type identification (byte 301-304)  */
	
	char idspxx[16];/* shotpoint identification at midpoint */
			/* location (byte 305-320) */ 

	char lintxx[16];/* line intersection at midpoint */
			/* location (byte 321-336) */ 

	short sdrsi2;	/* depth of source at detector location */
			/* (byte 337-338)  */

	short exbfi2;	/* exception bit-flags (byte 339-340)  */

	float sptrr4;	/* shotpoint number (byte 341-344)  */

	double stacr8;	/* station number at midpoint location (byte 345-352) */

	float lbhar4;	/* local borehole azimuth for vsp (byte 353-356) */

	float lbhdr4;	/* local borehole deviation for vsp (byte 357-360) */
			/* or inline dip */

	float atrar4;	/* azimuth of tool reference axis for vsp */
			/* (byte 361-364) */
			/* or crossline dip */

	long itodi4;	/* time of day (byte 365-368) */

	float ptimr4;	/* pick time/static (byte 369-372) */

	float pqalr4;	/* pick quality (byte 373-376) */

	float rfs1r4;	/* reserved for special use (byte 377-380) */

	float rfs2r4;	/* reserved for special use (byte 381-384) */

	long fdrni4;	/* field reel number (byte 385-388) */

	char rffd[12];  /* reserved for future development (byte 389-400) */ 

	float  data[WGC_NFLTS];

} wgc4, wgc4trace;


#endif
