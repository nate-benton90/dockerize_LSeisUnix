      subroutine rowcc(n1,n2,cx,sign2,scale)
      complex cx(n1,n2),cmplx,cw,cdel
      integer i1,n1, i2,n2, j,i,m,lstep,istep
      real sign2, scale, arg
      do 23000 i1 = 1,n1
      do 23002 i2 = 1,n2
      cx(i1,i2) = cx(i1,i2)*scale
23002 continue
23000 continue
      j = 1
      do 23004 i = 1,n2 
      if(.not.(i.le.j))goto 23006
      call twid1(n1,cx(1,i),cx(1,j))
23006 continue
      m = n2/2
23008 if(.not.(j.gt.m))goto 23009
      j = j-m
      m = m/2
      if(.not.(m.lt.1))goto 23010
      goto 23009
23010 continue
      goto 23008
23009 continue
      j = j+m 
23004 continue
      lstep = 1
23012 continue
      istep = 2*lstep
      cw = 1.
      arg = sign2*3.14159265/lstep
      cdel = cmplx(cos(arg),sin(arg))
      do 23015 m = 1,lstep 
      do 23017 i = m,n2,istep
      call twid2(n1,cw,cx(1,i),cx(1,i+lstep))
23017 continue
      cw = cw*cdel
23015 continue
      lstep = istep
23013 if(.not.(lstep.ge.n2))goto 23012
      return
      end
      subroutine twid1(n,cx,cy)
      complex cx(n),cy(n),ct
      integer i,n
      do 23019 i = 1,n 
      ct = cx(i)
      cx(i) = cy(i)
      cy(i) = ct 
23019 continue
      return
      end
      subroutine twid2(n,cw,cx,cy)
      complex cx(n),cy(n),ctemp,cw
      integer i,n
      do 23021 i = 1,n 
      ctemp = cw*cy(i)
      cy(i) = cx(i)-ctemp
      cx(i) = cx(i)+ctemp 
23021 continue
      return
      end
