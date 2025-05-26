h15319
s 00000/00000/00150
d D 1.3 88/11/15 14:04:23 shuki 3 2
c 
e
s 00004/00001/00146
d D 1.2 88/05/29 09:39:42 moshe 2 1
c 
e
s 00147/00000/00000
d D 1.1 88/05/29 09:36:35 moshe 1 0
c date and time created 88/05/29 09:36:35 by moshe
e
u
U
f e 0
t
T
I 1
        subroutine nextfft(num,nfft)
c------------------------------------------------------------
C       Subroutine finds the closest FFT number (nfft) to num.
C
D 2
C  Unused options : iopt and ifax nexfft(num,nfft,ifax,iopt)
E 2
I 2
C  (I*4)  num   - input nunber
C  (I*4)  nfft  - output, FFT size
C
C  Unused options : iopt nexfft(num,nfft,iopt)
E 2
C
C       iopt  : 1 - factors = 2,3,4,5,6
C               0 - factors = 2,3,4,5,6,7,8,9,11
c------------------------------------------------------------
       dimension ifc(15),ifax(1)
****
        iopt=1
****
        do 1 i=num,num*3
       call fffac(ifc,nfac,i,iopt,iou)
       if(iou.eq.1)then
        nfft=i
        ifax(1)=nfac
        do 2 j=2,nfac+1
2       ifax(j)=ifc(j-1)
        goto 101
       endif
1       continue
101       continue
       return
       end
c------------------------------------------------------
        subroutine fffac(ifc,nfac,n,iopt,iyn)
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
E 1
