      PROGRAM sio2hp
!   Convert 2 color sioseis plot files to a single HP-RTL file.
!
!  Copyright (C) 1992, 1993, 1994 The Regents of the University of California
!  Paul Henkart, Scripps Institution of Oceanography, La Jolla, Ca. 92093
!
!   mod 28 June 1993 - Use the HPGL/2 PS command on long plots.
!   mod 9 Nov '93 - Put the PS immediate after BP and before any pen
!                   commands, else PS is ignored.
!  mod 5 Jan 94 - Add second input file option.
!  mod28 Feb 94 - Add rotate
!               - Allow n plots to be plotted
!               - move parameter stuff to subroutine getparams.f
!  mod 18 Aug 95 - Add parameter header and print of SEG-Y header
!  mod 24 Oct 95 - Eliminate the color parameter and do an automatic
!                  determination of whether it's a color plot or not.
!  mod 4 Jan 96 - Remove the PJL command, which was documented when it
!                 was added!  It confuses the DesignJet.  LaserJet needs
!                 it, but doesn't have RTL!.
!  mod 6 Feb 96 - Do the SEG-Y header correctly!  'Tis hard!
!                 If length < width in PS command, HPGL rotates the
!                 coordinates.  RO-90 is the same as RO0.  RO270 then
!                 makes the label upside down.
!  mod 6 Oct 96 - The blank fill when plots are a different length is
!                 different on color plots than on B&W plots!
!  mod 23 May 97 - Fiddle with plot width if the side label is longer than
!                 the plot.
!               - Add side label annotation file
!  mod 16 June 98 - Multiple color files didn't work.
!  mod 26 Jun 98 - Add parameter MARGIN.
!  mod 3 Sept 99 - Check for IPATH being a Sun raster file
!  mod 29 Feb 00 - Check for nibs being 2848, 2849, 2958, 2959
!  mod 12 June 00 - byte swap for Intel & Dec
!  mod 4 Oct 01 - Add gray scale plotting
!  mod 4 Nov 04 - Multiple color plots were bad.
!  mod 27 Dec 06 - Color determination was bad due to use of (i) and not in loop
!  mod 31 Dec 06 - Do automatic byte swap determination.
!                  DETERMINE COLOR O ALL PLOTS FROM THE FIRST ONE.
!  mod 23 Feb 10 - gfortran didn't like num_lines being REAL
!  mod 29 Apr 10 - Allow 44 inch Z2100 and 24 inch Z2100
!
!    sioseis has color being on or off.   gray is an intensity of
!    all colors.  When sioseis does gray, it uses the color as being
!    the intensity of r g b.  We need to reassemble the color raster
!    rows into a single 3 bit pixel that's an index into an intensity array.
!    The HP palette says:
!    black = 0, 0, 0
!    80% = 26, 26, 26
!    70% = 77, 77, 77
!    60% = 102, 102, 102
!    50% = 127, 127, 127
!    40% = 153, 153, 153
!    30% = 178, 178, 178
!    20% = 204, 204, 204
!    10% = 233, 233, 233
!    white = 255, 255, 255
!    sioseis says:
!    0    255 255 255         gray7     white
!    1    219 219 219         gray6
!    2    181 181 181         gray5
!    3    145 145 145         gray4
!    4    110 110 110         gray3
!    5     71  71  71         gray2
!    6     36  36  36         grey1
!    7      0   0   0         gray0     black
!
!   iwrdcount comes from trplot and is the length (32 bit words) of the
!            seismic image.
!   nbytesin is the number of bytes trplot wrote for each raster line.
!   trplot always writes a full plot image to disk though.  We will
!      throw away the unused portion of the image.  When plotting 2
!      plots, read the second one into the buffer after the seismic
!      plot, not after the whole image!
!
!  RTL is part of PCL
!  PCL commands start with ESC %
!  HPGL commands are 2 letters followed by other stuff
     
      INTEGER*2 ibuf(5000)    
      INTEGER*4 lbuf(1)
      EQUIVALENCE (ibuf(1),lbuf(1)), (token(1:1),ibuf(1))
      CHARACTER*100 token
      INTEGER luno, ilun(9), istat(9), nbits(9), length(9), i, j, k
      INTEGER nbytesin(9), nbytesout, iwrdcount(9), hpgl_length, nibs(9)
      INTEGER nplots, ro, index, width, nlines, itemp, iblank
      INTEGER side_label, sllun, nbmargin, margin
      LOGICAL color, lprint, header, gray/.FALSE./
      COMMON /sioln1/ cbuf
      CHARACTER*200 cbuf
      COMMON /sioln2/ ichar1, nchars
      INTEGER ichar1, nchars, icompt
      INTEGER dpi/300/
      REAL rmargin
      DATA nbytesin/9*1312/, color/.FALSE./, nplots/2/, lprint/.FALSE./
      DATA ilun/9*0/,luno/0/, nbytesout/0/, width/0/, nlines/0/,
     &     length/9*0/, nbits/9*0/, iblank/0/, icompt/0/

      CALL version
      IF( is_big_endian() .LT. 0 ) icompt = 2
      CALL getparams( ilun, luno, nplots, ro, lprint, header, sllun,
     &     rmargin )
      IF( ilun(1) .EQ. 0 ) THEN
          PRINT *,' IPATH must be given.'
          CALL EXIT(0)
      ENDIF
      IF( luno .EQ. 0 ) THEN
          PRINT *,' OPATH must be given.'
          CALL EXIT(0)
      ENDIF
