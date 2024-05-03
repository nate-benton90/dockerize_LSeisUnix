      SUBROUTINE GETOKE(CBUFO,NCHARS)
!  ****   Watch out - getoke writes a null after the last char,
!         so cbufo must be bigger than the number of characters
!
!   GETOKE RETURNS CONSECUTIVE TOKENS (ITEMS BETWEEN A DELIMITER), ONE PER CALL,
!   FROM CHARACTER STRING in COMMON/sioln1/. AN ALPHA STRING IS RETURNED IN
!   TOKEN WHEN IT STARTS AND ENDS WHEN SINGLE QUOTES.  (THE QUOTES ARE NOT
!   RETURNED.  THE STRING MUST BE TERMINATED WITH A QUOTE AND A BLANK, SO THAT
!   QUOTES MAY BE INCUDED IN THE STRING SO LONG AS THE QUOTE IS NOT FOLLOWED BY
!   A BLANK).
!       nchars will be a zero if end of line is detected or an comment character
!   is detected.  A comment character is either ! (VMS & Cray), { (Apollo), 
!   # (Unix). Any of theses comment characters will work on any machine!
!
!  AGUMENTS:
!    CBUFO  - THE CHARACTER*(*) STRING SET BY GETOKE CONTAINING THE NEXT TOKEN
!             FOUND.  CBUFO MUST BE NCHARS+1 CHARACTERS LONG (SINCE C STRINGS
!             MUST BE TERMINATED WITH A NULL).
!    NCHARS - THE NUMBER OF CHARACTERS IN THE TOKEN RETURNED IN CBUFO. A 0
!             (ZERO) NUMBER OF CHARACTERS MEANS THAT NO TOKEN WAS FOUND AND
!             THAT ANOTHER LINE SHOULD BE READ AND GETOKE CALLED AGAIN WITH
!             jchar=1.
!
!
!
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 29 FEBRUARY 1984
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  mod 25 Aug 98 - Make a NULL be a delimiter
!  mod 19 Jul 02 - Add entry getoke1 to go along with rline1, using different
!                  common blocks.
!  mod 2 May 04 - Add entry getokec for getoke with a comma delimiter.
!
      CHARACTER*1 CDELIM,QUOTE,tab, null
      CHARACTER*200 CBUFIN
      COMMON /SIOLN1/ CBUFIN
      COMMON /sioln2/ jchar,NCBUF
      CHARACTER* (*) CBUFO
      DATA CDELIM/' '/, QUOTE/''''/

      cbufo = ' '
      NCHARS=0                                                          ! COUNT THE NON BLANK CHARACTERS IN THE TOKEN
      IQUOTE=0                                                          ! COUNT THE QUOTES IN THE TOKEN
      tab = CHAR(9)                                                     ! the tab character
      null = CHAR(0)                                                    ! NULL
      IF(jchar.LT.1) jchar=1
   10 CONTINUE
      IF( CBUFIN(jchar:jchar) .NE. CDELIM .AND. 
     *   cbufin(jchar:jchar) .NE. null .AND.
     *   cbufin(jchar:jchar) .NE. tab ) GOTO 20                         ! STRIP OFF LEADING BLANKS
      jchar=jchar+1
      IF(jchar.GT.NCBUF) RETURN
      GO TO 10
   20 IF(CBUFIN(jchar:jchar).NE.QUOTE) GO TO 30                         ! IS IT A QUOTE?
      ISTART=jchar+1                                                    ! STRIP OF THE LEADING QUOTE
      IQUOTE=1                                                          ! SIGNAL THAT THE STRING STARTED WITH A QUOTE
      GO TO 40
   30 CONTINUE
      IF( cbufin(jchar:jchar) .EQ. '!' .OR. 
     *    cbufin(jchar:jchar) .EQ. '{' .OR.
     *    cbufin(jchar:jchar) .EQ. '#' ) THEN
             nchars = 0
             GOTO 100
      ENDIF
!**** toss out non ASCII characters
      IF( ICHAR(cbufin(jchar:jchar)) .LT. 32 ) THEN
          jchar = jchar + 1
          GOTO 10
      ENDIF
      ISTART=jchar                                                      ! THE FIRST CHARACTER OF THE TOKEN TO BE RETURNED
   40 CONTINUE                                                          ! NOW FIND THE END OF THE TOKEN
      IF(jchar.GT.NCBUF) GO TO 110                                      ! ARE WE AT THE END OF THE BUFFER?
      IF(CBUFIN(jchar:jchar).EQ.CDELIM.AND.IQUOTE.NE.1) GOTO 100        ! WAS IT A BLANK?
      IF(CBUFIN(jchar:jchar).EQ. tab .AND. IQUOTE.NE.1) GOTO 100        ! WAS IT A BLANK?
      IF(CBUFIN(jchar:jchar).EQ. null .AND. IQUOTE.NE.1) GOTO 100       ! WAS IT A NULL?
      jchar=jchar+1
      IF( CBUFIN(jchar:jchar) .EQ. QUOTE .AND. IQUOTE .EQ. 1 ) THEN
          IQUOTE = 2
          GOTO 100
      ENDIF
      NCHARS=NCHARS+1                                                   ! THE CURREN CHARACTER IS NOT A BLANK OR A QUOTE
      GO TO 40                                                          ! GO LOOK AT THE NEXT CHARACTER
  100 CONTINUE
      IF( NCHARS .EQ. 0 ) RETURN                                        ! DON'T TRY TO MOVE ZERO CHARACTERS!!
      IF(IQUOTE.ne.1) GO TO 110                                         ! THE CURRENT CHARCTER IS A BLANK WITHIN QUOTES
      jchar=jchar+1
      nchars=nchars+1
      go to 40
!****  end the returned string with a null character so that c rcognizes
!**** the end of sting!!
  110 IF( nchars .GT. 0 ) CBUFO(1:NCHARS)=CBUFIN(ISTART:jchar)
      cbufo(nchars+1:nchars+1) = null
      jchar=jchar+1                                                     ! STRIP OFF THE BLANK
      RETURN
      END

      SUBROUTINE GETOKE1(CBUFO,NCHARS)
!**** Use rline1 if rline is being used by someone else.  This allows
!**** rdline and rline to coexist without rline using rdline's common.
!**** Use getoke1 to get the tokens from rline1 lines (common sioln3).
!
!   GETOKE RETURNS CONSECUTIVE TOKENS (ITEMS BETWEEN A DELIMITER), ONE PER CALL,
!   FROM CHARACTER STRING in COMMON/sioln1/. AN ALPHA STRING IS RETURNED IN
!   TOKEN WHEN IT STARTS AND ENDS WHEN SINGLE QUOTES.  (THE QUOTES ARE NOT
!   RETURNED.  THE STRING MUST BE TERMINATED WITH A QUOTE AND A BLANK, SO THAT
!   QUOTES MAY BE INCUDED IN THE STRING SO LONG AS THE QUOTE IS NOT FOLLOWED BY
!   A BLANK).
!       nchars will be a zero if end of line is detected or an comment character
!   is detected.  A comment character is either ! (VMS & Cray), { (Apollo), 
!   # (Unix). Any of theses comment characters will work on any machine!
!
!  AGUMENTS:
!    CBUFO  - THE CHARACTER*(*) STRING SET BY GETOKE CONTAINING THE NEXT TOKEN
!             FOUND.  CBUFO MUST BE NCHARS+1 CHARACTERS LONG (SINCE C STRINGS
!             MUST BE TERMINATED WITH A NULL).
!    NCHARS - THE NUMBER OF CHARACTERS IN THE TOKEN RETURNED IN CBUFO. A 0
!             (ZERO) NUMBER OF CHARACTERS MEANS THAT NO TOKEN WAS FOUND AND
!             THAT ANOTHER LINE SHOULD BE READ AND GETOKE CALLED AGAIN WITH
!             jchar=1.
!
!
!
!  WRITTEN AND COPYRIGHTED BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 29 FEBRUARY 1984
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
!  mod 25 Aug 98 - Make a NULL be a delimiter
!
      CHARACTER*1 CDELIM,QUOTE,tab, null
      CHARACTER*200 CBUFIN
      COMMON /SIOLN3/ CBUFIN
      COMMON /sioln4/ jchar,NCBUF
      CHARACTER* (*) CBUFO
      DATA CDELIM/' '/, QUOTE/''''/

      cbufo = ' '
      NCHARS=0                                                          ! COUNT THE NON BLANK CHARACTERS IN THE TOKEN
      IQUOTE=0                                                          ! COUNT THE QUOTES IN THE TOKEN
      tab = CHAR(9)                                                     ! the tab character
      null = CHAR(0)                                                    ! NULL
      IF(jchar.LT.1) jchar=1
   10 CONTINUE
      IF( CBUFIN(jchar:jchar) .NE. CDELIM .AND. 
     *   cbufin(jchar:jchar) .NE. null .AND.
     *   cbufin(jchar:jchar) .NE. tab ) GOTO 20                         ! STRIP OFF LEADING BLANKS
      jchar=jchar+1
      IF(jchar.GT.NCBUF) RETURN
      GO TO 10
   20 IF(CBUFIN(jchar:jchar).NE.QUOTE) GO TO 30                         ! IS IT A QUOTE?
      ISTART=jchar+1                                                    ! STRIP OF THE LEADING QUOTE
      IQUOTE=1                                                          ! SIGNAL THAT THE STRING STARTED WITH A QUOTE
      GO TO 40
   30 CONTINUE
      IF( cbufin(jchar:jchar) .EQ. '!' .OR. 
     *    cbufin(jchar:jchar) .EQ. '{' .OR.
     *    cbufin(jchar:jchar) .EQ. '#' ) THEN
             nchars = 0
             GOTO 100
      ENDIF
!**** toss out non ASCII characters
      IF( ICHAR(cbufin(jchar:jchar)) .LT. 32 ) THEN
          jchar = jchar + 1
          GOTO 10
      ENDIF
      ISTART=jchar                                                      ! THE FIRST CHARACTER OF THE TOKEN TO BE RETURNED
   40 CONTINUE                                                          ! NOW FIND THE END OF THE TOKEN
      IF(jchar.GT.NCBUF) GO TO 110                                      ! ARE WE AT THE END OF THE BUFFER?
      IF(CBUFIN(jchar:jchar).EQ.CDELIM.AND.IQUOTE.NE.1) GOTO 100        ! WAS IT A BLANK?
      IF(CBUFIN(jchar:jchar).EQ. tab .AND. IQUOTE.NE.1) GOTO 100        ! WAS IT A BLANK?
      IF(CBUFIN(jchar:jchar).EQ. null .AND. IQUOTE.NE.1) GOTO 100       ! WAS IT A NULL?
      jchar=jchar+1
      IF( CBUFIN(jchar:jchar) .EQ. QUOTE .AND. IQUOTE .EQ. 1 ) THEN
          IQUOTE = 2
          GOTO 100
      ENDIF
      NCHARS=NCHARS+1                                                   ! THE CURREN CHARACTER IS NOT A BLANK OR A QUOTE
      GO TO 40                                                          ! GO LOOK AT THE NEXT CHARACTER
  100 CONTINUE
      IF( NCHARS .EQ. 0 ) RETURN                                        ! DON'T TRY TO MOVE ZERO CHARACTERS!!
      IF(IQUOTE.ne.1) GO TO 110                                         ! THE CURRENT CHARCTER IS A BLANK WITHIN QUOTES
      jchar=jchar+1
      nchars=nchars+1
      go to 40
!****  end the returned string with a null character so that c rcognizes
!**** the end of sting!!
  110 IF( nchars .GT. 0 ) CBUFO(1:NCHARS)=CBUFIN(ISTART:jchar)
      cbufo(nchars+1:nchars+1) = null
      jchar=jchar+1                                                     ! STRIP OFF THE BLANK
      RETURN
      END


      SUBROUTINE GETOKEC(CBUFO,NCHARS)
!     This is the same as GETOKE, except COMMA is also a delimiter.
!   ( I don't know what will break if I just add it to geoke, so make
!   it a unique entry point).
!  mod 25 Aug 98 - Make a NULL be a delimiter
!  mod 2 May 98 - Make a COMMA be a delimiter
!               - Don't honor the comment convention.
! mod 22 July 2007 - Look out for ,, - vacuous field
!
      CHARACTER*1 CDELIM,QUOTE,tab, null, comma
      CHARACTER*200 CBUFIN
      COMMON /SIOLN1/ CBUFIN
      COMMON /sioln2/ jchar,NCBUF
      CHARACTER* (*) CBUFO
      DATA CDELIM/' '/, QUOTE/''''/, comma/','/

      cbufo = ' '
      NCHARS=0                                                          ! COUNT THE NON BLANK CHARACTERS IN THE TOKEN
!****   look out for vacuous field ( ,, )
      IF( cbufin(jchar:jchar) .EQ. comma ) GOTO 110
      IQUOTE=0                                                          ! COUNT THE QUOTES IN THE TOKEN
      tab = CHAR(9)                                                     ! the tab character
      null = CHAR(0)                                                    ! NULL
      IF(jchar.LT.1) jchar=1
   10 CONTINUE
      IF( CBUFIN(jchar:jchar) .NE. CDELIM .AND. 
     *   cbufin(jchar:jchar) .NE. null .AND.
     *   cbufin(jchar:jchar) .NE. comma .AND.
     *   cbufin(jchar:jchar) .NE. tab ) GOTO 20                         ! STRIP OFF LEADING BLANKS
      jchar=jchar+1
      IF(jchar.GT.NCBUF) RETURN
      GO TO 10
   20 IF(CBUFIN(jchar:jchar).NE.QUOTE) GO TO 30                         ! IS IT A QUOTE?
      ISTART=jchar+1                                                    ! STRIP OF THE LEADING QUOTE
      IQUOTE=1                                                          ! SIGNAL THAT THE STRING STARTED WITH A QUOTE
      GO TO 40
   30 CONTINUE
!**** toss out non ASCII characters
      IF( ICHAR(cbufin(jchar:jchar)) .LT. 32 ) THEN
          jchar = jchar + 1
          GOTO 10
      ENDIF
      ISTART=jchar                                                      ! THE FIRST CHARACTER OF THE TOKEN TO BE RETURNED
   40 CONTINUE                                                          ! NOW FIND THE END OF THE TOKEN
      IF(jchar.GT.NCBUF) GO TO 110                                      ! ARE WE AT THE END OF THE BUFFER?
      IF(CBUFIN(jchar:jchar).EQ.CDELIM.AND.IQUOTE.NE.1) GOTO 100        ! WAS IT A BLANK?
      IF(CBUFIN(jchar:jchar).EQ. tab .AND. IQUOTE.NE.1) GOTO 100        ! WAS IT A BLANK?
      IF(CBUFIN(jchar:jchar).EQ. null .AND. IQUOTE.NE.1) GOTO 100       ! WAS IT A NULL?
      IF(CBUFIN(jchar:jchar).EQ. comma .AND. IQUOTE.NE.1) GOTO 100       ! WAS IT A COMMA?
      jchar=jchar+1
      IF( CBUFIN(jchar:jchar) .EQ. QUOTE .AND. IQUOTE .EQ. 1 ) THEN
          IQUOTE = 2
          GOTO 100
      ENDIF
      NCHARS=NCHARS+1                                                   ! THE CURREN CHARACTER IS NOT A BLANK OR A QUOTE
      GO TO 40                                                          ! GO LOOK AT THE NEXT CHARACTER
  100 CONTINUE
      IF( NCHARS .EQ. 0 ) RETURN                                        ! DON'T TRY TO MOVE ZERO CHARACTERS!!
      IF(IQUOTE.ne.1) GO TO 110                                         ! THE CURRENT CHARCTER IS A BLANK WITHIN QUOTES
      jchar=jchar+1
      nchars=nchars+1
      go to 40
!****  end the returned string with a null character so that c rcognizes
!**** the end of sting!!
  110 IF( nchars .GT. 0 ) CBUFO(1:NCHARS)=CBUFIN(ISTART:jchar)
      cbufo(nchars+1:nchars+1) = null
      jchar=jchar+1                                                     ! STRIP OFF THE BLANK
      RETURN
      END
!
!
      SUBROUTINE GETOKE1C(CBUFO,NCHARS)
!     This is the same as GETOKE, except COMMA is also a delimiter.
!   ( I don't know what will break if I just add it to geoke, so make
!   it a unique entry point).
!  mod 25 Aug 98 - Make a NULL be a delimiter
!  mod 2 May 98 - Make a COMMA be a delimiter
!               - Don't honor the comment convention.
! mod 22 July 2007 - Look out for ,, - vacuous field
!
      CHARACTER*1 CDELIM,QUOTE,tab, null, comma
      CHARACTER*200 CBUFIN
      COMMON /SIOLN3/ CBUFIN
      COMMON /sioln4/ jchar,NCBUF
      CHARACTER* (*) CBUFO
      DATA CDELIM/' '/, QUOTE/''''/, comma/','/

      cbufo = ' '
      NCHARS=0                                                          ! COUNT THE NON BLANK CHARACTERS IN THE TOKEN
!****   look out for vacuous field ( ,, )
      IF( cbufin(jchar:jchar) .EQ. comma ) GOTO 110
      IQUOTE=0                                                          ! COUNT THE QUOTES IN THE TOKEN
      tab = CHAR(9)                                                     ! the tab character
      null = CHAR(0)                                                    ! NULL
      IF(jchar.LT.1) jchar=1
   10 CONTINUE
      IF( CBUFIN(jchar:jchar) .NE. CDELIM .AND. 
     *   cbufin(jchar:jchar) .NE. null .AND.
     *   cbufin(jchar:jchar) .NE. comma .AND.
     *   cbufin(jchar:jchar) .NE. tab ) GOTO 20                         ! STRIP OFF LEADING BLANKS
      jchar=jchar+1
      IF(jchar.GT.NCBUF) RETURN
      GO TO 10
   20 IF(CBUFIN(jchar:jchar).NE.QUOTE) GO TO 30                         ! IS IT A QUOTE?
      ISTART=jchar+1                                                    ! STRIP OF THE LEADING QUOTE
      IQUOTE=1                                                          ! SIGNAL THAT THE STRING STARTED WITH A QUOTE
      GO TO 40
   30 CONTINUE
!**** toss out non ASCII characters
      IF( ICHAR(cbufin(jchar:jchar)) .LT. 32 ) THEN
          jchar = jchar + 1
          GOTO 10
      ENDIF
      ISTART=jchar                                                      ! THE FIRST CHARACTER OF THE TOKEN TO BE RETURNED
   40 CONTINUE                                                          ! NOW FIND THE END OF THE TOKEN
      IF(jchar.GT.NCBUF) GO TO 110                                      ! ARE WE AT THE END OF THE BUFFER?
      IF(CBUFIN(jchar:jchar).EQ.CDELIM.AND.IQUOTE.NE.1) GOTO 100        ! WAS IT A BLANK?
      IF(CBUFIN(jchar:jchar).EQ. tab .AND. IQUOTE.NE.1) GOTO 100        ! WAS IT A BLANK?
      IF(CBUFIN(jchar:jchar).EQ. null .AND. IQUOTE.NE.1) GOTO 100       ! WAS IT A NULL?
      IF(CBUFIN(jchar:jchar).EQ. comma .AND. IQUOTE.NE.1) GOTO 100       ! WAS IT A COMMA?
      jchar=jchar+1
      IF( CBUFIN(jchar:jchar) .EQ. QUOTE .AND. IQUOTE .EQ. 1 ) THEN
          IQUOTE = 2
          GOTO 100
      ENDIF
      NCHARS=NCHARS+1                                                   ! THE CURREN CHARACTER IS NOT A BLANK OR A QUOTE
      GO TO 40                                                          ! GO LOOK AT THE NEXT CHARACTER
  100 CONTINUE
      IF( NCHARS .EQ. 0 ) RETURN                                        ! DON'T TRY TO MOVE ZERO CHARACTERS!!
      IF(IQUOTE.ne.1) GO TO 110                                         ! THE CURRENT CHARCTER IS A BLANK WITHIN QUOTES
      jchar=jchar+1
      nchars=nchars+1
      go to 40
!****  end the returned string with a null character so that c rcognizes
!**** the end of sting!!
  110 IF( nchars .GT. 0 ) CBUFO(1:NCHARS)=CBUFIN(ISTART:jchar)
      cbufo(nchars+1:nchars+1) = null
      jchar=jchar+1                                                     ! STRIP OFF THE BLANK
      RETURN
      END
