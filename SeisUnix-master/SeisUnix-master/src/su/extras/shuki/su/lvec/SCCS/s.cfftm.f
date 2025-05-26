h25110
s 00000/00000/01079
d D 1.4 88/11/15 14:04:29 shuki 4 3
c 
e
s 00002/00003/01077
d D 1.3 88/05/29 10:03:11 moshe 3 2
c 
e
s 00701/00699/00379
d D 1.2 88/05/29 09:56:30 moshe 2 1
c 
e
s 01078/00000/00000
d D 1.1 88/05/29 09:52:45 moshe 1 0
c date and time created 88/05/29 09:52:45 by moshe
e
u
U
f e 0
t
T
I 1
       SUBROUTINE CFFTM(A,B,C,TRIG,IFAX,ISKIP1,ISKIP2,NFFT,
     .          NUM,ISIGN)
************************************************************
C       MIXED RADIX COMPLEX FFT ROUTINE BASED ON TEMPERTON ,JOUR.COMP.
C       PHYS. VOL 52 NUMBER 1 OCT 1983 PAGE 1.
C
D 2
C       A      - first real element (input and output)
C       B      - first imaginary element (input and output)
C       C      - work array (2*nfft)
C       TRIG   - trigonometric functions (2*nfft)
C       IFAX   - FFT factors
C       ISKIP1 - stride within each transform (real count)
C       ISKIP2 - stride between transforms (real count)
C       NFFT   - FFT LENGTH
C       NUM    - NUMBER OF FFT'S TO BE PERFORMED
C       ISIGN  - FFT sign  (-1 = forward, +1 = inverse )
E 2
I 2
C  (R*4)  A      - first real element in data array
C                  (input and output)
C  (R*4)  B      - first imaginary element in data array
C                  (input and output)
C  (R*4)  C      - work array (2*nfft)
C  (R*4)  TRIG   - array containing trigonometric functions (2*nfft)
C  (I*4)  IFAX   - array containing FFT factors
C  (I*4)  ISKIP1 - stride within each transform (real count)
C  (I*4)  ISKIP2 - stride between transforms (real count)
C  (I*4)  NFFT   - FFT LENGTH
C  (I*4)  NUM    - NUMBER OF FFT'S TO BE PERFORMED
C  (I*4)  ISIGN  - FFT sign  (-1 = forward, +1 = inverse )
E 2
************************************************************
       DIMENSION A(1),B(1),C(1),TRIG(1),IFAX(1)
       DO 1 N=1,NUM
        CALL FFTB(A(1+(N-1)*ISKIP2),B(1+(N-1)*ISKIP2),C(1),
     .        C(NFFT+1),NFFT,TRIG,IFAX(2),IFAX(1),ISKIP1,ISIGN)
1       continue
I 3
        return
        end
E 3
C---------------------------------------------------------------------
       subroutine fftb(a,b,c,d,n,trigs,ifax,nfac,iskip,isign)
       complex trigs(1)
       integer ifax(1)
       dimension a(1),c(1),b(1),d(1)
       la=1
        ii=1
       do 10 i=1,nfac
        ifac=ifax(i)
        if(ii.gt.0) call pass(a,b,c,d,
     .     trigs,ifac,la,n,iskip,1,isign)
        if(ii.lt.0) call pass(c,d,a,b,
     .     trigs,ifac,la,n,1,iskip,isign)
        ii=-ii
        la=la*ifac
10      continue
       if(ii.lt.0) then
       n2=n*iskip
        jj=0
        do 11 j=1,n2,iskip
         jj=jj+1
         a(j)=c(jj)
         b(j)=d(jj)
11      continue
       end if
       return
       end
c--------------------------------------------------------
       subroutine pass(a,b,c,d,trigs,ifac,la,n,iskip,
     .     iskip1,isign)
       dimension a(1),c(1),trigs(1),b(1),d(1)
       save sin60,sin72,sin36,sq54,sin3672
       data iflag/0/
       if(iflag.eq.0) then
        iflag=1
       sin60=sin(3.14159265/3.)
       sin72=sin(3.14159265*72./180.)
       sin36=sin(3.14159265*36./180.)
       sq54=0.25*sqrt(5.)
       sin3672=sin36/sin72
       end if
       m=n/ifac
       mskip=m*iskip
       ia=0
       ib=mskip
       ja=0
       laskip=la*iskip1
       jb=laskip
       i=1
       j=1
       jump=(ifac-1)*laskip
       go to(100,200,300,400,500,600),ifac
*100       print *,'radix one encountered'
*      stop 999
100     continue
200       continue
c----------------- factor two ----------------------
        do 10 l=1,la
D 2
         ai_real=a(i)
         ai_imag=b(i)
E 2
I 2
         aireal=a(i)
         aiimag=b(i)
E 2
         iib=i+ib
D 2
         aib_real=a(iib)
         aib_imag=b(iib)
         c(j)=ai_real+aib_real
         d(j)=ai_imag+aib_imag
E 2
I 2
         aibreal=a(iib)
         aibimag=b(iib)
         c(j)=aireal+aibreal
         d(j)=aiimag+aibimag
E 2
         jjb=j+jb
D 2
         c(jjb)=ai_real-aib_real
         d(jjb)=ai_imag-aib_imag
E 2
I 2
         c(jjb)=aireal-aibreal
         d(jjb)=aiimag-aibimag
E 2
        i=i+iskip
        j=j+iskip1
10       continue
        j=j+jump
       do 11 k=la,m-la,la
        kk=2*k+1
D 2
        t_real=trigs(kk)
        t_imag=trigs(kk+1)
        if(isign.eq.-1) t_imag=-t_imag
E 2
I 2
        treal=trigs(kk)
        timag=trigs(kk+1)
        if(isign.eq.-1) timag=-timag
E 2
        do 12 l=1,la
D 2
         ai_real=a(i)
         ai_imag=b(i)
E 2
I 2
         aireal=a(i)
         aiimag=b(i)
E 2
         iib=i+ib
D 2
         aib_real=a(iib)
         aib_imag=b(iib)
         c(j)=ai_real+aib_real
         d(j)=ai_imag+aib_imag
         f1=ai_real-aib_real
         f2=ai_imag-aib_imag
E 2
I 2
         aibreal=a(iib)
         aibimag=b(iib)
         c(j)=aireal+aibreal
         d(j)=aiimag+aibimag
         f1=aireal-aibreal
         f2=aiimag-aibimag
E 2
         jjb=j+jb
D 2
         c(jjb)=t_real*f1-t_imag*f2
         d(jjb)=t_imag*f1+t_real*f2
E 2
I 2
         c(jjb)=treal*f1-timag*f2
         d(jjb)=timag*f1+treal*f2
E 2
        i=i+iskip
        j=j+iskip1
12       continue
        j=j+jump
11       continue
       return
300       continue
c       ------ factor three ---------
       ic=ib+mskip
       jc=jb+laskip
        if(isign.gt.0) then
         do 13 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
          t1_real=aib_real+aic_real
          t1_imag=aib_imag+aic_imag
          t2_real=ai_real-0.5*t1_real
          t2_imag=ai_imag-0.5*t1_imag
          t3_real=sin60*(aib_real-aic_real)
          t3_imag=sin60*(aib_imag-aic_imag)
          c(j)=ai_real+t1_real
          d(j)=ai_imag+t1_imag
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
          t1real=aibreal+aicreal
          t1imag=aibimag+aicimag
          t2real=aireal-0.5*t1real
          t2imag=aiimag-0.5*t1imag
          t3real=sin60*(aibreal-aicreal)
          t3imag=sin60*(aibimag-aicimag)
          c(j)=aireal+t1real
          d(j)=aiimag+t1imag
E 2
          jjb=j+jb
D 2
          c(jjb)=t2_real-t3_imag
          d(jjb)=t2_imag+t3_real
E 2
I 2
          c(jjb)=t2real-t3imag
          d(jjb)=t2imag+t3real
E 2
          jjc=j+jc
D 2
          c(jjc)=t2_real+t3_imag
          d(jjc)=t2_imag-t3_real
E 2
I 2
          c(jjc)=t2real+t3imag
          d(jjc)=t2imag-t3real
E 2
          i=i+iskip
          j=j+iskip1
13        continue
         j=j+jump
        do 14 k=la,m-la,la
         k2=k+k
         kk=k2+1
