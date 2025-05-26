h47400
s 00000/00000/00024
d D 1.2 88/11/15 14:04:38 shuki 2 1
c 
e
s 00024/00000/00000
d D 1.1 88/06/15 14:26:57 shemer 1 0
c date and time created 88/06/15 14:26:57 by shemer
e
u
U
t
T
I 1
      subroutine semblan(stack1,stack2,gate,nt,sembl,i,nx)
      dimension stack1(0:nt-1),stack2(0:nt-1),sembl(0:nt-1)
      integer t0
             it=nt
             i=-1
         do 50   t0=gate/2,it-gate/2,gate/2
                  i=i+1
         sembl(i)=0.
         dn=0.
         rn=0.
         do 60   ig=t0-gate/2,t0+gate/2-1
              dn=dn+stack2(ig)
              rn=rn+stack1(ig)*stack1(ig)
   60      continue
           if (dn.eq.0.) then
           sembl(i)=0.
           go to 50
           end if
           sembl(i)=rn/(nx*dn)
   50      continue
           end


      
E 1
