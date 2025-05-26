{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 14 64 "Times-Bold" 14}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier" "Courier" "Co
urier" 4 12 192 "Courier" 12}{FONT 4 "Courier-Bold" "Courier" "Co
urier-Bold" 8 12 192 "Courier-Bold" 12}{FONT 5 "Times-Bold" "Time
s" "Times-Bold" 8 18 64 "Times-Bold" 18}{SCP_R 1 0 44{COM_R 2 0
{TEXT 0 23 "Parametric Curves in 2D"}{NXRTF 0 336 "\{\\rtf0\\ansi
\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\m
argl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\
}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx
9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx205
40\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f1\\b\\
i0\\ulnone\\qc\\fs48\\fc0\\cf0 Parametric Curves in 2D\012\}\012"
}}{COM_R 3 0{TEXT 1 39 "Two parametrizations of the unit circle"}
{NXRTF 0 343 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f
0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800\
\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx9000\\
tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx15300\\tx1
6200\\tx17100\\f1\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Two parametrizat
ions of the unit circle\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 60 "x 
:= t -> cos(t):\012y := t -> sin(t):\012plot( [x, y, 0..2*Pi] );"
}}{INP_R 5 0 "> "{TEXT 2 82 "x := t -> (1 - t^2)/(1 + t^2):\012y 
:= t -> 2*t/(1 + t^2):\012plot( [x, y, -100..100] );"}}{COM_R 6 0
{TEXT 3 42 "Verifying that we do indeed have a circle:"}{NXRTF 0 
254 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;\}\012\\margl
40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\01
2\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx5760\\tx6900\\tx8
060\\tx9200\\tx10360\\tx11520\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0
 Verifying that we do indeed have a circle:\012\}\012"}}{INP_R 7 
0 "> "{TEXT 2 28 "simplify( x(t)^2 + y(t)^2 );"}}{OUT_R 8 0 7{DAG
 j2x0001}}{COM_R 9 0{TEXT 1 19 "The brachistochrone"}{NXRTF 0 323
 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800\\tx2700\\tx3
600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx9000\\tx9900\\tx10
800\\tx11700\\tx12600\\tx13500\\tx14400\\tx15300\\tx16200\\tx1710
0\\f1\\b\\i0\\ulnone\\fs28\\fc0\\cf0 The brachistochrone\012\}\01
2"}}{INP_R 10 0 "> "{TEXT 2 73 "x := t -> t - sin(t)*cos(t):\012y
 := t -> sin(t)^2:\012plot( [x, y, 0..2*Pi] );"}}{COM_R 11 0{TEXT
 1 11 "The cycloid"}{NXRTF 0 315 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\
fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr
40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\
\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx72
00\\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\t
x14400\\tx15300\\tx16200\\tx17100\\f1\\b\\i0\\ulnone\\fs28\\fc0\\
cf0 The cycloid\012\}\012"}}{INP_R 12 0 "> "{TEXT 2 68 "x := t ->
 t - sin(t):\012y := t -> 1 - cos(t):\012plot( [x, y, 0..4*Pi] );
