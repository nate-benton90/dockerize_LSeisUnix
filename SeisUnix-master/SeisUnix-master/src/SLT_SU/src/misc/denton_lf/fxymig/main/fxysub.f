cccccccccccccc
c   name: wxymig ----   3-d w-x-y migration with compensating for f.d errors
c   authors:    zhiming li         4/92                                       
c   syntex:
c      	subroutine wxymig(nx,ny,ntau,nw,itau0,istep,icstep,
c     1			  nzv,nkx,nky,nq,nv,ncp,lvec,lplane,iorder,
c     2			  naux1,naux2,naux,
c     3 		  dt,dx,dy,dtau,para,vref,dzv,
c     4			  q,v,vxy,
c     5 		  om,oms,vyx,w,
c     6			  aux1,aux2,
c     7			  a,aa,r,cpt,
c     8			  caux,shift,al,ar,
c     9   		  cp,bcp,cpp,
c     a  		  diskxyw,diskxyt,diskvel,
c     b  		  lenxyw,lenxyt,lenvel,
c     c			  asave,aasave,iasave,va,
c     d			  namejp,lenjpf,nwp,
c     e			  dskxywb,dskxytb, 
c     f			  lenxywb,lenxytb,
c     g           qbuf,cpbuf,cph,dzov,ncph,
c     h           namedur,lendur,itaudur,iwdur,
c     j           ntauend,iwend,nwmig,nf,ifmin)
c
c   	integer nx,ny,ntau,nw,itau0,istep,icstep
c	integer nzv,nkx,nky,nq,nv,ncp,lvec,lplane,iorder
c	integer naux1,naux2,naux
c      	real dt,dx,dy,dtau,para,vref,dzv
c      	real q(nx,ny,nq),v(nx,ny,nv),vxy(nx,ny),w(nwp)
c      	real om(nf),oms(lvec,nwp),vyx(lplane)
c	real aux1(naux1,nwp),aux2(naux2,nwp),va(nv)
c	complex a(lplane,nwp),aa(lplane,nwp),r(lplane,nwp)
c	real cpt(lplane,2,nwp)
c	complex caux(naux,nwp),shift(nf),al(lvec,nwp),ar(lvec,nwp)
c      	complex cp(nx,ny,ncp),bcp(nkx,nky,nwp),cpp(nx,ny,nwmig)
c      	character*(*) diskxyw,diskxyt,diskvel
c	integer lenxyw,lenxyt,lenvel
c	complex asave(lplane,nwp),aasave(lplane,nwp)
c	integer iasave
c      	character*(*) namejp
c	integer lenjpf,nwp
c       real qbuf(lplane)
c       complex cpbuf(lplane)
c       complex cph(ncph)
c       real dzov(lplane)
c       integer ncph
c       character*(*) namedur
c       integer lendur, itaudur, iwdur
c       integer ntauend, iwend 
c   input:                                                                      
c              nx ---   int*4       number of traces per line
c              ny ---   int*4       number of lines (1=2-D >1=3-D)
c            ntau ---   int*4       maximum migrated time in sample 
c              nw ---   int*4       total number of frequecies to migrate
c           itau0 ---   int*4       starting migrated time in sample
c			            when itau0>1, data have been downward
c				    extrapolated and imaged to this depth
c           istep ---   int*4       depth step of extrapolation in sample
c          icstep ----  int*4       depth step to do f.d. error compensation
c             nzv ----  int*4       number of depth slices in velocity file 
c    	      nkx ----  int*4       fft length along x used for f.d. error
c			            compensation (from radix routine) 
c    	      nky ----  int*4       fft length along y used for f.d. error
c			            compensation (from radix routine) 
c              nq ---   int*4       image array (q) allocation code
c                                   1 = q is q(nx,ny,1) to hold only one
c					depth slice of image
c                                   ntau-itau0+1 = q is q(nx,ny,nq) to hold
c					the whole image volume from itau0 to
c                                       ntau 
c                                   1< nq <ntau-itau0+1 = all frequencies will
c                                   be downward extrapolated nq steps before
c                                   writting back to cp or disk
c              nv ---   int*4       velocity array (v) allocation code
c                                   <nzv = v is v(nx,ny,nv) to hold only nv
c					depth slice of velocity
c                                   nzv = v is v(nx,ny,nzv) to hold
c					the whole velocity volume 
c             ncp ---   int*4       wavefield array (cp) allocation code
c                                   1 = cp is cp(nx,ny,1) to hold only one
c					omega slice of wavefield
c                                   nw = cp is cp(nx,ny,nw) to hold
c					the whole wavefield volume 
c				    (for 2-d migration, ny=1, ncp must be nw)
c            lvec ---   int*4       length of vectorization
c				    =max0(nx,ny), for 3-D migration (ny>1)
c				    <=nw, for 2-D migration (ny=1)
c          lplane ---   int*4       length of working arrays for vectorization
c				    =nx*ny, for 3-D migration (ny>1)
c				    =nx*lvec, for 2-D migration (ny=1)
c          iorder ---   int*4       order of migration                          
c                                   0=45 degree                                 
c                                   1=65 degree                                 
c                                   2=80 degree                                 
c                                   3=87 degree                                 
c                                   4=89.99999 degree                           
c                                   5=90 degree                                 
c           naux1 ---   int*4       length of aux1 array 
c                                   (at least 2*(nkx+nky+30))
c           naux2 ---   int*4       length of aux2 array (at least 4*nky+15)
c            naux ---   int*4       length of aux array (max0(nkx,nky))
c              dt ---   real*4      time sampling interval of data (in seconds)
c              dx ---   real*4      cdp spacing                               
c              dy ---   real*4      line spacing                               
c            dtau ---   real*4      migrated time interval (in seconds) 
c            para ---   real*4      dip-filtering parameter (0. to 1.) 
c            vref ---   real*4      reference velocity used to convert migrated
c			            time samples to depth samples)	       
c                                   >0. performs depth migration (the output
c				    migrated time multipled by vref/2 gives
c				    the migrated depth)
c				    =0. perform time migration (the output is
c				    migrated time)
c             dzv ---   real*4      sampling interval of velocity grid 
c				    in seconds, when time migration
c				    in meters or feet, when depth migration
c     q(nx,ny,nq) ---   real*4      image array 
c     v(nx,ny,nv) ---   real*4      velocity array
c      vxy(nx,ny) ---   real*4      velocity depth slice
c     vyx(lplane) ---   real*4      working array
c           w(nwp) ---  real*4      working array
c          om(nw) ---   real*4      circular frequencies to migrate
c       oms(lvec,nwp) ---   real*4      working array
c     aux1(naux1,nwp) ---   real*4      working array
c     aux2(naux2,nwp) ---   real*4      working array
c       a(lplane,nwp) ---   cplx*8      working array
c      aa(lplane,nwp) ---   cplx*8      working array
c       r(lplane,nwp) ---   cplx*8      working array
c   cpt(lplane,2,nwp) ---   real*4      working array
c       aux(naux) ---   cplx*8      working array
c       shift(nw) ---   cplx*8      working array
c          al(nw,nwp) ---   cplx*8      working array
c          ar(nw,nwp) ---   cplx*8      working array
c   cp(nx,ny,ncp) ---   cplx*8      input wave field array
c    bcp(nkx,nky,nwp) ----  cplx*8  working array when icstep >0 (not needed
c				    when icstep=0)
c    cpp(nx,ny,nwmig) ----  cplx*8     working array
c         diskxyw ---   char*       disk name of input data ffted and 
c				    transposed
c				    (used only when ncp=1)
c         diskxyt ---   char*       disk name of migrated data transposed  
c				    (used only when nq not equal to ntau-itau0+1)
c         diskvel ---   char*       velocity disk file name 
c				    (used only when nv=1)
c          lenxyw ---   int*4       length of string diskxyw
c          lenxyt ---   int*4       length of string diskxyt
c          lenvel ---   int*4       length of string diskvel
c           asave ---   cplx*8      working array 
c          aasave ---   cplx*8      working array 
c          iasave ---   int*4       allocation of asave and aasave
c                                   0=not allocated
c                                   1=alloacated 
c          va(nv) ---   real*4      working array
c          namejp ---   char*       name of job print file  
c          lenjpf ---   int*4       length of string namejp
c             nwp ---   int*4       number of frequencies to migrate in
c                                   parallel CPUs for 3D (1 if 2d)
c         dskxywb ---   char*      disk name to backup wavefield 
c         dskxytb ---   char*      disk name to backup images
c          lenxywb ---   int*4      length of string dskxywb
c          lenxytb ---   int*4      length of string dskxytb
c     qbuf(lplane) ---   real*4     read/write buffer for image
c    cpbuf(lplane) ---  cplx*8     read/write buffer for wavefield
c        cph(ncph) ---  cplx*8     work array for complex(cos,sin) table
c      dzov(nx,ny) ---   real*4    work array for dz/v
c             ncph ---   int*4     length of complex table cph
c          namedur ---   char*     name of disk update record file
c           lendur ---   int*4     length of string namedur
c          itaudur ---   int*4     tau step where last disk update occured
c            iwdur ---   int*4     final frequency index of last disk 
c                                  update occured 
c         ntauend  ---   int*4     last step where iwend is specified 
c         iwend    ---   int*4     highest frequency index to migrate at 
c                                  step ntauend
c                                  the maxmimum frequency (index) to migrate
c                                  at given depth step is linearly 
c                                  interpolated from nw and iwend
c                                  global minimum frequency index is at 1
c          nwmig   ---   int*4     number of frequecies to migrate per backup
c             nf   ---   int*4     number of frequecies to migrate in this job
c          ifmin   ---   int*4     lowest frequency index (relative to
c                                  the lowest global minimum frequency index)
c                                  in this job
c                                  
c  output:                                                                 
c     q(nx,ny,nq) ---   real*4      migrated 3-D data.
c				    when nq=ntau-itau0+1, 
c                                   q stores the whole migrated
c				    3-D volume from itau0 to ntau
c				    when nq not equal to ntau-itau0+1,
c                                   diskxyt is used to store  
c				    the migrated 3-D volume
c  
ccccccccccccccccccccccccc
      	subroutine wxymig(nx,ny,ntau,nw,itau0,istep,icstep,
     1			  nzv,nkx,nky,nq,nv,ncp,lvec,lplane,iorder,
     2			  naux1,naux2,naux,
     3 			  dt,dx,dy,dtau,para,vref,dzv,
     4			  q,v,vxy,
     5 			  om,oms,vyx,w,
     6			  aux1,aux2,
     7			  a,aa,r,cpt,
     8			  caux,shift,al,ar,
     9   		  cp,bcp,cpp,
     a  		  diskxyw,diskxyt,diskvel,
     b  		  lenxyw,lenxyt,lenvel,
     c			  asave,aasave,iasave,va,
     d			  namejp,lenjpf,nwp,
     e  		  dskxywb,dskxytb,
     f  		  lenxywb,lenxytb,
     g                    qbuf,cpbuf,cph,dzov,ncph,
     h                    namedur,lendur,itaudur,iwdur,
     i                    ntauend,iwend,nwmig,nf,ifmin)
