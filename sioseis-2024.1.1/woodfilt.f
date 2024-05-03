!
!--------------------------------------------------------
!
!
!      subroutine notch(inunit,npts,nsize,itype)
!
!       real*4            data(30000)
!       real wrk(30000)
!
!       integer*2         H2(120)
!       integer*4         H4(60)
!
!       integer*4        ioutunit
!
!       equivalence(H2(1),H4(1))
!
!       ioutunit=20
!
!      call open_out_io(inunit,ioutunit,npts,nsize,itype)
!
! 01   write(6,92)
!      read(5,*,err=01) f0,width
!
!
! 92   format(/,' Central frequency and notch width (Hz)-> ',$)
!
!
!          f1 = f0 - width
!          f2 = f0
!          f3 = f0
!          f4 = f0 + width
!      call count(inunit,npts,nsize,nrecords)
!
!      CALL PODISC( inunit,  1, 900)
!      CALL RDDISC( inunit, H4,  60, ISTAT )
!
!      si_ms=float(H2(59))*.001
!      dt = si_ms*0.001
!
!      print *, ' '
!      print *, '   Header sample interval is',si_ms,' msec.'
!      iwin = nint(twin/si_ms)
!
!      call bandpas(f1,f2,si_ms,d1,fg)
!
!      print *, ' '
!      print *, '  Notch filtering',nrecords,' records ... '
!      print *, ' '
!
!      do irec=1,nrecords
!         iword=900+(irec-1)*nsize
!         CALL PODISC( inunit,    1, iword)
!         CALL RDDISC( inunit,   H4,    60, ISTAT )
!         CALL RDDATA( inunit, data,  npts, itype )
!
!-----|--1----.----2----.----3----.----4----.----5----.----6----.----7-|
! Pad the data to a power of 2
!      nt = npts
!      npad  = 2*ip2ge(nt)
!      npad2 = npad - nt
!      ntold = nt
!      call pad2(data,1,nt,0,0,0,npad2)
!
! Apply filter
!      write(*,*),nt,dt,nx,f1,f2,f3,f4,key
!      call filt(data,nt,dt,1,f1,f2,f3,f4,-1)
!
! Decimate data to original size
!      call decm(data,1,1,1,nt,1,ntold,1)
!
!-----|--1----.----2----.----3----.----4----.----5----.----6----.----7-|
!         CALL PODISC( ioutunit,    1, iword)
!         CALL WRDISC( ioutunit,   H4,    60, ISTAT )
!         CALL WRDISC( ioutunit, data,  npts, ISTAT )
!
!         if(mod(irec,100) .eq. 0) type *,' Wrote rec ',irec
!       end do
!
! 999   print *, ' ... done. '
!       print *, ' '
!       close(ioutunit)
! 901   return
!       end
!-----|--1----.----2----.----3----.----4----.----5----.----6----.----7-|
      subroutine woodfilt(data,nt,dt,nx,f1,f2,f3,f4,key)
!
! This will filter an array of real points in the Fourier domain
! according to the user specified bandpass.
!
!     DO PADDING PRIOR TO CALLING THIS ROUTINE
!   
! dt sample rate. if dt is in seconds f1 - f4 are in Hz.
!   
!                         wtw 1991
!                         modified 9/93
!
! From the window subroutine:
! For key = 1 the ramp used between f1,f2 and f3,f4 is a cosine taper.
! For key = 2 f1 and f4 are slopes in db/octave.
!
! The window is fully open between f2 and f3 inclusive.
! The window is fully closed at and outside of f1 and f4.
! If f1 = f2 and f3 = f4 you get a simple boxcar truncation, on at f2, f3
! If f1 = f2 = f3 = f4 you get a delta function.
!
! If key is negative then the middle is cut out and the ends are kept.
!
!                         wtw 1992
!
!--------------------------------------------------------------------
!
      dimension data(*)
      if (nt.ne.ip2ge(nt)) stop'nt is not a power of 2 in woodfilt.f'
!
      pi = 3.141592654
      df = 1/(dt*(nt+2))
      fnt = float(nt)
!
      do 10, jx=1,nx
        k1 = (jx-1)*nt+1
!
! Go into the fourier domain
!--------------------
        call four2(data(k1),nt,1,-1,0)