!**** ESC E = RESET PCL/HP-RTL and HP-GL/2 defaults
      token(1:1) = CHAR(27)
      token(2:2) = 'E'
      CALL wrdiscb( luno, token, 2 )
      IF( lprint ) PRINT *,token(2:2)
      DO 10 i = 1, nplots
         CALL rddiscb( ilun(i), ibuf, 80, istat(i) )
!        make sure the sucker isn't a Sun rasterfile.
         IF( lbuf(1) .EQ. 1504078485 ) THEN                              ! in hex = 59a66a95
             PRINT *,' *** WARNING ***  IPATH appears to be a Sun file.'
             PRINT *,' IPATH must be an SIO file (plot OPATH).'
         ENDIF
!             35-58 = date the plot was created
!                65 = g if gray scale
!             66-70 = dots per inch.
!             71-74 = number of 32 bit words in every raster line
!             75-80 = number of raster lines
         IF( token(65:65) .EQ. 'g' ) gray = .TRUE.
         READ( token,'(66X,I4,I4,I6)' ) nibs(i), iwrdcount(i), length(i)
         IF( lprint ) THEN
             PRINT *,token
            PRINT *,' i=',i,' nibs=',nibs(i),' iwrdcount=',iwrdcount(i),
     &               ' length=',length(i)
         ENDIF
         IF( nibs(i) .NE. 2847 .AND.  nibs(i) .NE. 2848 .AND.
     &        nibs(i) .NE. 2124 .AND.  nibs(i) .NE. 2144 .AND.
     &        nibs(i) .NE. 2858 .AND.  nibs(i) .NE. 2859 ) THEN
             PRINT *,' ***  ERROR  ***  Bad SIOSEIS plot file.'
             PRINT *, ' SIOSEIS plot parameter NIBS must be 2124, 2144, 
     &2847, 2848, 2958, or 2859'
             STOP
         ENDIF
         IF( nibs(i) .EQ. 2124 ) nbytesin(i) = 2024  ! 1012 * 2
         IF( nibs(i) .EQ. 2144 ) nbytesin(i) = 3252  ! 1626 * 2
         IF( nibs(i) .EQ. 2847 .OR. nibs(i) .EQ. 2858 ) nbytesin(i) =900
         IF( nibs(i) .EQ. 2848 .OR. nibs(i) .EQ. 2859 ) 
     &       nbytesin(i) = 1312
         IF( nibs(i) .EQ. 300 ) nbytesin(i) = 318
         IF( lprint ) PRINT *, gray, nibs(i), iwrdcount(i), length(i),
     &        nbytesin(i)
         nbits(i) = iwrdcount(i) * 32
         IF( nibs(i) .EQ. 2124 .OR. nibs(1) .EQ. 2144 ) dpi = 600
         margin = rmargin * dpi     ! convert inches to dots
         margin = margin / 32 * 32
         nbmargin = margin / 8
         width = width + nbits(i) + margin
         nlines = MAX0(length(i),nlines)
         nbytesout = nbytesout + (nbits(i)+margin) / 8
         CALL podiscb( ilun(i), 1, 3520 )
   10 CONTINUE