c input/output arrays and variables
      	integer nx,ny,ntau,nw,itau0,istep,icstep
        integer nzv,nkx,nky,nq,nv,ncp,lvec,lplane,iorder
        integer naux1,naux2,naux,nf,ifmin
      	real dt,dx,dy,dtau,para,vref,dzv
      	real q(nx,ny,nq),v(nx,ny,nv),vxy(nx,ny),w(nwp)
      	real om(nf),oms(lvec,nwp),vyx(lplane)
        real aux1(naux1,nwp),aux2(naux2,nwp),va(nv)
        complex a(lplane,nwp),aa(lplane,nwp),r(lplane,nwp)
        real cpt(lplane,2,nwp)
        complex caux(naux,nwp),shift(nf),al(lvec,nwp),ar(lvec,nwp)
      	complex cp(nx,ny,ncp),bcp(nkx,nky,nwp),cpp(nx,ny,nwmig)
      	character*(*) diskxyw,diskxyt,diskvel
      	character*(*) dskxywb,dskxytb
        integer lenxyw,lenxyt,lenvel
        integer lenxywb,lenxytb
        complex asave(lplane,nwp),aasave(lplane,nwp)
        integer iasave
      	character*(*) namejp
        integer lenjpf,nwp,nwmig
        real qbuf(lplane)
        complex cpbuf(lplane)
        complex cph(ncph)
        real dzov(nx,ny)
        integer ncph
        character*(*) namedur
        integer lendur, itaudur, iwdur