D 2
         tr1_real=trigs(kk)
         tr1_imag=trigs(kk+1)
E 2
I 2
         tr1real=trigs(kk)
         tr1imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr2_real=trigs(kk)
         tr2_imag=trigs(kk+1)
E 2
I 2
         tr2real=trigs(kk)
         tr2imag=trigs(kk+1)
E 2
         do 15 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
          t1_real=aib_real+aic_real
          t1_imag=aib_imag+aic_imag
          t2_real=ai_real-0.5*t1_real
          t2_imag=ai_imag-0.5*t1_imag
          t3_real=sin60*(aib_real-aic_real)
          t3_imag=sin60*(aib_imag-aic_imag)
          c(j)=ai_real+t1_real
          d(j)=ai_imag+t1_imag
          x1_real=t2_real-t3_imag
          x1_imag=t2_imag+t3_real
          x2_real=t2_real+t3_imag
          x2_imag=t2_imag-t3_real
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
          t1real=aibreal+aicreal
          t1imag=aibimag+aicimag
          t2real=aireal-0.5*t1real
          t2imag=aiimag-0.5*t1imag
          t3real=sin60*(aibreal-aicreal)
          t3imag=sin60*(aibimag-aicimag)
          c(j)=aireal+t1real
          d(j)=aiimag+t1imag
          x1real=t2real-t3imag
          x1imag=t2imag+t3real
          x2real=t2real+t3imag
          x2imag=t2imag-t3real
E 2
          jjb=j+jb
D 2
          c(jjb)=tr1_real*x1_real-tr1_imag*x1_imag
          d(jjb)=tr1_imag*x1_real+tr1_real*x1_imag
E 2
I 2
          c(jjb)=tr1real*x1real-tr1imag*x1imag
          d(jjb)=tr1imag*x1real+tr1real*x1imag
E 2
          jjc=j+jc
D 2
          c(jjc)=tr2_real*x2_real-tr2_imag*x2_imag
          d(jjc)=tr2_imag*x2_real+tr2_real*x2_imag
E 2
I 2
          c(jjc)=tr2real*x2real-tr2imag*x2imag
          d(jjc)=tr2imag*x2real+tr2real*x2imag
E 2
          i=i+iskip
          j=j+iskip1
15        continue
         j=j+jump
14        continue
       else
         do 16 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
          t1_real=aib_real+aic_real
          t1_imag=aib_imag+aic_imag
          t2_real=ai_real-0.5*t1_real
          t2_imag=ai_imag-0.5*t1_imag
          t3_real=sin60*(aib_real-aic_real)
          t3_imag=sin60*(aib_imag-aic_imag)
          c(j)=ai_real+t1_real
          d(j)=ai_imag+t1_imag
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
          t1real=aibreal+aicreal
          t1imag=aibimag+aicimag
          t2real=aireal-0.5*t1real
          t2imag=aiimag-0.5*t1imag
          t3real=sin60*(aibreal-aicreal)
          t3imag=sin60*(aibimag-aicimag)
          c(j)=aireal+t1real
          d(j)=aiimag+t1imag
E 2
          jjb=j+jb
D 2
          c(jjb)=t2_real+t3_imag
          d(jjb)=t2_imag-t3_real
E 2
I 2
          c(jjb)=t2real+t3imag
          d(jjb)=t2imag-t3real
E 2
          jjc=j+jc
D 2
          c(jjc)=t2_real-t3_imag
          d(jjc)=t2_imag+t3_real
E 2
I 2
          c(jjc)=t2real-t3imag
          d(jjc)=t2imag+t3real
E 2
          i=i+iskip
          j=j+iskip1
16         continue
         j=j+jump
        do 17 k=la,m-la,la
         k2=k+k
         kk=k2+1
D 2
         tr1_real=trigs(kk)
         tr1_imag=-trigs(kk+1)
E 2
I 2
         tr1real=trigs(kk)
         tr1imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr2_real=trigs(kk)
         tr2_imag=-trigs(kk+1)
E 2
I 2
         tr2real=trigs(kk)
         tr2imag=-trigs(kk+1)
E 2
         do 18 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
          t1_real=aib_real+aic_real
          t1_imag=aib_imag+aic_imag
          t2_real=ai_real-0.5*t1_real
          t2_imag=ai_imag-0.5*t1_imag
          t3_real=sin60*(aib_real-aic_real)
          t3_imag=sin60*(aib_imag-aic_imag)
          c(j)=ai_real+t1_real
          d(j)=ai_imag+t1_imag
          x1_real=t2_real+t3_imag
          x1_imag=t2_imag-t3_real
          x2_real=t2_real-t3_imag
          x2_imag=t2_imag+t3_real
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
          t1real=aibreal+aicreal
          t1imag=aibimag+aicimag
          t2real=aireal-0.5*t1real
          t2imag=aiimag-0.5*t1imag
          t3real=sin60*(aibreal-aicreal)
          t3imag=sin60*(aibimag-aicimag)
          c(j)=aireal+t1real
          d(j)=aiimag+t1imag
          x1real=t2real+t3imag
          x1imag=t2imag-t3real
          x2real=t2real-t3imag
          x2imag=t2imag+t3real
E 2
          jjb=j+jb
D 2
          c(jjb)=tr1_real*x1_real-tr1_imag*x1_imag
          d(jjb)=tr1_imag*x1_real+tr1_real*x1_imag
E 2
I 2
          c(jjb)=tr1real*x1real-tr1imag*x1imag
          d(jjb)=tr1imag*x1real+tr1real*x1imag
E 2
          jjc=j+jc
D 2
          c(jjc)=tr2_real*x2_real-tr2_imag*x2_imag
          d(jjc)=tr2_imag*x2_real+tr2_real*x2_imag
E 2
I 2
          c(jjc)=tr2real*x2real-tr2imag*x2imag
          d(jjc)=tr2imag*x2real+tr2real*x2imag
E 2
          i=i+iskip
          j=j+iskip1
18        continue
         j=j+jump
17      continue
       endif
       return
400       continue
c ------------  factor 4 ---------------------------
       ic=ib+mskip
       id=ic+mskip
       jc=jb+laskip
       jd=jc+laskip
        if(isign.gt.0) then
         do 19 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
E 2
          iid=i+id
D 2
          aid_real=a(iid)
          aid_imag=b(iid)
          t1_real=ai_real+aic_real
          t1_imag=ai_imag+aic_imag
          t2_real=aib_real+aid_real
          t2_imag=aib_imag+aid_imag
          t3_real=ai_real-aic_real
          t3_imag=ai_imag-aic_imag
          t4_real=aib_real-aid_real
          t4_imag=aib_imag-aid_imag
          c(j)=t1_real+t2_real
          d(j)=t1_imag+t2_imag
E 2
I 2
          aidreal=a(iid)
          aidimag=b(iid)
          t1real=aireal+aicreal
          t1imag=aiimag+aicimag
          t2real=aibreal+aidreal
          t2imag=aibimag+aidimag
          t3real=aireal-aicreal
          t3imag=aiimag-aicimag
          t4real=aibreal-aidreal
          t4imag=aibimag-aidimag
          c(j)=t1real+t2real
          d(j)=t1imag+t2imag
E 2
          jjb=j+jb
D 2
          c(jjb)=t3_real-t4_imag
          d(jjb)=t3_imag+t4_real
E 2
I 2
          c(jjb)=t3real-t4imag
          d(jjb)=t3imag+t4real
E 2
          jjc=j+jc
D 2
          c(jjc)=t1_real-t2_real
          d(jjc)=t1_imag-t2_imag
E 2
I 2
          c(jjc)=t1real-t2real
          d(jjc)=t1imag-t2imag
E 2
          jjd=j+jd
D 2
          c(jjd)=t3_real+t4_imag
          d(jjd)=t3_imag-t4_real
E 2
I 2
          c(jjd)=t3real+t4imag
          d(jjd)=t3imag-t4real
E 2
          i=i+iskip
          j=j+iskip1
19        continue
         j=j+jump
        do 20 k=la,m-la,la
         k2=k+k
         kk=k2+1
D 2
         tr1_real=trigs(kk)
         tr1_imag=trigs(kk+1)
E 2
I 2
         tr1real=trigs(kk)
         tr1imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr2_real=trigs(kk)
         tr2_imag=trigs(kk+1)
