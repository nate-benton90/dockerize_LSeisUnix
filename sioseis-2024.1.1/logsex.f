      SUBROUTINE logsex( n, buf, lbuf, ibuf, scr)
!
!  ARGUMENTS:
!  buf   - The trace, with SEGY header as TYPE REAL
!  lbuf  - The trace, with SEGY header as TYPE INTEGER*4
!  ibuf  - The trace, with SEGY header as TYPE INTEGER*2
!  scr   - A scratch array.
!
!  COPYRIGHT (C) Seismic Reflection Processors, Solana Beach, CA. 92075
!  ALL RIGHTS RESERVED.  
!  Written by Graham M. Kent, SRP, 16 September 1991
!  mod 1 Apr 09 - Use numdat in common for nsamps rather than the header.
!
      DIMENSION buf(1), lbuf(1111), ibuf(1111), scr(11111)
      INTEGER*2 ibuf
      COMMON /readt/ itunit, numhdr, numdat, ihunit, ireeln, jntrcs,
     *               ifmt, nskip, secs, lrenum, isrcf, idtype,
     *               nfskip, jform, itxsi, itxdel, nfktrc, norigtr
      COMMON /sioap/ iasgnd, irelse, in, iout, nextad, lapsiz, ifree,
     *     iuseap
      COMMON /segyptr/ llsegptr, lrseqptr, lshotptr, lshtrptr, lrpnptr,
     *                 lrptrptr, itridptr, ldisptr,  lwbdptr,  lsxcoptr,
     *                 lrxcoptr, idelmptr, istmptr,  iendmptr, isampptr,
     *                 isiptr,   iyrptr,   idayptr,  ihrptr,   iminptr,
     *                 isecptr,  igmtptr,  ldelsptr, lsmusptr,lemusptr,
     *                 lsisptr,  lwbtsptr, lgatptr,  lssmsptr, lesmsptr,
     *                 lsbptr,   ifoldptr, icvleptr, lespnptr
      COMMON /apmem/ a(32767)
      DIMENSION params1(8), lparams1(8), xdata(10000), ydata(10000),
     *yderv(10000)
      EQUIVALENCE (params1(1),lparams1(1)) 
      INTEGER logtype
      COMMON /logcom/ ilogunit1, ilogunit2, nloglists, nlogwrds    
      SAVE  
      DATA mlists1/0/, mlists2/0/,xdata/10000*0./, ydata/10000*0./,
     *yderv/10000*0./
!
!
      IF( ibuf(itridptr) .EQ. 2 ) RETURN                                ! forget dead traces              
!*GMK Have we read a list for logst1, if not get buf variables once - write over them
      IF (mlists1.eq.0) THEN 
!           nsamps1 = ibuf(isampptr)
           nsamps1 = numdat
           delay1 = buf(ldelsptr)
           si1 = buf(lsisptr)                                                 ! sample interval in seconds
           wbt1 = buf(lwbtsptr)                                               ! water bottom time in seconds
           goto 10         
      ELSE                          
!*GMK Have we read a list for logst2, if not get buf variables once - write over them
         IF (mlists2.eq.0) THEN  
!           nsamps2 = ibuf(isampptr)
           nsamps2 = numdat
           delay2 = buf(ldelsptr)
           si2 = buf(lsisptr)                                                 ! sample interval in seconds
           wbt2 = buf(lwbtsptr)                                               ! water bottom time in seconds
           goto 10
         ELSE
           goto 11
         ENDIF
      ENDIF

!*GMK Checks logstretch and reads appropriate disk file
10    IF( n .EQ. 1 ) THEN
          CALL podisc( ilogunit1, 1, 0 ) 
          CALL rddisc( ilogunit1, params1, nlogwrds, istat )
          mlists1 = mlists1 + 1
          logtype1  = lparams1(1)
          rtsamp11  = params1(2)
          rtsamp21  = params1(3)
          rtcut1    = params1(4)
          rsltime1  = params1(5)
          reltime1  = params1(6)
          rloghz1   = params1(7)
          lprint1   = lparams1(8) 
!*GMK Lprint statement n=1
         IF( lprint1 .EQ. 1 ) THEN
          PRINT *,'  type=',logtype1,' tsamp1=',rtsamp11,' tsamp2=',
     &rtsamp22,' tcut=',rtcut1,' sltime=',rsltime1,' eltime=',reltime1,
     &' loghz=', rloghz1
         ENDIF                                  
 
      ELSE  
          CALL podisc( ilogunit2, 1, 0 ) 
          CALL rddisc( ilogunit2, params1, nlogwrds, istat )
          mlists2 = mlists2 + 1
          logtype2  = lparams1(1)
          rtsamp12  = params1(2)
          rtsamp22  = params1(3)
          rtcut2    = params1(4)
          rsltime2  = params1(5)
          reltime2  = params1(6)
          rloghz2   = params1(7)
          lprint2   = lparams1(8)
