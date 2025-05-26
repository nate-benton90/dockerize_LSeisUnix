h18007
s 00000/00000/00086
d D 1.2 88/11/15 14:19:41 shuki 2 1
c 
e
s 00086/00000/00000
d D 1.1 88/05/05 07:25:31 shuki 1 0
c date and time created 88/05/05 07:25:31 by shuki
e
u
U
f e 0
t
T
I 1
       subroutine synttr(trace,nsamp,dt,iwave,hcut)
********************************************************
**    convolution of a trace with synthetic wavelet.
**
**    trace  -  input trace
**    nsamp  -  # of samples
**    dt     -  time interval (sec)
**    iwave  -  1 - gaussian
**              2 - ricker
**              3 - min. phase
**    hcut   -  highcut frequency (Hz)
**
********************************************************
        dimension trace(1),wavelt(500)
        call wave(wavelt,nwavlt,iwave,hcut,dt)
        call convol(trace,wavelt,nsamp,nwavlt)
	return
	end
************************************************	
      SUBROUTINE CONVOL(A,B,NA,NB)
C     CONVOLUTION OF A AND B. RESULT IN A. NA,NB LENGTH OF A AND B.
      dIMENSION A(1),B(1),D(7000)
      ishift=nb/2

		ishift = 0

      MM=NA+NB-1
      DO 1 I=1,MM
1     D(I)=0.
      DO 2 I=1,NA
      DO 2 J=1,NB
 2    D(I+J-1)=D(I+J-1)+A(I)*B(J)
      DO 5 I=1,NA
 5    A(I)=D(I+ishift)
      RETURN
      eND
************************************************	
        SUBROUTINE WAVE(wavelt,nwavlt,iwave,hcut,dt)
***********************************************
**
**     iwave   :  1 - gaussian
**                2 - ricker
**                3 - min. phase
**
***********************************************
        DIMENSION WAVELT(1),wmin(16)
c        data wmin/0.01,0.7,1.,0.47,-0.45,-0.9,-0.62,-0.25,0.3,
c     .            0.4,0.37,0.24,0.18,0.12,0.08,0.01/
        data wmin/1.,1.368,0.6765,-0.6542,-1.221,-0.8854,-0.3306,
     .            0.4044,0.5619,0.5086,0.3384,0.2478,0.17,0.1096,
     .            0.0148,0.00019/
        PI=3.14159265
        TWPI=2.*PI
        if(iwave.eq.1)then
        agauss=hcut*2.
        tcut=3./agauss
        else
        agauss=hcut*0.5
        tcut=1.5/agauss
        endif
        NWAVLT=TCUT*2/DT
        IF(IWAVE.EQ.2) GO TO 50
        IF(IWAVE.EQ.3) GO TO 60
C       ****************
C       ...RICKER.......
C       ****************
        NW=NWAVLT/2
        DO 10 I=1,NWAVLT
        S=(-NW+I-1)*DT*AGAUSS
10      WAVELT(I)=EXP(-2.*S*S)*COS(TWPI*S)
        RETURN
C       ****************
C       ...GAUSSIAN.....
C       ****************
50      DO 11 I=1,NWAVLT
        X=(I-NWAVLT/2)*DT*AGAUSS
11      WAVELT(I)=EXP(-X*X*0.5)
        RETURN
C       ****************
C       ...MIN. PHASE...
C       ****************
60      nwavlt=16
        do 12 i=1,nwavlt
12      wavelt(i)=wmin(i)/1.368
        return
        END
E 1