E 2
I 2
         tr2real=trigs(kk)
         tr2imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr3_real=trigs(kk)
         tr3_imag=trigs(kk+1)
E 2
I 2
         tr3real=trigs(kk)
         tr3imag=trigs(kk+1)
E 2
         do 21 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
E 2
          iid=i+id
D 2
          aid_real=a(iid)
          aid_imag=b(iid)
          t1_real=ai_real+aic_real
          t1_imag=ai_imag+aic_imag
          t2_real=aib_real+aid_real
          t2_imag=aib_imag+aid_imag
          t3_real=ai_real-aic_real
          t3_imag=ai_imag-aic_imag
          t4_real=aib_real-aid_real
          t4_imag=aib_imag-aid_imag
          c(j)=t1_real+t2_real
          d(j)=t1_imag+t2_imag
          x1_real=t3_real-t4_imag
          x1_imag=t3_imag+t4_real
          x2_real=t1_real-t2_real
          x2_imag=t1_imag-t2_imag
          x3_real=t3_real+t4_imag
          x3_imag=t3_imag-t4_real
E 2
I 2
          aidreal=a(iid)
          aidimag=b(iid)
          t1real=aireal+aicreal
          t1imag=aiimag+aicimag
          t2real=aibreal+aidreal
          t2imag=aibimag+aidimag
          t3real=aireal-aicreal
          t3imag=aiimag-aicimag
          t4real=aibreal-aidreal
          t4imag=aibimag-aidimag
          c(j)=t1real+t2real
          d(j)=t1imag+t2imag
          x1real=t3real-t4imag
          x1imag=t3imag+t4real
          x2real=t1real-t2real
          x2imag=t1imag-t2imag
          x3real=t3real+t4imag
          x3imag=t3imag-t4real
E 2
          jjb=j+jb
D 2
          c(jjb)=tr1_real*x1_real-tr1_imag*x1_imag
          d(jjb)=tr1_imag*x1_real+tr1_real*x1_imag
E 2
I 2
          c(jjb)=tr1real*x1real-tr1imag*x1imag
          d(jjb)=tr1imag*x1real+tr1real*x1imag
E 2
          jjc=j+jc
D 2
          c(jjc)=tr2_real*x2_real-tr2_imag*x2_imag
          d(jjc)=tr2_imag*x2_real+tr2_real*x2_imag
E 2
I 2
          c(jjc)=tr2real*x2real-tr2imag*x2imag
          d(jjc)=tr2imag*x2real+tr2real*x2imag
E 2
          jjd=j+jd
D 2
          c(jjd)=tr3_real*x3_real-tr3_imag*x3_imag
          d(jjd)=tr3_imag*x3_real+tr3_real*x3_imag
E 2
I 2
          c(jjd)=tr3real*x3real-tr3imag*x3imag
          d(jjd)=tr3imag*x3real+tr3real*x3imag
E 2
          i=i+iskip
          j=j+iskip1
21        continue
         j=j+jump
20       continue
       else
         do 22 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
E 2
          iid=i+id
D 2
          aid_real=a(iid)
          aid_imag=b(iid)
          t1_real=ai_real+aic_real
          t1_imag=ai_imag+aic_imag
          t2_real=aib_real+aid_real
          t2_imag=aib_imag+aid_imag
          t3_real=ai_real-aic_real
          t3_imag=ai_imag-aic_imag
          t4_real=aib_real-aid_real
          t4_imag=aib_imag-aid_imag
          c(j)=t1_real+t2_real
          d(j)=t1_imag+t2_imag
E 2
I 2
          aidreal=a(iid)
          aidimag=b(iid)
          t1real=aireal+aicreal
          t1imag=aiimag+aicimag
          t2real=aibreal+aidreal
          t2imag=aibimag+aidimag
          t3real=aireal-aicreal
          t3imag=aiimag-aicimag
          t4real=aibreal-aidreal
          t4imag=aibimag-aidimag
          c(j)=t1real+t2real
          d(j)=t1imag+t2imag
E 2
          jjb=j+jb
D 2
          c(jjb)=t3_real+t4_imag
          d(jjb)=t3_imag-t4_real
E 2
I 2
          c(jjb)=t3real+t4imag
          d(jjb)=t3imag-t4real
E 2
          jjc=j+jc
D 2
          c(jjc)=t1_real-t2_real
          d(jjc)=t1_imag-t2_imag
E 2
I 2
          c(jjc)=t1real-t2real
          d(jjc)=t1imag-t2imag
E 2
          jjd=j+jd
D 2
          c(jjd)=t3_real-t4_imag
          d(jjd)=t3_imag+t4_real
E 2
I 2
          c(jjd)=t3real-t4imag
          d(jjd)=t3imag+t4real
E 2
          i=i+iskip
          j=j+iskip1
22         continue
         j=j+jump
        do 23 k=la,m-la,la
         k2=k+k
         kk=k2+1
D 2
         tr1_real=trigs(kk)
         tr1_imag=-trigs(kk+1)
E 2
I 2
         tr1real=trigs(kk)
         tr1imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr2_real=trigs(kk)
         tr2_imag=-trigs(kk+1)
E 2
I 2
         tr2real=trigs(kk)
         tr2imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr3_real=trigs(kk)
         tr3_imag=-trigs(kk+1)
E 2
I 2
         tr3real=trigs(kk)
         tr3imag=-trigs(kk+1)
E 2
         do 24 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
           iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
E 2
          iid=i+id
D 2
          aid_real=a(iid)
          aid_imag=b(iid)
          t1_real=ai_real+aic_real
          t1_imag=ai_imag+aic_imag
          t2_real=aib_real+aid_real
          t2_imag=aib_imag+aid_imag
          t3_real=ai_real-aic_real
          t3_imag=ai_imag-aic_imag
          t4_real=aib_real-aid_real
          t4_imag=aib_imag-aid_imag
          c(j)=t1_real+t2_real
          d(j)=t1_imag+t2_imag
          x1_real=t3_real+t4_imag
          x1_imag=t3_imag-t4_real
          x2_real=t1_real-t2_real
          x2_imag=t1_imag-t2_imag
          x3_real=t3_real-t4_imag
          x3_imag=t3_imag+t4_real
E 2
I 2
          aidreal=a(iid)
          aidimag=b(iid)
          t1real=aireal+aicreal
          t1imag=aiimag+aicimag
          t2real=aibreal+aidreal
          t2imag=aibimag+aidimag
          t3real=aireal-aicreal
          t3imag=aiimag-aicimag
          t4real=aibreal-aidreal
          t4imag=aibimag-aidimag
          c(j)=t1real+t2real
          d(j)=t1imag+t2imag
          x1real=t3real+t4imag
          x1imag=t3imag-t4real
          x2real=t1real-t2real
          x2imag=t1imag-t2imag
          x3real=t3real-t4imag
          x3imag=t3imag+t4real
E 2
          jjb=j+jb
D 2
          c(jjb)=tr1_real*x1_real-tr1_imag*x1_imag
          d(jjb)=tr1_imag*x1_real+tr1_real*x1_imag
E 2
I 2
          c(jjb)=tr1real*x1real-tr1imag*x1imag
          d(jjb)=tr1imag*x1real+tr1real*x1imag
E 2
          jjc=j+jc
D 2
          c(jjc)=tr2_real*x2_real-tr2_imag*x2_imag
          d(jjc)=tr2_imag*x2_real+tr2_real*x2_imag
E 2
I 2
          c(jjc)=tr2real*x2real-tr2imag*x2imag
          d(jjc)=tr2imag*x2real+tr2real*x2imag
E 2
          jjd=j+jd
D 2
          c(jjd)=tr3_real*x3_real-tr3_imag*x3_imag
          d(jjd)=tr3_imag*x3_real+tr3_real*x3_imag
E 2
I 2
          c(jjd)=tr3real*x3real-tr3imag*x3imag
          d(jjd)=tr3imag*x3real+tr3real*x3imag
E 2
          i=i+iskip
          j=j+iskip1
24        continue
         j=j+jump
23      continue
       endif
       return
500       continue
c ------------  factor 5 ---------------------------
       ic=ib+mskip
       id=ic+mskip
       ie=id+mskip
       jc=jb+laskip
       jd=jc+laskip
       je=jd+laskip
        if(isign.gt.0) then
         do 25 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
