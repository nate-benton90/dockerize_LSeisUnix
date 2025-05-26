(*^

::[paletteColors = 128; automaticGrouping; currentKernel; 
	fontset = title, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e8,  24, "Times"; ;
	fontset = subtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e6,  18, "Times"; ;
	fontset = subsubtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, italic, L1, e6,  14, "Times"; ;
	fontset = section, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, grayBox, M22, bold, L1, a20,  18, "Times"; ;
	fontset = subsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, blackBox, M19, bold, L1, a15,  14, "Times"; ;
	fontset = subsubsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, whiteBox, M18, bold, L1, a12,  12, "Times"; ;
	fontset = text, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = smalltext, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  10, "Times"; ;
	fontset = input, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeInput, M42, N23, bold, L1,  12, "Courier"; ;
	fontset = output, output, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L-5,  12, "Courier"; ;
	fontset = message, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = print, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = info, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = postscript, PostScript, formatAsPostScript, output, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeGraphics, M7, l34, w282, h287, L1,  12, "Courier"; ;
	fontset = name, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, italic, L1,  10, "Times"; ;
	fontset = header, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = Left Header, nohscroll, cellOutline,  12;
	fontset = footer, inactive, nohscroll, noKeepOnOnePage, preserveAspect, center, M7, L1,  12;
	fontset = Left Footer, cellOutline, blackBox,  12;
	fontset = help, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  10, "Times"; ;
	fontset = clipboard, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = completions, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12, "Courier"; ;
	fontset = special1, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special2, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special3, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special4, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special5, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;]
:[font = title; inactive; preserveAspect; startGroup; ]
Area
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Generating Figure 1
:[font = text; inactive; preserveAspect; ]
Added embellishments with a Draw tool.
:[font = input; preserveAspect; ]
f[x_] := x^2
a = 0.0; b = 4.0;
n = 4;  delx = (b -a )/n;
:[font = input; preserveAspect; ]
p = 0.5;
step[x_] := 1/2 Sum[f[a+(i+p)delx]
         (Sign[x-(a+i delx)] - Sign[x-(a+(i+1)delx)]),
         {i,0,n-1}]
:[font = input; preserveAspect; endGroup; ]
Plot[{f[x], step[x]}, {x,0,4.1}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Generating Figure 2
:[font = text; inactive; preserveAspect; ]
(* Adapted from  Maeder's Book *)
:[font = input; preserveAspect; ]
points =
Table[N@{x, y}, {x, -10, 10, 1}, {y, -10, 10, 1}];
vlines = Line /@ points;
hlines = Line /@ Transpose[points];
grid = Join[hlines, vlines];
Show[Graphics[grid],
	AspectRatio -> Automatic
]
:[font = input; preserveAspect; ]
<<Graphics`Master` (* To get PolarPlot *)
:[font = input; preserveAspect; ]
circle = PolarPlot[8, {theta, 0, 2Pi}]
:[font = input; preserveAspect; endGroup; ]
Show[{Graphics[grid], circle}, AspectRatio->Automatic]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Generating Figure 3
:[font = input; preserveAspect; ]
ellipse =
ParametricPlot[{8 Cos[theta], 4 Sin[theta]},
	{theta, 0, 2Pi},
	AspectRatio->Automatic
]
:[font = input; preserveAspect; endGroup; ]
Show[{Graphics[grid], ellipse}, AspectRatio->Automatic]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Sum Command
:[font = input; preserveAspect; endGroup; ]
Sum[i^2, {i, 2, 6}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6--Sums
:[font = input; preserveAspect; ]
Table[4^k, {k, 5}]
:[font = input; preserveAspect; ]
Table[{4^k, Sum[i^3, {i, 1, 4^k}]},
{k, 5}] //TableForm
:[font = text; inactive; preserveAspect; ]
Some variations on this theme:
:[font = input; preserveAspect; ]
Table[{n, Sum[i^3, {i, 1, n}]} /. n -> 4^k,
{k, 5}] //TableForm
:[font = input; preserveAspect; endGroup; ]
TableForm[
	Table[{4^k, Sum[i^3, {i, 1, 4^k}]},
	{k, 5}],
	TableHeadings -> {None, {"n", "Sum[i^3, {i, 1, n}]"}}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 7--Product
:[font = input; preserveAspect; endGroup; ]
Product[i^2, {i, 2, 6}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 8--Computing Pi
:[font = input; preserveAspect; ]
f[x_] := Sqrt[1 - x^2]
a = 0.0; b = 1.0;
:[font = input; preserveAspect; ]
n = 5; delx = (b - a)/n;
Rsum = delx Sum[f[a + (i+1/2) delx] , {i, 0, n-1}] //N;
approxPi = 4 Rsum
:[font = input; preserveAspect; endGroup; ]
Do[
	delx = (b - a)/n;
	Rsum =  delx Sum[f[a + (i+1/2) delx] , {i, 0, n-1}] //N;
	Print[n, "    ",  4Rsum],
{n, 6, 30}]

:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 9--Area under Sin[x]
:[font = input; preserveAspect; ]
f[x_] := Sin[x]
a = 0.0; b = N[Pi];
:[font = input; preserveAspect; endGroup; ]
Do[
	delx = (b - a)/n;
	Rsum =  delx Sum[f[a + (i+1/2) delx] , {i, 0, n-1}] //N;
	Print[n, "    ",  Rsum],
{n, 5, 30, 5}]

:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 10--Area under Sin[Sin[x]]
:[font = input; preserveAspect; ]
f[x_] := Sin[Sin[x]]
a = 0.0; b = N[Pi];
:[font = input; preserveAspect; endGroup; endGroup; ]
Do[
	delx = (b - a)/n;
	Rsum =  delx Sum[f[a + (i+1/2) delx] , {i, 0, n-1}] //N;
	Print[n, "    ",  Rsum],
{n, 5, 50, 5}]

^*)