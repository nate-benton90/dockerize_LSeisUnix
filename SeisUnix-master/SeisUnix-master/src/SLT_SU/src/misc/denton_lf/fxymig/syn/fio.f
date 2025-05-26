			parameter(nx=100,ny=50,nz=40)
			real v(nx,ny)

            ireclv = nx * ny * 4
			ivdisk = 30
			open(ivdisk,file="/f0/data/stgpzli/vtest.data",
     1 			form="unformatted",iostat=ios,
     2          status="old",access="direct",recl=ireclv)
            if(ios.ne.0) write(*,*) "ios=",ios
			close(ivdisk)
			open(ivdisk,file="/f0/data/stgpzli/vtest.data",
     1 			form="unformatted",iostat=ios,
     2          status="unknown",access="direct",recl=ireclv)
            if(ios.ne.0) write(*,*) "ios=",ios

			do iz=1,nz
			   read(ivdisk,rec=iz) v
			   write(*,*) "iz=",iz, "  vn=",v(nx,ny)
			end do
			stop
			end
