
ccccc complex to complex 2-d fft routines
ccc
ccc     names: cfft2di, cfft2df, cfft2db
ccc 	author: zhiming li		      		4/92
ccc     note:   a call to cfft2df followed by cfft2db will return data
ccc		scaled by (n1*n2) 
ccc
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
ccc
ccc    	cfft2di initializes 2-d fft 
ccc
ccc	input:
ccc		n1	---	int*4		fft length of 1st dimension 
ccc		n2	---	int*4		fft length of 2nd dimension 
ccc		nws1    ---     int*4           length of ws1 array
ccc						(should be at least 4*n1+15)
ccc		nws2    ---     int*4           length of ws2 array
ccc						(should be at least 4*n2+15)
ccc	output:
ccc		ws1	---	real*4		computed fft coefficients
ccc						along 1st dimension
ccc		ws2	---	real*4		computed fft coefficients
ccc						along 2nd dimension
ccc
ccc
	subroutine cfft2di(n1,n2,ws1,ws2,nws1,nws2)
	real ws1(nws1), ws2(nws2)
	integer n1, n2, nws1, nws2
	if(n1.gt.1) call cffti(n1,ws1)
	if(n2.gt.1) call cffti(n2,ws2)
	return
	end 
ccc
ccc    	cfft2df does forward 2-d fft 
ccc		forward fft uses kernel exp(-i*(kx*x+ky*y))
ccc	input:
ccc		n1	---	int*4		fft length of 1st dimension 
ccc		n2	---	int*4		fft length of 2nd dimension 
ccc		ws1	---	real*4		computed fft coefficients
ccc						along 1st dimension by cfft2di
ccc		ws2	---	real*4		computed fft coefficients
ccc						along 2nd dimension by cfft2di
ccc		c(n1,n2)---	cmpx*8 		data to do forward 2-d fft
ccc		work(n2)---     cmpx*8		working array
ccc		nws1    ---     int*4           length of ws1 array
ccc						(should be at least 4*n1+15)
ccc		nws2    ---     int*4           length of ws2 array
ccc						(should be at least 4*n2+15)
ccc	output:
ccc		c(n1,n2)---	cmpx*8 		2-d forward fft result
ccc
	subroutine cfft2df(n1,n2,ws1,ws2,c,work,nws1,nws2)
	real ws1(nws1), ws2(nws2)
	integer n1, n2, nws1, nws2
	complex c(n1,n2),work(n2)
	if(n1.gt.1) then
		do i2=1,n2
			call cfftf(n1,c(1,i2),ws1)
		end do
	end if
	if(n2.gt.1) then
		do i1=1,n1
			do i2=1,n2
				work(i2) = c(i1,i2)
			end do
			call cfftf(n2,work,ws2)
			do i2=1,n2
				c(i1,i2) = work(i2)
			end do
		end do
	end if
	return
	end 
ccc
ccc    	cfft2db does backward 2-d fft 
ccc		backward fft uses kernel exp(i*(kx*x+ky*y))
ccc	input:
ccc		n1	---	int*4		fft length of 1st dimension 
ccc		n2	---	int*4		fft length of 2nd dimension 
ccc		ws1	---	real*4		computed fft coefficients
ccc						along 1st dimension by cfft2di
ccc		ws2	---	real*4		computed fft coefficients
ccc						along 2nd dimension by cfft2di
ccc		c(n1,n2)---	cmpx*8 		data to do backward 2-d fft
ccc		work(n2)---     cmpx*8		working array
ccc		nws1    ---     int*4           length of ws1 array
ccc						(should be at least 4*n1+15)
ccc		nws2    ---     int*4           length of ws2 array
ccc						(should be at least 4*n2+15)
ccc	output:
ccc		c(n1,n2)---	cmpx*8 		2-d backward fft result
ccc
	subroutine cfft2db(n1,n2,ws1,ws2,c,work,nws1,nws2)
	real ws1(nws1), ws2(nws2)
	integer n1, n2, nws1, nws2
	complex c(n1,n2),work(n2)
	if (n1.gt.1) then
		do i2=1,n2
			call cfftb(n1,c(1,i2),ws1)
		end do
	end if
	if (n2.gt.1) then
		do i1=1,n1
			do i2=1,n2
				work(i2) = c(i1,i2)
			end do
			call cfftb(n2,work,ws2)
			do i2=1,n2
				c(i1,i2) = work(i2)
			end do
		end do
	end if
	return
	end
