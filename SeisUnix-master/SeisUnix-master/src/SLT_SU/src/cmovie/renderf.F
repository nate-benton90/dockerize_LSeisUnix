	SUBROUTINE RenderBasicHorz
#include "rargsf.h"
	h1 = h0 + vsize * skew
	DO iv=1,vsize
		v1 = zframe * zstride + vmap(iv)
		DO ih=1,hsize
			image(ih+h1,iv+v0) = data(v1+hmap(ih))
		ENDDO
		h1 = h1 - skew
	ENDDO
	END

	SUBROUTINE RenderBasicVert
#include "rargsf.h"
	v1 = v0
	DO ih=1,hsize
		h1 = zframe * zstride + hmap(hsize-ih+1)
		DO iv=1,vsize
			image(ih+h0,iv+v1) = data(h1+vmap(iv))
		ENDDO
		v1 = v1 - skew
	ENDDO
	END

	SUBROUTINE RenderShadowHorz
#include "rargsf.h"
	h1 = h0 + vsize * skew
	DO iv=1,vsize
		v1 = zframe * zstride + vmap(iv)
		DO ih=1,hsize
			shadow(ih+h1,iv+v0) = v1 + hmap(ih)
		ENDDO
		h1 = h1 - skew
	ENDDO
	END

	SUBROUTINE RenderShadowVert
#include "rargsf.h"
	v1 = v0
	DO ih=1,hsize
		h1 = zframe * zstride + hmap(hsize-ih+1)
		DO iv=1,vsize
			shadow(ih+h0,iv+v1) = h1 + vmap(iv)
		ENDDO
		v1 = v1 - skew
	ENDDO
	END

	SUBROUTINE RenderInterpHorz
#include "rargsf.h"
	INTEGER hinterp1, hinterp2, vinterp1, vinterp2, h2
	INTEGER map(-128:256), i, data11, data12, data21, data22
	DO i=-128,-1
		map(i) = 256 + i
	ENDDO
	DO i=0,127
		map(i) = i
	ENDDO
	h1 = h0 + vsize * skew
	DO iv=1,vsize
		v1 = zframe * zstride + vmap(iv)
		vinterp1 = vinterp(iv)
		vinterp2 = (MAP_INTERP - vinterp1)
		DO ih=1,hsize
			hinterp1 = hinterp(ih)
			hinterp2 = (MAP_INTERP - hinterp1)
			h2 = v1 + hmap(ih)
			data22 = data(h2)
			data21 = data(h2+hstride)
			data12 = data(h2+vstride)
			data11 = data(h2+hstride+vstride)
			image(ih+h1,iv+v0) = 
     &			( map(data11) * vinterp1 * hinterp1
     &			+ map(data12) * vinterp1 * hinterp2
     &			+ map(data21) * vinterp2 * hinterp1
     &			+ map(data22) * vinterp2 * hinterp2
     &			) / RENDER_INTERP
		ENDDO
		h1 = h1 - skew
	ENDDO
	END

	SUBROUTINE RenderInterpVert
#include "rargsf.h"
	INTEGER hinterp1, hinterp2, vinterp1, vinterp2, v2
	INTEGER map(-128:256), i, data11, data12, data21, data22
	DO i=-128,-1
		map(i) = 256 + i
	ENDDO
	DO i=0,127
		map(i) = i
	ENDDO
	v1 = v0
	DO ih=1,hsize
		h1 = zframe * zstride + hmap(hsize-ih+1)
		hinterp1 = hinterp(hsize-ih+1)
		hinterp2 = (MAP_INTERP - hinterp1)
		DO iv=1,vsize
			vinterp1 = vinterp(iv)
			vinterp2 = (MAP_INTERP - vinterp1)
			v2 = h1 + vmap(iv)
			data22 = data(v2)
			data21 = data(v2+vstride)
			data12 = data(v2+hstride)
			data11 = data(v2+vstride+hstride)
			image(ih+h0,iv+v1) = 
     &			( map(data11) * hinterp1 * vinterp1
     &			+ map(data12) * hinterp1 * vinterp2
     &			+ map(data21) * hinterp2 * vinterp1
     &			+ map(data22) * hinterp2 * vinterp2
     &			) / RENDER_INTERP
		ENDDO
		v1 = v1 - skew
	ENDDO
	END

	SUBROUTINE RenderTranspHorz
#include "rargsf.h"
	INTEGER map(-128:256), i, data1, image1
	DO i=-128,-1
		map(i) = 257 + i
	ENDDO
	DO i=0,127
		map(i) = i + 1
	ENDDO
	h1 = h0 + vsize * skew
	DO iv=1,vsize
		v1 = zframe * zstride + vmap(iv)
		DO ih=1,hsize
			image1 = image(ih+h1,iv+v0)
			data1 = data(v1+hmap(ih))
			image(ih+h1,iv+v0) = tmap(map(data1),map(image1))
		ENDDO
		h1 = h1 - skew
	ENDDO
	END

	SUBROUTINE RenderTranspVert
#include "rargsf.h"
	INTEGER map(-128:256), i, data1, image1
	DO i=-128,-1
		map(i) = 257 + i
	ENDDO
	DO i=0,127
		map(i) = i + 1
	ENDDO
	v1 = v0
	DO ih=1,hsize
		h1 = zframe * zstride + hmap(hsize-ih+1)
		DO iv=1,vsize
			image1 = image(ih+h0,iv+v1)
			data1 = data(h1+vmap(iv))
			image(ih+h0,iv+v1) = tmap(map(data1),map(image1))
		ENDDO
		v1 = v1 - skew
	ENDDO
	END

