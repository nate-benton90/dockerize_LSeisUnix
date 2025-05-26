* Stolen from Gplib--should be eliminated ASAP

*------------------------------------------------------------------
* This subroutine does predictive deconvolution:
*
* Usage:
*     call pdcon86(x,y,leng,istart,lwin,nop,ipred,pnois)
*
*     x          the input data
*     y          the output data
*     istart     starting point of designed window
*     lwin       length of the desingned window
*     nop        the order of the W-L matrix to be computed
*     ipred      prediction distance
*     pnois      additive white noise
*
* Note:
*     (1) length of autocorrelation truncation = nop + ipred
*     (2) order of W-L matrix = nop
*     (3) length of actual decon operator = nop + ipred
*------------------------------------------------------------------
*
* Author:
* Source: /src/public/gplib
* Revision: Kidane Araya 03/22/1988.
*------------------------------------------------------------------
      subroutine pdcon86(x,y,leng,istart,lwin,nop,ipred,pnois)
      dimension  x(1),y(1),w1(1250),w2(1250),w3(1250),w4(1250)
	irms =1
	ifirst=2
	imax  =3
*
*----	PARAMETER CHECK
*
	ilast=istart+lwin-1
	lauto=nop+ipred
*
	if(leng.gt.0) then
	   if(istart.gt.0.and.istart.le.leng) then
	      if(lwin.le.leng.and.ilast.le.leng) then
	         if(lauto.le.lwin) then
	            if(pnois.lt.0.0) then
	               pnois=0.0
	               write(0,*) '**    PNOIS RESET TO 0.0    **'
                    endif
*
*----	AUTOCORRELATION OF DATA
*
	            call corr85(x(istart),lwin,x(istart),lwin,w1,lauto)
*
*----	NORMALIZATION & ADDITIVE WHITE NOISE ADDITION
*
	            call norm85(w1,lauto,1,lauto,ifirst)
	            w1(1)=w1(1)*(1.0+pnois)
*
*----	MOVE AUTOCORRELATED DATA TO W2 FOR W-L ALGORITHM
*
	            call move85(w1(ipred+1),w2(1),nop)
*
*----	W-L ALGORITHM
*
	            call wlev85(w1,w2,nop,w3,w4)
*
*----	MOVE & POLARITY REVERSAL AUTOCORRELATED DATA TO WORK ARRAY
*
	            do 100 i=1,nop
	               w1(ipred+i)=-w3(i)
100	            continue
*
*----	ZERO PADDING TO W1
*
	            do 200 i=1,ipred
	               w1(i)=0.0
200	            continue
*
*----	1.0 SET AT ZERO TIME LAG
*
	            w1(1)=1.0
*
*----	CONVOLUTION OF PREDICTION OPERATOR WITH ORIGINAL TRACE
*
	            call conv85(x,leng,w1,lauto,y,leng)
*
*----	END
*
                 else
	            write(0,*) '** NOP/LWIN REALTION ERROR (PDCON86) **'
                 endif
              else
	         write(0,*) '** LWIN LENGTH ERROR (PDCON86) **'
              endif
           else
	      write(0,*) '** ISTART ERROR (PDCON86) **'
           endif
        else
	   write(0,*) '** LENG ERROR (PDCON86) **'
        endif
	return
	end

