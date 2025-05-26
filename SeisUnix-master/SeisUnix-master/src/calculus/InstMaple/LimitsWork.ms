{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Ohlfs" "Ohlfs
" "Courier" 4 11 192 "Ohlfs" 11}{FONT 1 "Times-Bold" "Times" "Tim
es-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Times-Bold" "Times" "Ti
mes-Bold" 8 14 64 "Times-Bold" 14}{FONT 3 "Courier-Bold" "Courier
" "Courier-Bold" 8 12 192 "Courier-Bold" 12}{SCP_R 1 0 21{INP_R 2
 0 "> "{TEXT 0 95 "f := proc(x)\012  if x <= -1 then x^3 + 2\012 
 elif x >= 1 then x^4 + 1\012  else x^2 + x + 1\012  fi;\012end;"
}}{COM_R 3 0{TEXT 1 10 "Exercise 2"}{NXRTF 0 318 "\{\\rtf0\\ansi\
{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7
080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\
\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\
\ulnone\\fs36\\fc0\\cf0 Exercise 2\012\}\012"}}{INP_R 4 0 "> "
{TEXT 0 117 "f :=  x -> x^3 + 3*x^2;\012f(2);\012subs(x = 2, f(x)
);\012f(t);\012f(cat);\012for n to 5 do\012   print(f(n))\012od;\
012seq( f(n), n = 1..5);"}}{COM_R 5 0{TEXT 1 10 "Exercise 3"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f
0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 3\0
12\}\012"}}{INP_R 6 0 "> "{TEXT 0 145 "factor((x^3 - 9*x^2 - 45*x
 - 91)/(x - 13));\012factor((x^3 - 9*x^2 - 39*x - 86)/(x - 13));\
012factor((x^4 - 26*x^3 + 178*x^2 - 234*x + 1521)/(x - 13));"}}
{COM_R 7 0{TEXT 2 50 "Behind the scenes: how the problem was cons
tructed"}{NXRTF 0 354 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-
Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900
\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\
\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx1
5300\\tx16200\\tx17100\\f1\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Behind 
the scenes: how the problem was constructed\012\}\012"}}{INP_R 8 
0 "> "{TEXT 0 153 "expand( (x-13)*(x^2 + 4*x + 7) );\012collect(\
", x);\012next := expand( (x-13)*(x^2 + 4*x + 7)+(5+6*x));\012fac
tor(next);\012expand( (x-13)^2 *(x^2 + 9) );\012factor(\");"}}
{COM_R 9 0{TEXT 1 10 "Exercise 4"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\
\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\marg
l40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\0
12\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx708
0\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\t
x16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\u
lnone\\fs36\\fc0\\cf0 Exercise 4\012\}\012"}}{INP_R 10 0 "> "
{TEXT 0 132 "f := x -> (sqrt(25 + 3*x) - sqrt(25 - 2*x))/x:\012pl
ot(f(x), x = -1..1);\012for x from -1.0 by 0.2 to 1.0 do\012   pr
int(x, `   `, f(x))\012od; "}}{INP_R 11 0 "> "{TEXT 0 102 "a := 0
: ntimes := 5:\012for k to ntimes do\012   print( evalf(a + 1/2^k
), `   `, f(evalf(a + 1/2^k)) )\012od; \012"}}{INP_R 12 0 "> "
{TEXT 0 102 "a := 0: ntimes := 5:\012for k to ntimes do\012   pri
nt( evalf(a - 1/2^k), `    `, f(evalf(a - 1/2^k)) )\012od; "}}
{COM_R 13 0{TEXT 1 10 "Exercise 5"}{NXRTF 0 318 "\{\\rtf0\\ansi\{
\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\mar
gl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\
012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx70
80\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\
tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\
ulnone\\fs36\\fc0\\cf0 Exercise 5\012\}\012"}}{INP_R 14 0 "> "
{TEXT 0 43 "subs( x=0, (x^3 - x^3 - 4*x + 4)/(x - 1) );"}}{INP_R 
15 0 "> "{TEXT 0 125 "f := x -> sin(x) / x; \012a := 0: ntimes :=
 10:\012for k to ntimes do\012   print( evalf(a + 1/2^k), `   `, 
f(evalf(a + 1/2^k)) )\012od; "}}{INP_R 16 0 "> "{TEXT 0 131 "f :=
 x -> (1 - cos(x)) / x; \012a := 0: ntimes := 10:\012for k to nti
mes do\012   print( evalf(a + 1/2^k), `   `, f(evalf(a + 1/2^k)) 
)\012od; "}}{INP_R 17 0 "> "{TEXT 0 127 "f := x -> sin(5*x) / x; 
\012a := 0: ntimes := 10:\012for k to ntimes do\012   print( eval
f(a + 1/2^k), `   `, f(evalf(a + 1/2^k)) )\012od; "}}{OUT_R 18 0 
17{DAG :3n3\`f`@7,2n3\`x`,1,3n5\`operator`n4\`arrow`p7*5(3n3\`sin
`,2+3a2x0001j2x0005j2x0001p17i2x0001p7}}{INP_R 19 0 "> "{TEXT 0 
128 "f := x -> (1 + x)^(1/x); \012a := 0: ntimes := 10:\012for k 
to ntimes do\012   print( evalf(a + 1/2^k), `   `, f(evalf(a + 1/
2^k)) )\012od; "}}{COM_R 20 0{TEXT 1 10 "Exercise 6"}{NXRTF 0 318
 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx
4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\
tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx
22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 6\012\}\012"}}
{INP_R 21 0 "> "{TEXT 0 95 "f := proc(x)\012  if x <= -1 then x^3
 + 2\012  elif x >= 1 then x^4 + 1\012  else x^2 + x + 1\012  fi;
\012end;"}}{INP_R 22 0 "> "{TEXT 0 15 "plot(f, -2..2);"}}}{END}
