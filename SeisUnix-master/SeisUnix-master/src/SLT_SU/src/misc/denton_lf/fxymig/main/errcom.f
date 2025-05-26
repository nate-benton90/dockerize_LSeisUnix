cccccc compensating finite-difference errors in 3-d w-x migration/modeling      
c                                                                               
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc           
c        subroutine ecom(cpk,nkx,nky,dx,dy,cstep,dstep,                
c      *                 w,iorder,trick,va,                              
c      *                 timig,caux,naux)                 
c        complex cpk(nkx,nky)                                         
c        complex caux(naux)                                                     
c        real va                                                                
c        integer naux                                             
c        integer nkx, nky                                               
c        real dx, dy, cstep, w, trick, dstep                                    
c        logical timig                                                          
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc           
c                                                                               
c   input:                                                                      
c                                                                               
c      cpk   ---   2-d fft of wavefield slice nkx by nky for frequency w
c      nkx   ---   number of x wavenumbers (fft length of x)                    
c      nky   ---   number of y wavenumbexs (fft length of y)                    
c       dx   ---   cdp spacing                                                  
c       dy   ---   line spacing                                                 
c    cstep   ---   error compensation step size                                 
c                  (=in s if time migration used                           
c                   =in m or ft if depth migration used)                        
c    dstep   ---   depth extrapolation step                                     
c                  (=in s if time migration used                           
c                   =in m or ft if depth migration used)                        
c        w   ---   circular frequency of input slice                            
c   iorder   ---   order of w-x migration/modeling operator                     
c    trick   ---   1/6 trick value used in f.d. approximation of d2/dx2         
c       va   ---   velocity at current depth                                    
c    timig   ---   .true. for time migration; .false. for depth migration
c     caux   ---   work array                                                   
c     naux   ---   length of caux (max0(nkx,nky))                               
c                                                                               
c   output:                                                                     
c                                                                               
c      cpk   ---   wavefield (fft) slice of nkx by nky for given frequency w
c                                                                               
c   author:   zhiming li                                                        
c.............                                                                  
       subroutine ecom(cpk,nkx,nky,dx,dy,cstep,dstep,
     1                 w,iorder,trick,va,
     2                 timig,caux,naux)
       complex cpk(nkx,nky)
       complex caux(naux)
       real va
       integer naux
       integer nkx, nky
       real dx, dy, cstep, w, trick, dstep
       logical timig
       real alpha(5), beta(5)
       real zcstep, zdstep
cccccc initializations 
       scale = 1./(nkx*nky)                                                  
       n = max0(iorder,1)                                                    
       call alpbe(iorder,alpha,beta,n)                                
cccc error compensation                                                         
cc                                                                              
       if ( timig ) then                                                        
          zcstep = cstep * va                                                   
          zdstep = dstep * va                                                   
       else                                                                     
          zcstep = cstep                                                        
          zdstep = dstep                                                        
       end if                                                                   
cc  compensating errors                                                         
       call errcom(cpk,nkx,nky,dx,dy,w,zcstep,zdstep,va,
     1             n,alpha,beta,trick,caux,naux)
       return                                                                   
       end                                                                      
cccc                                                                            
cccccccc   3-d implicit f.d. error compensation by phase shift                  
c  input:                                                                       
c   cp(nkx,nky)     ----   wave field for given frequency w (p(kx,ky,w))        
c              nkx  ----   fft length along x direction                         
c              nky  ----   fft length along y direction                         
c               dx  ----   sampling interval along x axis                       
c               dx  ----   sampling interval along y axis                       
c                w  ----   circular frequency (in radians)                      
c            cstep  ----   compensation depth step (in m or ft)                 
c            dstep  ----   extrapolation depth step (in m or ft)                
c                v  ----   velocity used in compensation (in m/s or f   t/s)    
c                n  ----   migration order used in w-x migration                
c         alpha(n)  ----   alpha values used in w-x migration                   
c          beta(n)  ----   beta values used in w-x migration                    
c            trick  ----   1/6 trick value used in w-x migration                
c       caux(naux)  ----   complex auxiliary array                              
c             naux  ----   max0(nkx,nky)                                        
c  output:                                                                      
c   cp(nkx,nky)     ----   wave field for given frequency w (p(kx,ky,w))        
c                             compensated                                       
c                                                                               
c                                                                               
        subroutine errcom(cp,nkx,nky,dx,dy,w,cstep,dstep,v,
     1                    n,alpha,beta,trick,caux,naux)

        complex cp(nkx,nky), cerr
	complex cerry, cerrx
        complex caux(naux)
        real alpha(n),beta(n),trick
        real dx,dy,cstep,w,v,dstep
        real ax,bx,ay,by,errr,err1,err2,err3,amp
        integer nkx,nky,n