c variables used inside this subtoutine
      	real alpha(5), beta(5)
      	logical timig	
        real cstep,dstep,dz,trick,paras,tmp,scale,csteps,vac
        integer iqdisk,ivdisk,ipdisk,ireclq,ireclv,ireclp
        integer itau,iextra,icom,izv,jq,jw,iw,iz
        integer i1,i2,nsplit,is,nwm,nprint
        integer isave,nvs,ivs
        integer ijpf, ios, idurf
        real odpp, opi2
        integer itaustart
        real scxy

	character*24 ctime
	integer*4 time 

        isave = iasave
        if(ny.eq.1) isave = 0 

ccccc open disk to store migration section transposed
      	if (lenxyt.gt.0 .or. lendur.gt.0) then 
         	ireclq = nx * ny * 4
cccc_sgi         	ireclq = nx * ny
	        iqdisk = 10
         	open(iqdisk,file=diskxyt(1:lenxyt),
     1			form="unformatted",iostat=ios,
     2     		status="unknown",access="direct",recl=ireclq)
		if(ios.ne.0) call cancel(ios,' error open iqdisk ')
c-------------- check to see if disk space can be allocated 
         	call zeroal(q,nx*ny*nq,4)
		call zeroal(qbuf,nx*ny,4)
		itaustart = itau0
c---5---10--------20--------30--------40--------50--------60--------70--------80
		if(itaudur.gt.itau0) itaustart = itaudur + 1
ccc		    do itau=itaustart,ntau
ccc        		write(iqdisk,rec=itau,iostat=ios) qbuf
cc			    if(ios.ne.0) 
cc     1              call cancel(ios,' error write iqdisk ')
cc		    end do
            close(iqdisk)
            open(iqdisk,file=diskxyt(1:lenxyt),
     1		 form="unformatted",iostat=ios,
     2        	 status="unknown",access="direct",recl=ireclq)
	    call flush(iqdisk)
      	end if
cccc open ffted data disk
        if(lenxyw.gt.0) then
      		ireclp = nx * ny * 4 * 2
ccc_sgi      		ireclp = nx * ny * 2
      		ipdisk = 15
      		open(ipdisk,file=diskxyw(1:lenxyw),
     1		     form="unformatted",iostat=ios,
     2     	     status="unknown",access="direct",recl=ireclp)
		if(ios.ne.0) call cancel(ios,' error open ipdisk ')
                close(ipdisk)
      		open(ipdisk,file=diskxyw(1:lenxyw),
     1		     form="unformatted",iostat=ios,
     2     	     status="unknown",access="direct",recl=ireclp)
		if(ios.ne.0) call cancel(ios,' error open ipdisk ')
        end if
cccc open velocity grid disk file
        if(nv.ne.nzv) then
      		ireclv = nx * ny * 4
ccc_sgi      		ireclv = nx * ny 
      		ivdisk = 20
      		open(ivdisk,file=diskvel(1:lenvel),
     1 		     form="unformatted",iostat=ios,
     2     	     status="old",access="direct",recl=ireclv)
		if(ios.ne.0) call cancel(ios,' error open ivdisk ')
		close(ivdisk)
      		open(ivdisk,file=diskvel(1:lenvel),
     1 		     form="unformatted",iostat=ios,
     2     	     status="old",access="direct",recl=ireclv)
		if(ios.ne.0) call cancel(ios,' error open ivdisk ')

        end if
cccc open print file
        if(lenjpf.gt.0) then
		ijpf = 25 
		ios = 0
      		open(ijpf,file=namejp(1:lenjpf),
     1     	    status="old",iostat=ios,access="append")
		call flush(ijpf)
		close(ijpf)
      		open(ijpf,file=namejp(1:lenjpf),
     1     	     status="old",iostat=ios,access="append")
		if (ios.ne.0) call cancel(ios,' error open jpfile')
	    end if

ccc open disk update record file 
        if(lendur.gt.0) then
                idurf = 30
                ios = 0
                open(idurf,file=namedur(1:lendur),
     1               status="old",iostat=ios,access="append")
		if(itau0.eq.1 .and. iwdur.eq.0) then
		ntauq = nq
		idurw = 0
		idurtau = itau0
c---5---10--------20--------30--------40--------50--------60--------70--------80
		write(idurf,*)
     1		" itau0dur:", itau0," itaudur:",itau0+ntauq-1,
     2		" iwdur:",idurw,"  ",ctime(time())
				end if
                call flush(idurf)
                close(idurf)
                open(idurf,file=namedur(1:lendur),
     1               status="old",iostat=ios,access="append")
                if (ios.ne.0) call cancel(ios,' error open durfile')
        end if

	if(lenxytb.gt.0) then
         	ireclq = nx * ny * 4
cccc_sgi         	ireclq = nx * ny
      		iqbdisk = 35
      		open(iqbdisk,file=dskxytb(1:lenxytb),
     1 		     form="unformatted",iostat=ios,
     2     	     status="unknown",access="direct",recl=ireclq)
		if(ios.ne.0) call cancel(ios,' error open iqbdisk ')
	end if
	if(lenxywb.gt.0) then
      		ireclp = nx * ny * 4 * 2