!*GMK Lprint statement n=2           
         IF( lprint2 .EQ. 1 ) THEN
          PRINT *,'  type=',logtype2,' tsamp1=',rtsamp12,' tsamp2=',
     &rtsamp22,' tcut=',rtcut2,' sltime=',rsltime2,' eltime=',reltime2,
     &' loghz=', rloghz2
         ENDIF                                  
       
      ENDIF  

!*GMK Determine what type of log stetch is needed  
!*    And get appropriate variables which are generalized

11    if( n .eq. 1 ) then
          logtype =  logtype1 
          rtsamp1 =  rtsamp11 
          rtsamp2 =  rtsamp21 
          rtcut   =  rtcut1   
          rsltime =  rsltime1   
          reltime =  reltime1 
          rloghz  =  rloghz1  
          lprint  =  lprint1  
        goto 500
      else 
          logtype =  logtype2
          rtsamp1 =  rtsamp12 
          rtsamp2 =  rtsamp22 
          rtcut   =  rtcut2   
          rsltime =  rsltime2   
          reltime =  reltime2 
          rloghz  =  rloghz2  
          lprint  =  lprint2  
        goto 501
      endif 


!*GMK Going to stretch the data prior to DMO
!*    *******************PROCESS LOGST1*********************************** 

500   continue

!*GMK Going to log stretch the data and ensure that TCUT is 
!*    used so as to not take the LN of zero. Data are then 
!*    place from array buf into ydata for spline routine and
!*    new stretched times are calculated for array xdata

      if (rtcut.lt.delay1) then 
         do 100 i = 1, nsamps1
          t = delay1 + (i-1)*(si1/1.0)
          xdata(i) = log(t/rtcut)
!*GMK     Is data in AP 
          if (in.eq.0) then
           ydata(i) = buf(i+numhdr)
          else
           ydata(i) = a(i)
          endif
100   continue
      nspline = nsamps1
      slog = xdata(1)
      elog = xdata(nspline) 
      else  
                                    
!*GMK IF tcut is greater, how much data do I need to skip before tcut - jump

          jump = nint( ( (rtcut-delay1)*1.0 )/si1) +1 
         do 110 j = jump, nsamps1
          t = delay1 + (j-1)*(si1/1.0)
          xdata(j-jump+1) = log(t/rtcut)
!*GMK     Is data in AP 
          if (in.eq.0) then
            ydata(j-jump+1) = buf(j+numhdr)   
          else
            ydata(j-jump+1) = a(j)  
          endif 

110   continue
      nspline = (nsamps1-jump) + 1
      slog = xdata(1)
      elog = xdata(nspline)   
      endif            

!*GMK Big requires the natural spline to be taken (i= -1 and n+1 are 0)
      big = 1.0E+31   

!*GMK Subroutine spline calculates the necessary 2nd derevitives 
      call spline(xdata,ydata,nspline,big,big,yderv)  
                        
!*GMK Once the partials are calculated, we need to spline the data on
!*    to the new trace of sample interval tsamp1. New Trace will start a time 
!*    t = 0 (going to FK anyways) and final time given by stretch function.
                
!*GMK Zero scratch array SCR to place final interpolated trace values
      do 120 k = 1, 10000
         scr(k) = 0.0
120   continue                                                       

!*GMK Find first sample greater than first value for log stretch, and do the same
!*    for largest value but firsat sample less than greatest log time value.

      islog = nint( (slog*1.0)/rtsamp1 ) + 1
      ielog = nint( (elog*1.0)/rtsamp1 )         

!*GMK Calculate new value of y at every new evenly spaced x starting at time
!*    corresponding to sample islog
         
      do 200 n = islog, ielog                   
       x = (rtsamp1/1.0)*(n-1)
       call splint(xdata,ydata,yderv,nspline,x,y)
       scr(n) = y
200   continue

      do 210 i = 1, ielog  
!*GMK     Is data in AP 
       if (in.eq.0) then
        buf(i+numhdr) = scr(i)
       else
        a(i) = scr(i)
       endif
210   continue
                               
