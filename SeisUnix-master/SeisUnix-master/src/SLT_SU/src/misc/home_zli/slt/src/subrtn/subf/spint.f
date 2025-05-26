c
c   from:    numerical recipes
c
c   given arrays x and y of length n containing a tabulated function, 
c   y(i) = f(x(i)), with x(1) < x(2) < ... < x(n) and given values
c   yp1 and ypn for the first derivative of the interpolating function 
c   at points 1 and n, respectively, this routine returns an array y2 
c   of length n which contains the second derivatives of the interpolating
c   function at the tabulated points x(i).  
c 
      subroutine spline(x,y,n,yp1,ypn,y2)
      parameter (nmax=4096)
      dimension x(n),y(n),y2(n),u(nmax)
      real yp1, ypn, big
      integer n
      big = 0.99 * 10**30

      if (yp1.gt.big) then
        y2(1)=0.
        u(1)=0.
      else
        y2(1)=-0.5
        u(1)=(3./(x(2)-x(1)))*((y(2)-y(1))/(x(2)-x(1))-yp1)
      endif
      do 11 i=2,n-1
        sig=(x(i)-x(i-1))/(x(i+1)-x(i-1))
        p=sig*y2(i-1)+2.
        y2(i)=(sig-1.)/p
        u(i)=(6.*((y(i+1)-y(i))/(x(i+1)-x(i))-(y(i)-y(i-1))
     *      /(x(i)-x(i-1)))/(x(i+1)-x(i-1))-sig*u(i-1))/p
11    continue
      if (ypn.gt.big) then
        qn=0.
        un=0.
      else
        qn=0.5
        un=(3./(x(n)-x(n-1)))*(ypn-(y(n)-y(n-1))/(x(n)-x(n-1)))
      endif
      y2(n)=(un-qn*u(n-1))/(qn*y2(n-1)+1.)
      do 12 k=n-1,1,-1
        y2(k)=y2(k)*y2(k+1)+u(k)
12    continue
      return
      end

c	From numerical recipe
c   given the arrays xa and ya of length n, which tabulates  a function,
c   with x(1) < x(2) < ... < x(n), and given the array y2a (second 
c   derivatives of the function at x(i), i=1,2,..,n) computed from
c   subroutine spline, this routine returns a cubic-spline interpolated
c   function value at x
c   
ccc     z.li	changed pause(...) to msgsc call
c 
      subroutine splint(xa,ya,y2a,n,x,y)
      dimension xa(n),ya(n),y2a(n)
      klo=1
      khi=n
1     if (khi-klo.gt.1) then
        k=(khi+klo)/2
        if(xa(k).gt.x)then
          khi=k
        else
          klo=k
        endif
      goto 1
      endif
      h=xa(khi)-xa(klo)
      if (h.eq.0.) call msgsc("bad xa input.\0",h)
      a=(xa(khi)-x)/h
      b=(x-xa(klo))/h
      y=a*ya(klo)+b*ya(khi)+
     *      ((a**3-a)*y2a(klo)+(b**3-b)*y2a(khi))*(h**2)/6.
      return
      end

ccccccccccc
ccc
ccc
ccc    given (xi(i),yi(i), i=1,...,ni) data points, spint interpolates 
ccc    function y to output positions xo(i), i=1,...,no. zero slope
ccc    conditions at edges are assumed, i.e.,
ccc                    yo = yi(1),   if xo < xi(1)   and
ccc                    yo = yi(ni),   if xo > xi(ni)
ccc
ccc     input:
ccc          xi(ni)	----    real*4        tabulated data positions
ccc                                           (xi(1)<xi(2)<...<xi(ni))
ccc          yi(ni)	----    real*4        tabulated function values
ccc              ni     ----    int*4	      length of input data points
ccc          xo(no)	----    real*4        output data positions
ccc              no     ----    int*4	      length of output data points
ccc        work(ni)     ----    real*4        working array
ccc     output:
ccc          yo(no)	----    real*4        function values at xo
ccc        work(ni)     ----    real*4        second derivative of function
ccc 					      at xi 
ccc     note:  this subroutine calls NUMERICAL RECIPE's spline and splint
ccc            routines.
ccc     author:   zhiming li		2-21-92
ccc
ccc
   	subroutine spint(xi,yi,ni,xo,yo,no,work) 
	real xi(ni),yi(ni),xo(no),yo(no),work(ni)
	real x,xmin,xmax
	integer ni,no

	xmin = xi(1)
	xmax = xi(ni)

	call spline(xi,yi,ni,0.,0.,work)
	
	do i=1,no
		x = xo(i)
		if(x.lt.xmin) then
			yo(i) = yi(1)
		else if(x.gt.xmax) then
			yo(i) = yi(ni)
		else	
      			call splint(xi,yi,work,ni,x,yo(i))
		end if
	end do
	return
	end
	
