
		real hz(201,51)

		dy = 100
		dx = 20
		yo = 2500
		xo = 2000
		nx = 201
		ny = 51

		do iy=1,ny
			y = (iy-1)*dy
			do ix=1,nx
				x = (ix-1)*dx
				dd = (x-xo)**2 + (y-yo)**2
				dd = dd/(2000*2000)
				dd = 1. - dd
				z = 450.
				if(dd.ge.0.) then 
					z = sqrt(dd*(2000*2000-1000*1000)) - 50*5
				end if
				if(z.lt.450.) z = 450
				hz(ix,iy) = z
			end do
		end do
		open(9,file="junk",form="unformatted",status="new",
     1       access="direct",recl=ny*nx*4)
		write(9,rec=1) hz	

		stop
		end