!--------------------
! The frequencies are complex and loaded in the real array such that
! data(1) and data(2) are the real and imaginary parts of the first 
! frequency, data(3) and data(4) are the complex values for the second
! frequency, etc. For example a series of 128 real samples will end up
! as 64 complex frequencies plus one more sample which holds nyquist.
!
        if (abs(key).eq.1) then
          s1 = 2.*f1/df
          s4 = 2.*f4/df
        else
          s1 = f1
          s4 = f4
        endif
        s2 = 2.*f2/df
        s3 = 2.*f3/df
! To make this filter truly a zero phase filter, we should adjust it so
! the edges of the window apply the same gain to each pair of numbers 
! (single complex number) rather than to each number individually. As it
! is now, the high edge of the window knocks the i part down slightly
! more than its r compliment. If the i and r parts are gained equally, 
! the phase is unaltered.
        call wwindow(data(k1),nt,s1,s2,s3,s4,key)
        data(k1+nt-1) = 0.0
        data(k1+nt) = 0.0
!
! Come out of the fourier domain
!--------------------
        call four2(data(k1),nt,1,1,-1)
!--------------------
!
        data(k1+nt) = 0.0
        data(k1+nt+1) = 0.0
10    continue
!
! Scale data by number of fourier transform elements.
      ndat = nx*nt
      do j=1,ndat
20       data(j) = data(j)/fnt
      enddo
      return
      end
!-----|--0---------0---------0---------0---------0---------0---------0-|
      subroutine wwindow(ser,ns,s1,s2,s3,s4,key)
!
! This windows a series.
! ser   is returned as the windowed series.
! n     is the no. of ser points
!   
!  1.0 |             ________________
!      |            /                \
!      |           /                  \
!      |          /                    \ 
!      |         /                      \
!      |        /                        \
!  0.0 |_______/__________________________\________________________
!   
!      ^       ^     ^               ^    ^                       ^      
!      |       |     |               |    |                       |
!      1       s1    s2              s3   s4                      ns
!
! For key = 1 the ramp used between s1,s2 and s3,s4 is a cosine taper.
! For key = 2 s1 and s4 are slopes in db/octave.
!
! The window is fully open between s2 and s3 inclusive.
! The window is fully closed at and outside of s1 and s4.
! If s1 = s2 and s3 = s4 you get a simple boxcar truncation, on at s2,s3
! If s1 = s2 = s3 = s4 you get a delta function.
!
! If key is negative then the middle is cut out and the ends are kept.
!
!                         wtw 1992
!
      dimension ser(*)
      pi = 2.*acos(0.0)
!
! Loop over samples
      do 20 i = 1,ns
        si = float(i)
!
! ----> cosine ramp or truncation
        if (abs(key).eq.1) then
          if (si.ge.s2.and.si.le.s3) then
            win = 1.0
          else if (si.le.s1.or.si.ge.s4) then
            win = 0.0
          else if (si.gt.s1.and.si.lt.s2) then
            win = 0.5 + 0.5 * cos(pi*(s2-si)/(s2-s1))
          else if (si.gt.s3.and.si.lt.s4) then
            win = 0.5 + 0.5 * cos(pi*(si-s3)/(s4-s3)) 
          endif
!
! ----> decibel per octave ramp
        else if (abs(key).eq.2) then
          if (si.lt.s2) then
            win = 10.**( -s1*log(s2/si)/(20*log(2.)) )
          else if (si.ge.s2.and.si.le.s3) then
            win = 1.0
          else if (si.gt.s3) then
            win = 10.**( -s4*log(si/s3)/(20*log(2.)) )
          endif
        else
          write(*,*)' key =',key
          stop'Wrong key value in window.f'
        endif
!
        if (key.lt.0) win = 1 - win
        ser(i) = ser(i) * win
!        write(97,*)key,win
20    continue
      return
      end
!-----|--0---------0---------0---------0---------0---------0---------0-|
      SUBROUTINE FOUR2 (DATA,N,NDIM,ISIGN,IFORM)                                
