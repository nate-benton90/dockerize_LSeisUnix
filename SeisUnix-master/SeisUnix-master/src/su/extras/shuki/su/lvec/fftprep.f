        subroutine fftprep(nfft,ifax,trig,iopt,ier)
c------------------------------------------------------------
C  (I*4)  nfft  - FFT length
C  (I*4)  ifax  - FFT factors. (ifax(1) = number of factors)
C  (R*4)  trig  - trigonometric functions (2*nfft)
C  (I*4)  iopt  : 1 - factors = 2,3,4,5,6 (real and complex)
C               0 - factors = 2,3,4,5,6,7,8,9,11 (pfa)
C  (I*4)  ier   : 0 - ok
C               9999 - nfft cannot be factorized 
C
C         * iopt=0 not in use !
c------------------------------------------------------------
       dimension ifc(15),ifax(1),trig(1)
****
        iopt=1
****
       call fftfac(ifc,nfac,nfft,iopt,iou)
       if(iou.eq.1)then
        ifax(1)=nfac
        do 2 j=2,nfac+1
2       ifax(j)=ifc(j-1)
        ier=0
       else
       ier=9999
       endif
       call ffttrig(nfft,trig)
       return
       end
c------------------------------------------------------
        subroutine fftfac(ifc,nfac,n,iopt,iyn)
        dimension ifc(15),ic(15)
       iyn=0
        do 11 i=1,15
11        ic(i)=0
        num=n
        icc=0
1002    continue
        if(num/2*2.eq.num)then
        icc=icc+1
        ic(2)=ic(2)+1
        ifc(icc)=2
        num=num/2
        if(num.eq.1)goto 555
        goto 1002
        endif
1003    continue
        if(num/3*3.eq.num)then
        icc=icc+1
        ic(3)=ic(3)+1
        ifc(icc)=3
        num=num/3
        if(num.eq.1)goto 555
        goto 1003
        endif
1005    continue
        if(num/5*5.eq.num)then
        icc=icc+1
        ic(5)=ic(5)+1
        ifc(icc)=5
        num=num/5
        if(num.eq.1)goto 555
        goto 1005
        endif
C***
        if(iopt.eq.1)goto 555
C***
1007    continue
        if(num/7*7.eq.num)then
        icc=icc+1
        ic(7)=ic(7)+1
        ifc(icc)=7
        num=num/7
        if(num.eq.1)goto 555
        goto 1007
        endif
1011    continue
        if(num/11*11.eq.num)then
        icc=icc+1
        ic(11)=ic(11)+1
        ifc(icc)=11
        num=num/11
        if(num.eq.1)goto 555
        goto 1011
        endif
555        continue
        if(icc.eq.0)then
        return
        endif
        nnn=1
        do 10 i=1,icc
10        nnn=nnn*ifc(i)
        if(nnn.ne.n)then
        return
        endif
C***
        if(iopt.eq.1)then
102     continue
        if(ic(2).ge.1.and.ic(3).ge.1)then
        ic(6)=ic(6)+1
        ic(2)=ic(2)-1
        ic(3)=ic(3)-1
        goto 102
        endif
103     continue
        if(ic(2).ge.2)then
        ic(4)=ic(4)+1
        ic(2)=ic(2)-2
        goto 103
        endif
        icc=0
        do 15 i=2,6
        do 16 j=1,ic(i)
        icc=icc+1
16        ifc(icc)=i
15        continue
        goto 556
        endif
C***
        if(ic(2).eq.3)then
        ic(8)=1
        ic(2)=ic(2)-3
        endif
        if(ic(2).eq.2)then
        ic(4)=1
        ic(2)=ic(2)-2
        endif
        if(ic(3).eq.2)then
        ic(9)=1
        ic(3)=ic(3)-2
        endif
        if(ic(2).eq.1.and.ic(3).eq.1)then
        ic(6)=1
        ic(2)=ic(2)-1
        ic(3)=ic(3)-1
        endif
        icc=0
        do 12 i=2,11
        if(ic(i).gt.1)then
        return
        endif
        if(ic(i).eq.0)goto 12
        icc=icc+1
        ifc(icc)=i
12        continue
556        continue
        nfac=icc
       iyn=1
        return
        end
c-----------------------------------------------
       subroutine ffttrig(nfft,trig)
       complex trig(1),cc
       dinc=2.*3.14159265/nfft
       do 1 n=1,nfft
       cc=dinc*(n-1)
       trig(n)=cexp((0.,1.)*cc)
1      continue
       return
       end
