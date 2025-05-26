cccccccc subroutine vccvft computes the convolution of  
c	 two given complex functions
c
c			   nr
c          vcon(k,ic) = SUM d(i) s(k-i,ic)
c		          i=1
c	         (km <= k <= kn; 1<= ic <nc ) 
c
c		 ( 1<=kn,km<=2*nr-1 )
c via fft method
c
c
c  author:    zhiming li	           7-17-91
c
	subroutine vccvft(s,d,vcon,nr,km,kn,nc)
	parameter(lwsave=32783,ld=8196)
	integer nr,nc,km,kn
	complex s(nr,nc), d(nr)
	complex vcon(km:kn,nc)
	complex dd(ld),ss(ld)
        real wsave(lwsave) 
	nn = 2*nr-1
	if (lwsave .lt. 4*nn+15) then
	   call msgsc("lwsave in vccvft too short \0",1.)
	   stop
	end if
	scale = 1./nn
	do ir=1,nr
	   dd(ir) = d(ir) * scale
	end do
	do ir=nr+1,nn
	   dd(ir) = (0.,0.)
	end do
	call cffti(nn,wsave)
	call cfftf(nn,dd,wsave)
	do ic=1,nc
	   do ir=1,nr
	      ss(ir) = s(ir,ic) 
	   end do
	   do ir=nr+1,nn
	      ss(ir) = (0.,0.)
	   end do
	   call cfftf(nn,ss,wsave)
	   do ir=1,nn
	      ss(ir) = ss(ir) * dd(ir)
	   end do
	   call cfftb(nn,ss,wsave)
	   do k=km,kn
	      vcon(k,ic) = ss(k)
	   end do
	end do
	return
	end