!     COOLEY-TUKEY FAST FOURIER TRANSFORM IN USASI BASIC FORTRAN.               
!     MULTI-DIMENSIONAL TRANSFORM, EACH DIMENSION A POWER OF TWO,               
!     COMPLEX OR REAL DATA.                                                     
!     TRANSFORM(K1,K2,...) = SUM(DATA(J1,J2,...)*EXP(ISIGN*2*PI*SQRT(-1)        
!     *((J1-1)*(K1-1)/N(1)+(J2-1)*(K2-1)/N(2)+...))), SUMMED FOR ALL            
!     J1 AND K1 FROM 1 TO N(1), J2 AND K2 FROM 1 TO N(2),                       
!     ETC. FOR ALL NDIM SUBSCRIPTS.  NDIM MUST BE POSITIVE AND                  
!     EACH N(IDIM) MUST BE A POWER OF TWO.  ISIGN IS +1 OR -1.                  
!     LET NTOT = N(1)*N(2)*...*N(NDIM).  THEN A -1 TRANSFORM                    
!     FOLLOWED BY A +1 ONE (OR VICE VERSA) RETURNS NTOT                         
!     TIMES THE ORIGINAL DATA.  IFORM = 1, 0 OR -1, AS DATA IS                  
!     COMPLEX, REAL OR THE FIRST HALF OF A COMPLEX ARRAY.  TRANSFORM            
!     VALUES ARE RETURNED TO ARRAY DATA.  THEY ARE COMPLEX, REAL OR             
!     THE FIRST HALF OF A COMPLEX ARRAY, AS IFORM = 1, -1 OR 0.                 
!     THE TRANSFORM OF A REAL ARRAY (IFORM = 0) DIMENSIONED N(1) BY N(2)        
!     BY ... WILL BE RETURNED IN THE SAME ARRAY, NOW CONSIDERED TO              
!     BE COMPLEX OF DIMENSIONS N(1)/2+1 BY N(2) BY ....  NOTE THAT IF           
!     IFORM = 0 OR -1, N(1) MUST BE EVEN, AND ENOUGH ROOM MUST BE               
!     RESERVED.  THE MISSING VALUES MAY BE OBTAINED BY COMPLEX CONJUGA-         
!     TION.  THE REVERSE TRANSFORMATION, OF A HALF COMPLEX ARRAY DIMEN-         
!     SIONED N(1)/2+1 BY N(2) BY ..., IS ACCOMPLISHED BY SETTING IFORM          
!     TO -1.  IN THE N ARRAY, N(1) MUST BE THE TRUE N(1), NOT N(1)/2+1.         
!     THE TRANSFORM WILL BE REAL AND RETURNED TO THE INPUT ARRAY.               
!     RUNNING TIME IS PROPORTIONAL TO NTOT*LOG2(NTOT), RATHER THAN              
!     THE NAIVE NTOT**2.  FURTHERMORE, LESS ERROR IS BUILT UP.                  
!     WRITTEN BY NORMAN BRENNER OF MIT LINCOLN LABORATORY, JANUARY 1969.        
!     SEE-- IEEE AUDIO TRANSACTIONS (JUNE 1967), SPECIAL ISSUE ON FFT.          
!
! Alternate documentation of IFORM by wtwood, 5/95
!
! To go from a real time series to frequency
!       call four2(data,n,ndim,-1,0)
! To get back again 
!       call four2(data,n,ndim,1,-1)
!
! IFORM = 0
! 	When IFORM = 0 then the input is assumed to be n real numbers where
! n is a power of two. If this is true the output will be n/2+1 complex
! numbers or n+2 real numbers. BE SURE TO LEAVE SPACE! These represent 
! the positive frequencies. Since the input was real the negative
! frequencies are the complex conjugate of the positive frequencies and
! you can compute these outside of this subroutine. To get back into the 
! original domain use IFORM = -1, and keep n the same.
!
! IFORM = -1
!	When IFORM = -1 the input is assumed to be n/2+1 complex numbers
! (n+2) real numbers which represent only the half the series, namely the
! positive frequencies. The output is then n real samples.
!
! IFORM = 1
!	When IFORM = 1 things are a bit simpler. The input is assumed to
! be a complex series of n complex numbers (2n numbers). That is if you
! have 128 complex samples n should be 128, but there will be 256 numbers.
! The output has the same form, with n complex frequencies, from D.C. to
! nyquist and back to D.C.
!
      DIMENSION DATA(*), N(*)
      NTOT=1
      DO IDIM=1,NDIM
 10      NTOT=NTOT*N(IDIM)
      ENDDO
!      IF (IFORM) 70,20,20
      IF (IFORM .LT 0 ) THEN
 20   NREM=NTOT
      DO 60 IDIM=1,NDIM
      NREM=NREM/N(IDIM)
      NPREV=NTOT/(N(IDIM)*NREM)
      NCURR=N(IDIM)