E 2
          iid=i+id
D 2
          aid_real=a(iid)
          aid_imag=b(iid)
E 2
I 2
          aidreal=a(iid)
          aidimag=b(iid)
E 2
          iie=i+ie
D 2
          aie_real=a(iie)
          aie_imag=b(iie)
          t1_real=aib_real+aie_real
          t1_imag=aib_imag+aie_imag
          t2_real=aic_real+aid_real
          t2_imag=aic_imag+aid_imag
          t3_real=sin72*(aib_real-aie_real)
          t3_imag=sin72*(aib_imag-aie_imag)
          t4_real=sin72*(aic_real-aid_real)
          t4_imag=sin72*(aic_imag-aid_imag)
          t5_real=t1_real+t2_real
          t5_imag=t1_imag+t2_imag
          t6_real=sq54*(t1_real-t2_real)
          t6_imag=sq54*(t1_imag-t2_imag)
          t7_real=ai_real-t5_real*0.25
          t7_imag=ai_imag-t5_imag*0.25
          t8_real=t7_real+t6_real
          t8_imag=t7_imag+t6_imag
          t9_real=t7_real-t6_real
          t9_imag=t7_imag-t6_imag
          t10_real=t3_real+sin3672*t4_real
          t10_imag=t3_imag+sin3672*t4_imag
          t11_real=sin3672*t3_real-t4_real
          t11_imag=sin3672*t3_imag-t4_imag
          c(j)=ai_real+t5_real
          d(j)=ai_imag+t5_imag
E 2
I 2
          aiereal=a(iie)
          aieimag=b(iie)
          t1real=aibreal+aiereal
          t1imag=aibimag+aieimag
          t2real=aicreal+aidreal
          t2imag=aicimag+aidimag
          t3real=sin72*(aibreal-aiereal)
          t3imag=sin72*(aibimag-aieimag)
          t4real=sin72*(aicreal-aidreal)
          t4imag=sin72*(aicimag-aidimag)
          t5real=t1real+t2real
          t5imag=t1imag+t2imag
          t6real=sq54*(t1real-t2real)
          t6imag=sq54*(t1imag-t2imag)
          t7real=aireal-t5real*0.25
          t7imag=aiimag-t5imag*0.25
          t8real=t7real+t6real
          t8imag=t7imag+t6imag
          t9real=t7real-t6real
          t9imag=t7imag-t6imag
          t10real=t3real+sin3672*t4real
          t10imag=t3imag+sin3672*t4imag
          t11real=sin3672*t3real-t4real
          t11imag=sin3672*t3imag-t4imag
          c(j)=aireal+t5real
          d(j)=aiimag+t5imag
E 2
          jjb=j+jb
D 2
          c(jjb)=t8_real-t10_imag
          d(jjb)=t8_imag+t10_real
E 2
I 2
          c(jjb)=t8real-t10imag
          d(jjb)=t8imag+t10real
E 2
          jjc=j+jc
D 2
          c(jjc)=t9_real-t11_imag
          d(jjc)=t9_imag+t11_real
E 2
I 2
          c(jjc)=t9real-t11imag
          d(jjc)=t9imag+t11real
E 2
          jjd=j+jd
D 2
          c(jjd)=t9_real+t11_imag
          d(jjd)=t9_imag-t11_real
E 2
I 2
          c(jjd)=t9real+t11imag
          d(jjd)=t9imag-t11real
E 2
          jje=j+je
D 2
          c(jje)=t8_real+t10_imag
          d(jje)=t8_imag-t10_real
E 2
I 2
          c(jje)=t8real+t10imag
          d(jje)=t8imag-t10real
E 2
          i=i+iskip
          j=j+iskip1
25        continue
         j=j+jump
        do 26 k=la,m-la,la
         k2=k+k
         kk=k2+1
D 2
         tr1_real=trigs(kk)
         tr1_imag=trigs(kk+1)
E 2
I 2
         tr1real=trigs(kk)
         tr1imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr2_real=trigs(kk)
         tr2_imag=trigs(kk+1)
E 2
I 2
         tr2real=trigs(kk)
         tr2imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr3_real=trigs(kk)
         tr3_imag=trigs(kk+1)
E 2
I 2
         tr3real=trigs(kk)
         tr3imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr4_real=trigs(kk)
         tr4_imag=trigs(kk+1)
E 2
I 2
         tr4real=trigs(kk)
         tr4imag=trigs(kk+1)
E 2
         do 27 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
E 2
          iic=i+ic
D 2
          aic_real=a(iic)
          aic_imag=b(iic)
E 2
I 2
          aicreal=a(iic)
          aicimag=b(iic)
E 2
          iid=i+id
D 2
          aid_real=a(iid)
          aid_imag=b(iid)
E 2
I 2
          aidreal=a(iid)
          aidimag=b(iid)
E 2
          iie=i+ie
D 2
          aie_real=a(iie)
          aie_imag=b(iie)
          t1_real=aib_real+aie_real
          t1_imag=aib_imag+aie_imag
          t2_real=aic_real+aid_real
          t2_imag=aic_imag+aid_imag
          t3_real=sin72*(aib_real-aie_real)
          t3_imag=sin72*(aib_imag-aie_imag)
          t4_real=sin72*(aic_real-aid_real)
          t4_imag=sin72*(aic_imag-aid_imag)
          t5_real=t1_real+t2_real
          t5_imag=t1_imag+t2_imag
          t6_real=sq54*(t1_real-t2_real)
          t6_imag=sq54*(t1_imag-t2_imag)
          t7_real=ai_real-t5_real*0.25
          t7_imag=ai_imag-t5_imag*0.25
          t8_real=t7_real+t6_real
          t8_imag=t7_imag+t6_imag
          t9_real=t7_real-t6_real
          t9_imag=t7_imag-t6_imag
          t10_real=t3_real+sin3672*t4_real
          t10_imag=t3_imag+sin3672*t4_imag
          t11_real=sin3672*t3_real-t4_real
          t11_imag=sin3672*t3_imag-t4_imag
          c(j)=ai_real+t5_real
          d(j)=ai_imag+t5_imag
          x1_real=t8_real-t10_imag
          x1_imag=t8_imag+t10_real
          x2_real=t9_real-t11_imag
          x2_imag=t9_imag+t11_real
          x3_real=t9_real+t11_imag
          x3_imag=t9_imag-t11_real
          x4_real=t8_real+t10_imag
          x4_imag=t8_imag-t10_real
E 2
I 2
          aiereal=a(iie)
          aieimag=b(iie)
          t1real=aibreal+aiereal
          t1imag=aibimag+aieimag
          t2real=aicreal+aidreal
          t2imag=aicimag+aidimag
          t3real=sin72*(aibreal-aiereal)
          t3imag=sin72*(aibimag-aieimag)
          t4real=sin72*(aicreal-aidreal)
          t4imag=sin72*(aicimag-aidimag)
          t5real=t1real+t2real
          t5imag=t1imag+t2imag
          t6real=sq54*(t1real-t2real)
          t6imag=sq54*(t1imag-t2imag)
          t7real=aireal-t5real*0.25
          t7imag=aiimag-t5imag*0.25
          t8real=t7real+t6real
          t8imag=t7imag+t6imag
          t9real=t7real-t6real
          t9imag=t7imag-t6imag
          t10real=t3real+sin3672*t4real
          t10imag=t3imag+sin3672*t4imag
          t11real=sin3672*t3real-t4real
          t11imag=sin3672*t3imag-t4imag
          c(j)=aireal+t5real
          d(j)=aiimag+t5imag
          x1real=t8real-t10imag
          x1imag=t8imag+t10real
          x2real=t9real-t11imag
          x2imag=t9imag+t11real
          x3real=t9real+t11imag
          x3imag=t9imag-t11real
          x4real=t8real+t10imag
          x4imag=t8imag-t10real
E 2
          jjb=j+jb
          jjc=j+jc
          jjd=j+jd
          jje=j+je
