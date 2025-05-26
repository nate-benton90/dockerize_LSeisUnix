{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier" "Courier" "Co
urier" 4 12 192 "Courier" 12}{SCP_R 1 0 18{COM_R 2 0{TEXT 0 31 "F
ourier Frequency Decomposition"}{NXRTF 0 344 "\{\\rtf0\\ansi\{\\f
onttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl4
0\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012
\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\
\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\t
x22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f1\\b\\i0\\u
lnone\\qc\\fs48\\fc0\\cf0 Fourier Frequency Decomposition\012\}\0
12"}}{COM_R 3 0{TEXT 1 10 "Exercise 3"}{NXRTF 0 318 "\{\\rtf0\\an
si\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\
\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255
;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\
tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx153
40\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\
i0\\ulnone\\fs36\\fc0\\cf0 Exercise 3\012\}\012"}}{INP_R 4 0 "> "
{TEXT 2 122 "f := x -> x*(Pi - x)*(Pi + x):\012nmax := 4: npi := 
evalf(Pi):\012b := seq( 2/npi * int( f(x)*sin(k*x), x=0..npi), k=
1..nmax);  "}}{INP_R 5 0 "> "{TEXT 2 44 "fn := x -> sum( b[n]*sin
(n*x), n = 1..nmax):"}}{INP_R 6 0 "> "{TEXT 2 6 "fn(x);"}}{INP_R 
7 0 "> "{TEXT 2 21 "plot(\{f, fn\}, 0..Pi);"}}{COM_R 8 0{TEXT 1 
10 "Exercise 4"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil
 Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\0
12\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0
\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\
\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\t
x18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\
cf0 Exercise 4\012\}\012"}}{INP_R 9 0 "> "{TEXT 2 99 "f := x -> x
^2 * cot(x/2):\012nmax := 6:\012b := seq( 2/npi * int( f(x)*sin(k
*x), x=0..npi), k=1..nmax);  "}}{INP_R 10 0 "> "{TEXT 2 89 "for N
 to nmax do\012   fn := x -> sum( b[n]*sin(n*x), n = 1..N):\012  
 plot(\{f, fn\}, 0..Pi)\012od;"}}{COM_R 11 0{TEXT 1 10 "Exercise 
5"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;
\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortb
l;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2
360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1
1800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx200
60\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 
5\012\}\012"}}{INP_R 12 0 "> "{TEXT 2 129 "nmax := 6:\012printlev
el := 0:\012for k to nmax do\012   c[k] := b[k]/(8 - k^2)\012od;\
012k := 'k':\012yn := x -> sum( c[k]*sin(k*x), k = 1..nmax);"}}
{INP_R 13 0 "> "{TEXT 2 16 "plot(yn, 0..Pi);"}}{INP_R 14 0 "> "
{TEXT 2 9 "yn(Pi/2);"}}{INP_R 15 0 "> "{TEXT 2 43 "k := 'k':\012y
p0 := sum( k*c[k], k = 1..nmax);"}}{INP_R 16 0 "> "{TEXT 2 133 "e
p := 10^(-20):\012diffequ := diff(y(x), x$2) + 8*y(x) = x^2 * cot
(x/2);\012F := dsolve( \{diffequ, y(ep)=0, D(y)(ep)= yp0\}, y(x),
 numeric);"}}{INP_R 17 0 "> "{TEXT 2 18 "F(Pi/2);\012yn(Pi/2);"}}
{INP_R 18 0 "> "{TEXT 2 15 "plot(h, 0..Pi);"}}{COM_R 19 0{TEXT 3 
111 "I cannot get the thing to work with starting at x = 0.\012Ev
en with the def. of h above, removing the singularity."}{NXRTF 0 
324 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;\}\012\\margl
40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\01
2\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx6912\\tx8
064\\tx9216\\tx10368\\tx11520\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0
 I cannot get the thing to work with starting at x = 0.\\\012Even
 with the def. of h above, removing the singularity.\012\}\012"}}
}{END}
