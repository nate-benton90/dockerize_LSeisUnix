{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{SCP_R 1 0 25{COM_R 2 0{TEXT 0 
26 "Limits with your Software\012"}{NXRTF 0 359 "\{\\rtf0\\ansi\{
\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\mar
gl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\\r
ed0\\green0\\blue0;\}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740
\\tx6320\\tx7900\\tx9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx1
7380\\tx18960\\tx20540\\tx22120\\tx23700\\tx25280\\tx26860\\tx284
40\\tx30020\\f0\\b\\i0\\ulnone\\qc\\fs48\\fc2\\cf2 Limits with yo
ur Software\\\012\012\}\012"}}{COM_R 3 0{TEXT 1 10 "Exercise 2"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 2\0
12\}\012"}}{INP_R 4 0 "> "{TEXT 2 122 "f := x -> 1/sin(x)^2 - 1/x
^2:\012a := 0: ntimes := 5:\012for k to ntimes do\012  print( eva
lf(a + 1/2^k), evalf(f(a + 1/2^k)) )\012od;"}}{INP_R 5 0 "> "
{TEXT 2 17 "limit(f(x), x=0);"}}{INP_R 6 0 "> "{TEXT 2 153 "Digit
s := 16:\012f := x -> ( sin(tan(x)) - tan(sin(x)) ) / x^7:\012a :
= 0: ntimes := 5:\012for k to ntimes do\012  print( evalf(a + 1/2
^k), evalf(f(a + 1/2^k)) )\012od;"}}{INP_R 7 0 "> "{TEXT 2 17 "li
mit(f(x), x=0);"}}{COM_R 8 0{TEXT 1 10 "Exercise 3"}{NXRTF 0 318 
"\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Co
urier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gree
n255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4
720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\t
x14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx2
2420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 3\012\}\012"}}
{INP_R 9 0 "> "{TEXT 2 43 "limit( (x^3 - x^2 - 4*x + 4)/(x - 1), 
x=0);"}}{INP_R 10 0 "> "{TEXT 2 24 "limit( sin(x) / x, x=0);"}}
{INP_R 11 0 "> "{TEXT 2 30 "limit( (1 - cos(x)) / x, x=0);"}}
{INP_R 12 0 "> "{TEXT 2 26 "limit( sin(5*x) / x, x=0);"}}{INP_R 
13 0 "> "{TEXT 2 27 "limit( (1 + x)^(1/x), x=0);"}}{COM_R 14 0
{TEXT 1 10 "Exercise 4"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 4\012\}\012"}}{INP_R 15 0 "> "{TEXT 2 29 "li
mit( (1 + 2*x)^(1/x), x=0);"}}{INP_R 16 0 "> "{TEXT 2 29 "limit( 
(1 + 3*x)^(1/x), x=0);"}}{INP_R 17 0 "> "{TEXT 2 29 "limit( (1 - 
2*x)^(1/x), x=0);"}}{INP_R 18 0 "> "{TEXT 2 39 "a := 'a':\012limi
t( (1 + a*x)^(1/x), x=0);"}}{COM_R 19 0{TEXT 1 10 "Exercise 5"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 5\0
12\}\012"}}{INP_R 20 0 "> "{TEXT 2 29 "limit( (1 + x^2)^(1/x), x=
0);"}}{INP_R 21 0 "> "{TEXT 2 31 "limit( (1 + 2*x^2)^(1/x), x=0);
"}}{INP_R 22 0 "> "{TEXT 2 29 "limit( (1 + x^3)^(1/x), x=0);"}}
{INP_R 23 0 "> "{TEXT 2 33 "limit( (1 + x^(3/2))^(1/x), x=0);"}}
{INP_R 24 0 "> "{TEXT 2 33 "limit( (1 + sqrt(x))^(1/x), x=0);"}}
{COM_R 25 0{TEXT 1 10 "Exercise 6"}{NXRTF 0 318 "\{\\rtf0\\ansi\{
\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\mar
gl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\
012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx70
80\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\
tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\
ulnone\\fs36\\fc0\\cf0 Exercise 6\012\}\012"}}{INP_R 26 0 "> "
{TEXT 2 26 "limit( sin(a*x) / x, x=0);"}}}{END}