ccc	real tmpr, tmpi
ccccccc  compute some constants 

        nkxq = nkx/2+1                                                       
        nkyq = nky/2+1                                                       
        if ( nkx .gt. 1 ) then                                               
              dx2 = dx*dx                                                       
              odx2 = 1./dx2                                                     
              dkx = 2.*3.141592654/(nkx*dx)                                     
        else                                                                 
              dx2 = 0.                                                          
              odx2 = 0.                                                         
              dkx = 0.                                                          
        end if                                                               
        if ( nky .gt. 1 ) then                                               
              dy2 = dy*dy                                                       
              ody2 = 1./dy2                                                     
              dky = 2.*3.141592654/(nky*dy)                                     
        else                                                                 
              dy2 = 0.                                                          
              ody2 = 0.                                                         
              dky = 0.                                                          
        end if                                                               
        wv = w/v                                                                
        wv2 = wv*wv                                                             
        vw = v/w                                                                
        vw2 = vw*vw                                                             
        vwh = .5   * vw                                                         
        if ( dstep .gt. 0. ) then                                               
           fm = cstep/dstep                                                     
        else                                                                    
           fm = 0.                                                              
        end if                                                                  

        if ( n .eq. 1 ) then                                                    
cccccccc                                                                        
           al = alpha(1)                                                        
           bl = beta(1)                                                         
c....ky=0                                                                       
           fkxcut = wv/dkx
           itmpx = fkxcut
           ikxcut = min0(itmpx,nkxq-1)
           if ( cstep .eq. 0. ) goto 10000
           do ikx=2,ikxcut
              fkx = (ikx-1)*dkx 
              fkx2 = fkx*fkx
              tmp = wv2 - fkx2
              fkx2n = 2.*(1.-cos(fkx*dx))*odx2
              ax = (1. - (trick*dx2+bl*vw2)*fkx2n)
              bx = al*vwh*fkx2n*dstep
	      amp = ax*ax + bx*bx
	      if(amp.eq.0.) then
		cerr = cmplx(1.,0.)
	      else 
		amp = 1./amp
                cerr = cmplx((ax*ax-bx*bx)*amp,2.*ax*bx*amp)
	      end if
              cerr = cerr**fm 
              err1 = (sqrt(tmp) - wv)*cstep
              if(err1.ne.0.) cerr = cerr*cmplx(cos(err1),sin(err1))
              caux(ikx) = cerr
              cp(ikx,1) = cp(ikx,1)*cerr
           end do
           do ikx=2,ikxcut
              ikxs = nkx-ikx+2
              cp(ikxs,1) = cp(ikxs,1)*caux(ikx)
           end do
10000      continue
           do ikx=ikxcut+1,nkxq-1
              cp(ikx,1) = (0.,0.)
           end do
           do ikx=ikxcut+1,nkxq-1
              ikxs = nkx-ikx+2
              cp(ikxs,1) = (0.,0.)
           end do
           cp(nkxq,1) = (0.,0.)
c....ky=nyquist
           if (nky.eq.1) return
           do ikx=1,nkx
              cp(ikx,nkyq) = (0.,0.)
           end do
c....other ky's 
           fkycut = wv/dky
           itmpy = fkycut
           ikycut = min0(itmpy,nkyq-1)
           do iky=2,ikycut
              fky = (iky-1)*dky
              fky2 = fky*fky
              fky2n = 2.*(1.-cos(fky*dy))*ody2
              ikys = nky-iky+2
              ay = (1. - (trick*dy2+bl*vw2)*fky2n)
              by = al*vwh*fky2n*dstep
	      amp = ay*ay + by*by
	      if(amp.eq.0.) then
		cerry = cmplx(1.,0.)
	      else 
		amp = 1./amp
                cerry = cmplx((ay*ay-by*by)*amp,2.*ay*by*amp)
	      end if
              tmp = wv2 - fky2
c..........kx=0                                                                 
              cerr = cerry**fm
              err2 = (sqrt(tmp) - wv)*cstep
              if(err2.ne.0.) cerr = cerr*cmplx(cos(err2),sin(err2))
              cp(1,iky) = cp(1,iky)*cerr
              cp(1,ikys) = cp(1,ikys)*cerr
c..........kx=nyquist                                                           
              if ( nkxq .gt. 1 ) then                                           
                 cp(nkxq,iky) = (0.,0.)                                         
                 cp(nkxq,ikys) = (0.,0.)                                        
              end if                                                            
