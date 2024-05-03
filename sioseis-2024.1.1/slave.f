      SUBROUTINE slave( bufl, nlong, bufs, nshort, nsamps, thres, index,
     &   aves, avel, iprint )
!   Short-long average amplitude picker.
!   buf - the data array
!   nsamps - the number of samples in buf
!   nshort - the number of samples in the short average window.
!   nlong - the number of samples in the long average window.
!   thres - the threshold ratio of short/long that defines a pick
!   index - the index of the pick in buf.  The index is the "center" of
!           the window when (short average) / (long average) exceeded
!           thres.  An index of 0 means thres was never exceed (pick
!           failed).
!   aves - the computed short average of the window when the ratio
!          exceed thres.
!   avel - the computed long window average.
!
!  Copyrighted and written by:  Paul Henkart, 
!  SCRIPPS INSTITUTION OF OCEANOGRAPHY, September 1995
!  All Rights Reserved.
!
!  mod 8 Jan 03 - Skip the short window if it's 0.
!
      REAL bufl(1), bufs(1), thres, aves, avel
      INTEGER nlong, nshort, nsamps, index
      REAL sum, avelong, aveshort, shortn, ratio
      INTEGER index1, index2, i
      LOGICAL iprint
!
      ratio = thres
      sum = 0.
      DO i = 1, nlong
         sum = sum + ABS(bufl(i))
      ENDDO
      avelong = sum / FLOAT(nlong)
      avel = avelong
      shortn = FLOAT(nshort)
      sum = 0.
      DO i = 1, nshort
         sum = sum + ABS(bufs(i))
      ENDDO
      aveshort = sum / shortn
      index = 0
      index1 = 1
      DO index2 = nshort+1, nsamps
         IF( iprint ) PRINT *,' short=',aveshort,' long=',avelong,
     &       ' ratio=',ABS(aveshort / avelong)
         IF( aveshort .NE. 0. .AND. avelong .NE. 0. ) THEN
             IF( ABS(aveshort / avelong) .GT. ratio ) THEN
                 index = index1 + nshort / 2
                 aves = aveshort
                 RETURN
             ENDIF
         ENDIF
         sum = sum - ABS(bufs(index1)) + ABS(bufs(index2))
         aveshort = sum / shortn
         index1 = index1 + 1
      ENDDO
      RETURN
      END
