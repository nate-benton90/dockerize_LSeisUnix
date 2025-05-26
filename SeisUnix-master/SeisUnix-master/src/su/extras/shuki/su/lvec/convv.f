      SUBROUTINE CONVV(lop,ish,op,ltr,tri,tro)
C
c*****************************************************************
c	DESTINATION:
C	     CONVOLUTION TRI WITH OP.
c
c	ENTRANCE:
C	     LOP,LTR lengths of OP,TRI;
C	     ISH needed shift of rezult TRO
C         (for example: index-1 of MIDpoint of symmetrical operator 
C                       or 0 in case of no shift)
c	RESULT:
C     	   TRO of LTR-length
C
C     NOTE: TRI AND TRO CAN'T BE COINCIDENT !!!!!!
c
c	FORTRAN					VALERY
c*****************************************************************
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
