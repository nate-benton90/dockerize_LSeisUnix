      SUBROUTINE resaex( buf, lbuf, ibuf, iscr, scr )
!
!  ARGUMENTS:
!  buf   - The trace, with SEGY header as TYPE REAL
!  lbuf  - The trace, with SEGY header as TYPE INTEGER*4
!  ibuf  - The trace, with SEGY header as TYPE INTEGER*2
!  scr   - A scratch array.
!
!  COPYRIGHT (C) The Regents of the University of California
!  ALL RIGHTS RESERVED.  
!  Written by Paul Henkart, SIO, 16 October 1991
!  mod 12 June 95 - SEG-Y trace header value for sample rate was wrong!
!  mod 25 Jan. 96 - Do it in the "ap" if that's where the data is!
!  mod 1 Jun 05 - Add warning if decimating by 2 or 4.
!  mod 18 Jul 08 - Use nsamps from numdat rather than segy because of 16 bit
!  mod 19 May 11 - Didn't work when data was in ap
!
      DIMENSION buf(1), lbuf(1), ibuf(1), iscr(1), xa(10), scr(1)
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
     *                 isecptr,  igmtptr,  ldelsptr,  lsmusptr,lemusptr,
     *                 lsisptr,  lwbtsptr, lgatptr,  lssmsptr, lesmsptr,
     *                 lsbptr,   ifoldptr, icvleptr, lespnptr
      COMMON /apmem/ apdata(32767)
      COMMON /transp/ x(32000)                                          ! the transpose array!
      COMPLEX x
      COMMON /resamp/ dtout, lprint, type, order
      INTEGER type, order
      LOGICAL first
      SAVE
      DATA first/.TRUE./
!
!
      nin = ibuf(isampptr)
      dtin = buf(lsisptr)                                               ! sample interval in seconds
      IF( first ) THEN
          first = .FALSE.
          IF( dtout / dtin .EQ. 2 .OR. dtout / dtin .EQ. 4 ) THEN
              PRINT *,' ***   WARNING   ***  DISKIN/DISKOX can be used 
     &with DECIMF ',INT(dtout / dtin)
          ENDIF
          IF( dtin .EQ. dtout ) THEN
              PRINT *,' ***  WARNING  ***  Trace not resampled.'
              PRINT *,' Input sample interval equals output interval',
     &                dtin
              RETURN
          ENDIF
      ENDIF
      IF( IAND(lprint,2) .NE. 0 ) THEN
         PRINT *,' numdat=',numdat,' dtin=',dtin,' dtout=',dtout
      ENDIF
      IF( type .EQ. 2 ) GOTO 1000
!****
!****   DO OSU   IMSL  frequency domain interpolation here
!****
! first make sure input has an even number of sample
!
!        nin = nin / 2
!        nin = nin * 2
!
! now calculate output parameters
!
!        nout=(nin)*(dtin/dtout)
!        nout=nout/2
!        nout=nout*2
!
! now recalculate dtout to allow for truncation
!
!        dtout=(nin)*dtin/(nout)
!      IF( ibuf(itridptr) .EQ. 2 ) GOTO 1200                             ! forget dead traces
!        fnyquistin=1.e0/(2.e0*dtin)
!        fnyquistout=1.e0/(2.e0*dtout)
!
! give some printout
!        IF( IAND(lprint,2) .NE. 0 ) THEN
!            write(*,*)'Input  nyquist',nin,dtin,fnyquistin
!            write(*,*)'Output nyquist',nout,dtout,fnyquistout
!        ENDIF
!
! do fft with IMSL routine
!        IF( iuseap .NE. 0 .AND. in .NE. 0 ) THEN
!            call FFTRC (apdata(in),nin,X,IWORK,WORK)
!        ELSE
!            call FFTRC (buf(numhdr+1),nin,X,IWORK,WORK)
!        ENDIF
!
! as a check on what your doing check on power at nyquist
!
!        nd2=nout/2
!        power=0
!        do 100 j=1,nd2
!  100    power=power+abs(X(j))**2
!        power=power/nd2
!        powern=abs(X(nd2))*2
!        IF( IAND(lprint,2) .NE. 0 ) THEN
!            write(*,*)' Average power of input series ',power
!            write(*,*)' power at nyquist ',powern
!        ENDIF
!
! now add in extra points if necessary
!
!        if(nout.gt.nin) then
!         ni2=nin/2
!         do 110 j=ni2+1,nd2
!  110      x(j)=(0.0e0,0.0e0)
!        endif
!
! now fill up last half of time series with complex conjugate of first
!
!        do 120 j=2,nd2
!  120     x(nout+2-j)=conjg(x(j))
!
! now do inverse transform by taking conjugate and doing fft
!         do 130 j=1,nout
!  130     x(j)=conjg(x(j))
!         call FFTCC(X,nout,IWORK,WORK)
!         do 140 j=1,nout
!          X(j)=conjg(X(j))/nin
!          buf(numhdr+j)=real(x(j))
!  140    continue 
!         GOTO 1200
!****
!****   Do  time domain interpolation here
!****
 1000 CONTINUE
      nsamps = numdat
      si = buf(lsisptr)
      index = 1
      nout = nsamps * si / dtout - order
      DO 1050 i = 1, nout
         DO j = index, index+order-1
 1010       xa(j-index+1) = FLOAT(index+(j-index)-1) * si
         ENDDO
         xx =  FLOAT(i-1) * dtout
         IF( IAND(lprint,2) .NE. 0 )
     &    PRINT *,' nout=',nout,' si=',si,' xx=',xx,' order =',order
         IF( in .NE. 0 ) THEN
             CALL polint( xa, apdata(in+index-1), order, xx, scr(i), dy)
         ELSE
             CALL polint( xa, buf(numhdr+index), order, xx, scr(i), dy )
         ENDIF
!      print *,' index=',index,' i=',i,FLOAT(index+1)*si,FLOAT(i+1)*dtout
 1020    IF( FLOAT(index)*si .LE. FLOAT(i)*dtout ) THEN
             index = index + 1
             IF( index .GT. nsamps ) GOTO 1050
             GOTO 1020
         ENDIF
 1050 CONTINUE
      IF( in .NE. 0 ) THEN
          DO  i = 1, nout
              apdata(in+i-1) = scr(i)
          ENDDO
      ELSE
          DO  i = 1, nout
             buf(numhdr+i) = scr(i)
          ENDDO
      ENDIF
 1100 CONTINUE
 1200 ibuf(isampptr) = nout
      CALL long2ushort( nout, ibuf(isampptr) )
      numdat = nout
      buf(lsisptr) = dtout
      ibuf(isiptr) = dtout*1000000.
      RETURN
!****
!****
      END