!****
!****   shi*.   Assume all plots are either color or B&W.  i.e.
!****  DO NOT MIX B&W AND COLOR PLOTS
!****   Determine color/b&w from the first plot.
!****
!****  check for color on all plotters
!      IF( nibs(1) .EQ. 2858 .OR. nibs(1) .EQ. 2859 ) THEN
!        if there are 3 planes it must be a color plot!
!         nlines * bits per lin / 8 bits per byte * 2 planes
          CALL podiscb( ilun(1), 1, length(1)*nbytesin(1)*2 )
          CALL rddiscb( ilun(1), ibuf, 2, i )
          IF( i .EQ. 2 ) THEN
              color = .TRUE.
              iblank = -1
          ENDIF
!      ENDIF
      IF( lprint ) PRINT *,' color ',color
!****
!**** ESC %0B = Enter HP-GL/2  (see page 15 of RTL manual)
!**** BP5,1 = Turnoff autorotation
      token(1:1) = CHAR(27)
!     BP = Begin Plot
!     PU = Pen UP
!     PA = Plot Absolute
      token(2:11) = '%0B;BP5,1;'
      CALL wrdiscb( luno, token, 11 )
      IF( lprint ) PRINT *,token(2:11)
      IF( ro .NE. 0 ) THEN
          token(1:1) = CHAR(27)
          WRITE( token(2:7), 50 ) ro
   50     FORMAT ( 'RO', I3.3, ';' )
          CALL wrdiscb( luno, token, 7 )
          IF( lprint ) PRINT *,token(2:7)
      ENDIF
!****
!****  Set up all the HP-GL and HP RTL commands
!****
!****
!****   set the orientation to portrait
!****
      IF( nibs(i) .EQ. 300 ) THEN
          token(1:1) = CHAR(27)
          token(2:5) = '&10O'
          CALL wrdiscb( luno, token, 5 )
          IF( lprint ) PRINT *,token(2:5)
!****
!****   set the top margin to 0
!****
          token(1:1) = CHAR(27)
          token(2:5) = '&10E'
          CALL wrdiscb( luno, token, 5 )
          IF( lprint ) PRINT *,token(2:5)
      ENDIF
!     PS (plot size) length in HPGL/2 defaults to 1.5 * media length
!     The format is:  PS(length(,width);)
!     The units are plotter units (1016 dots per inch)
!     Assume that the sioseis length was calculated in 300 dots per inch
!             (or 600dpi if 2124 or 2144)
!     Must be before any pen commands!
!     The plot might be narrower than the side label, so make the HPGL
!     width wide enough to hold it.  There are 44 lines in the SEG-Y header.
!     The font size is set later with SI.1425,.1875  (width,height) but
!     seems to be .085 in length and .15 in width.  There's a 1 inch
!     margin too (PA8000,1020 below).
      hpgl_length = MAX0(length(1),length(2)) * 1016 / dpi + 1016
      IF( header ) THEN
          hpgl_length = hpgl_length + 8000
          itemp = width * 1016 / dpi
          side_label = NINT(44. * .15 * 1016. + 1016.)
          IF( itemp .LT. side_label ) itemp = side_label
          IF( hpgl_length .LT. itemp ) hpgl_length = itemp
          WRITE( token, 60 ) hpgl_length, itemp
   60     FORMAT ( 'PS', I7.7, ',', I7.7, ';' ) 
          CALL wrdiscb( luno, token, 18 )
      ELSEIF( sllun .NE. 0 ) THEN
          hpgl_length = hpgl_length + 8000
          REWIND sllun
          num_lines = 0
          DO i = 1, 1000
             CALL rline( sllun )
             IF( nchars .LT. 0 ) GOTO 61
             num_lines = num_lines + 1
          ENDDO
  61      itemp = width * 1016 / dpi
          temp = FLOAT(num_lines) * .15 * 1016. + 1016.
          side_label = NINT(temop)
          itemp = MAX0(itemp,side_label)