c..........other kx's                                                           
              fkxcut = sqrt(tmp)/dkx
              itmpx =  fkxcut                                                   
              ikxcut = min0(itmpx,nkxq-1)                                       
              if ( cstep .eq. 0. ) goto 20000
              do ikx=2,ikxcut
                 fkx = (ikx-1)*dkx
                 fkx2 = fkx*fkx
                 tmp2 = tmp - fkx2
                 fkx2n = 2.*(1.-cos(fkx*dx))*odx2
                 ax = (1. - (trick*dx2+bl*vw2)*fkx2n)
                 bx = al*vwh*fkx2n*dstep
	         amp = ax*ax + bx*bx
	         if(amp.eq.0.) then
		   cerr = cmplx(1.,0.)
	         else 
		   amp = 1./amp
                   cerr = cmplx((ax*ax-bx*bx)*amp,2.*ax*bx*amp)
	         end if
                 cerr = cerr*cerry
                 cerr = cerr**fm
                 err3 = (sqrt(tmp2) - wv)*cstep
                 if(err3.ne.0.) cerr=cmplx(cos(err3),sin(err3))*cerr
                 caux(ikx) = cerr
                 cp(ikx,iky) = cp(ikx,iky)*cerr
                 cp(ikx,ikys)=cp(ikx,ikys)*cerr
              end do                                                            
              do ikx=2,ikxcut
                 ikxs = nkx-ikx+2
                 cp(ikxs,ikys)=cp(ikxs,ikys)*caux(ikx)
                 cp(ikxs,iky)=cp(ikxs,iky)*caux(ikx)
              end do
20000         continue                                                          
              do ikx=ikxcut+1,nkxq-1
                  cp(ikx,iky) = cmplx(0.,0.)
                  cp(ikx,ikys)=(0.,0.)
              end do
              do ikx=ikxcut+1,nkxq-1
                  ikxs = nkx - ikx + 2
                  cp(ikxs,ikys)=(0.,0.)
                  cp(ikxs,iky)=(0.,0.)
              end do
           end do
           do iky=ikycut+1,nkyq-1
              ikys = nky - iky + 2
              do ikx=1,nkx
                 cp(ikx,iky) = (0.,0.)
                 cp(ikx,ikys) = (0.,0.)
              end do
           end do
cccc  compensation for n>1                                                      
        else

        do 3000 iky=1,nkyq                                                      
            fky = (iky-1)*dky                                                   
            fky2 = fky*fky                                                      
            fky2n = 2.*(1.-cos(fky*dy))*ody2                                    
            ikys = 0                                                            
            if(iky.gt.1 .and. iky.lt.nkyq) ikys = nky-iky+2                     
            do 2500 ikx=1,nkxq                                                  
               fkx = (ikx-1)*dkx                                                
               fkx2 = fkx*fkx                                                   
               tmp = wv2 - fkx2 - fky2                                          
               ikxs = 0                                                         
               if (ikx.gt.1 .and. ikx.lt.nkxq ) ikxs = nkx-ikx+2                

               if (tmp .ge. 0.) then 
                  fkx2n = 2.*(1.-cos(fkx*dx))*odx2                              
                  cerr = cmplx(1.,0.)                                           
                  do i=1,n
                    ax = (1. - (trick*dx2+beta(i)*vw2)*fkx2n)
                    bx = alpha(i)*vwh*fkx2n*dstep            
                    ay = (1. - (trick*dy2+beta(i)*vw2)*fky2n)
                    by = alpha(i)*vwh*fky2n*dstep 
	             amp = ax*ax + bx*bx
	             if(amp.eq.0.) then
		        cerrx = cmplx(1.,0.)
	             else 
		        amp = 1./amp
                        cerrx = cmplx((ax*ax-bx*bx)*amp,2.*ax*bx*amp)
	             end if
c		     call msgsci("i =", i)
c		     call msgscf("ax =", ax)
c		     call msgscf("bx =", bx)
c		     call msgscf("amp =", amp)

	             amp = ay*ay + by*by

	             if(amp.eq.0.) then
			amp = 0. 
		        cerry = cmplx(1.,0.)
		     else 
		        amp = 1./amp
			cerry = cmplx((ay*ay-by*by)*amp,2.*ay*by*amp)
	             end if
c		     call msgscf("ay =", ay)
c		     call msgscf("by =", by)
c		     call msgscf("amp =", amp)


c			tmpr = real(cerrx)
c		     call msgscf("real cerrx=", tmpr)
c			tmpi = aimag(cerrx)
c		     call msgscf("imag cerrx=", tmpi)
c			tmpr = real(cerry)
c		     call msgscf("real cerry=", tmpr)
c			tmpi = aimag(cerry)
c		     call msgscf("imag cerry=", tmpi)
		     cerr = cerr * cerrx * cerry