D 2
          c(jjb)=tr1_real*x1_real-tr1_imag*x1_imag
          d(jjb)=tr1_imag*x1_real+tr1_real*x1_imag
          c(jjc)=tr2_real*x2_real-tr2_imag*x2_imag
          d(jjc)=tr2_imag*x2_real+tr2_real*x2_imag
          c(jjd)=tr3_real*x3_real-tr3_imag*x3_imag
          d(jjd)=tr3_imag*x3_real+tr3_real*x3_imag
          c(jje)=tr4_real*x4_real-tr4_imag*x4_imag
          d(jje)=tr4_imag*x4_real+tr4_real*x4_imag
E 2
I 2
          c(jjb)=tr1real*x1real-tr1imag*x1imag
          d(jjb)=tr1imag*x1real+tr1real*x1imag
          c(jjc)=tr2real*x2real-tr2imag*x2imag
          d(jjc)=tr2imag*x2real+tr2real*x2imag
          c(jjd)=tr3real*x3real-tr3imag*x3imag
          d(jjd)=tr3imag*x3real+tr3real*x3imag
          c(jje)=tr4real*x4real-tr4imag*x4imag
          d(jje)=tr4imag*x4real+tr4real*x4imag
E 2
          i=i+iskip
          j=j+iskip1
27        continue
         j=j+jump
26      continue
       else
         do 28 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
          iic=i+ic
          iid=i+id
          iie=i+ie
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
          aic_real=a(iic)
          aic_imag=b(iic)
          aid_real=a(iid)
          aid_imag=b(iid)
          aie_real=a(iie)
          aie_imag=b(iie)
          t1_real=aib_real+aie_real
          t1_imag=aib_imag+aie_imag
          t2_real=aic_real+aid_real
          t2_imag=aic_imag+aid_imag
          t3_real=sin72*(aib_real-aie_real)
          t3_imag=sin72*(aib_imag-aie_imag)
          t4_real=sin72*(aic_real-aid_real)
          t4_imag=sin72*(aic_imag-aid_imag)
          t5_real=t1_real+t2_real
          t5_imag=t1_imag+t2_imag
          t6_real=sq54*(t1_real-t2_real)
          t6_imag=sq54*(t1_imag-t2_imag)
          t7_real=ai_real-t5_real*0.25
          t7_imag=ai_imag-t5_imag*0.25
          t8_real=t7_real+t6_real
          t8_imag=t7_imag+t6_imag
          t9_real=t7_real-t6_real
          t9_imag=t7_imag-t6_imag
          t10_real=t3_real+sin3672*t4_real
          t10_imag=t3_imag+sin3672*t4_imag
          t11_real=sin3672*t3_real-t4_real
          t11_imag=sin3672*t3_imag-t4_imag
          c(j)=ai_real+t5_real
          d(j)=ai_imag+t5_imag
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
          aicreal=a(iic)
          aicimag=b(iic)
          aidreal=a(iid)
          aidimag=b(iid)
          aiereal=a(iie)
          aieimag=b(iie)
          t1real=aibreal+aiereal
          t1imag=aibimag+aieimag
          t2real=aicreal+aidreal
          t2imag=aicimag+aidimag
          t3real=sin72*(aibreal-aiereal)
          t3imag=sin72*(aibimag-aieimag)
          t4real=sin72*(aicreal-aidreal)
          t4imag=sin72*(aicimag-aidimag)
          t5real=t1real+t2real
          t5imag=t1imag+t2imag
          t6real=sq54*(t1real-t2real)
          t6imag=sq54*(t1imag-t2imag)
          t7real=aireal-t5real*0.25
          t7imag=aiimag-t5imag*0.25
          t8real=t7real+t6real
          t8imag=t7imag+t6imag
          t9real=t7real-t6real
          t9imag=t7imag-t6imag
          t10real=t3real+sin3672*t4real
          t10imag=t3imag+sin3672*t4imag
          t11real=sin3672*t3real-t4real
          t11imag=sin3672*t3imag-t4imag
          c(j)=aireal+t5real
          d(j)=aiimag+t5imag
E 2
          jjb=j+jb
          jjc=j+jc
          jjd=j+jd
          jje=j+je
D 2
          c(jjb)=t8_real+t10_imag
          d(jjb)=t8_imag-t10_real
          c(jjc)=t9_real+t11_imag
          d(jjc)=t9_imag-t11_real
          c(jjd)=t9_real-t11_imag
          d(jjd)=t9_imag+t11_real
          c(jje)=t8_real-t10_imag
          d(jje)=t8_imag+t10_real
E 2
I 2
          c(jjb)=t8real+t10imag
          d(jjb)=t8imag-t10real
          c(jjc)=t9real+t11imag
          d(jjc)=t9imag-t11real
          c(jjd)=t9real-t11imag
          d(jjd)=t9imag+t11real
          c(jje)=t8real-t10imag
          d(jje)=t8imag+t10real
E 2
          i=i+iskip
          j=j+iskip1
28        continue
         j=j+jump
        do 29 k=la,m-la,la
         k2=k+k
         kk=k2+1
D 2
         tr1_real=trigs(kk)
         tr1_imag=-trigs(kk+1)
E 2
I 2
         tr1real=trigs(kk)
         tr1imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr2_real=trigs(kk)
         tr2_imag=-trigs(kk+1)
E 2
I 2
         tr2real=trigs(kk)
         tr2imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr3_real=trigs(kk)
         tr3_imag=-trigs(kk+1)
E 2
I 2
         tr3real=trigs(kk)
         tr3imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr4_real=trigs(kk)
         tr4_imag=-trigs(kk+1)
E 2
I 2
         tr4real=trigs(kk)
         tr4imag=-trigs(kk+1)
E 2
         do 30 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
          iic=i+ic
          iid=i+id
          iie=i+ie
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
          aic_real=a(iic)
          aic_imag=b(iic)
          aid_real=a(iid)
          aid_imag=b(iid)
          aie_real=a(iie)
          aie_imag=b(iie)
          t1_real=aib_real+aie_real
          t1_imag=aib_imag+aie_imag
          t2_real=aic_real+aid_real
          t2_imag=aic_imag+aid_imag
          t3_real=sin72*(aib_real-aie_real)
          t3_imag=sin72*(aib_imag-aie_imag)
          t4_real=sin72*(aic_real-aid_real)
          t4_imag=sin72*(aic_imag-aid_imag)
          t5_real=t1_real+t2_real
          t5_imag=t1_imag+t2_imag
          t6_real=sq54*(t1_real-t2_real)
          t6_imag=sq54*(t1_imag-t2_imag)
          t7_real=ai_real-t5_real*0.25
          t7_imag=ai_imag-t5_imag*0.25
          t8_real=t7_real+t6_real
          t8_imag=t7_imag+t6_imag
          t9_real=t7_real-t6_real
          t9_imag=t7_imag-t6_imag
          t10_real=t3_real+sin3672*t4_real
          t10_imag=t3_imag+sin3672*t4_imag
          t11_real=sin3672*t3_real-t4_real
          t11_imag=sin3672*t3_imag-t4_imag
          c(j)=ai_real+t5_real
          d(j)=ai_imag+t5_imag
          x1_real=t8_real+t10_imag
          x1_imag=t8_imag-t10_real
          x2_real=t9_real+t11_imag
          x2_imag=t9_imag-t11_real
          x3_real=t9_real-t11_imag
          x3_imag=t9_imag+t11_real
          x4_real=t8_real-t10_imag
          x4_imag=t8_imag+t10_real
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
          aicreal=a(iic)
          aicimag=b(iic)
          aidreal=a(iid)
          aidimag=b(iid)
          aiereal=a(iie)
          aieimag=b(iie)
          t1real=aibreal+aiereal
          t1imag=aibimag+aieimag
          t2real=aicreal+aidreal
          t2imag=aicimag+aidimag
          t3real=sin72*(aibreal-aiereal)
          t3imag=sin72*(aibimag-aieimag)
          t4real=sin72*(aicreal-aidreal)
          t4imag=sin72*(aicimag-aidimag)
          t5real=t1real+t2real
          t5imag=t1imag+t2imag
          t6real=sq54*(t1real-t2real)
          t6imag=sq54*(t1imag-t2imag)
          t7real=aireal-t5real*0.25
          t7imag=aiimag-t5imag*0.25
          t8real=t7real+t6real
          t8imag=t7imag+t6imag
          t9real=t7real-t6real
          t9imag=t7imag-t6imag
          t10real=t3real+sin3672*t4real
          t10imag=t3imag+sin3672*t4imag
          t11real=sin3672*t3real-t4real
          t11imag=sin3672*t3imag-t4imag
          c(j)=aireal+t5real
          d(j)=aiimag+t5imag
          x1real=t8real+t10imag
          x1imag=t8imag-t10real
          x2real=t9real+t11imag
          x2imag=t9imag-t11real
          x3real=t9real-t11imag
          x3imag=t9imag+t11real
          x4real=t8real-t10imag
          x4imag=t8imag+t10real