!      IF (IDIM-1+IFORM) 30,30,40
      IF (IDIM-1+IFORM .LE 0 ) THEN
 30      NCURR=NCURR/2
      ENDIF
 40   CALL BITRV (DATA,NPREV,NCURR,NREM)
      CALL COOL2 (DATA,NPREV,NCURR,NREM,ISIGN)
      IF (IDIM-1+IFORM) 50,50,60
 50   CALL FIXRL (DATA,N(1),NREM,ISIGN,IFORM)
      NTOT=(NTOT/N(1))*(N(1)/2+1)
 60   CONTINUE
      RETURN

      ELSE

 70   NTOT=(NTOT/N(1))*(N(1)/2+1)
      NREM=1
      DO 100 JDIM=1,NDIM
      IDIM=NDIM+1-JDIM
      NCURR=N(IDIM)
!      IF (IDIM-1) 80,80,90
      IF (IDIM-1 .LE. 0 ) THEN
 80      NCURR=NCURR/2
         CALL FIXRL (DATA,N(1),NREM,ISIGN,IFORM)
         NTOT=NTOT/(N(1)/2+1)*N(1)
      ENDIF
 90   NPREV=NTOT/(N(IDIM)*NREM)
      CALL BITRV (DATA,NPREV,NCURR,NREM)
      CALL COOL2 (DATA,NPREV,NCURR,NREM,ISIGN)
 100  NREM=NREM*N(IDIM)

      ENDIF
      RETURN
      END
!
!*************************************************
!
      SUBROUTINE BITRV (DATA,NPREV,N,NREM)
!     SHUFFLE THE DATA BY BIT REVERSAL.
!     DIMENSION DATA(NPREV,N,NREM)
!     COMPLEX DATA
!     EXCHANGE DATA(J1,J4REV,J5) WITH DATA(J1,J4,J5) FOR ALL J1 FROM 1
!     TO NPREV, ALL J4 FROM 1 TO N (WHICH MUST BE A POWER OF TWO), AND
!     ALL J5 FROM 1 TO NREM.  J4REV-1 IS THE BIT REVERSAL OF J4-1.  E.G.
!     SUPPOSE N = 32.  THEN FOR J4-1 = 10011, J4REV-1 = 11001, ETC.
      DIMENSION DATA(*)
      IP0=2
      IP1=IP0*NPREV
      IP4=IP1*N
      IP5=IP4*NREM
      I4REV=1
!     I4REV = 1+(J4REV-1)*IP1
      DO 60 I4=1,IP4,IP1
!     I4 = 1+(J4-1)*IP1
      IF (I4-I4REV) 10,30,30
 10   I1MAX=I4+IP1-IP0
      DO 20 I1=I4,I1MAX,IP0
!     I1 = 1+(J1-1)*IP0+(J4-1)*IP1
      DO 20 I5=I1,IP5,IP4
!     I5 = 1+(J1-1)*IP0+(J4-1)*IP1+(J5-1)*IP4
      I5REV=I4REV+I5-I4
!     I5REV = 1+(J1-1)*IP0+(J4REV-1)*IP1+(J5-1)*IP4
      TEMPR=DATA(I5)
      TEMPI=DATA(I5+1)
      DATA(I5)=DATA(I5REV)
      DATA(I5+1)=DATA(I5REV+1)
      DATA(I5REV)=TEMPR
 20   DATA(I5REV+1)=TEMPI
!     ADD ONE WITH DOWNWARD CARRY TO THE HIGH ORDER BIT OF J4REV-1.
 30   CONTINUE
! 30   IP2=IP4/2
      IP2=IP4/2
 40   IF (I4REV-IP2) 60,60,50
 50   I4REV=I4REV-IP2
      IP2=IP2/2
      IF (IP2-IP1) 60,40,40
 60   I4REV=I4REV+IP2
      RETURN
      END
!
!************************************
!
      SUBROUTINE COOL2 (DATA,NPREV,N,NREM,ISIGN)
