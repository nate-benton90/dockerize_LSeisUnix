h21943
s 00000/00000/00037
d D 1.3 88/11/15 14:03:58 shuki 3 2
c 
e
s 00012/00004/00025
d D 1.2 88/05/18 11:31:34 valer 2 1
c 
e
s 00029/00000/00000
d D 1.1 88/04/14 13:49:49 shuki 1 0
c date and time created 88/04/14 13:49:49 by shuki
e
u
U
f e 0
t
T
I 1
      SUBROUTINE CONVV(lop,ish,op,ltr,tri,tro)
C
D 2
C     CONVOLUTION TRI WITH OP.
C     LOP,LTR lengths of OP,TRI;
C     ISH needed shift of rezult TRO
E 2
I 2
c*****************************************************************
c	DESTINATION:
C	     CONVOLUTION TRI WITH OP.
c
c	ENTRANCE:
C	     LOP,LTR lengths of OP,TRI;
C	     ISH needed shift of rezult TRO
E 2
C         (for example: index-1 of MIDpoint of symmetrical operator 
C                       or 0 in case of no shift)
D 2
C     RESULT in TRO of LTR-length
E 2
I 2
c	RESULT:
C     	   TRO of LTR-length
E 2
C
C     NOTE: TRI AND TRO CAN'T BE COINCIDENT !!!!!!
I 2
c
c	FORTRAN					VALERY
c*****************************************************************
E 2
C     
	dimension op(1),tri(1),tro(1)
	do 10 jtr=1,ltr
10	tro(jtr)=0.0
	mid=lop-ish
	do 20 jop=1,mid
	r=op(lop-jop+1)
	jtr1=mid+1-jop
	do 20 jtr=jtr1,ltr
20	tro(jtr)=tro(jtr)+r*tri(jtr-mid+jop)
	if (ish.eq.0) return
	mid1=mid+1
	do 30 jop=mid1,lop 
	r=op(lop-jop+1)
	jtrn=ltr+mid-jop
	do 30 jtr=1,jtrn
30	tro(jtr)=tro(jtr)+r*tri(jtr-mid+jop)
      return
      end
E 1