E 2
          jjb=j+jb
          jjc=j+jc
          jjd=j+jd
          jje=j+je
D 2
          c(jjb)=tr1_real*x1_real-tr1_imag*x1_imag
          d(jjb)=tr1_imag*x1_real+tr1_real*x1_imag
          c(jjc)=tr2_real*x2_real-tr2_imag*x2_imag
          d(jjc)=tr2_imag*x2_real+tr2_real*x2_imag
          c(jjd)=tr3_real*x3_real-tr3_imag*x3_imag
          d(jjd)=tr3_imag*x3_real+tr3_real*x3_imag
          c(jje)=tr4_real*x4_real-tr4_imag*x4_imag
          d(jje)=tr4_imag*x4_real+tr4_real*x4_imag
E 2
I 2
          c(jjb)=tr1real*x1real-tr1imag*x1imag
          d(jjb)=tr1imag*x1real+tr1real*x1imag
          c(jjc)=tr2real*x2real-tr2imag*x2imag
          d(jjc)=tr2imag*x2real+tr2real*x2imag
          c(jjd)=tr3real*x3real-tr3imag*x3imag
          d(jjd)=tr3imag*x3real+tr3real*x3imag
          c(jje)=tr4real*x4real-tr4imag*x4imag
          d(jje)=tr4imag*x4real+tr4real*x4imag
E 2
          i=i+iskip
          j=j+iskip1
30        continue
         j=j+jump
29      continue
       endif
       return
600       continue
c ------------  factor 6 ---------------------------
       ic=ib+mskip
       id=ic+mskip
       ie=id+mskip
       ig=ie+mskip
       jc=jb+laskip
       jd=jc+laskip
       je=jd+laskip
       jg=je+laskip
        if(isign.gt.0) then
         do 31 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
          iic=i+ic
          iid=i+id
          iie=i+ie
          iig=i+ig
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
          aic_real=a(iic)
          aic_imag=b(iic)
          aid_real=a(iid)
          aid_imag=b(iid)
          aie_real=a(iie)
          aie_imag=b(iie)
          aig_real=a(iig)
          aig_imag=b(iig)
          t1_real=aic_real+aie_real
          t1_imag=aic_imag+aie_imag
          t2_real=ai_real-0.5*t1_real
          t2_imag=ai_imag-0.5*t1_imag
          t3_real=sin60*(aic_real-aie_real)
          t3_imag=sin60*(aic_imag-aie_imag)
          y0_real=ai_real+t1_real
          y0_imag=ai_imag+t1_imag
          y4_real=t2_real-t3_imag
          y4_imag=t2_imag+t3_real
          y2_real=t2_real+t3_imag
          y2_imag=t2_imag-t3_real
          t1_real=aig_real+aib_real
          t1_imag=aig_imag+aib_imag
          t2_real=aid_real-0.5*t1_real
          t2_imag=aid_imag-0.5*t1_imag
          t3_real=sin60*(aig_real-aib_real)
          t3_imag=sin60*(aig_imag-aib_imag)
          y3_real=aid_real+t1_real
          y3_imag=aid_imag+t1_imag
          y1_real=t2_real-t3_imag
          y1_imag=t2_imag+t3_real
          y5_real=t2_real+t3_imag
          y5_imag=t2_imag-t3_real
          x0_real=y0_real+y3_real
          x0_imag=y0_imag+y3_imag
          x4_real=y4_real+y1_real
          x4_imag=y4_imag+y1_imag
          x2_real=y2_real+y5_real
          x2_imag=y2_imag+y5_imag
          x3_real=y0_real-y3_real
          x3_imag=y0_imag-y3_imag
          x1_real=y4_real-y1_real
          x1_imag=y4_imag-y1_imag
          x5_real=y2_real-y5_real
          x5_imag=y2_imag-y5_imag
          c(j)=x0_real
          d(j)=x0_imag
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
          aicreal=a(iic)
          aicimag=b(iic)
          aidreal=a(iid)
          aidimag=b(iid)
          aiereal=a(iie)
          aieimag=b(iie)
          aigreal=a(iig)
          aigimag=b(iig)
          t1real=aicreal+aiereal
          t1imag=aicimag+aieimag
          t2real=aireal-0.5*t1real
          t2imag=aiimag-0.5*t1imag
          t3real=sin60*(aicreal-aiereal)
          t3imag=sin60*(aicimag-aieimag)
          y0real=aireal+t1real
          y0imag=aiimag+t1imag
          y4real=t2real-t3imag
          y4imag=t2imag+t3real
          y2real=t2real+t3imag
          y2imag=t2imag-t3real
          t1real=aigreal+aibreal
          t1imag=aigimag+aibimag
          t2real=aidreal-0.5*t1real
          t2imag=aidimag-0.5*t1imag
          t3real=sin60*(aigreal-aibreal)
          t3imag=sin60*(aigimag-aibimag)
          y3real=aidreal+t1real
          y3imag=aidimag+t1imag
          y1real=t2real-t3imag
          y1imag=t2imag+t3real
          y5real=t2real+t3imag
          y5imag=t2imag-t3real
          x0real=y0real+y3real
          x0imag=y0imag+y3imag
          x4real=y4real+y1real
          x4imag=y4imag+y1imag
          x2real=y2real+y5real
          x2imag=y2imag+y5imag
          x3real=y0real-y3real
          x3imag=y0imag-y3imag
          x1real=y4real-y1real
          x1imag=y4imag-y1imag
          x5real=y2real-y5real
          x5imag=y2imag-y5imag
          c(j)=x0real
          d(j)=x0imag
E 2
          jjb=j+jb
          jjc=j+jc
          jjd=j+jd
             jje=j+je
          jjg=j+jg
D 2
          c(jjb)=x1_real
          d(jjb)=x1_imag
          c(jjc)=x2_real
          d(jjc)=x2_imag
          c(jjd)=x3_real
          d(jjd)=x3_imag
          c(jje)=x4_real
          d(jje)=x4_imag
          c(jjg)=x5_real
          d(jjg)=x5_imag
E 2
I 2
          c(jjb)=x1real
          d(jjb)=x1imag
          c(jjc)=x2real
          d(jjc)=x2imag
          c(jjd)=x3real
          d(jjd)=x3imag
          c(jje)=x4real
          d(jje)=x4imag
          c(jjg)=x5real
          d(jjg)=x5imag
E 2
          i=i+iskip
          j=j+iskip1
31        continue
         j=j+jump
        do 32 k=la,m-la,la
         k2=k+k
         kk=k2+1
D 2
         tr1_real=trigs(kk)
         tr1_imag=trigs(kk+1)
E 2
I 2
         tr1real=trigs(kk)
         tr1imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr2_real=trigs(kk)
         tr2_imag=trigs(kk+1)
E 2
I 2
         tr2real=trigs(kk)
         tr2imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr3_real=trigs(kk)
         tr3_imag=trigs(kk+1)
E 2
I 2
         tr3real=trigs(kk)
         tr3imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr4_real=trigs(kk)
         tr4_imag=trigs(kk+1)
E 2
I 2
         tr4real=trigs(kk)
         tr4imag=trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr5_real=trigs(kk)
         tr5_imag=trigs(kk+1)
E 2
I 2
         tr5real=trigs(kk)
         tr5imag=trigs(kk+1)