cccc_sgi      		ireclp = nx * ny * 2
      		ipbdisk = 40
      		open(ipbdisk,file=dskxywb(1:lenxywb),
     1		     form="unformatted",iostat=ios,
     2     	     status="unknown",access="direct",recl=ireclp)
		if(ios.ne.0) call cancel(ios,' error open ipbdisk ')
	end if
	if(ntau.eq.0 .or. ntau.lt.itau0) then
		if(lenjpf.eq.0) then
		  call msgsci(' No migration ntau= ', ntau)
		else 
       		  write(ijpf,*) ' No migration ntau= ', ntau
		  call flush(ijpf)
		end if
		go to 30000 
        end if
cccc initializations 
c---5---10--------20--------30--------40--------50--------60--------70--------80
      	if( vref.gt.0.) then
		dz = dtau * vref * 0.5
		timig = .false.	
		if(lenjpf.eq.0) then
		  call msgscf(' Depth Migration with dz= ', dz)
		  call msgscf(' Velocity Depth Interval dzv= ', dzv)
		else 
       		  write(ijpf,*) ' Depth Migration with dz= ', dz 
       		  write(ijpf,*) ' Velocity Depth Interval dzv= ', dzv 
		  call flush(ijpf)
		end if
      	else
		dz = dtau
		timig = .true.	
		if(lenjpf.eq.0) then
		  call msgscf(' Time Migration with dtau= ', dz)
		  call msgscf(' Velocity Time Interval dtauv= ', dzv)
		else 
       		  write(ijpf,*) ' Time Migration with dtau= ', dz 
       		  write(ijpf,*) ' Velocity Time Interval dtauv= ', dzv
		  call flush(ijpf)
		end if
      	end if
c----- compensation step and depth extrapolatin step
      	cstep = dz * icstep
      	dstep = dz * istep	
c----- 1/6 trick value
      	trick = 0.14

c----- no dipfiltering needed if f.d. error compensation is used
	    paras = para
ccc    	if (icstep.gt.0) paras=0. 
        if(icstep.gt.0) then
		call cfft2di(nkx,nky,aux1(1,1),aux2(1,1),
     1			     naux1,naux2)
		do ip=2,nwp
		    do i=1,naux1
			    aux1(i,ip) = aux1(i,1)
		    end do
		    do i=1,naux2
			    aux2(i,ip) = aux2(i,1)
		    end do
		end do
        end if 
c----- compute coefficients for different order of migration 
        nsplit = max0(iorder,1)
      	call alpbe(iorder,alpha,beta,nsplit)

c----- compute shift term
      	if(timig) then
    		do iw=1,nf
		    tmp = om(iw)*dtau
		    shift(iw) = cmplx(cos(tmp),sin(tmp))
	     	end do	
	else 
	        pi = 3.141592654
	    	pi2 = 2. * pi
        	dpp = pi2 / (ncph-1)
        	odpp = ncph-1.
        	opi2 = 1./pi2
        	do i=1,ncph
                	tmp = (i-1)*dpp
                	cph(i)=cmplx(cos(tmp),sin(tmp))
        	end do
      	end if
c----- scale image by the number of frequencies summed
      	scale = 1./nw
        scxy = 1./(nkx*nky)

cccc job print interval
        nprint = istep * 10 

cccc scale velocity
        if(nv.eq.nzv) then
    		call rrscale(v,0.5,nx*ny*nv)
	     	do ivs=1,nv
cccc	 		    call ravg(v(1,1,ivs),nx*ny,va(ivs))
			call rmin(v(1,1,ivs),nx*ny,va(ivs))
	    	end do
	    	nvs = nv
        else 
	    	tmp = dz * nq / dzv + 1.5 
	    	nvs = tmp
	    	if (tmp.lt.1.) nvs = 1
	    	if(nvs.gt.nv) call cancel(nvs,"error in value of nv")
        end if
cccc
cccc   3d:  ny>1
cccc   2d:  ny=1
        idurtau = itaudur
        idurw = iwdur 
        if(lendur.eq.0) then
    		idurtau = 0
	    	idurw = 0
        end if
        if(ny.eq.1) go to 20000
ccccc 3d migration over depth step
      	do itau = itau0,ntau,nq
		ntauq = nq
		if(itau+ntauq-1 .gt. ntau) ntauq = ntau - itau + 1
cccc----------	read in velocity slices 
		if(nv.ne.nzv) then 
		   do ivs=1,nvs
		      tmp = itau*dz/dzv + ivs + 0.5
		      izv = tmp
		      if(izv.gt.nzv) izv = nzv
		      if(izv.lt.1) izv = 1
c---5---10--------20--------30--------40--------50--------60--------70--------80
		      if(lenjpf.eq.0) then
		         call msgsci(' Velocity read at slice= ', izv)
		      else 
       		         write(ijpf,*) ' Velocity read at slice= ',izv
		         call flush(ijpf)
		      end if
		      read(ivdisk,rec=izv,iostat=ios) vxy
		      if(ios.ne.0) 
     1                   call cancel(ios,'error read ivdisk')
		      call rrscale(vxy,0.5,nx*ny)
		      call rrcopy(vxy,v(1,1,ivs),nx*ny)
cccc			call ravg(vxy,nx*ny,va(ivs))
		      call rmin(vxy,nx*ny,va(ivs))
		   end do
		end if
ccccccc ------- zero image output before migration
	    	call zeroal(q,nx*ny*ntauq,4)
		if(itau.eq.itau0 .and. iwdur.gt.0
     1              .and. itaudur.gt.itau0) then

			if(lenjpf.eq.0) then
                		call msgsci(
     1                  ' reading partial image back at itau = ',
     2                          itau)
			else 
       	    write(ijpf,*) " reading partial image back at itau = ", 
     1                        itau
				call flush(ijpf)
			end if
			do ita=1,ntauq
			   iz = itau + ita - 1
                           read(iqdisk,rec=iz,iostat=ios) qbuf
			   call rrcopy(qbuf,q(1,1,ita),nx*ny)
			   if(ios.ne.0) 
     1                           call cancel(ios,'error read iqdisk')
			end do
		end if		
