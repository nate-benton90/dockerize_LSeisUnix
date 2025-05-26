{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier" "Courier" "Co
urier" 4 12 192 "Courier" 12}{FONT 4 "Courier-Bold" "Courier" "Co
urier-Bold" 8 12 192 "Courier-Bold" 12}{SCP_R 1 0 21{COM_R 2 0
{TEXT 0 27 "Inverse Square Law Problems"}{NXRTF 0 340 "\{\\rtf0\\
ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\01
2\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue2
55;\}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900
\\tx9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\t
x20540\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\
\b\\i0\\ulnone\\qc\\fs48\\fc0\\cf0 Inverse Square Law Problems\01
2\}\012"}}{COM_R 3 0{TEXT 1 47 "Illustrative Problem: particle ou
tside a sphere"}{NXRTF 0 355 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil
 Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\0
12\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0
\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\
\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\t
x18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\
cf0 Illustrative Problem: particle outside a sphere\012\}\012"}}
{INP_R 4 0 "> "{TEXT 2 181 "R := sqrt( q^2 + r^2 - 2*q*r*cos(phi)
 ):\012F := -3*G*M*m/(4*Pi*a^3) * \012      int( int( int( (q - r
*cos(phi))/R^3*r^2*sin(phi),\012           phi = 0..Pi ), r = 0..
a ), theta = 0..2*Pi );"}}{COM_R 5 0{TEXT 3 83 "Oops, Maple took 
an improper square root, not knowing q > a,\012so to get around t
hat:"}{NXRTF 0 296 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courie
r;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255
\\blue255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx57
60\\tx6912\\tx8064\\tx9216\\tx10368\\tx11520\\f1\\b0\\i0\\ulnone\
\fs24\\fc0\\cf0 Oops, Maple took an improper square root, not kno
wing q > a,\\\012so to get around that:\012\}\012"}}{INP_R 6 0 ">
 "{TEXT 2 195 "q := a + eps:\012R := sqrt( q^2 + r^2 - 2*q*r*cos(
phi) ):\012F := -3*G*M*m/(4*Pi*a^3) * \012      int( int( int( (q
 - r*cos(phi))/R^3*r^2*sin(phi),\012           phi = 0..Pi ), r =
 0..a ), theta = 0..2*Pi );"}}{INP_R 7 0 "> "{TEXT 2 30 "q := 'q'
:\012subs( eps = q-a, F);"}}{COM_R 8 0{TEXT 1 13 "Hollow Sphere"}
{NXRTF 0 321 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Hollow Spher
e\012\}\012"}}{INP_R 9 0 "> "{TEXT 2 203 "R := sqrt( q^2 + r^2 - 
2*q*r*cos(phi) ):\012q := a + eps:\012F := -3*G*M*m/(4*Pi*(b^3 - 
a^3)) * \012      int( int( int( (q - r*cos(phi))/R^3*r^2*sin(phi
),\012           phi = 0..Pi ), r = a..b ), theta = 0..2*Pi );"}}
{INP_R 10 0 "> "{TEXT 2 47 "q := 'q':\012F := simplify( subs( eps
 = q-a, F) );"}}{COM_R 11 0{TEXT 1 8 "Cylinder"}{NXRTF 0 316 "\{\
\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courie
r;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255
\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\
\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx141
60\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420
\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Cylinder\012\}\012"}}{INP_R 
12 0 "> "{TEXT 2 38 "q := 'q':\012R := sqrt( r^2 + (q -z)^2 ):"}}
{INP_R 13 0 "> "{TEXT 2 117 "F := -G*M*m/(Pi*a^2*2*h) * \012     
 int( int( int( r*(q - z)/R^3,\012           r = 0..a ), z = -h..
h), theta = 0..2*Pi );"}}{COM_R 14 0{TEXT 3 36 "Wrong answer; aga
in our above trick:"}{NXRTF 0 248 "\{\\rtf0\\ansi\{\\fonttbl\\f1\
\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\r
ed255\\green255\\blue255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx345
6\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\\tx11520\\f1\\
b0\\i0\\ulnone\\fs24\\fc0\\cf0 Wrong answer; again our above tric
k:\012\}\012"}}{INP_R 15 0 "> "{TEXT 2 158 "q := h + e:\012R := s
qrt( r^2 + (q -z)^2 ):\012F := -G*M*m/(Pi*a^2*2*h) * \012      in
t( int( int( r*(q - z)/R^3,\012           r = 0..a ), z = -h..h),
 theta = 0..2*Pi );"}}{INP_R 16 0 "> "{TEXT 2 46 "q := 'q':\012F 
:= simplify( subs( e = q -h, F) );"}}{INP_R 17 0 "> "{TEXT 2 40 "
simplify( series( F, q = infinity, 3) );"}}{INP_R 18 0 "> "{TEXT 
2 21 "Fpoint := -G*M*m/q^2:"}}{INP_R 19 0 "> "{TEXT 2 82 "for k f
rom 0 to 5 do\012  ratio := evalf( subs( \{a=1, h=1/2, q=2^k\}, F
/Fpoint ) )\012od;"}}{COM_R 20 0{TEXT 1 14 "Infinite plate"}
{NXRTF 0 322 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Infinite pla
te\012\}\012"}}{INP_R 21 0 "> "{TEXT 2 151 "R := sqrt( x^2 + y^2 
+ q^2 ):\012F := -G*rho*m*q* int( int( 1/R^3, x = -infinity..infi
nity ),\012                                  y = -infinity..infin
ity );"}}{OUT_R 22 0 21{DAG :3n3\`F`+3*Dn3\`G`j2x0001n3\`rho`p7n3
\`m`p7n3\`q`i2x0001n3\`Pi`p7(3n3\`abs`,2pFp7i2x0002}}}{END}