!         This will force the length to be at least the width
          hpgl_length = MAX0(itemp,hpgl_length)
          WRITE( token, 60 ) hpgl_length, itemp
          CALL wrdiscb( luno, token, 18 )
      ELSE
          WRITE( token, 62 ) hpgl_length
   62     FORMAT ( 'PS', I7.7, ';' ) 
          CALL wrdiscb( luno, token, 10 )
      ENDIF
      IF( lprint ) PRINT *,token(1:10)
!****
!****  Plot the SEG-Y header as a separate plot
!****  Do the PS width = length.  Allows 80 columns and 53 rows.
!****
      IF( header ) THEN
          token(1:45)='SP1;PA8000,1020;RO180;LO3;SD;SI.1425,.1875;LB'
!                      1234567890123456789012345678901234567890123456789
          CALL wrdiscb( luno, token, 45 )
          itemp = 3520
          CALL podiscb( ilun(1), 1, 0 )
          DO i = 1, 44
             CALL rddiscb( ilun(1), token, 80, istat )
             token(81:81) = CHAR(10)
             token(82:82) = CHAR(13)
             CALL wrdiscb( luno, token, 82 )
          ENDDO
          token(1:1) = CHAR(3)                                         ! labels must terminate with ETX
          token(2:17) = 'PU;RO0;PA9500,0;'
!                       123456789012345678901234567890123456789
          CALL wrdiscb( luno, token, 17 )
      ELSEIF( sllun .NE. 0 ) THEN
          token(1:45)='SP1;PA8000,1020;RO180;LO3;SD;SI.1425,.1875;LB'
          CALL wrdiscb( luno, token, 45 )
          REWIND sllun
          DO 70 i = 1, num_lines
             CALL rline( sllun )
             cbuf(81:81) = CHAR(10)
             cbuf(82:82) = CHAR(13)
             CALL wrdiscb( luno, cbuf, 82 )
   70     CONTINUE
          token(1:1) = CHAR(3)                                         ! labels must terminate with ETX
          token(2:17) = 'PU;RO0;PA9500,0;'
!                       123456789012345678901234567890123456789
          CALL wrdiscb( luno, token, 17 )
      ELSE
!         PU = Pen UP
!         PA = Plot Absolute
          token(1:9) = 'PU;PA0,0;'
          CALL wrdiscb( luno, token, 9 )
          IF( lprint ) PRINT *,token(1:9)
      ENDIF
      CALL podiscb( ilun(1), 1, 3520 )
      token(1:1) = CHAR(27)
!     now "Enter PCL Mode" using HP-GL/2 penposition as CAP
      token(2:5) = '%1A;'
      CALL wrdiscb( luno, token, 5 )
      IF( lprint ) PRINT *,token(2:5)

!     set width in pixels
      token(1:1) = CHAR(27)
      WRITE( token(2:9), '(2H*r,I5.5,1HS)' ) nbytesout * 8
      CALL wrdiscb( luno, token, 9 )
      IF( lprint ) PRINT *,token(2:9)

!     No negative motion
      token(1:1) = CHAR(27)
      token(2:5) = '&a1N'
      CALL wrdiscb( luno, token, 5 )
      IF( lprint ) PRINT *,token(2:5)

!     set number of planes per row
      token(1:1) = CHAR(27)
      token(2:5) = '*v6W'
      CALL wrdiscb( luno, token, 5 )
      IF( lprint ) PRINT *,token(2:5)

!     byte 0 = 0
!     byte 1 = 0        row-by-row
      ibuf(1) = 0
      IF( color ) THEN
!         byte 2 = 3    number of planes per row
!         byte 3 = 8
          ibuf(2) = 3 * 256 + 8
      ELSE
!         byte 2 = 1    number of planes per row
!         byte 3 = 8
          ibuf(2) = 1 * 256 + 8
      ENDIF
