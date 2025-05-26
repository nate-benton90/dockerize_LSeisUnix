h25885
s 00000/00000/00035
d D 1.4 88/11/15 14:04:16 shuki 4 3
c 
e
s 00002/00001/00033
d D 1.3 88/05/08 17:29:17 valer 3 2
c 
e
s 00001/00001/00033
d D 1.2 88/05/05 17:54:34 valer 2 1
c 
e
s 00034/00000/00000
d D 1.1 88/05/05 16:20:13 valer 1 0
c date and time created 88/05/05 16:20:13 by valer
e
u
U
t
T
I 1
      SUBROUTINE CROSCOR(lvec,vec1,vec2,lwind,lcr,cr)
C
C     DESTINATION:
C        CROSSCORRELATION vec1 with vec2: 
C
C                       lwind 
C                CR(icr)=SUM[VEC1(i)*VEC2(i+icr)]
C                        i=1
C     
C     ENTRANCE:
D 3
C        vec1,vec2     two source vectors;
E 3
I 3
C        vec1,vec2     two source vectors
C                      (may be coincident - for autocorrelation); 
E 3
C        lvec          length of vec1,vec2;
C        lwind         window length for crosscorrelation computation;
C        lcr           crosscorrelation length;
C
C     RESULT:
D 2
C        lcr           crosscorrelation function;
E 2
I 2
C        cr            crosscorrelation function;
E 2
C
C     NOTE!            On user to provide needed shift
C                       of vec1 relative vec2;
C 
C     FORTRAN                                          VALERY
C
        dimension vec1(1),vec2(1),cr(1)
        lw=lwind
        do 20 icr=1,lcr
        sum=0.0
        if ((lw+icr-1).gt.lvec) lw=lvec-icr+1
	do 10 i=1,lw
10      sum=sum+vec1(i)*vec2(i+icr-1)
        cr(icr)=sum
20      continue 
        return
        end
E 1
