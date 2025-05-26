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
C        vec1,vec2     two source vectors
C                      (may be coincident - for autocorrelation); 
C        lvec          length of vec1,vec2;
C        lwind         window length for crosscorrelation computation;
C        lcr           crosscorrelation length;
C
C     RESULT:
C        cr            crosscorrelation function;
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