"}}{COM_R 13 0{TEXT 5 24 "Unit Circle (Exercise 1)"}{NXRTF 0 332 
"\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Co
urier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gree
n255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4
720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\t
x14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx2
2420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Unit Circle (Exercise 1)\
012\}\012"}}{INP_R 14 0 "> "{TEXT 2 55 "x := t -> (1 - t^2)/(1 + 
t^2):\012y := t -> 2*t/(1 + t^2):"}}{INP_R 15 0 "> "{TEXT 2 52 "v
 := t -> simplify( sqrt( D(x)(t)^2 + D(y)(t)^2 ) );"}}{OUT_R 16 0
 15{DAG :3n3\`v`@7,2n3\`t`,1,3n5\`operator`n4\`arrow`p7(3n5\`simp
lify`,2(3n4\`sqrt`,2+5*3(3(3n3\`D`,2n3\`x`,2a2x0001j2x0002j2x0001
*3(3(3p20,2n3\`y`p25p28p2Ap7}}{INP_R 17 0 "> "{TEXT 2 5 "v(t);"}}
{OUT_R 18 0 17{DAG +3*3+5j2x0001p3*3n3\`t`j2x0002p3i2x0001p9}}
{INP_R 19 0 "> "{TEXT 2 87 "for k from -5 to 5 do\012   lprint( k
, evalf( x(k) ),  evalf( y(k) ),  evalf( v(k) ) )\012od;"}}{OUT_R
 20 0 19{TEXT 4 428 "-5   -.9230769231   -.3846153846   769230769
2.0E-11\012-4   -.8823529412   -.4705882353   .1176470588\012-3  
 -.8000000000   -.6000000000   .2000000000\012-2   -.6000000000  
 -.8000000000   .4000000000\012-1   0   -1.   1.\0120   1.   0   
2.\0121   0   1.   1.\0122   -.6000000000   .8000000000   .400000
0000\0123   -.8000000000   .6000000000   .2000000000\0124   -.882
3529412   .4705882353   .1176470588\0125   -.9230769231   .384615
3846   7692307692.0E-11\012"}}{COM_R 21 0{TEXT 5 27 "Hypocycloid 
(Exercises 2-8)"}{NXRTF 0 335 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fni
l Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\
012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx
0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440
\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\
tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\
\cf0 Hypocycloid (Exercises 2-8)\012\}\012"}}{INP_R 22 0 "> "
{TEXT 2 108 "x := t -> (a-b)*cos(t) + b*cos( (a-b)*t/b ):\012y :=
 t -> (a-b)*sin(t) - b*sin( (a-b)*t/b ):\012a := 1:  b := 1/k:"}}
{INP_R 23 0 "> "{TEXT 2 81 "printlevel := 1:\012for k from 3 to 6
 do\012   b := 1/k:\012   plot( [x, y, 0..2*Pi] )\012od;"}}{OUT_R
 24 0 23{DAG :3n3\`b`/3j2x0001j2x0003}}{OUT_R 25 0 23{DAG :3n3\`b
`/3j2x0001j2x0004}}{OUT_R 26 0 23{DAG :3n3\`b`/3j2x0001j2x0005}}
{OUT_R 27 0 23{DAG :3n3\`b`/3j2x0001j2x0006}}{COM_R 28 0{TEXT 3 
49 "Maple needed lots of help with the algrebra here!"}{NXRTF 0 
261 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;\}\012\\margl
40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\01
2\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx5760\\tx6900\\tx8
060\\tx9200\\tx10360\\tx11520\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0
 Maple needed lots of help with the algrebra here!\012\}\012"}}
{INP_R 29 0 "> "{TEXT 2 35 "temp := ( D(x)(t)^2 + D(y)(t)^2 ); "}
}{INP_R 30 0 "> "{TEXT 2 13 "expand(temp);"}}{INP_R 31 0 "> "
{TEXT 2 12 "simplify(\");"}}{INP_R 32 0 "> "{TEXT 2 21 "collect(\
", cos(t/b));"}}{INP_R 33 0 "> "{TEXT 2 10 "tmp2 := \";"}}{INP_R 
34 0 "> "{TEXT 2 60 "div := (k^2 - 2*k + 1) / k^2:\012temp := sim
plify( tmp2/div  );"}}{INP_R 35 0 "> "{TEXT 2 27 "tempInt := 2 * 
sin(t*k/2) ;"}}{INP_R 36 0 "> "{TEXT 2 41 "tmplength :=int( tempI
nt, t = 0..2*Pi/k);"}}{INP_R 37 0 "> "{TEXT 2 26 "length := tmple
ngth*div*k;"}}{INP_R 38 0 "> "{TEXT 2 29 "limit( length, k = infi
nity);"}}{OUT_R 39 0 38{DAG j2x0008}}{COM_R 40 0{TEXT 1 10 "Exerc
ise 5"}{NXRTF 0 314 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Ro
man;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\col
ortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\
tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\t
x9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx153
00\\tx16200\\tx17100\\f1\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Exercise 
5\012\}\012"}}{INP_R 41 0 "> "{TEXT 2 66 "for k by 2 to 15 do\012
   b := k/16:\012   plot( [x, y, 0..2*k*Pi] )\012od;"}}{COM_R 42 
0{TEXT 5 10 "Exercise 9"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\
\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\
margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\
pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260
\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\t
x17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs
36\\fc0\\cf0 Exercise 9\012\}\012"}}{INP_R 43 0 "> "{TEXT 2 49 "k
 := 'k':\012x := t -> cos(t)^k:\012y := t -> sin(t)^k:"}}{INP_R 
44 0 "> "{TEXT 2 49 "for k by 2 to 7 do\012   plot( [x, y, 0..2*P
i] )\012od;"}}{INP_R 45 0 "> "{TEXT 2 189 "printlevel := 0:\012fo
r n from 1 to 6 do\012   k := 2^n:\012   dssq := factor( D(x)(t)^
2 + D(y)(t)^2 ) ; \012   ds := sqrt(dssq):   \012   ans := 4 * ev
alf( Int( ds, t = 0..Pi/2) ):\012   lprint( k, ans)\012od;"}}}
{END}