!     byte 4 = 8 
!     byte 5 = 8
      ibuf(3) = 8 * 256 + 8
      IF( icompt .NE. 0 ) CALL swap16( ibuf(2), 2 )
      CALL wrdiscb( luno, token, 6 )

!     set graphics resolution
      token(1:1) = CHAR(27)
      token(2:7) = '*t300R'
      IF( dpi .EQ. 600 ) token(2:7) = '*t600R'
      CALL wrdiscb( luno, token, 7 )
      IF( lprint ) PRINT *,token(2:7)

!     Change the Default palette if grey scale
      IF( gray ) THEN
          token(1:1) = CHAR(27)
          token(2:5) = '*v0A'
          token(6:6) = CHAR(27)
          token(7:10) = '*v0B'
          token(11:11) = CHAR(27)
          token(12:15) = '*v0C'
          token(16:16) = CHAR(27)
          token(17:20) = '*v0I'
          CALL wrdiscb( luno, token, 20 )
          IF( lprint ) PRINT *,token(2:20)
          token(1:1) = CHAR(27)
          token(2:7) = '*v036A'
          token(8:8) = CHAR(27)
          token(9:14) = '*v036B'
          token(15:15) = CHAR(27)
          token(16:21) = '*v036C'
          token(22:22) = CHAR(27)
          token(23:26) = '*v1I'
          CALL wrdiscb( luno, token, 26 )
          IF( lprint ) PRINT *,token(2:26)
          token(1:1) = CHAR(27)
          token(2:7) = '*v071A'
          token(8:8) = CHAR(27)
          token(9:14) = '*v071B'
          token(15:15) = CHAR(27)
          token(16:21) = '*v071C'
          token(22:22) = CHAR(27)
          token(23:26) = '*v2I'
          CALL wrdiscb( luno, token, 26 )
          IF( lprint ) PRINT *,token(2:26)
          token(1:1) = CHAR(27)
          token(2:7) = '*v110A'
          token(8:8) = CHAR(27)
          token(9:14) = '*v110B'
          token(15:15) = CHAR(27)
          token(16:21) = '*v110C'
          token(22:22) = CHAR(27)
          token(23:26) = '*v3I'
          CALL wrdiscb( luno, token, 26 )
          IF( lprint ) PRINT *,token(2:26)
          token(1:1) = CHAR(27)
          token(2:7) = '*v145A'
          token(8:8) = CHAR(27)
          token(9:14) = '*v145B'
          token(15:15) = CHAR(27)
          token(16:21) = '*v145C'
          token(22:22) = CHAR(27)
          token(23:26) = '*v4I'
          CALL wrdiscb( luno, token, 26 )
          IF( lprint ) PRINT *,token(2:26)
          token(1:1) = CHAR(27)
          token(2:7) = '*v181A'
          token(8:8) = CHAR(27)
          token(9:14) = '*v181B'
          token(15:15) = CHAR(27)
          token(16:21) = '*v181C'
          token(22:22) = CHAR(27)
          token(23:26) = '*v5I'
          CALL wrdiscb( luno, token, 26 )
          IF( lprint ) PRINT *,token(2:26)
          token(1:1) = CHAR(27)
          token(2:7) = '*v219A'
          token(8:8) = CHAR(27)
          token(9:14) = '*v219B'
          token(15:15) = CHAR(27)
          token(16:21) = '*v219C'
          token(22:22) = CHAR(27)
          token(23:26) = '*v6I'
          CALL wrdiscb( luno, token, 26 )
          IF( lprint ) PRINT *,token(2:26)
          token(1:1) = CHAR(27)
          token(2:7) = '*v255A'
          token(8:8) = CHAR(27)
          token(9:14) = '*v255B'
          token(15:15) = CHAR(27)
          token(16:21) = '*v255C'
          token(22:22) = CHAR(27)
          token(23:26) = '*v7I'
          CALL wrdiscb( luno, token, 26 )
          IF( lprint ) PRINT *,token(2:26)
      ENDIF
    
