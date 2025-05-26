{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Courier" "Cou
rier" "Courier" 4 12 192 "Courier" 12}{FONT 1 "Times-Bold" "Times
" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 2 "Times-Bold" "Time
s" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 3 "Times-Roman" "Ti
mes" "Times-Roman" 4 12 64 "Times-Roman" 12}{FONT 4 "Ohlfs" "Ohlf
s" "Courier" 4 11 192 "Ohlfs" 11}{FONT 5 "Courier-Bold" "Courier"
 "Courier-Bold" 8 12 192 "Courier-Bold" 12}{SCP_R 1 0 70{COM_R 2 
0{TEXT 0 0 ""}{NXRTF 0 160 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\f1\\b0\\i0\\ulnone\\ql\\fs24\\fi0\
\li0\\gray0\\fc0\\cf0\\up0\\dn0 \012\}\012"}}{COM_R 3 0{TEXT 1 32
 "Derivatives, Slopes and Tangents"}{NXRTF 0 345 "\{\\rtf0\\ansi\
{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9
480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx2054
0\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i
0\\ulnone\\qc\\fs48\\fc0\\cf0 Derivatives, Slopes and Tangents\01
2\}\012"}}{COM_R 4 0{TEXT 2 10 "Exercise 1"}{NXRTF 0 318 "\{\\rtf
0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}
\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\bl
ue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5
900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\
tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0
\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 1\012\}\012"}}{COM_R 5 0
{TEXT 3 25 "Loop for figures 1 and 2."}{NXRTF 0 328 "\{\\rtf0\\an
si\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\
\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255
;\}\012\\cb1\\pard\\tx0\\tx720\\tx1440\\tx2160\\tx2880\\tx3600\\t
x4320\\tx5040\\tx5760\\tx6480\\tx7200\\tx7920\\tx8640\\tx9360\\tx
10080\\tx10800\\tx11520\\tx12240\\tx12960\\tx13680\\f0\\b0\\i0\\u
lnone\\fs24\\fc0\\cf0 Loop for figures 1 and 2.\012\}\012"}}
{INP_R 6 0 "> "{TEXT 4 78 "a  := 2.0: h := 4.0:\012for k to 5 do\
012   h := h/2:\012   plot(x^2, x = a-h..a+h)\012od;"}}{COM_R 7 0
{TEXT 2 10 "Exercise 2"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 2\012\}\012"}}{INP_R 8 0 "> "{TEXT 4 49 "h :
= 'h';\012f := x -> x^3;\012expand((f(x+h)-f(x))/h);"}}{INP_R 9 0
 "> "{TEXT 4 15 "subs(h = 0, \");"}}{INP_R 10 0 "> "{TEXT 4 49 "h
 := 'h';\012f := x -> x^4;\012expand((f(x+h)-f(x))/h);"}}{INP_R 
11 0 "> "{TEXT 4 15 "subs(h = 0, \");"}}{COM_R 12 0{TEXT 2 10 "Ex
ercise 3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times
-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\
colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx11
80\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx106
20\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880
\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Ex
ercise 3\012\}\012"}}{INP_R 13 0 "> "{TEXT 4 42 "f := x -> (x^3 -
 5)*(x^2 - 1) / (x^2 + 1);"}}{OUT_R 14 0 13{DAG :3n3\`f`@7,2n3\`x
`,1,3n5\`operator`n4\`arrow`p7*7+5*3a2x0001j2x0003j2x0001i2x0005p
18p18+5*3p14j2x0002p18i2x0001p18p18+5p1Fp18p18p18p24p7}}{INP_R 15
 0 "> "{TEXT 4 144 "b := 2.1:\012slope := (f(b) - f(2)) / (b - 2.
0):\012secant := f(2.0) + slope*(x - 2.0):\012print(`slope is `, 
slope);\012plot(\{secant, f(x)\}, x = 1.5..3.0);"}}{INP_R 16 0 ">
 "{TEXT 4 19 "f'[x] /. x -> 2 //N"}}{INP_R 17 0 "> "{TEXT 4 13 "d
iff(f(x),x);"}}{INP_R 18 0 "> "{TEXT 4 19 "evalf(subs(x=2,\"));"}
}{COM_R 19 0{TEXT 2 10 "Exercise 4"}{NXRTF 0 318 "\{\\rtf0\\ansi\
{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7
080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\
\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\
\ulnone\\fs36\\fc0\\cf0 Exercise 4\012\}\012"}}{COM_R 20 0{TEXT 0
 8 "For x^3:"}{NXRTF 0 220 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx46
00\\tx5760\\tx6900\\tx8060\\tx9200\\tx10360\\tx11520\\f1\\b0\\i0\
\ulnone\\fs24\\fc0\\cf0 For x^3:\012\}\012"}}{INP_R 21 0 "> "
{TEXT 4 74 "a  := 2.0: h := 4.0:\012for k to 5 do\012\011h := h/2
:\012\011plot(x^3, x = a-h..a+h)\012od;"}}{COM_R 22 0{TEXT 0 20 "
The slope is about: "}{NXRTF 0 232 "\{\\rtf0\\ansi\{\\fonttbl\\f1
\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\
red255\\green255\\blue255;\}\012\\cb1\\pard\\tx1140\\tx2300\\tx34
40\\tx4600\\tx5760\\tx6900\\tx8060\\tx9200\\tx10360\\tx11520\\f1\
\b0\\i0\\ulnone\\fs24\\fc0\\cf0 The slope is about: \012\}\012"}}
{INP_R 23 0 "> "{TEXT 4 35 "slope := (9.6 - 6.6)/(2.13 - 1.87);"}
}{OUT_R 24 0 23{DAG :3n4\`slope`e3j4x001153846154i2x0008}}{INP_R 
25 0 "> "{TEXT 4 31 "tangent := 2^3 + slope*(x - 2);"}}{OUT_R 26 
0 25{DAG :3n4\`tangent`+5e3i4x001507692308i2x0008j2x0001n3\`x`e3j
4x001153846154pA}}{INP_R 27 0 "> "{TEXT 4 38 "trueSlope := subs(x
 = 2, diff(x^3,x));"}}{OUT_R 28 0 27{DAG :3n5\`trueSlope`j2x0012}
}{COM_R 29 0{TEXT 0 44 "Similarly for x^4, the real slope is 32 a
nd:"}{NXRTF 0 256 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courier
;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\
\blue255;\}\012\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx576
0\\tx6900\\tx8060\\tx9200\\tx10360\\tx11520\\f1\\b0\\i0\\ulnone\\
fs24\\fc0\\cf0 Similarly for x^4, the real slope is 32 and:\012\}
\012"}}{INP_R 30 0 "> "{TEXT 4 74 "a  := 2.0: h := 4.0:\012for k 
to 5 do\012\011h := h/2:\012\011plot(x^4, x = a-h..a+h)\012od;"}}
{INP_R 31 0 "> "{TEXT 4 34 "slope := (25 - 9.5)/(2.25 - 1.75);"}}
{OUT_R 32 0 31{DAG :3n4\`slope`e3j4x003100000000i2x0008}}{INP_R 
33 0 "> "{TEXT 4 31 "tangent := 2^4 + slope*(x - 2);"}}{OUT_R 34 
0 33{DAG :3n4\`tangent`+5e3i4x004600000000i2x0008j2x0001n3\`x`e3j
4x003100000000pA}}{COM_R 35 0{TEXT 2 10 "Exercise 5"}{NXRTF 0 318
 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx
4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\
tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx
22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 5\012\}\012"}}
{INP_R 36 0 "> "{TEXT 4 42 "f := x -> (x^3 - 5)*(x^2 - 1) / (x^2 
+ 1);"}}{OUT_R 37 0 36{DAG :3n3\`f`@7,2n3\`x`,1,3n5\`operator`n4\
`arrow`p7*7+5*3a2x0001j2x0003j2x0001i2x0005p18p18+5*3p14j2x0002p1
8i2x0001p18p18+5p1Fp18p18p18p24p7}}{INP_R 38 0 "> "{TEXT 4 76 "\0
12a  := 2.0: h := 4.0:\012for k to 5 do\012\011h := h/2:\012\011p
lot(f(x), x = a-h..a+h)\012od;"}}{INP_R 39 0 "> "{TEXT 4 36 "slop
e := (2.9 - .92)/(2.15 - 1.875);"}}{OUT_R 40 0 39{DAG :3n4\`slope
`e3j4x007200000000i2x0009}}{INP_R 41 0 "> "{TEXT 4 32 "tangent :=
 f(2) + slope*(x - 2);"}}{OUT_R 42 0 41{DAG :3n4\`tangent`+5e3i4x
001260000000i2x0008j2x0001n3\`x`e3j4x007200000000i2x0009}}{INP_R 
43 0 "> "{TEXT 4 49 "trueSlope := subs(x = 2, diff(f(x),x)):\012e
valf(\");"}}{OUT_R 44 0 43{DAG e3j4x008160000000i2x0009}}{COM_R 
45 0{TEXT 2 10 "Exercise 6"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fontt
bl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\01
2\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb
1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8
260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520
\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\
\fs36\\fc0\\cf0 Exercise 6\012\}\012"}}{INP_R 46 0 "> "{TEXT 4 27
 "g := (x - 2)^(2/3) + 2*x^3:"}}{INP_R 47 0 "> "{TEXT 4 81 "a := 
2.0:  h := 1.0:\012for k to 16 do\012   h := h/2.0:\012   plot(g,
 x = a-h..a+h)\012od;  "}}{COM_R 48 0{TEXT 0 42 "After many zooms
 the `cusp' becomes clear."}{NXRTF 0 254 "\{\\rtf0\\ansi\{\\fontt
bl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\color
tbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx1152\\tx2304
\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\\tx1152
0\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 After many zooms the `cusp'
 becomes clear.\012\}\012"}}{COM_R 49 0{TEXT 2 10 "Exercise 7"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 7\0
12\}\012"}}{COM_R 50 0{TEXT 0 41 "Let's approximate the slopes nu
merically:"}{NXRTF 0 253 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern 
Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gr
een255\\blue255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608
\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\\tx11520\\f1\\b0\\i0\\u
lnone\\fs24\\fc0\\cf0 Let's approximate the slopes numerically:\0
12\}\012"}}{INP_R 51 0 "> "{TEXT 4 17 "f := x -> sin(x);"}}{OUT_R
 52 0 51{DAG :3n3\`f`n3\`sin`}}{INP_R 53 0 "> "{TEXT 4 151 "a := 
Pi/3.0:  b := Pi/6.0:\012h := 1.0:\012for k to 4 do\012   h := h/
10.0:\012   slope1 := evalf( (f(a+h) - f(a))/h);\012   slope2 := 
evalf( (f(b+h) - f(b))/h)\012od;  "}}{OUT_R 54 0 53{DAG :3n3\`h`e
3j4x001000000000i2x0010}}{OUT_R 55 0 53{DAG :3n4\`slope1`e3j4x000
455901885i2x0009}}{OUT_R 56 0 53{DAG :3n4\`slope2`e3j4x0008396035
76i2x0009}}{OUT_R 57 0 53{DAG :3n3\`h`e3j4x001000000000i2x0011}}
{OUT_R 58 0 53{DAG :3n4\`slope1`e3j3x49566157i2x0008}}{OUT_R 59 0
 53{DAG :3n4\`slope2`e3j3x86351099i2x0008}}{OUT_R 60 0 53{DAG :3n
3\`h`e3j4x001000000000i2x0012}}{OUT_R 61 0 53{DAG :3n4\`slope1`e3
j3x04995669i2x0007}}{OUT_R 62 0 53{DAG :3n4\`slope2`e3j3x08657752
i2x0007}}{OUT_R 63 0 53{DAG :3n3\`h`e3j4x001000000000i2x0013}}
{OUT_R 64 0 53{DAG :3n4\`slope1`e3j3x00499957i2x0006}}{OUT_R 65 0
 53{DAG :3n4\`slope2`e3j3x00866000i2x0006}}{COM_R 66 0{TEXT 0 27 
"Since D(sin) = cos we have:"}{NXRTF 0 239 "\{\\rtf0\\ansi\{\\fon
ttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\col
ortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx1152\\tx23
04\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\\tx11
520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Since D(sin) = cos we hav
e:\012\}\012"}}{INP_R 67 0 "> "{TEXT 4 25 "deriv1 := evalf( cos(a
));"}}{OUT_R 68 0 67{DAG :3n4\`deriv1`e3j4x005000000002i2x0010}}
{INP_R 69 0 "> "{TEXT 4 25 "deriv1 := evalf( cos(b));"}}{OUT_R 70
 0 69{DAG :3n4\`deriv1`e3j4x008660254037i2x0010}}{INP_R 71 0 "> "
{TEXT 4 0 ""}}}{END}
