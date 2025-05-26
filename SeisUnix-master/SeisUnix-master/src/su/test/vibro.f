      program vibro
c
c Generates a Vibroseis sweep (linear FM sweep)
c M. Dietrich, CWP, 10/10
c
      parameter (ntm = 8192, pi = 3.1415926535898, pi2 = pi + pi)
      dimension vs(ntm), wt(ntm)
c
c.... Input parameters
c
      f1 = 10.
      f2 = 40.
      tv = 20.
      t1 = 5.
      t2 = 10.
      dt = 0.002
c
c....
c
      write (*,*) char(7)
      write (*,*) char(13)
      nt  = tv/dt
        if (nt.gt.ntm) stop '*** Too many time samples ***'
      ak  = (f2 - f1)/tv
      mt1 = int(t1/dt)
      mt2 = int(t2/dt)
      mt3 = nt - mt2 + 1
c
      if (f1.lt.f2) then
        write (*,*) tv," seconds, ",f1," -",f2," Hz Up-Sweep"
      else
        write (*,*) tv," seconds, ",f1," -",f2," Hz Down-Sweep"  
      end if      
c
      do 10 it=1,nt
        vs(it) = 0.
	wt(it) = 0.
 10   continue
c
c.... Tapering function (cosine bells) 
c
	am1 = 0.
	am2 = 0.
      do 15 it=1,nt
        if (mt1.ne.0) am1 = float(mt1-it)/float(mt1)
        if (mt2.ne.0) am2 = float(it-mt3)/float(mt2)
        if (it.lt.mt1)               wt(it) = (1. + cos(pi*am1))/2.
        if (it.ge.mt1.and.it.le.mt3) wt(it) =  1.
        if (it.gt.mt3)               wt(it) = (1. + cos(pi*am2))/2.
 15   continue
c
c.... Vibroseis signal
c
      do 20 it=1,nt
        tvb = float(it-1)*dt
	vs(it) = wt(it)*cos(2*pi*(f1 + ak*tvb/2.)*tvb)
 20   continue
c
      open (10,file='vibro.data',form='unformatted',status='new')
        write (10) (vs(it),it=1,nt)
c
      stop
      end