ccccccc frequency loop
		iwstart = 1
		if(itau.eq.itau0 .and. iwdur.ne.0) iwstart=iwdur+1
		kwcpp = 0
		iwcpp = iwstart
c---5---10--------20--------30--------40--------50--------60--------70--------80
		do iw=iwstart,nf,nwp
			nwps = nwp
			if(iw+nwps-1 .gt. nf) nwps = nf - iw + 1
			tmp = (iwend-nw)
			tmp = tmp/ntauend*itau + nw + 0.5
			jwend = tmp
			if(jwend.lt.(iw+ifmin-1) .and. kwcpp.eq.0) 
     1				go to 22222
			if(jwend.lt.(iw+ifmin-1) .and. kwcpp.ne.0) 
     1				go to 11111
			do iws=1,nwps
			   jw = iw + iws - 1
			   kws = kwcpp + iws
   			   w(iws) = om(jw)
cccccc------------------------  read from disk if needed
			   if (ncp.ne.nf) then
       			      read(ipdisk,rec=jw,iostat=ios) cpbuf
			      if(ios.ne.0) 
     1                           call cancel(ios,'error read ipdisk')
     			      call cccopy(cpbuf,cpp(1,1,kws),nx*ny)
			   else 
			    call cccopy(cp(1,1,jw),cpp(1,1,kws),nx*ny)
			   end if
			end do
c---5---10--------20--------30--------40--------50--------60--------70--------80
cccccccc   inner depth loop within nq steps 
c---5---10---15
			do ita = 1, ntauq
				iz = itau + ita - 1  
				tmp = (iwend-nw)
				tmp = tmp/ntauend*iz + nw + 0.5
				itmp = tmp
				itmp = itmp - ifmin + 1
			        if(itmp.gt.nf) itmp = nf
				mwps = itmp - iw
                if(mwps.gt.nwps) mwps = nwps
				iextra = 0
         		if(mod(iz,istep).eq.0) iextra=1
	 			icom = 0
	 			if(icstep.gt.0) then
					if(mod(iz,icstep).eq.0) icom=1
					csteps = cstep
ccccc------------------- apply filtering of evanacent energy
					if(iz.eq.1) then
						csteps = 0.
						icom = 1
					end if
	 			end if
c get velocity for diffraction term 
	 			if(iextra.eq.1 .or. icom.eq.1 .or. 
     1				     iz.eq.itau0 .or. ita.eq.1) then
c---5---10--------20--------30--------40--------50--------60--------70--------80

				   if(nv.eq.nzv) then 
	 				tmp = iz * dz / dzv + 1.5 
				   else 
	 				tmp = ita * dz / dzv + 1.5 
				   end if
				   izv = tmp
				   izv = max0(1,izv)
				   izv = min0(nvs,izv)
				   call rrcopy(v(1,1,izv),vxy,nx*ny)
				   vac = va(izv)
				end if
cccc get the velocity for phase-shift term
				if(.not.timig) then
				   if(nv.eq.nzv) then
	 				tmp = iz * dz / dzv + 1. 
				   else 
	 				tmp = ita * dz / dzv + 1. 
				   end if
				   izv = tmp
c---5---10--------20--------30--------40--------50--------60--------70--------80
				   if(izv.ge.1 .and. izv.lt.nvs) then
				      tmp = tmp - izv
				      do iy=1,ny
				      do ix=1,nx
					 dzov(ix,iy) = v(ix,iy,izv) + 
     1				tmp*(v(ix,iy,izv+1)-v(ix,iy,izv))
				      end do
				      end do
				   else 
				      izv = max0(1,izv)
				      izv = min0(nvs,izv)
				call rrcopy(v(1,1,izv),dzov,nx*ny)
				   end if
				   do iy=1,ny
				   do ix=1,nx
				      dzov(ix,iy)=dz/dzov(ix,iy)
				   end do
				   end do
				end if
ccccccc  f.d. splitting error compensation if needed 
c---5---10--------20--------30--------40--------50--------60--------70--------80
	 			if (icom .eq. 1) then
	    		           call zeroal(bcp,nkx*nky*nwp,8)
                                   do iws=1,mwps
				      kws = kwcpp + iws
				      do iy=1,ny
				      do ix=1,nx
                                   bcp(ix,iy,iws) = cpp(ix,iy,kws)  
                                      end do
                                      end do
                                   call cfft2df(nkx,nky,aux1(1,iws), 
     1                                          aux2(1,iws),
     2                                          bcp(1,1,iws),
     3                                          caux(1,iws),naux1,
     4                                          naux2)
                                   end do
c---5---10--------20--------30--------40--------50--------60--------70--------80
c$par doall
				   do iws=1,mwps
				      call ecom(bcp(1,1,iws),
     2					  	nkx,nky,dx,dy,csteps,
     3				  		dstep,w(iws),iorder,
     4	 					trick,vac,timig,
     7						caux(1,iws),naux)
				   end do
                                   do iws=1,mwps
                                   call cfft2db(nkx,nky,aux1(1,iws), 
     1                                          aux2(1,iws),
     2                                          bcp(1,1,iws),
     3                                          caux(1,iws),naux1,
     4                                          naux2)
				      kws = kwcpp + iws
				      do iy=1,ny
				      do ix=1,nx
                                   cpp(ix,iy,kws) = bcp(ix,iy,iws)  
     1                                 *scxy
                                      end do
                                      end do
                                   end do
	 			end if
cccccc apply shift term
c---5---10--------20--------30--------40--------50--------60--------70--------80
				if(timig) then
c$par doall private(jw,kws)
				   do iws=1,mwps
			   	      jw = iw + iws - 1
					  kws = kwcpp + iws
				      call ccscale(cpp(1,1,kws),
     1					     	shift(jw),nx*ny)
				   end do
				else
