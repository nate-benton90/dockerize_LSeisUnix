      SUBROUTINE BLEND(lvec,vec1,vec2,vecres)
c
c	DESTINATION:
c	   LINEAR BLENDING vec1 with vec2:
c
c		vecres(iv)=(1-iv/lvec)*vec1(iv)+(iv/lvec)*vec2(iv);
c
c	ENTRANCE:
c	   vec1,vec2     two source vectors;
c	   lvec          length of vec1,vec2;
c
c	RESULT:
c	   vecres        result vector
c                        (may be coincident with vec1 or vec2);
c
c	FORTRAN                                              VALERY
c
        dimension vec1(1),vec2(1),vecres(1)
        rl=lvec
        do 10 iv=1,lvec
           r=iv/rl 
           vecres(iv)=(1.0-r)*vec1(iv)+r*vec2(iv)
10      continue
        return
        end 