c			tmpr = real(cerr)
c		     call msgscf("real cerr=", tmpr)
c			tmpi = aimag(cerr)
c		     call msgscf("imag cerr=", tmpi)
		  end do
                  cerr = cerr**fm                                               
                  errr = (sqrt(tmp) - wv)*cstep                                 
                  if(errr.ne.0.) cerr=cmplx(cos(errr),sin(errr))*cerr 
                  cp(ikx,iky) = cp(ikx,iky)*cerr                                


                 if (ikys.ne.0 .and. ikxs.eq.0) then                           
                                   cp(ikx,ikys)=cp(ikx,ikys)*cerr              
                 else if (ikys.eq.0 .and. ikxs.ne.0) then                      
                                 cp(ikxs,iky)=cp(ikxs,iky)*cerr                
                 else if (ikys.ne.0 .and. ikxs.ne.0) then                      
                                 cp(ikxs,ikys)=cp(ikxs,ikys)*cerr              
                                 cp(ikx,ikys)=cp(ikx,ikys)*cerr                
                                 cp(ikxs,iky)=cp(ikxs,iky)*cerr                
                 end if 
               else                                                             
                 cp(ikx,iky) = cmplx(0.,0.)                                    
                 if (ikys.ne.0 .and. ikxs.eq.0) then                           
                    cp(ikx,ikys)=cmplx(0.,0.)                                  
                 else if (ikys.eq.0 .and. ikxs.ne.0) then                      
                    cp(ikxs,iky)=cmplx(0.,0.)                                  
                 else if (ikys.ne.0 .and. ikxs.ne.0) then                      
                    cp(ikxs,ikys)=cmplx(0.,0.)                                 
                    cp(ikx,ikys)=cmplx(0.,0.)                                  
                    cp(ikxs,iky)=cmplx(0.,0.)                                  
                 end if                                                        
               end if      
 2500      continue                                                             
 3000   continue                                                                
        end if                                                                  
        return                                                                  
        end                                                                     
ccccc                                                                           
c compute alpha and beta used in w-x migration                                  
c                                                                               
c  input:                                                                       
c    iorder    ---   order of migration/modeling operator                       
c                    1=65 degree                                                
c                    2=80 degree                                                
c                    3=85 degree                                                
c                    4=89 degree                                                
c                    5=90 degree                                                
c                    0=45 degree                                                
c                   -1=15 degree                                                
c    n         ---   dimension of alpha and beta array; =max(iorder,1)          
c  output:                                                                      
c    alpha(n)  ---   alpha values used in migration/modeling                    
c    beta(n)   ---   beta values used in migration/modeling                     
c                                                                               
       subroutine alpbe(iorder,alpha,beta,n)                             
       real alpha(n),beta(n)                                                    
       integer iorder,n                
       if ( iorder .eq. 1 ) then                                                
cccc 65-degrees                                                                 
          alpha(1) = 0.47824060                                      
          beta(1) =  0.376369527                                     
       else if ( iorder .eq. 2 ) then                                           
cccc 80-degrees                                                                 
          alpha(1) = 0.040315157                                                
          alpha(2) = 0.457289566                                                
          beta(1) =  0.873981642                                                
          beta(2) =  0.222691983                                                
       else if ( iorder .eq. 3 ) then                                           
cccc 85-degrees                                                                 
          alpha(1) = 0.004210420                                                
          alpha(2) = 0.081312882                                                
          alpha(3) = 0.414236605                                                
          beta(1) =  0.972926132                                                
          beta(2) =  0.744418059                                                
          beta(3) =  0.150843924                                                
       else if ( iorder .eq. 4 ) then                                           
cccc 89-degrees                                                                 
          alpha(1) = 0.000523275                                                
          alpha(2) = 0.014853510                                                
          alpha(3) = 0.117592008                                                
          alpha(4) = 0.367013245                                                
          beta(1) =  0.994065088                                                
          beta(2) =  0.919432661                                                
          beta(3) =  0.614520676                                                
          beta(4) =  0.105756624                                                
       else if ( iorder .eq. 5 ) then                                           
cccc 90-degrees                                                                 
          alpha(1) = 0.000153427                                                
          alpha(2) = 0.004172967                                                
          alpha(3) = 0.033860918                                                
          alpha(4) = 0.143798076                                                
          alpha(5) = 0.318013812                                                
          beta(1) =  0.997370236                                                
          beta(2) =  0.964827992                                                
          beta(3) =  0.824918565                                                
          beta(4) =  0.483340757                                                
          beta(5) =  0.073588213                                                
       else if ( iorder .eq. 0 ) then                                           
cccc 45-degrees                                                                 
          alpha(1) = 0.5                                                        
          beta(1) = 0.25                                                        
       else if ( iorder .eq. -1 ) then                                          
cccc 15-degrees                                                                 
          alpha(1) = 0.5                                                        
          beta(1) =  0.                                                         
       end if                                                                   
       return                                                                   
       end                                                                      