c$par doall private(kws)
				   do iws=1,mwps 
					  kws = kwcpp + iws
				   	  call shiftc(cpp(1,1,kws),
     1				       	    w(iws),dzov,nx*ny,
     2                          cph,ncph,odpp,opi2)
				   end do
				end if
cccccc apply diffraction term 
				if(iextra.eq.1) then
cccccc-----------------------       splitting cross line (inline migration)
cccccc---------------------------------- transpose v and wavefield
					call rtransp(vxy,vyx,nx,ny)
c$par doall private(kws)
				   do iws=1,mwps
					kws = kwcpp + iws
					call c2rit(cpp(1,1,kws),
     1						cpt(1,1,iws),   
     2						cpt(1,2,iws),nx,ny)
				   end do
ccccc------------------------       splitting in order of f.d. operator
				   do is=1,nsplit
				      do iws=1,mwps
				      do i1=1,ny
					     oms(i1,iws) = w(iws)
				      end do
				      end do
c---5---10--------20--------30--------40--------50--------60--------70--------80
c$par doall
				      do iws=1,mwps
					 call vwmig(ny,nx,oms(1,iws),
     1					  	vyx,dt,dstep,dx,
     2						paras,trick,
     3						alpha(is),
     4						beta(is),timig,
     5						cpt(1,1,iws),a(1,iws),
     6						aa(1,iws),r(1,iws),
     7            			 	al(1,iws),ar(1,iws),
     8						asave(1,iws),
     9						aasave(1,iws),isave)
				      end do
				   end do
cccccc---------------------------------- transpose wavefield
c$par doall private(kws)
				   do iws=1,mwps
					  kws = kwcpp + iws
				      call ri2ct(cpt(1,1,iws),
     1					   cpt(1,2,iws),
     2					   cpp(1,1,kws),ny,nx)
				   end do
cccccc-----------------------       splitting in line (crossline migration)
c$par doall private(kws)
				   do iws=1,mwps
					  kws = kwcpp + iws
				      call c2ri(cpp(1,1,kws),
     1						cpt(1,1,iws),
     2						cpt(1,2,iws),nx*ny)
				   end do

ccccc------------------------       splitting in order of f.d. operator
				   do is=1,nsplit
c---5---10--------20--------30--------40--------50--------60--------70--------80
				      do iws=1,mwps
					 do i1=1,nx
					    oms(i1,iws) = w(iws)
					 end do
				      end do
				      if (isave.eq.0) then
c$par doall
				         do iws=1,mwps
					 call vwmig(nx,ny,oms(1,iws),
     1					   	vxy,dt,dstep,dy,
     2					   	paras,trick,
     3					        alpha(is),beta(is),
     4						timig,cpt(1,1,iws),
     5						a(1,iws),aa(1,iws),
     6						r(1,iws),al(1,iws),
     7					        ar(1,iws),
     8						asave(1,iws),
     9					        aasave(1,iws),0)
				         end do	
				      else 
c$par doall
					 do iws=1,mwps
				         call vwmig(nx,ny,oms(1,iws),        
     1					 	vxy,dt,dstep,dy,
     2						paras,trick,
     3						alpha(is),beta(is),
     4					        timig,cpt(1,1,iws),
     5				   		asave(1,iws),
     6					   	aasave(1,iws),
     7						r(1,iws),al(1,iws),
     8					  	ar(1,iws),a(1,iws),
     9					  	aa(1,iws),-1)
					 end do
				      end if
				   end do
c$par doall private(kws)
				   do iws=1,mwps
					  kws = kwcpp + iws
				      call ri2c(cpt(1,1,iws),
     1						cpt(1,2,iws),
     2						cpp(1,1,kws),nx*ny)
				   end do
				end if

c---5---10--------20--------30--------40--------50--------60--------70--------80
cccccc------------------------- imaging condition (sum over omega)
				do iws=1,mwps
				   kws = kwcpp + iws
                   do iy=1,ny
                      do ix=1,nx
                         q(ix,iy,ita)=q(ix,iy,ita)
     2                               +real(cpp(ix,iy,kws))
                      end do
				   end do
                end do
			end do
c---5---10--------20--------30--------40--------50--------60--------70--------80
cccccc end of inner depth loop
			kwcpp = kwcpp + nwps
11111       continue
			if(kwcpp.lt.nwmig .and. iw+nwp-1.lt.nf 
     1		                  .and. jwend.ge.iw) go to 11555
cccc
cccccc------------------------  write wavefield to disk if needed
cccc
			do iws=1,kwcpp
		       jw = iwcpp+iws-1		
cccc       	write(ijpf,*) " update frequency jw =",jw ," kwcpp=",kwcpp,
cccc     1	              " iw=",iw," iwcpp=",iwcpp
               if (ncp.ne.nf .or. lendur.gt.0) then
			      call cccopy(cpp(1,1,iws),cpbuf,
     1					  nx*ny)
                  write(ipdisk,rec=jw,iostat=ios) 
     1			      cpbuf
			      if(ios.ne.0) 
     1                call cancel(ios,'error write ipdisk')
			   end if
			   if (ncp.eq.nf) then
			      call cccopy(cpp(1,1,iws),cp(1,1,jw),nx*ny)
               end if
			end do
		    if(lendur.gt.0) then
ccccc a call to flush is not enough to savely write everything to disk
ccccc use close and open instead  
cccc =====>			call flush(ipdisk)
				close(ipdisk)
      			open(ipdisk,file=diskxyw(1:lenxyw),
     1			form="unformatted",iostat=ios,
     2     		status="unknown",access="direct",recl=ireclp)
			else if(ncp.ne.nf .and. lendur.eq.0) then
				call flush(ipdisk)
			end if
cccc -- backup partial images if needed
			if(lendur.gt.0) then
