{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{SCP_R 1 0 27{COM_R 2 0{TEXT 0 
26 "Derivatives in Mathematica"}{NXRTF 0 339 "\{\\rtf0\\ansi\{\\f
onttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl4
0\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012
\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\
\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\t
x22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i0\\u
lnone\\qc\\fs48\\fc0\\cf0 Derivatives in Mathematica\012\}\012"}}
{COM_R 3 0{TEXT 1 23 "Before the Lab Examples"}{NXRTF 0 331 "\{\\
rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier
;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\
\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\
tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx1416
0\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\
\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Before the Lab Examples\012\}\
012"}}{INP_R 4 0 "> "{TEXT 2 13 "diff(x^3, x);"}}{INP_R 5 0 "> "
{TEXT 2 53 "y := 1/(1 + sqrt(1 + sqrt(x)));\012diff(y, x);\012y :
= 'y':"}}{INP_R 6 0 "> "{TEXT 2 43 "f := x -> (1 + (1 + x)^2)^3;\
012diff(f(x), x);"}}{INP_R 7 0 "> "{TEXT 2 5 "D(f);"}}{INP_R 8 0 
"> "{TEXT 2 8 "D(f)(x);"}}{INP_R 9 0 "> "{TEXT 2 17 "plot(D(f), 0
..2);"}}{INP_R 10 0 "> "{TEXT 2 44 "slope := D(f)(2);\012y := f(2
) + slope*(x - 2);"}}{INP_R 11 0 "> "{TEXT 2 31 "diff( \{sin(x)^2
, sin(x^2)\}, x);"}}{COM_R 12 0{TEXT 1 10 "Exercise 3"}{NXRTF 0 
318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\
\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx1298
0\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\
\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 3\012\}\012"
}}{INP_R 13 0 "> "{TEXT 2 64 "f := x -> x^4 - 4*x^3 + 3*x^2 - 2*x
 + 1;\012f(2) + D(f)(2)*(x - 2);"}}{COM_R 14 0{TEXT 1 10 "Exercis
e 4"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roma
n;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\color
tbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\t
x2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\t
x11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx2
0060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercis
e 4\012\}\012"}}{INP_R 15 0 "> "{TEXT 2 31 "diff((2*x^2 - 1)*(x^3
 + 2), x);"}}{INP_R 16 0 "> "{TEXT 2 10 "expand(\");"}}{INP_R 17 
0 "> "{TEXT 2 10 "factor(\");"}}{COM_R 18 0{TEXT 1 10 "Exercise 5
"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\
\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl
;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx23
60\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11
800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx2006
0\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 5
\012\}\012"}}{INP_R 19 0 "> "{TEXT 2 19 "diff(x/(x - 2), x);"}}
{INP_R 20 0 "> "{TEXT 2 10 "normal(\");"}}{COM_R 21 0{TEXT 1 10 "
Exercise 6"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Tim
es-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{
\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx
1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1
0620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx188
80\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 
Exercise 6\012\}\012"}}{INP_R 22 0 "> "{TEXT 2 43 "y := (x + x^2)
^5 * (1 + x^3)^2;\012diff(y, x);"}}{INP_R 23 0 "> "{TEXT 2 13 "ex
pand(\", x);"}}{INP_R 24 0 "> "{TEXT 2 12 "simplify(\");"}}{INP_R
 25 0 "> "{TEXT 2 10 "factor(\");"}}{COM_R 26 0{TEXT 1 10 "Exerci
se 7"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Rom
an;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colo
rtbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\
tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\
tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx
20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exerci
se 7\012\}\012"}}{INP_R 27 0 "> "{TEXT 2 22 "diff( 1/(1 - 2/x), x
);"}}{INP_R 28 0 "> "{TEXT 2 12 "simplify(\");"}}}{END}
