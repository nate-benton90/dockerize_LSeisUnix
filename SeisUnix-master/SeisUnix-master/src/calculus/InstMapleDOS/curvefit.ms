{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier" "Courier" "Co
urier" 4 12 192 "Courier" 12}{SCP_R 1 0 33{COM_R 2 0{TEXT 0 36 "C
urve Fitting for Discrete Data Sets"}{NXRTF 0 349 "\{\\rtf0\\ansi
\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\m
argl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\
}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx
9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx205
40\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f1\\b\\
i0\\ulnone\\qc\\fs48\\fc0\\cf0 Curve Fitting for Discrete Data Se
ts\012\}\012"}}{COM_R 3 0{TEXT 1 10 "Exercise 1"}{NXRTF 0 318 "\{
\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Couri
er;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green25
5\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720
\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14
160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx2242
0\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 1\012\}\012"}}
{INP_R 4 0 "> "{TEXT 2 115 "xdata := [.2, 1.1, 2.0, 3.1, 3.9, 5.1
]:\012ydata := [-2, 0, 1, 2.8, 5, 7]:\012data := zip( (x,y) -> [x
,y], xdata, ydata);"}}{INP_R 5 0 "> "{TEXT 2 47 "gr1 := plot(data
, style = POINT):\012display(gr1);"}}{INP_R 6 0 "> "{TEXT 2 45 "w
ith(stats):\012cof := linregress(ydata, xdata);"}}{INP_R 7 0 "> "
{TEXT 2 25 "y := cof[1] + cof[2] * x;"}}{INP_R 8 0 "> "{TEXT 2 28
 "gr2 := plot(y, x = .2..5.1):"}}{INP_R 9 0 "> "{TEXT 2 34 "with(
plots):\012display( [gr1, gr2]);"}}{COM_R 10 0{TEXT 1 10 "Exercis
e 2"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roma
n;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\color
tbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\t
x2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\t
x11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx2
0060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercis
e 2\012\}\012"}}{INP_R 11 0 "> "{TEXT 2 209 "ddata := [8.6, 10.7,
 11., 11.4, 12.0, 13.3, 14.5, 16.0,\012\011\01117.3, 18.0, 20.6]:
\012hdata := [65, 81, 75, 76, 75, 86, 74, 72, 81, 80, 87]:\012vda
ta := [10.3, 18.8, 18.2, 21.4, 19.1, 27.4, 36.3, 38.3,\012\011\01
155.4, 51.5, 77.]:"}}{COM_R 12 0{TEXT 3 7 "d vs. v"}{NXRTF 0 291 
"\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;\}\012\\margl40\0
12\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\c
b1\\pard\\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6
300\\tx7200\\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\t
x13500\\tx14400\\tx15300\\tx16200\\tx17100\\f0\\b0\\i0\\ulnone\\f
s24\\fc0\\cf0 d vs. v\012\}\012"}}{INP_R 13 0 "> "{TEXT 2 76 "dat
a := zip( (x,y) -> [x,y], ddata, vdata);\012cof := linregress(vda
ta, ddata);"}}{INP_R 14 0 "> "{TEXT 2 25 "y := cof[1] + cof[2] * 
x;"}}{INP_R 15 0 "> "{TEXT 2 87 "gr1 := plot(data, style = POINT)
:\012gr2 := plot(y, x = 8.6.. 20.6):\012display( [gr1, gr2]);"}}
{COM_R 16 0{TEXT 3 7 "h vs. v"}{NXRTF 0 291 "\{\\rtf0\\ansi\{\\fo
nttbl\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\co
lortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\
\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\
tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx15
300\\tx16200\\tx17100\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 h vs. v
\012\}\012"}}{INP_R 17 0 "> "{TEXT 2 76 "data := zip( (x,y) -> [x
,y], hdata, vdata);\012cof := linregress(vdata, hdata);"}}{INP_R 
18 0 "> "{TEXT 2 25 "y := cof[1] + cof[2] * x;"}}{INP_R 19 0 "> "
{TEXT 2 84 "gr1 := plot(data, style = POINT):\012gr2 := plot(y, x
 = 65.. 87):\012display( [gr1, gr2]);"}}{COM_R 20 0{TEXT 1 10 "Ex
ercise 3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times
-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\
colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx11
80\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx106
20\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880
\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Ex
ercise 3\012\}\012"}}{INP_R 21 0 "> "{TEXT 2 182 "tdata := [0, 1.
8, 2.8, 3.9, 5.1, 6.8, 8.6, 10.5, 13,\012\011\01116.3, 19.7, 25.6
]:\012vdata := [0, 30, 40, 50, 60, 70, 80, 90, 100,\012\011\01111
0, 120, 130]:\012data := zip( (x,y) -> [x,y], tdata, vdata);\012"
}}{INP_R 22 0 "> "{TEXT 2 47 "v := 'v':\012dataMat := array( [[t,
v], op(data)]);"}}{INP_R 23 0 "> "{TEXT 2 59 "with(stats):\012cof
 := regression(dataMat, v=a + b*t + c*t^2);"}}{INP_R 24 0 "> "
{TEXT 2 31 "v := subs(cof, a +b*t + c*t^2);"}}{INP_R 25 0 "> "
{TEXT 2 85 "gr1 := plot(data, style = POINT):\012gr2 := plot(v, t
 = 0.. 25.6):\012display( [gr1, gr2]);"}}{COM_R 26 0{TEXT 3 10 "E
xercise 4"}{NXRTF 0 298 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx
4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\
tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx
22420\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Exercise 4\012\}\012"}}
{COM_R 27 0{TEXT 3 44 "Lets make one try: v = at + bt^2 + ct^(1/2
):"}{NXRTF 0 256 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;
\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\
blue255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760
\\tx6912\\tx8064\\tx9216\\tx10368\\tx11520\\f0\\b0\\i0\\ulnone\\f
s24\\fc0\\cf0 Lets make one try: v = at + bt^2 + ct^(1/2):\012\}\
012"}}{INP_R 28 0 "> "{TEXT 2 54 "cof := regression(dataMat, v=a*
t + b*t^2 + c*t^(1/2));"}}{INP_R 29 0 "> "{TEXT 2 39 "v := subs(c
of, a*t +b*t^2 + c*sqrt(t));"}}{INP_R 30 0 "> "{TEXT 2 85 "gr1 :=
 plot(data, style = POINT):\012gr2 := plot(v, t = 0.. 25.6):\012d
isplay( [gr1, gr2]);"}}{COM_R 31 0{TEXT 3 16 "Not much better."}
{NXRTF 0 228 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;\}\0
12\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue
255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx
6912\\tx8064\\tx9216\\tx10368\\tx11520\\f0\\b0\\i0\\ulnone\\fs24\
\fc0\\cf0 Not much better.\012\}\012"}}{COM_R 32 0{TEXT 1 10 "Exe
rcise 5"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-
Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx118
0\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1062
0\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\
\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exe
rcise 5\012\}\012"}}{INP_R 33 0 "> "{TEXT 2 78 "xdata := [1, 2, 3
, 4]:\012ydata := [1, 4, 5, 6]:\012cof := linregress(ydata, xdata
);"}}{COM_R 34 0{TEXT 3 31 "So b = 0 was a good assumption."}
{NXRTF 0 243 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;\}\0
12\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue
255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx
6912\\tx8064\\tx9216\\tx10368\\tx11520\\f0\\b0\\i0\\ulnone\\fs24\
\fc0\\cf0 So b = 0 was a good assumption.\012\}\012"}}}{END}