cccc following two lines to skip updating image files 
				idurw = iwcpp + kwcpp - 1
				if(idurw.eq.0) goto 11000
				do ita=1,ntauq
				 iz = itau + ita - 1
				 call rrcopy(q(1,1,ita),qbuf,nx*ny)
               	 write(iqdisk,rec=iz,iostat=ios) qbuf
				 if(ios.ne.0) 
     1             call cancel(ios,'error write iqdisk')
				end do
ccccc a call to flush is not enough to savely write everything to disk
ccccc use close and open instead  
ccccc	        call flush(iqdisk)
				close(iqdisk)
         		open(iqdisk,file=diskxyt(1:lenxyt),
     1			form="unformatted",iostat=ios,
     2        	status="unknown",access="direct",recl=ireclq)
11000 	continue 
				idurw = iwcpp + kwcpp - 1
				idurtau = itau + ntauq -1
				idurtau0 = itau
				write(idurf,*)
c---5---10--------20--------30--------40--------50--------60--------70--------80
     1			" itau0dur:", idurtau0," itaudur:",idurtau,
     2          	" iwdur:",idurw, "  ",ctime(time())
				call flush(idurf)
            end if
			iwcpp = iwcpp + kwcpp
			kwcpp = 0
11555   continue
        end do
c---5---10--------20--------30--------40--------50--------60--------70--------80
cccc end of frequency loop
22222    continue
         if (ncp.ne.nf .or. lendur.gt.0) call flush(ipdisk)
cccccc----------- scale images and write to disk if needed
         call rrscale(q,scale,nx*ny*nq)
         if(lendur.gt.0 .or. nq.ne.(ntau-itau0+1)) then
		 	do ita=1,ntauq
				iz = itau + ita - 1
				call rrcopy(q(1,1,ita),qbuf,nx*ny)
               	write(iqdisk,rec=iz,iostat=ios) 
     1				qbuf
				if(ios.ne.0) 
     1         		call cancel(ios,'error write iqdisk')
		 	end do
		   	call flush(iqdisk)
			if(lendur.gt.0) then
		  		ntauq = nq
		  		idurw = 0
c---5---10--------20--------30--------40--------50--------60--------70--------80
		  		if(idurtau+ntauq-1 .gt. ntau) 
     1				ntauq = ntau - idurtau + 1
		  		write(idurf,*)
     1		" itau0dur:", idurtau+1," itaudur:",idurtau+ntauq,
     2		" iwdur:",idurw,"  ",ctime(time())
				call flush(idurf)
			end if
         end if
ccccccccccccccc   backup to disk
		 if (lenxytb.gt.0) then
            if (ncp.ne.nf) then
		       do iw=1,nf
                  read(ipdisk,rec=iw,iostat=ios) cpbuf
			      if (ios.ne.0) 
     1                call cancel(ios,'error read ipdisk')
                  write(ipbdisk,rec=iw,iostat=ios) cpbuf
			      if (ios.ne.0) 
     1                call cancel(ios,'error write ipbdisk')
		       end do
		    else 
		       do iw=1,nf
			      call cccopy(cp(1,1,iw),cpbuf,nx*ny)
                  write(ipbdisk,rec=iw,iostat=ios) cpbuf
			      if (ios.ne.0) 
     1                call cancel(ios,'error write ipbdisk')
		       end do
		    end if
		    do ita=1,ntauq
		       iz = itau + ita - 1
		       call rrcopy(q(1,1,ita),qbuf,nx*ny)
               write(iqbdisk,rec=iz,iostat=ios) qbuf
		       if (ios.ne.0) 
     1            call cancel(ios,'error write iqbdisk')
		    end do
		    call flush(ipbdisk)
		    call flush(iqbdisk)
		end if
cccc job printout
		if(timig) then
                        tmp = (itau+ntauq-1) * dtau * 1000.
			if(lenjpf.eq.0) then
                		call msgscf(
     1                  " Migration Done at Migrated Time (in ms) = ",
     2                          tmp)
			else 
       	write(ijpf,*) " Migration Done at Migrated Time (in ms) = ", 
     1				tmp
				call flush(ijpf)
			end if
        else 
            tmp = (itau+ntauq-1) * dz
			if(lenjpf.eq.0) then
                	call msgscf(
     1                  " Migration Done at Depth (in m or ft) = ",
     2                          tmp)
			else 
       	    write(ijpf,*) " Migration Done at Depth (in m or ft) = ",
     1				tmp
				call flush(ijpf)
			end if
		end if
	    end do
        if(nq .ne. (ntau-itau0+1) .and. lendur.gt.0) then 
 		    write(idurf,*)
     1	" itau0dur:", 0, " itaudur:",0," iwdur:",0,"  ",ctime(time())
        end if
        go to 30000
cccc
ccccccc 2d migration
cccc
20000   continue
        do itau=itau0,ntau
                iz = itau
                iextra = 0
                if(mod(iz,istep).eq.0) iextra=1
                icom = 0
                if(icstep.gt.0) then
                        if(mod(iz,icstep).eq.0) icom = 1
                        csteps = cstep
ccccc------------------- apply filtering of evanacent energy
                        if(itau.eq.1) then
                                csteps = 0.
                                icom = 1
                        end if
                end if
c read in velocity at this depth
c---5---10--------20--------30--------40--------50--------60--------70--------80
                if(iextra.eq.1 .or. icom.eq.1 .or. iz.eq.itau0) then
                        tmp = iz * dz / dzv + 1.5
                        izv = tmp
                        izv = max0(1,izv)
                        izv = min0(nzv,izv)
                        if(nv.ne.nzv) then
                            read(ivdisk,rec=izv) vxy
			    call rrscale(vxy,0.5,nx*ny)
                        else
                            call rrcopy(v(1,1,izv),vxy,nx*ny)
			end if
                end if
		if(.not.timig) then
	 		tmp = iz * dz / dzv + 1. 
			izv = tmp
                    	if(nv.ne.nzv) then
                        	izv = max0(1,izv)
                        	izv = min0(nzv,izv)
                        	read(ivdisk,rec=izv) vxy
				call rrscale(dzov,0.5,nx*ny)
			else 