!     DISCRETE FOURIER TRANSFORM OF LENGTH N.  IN-PLACE COOLEY-TUKEY
!     ALGORITHM, BIT-REVERSED TO NORMAL ORDER, SANDE-TUKEY PHASE SHIFTS.
!     DIMENSION DATA(NPREV,N,NREM)
!     COMPLEX DATA
!     DATA(J1,K4,J5) = SUM(DATA(J1,J4,J5)*EXP(ISIGN*2*PI*I*(J4-1)*
!     (K4-1)/N)), SUMMED OVER J4 = 1 TO N FOR ALL J1 FROM 1 TO NPREV,
!     K4 FROM 1 TO N AND J5 FROM 1 TO NREM.  N MUST BE A POWER OF TWO.
!     METHOD--LET IPREV TAKE THE VALUES 1, 2 OR 4, 4 OR 8, ..., N/16,
!     N/4, N.  THE CHOICE BETWEEN 2 OR 4, ETC., DEPENDS ON WHETHER N IS
!     A POWER OF FOUR.  DEFINE IFACT = 2 OR 4, THE NEXT FACTOR THAT
!     IPREV MUST TAKE, AND IREM = N/(IFACT*IPREV).  THEN--
!     DIMENSION DATA(NPREV,IPREV,IFACT,IREM,NREM)
!     COMPLEX DATA
!     DATA(J1,J2,K3,J4,J5) = SUM(DATA(J1,J2,J3,J4,J5)*EXP(ISIGN*2*PI*I*
!     (K3-1)*((J3-1)/IFACT+(J2-1)/(IFACT*IPREV)))), SUMMED OVER J3 = 1
!     TO IFACT FOR ALL J1 FROM 1 TO NPREV, J2 FROM 1 TO IPREV, K3 FROM
!     1 TO IFACT, J4 FROM 1 TO IREM AND J5 FROM 1 TO NREM.  THIS IS
!     A PHASE-SHIFTED DISCRETE FOURIER TRANSFORM OF LENGTH IFACT.
!     FACTORING N BY FOURS SAVES ABOUT TWENTY FIVE PERCENT OVER FACTOR-
!     ING BY TWOS.  DATA MUST BE BIT-REVERSED INITIALLY.
!     IT IS NOT NECESSARY TO REWRITE THIS SUBROUTINE INTO COMPLEX
!     NOTATION SO LONG AS THE FORTRAN COMPILER USED STORES REAL AND
!     IMAGINARY PARTS IN ADJACENT STORAGE LOCATIONS.  IT MUST ALSO
!     STORE ARRAYS WITH THE FIRST SUBSCRIPT INCREASING FASTEST.
      DIMENSION DATA(*)
      TWOPI=6.2831853072*FLOAT(ISIGN)
      IP0=2
      IP1=IP0*NPREV
      IP4=IP1*N
      IP5=IP4*NREM
      IP2=IP1
!     IP2=IP1*IPROD
      NPART=N
 10   IF (NPART-2) 60,30,20
 20   NPART=NPART/4
      GO TO 10
!     DO A FOURIER TRANSFORM OF LENGTH TWO
 30   IF (IP2-IP4) 40,160,160
 40   IP3=IP2*2
!     IP3=IP2*IFACT
      DO 50 I1=1,IP1,IP0
!     I1 = 1+(J1-1)*IP0
      DO 50 I5=I1,IP5,IP3
!     I5 = 1+(J1-1)*IP0+(J4-1)*IP3+(J5-1)*IP4
      I3A=I5
      I3B=I3A+IP2
!     I3 = 1+(J1-1)*IP0+(J2-1)*IP1+(J3-1)*IP2+(J4-1)*IP3+(J5-1)*IP4
      TEMPR=DATA(I3B)
      TEMPI=DATA(I3B+1)
      DATA(I3B)=DATA(I3A)-TEMPR
      DATA(I3B+1)=DATA(I3A+1)-TEMPI
      DATA(I3A)=DATA(I3A)+TEMPR
 50   DATA(I3A+1)=DATA(I3A+1)+TEMPI
      IP2=IP3
!     DO A FOURIER TRANSFORM OF LENGTH FOUR (FROM BIT REVERSED ORDER)
 60   IF (IP2-IP4) 70,160,160
 70   IP3=IP2*4
!     IP3=IP2*IFACT
!     COMPUTE TWOPI THRU WR AND WI IN DOUBLE PRECISION, IF AVAILABLE.
      THETA=TWOPI/FLOAT(IP3/IP1)
      SINTH=SIN(THETA/2.)
      WSTPR=-2.*SINTH*SINTH
      WSTPI=SIN(THETA)
      WR=1.
      WI=0.
      DO 150 I2=1,IP2,IP1
!     I2 = 1+(J2-1)*IP1
      IF (I2-1) 90,90,80
 80   W2R=WR*WR-WI*WI
      W2I=2.*WR*WI
      W3R=W2R*WR-W2I*WI
      W3I=W2R*WI+W2I*WR
 90   I1MAX=I2+IP1-IP0
      DO 140 I1=I2,I1MAX,IP0