*---------------------------------------------------------------
* This subroutine finds the solution of single-channel
* normal equations which arise in least-squares filtering
* and prediction problems for single-channel time series.
*
* Usage:
*     call wlev85(r,g,lr,f,a)
*
*     r        (r(0), r(1), ..., r(n))
*              autoccorelation coeffients
*     g        (g(0), g(1), ..., g(n)
*              right-hand side coeffients
*     lr       length of filter = n + 1
*     f        (f(0), f(1), ..., f(n)
*              filter coeffients
*     a        (a(0), a(1), ..., a(n)
*              prediction error operator
*----------------------------------------------------------------
*
* Author: A. Robinson
* Source: /src/public/gplib
* Revision: Kidane Araya 03/01/1988.
*----------------------------------------------------------------

      subroutine wlev85(r,g,lr,f,a)
      dimension r(lr),g(lr),f(lr),a(lr)

      v  =r(1)
      d  =r(2)
      a(1)=1.
      f(1)=g(1)/v
      q  =f(1)*r(2)
      if(lr.eq.1) return
          do 4 l=2,lr
             a(l)=-d/v
             if(l.eq.2) goto 2
                l1  =(l-2)/2
                l2  =l1+1
             if(l2.lt.2) goto 5
                do 1 j=2,l2
                     hold=a(j)
                     k=l-j+1
                     a(j)=a(j)+a(l)*a(k)
    1                a(k)=a(k)+a(l)*hold
    5        if(2*l1.eq.l-2) goto 2
                a(l2+1)=a(l2+1)+a(l)*a(l2+1)
    2           v=v+a(l)*d
                f(l)=(g(l)-q)/v
                l3=l-1
                do 3 j=1,l3
                    k=l-j+1
    3               f(j)=f(j)+f(l)*a(k)
                if(l.eq.lr) return
                    d   =0.0
                    q   =0.0
                    do 4 i=1,l
                         k=l-i+2
                         d=d+a(i)*r(k)
    4                    q=q+f(i)*r(k)
      end

*---------------------------------------------------------------
* This subroutine performs the complete transient convolution
* of two signals.
*
* Usage:
*     call conv85(x,i,y,j,z,k)
*
*     i = length of x = m + 1
*     x = (x0, x1, ...,xm)
*     j = length of y = n + 1
*     y = (y0, y1, ...,yn)
*     k = length of z = m + n + 1
*     z = (z0, z1, ...,zm+n)
*
*_______________________________________________________________
* Author: A. Robinson
* Source: /src/public/gplib
* Revision: Kidane Araya 02/22/1988.
*---------------------------------------------------------------   

      subroutine conv85(x,i,y,j,z,k)


      real x(i), y(j), z(k)
      real*8 s8
      do 10 n=1,k
         s8=0.
         np1=n+1
         do 20 m=np1-min0(n,j),min0(n,i)
  20        s8=s8+x(m)*y(np1-m)
  10        z(n)=s8
      return
      end

*-----------------------------------------------------------
* THIS SUBROUTINE MOVES  AN ARRAY FROM ONE STORAGE
* LOCATION TO ANOTHER.
*
* Usage:
*     call move85(x,y,n)
*
*     x    array to be moved
*     y    the new array
*     n    the lenght of x and y
*     i    index increment of x and y
*-----------------------------------------------------------
*
* Author: A. Robinson
* Source: /src/public/gplib
* Revision: Kidane Araya 02/22/1988.
*-----------------------------------------------------------
*
      subroutine move85(x,y,n)
      real x(n), y(n)
      do 10 i=1,n
         y(i)=x(i)
 10   continue
      return
      end

*--------------------------------------------------------------------------
* This subroutine normalizes an array by its RMS energy, or the
* first value in the array or by the largest magnitude in the 
* Array:
*
* Usage:
*     call norm85(x,lx,is,ie,in)
*
*     x		the input array
*     lx	the length of array
*     is        the initial point to generate factor
*     ie        the last point to generate factor
*     in        operation flag
*               = 1 normalizes by the RMS energy
*               = 2 normalizes by the first value
*               = 3 normalizes by the largest magnitude
*---------------------------------------------------------------------------
*
* Author:
* Source: /src/public/gplib
* Revision: Kidane Araya 03/14/1988.
*---------------------------------------------------------------------------
      subroutine norm85(x,lx,is,ie,in)
      dimension  x(lx)
      if(lx.ge.1) then
         if(is.le.ie) then
            if(is.le.0) then
               write(0,*) '? START POINT = 0 CHANGE TO 1(WARN-NORM85)'
               is=1
            endif
            if(ie.gt.lx) then
               write(0,*) ' END POINT OVER LENGTH CHANGE TO LX (NORM85)'
               ie=lx
            endif
            if(in.eq.1) then
               p=0.0
               do 150 i=is,ie
                  p=p+x(i)**2
  150          continue
               p=sqrt(p)
               do 160 i=1,lx
                  x(i)=x(i)/p
  160          continue
            elseif(in.eq.2) then
               x1=x(is)
               if(x1.ne.0.0) then
                  do 250 i=1,lx
                     x(i)=x(i)/x1
  250             continue
               else
                  write(0,*)'***** 1-ST ELEMENT IS ZERO STOP *****'
               endif
            elseif(in.eq.3) then
               b=0.0
               do 350 i=is,ie
                  b=amax1(abs(x(i)),b)
  350          continue
               do 360 i=1,lx
                  x(i)=x(i)/b
  360          continue
            else
               write(0,*)'***** OPERATION SELECT ERROR IN =i2 (NORM85)'
            endif
         else
            write(0,*) '***** START/END POINT CONFLICTION(NORM85)'
         endif
      else
         write(0,*) '***** LENGTH ERROR LX =',lx, '(norm85) *****'
      endif
      return
      end

*-----------------------------------------------------------------

* This subroutine computes the one-sided (Bartlett truncation)
* cross correlation of two vectors:
*
* Usage:
*     call corr85(x,lx,y,ly,z,lz)
*
*     lx   length of x = m + 1
*      x   (x0, x1, ..., xm)
*     ly   length of y = n + 1
*      y   (y0, y1, ..., yn)
*     lz   desired length of cross correlation = k + 1.
*     z    (z0, z1, ..., zk)
*     im1  index increment in x
*     j    index increment in y
*     i    index increment in z
*
*----------------------------------------------------------------
*
* Author: A. Robinson
* Source: /src/public/gplib
* Revision: Kidane Araya 02/22/1988.
*----------------------------------------------------------------

      subroutine corr85(x, lx, y, ly, z, lz)
      dimension x(lx), y(ly), z(lz)


      real*8 s8
      do 200 i=1,lz
         s8=0.
         im1=i-1
         do 100 j=1,min0(ly,lx-im1)
  100       s8=s8+x(im1+j)*y(j)
  200    z(i)=s8
      return
      end