!*GMK Now change header values (Ouch that's dangerous) so the next PROCESS
!*    has a clue to the tweeked trace.  

      ibuf(isampptr)  = ielog
      numdat = ielog
      buf(ldelsptr)   = 0.0
      ibuf(idelmptr)  = 0
      buf(lsisptr)    = rtsamp1
      ibuf(isiptr)    = rtsamp1*1.0E+06
!*GMK finished leave process logstr1
      goto 999                      




!*GMK Enter at 501 if data needs to be compressed subsquent to DMO
501   continue

!*GMK Going to log compress the data.  Data are then        
!*    place from array buf into ydata for spline routine and
!*    new compressed times are calculated for array xdata                   

!*    *******************PROCESS LOGSTR2************************************

!*    This case should not happen because trace should have no delay after DMO
!*    but you never know!

      if (rtcut.lt.delay2) then 
         do 1000 i = 1, nsamps2
          tau = delay2 + (i-1)*(si2/1.0)
          xdata(i) = rtcut*exp(tau)
!*GMK     Is data in AP   
          if (in.eq.0) then
           ydata(i) = buf(i+numhdr)
          else
           ydata(i) = a(i)                      
          endif
1000  continue
      nspline = nsamps2
      slog = xdata(1)
      elog = xdata(nspline) 
      else                                 
     
!*GMK rtcut greater than delay
!*    This should always be true for this process

         do 1100 j = 1, nsamps2       
C*GMK    t is really tau here because of compression
          tau = delay2 + (j-1)*(si2/1.0)
          xdata(j) = rtcut*exp(tau) 
!*GMK     Is data in AP   
          if (in.eq.0) then
           ydata(j) = buf(j+numhdr)  
          else
           ydata(j) = a(j)    
          endif
1100  continue
      nspline = nsamps2  
      slog = xdata(1)
      elog = xdata(nspline)
      endif            

!*GMK Big requires the natural spline to be taken (i= -1 and n+1 are 0)
      big = 1.0E+31
!*GMK Subroutine spline calculates the necessary 2nd derevitives
      call spline(xdata,ydata,nspline,big,big,yderv)

!*GMK Once the partials are calculated, we need to spline the data on
!*    to the new trace of sample interval tsamp1. New Trace will start a time 
!*    t = 0 (going to FK anyways) and final time given by stretch function.
                
!*GMK Zero scratch array SCR to place final interpolated trace values
      do 1200 k = 1, 10000
         scr(k) = 0.0
1200  continue                                                       

!*GMK Find first sample greater than frist value for log stretch, and do the same
!*    for largest value but firsat sample less than greatest log time value.

      islog = nint( (slog)/rtsamp2 ) + 1
      ielog = nint( (elog)/rtsamp2 ) 

!*GMK Calculate new value of y at every new evenly spaced x starting at time
!*    corresponding to sample islog
         
      do 2000 n = islog, ielog                   
       x = (rtsamp2/1.0)*(n-1)
       call splint(xdata,ydata,yderv,nspline,x,y)   
       scr(n) = y            
2000  continue

!*GMK Account for new tsamp rate, start and end times 
!*    Going to have to tweeked the vales in common blks also to be consistent

      istart =  nint ( (rsltime-delay2)*(1.0/rtsamp2) ) + 1
      ifinish = nint ( (reltime-rsltime)*(1.0/rtsamp2) )
     &+istart
      do 2100 i = istart, ifinish    
!*GMK     Is data in AP   
          if (in.eq.0) then
           buf(i-istart+1+numhdr) = scr(i)
          else
           a(i-istart+1) = scr(i)  
          endif
2100  continue
                               
!*GMK Now change header values (Ouch that's dangerous) so the next PROCESS
!*    has a clue to the tweeked trace.  

      numdat = (ifinish-istart) + 1
      ibuf(isampptr)  = numdat
      buf(ldelsptr)   = rsltime
      ibuf(idelmptr)  = rsltime*1000
      buf(lsisptr)    = rtsamp2
      ibuf(isiptr)    = rtsamp1*1.0E+06                 
!*GMK finished leave process logstr2
      goto 999                      

999   continue
      return
      end


      subroutine spline(x,y,n,yp1,ypn,y2)
      parameter (nmax=10000)
      dimension x(n),y(n),y2(n),u(nmax)
      if (yp1.gt..99e30) then
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
      if (ypn.gt..99e30) then
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
      if (h.eq.0.) THEN
         PRINT *,'****  ERROR  ****  bad xa input.'
         CALL EXIT
      ENDIF
      a=(xa(khi)-x)/h
      b=(x-xa(klo))/h
      y=a*ya(klo)+b*ya(khi)+
     *      ((a**3-a)*y2a(klo)+(b**3-b)*y2a(khi))*(h**2)/6.
      return
      end