!     I1 = 1+(J1-1)*IP0+(J2-1)*IP1
      DO 140 I5=I1,IP5,IP3
!     I5 = 1+(J1-1)*IP0+(J2-1)*IP1+(J4-1)*IP3+(J5-1)*IP4
      I3A=I5
      I3B=I3A+IP2
      I3C=I3B+IP2
      I3D=I3C+IP2
!     I3 = 1+(J1-1)*IP0+(J2-1)*IP1+(J3-1)*IP2+(J4-1)*IP3+(J5-1)*IP4
      IF (I2-1) 110,110,100
!     APPLY THE PHASE SHIFT FACTORS
 100  TEMPR=DATA(I3B)
      DATA(I3B)=W2R*DATA(I3B)-W2I*DATA(I3B+1)
      DATA(I3B+1)=W2R*DATA(I3B+1)+W2I*TEMPR
      TEMPR=DATA(I3C)
      DATA(I3C)=WR*DATA(I3C)-WI*DATA(I3C+1)
      DATA(I3C+1)=WR*DATA(I3C+1)+WI*TEMPR
      TEMPR=DATA(I3D)
      DATA(I3D)=W3R*DATA(I3D)-W3I*DATA(I3D+1)
      DATA(I3D+1)=W3R*DATA(I3D+1)+W3I*TEMPR
 110  T0R=DATA(I3A)+DATA(I3B)
      T0I=DATA(I3A+1)+DATA(I3B+1)
      T1R=DATA(I3A)-DATA(I3B)
      T1I=DATA(I3A+1)-DATA(I3B+1)
      T2R=DATA(I3C)+DATA(I3D)
      T2I=DATA(I3C+1)+DATA(I3D+1)
      T3R=DATA(I3C)-DATA(I3D)
      T3I=DATA(I3C+1)-DATA(I3D+1)
      DATA(I3A)=T0R+T2R
      DATA(I3A+1)=T0I+T2I
      DATA(I3C)=T0R-T2R
      DATA(I3C+1)=T0I-T2I
      IF (ISIGN) 120,120,130
 120  T3R=-T3R
      T3I=-T3I
 130  DATA(I3B)=T1R-T3I
      DATA(I3B+1)=T1I+T3R
      DATA(I3D)=T1R+T3I
 140  DATA(I3D+1)=T1I-T3R
      TEMPR=WR
      WR=WSTPR*TEMPR-WSTPI*WI+TEMPR
 150  WI=WSTPR*WI+WSTPI*TEMPR+WI
      IP2=IP3
      GO TO 60
 160  RETURN
      END
!
!*********************************
!
      SUBROUTINE FIXRL (DATA,N,NREM,ISIGN,IFORM)
!     FOR IFORM = 0, CONVERT THE TRANSFORM OF A DOUBLED-UP REAL ARRAY,
!     CONSIDERED COMPLEX, INTO ITS TRUE TRANSFORM.  SUPPLY ONLY THE
!     FIRST HALF OF THE COMPLEX TRANSFORM, AS THE SECOND HALF HAS
!     CONJUGATE SYMMETRY.  FOR IFORM = -1, CONVERT THE FIRST HALF
!     OF THE TRUE TRANSFORM INTO THE TRANSFORM OF A DOUBLED-UP REAL
!     ARRAY.  N MUST BE EVEN.
!     USING COMPLEX NOTATION AND SUBSCRIPTS STARTING AT ZERO, THE
!     TRANSFORMATION IS--
!     DIMENSION DATA(N,NREM)
!     ZSTP = EXP(ISIGN*2*PI*I/N)
!     DO 10 I2=0,NREM-1
!     DATA(0,I2) = CONJ(DATA(0,I2))*(1+I)
!     DO 10 I1=1,N/4
!     Z = (1+(2*IFORM+1)*I*ZSTP**I1)/2
!     I1CNJ = N/2-I1
!     DIF = DATA(I1,I2)-CONJ(DATA(I1CNJ,I2))
!     TEMP = Z*DIF
!     DATA(I1,I2) = (DATA(I1,I2)-TEMP)*(1-IFORM)
! 10  DATA(I1CNJ,I2) = (DATA(I1CNJ,I2)+CONJ(TEMP))*(1-IFORM)
!     IF I1=I1CNJ, THE CALCULATION FOR THAT VALUE COLLAPSES INTO
!     A SIMPLE CONJUGATION OF DATA(I1,I2).
      DIMENSION DATA(*)
      TWOPI=6.283185307*FLOAT(ISIGN)
      IP0=2
      IP1=IP0*(N/2)
      IP2=IP1*NREM
      IF (IFORM) 10,70,70
