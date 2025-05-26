


SUFILT(1l)        UNKNOWN SECTION OF THE MANUAL        SUFILT(1l)



NAME
     filt - band pass filtering in time domain

SYNOPSIS
     sufilt [options parameters] <stdin >stdout

DESCRIPTION
     sufilt performs a  frequency  filtering  by  convolution  of
     every  trace  with the filter operator constructed according
     to the frequency pass  band  and  the  operator  length  _l_o_p
     (number  of  time  samples).   The  pass band is set by four
     values of frequency (Hertz) _f_l_0,_f_l,_f_h,_f_h_0 in the shape:


                               *******
                              *|*****|*
                             **|*****|**
                            ***|*****|***
                           ****|*****|****
                      ----|----|-----|----|----
                         fl0   fl   fh   fh0


     Low cut (between fl0 and fl) and high cut  (between  fh  and
     fh0) are approximated by Hanning window.
     NOTE! 0<=fl0<=fl<=fh<=fh0     0<lop

OPTIONS
     -v                 turn verbose on
                        (by default: off)

PARAMETERS
     fl0= fl= fh= fh0=  four values (Hertz) of frequency
                        for pass band
                        (by default: fl=0 fh=fNyquist
                                     fl0=fl/1.25 fh0=fh*1.25)

     lop=               value of operator length
                        (number of time samples)
                        (by default: 64)

EXAMPLES
     (band pass filtering)
     sufilt -v fl0=10 fl=15 fh=50 fh0=65 lop=120 <data >filtereddata

     (low pass filtering)
     susort <data | sustack ... | sufilt fh=35 fh0=45 >outdata

SEE ALSO
     suepow(1l), sugpow(1l), suagc(1l).





Sun Release 3.5          Last change: SU                        1






SUFILT(1l)        UNKNOWN SECTION OF THE MANUAL        SUFILT(1l)



DIAGNOSTICS
     On user to specify one value _f_l,_f_h at least, otherwise  pro-
     gram results in a warning and self documentation abort.

AUTHOR
     Valery

















































Sun Release 3.5          Last change: SU                        2



