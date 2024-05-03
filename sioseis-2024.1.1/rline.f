      SUBROUTINE rline( lun )
!     RLINE READS A LINE OF INPUT FROM unit lun.
!  ********    The unit must have been opened by FORTRAN.   *********
!  THE LINE IS STORED IN LABELED COMMON SIOLN1 AS A CHARACTER STRING.
!  THE STRING IS CLEARED TO BLANKS PRIOR TO READ.
!
!  COMMON variable nchars is the column count of the last non
!  blank character on the line.  nchars 0 means it is a blank
!  line.  nchars < 1 means EOF was detected.
!
!  WRITTEN AND COPYRIGHTED (C) BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 29 FEBRUARY 1984
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  mod Feb 97 - add wline
!  mod 21 Dec 98 - Remove optional printing (conflict with rdline)
!  mod 19 Jul 02 - Add entry rline1 to use different common blocks.
!
      INTEGER maxc, lun
      PARAMETER (MAXC=200)
      COMMON /SIOLN1/ CBUF
      COMMON /sioln2/ ICHAR, NCHARS, iprint, lunpo
      INTEGER ichar, nchars, iprint, i, lunpo
      CHARACTER*200 CBUF

      nchars = -1
   10 CBUF(1:maxc)=' '
      READ (lun,20,END=100,ERR=100) CBUF(1:MAXC)
      nchars = 0
   20 FORMAT(A200)
      DO 30 i=maxc,1,-1
         IF( cbuf(i:i) .NE. ' ') THEN
             nchars=i
             GOTO 40
         ENDIF
   30 CONTINUE
   40 cbuf(nchars+1:nchars+1)=' '
      cbuf(nchars+2:nchars+2)=char(0)
!***   watch out for DOS lines that have cr/lf - blank the cr
      IF( cbuf(nchars-1:nchars-1) .EQ. CHAR(10) ) 
     &    cbuf(nchars-1:nchars-1) = ' '
      IF( cbuf(nchars-1:nchars-1) .EQ. CHAR(13) ) 
     &    cbuf(nchars-1:nchars-1) = ' '
      IF( cbuf(nchars:nchars) .EQ. CHAR(10) ) 
     &    cbuf(nchars:nchars) = ' '
      IF( cbuf(nchars:nchars) .EQ. CHAR(13) ) 
     &    cbuf(nchars:nchars) = ' '
!      IF( iprint .EQ. 1 ) PRINT *,cbuf(1:nchars)
      ichar = 1
  100 RETURN

      ENTRY wline( lun )
!    Write nchars+2 characters (includes a blank and null)
!  in A format to Fortran unit lun
      WRITE( lun, '(A)' ) cbuf(1:nchars+2)
      RETURN
      END

      SUBROUTINE rline1( lun )
!**** Use rline1 if rline is being used by someone else.  This allows
!**** rdline and rline to coexist without rline using rdline's common.
!**** Use getoke1 to get the tokens from rline1 lines (common sioln3).
!     RLINE READS A LINE OF INPUT FROM unit lun.
!  ********    The unit must have been opened by FORTRAN.   *********
!  THE LINE IS STORED IN LABELED COMMON SIOLN1 AS A CHARACTER STRING.
!  THE STRING IS CLEARED TO BLANKS PRIOR TO READ.
!
!  COMMON variable nchars is the column count of the last non
!  blank character on the line.  nchars 0 means it is a blank
!  line.  nchars < 1 means EOF was detected.
!
!  WRITTEN AND COPYRIGHTED (C) BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 29 FEBRUARY 1984
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  mod Feb 97 - add wline
!  mod 21 Dec 98 - Remove optional printing (conflict with rdline)
!  mod Jul 02 - Add entry rline1
!
      INTEGER maxc, lun
      PARAMETER (MAXC=200)
      COMMON /SIOLN3/ CBUF
      COMMON /sioln4/ ICHAR4, NCHARS4, iprint4, lunpo4
      INTEGER ichar4, nchars4, iprint4, i, lunpo4
      CHARACTER*200 CBUF

      nchars4 = -1
   10 CBUF(1:maxc)=' '
      READ (lun,20,END=100,ERR=200) CBUF(1:MAXC)
      nchars4 = 0
   20 FORMAT(A200)
      DO 30 i=maxc,1,-1
         IF( cbuf(i:i) .NE. ' ') THEN
             nchars4=i
             GOTO 40
         ENDIF
   30 CONTINUE
   40 cbuf(nchars4+1:nchars4+1)=' '
      cbuf(nchars4+2:nchars4+2)=char(0)
!***   watch out for DOS lines that have cr/lf - blank the cr
      IF( cbuf(nchars4-1:nchars4-1) .EQ. CHAR(10) ) 
     &    cbuf(nchars4-1:nchars4-1) = ' '
      IF( cbuf(nchars4-1:nchars4-1) .EQ. CHAR(13) ) 
     &    cbuf(nchars4-1:nchars4-1) = ' '
      IF( cbuf(nchars4:nchars4) .EQ. CHAR(10) ) 
     &    cbuf(nchars4:nchars4) = ' '
      IF( cbuf(nchars4:nchars4) .EQ. CHAR(13) ) 
     &    cbuf(nchars4:nchars4) = ' '
      IF( iprint4 .EQ. 1 ) PRINT *,cbuf(1:nchars4)
      ichar4 = 1
      RETURN
  100 CONTINUE
!  100 PRINT *,' END'
      RETURN
  200 CONTINUE
!      PRINT *,' rline err'
      RETURN
      END
