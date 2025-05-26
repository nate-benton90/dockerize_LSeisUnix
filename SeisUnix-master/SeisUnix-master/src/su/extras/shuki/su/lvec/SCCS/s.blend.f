h52098
s 00000/00000/00025
d D 1.2 88/11/15 14:04:17 shuki 2 1
c 
e
s 00025/00000/00000
d D 1.1 88/05/08 17:27:59 valer 1 0
c date and time created 88/05/08 17:27:59 by valer
e
u
U
t
T
I 1
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
E 1
