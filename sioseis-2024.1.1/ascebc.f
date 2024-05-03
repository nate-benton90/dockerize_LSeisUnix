      SUBROUTINE ASCEBC( STRIN, NCHARS, STROUT ) 
!    ASCEBC CONVERTS ASCII CHARACTERS TO EBCDIC CHARACTERS.
!    UNKNOWN CHARACTERS ARE CONVERTED INTO QUESTION MARKS.
!
!  ARGUMENTS:
!     STRIN  - THE STRING OF CHARACTERS TO BE CONVERTED.  STRIN must be type
!              CHARACTER
!     NCHARS - THE NUMBER OF CHARACTERS TO CONVERT.
!     STROUT - THE OUTPUT STRING OF CHARACTERS. ISTROU MAY BE THE SAME AS
!              STRIN.  STROUT must be type CHARACTER.
!
!
!  WRITTEN AND COPYRIGHTED (C) BY:
!  PAUL HENKART, SCRIPPS INSTITUTION OF OCEANOGRAPHY, 3 MAY 1984
!  ALL RIGHTS ARE RESERVED BY THE AUTHOR.  PERMISSION TO COPY OR REPRODUCE THIS
!  SUBROUTINE, BY COMPUTER OR OTHER MEANS, MAY BE OBTAINED ONLY FROM THE AUTHOR.
!
      INTEGER IASCII(126)
      CHARACTER*(*) strin, strout
      DATA IASCII /00,01,02,03,55,45,47,22,05,37,11,12,03,14,15,
     *  16,17,18,19,60,61,50,38,24,25,63,39,28,29,30,31,
     *   64,90,127,123,91,108,80,125,77,93,92,78,107,96,75,
!       SP   !  "   #   $  %   &  '   (  )  *  +  ,   -  .
     *97,240,241,242,243,244,245,246,247,248,249,122,94,76,126,110,111,
!      /  0   1   2   3   4   5   6   7   8   9   :      <  =   >   ?
     *  124,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
!        @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
     *  215,216,217,226,227,228,229,230,231,232,233,173,224,189,95,109,
!        P   Q   R   S   T   U   V   W   X   Y   Z   [                _
     *  121,129,130,131,132,133,134,135,136,137,145,146,147,148,149,150,
!        `   a    b  c    d  e   f   g    h  i   j   k   l   m   n   o
     *  151,152,153,162,163,164,165,166,167,168,169,192,79,208,161  /
!        p   q   r   s   t   u   v   w   x   y   z   {  |       ~
!
      i = 1
   10 ichr = ICHAR(strin(i:i))
      strout(i:i) = CHAR(iascii(ichr))
      i = i + 1
      IF( i .GT. nchars ) RETURN
      GOTO 10
      END
