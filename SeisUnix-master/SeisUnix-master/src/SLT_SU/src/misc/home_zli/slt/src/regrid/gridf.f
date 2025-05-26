C--------------------------------------------------------------------------
C       grid remapping by tri-linear interpolation
C       arguments:
C       [i,o][g,n,o,d][1,2,3]   i-input
C                               o-output
C                               n-axis length
C                               o-origin value
C                               d-sampling rate
C                               g-grid array
C                               1-fast axis
C                               2-middle axis
C                               3-slow axis
C--------------------------------------------------------------------------
        SUBROUTINE regrid3d (ig,in1,in2,in3,io1,io2,io3,id1,id2,id3,
     1          og,on1,on2,on3,oo1,oo2,oo3,od1,od2,od3,
     2          pflag1,pflag2,pflag3,pad1,pad2,pad3)
C input grid lengths (input)
        INTEGER in1, in2, in3
C output grid lengths (input)
        INTEGER on1, on2, on3
C input grid origins (input)
        REAL    io1, io2, io3
C output grid origins (input)
        REAL    oo1, oo2, oo3
C input grid spacings (input)
        REAL    id1, id2, id3
C output grid spacings (input)
        REAL    od1, od2, od3
C output pad flag (input)
	INTEGER	pflag1, pflag2, pflag3
C output padding (input)
	REAL	pad1, pad2, pad3
C input grid (input)
        REAL    ig(in1,in2,in3)
C output grid (output)
        REAL    og(on1,on2,on3)
C linear interpolation weights (internal)
        REAL    resid(16384)
C interpolation indexes (internal)
        INTEGER indx(16384)
C sample indexes (internal)
        INTEGER o1, o2, o3, i1, i2, i3
C interpolation weights (internal)
        REAL    d1, d2, d3
        REAL    a000, a001, a010, a011, a100, a101, a110, a111
C pad bounds (internal)
	INTEGER	mn1, mn2, mn3, mx1, mx2, mx3
C
C       compute linear intepolation coefficients: index and residual
C
	IF (on1+on2+on3 .GE. 16384) WRITE (2,*) 'sum of three output indexes > 16384'
        CALL axiscoeff (in1,io1,id1,on1,oo1,od1,
     1	mn1,mx1,resid(1),indx(1))
        CALL axiscoeff (in2,io2,id2,on2,oo2,od2,
     1  mn2,mx2,resid(on1+1),indx(on1+1))
        CALL axiscoeff (in3,io3,id3,on3,oo3,od3,
     1  mn3,mx3,resid(on1+on2+1),indx(on1+on2+1))
C
C apply padding
C
	IF (pflag1 .EQ. 0) THEN
		mn1 = 1
		mx1 = on1
	ELSE IF (mn1 .NE. 1 .OR. mx1 .NE. on1) THEN
		DO o2=1,on2
		DO o3=1,on3
		DO o1=1,mn1
			og(o1,o2,o3) = pad1
		ENDDO
		DO o1=mx1,on1
			og(o1,o2,o3) = pad1
		ENDDO
		ENDDO
		ENDDO
	ENDIF
	IF (pflag2 .EQ. 0) THEN
		mn2 = 1
		mx2 = on2
	ELSE IF (mn2 .NE. 1 .OR. mx2 .NE. on2) THEN
		DO o1=1,on1
		DO o3=1,on3
		DO o2=1,mn2
			og(o1,o2,o3) = pad2
		ENDDO
		DO o2=mx2,on2
			og(o1,o2,o3) = pad2
		ENDDO
		ENDDO
		ENDDO
	ENDIF
	IF (pflag3 .EQ. 0) THEN
		mn3 = 1
		mx3 = on3
	ELSE IF (mn3 .NE. 1 .OR. mx3 .NE. on3) THEN
		DO o1=1,on1
		DO o2=1,on2
		DO o3=1,mn3
			og(o1,o2,o3) = pad3
		ENDDO
		DO o3=mx3,on3
			og(o1,o2,o3) = pad3
		ENDDO
		ENDDO
		ENDDO
	ENDIF
C
C       loop through output array
C
        DO o3=mn3,mx3
        i3 = indx(on1+on2+o3)
        d3 = resid(on1+on2+o3)
                DO o2=mn2,mx2
                i2 = indx(on1+o2)
                d2 = resid(on1+o2)
                        DO o1=mn1,mx1
                        i1 = indx(o1)
                        d1 = resid(o1)
C
C       trilinear interpolation from nearest neighbor cube of input grid
C
                        a000 = ig(i1  ,i2  ,i3  )
                        a001 = ig(i1  ,i2  ,i3+1)
                        a010 = ig(i1  ,i2+1,i3  )
                        a100 = ig(i1+1,i2  ,i3  )
                        a011 = ig(i1,  i2+1,i3+1)
                        a110 = ig(i1+1,i2+1,i3  )
                        a101 = ig(i1+1,i2,  i3+1)
                        a111 = ig(i1+1,i2+1,i3+1)
                        og(o1,o2,o3) = (a111 - a110 - a011 - a101 + 
     1                  a001 + a010 + a100 - a000) * d3 * d2 * d1 + 
     2                  (a011 + a000 - a001 - a010) * d3 * d2 + 
     3                  (a101 + a000 - a100 - a001) * d3 * d1 + 
     4                  (a110 + a000 - a100 - a010) * d2 * d1 + 
     5                  (a001 - a000) * d3 + (a010 - a000) * d2 + 
     6                  (a100 - a000) * d1 + a000
                        ENDDO
                ENDDO
        ENDDO
        END
C--------------------------------------------------------------------------
C       compute linear intepolation coefficients: index and residual
C--------------------------------------------------------------------------
        SUBROUTINE axiscoeff (in,io,id,on,oo,od,mn,mx,resid,indx)
C input length (input)
        INTEGER in
C output length (input)
        INTEGER on
C input origin (input)
        REAL    io
C output origin (input)
        REAL    oo
C input sampling (input)
        REAL    id
C outout sampling (input)
        REAL    od
C pad bounds (output)
	INTEGER mn, mx
C residuals (output)
        REAL    resid(on)
C interpolation indexes (output)
        INTEGER indx(on)
C loop index (internal)
        INTEGER i
C bounds (internal)
        REAL    x, y, z, min, mxx
        REAL    od1, oo1, io1

        od1 = od / id
        oo1 = oo / id
        io1 = io / id
	mn = 1
	mx = on
        min = 1.0
        mxx = in
        x = oo1 - io1 - od1 + 1
        DO i=1,on
                x = x + od1
                y = AMAX1 (x,min)
                z = AMIN1 (y,mxx)
                indx(i) = z
                resid(i) = z - indx(i)
		IF (x .NE. z) THEN
			IF (mn .EQ. i) THEN
				mn = i + 1
			ELSE IF (mx .EQ. on) THEN
				mx = i
			ENDIF
		ENDIF
        ENDDO
C	WRITE (*,*) 1,mn,mx,on
        END