!     PACK THE REAL INPUT VALUES (TWO PER COLUMN)
 10   J1=IP1+1
      DATA(2)=DATA(J1)
      IF (NREM-1) 70,70,20
 20   J1=J1+IP0
      I2MIN=IP1+1
      DO 60 I2=I2MIN,IP2,IP1
      DATA(I2)=DATA(J1)
      J1=J1+IP0
      IF (N-2) 50,50,30
 30   I1MIN=I2+IP0
      I1MAX=I2+IP1-IP0
      DO 40 I1=I1MIN,I1MAX,IP0
      DATA(I1)=DATA(J1)
      DATA(I1+1)=DATA(J1+1)
 40   J1=J1+IP0
 50   DATA(I2+1)=DATA(J1)
 60   J1=J1+IP0
 70   DO 80 I2=1,IP2,IP1
      TEMPR=DATA(I2)
      DATA(I2)=DATA(I2)+DATA(I2+1)
 80   DATA(I2+1)=TEMPR-DATA(I2+1)
      IF (N-2) 200,200,90
 90   THETA=TWOPI/FLOAT(N)
      SINTH=SIN(THETA/2.)
      ZSTPR=-2.*SINTH*SINTH
      ZSTPI=SIN(THETA)
      ZR=(1.-ZSTPI)/2.
      ZI=(1.+ZSTPR)/2.
      IF (IFORM) 100,110,110
 100  ZR=1.-ZR
      ZI=-ZI
 110  I1MIN=IP0+1
      I1MAX=IP0*(N/4)+1
      DO 190 I1=I1MIN,I1MAX,IP0
      DO 180 I2=I1,IP2,IP1
      I2CNJ=IP0*(N/2+1)-2*I1+I2
      IF (I2-I2CNJ) 150,120,120
 120  IF (ISIGN*(2*IFORM+1)) 130,140,140
 130  DATA(I2+1)=-DATA(I2+1)
 140  IF (IFORM) 170,180,180
 150  DIFR=DATA(I2)-DATA(I2CNJ)
      DIFI=DATA(I2+1)+DATA(I2CNJ+1)
      TEMPR=DIFR*ZR-DIFI*ZI
      TEMPI=DIFR*ZI+DIFI*ZR
      DATA(I2)=DATA(I2)-TEMPR
      DATA(I2+1)=DATA(I2+1)-TEMPI
      DATA(I2CNJ)=DATA(I2CNJ)+TEMPR
      DATA(I2CNJ+1)=DATA(I2CNJ+1)-TEMPI
      IF (IFORM) 160,180,180
 160  DATA(I2CNJ)=DATA(I2CNJ)+DATA(I2CNJ)
      DATA(I2CNJ+1)=DATA(I2CNJ+1)+DATA(I2CNJ+1)
 170  DATA(I2)=DATA(I2)+DATA(I2)
      DATA(I2+1)=DATA(I2+1)+DATA(I2+1)
 180  CONTINUE
      TEMPR=ZR-.5
      ZR=ZSTPR*TEMPR-ZSTPI*ZI+ZR
 190  ZI=ZSTPR*ZI+ZSTPI*TEMPR+ZI
!     RECURSION SAVES TIME, AT A SLIGHT LOSS IN ACCURACY.  IF AVAILABLE,
!     USE DOUBLE PRECISION TO COMPUTE ZR AND ZI.
 200  IF (IFORM) 270,210,210
!     UNPACK THE REAL TRANSFORM VALUES (TWO PER COLUMN)
 210  I2=IP2+1
      I1=I2
      J1=IP0*(N/2+1)*NREM+1
      GO TO 250
 220  DATA(J1)=DATA(I1)
      DATA(J1+1)=DATA(I1+1)
      I1=I1-IP0
      J1=J1-IP0
 230  IF (I2-I1) 220,240,240
 240  DATA(J1)=DATA(I1)
      DATA(J1+1)=0.
 250  I2=I2-IP1
      J1=J1-IP0
      DATA(J1)=DATA(I2+1)
      DATA(J1+1)=0.
      I1=I1-IP0
      J1=J1-IP0
      IF (I2-1) 260,260,230
 260  DATA(2)=0.
 270  RETURN
      END