E 2
         do 33 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
          iic=i+ic
          iid=i+id
          iie=i+ie
          iig=i+ig
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
          aic_real=a(iic)
          aic_imag=b(iic)
          aid_real=a(iid)
          aid_imag=b(iid)
          aie_real=a(iie)
          aie_imag=b(iie)
          aig_real=a(iig)
          aig_imag=b(iig)
          t1_real=aic_real+aie_real
          t1_imag=aic_imag+aie_imag
          t2_real=ai_real-0.5*t1_real
          t2_imag=ai_imag-0.5*t1_imag
          t3_real=sin60*(aic_real-aie_real)
          t3_imag=sin60*(aic_imag-aie_imag)
          y0_real=ai_real+t1_real
          y0_imag=ai_imag+t1_imag
          y4_real=t2_real-t3_imag
          y4_imag=t2_imag+t3_real
          y2_real=t2_real+t3_imag
          y2_imag=t2_imag-t3_real
          t1_real=aig_real+aib_real
          t1_imag=aig_imag+aib_imag
          t2_real=aid_real-0.5*t1_real
          t2_imag=aid_imag-0.5*t1_imag
          t3_real=sin60*(aig_real-aib_real)
          t3_imag=sin60*(aig_imag-aib_imag)
          y3_real=aid_real+t1_real
          y3_imag=aid_imag+t1_imag
          y1_real=t2_real-t3_imag
          y1_imag=t2_imag+t3_real
          y5_real=t2_real+t3_imag
          y5_imag=t2_imag-t3_real
          x0_real=y0_real+y3_real
          x0_imag=y0_imag+y3_imag
          x4_real=y4_real+y1_real
          x4_imag=y4_imag+y1_imag
          x2_real=y2_real+y5_real
          x2_imag=y2_imag+y5_imag
          x3_real=y0_real-y3_real
          x3_imag=y0_imag-y3_imag
          x1_real=y4_real-y1_real
          x1_imag=y4_imag-y1_imag
          x5_real=y2_real-y5_real
          x5_imag=y2_imag-y5_imag
          c(j)=x0_real
          d(j)=x0_imag
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
          aicreal=a(iic)
          aicimag=b(iic)
          aidreal=a(iid)
          aidimag=b(iid)
          aiereal=a(iie)
          aieimag=b(iie)
          aigreal=a(iig)
          aigimag=b(iig)
          t1real=aicreal+aiereal
          t1imag=aicimag+aieimag
          t2real=aireal-0.5*t1real
          t2imag=aiimag-0.5*t1imag
          t3real=sin60*(aicreal-aiereal)
          t3imag=sin60*(aicimag-aieimag)
          y0real=aireal+t1real
          y0imag=aiimag+t1imag
          y4real=t2real-t3imag
          y4imag=t2imag+t3real
          y2real=t2real+t3imag
          y2imag=t2imag-t3real
          t1real=aigreal+aibreal
          t1imag=aigimag+aibimag
          t2real=aidreal-0.5*t1real
          t2imag=aidimag-0.5*t1imag
          t3real=sin60*(aigreal-aibreal)
          t3imag=sin60*(aigimag-aibimag)
          y3real=aidreal+t1real
          y3imag=aidimag+t1imag
          y1real=t2real-t3imag
          y1imag=t2imag+t3real
          y5real=t2real+t3imag
          y5imag=t2imag-t3real
          x0real=y0real+y3real
          x0imag=y0imag+y3imag
          x4real=y4real+y1real
          x4imag=y4imag+y1imag
          x2real=y2real+y5real
          x2imag=y2imag+y5imag
          x3real=y0real-y3real
          x3imag=y0imag-y3imag
          x1real=y4real-y1real
          x1imag=y4imag-y1imag
          x5real=y2real-y5real
          x5imag=y2imag-y5imag
          c(j)=x0real
          d(j)=x0imag
E 2
          jjb=j+jb
          jjc=j+jc
          jjd=j+jd
             jje=j+je
          jjg=j+jg
D 2
          c(jjb)=tr1_real*x1_real-tr1_imag*x1_imag
          d(jjb)=tr1_imag*x1_real+tr1_real*x1_imag
          c(jjc)=tr2_real*x2_real-tr2_imag*x2_imag
          d(jjc)=tr2_imag*x2_real+tr2_real*x2_imag
          c(jjd)=tr3_real*x3_real-tr3_imag*x3_imag
          d(jjd)=tr3_imag*x3_real+tr3_real*x3_imag
          c(jje)=tr4_real*x4_real-tr4_imag*x4_imag
          d(jje)=tr4_imag*x4_real+tr4_real*x4_imag
          c(jjg)=tr5_real*x5_real-tr5_imag*x5_imag
          d(jjg)=tr5_imag*x5_real+tr5_real*x5_imag
E 2
I 2
          c(jjb)=tr1real*x1real-tr1imag*x1imag
          d(jjb)=tr1imag*x1real+tr1real*x1imag
          c(jjc)=tr2real*x2real-tr2imag*x2imag
          d(jjc)=tr2imag*x2real+tr2real*x2imag
          c(jjd)=tr3real*x3real-tr3imag*x3imag
          d(jjd)=tr3imag*x3real+tr3real*x3imag
          c(jje)=tr4real*x4real-tr4imag*x4imag
          d(jje)=tr4imag*x4real+tr4real*x4imag
          c(jjg)=tr5real*x5real-tr5imag*x5imag
          d(jjg)=tr5imag*x5real+tr5real*x5imag
E 2
          i=i+iskip
          j=j+iskip1
33        continue
         j=j+jump
32      continue
       else
         do 34 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
          iic=i+ic
          iid=i+id
          iie=i+ie
          iig=i+ig
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
          aic_real=a(iic)
          aic_imag=b(iic)
          aid_real=a(iid)
          aid_imag=b(iid)
          aie_real=a(iie)
          aie_imag=b(iie)
          aig_real=a(iig)
          aig_imag=b(iig)
          t1_real=aic_real+aie_real
          t1_imag=aic_imag+aie_imag
          t2_real=ai_real-0.5*t1_real
          t2_imag=ai_imag-0.5*t1_imag
          t3_real=sin60*(aic_real-aie_real)
          t3_imag=sin60*(aic_imag-aie_imag)
          y0_real=ai_real+t1_real
          y0_imag=ai_imag+t1_imag
          y4_real=t2_real+t3_imag
          y4_imag=t2_imag-t3_real
          y2_real=t2_real-t3_imag
          y2_imag=t2_imag+t3_real
          t1_real=aig_real+aib_real
          t1_imag=aig_imag+aib_imag
          t2_real=aid_real-0.5*t1_real
          t2_imag=aid_imag-0.5*t1_imag
          t3_real=sin60*(aig_real-aib_real)
          t3_imag=sin60*(aig_imag-aib_imag)
          y3_real=aid_real+t1_real
          y3_imag=aid_imag+t1_imag
          y1_real=t2_real+t3_imag
          y1_imag=t2_imag-t3_real
          y5_real=t2_real-t3_imag
          y5_imag=t2_imag+t3_real
          x0_real=y0_real+y3_real
          x0_imag=y0_imag+y3_imag
          x4_real=y4_real+y1_real
          x4_imag=y4_imag+y1_imag
          x2_real=y2_real+y5_real
          x2_imag=y2_imag+y5_imag
          x3_real=y0_real-y3_real
          x3_imag=y0_imag-y3_imag
          x1_real=y4_real-y1_real
          x1_imag=y4_imag-y1_imag
          x5_real=y2_real-y5_real
          x5_imag=y2_imag-y5_imag
          c(j)=x0_real
          d(j)=x0_imag
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
          aicreal=a(iic)
          aicimag=b(iic)
          aidreal=a(iid)
          aidimag=b(iid)
          aiereal=a(iie)
          aieimag=b(iie)
          aigreal=a(iig)
          aigimag=b(iig)
          t1real=aicreal+aiereal
          t1imag=aicimag+aieimag
          t2real=aireal-0.5*t1real
          t2imag=aiimag-0.5*t1imag
          t3real=sin60*(aicreal-aiereal)
          t3imag=sin60*(aicimag-aieimag)
          y0real=aireal+t1real
          y0imag=aiimag+t1imag
          y4real=t2real+t3imag
          y4imag=t2imag-t3real
          y2real=t2real-t3imag
          y2imag=t2imag+t3real
          t1real=aigreal+aibreal
          t1imag=aigimag+aibimag
          t2real=aidreal-0.5*t1real
          t2imag=aidimag-0.5*t1imag
          t3real=sin60*(aigreal-aibreal)
          t3imag=sin60*(aigimag-aibimag)
          y3real=aidreal+t1real
          y3imag=aidimag+t1imag
          y1real=t2real+t3imag
          y1imag=t2imag-t3real
          y5real=t2real-t3imag
          y5imag=t2imag+t3real
          x0real=y0real+y3real
          x0imag=y0imag+y3imag
          x4real=y4real+y1real
          x4imag=y4imag+y1imag
          x2real=y2real+y5real
          x2imag=y2imag+y5imag
          x3real=y0real-y3real
          x3imag=y0imag-y3imag
          x1real=y4real-y1real
          x1imag=y4imag-y1imag
          x5real=y2real-y5real
          x5imag=y2imag-y5imag
          c(j)=x0real
          d(j)=x0imag