c---5---10--------20--------30--------40--------50--------60--------70--------80
				if(izv.ge.1 .and. izv.lt.nzv) then
				   tmp = tmp - izv
				   do iy=1,ny
				   do ix=1,nx
				     dzov(ix,iy) = v(ix,iy,izv) + 
     1				     tmp*(v(ix,iy,izv+1)-v(ix,iy,izv))
				   end do
				   end do
			        else 
				   izv = max0(1,izv)
				   izv = min0(nzv,izv)
				   call rrcopy(v(1,1,izv),dzov,nx*ny)
			        end if
			end if
		        do iy=1,ny
			do ix=1,nx
		   		dzov(ix,iy)=dz/dzov(ix,iy)
	  		end do
		        end do
		end if
ccccccc --------- find average velocity for error compensation
ccccc                if(icom.eq.1) call ravg(vxy,nx*ny,vac)
                if(icom.eq.1) call rmin(vxy,nx*ny,vac)
                jq = iz - itau0 + 1
                if(nq.ne.(ntau-itau0+1)) jq = 1
				call zeroal(q(1,1,jq),nx*ny,4)
c
ccccccc  f.d. splitting error compensation if needed
                if (icom .eq. 1) then
                	do iw=1,nf
                       	ww = om(iw)
                        call zeroal(bcp,nkx*nky,8)
                        do iy=1,ny
                           do ix=1,nx
                           	    bcp(ix,iy,1) = cp(ix,iy,iw)
                            end do
                        end do
                        call cfft2df(nkx,nky,aux1,aux2,
     1                               bcp,caux,naux1,naux2)
                        call ecom(bcp,
     1                            nkx,nky,dx,dy,csteps,
     2                            dstep,ww,iorder,trick,
     3                            vac,timig,caux,naux)

                        call cfft2db(nkx,nky,aux1,aux2,
     1                               bcp,caux,naux1,naux2)
                        do iy=1,ny
                           do ix=1,nx
                                cp(ix,iy,iw) = bcp(ix,iy,1)*scxy
                           end do
                        end do
                    end do
                end if
c---5---10--------20--------30--------40--------50--------60--------70--------80
cccccc apply shift term
                if(timig) then
                	do iw=1,nf
                    	call ccscale(cp(1,1,iw),
     1                         	shift(iw),nx*ny)
                    end do
                else
                    do iw=1,nf
			call shiftc(cp(1,1,iw),
     1			            om(iw),dzov,nx*ny,
     2                              cph,ncph,odpp,opi2)
c                       call cshift(cp(1,1,iw),om(iw),
c     1                             vxy,nx*ny,dz)
                    end do
                end if
ccccc ---------------   loop over omega for diffraction term
		if(iextra.eq.1) then
			do iw=1,nf,lvec
				nwm=min0(lvec,nf-iw+1)
ccccc-------------------------  transpose v and wavefield 
				do i2=1,nx
					i20 = (i2-1)*nwm
					vac = vxy(i2,1)
					do i1=1,nwm
					   cpt(i1+i20,1,1) = 
     1					     real(
     2					      cp(i2,1,iw+i1-1))
					   cpt(i1+i20,2,1) = 
     1					     aimag(
     2					      cp(i2,1,iw+i1-1))
					   vyx(i1+i20) = vac 
					end do
				end do
				do is=1,nsplit
					do i1=1,nwm
						oms(i1,1)=om(iw+i1-1)
					end do
					call vwmig(nwm,nx,oms,
     1					   	   vyx,dt,
     2					   	   dstep,dx,
     3					   	   paras,trick,
     4					   	   alpha(is),
     5					   	   beta(is),
     6					   	   timig,
     7					   	   cpt,a,
     8					   	   aa,r,al,
     9					   	   ar,asave,
     a  					   aasave,0)
				end do
ccccc---------------------------------  transpose wavefield 
				do i2=1,nx
					i20 = (i2-1)*nwm
					do i1=1,nwm
     				      	    cp(i2,1,iw+i1-1) =
     1					       cmplx(
     2						cpt(i1+i20,1,1),
     3						cpt(i1+i20,2,1))
					end do
				end do
			end do
		end if
cccccc------------------------- imaging condition (sum over omega)
		do ix=1,nx
			do iw=1,nf
				q(ix,1,jq) = q(ix,1,jq)
     1			     	           +real(cp(ix,1,iw))
			end do
		end do
cccccc----------- scale images and write to disk if needed
		call rrscale(q(1,1,jq),scale,nx*ny)
		if(nq.ne.(ntau-itau0+1)) then
			write(iqdisk,rec=iz) q
		end if
		if(mod(itau,nprint).eq.0 .and. timig ) then 
			tmp = itau * dtau * 1000. 
			if(lenjpf.eq.0) then
      			call msgscf(
     1			" Migration Done at Migrated Time (in s) = ",
     2          		tmp)
			else
       	write(ijpf,*) " Migration Done at Migrated Time (in s) = ", 
     1				tmp
				call flush(ijpf)
			end if
		end if
		if(mod(itau,nprint).eq.0 .and. .not.timig ) then 
			tmp = iz * dz 
			if(lenjpf.eq.0) then
      			call msgscf(
     1			" Migration Done at Depth (in m or ft) = ",
     2          		tmp)
			else
       	   write(ijpf,*) " Migration Done at Depth (in m or ft) = ", 
     1				tmp
				call flush(ijpf)
			end if
		end if
	    end do
ccc
ccc
30000   continue
ccc
ccccc close disk datasets
        if(nq.ne.(ntau-itau0+1)) close(iqdisk)
        if(nv.ne.nzv) close(ivdisk)
        if(ncp.ne.nf) close(ipdisk)
        if(lenjpf.gt.0) close(ijpf)
        if(lenxytb.gt.0) close(iqbdisk)
        if(lenxywb.gt.0) close(ipbdisk)
        if(lendur.gt.0) close(idurf)
      	return
      	end 
