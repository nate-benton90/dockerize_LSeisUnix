     &	(data,image,shadow,zbuffer,tmap,
     &	wide,hite,v0,h0,transp,hsize,hmap,hinterp,hstride,vsize,
     &	vmap,vinterp,vstride,zsize,zframe,zstride,zdir,zinv,skew)
C ARGUMENT DECLARATIONS *****************************************
C render buffer widths 
	INTEGER		wide
C render buffer heights 
	INTEGER		hite
C data cube buffer 
	LOGICAL*1	data(*)
C render image buffer 
	LOGICAL*1	image(wide,hite)
C render index shadow 
	INTEGER*4	shadow(wide,hite)
C render zbuffer 
	INTEGER*4	zbuffer(wide,hite)
C render transp map 
	LOGICAL*1	tmap(256,256)
C draw vertical origin 
	INTEGER		v0
C draw horizontal origin 
	INTEGER		h0
C render transparency threshhold 
	INTEGER		transp
C horizontal axis map size 
	INTEGER		hsize
C horizontal axis data2image map 
	INTEGER*4	hmap(hsize)
C horizontal axis interpolation map 
	INTEGER*4	hinterp(hsize)
C horizontal axis increment 
	INTEGER		hstride
C vertical axis map length 
	INTEGER		vsize
C vertical axis data2image map 
	INTEGER*4	vmap(vsize)
C vertical axis interpolation map 
	INTEGER*4	vinterp(vsize)
C vertical axis increment 
	INTEGER		vstride
C depth axis map length 
	INTEGER		zsize
C depth axis frame 
	INTEGER		zframe
C depth axis increment 
	INTEGER		zstride
C depth axis direction 
	INTEGER		zdir
C inverse mapping of depth frame 
	INTEGER		zinv
C draw skew 
	INTEGER		skew
C CONSTANTS ********************************************************
C interpolation coefficents
	INTEGER		MAP_INTERP
	PARAMETER	(MAP_INTERP = 4096)
	INTEGER		RENDER_INTERP
	PARAMETER	(RENDER_INTERP = 16777216)
C VARIABLES *********************************************************
C counters 
	INTEGER		ih, iv
C offsets
	INTEGER		h1, v1