E 2
          jjb=j+jb
          jjc=j+jc
          jjd=j+jd
             jje=j+je
          jjg=j+jg
D 2
          c(jjb)=x1_real
          d(jjb)=x1_imag
          c(jjc)=x2_real
          d(jjc)=x2_imag
          c(jjd)=x3_real
          d(jjd)=x3_imag
          c(jje)=x4_real
          d(jje)=x4_imag
          c(jjg)=x5_real
          d(jjg)=x5_imag
E 2
I 2
          c(jjb)=x1real
          d(jjb)=x1imag
          c(jjc)=x2real
          d(jjc)=x2imag
          c(jjd)=x3real
          d(jjd)=x3imag
          c(jje)=x4real
          d(jje)=x4imag
          c(jjg)=x5real
          d(jjg)=x5imag
E 2
          i=i+iskip
          j=j+iskip1
34       continue
         j=j+jump
        do 35 k=la,m-la,la
         k2=k+k
         kk=k2+1
D 2
         tr1_real=trigs(kk)
         tr1_imag=-trigs(kk+1)
E 2
I 2
         tr1real=trigs(kk)
         tr1imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr2_real=trigs(kk)
         tr2_imag=-trigs(kk+1)
E 2
I 2
         tr2real=trigs(kk)
         tr2imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr3_real=trigs(kk)
         tr3_imag=-trigs(kk+1)
E 2
I 2
         tr3real=trigs(kk)
         tr3imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr4_real=trigs(kk)
         tr4_imag=-trigs(kk+1)
E 2
I 2
         tr4real=trigs(kk)
         tr4imag=-trigs(kk+1)
E 2
         kk=kk+k2
D 2
         tr5_real=trigs(kk)
         tr5_imag=-trigs(kk+1)
E 2
I 2
         tr5real=trigs(kk)
         tr5imag=-trigs(kk+1)
E 2
         do 36 l=1,la
D 2
          ai_real=a(i)
          ai_imag=b(i)
E 2
I 2
          aireal=a(i)
          aiimag=b(i)
E 2
          iib=i+ib
          iic=i+ic
          iid=i+id
          iie=i+ie
          iig=i+ig
D 2
          aib_real=a(iib)
          aib_imag=b(iib)
          aic_real=a(iic)
          aic_imag=b(iic)
          aid_real=a(iid)
          aid_imag=b(iid)
          aie_real=a(iie)
          aie_imag=b(iie)
          aig_real=a(iig)
          aig_imag=b(iig)
          t1_real=aic_real+aie_real
          t1_imag=aic_imag+aie_imag
          t2_real=ai_real-0.5*t1_real
          t2_imag=ai_imag-0.5*t1_imag
          t3_real=sin60*(aic_real-aie_real)
          t3_imag=sin60*(aic_imag-aie_imag)
          y0_real=ai_real+t1_real
          y0_imag=ai_imag+t1_imag
          y4_real=t2_real+t3_imag
          y4_imag=t2_imag-t3_real
          y2_real=t2_real-t3_imag
          y2_imag=t2_imag+t3_real
          t1_real=aig_real+aib_real
          t1_imag=aig_imag+aib_imag
          t2_real=aid_real-0.5*t1_real
          t2_imag=aid_imag-0.5*t1_imag
          t3_real=sin60*(aig_real-aib_real)
          t3_imag=sin60*(aig_imag-aib_imag)
          y3_real=aid_real+t1_real
          y3_imag=aid_imag+t1_imag
          y1_real=t2_real+t3_imag
          y1_imag=t2_imag-t3_real
          y5_real=t2_real-t3_imag
          y5_imag=t2_imag+t3_real
          x0_real=y0_real+y3_real
          x0_imag=y0_imag+y3_imag
          x4_real=y4_real+y1_real
          x4_imag=y4_imag+y1_imag
          x2_real=y2_real+y5_real
          x2_imag=y2_imag+y5_imag
          x3_real=y0_real-y3_real
          x3_imag=y0_imag-y3_imag
          x1_real=y4_real-y1_real
          x1_imag=y4_imag-y1_imag
          x5_real=y2_real-y5_real
          x5_imag=y2_imag-y5_imag
          c(j)=x0_real
          d(j)=x0_imag
E 2
I 2
          aibreal=a(iib)
          aibimag=b(iib)
          aicreal=a(iic)
          aicimag=b(iic)
          aidreal=a(iid)
          aidimag=b(iid)
          aiereal=a(iie)
          aieimag=b(iie)
          aigreal=a(iig)
          aigimag=b(iig)
          t1real=aicreal+aiereal
          t1imag=aicimag+aieimag
          t2real=aireal-0.5*t1real
          t2imag=aiimag-0.5*t1imag
          t3real=sin60*(aicreal-aiereal)
          t3imag=sin60*(aicimag-aieimag)
          y0real=aireal+t1real
          y0imag=aiimag+t1imag
          y4real=t2real+t3imag
          y4imag=t2imag-t3real
          y2real=t2real-t3imag
          y2imag=t2imag+t3real
          t1real=aigreal+aibreal
          t1imag=aigimag+aibimag
          t2real=aidreal-0.5*t1real
          t2imag=aidimag-0.5*t1imag
          t3real=sin60*(aigreal-aibreal)
          t3imag=sin60*(aigimag-aibimag)
          y3real=aidreal+t1real
          y3imag=aidimag+t1imag
          y1real=t2real+t3imag
          y1imag=t2imag-t3real
          y5real=t2real-t3imag
          y5imag=t2imag+t3real
          x0real=y0real+y3real
          x0imag=y0imag+y3imag
          x4real=y4real+y1real
          x4imag=y4imag+y1imag
          x2real=y2real+y5real
          x2imag=y2imag+y5imag
          x3real=y0real-y3real
          x3imag=y0imag-y3imag
          x1real=y4real-y1real
          x1imag=y4imag-y1imag
          x5real=y2real-y5real
          x5imag=y2imag-y5imag
          c(j)=x0real
          d(j)=x0imag
E 2
          jjb=j+jb
          jjc=j+jc
          jjd=j+jd
             jje=j+je
          jjg=j+jg
D 2
          c(jjb)=tr1_real*x1_real-tr1_imag*x1_imag
          d(jjb)=tr1_imag*x1_real+tr1_real*x1_imag
          c(jjc)=tr2_real*x2_real-tr2_imag*x2_imag
          d(jjc)=tr2_imag*x2_real+tr2_real*x2_imag
          c(jjd)=tr3_real*x3_real-tr3_imag*x3_imag
          d(jjd)=tr3_imag*x3_real+tr3_real*x3_imag
          c(jje)=tr4_real*x4_real-tr4_imag*x4_imag
          d(jje)=tr4_imag*x4_real+tr4_real*x4_imag
          c(jjg)=tr5_real*x5_real-tr5_imag*x5_imag
          d(jjg)=tr5_imag*x5_real+tr5_real*x5_imag
E 2
I 2
          c(jjb)=tr1real*x1real-tr1imag*x1imag
          d(jjb)=tr1imag*x1real+tr1real*x1imag
          c(jjc)=tr2real*x2real-tr2imag*x2imag
          d(jjc)=tr2imag*x2real+tr2real*x2imag
          c(jjd)=tr3real*x3real-tr3imag*x3imag
          d(jjd)=tr3imag*x3real+tr3real*x3imag
          c(jje)=tr4real*x4real-tr4imag*x4imag
          d(jje)=tr4imag*x4real+tr4real*x4imag
          c(jjg)=tr5real*x5real-tr5imag*x5imag
          d(jjg)=tr5imag*x5real+tr5real*x5imag
E 2
          i=i+iskip
          j=j+iskip1
36        continue
         j=j+jump
35      continue
       endif
       return
       end
D 3
       
       RETURN
       END
E 3
E 1
