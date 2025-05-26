ccccc
c apply 2.5-D or 3-D intergration filter 
c       input: 
c          trace         input trace of length n
c          n             length of trace in samples
c          i3d           3d flag (2=2d 3=3d)
c       output:
c          trace         output trace with sqrt(-iw) applied 
c
c       author:	zhiming li	      
c
	subroutine f2p5n (trace,n,i3d,m,l)
	parameter(nwsave=32768,nf=16384)
	real trace(n)
	real cos45
	real wsave(nwsave), sqrtf(nf) 
	real work(nwsave)
	integer n, ifirst, l, m, i3d
	real scale
	data ifirst/1/
c compute the filter
	if (ifirst.eq.1) then
	   ifirst = 999
	   m = (n+1)*3/2*2
	   call radix(m,l)
	   m = l
	   if ( 2*m+15 .gt. nwsave ) then
	      call msgscs("increase nwsave in f2p5")
	      stop
	   end if   
	   call rffti(m,wsave)
	   cos45 = 0.5 * sqrt(2.)
	   scale = (2./m)
	   if(i3d.eq.2) then
	   	do i=1,m/2+1
	      		sqrtf(i) = sqrt( (i-1)*scale ) * cos45 / m
	   	end do
	   else
	   	do i=1,m/2+1
	      		sqrtf(i) = (i-1)*scale / m
	   	end do
	   end if
	   l = (m+1)/2 
	end if	
c forward fft
	do i=1,n
		work(i) = trace(i) 
	end do
	do i=n+1,m
		work(i) = 0. 
	end do
	call rfftf(m,work,wsave)
c apply the filter
	work(1) = 0.
        if(i3d.eq.2) then
	   do k=2,l
	   	ir = 2 * k - 2 
           	ii = 2 * k - 1
	   	tr = work(ir)*sqrtf(k)
	   	ti = work(ii)*sqrtf(k)
cccc -45 degree 
ccc	   	work(ir) = tr + ti 	
ccc	   	work(ii) = ti - tr 	
cccc +45 degree
	   	work(ir) = tr - ti
	   	work(ii) = tr + ti
	   end do  
	else
	   do k=2,l
	   	ir = 2 * k - 2 
           	ii = 2 * k - 1
	   	tr = work(ii)*sqrtf(k)
	   	ti = -work(ir)*sqrtf(k)
	   	work(ir) = tr
	   	work(ii) = ti
	   end do
	end if
        work(m) = 0.
c inverse fft
	call rfftb(m,work,wsave)
	do i=1,n
		trace(i) = work(i)
	end do
	return
	end

