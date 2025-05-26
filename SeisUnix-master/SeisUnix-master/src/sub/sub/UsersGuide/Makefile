include ${FOCUSDIR}/etc/gmake.init

Data		:=	sample.segy

IncFiles	:=	std1 	stdscript1.inc	\
			lead1	leadscript1.inc	\
    	    	    	dftplot	dftplot.inc \
    	    	    	looper looper.inc   \
    	    	    	flooper	flooper.inc

Sub		:=	${shell which sub}

usersguide.ps: ${IncFiles} ${wildcard *.ms}

#
#	files of the form std% are computed by using stdscript%
#	and capturing stdout.
#
std%: stdscript% ${Data} ${Sub}
	sub $< < ${Data} 2> $@

%.inc: %
	sed 's/\\/\\\\/' < $< > $@

lead1: leadscript1 stdscript1 ${Data} ${Sub}
	sub leadscript1 < ${Data} | sub stdscript1 2> lead1

clean:
	rm -f std1 lead1
	rm -f ${wildcard *.inc}
	rm -rf ${wildcard *.ps}
