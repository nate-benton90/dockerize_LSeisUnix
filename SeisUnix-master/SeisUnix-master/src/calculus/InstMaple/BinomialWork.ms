{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier" "Courier" "Co
urier" 4 12 192 "Courier" 12}{SCP_R 1 0 14{COM_R 2 0{TEXT 0 33 "T
he Binomial Theorem and Calculus"}{NXRTF 0 346 "\{\\rtf0\\ansi\{\
\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\marg
l40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\0
12\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx948
0\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\
\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f1\\b\\i0\
\ulnone\\qc\\fs48\\fc0\\cf0 The Binomial Theorem and Calculus\012
\}\012"}}{COM_R 3 0{TEXT 1 10 "Exercise 7"}{NXRTF 0 318 "\{\\rtf0
\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\
012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blu
e255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx59
00\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\t
x15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\
\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 7\012\}\012"}}{INP_R 4 0 
"> "{TEXT 2 130 "f := x -> (1 + x)^5;\012g := x -> 1 + 5*x;\012a 
 := 0.0: h := 100.0:\012for k to 5 do\012   h := h/10:\012   plot
(\{f(x), g(x)\}, x = a-h..a+h)\012od;"}}{COM_R 5 0{TEXT 1 10 "Exe
rcise 8"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-
Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx118
0\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1062
0\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\
\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exe
rcise 8\012\}\012"}}{INP_R 6 0 "> "{TEXT 2 134 "f := x -> (1 + x)
^(1/3);\012g := x -> 1 + x/3;\012a  := 0.0: h := 100.0:\012for k 
to 5 do\012   h := h/10:\012   plot(\{f(x), g(x)\}, x = a-h..a+h)
\012od;"}}{COM_R 7 0{TEXT 1 10 "Exercise 9"}{NXRTF 0 318 "\{\\rtf
0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}
\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\bl
ue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5
900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\
tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1
\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 9\012\}\012"}}{COM_R 8 0
{TEXT 3 144 "Note: the `printlevel := 0' command avoids the print
ing of \012`S := S + 1.0/2^k' in the for loop.  Set printlevel ba
ck to 1 if\012plotting in a loop."}{NXRTF 0 358 "\{\\rtf0\\ansi\{
\\fonttbl\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{
\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx1152\
\tx2304\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\
\tx11520\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Note: the `printleve
l := 0' command avoids the printing of \\\012`S := S + 1.0/2^k' i
n the for loop.  Set printlevel back to 1 if\\\012plotting in a l
oop.\012\}\012"}}{INP_R 9 0 "> "{TEXT 2 151 "k := 'k': S := 'S':\
012lprint(`k`, `        `, `S`);\012S := 1:\012printlevel := 0:\0
12for k to 20 do\012   S := S + 1.0/2^k:\012   lprint(k, `       
 `, evalf(S,4))\012od;"}}{COM_R 10 0{TEXT 1 11 "Exercise 10"}
{NXRTF 0 319 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f
0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 10\
012\}\012"}}{INP_R 11 0 "> "{TEXT 2 144 "k := 'k': S := 'S':\012l
print(`k`, `        `, `S(k)`);\012S := 1:\012for k from 2 to 50 
do\012   S := S + 1.0/k^2:\012   lprint(k, `        `, evalf(S,4)
)\012od;"}}{COM_R 12 0{TEXT 1 11 "Exercise 11"}{NXRTF 0 319 "\{\\
rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier
;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\
\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\
tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx1416
0\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\
\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 11\012\}\012"}}{COM_R
 13 0{TEXT 3 107 "Note we are using the `modp' feature to print o
ut every 10th time.\012Of course this series does not converge."}
{NXRTF 0 320 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;\}\0
12\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue
255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx
6912\\tx8064\\tx9216\\tx10368\\tx11520\\f0\\b0\\i0\\ulnone\\fs24\
\fc0\\cf0 Note we are using the `modp' feature to print out every
 10th time.\\\012Of course this series does not converge.\012\}\0
12"}}{INP_R 14 0 "> "{TEXT 2 173 "k := 'k': S := 'S':\012lprint(`
k`, `        `, `S(k)`);\012S := 1:\012for k to 100 do\012   S :=
 S + 1.0/k;\012   if modp(k,10) = 0 then \012      lprint(k, `   
     `, evalf(S,4))\012   fi;\012od;"}}{INP_R 15 0 "> "{TEXT 2 0 
""}}}{END}