!     Start Raster Graphics at logical page left bound
      token(1:1) = CHAR(27)
      token(2:5) = '*r0A'
      CALL wrdiscb( luno, token, 5 )
      IF( lprint ) PRINT *,token(2:5)

  100 CONTINUE
      IF( color ) THEN
          DO 300 i = 1, 2
             index = 1
             DO 200 j = 1, nplots
                IF( nbmargin .GT. 0 ) THEN
                    DO k = 0, nbmargin/4 - 1
                       lbuf(index+k) = -1
                    ENDDO
                    index = index + nbmargin/4
                ENDIF
                istat(j) = 0
                IF( ilun(j) .NE. 0 ) CALL rddiscb(
     &                        ilun(j),lbuf(index),nbytesin(j),istat(j))
                IF( istat(j) .NE. nbytesin(j) ) THEN
                    ilun(j) = 0
!                   Fill the buffer with white (rgb color 0 = black)
                    DO 150 k = index, index+nbytesin(j)/4-1
  150               lbuf(k) = -1
                ENDIF
                index = index + iwrdcount(j)
  200        CONTINUE
             token(1:1) = CHAR(27)
!****        when color, planes 1 & 2 start with esc*b####V
             WRITE( token(2:8),'(2H*b,I4.4,1HV)' ) nbytesout
             CALL wrdiscb( luno, ibuf(1), nbytesout+8 )
             IF( lprint ) PRINT *,token(2:8),' color ',i,' plot ',j,
     &          ' index=',index,' out=',nbytesout,
     &          ibuf(35),ibuf(36),ibuf(37),ibuf(38)
  300    CONTINUE
      ENDIF
      index = 1
      DO 400 j = 1, nplots
         IF( nbmargin .GT. 0 ) THEN
             DO k = 0, nbmargin/4 - 1
                lbuf(index+k) = iblank
             ENDDO
             index = index + nbmargin/4
         ENDIF
         istat(j) = 0
         IF( ilun(j) .NE. 0 ) THEN
             CALL rddiscb( ilun(j), lbuf(index), nbytesin(j), istat(j) )
             IF( istat(j) .NE. nbytesin(j) ) THEN
                ilun(j) = 0
                DO k = index, index+nbytesin(j)/4-1
  350           lbuf(k) = iblank
                ENDDO
             ENDIF
         ENDIF
         index = index + iwrdcount(j)
         DO k = 0, 4
            lbuf(index+k) = iblank
         ENDDO
  400 CONTINUE
      token(1:1) = CHAR(27)
!**** The last plane starts with esc*b####W
      WRITE( token(2:8), '(2H*b,I4.4,1HW)' ) nbytesout
      CALL wrdiscb( luno, ibuf(1), nbytesout+8 )
      IF( lprint ) PRINT *,token(2:8),' 400 color ',i,' plot ',j,
     &          ' index=',index,' out=',nbytesout,
     &          ibuf(35),ibuf(36),ibuf(37),ibuf(38)
      IF( ilun(1) + ilun(2) .NE. 0 ) GOTO 100
!****
!****    END OF PLOT FILE
!****     
      token(1:1) = CHAR(27)
      token(2:4) = '*rC'                                                 ! end raster graphics
      CALL wrdiscb( luno, token, 4 )
      IF( lprint ) PRINT *,token(2:4)
      token(1:1) = CHAR(27)
      token(2:4) = '%0B'                                                 ! re-enter HP-GL/2
      CALL wrdiscb( luno, token, 4 )
      IF( lprint ) PRINT *,token(2:4)
      token(1:3) = 'PG;'                                                 ! end and plot current page (in HP-GL/2)
      CALL wrdiscb( luno, token, 3 )
      IF( lprint ) PRINT *,token(1:3)
!****
!****   Laser Jet needs PCL junk
!****
      token(1:1) = CHAR(27)
      token(2:9) = '%-12345X'
!      CALL wrdiscb( luno, token, 9)
      CALL frefil( 2, ilun(1), istat(i) )
      CALL frefil( 2, ilun(2), istat(i) )
      CALL frefil( 2, luno, istat(i) )
    
      END