!-----|--0---------0---------0---------0---------0---------0---------0-|
      integer function ip2ge(n)
! 
!     returns the smallest positive-integral power of 2 which is greater than 
!     or equal to n (an integer), up to a maximum of 2**99999
!   mod Oct 96 - rewrite.
! 
! 
!      do 10 n2=1,99999
!      ip2ge=2**n2
!   10 if(ip2ge.ge.n)return
!
!  mod 3 Dec 97 - start with ip2ge = 0 for Alpha compiler warning.

      m = 2
      ip2ge = 0
      DO n2 = 1 ,999
         IF( m .GE. n ) THEN
             ip2ge = m
             RETURN
         ENDIF
         m = m + m
      ENDDO
!
! This is a possible alternative method which is more elegant but may be
! more expensive. Check it out.
!  isize=2**nint(((alog10(float(n))/alog10(2.0))+0.5))
      return
      end 
!-----|--0---------0---------0---------0---------0---------0---------0-|
      subroutine pad2(data,nx,nt,im,ir,it,ib)
!
!                    wtwood 9/93
!
! INPUT:
! data  1-D array containing the data
! nx    No. of traces in the data
! nt    No. of samples (without 60 wrd header) in the data
! im    No. of zero traces  to add to the  LEFT   side
! ir    No. of zero traces  to add to the  RIGHT  side
! it    No. of zero samples to add to the  TOP 
! ib    No. of zero samples to add to the  BOTTOM
!
! OUTPUT:
! data  1-D array containing padded data set
! nx    New no. of traces
! nt    New no. of samples
!
      dimension data(*)
!
!-----|--0---------0---------0---------0---------0---------0---------0-|
      nx2 = im + nx + ir
      nt2 = it + nt + ib
!
! First zero the memory from the end of the data to the end of the new 
! data
      ndat1 = nt*nx
      ndat2 = nt2 * nx2
      do i=ndat1+1,ndat2
5        data(i) = 0.0
      enddo
!
! Start from the last new trace and work forward.
!
      do 60 jx = nx2,1,-1
        k1 = (jx-1)*nt2 
!
! Do the right hand side padding
        if (jx.gt.(im+nx)) then
          do jt=1,nt2
10           data(k1+jt) = 0.0
          enddo
!
! Copy the data to the right place and do the top and bottom padding.
        else if (jx.gt.im) then
          kold1 = (jx-im-1)*nt
          do 20, jt=nt2,1,-1
            if (jt.gt.it.and.jt.le.(it+nt)) then
              data(k1+jt) = data(kold1+jt-it)
            else 
              data(k1+jt) = 0.0
            endif
20        continue
!
! Do the left hand side padding
        else 
          do jt=1,nt2
30           data(k1+jt) = 0.0
          enddo
        endif
60    continue
      nx = nx2
      nt = nt2
!-----|--0---------0---------0---------0---------0---------0---------0-|
      return
      end
!-----|--0---------0---------0---------0---------0---------0---------0-|
      subroutine decm(data,jmin,jmax,nktrc,nt,itop,ibot,nksam)
!
!                           wtwood 10/93
!
! This decimates a data set by plucking out samples and traces, not the
! best way. For better results use a fourier decimation technique.
!
! INPUT
! data      1-D array with at least jmax traces of nt samples each
! jmin    First trace to keep
! jmax    Last trace to keep
! nktrc   Increment of trace to keep
! nt      Number of samples in each input trace 
! itop    first sample to keep
! ibot    Last sample to keep
! nksam   Increment of sample to keep
!
! OUTPUT:
! data   1-D array with   {(jmax-jmin)/nktrc}  traces
!        and              {(ibot-itop)/nksam}  samples.
!
      real data(*)
!-----|--0---------0---------0---------0---------0---------0---------0-|
!
      i1   = 0
      jtrc = 0
      js   = 0
!
      do 100 jx2 = jmin, jmax, nktrc
        jtrc = jtrc + 1
        jt1 = (jx2-1)*nt + itop
        jt2 = (jx2-1)*nt + ibot
!
! Copy relevent portion of trace
        do 20, k = jt1,jt2,nksam
          i1 = i1 + 1
          data(i1) = data(k)
20      continue
100   continue
!
      nx = jtrc
      nt = (ibot-itop+1)/nksam
!-----|--0---------0---------0---------0---------0---------0---------0-|
      return
      end
