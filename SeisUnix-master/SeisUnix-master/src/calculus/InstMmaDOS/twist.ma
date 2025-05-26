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
Twisting Space Curves
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = input; preserveAspect; endGroup; ]
x = r Cos[phi] Sin[theta];
y = r Sin[phi] Sin[theta];
z = r Cos[theta];
x^2 + y^2 + z^2 //Simplify
Clear[x, y, z]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
x[t_] := Cos[2n t] Sin[t]
y[t_] := Sin[2n t] Sin[t]
z[t_] := Cos[t]
:[font = input; preserveAspect; endGroup; ]
x[t]^2 + y[t]^2 + z[t]^2 //Simplify
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
x[t_] := Cos[2n t] Sin[t]
y[t_] := Sin[2n t] Sin[t]
z[t_] := 3 Cos[t]
:[font = input; preserveAspect; endGroup; ]
x[t]^2 + y[t]^2 + (z[t]/3)^2 //Simplify
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
n = 3;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, Pi}]
Clear[n]
:[font = input; preserveAspect; ]
n = 3;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, Pi},
	ViewPoint->{0.928, -2.834, 1.600}
]
Clear[n]
:[font = input; preserveAspect; ]
n = 3;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, Pi},
	ViewPoint->{2.663, -1.879, 0.910}
]
Clear[n]
:[font = input; preserveAspect; endGroup; ]
n = 4;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, Pi},
	ViewPoint->{2.663, -1.879, 0.910}
]
Clear[n]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
ds = Sqrt[x'[t]^2 + y'[t]^2 + z'[t]^2]//Simplify
:[font = text; inactive; preserveAspect; ]
Or somewhat fancier, looking ahead to vectors and dot products:
:[font = input; preserveAspect; ]
r[t_] = {x[t], y[t], z[t]};
ds = Sqrt[r'[t] . r'[t]]//Simplify
:[font = input; preserveAspect; ]
Integrate[ds, {t, 0, Pi}]
:[font = input; preserveAspect; ]
Do [
	length = NIntegrate[ds, {t, 0, Pi}];
	Print[n, "    ", length],
{n, 1, 6}]
Clear[n]
:[font = input; preserveAspect; endGroup; ]
Do [
	n = 2^k;
	length = NIntegrate[ds, {t, 0, Pi}];
	Print[n, "    ", length],
{k, 1, 8}]
Clear[n]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Case 1 (exercise 6)
:[font = input; preserveAspect; ]
x[t_] := Cos[2n t] Sin[t]
y[t_] := Sin[2n t] Sin[t]
z[t_] := 3 Cos[t]
:[font = input; preserveAspect; ]
r[t_] = {x[t], y[t], z[t]}
:[font = input; preserveAspect; ]
ds = Sqrt[r'[t] . r'[t]]//Simplify
:[font = input; preserveAspect; ]
Do [
	length = NIntegrate[ds, {t, 0, Pi}];
	Print[n, "    ", length],
{n, 1, 4}]
Clear[n]
:[font = input; preserveAspect; endGroup; ]
Do [
	n = 2^k;
	length = NIntegrate[ds, {t, 0, Pi}];
	Print[n, "    ", length],
{k, 1, 5}]
Clear[n]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Case 2 (not in project--would take too many hints)
:[font = input; preserveAspect; ]
x[t_] := Cos[2n t] Sin[t]
y[t_] := 3 Sin[2n t] Sin[t]
z[t_] := Cos[t]
:[font = input; preserveAspect; ]
x[t]^2 + (y[t]/3)^2 + z[t]^2//Simplify
:[font = input; preserveAspect; ]
r[t_] = {x[t], y[t], z[t]}
:[font = input; preserveAspect; ]
ds = Sqrt[r'[t] . r'[t]]//Simplify
:[font = input; preserveAspect; ]
Do [
	length = NIntegrate[ds, {t, 0, Pi}];
	Print[n, "    ", length],
{n, 1, 4}]
Clear[n]
:[font = input; Cclosed; preserveAspect; startGroup; ]
Do [
	n = 2^k;
	length = NIntegrate[ds, {t, 0, Pi}];
	Print[n, "    ", length],
{k, 1, 5}]
Clear[n]
:[font = message; inactive; preserveAspect; endGroup; ]
NIntegrate::ncvb: 
   NIntegrate failed to converge to prescribed accuracy
     after 7 recursive bisections in t near t = 1.50944
    .
:[font = input; preserveAspect; ]
dssq = ds^2//Expand
:[font = input; preserveAspect; ]
dssqlarge = Coefficient[dssq, n, 2]
:[font = input; preserveAspect; ]
n = 16;
2n NIntegrate[Sin[t] Sqrt[1 + 8 Cos[2n t]^2], {t, 0, Pi}]
Clear[n]
:[font = input; Cclosed; preserveAspect; startGroup; ]
n = 64;
2n NIntegrate[Sin[t] Sqrt[1 + 8 Cos[2n t]^2], {t, 0, Pi}]
Clear[n]
:[font = message; inactive; preserveAspect; endGroup; ]
NIntegrate::ncvb: 
   NIntegrate failed to converge to prescribed accuracy
     after 7 recursive bisections in t near t = 1.58307.
:[font = text; inactive; preserveAspect; ]
Seems  to be of order 8n
:[font = input; preserveAspect; ]
magic = Integrate[Sqrt[1 + 8 Cos[t]^2], {t, 0, Pi}]/Pi//N
:[font = input; preserveAspect; ]
Table[{n, 4n magic}, {n, 4, 64, 4}]//TableForm//N
Clear[n]
:[font = text; inactive; preserveAspect; ]
Wow!  But let's make sure n doesn't matter in the magic factor.
:[font = input; preserveAspect; ]
magic = Integrate[Sqrt[1 + 8 Cos[2t]^2], {t, 0, Pi}]/Pi//N
:[font = text; inactive; preserveAspect; ]
Remark: Could do general case of ellipsoid with semi-major axes a,b,c in a similar way.
Can reduce to scale factor times s ~ 2na Integrate[Sin[t] Sqrt[1 - p Cos[2nt]^2] ~  4na magic,
if a > b or to  ~ 2nb Integrate[Sin[t] Sqrt[1 - p Sin[2nt]^2 ]~  4na magic, if a < b.  These integrals are actually equal and we could make a table of `magic' factors:
:[font = input; preserveAspect; ]
Do [
	 f = NIntegrate[Sqrt[1 - p Cos[2t]^2], {t, 0, Pi}];
	 Print[p, "    ", f/N@Pi],
{p, 0.0, 1.0, 0.1}]
:[font = input; preserveAspect; endGroup; endGroup; ]
Do [
	 f = NIntegrate[Sqrt[1 - p Sin[2t]^2], {t, 0, Pi}];
	 Print[p, "    ", f/N@Pi],
{p, 0.0, 1.0, 0.1}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 7
:[font = input; preserveAspect; ]
x[t_] := Cos[n t]
y[t_] := Sin[n t]
z[t_] := t
:[font = input; preserveAspect; ]
n = 1;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, 2Pi},
	ViewPoint->{2.663, -1.879, 0.910}
]
Clear[n]
:[font = input; preserveAspect; ]
n = 2;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, 2Pi},
	ViewPoint->{2.663, -1.879, 0.910}
]
Clear[n]
:[font = input; preserveAspect; ]
n = 3;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, 2Pi},
	ViewPoint->{2.663, -1.879, 0.910}
]
Clear[n]
:[font = input; preserveAspect; endGroup; ]
n = 4;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, 2Pi},
	ViewPoint->{2.663, -1.879, 0.910}
]
Clear[n]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 9
:[font = input; preserveAspect; ]
n = 0;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, Pi},
	ViewPoint->{2.663, -1.879, 0.910}
]
Clear[n]
:[font = input; preserveAspect; endGroup; endGroup; ]
n = 1;
ParametricPlot3D[{x[t], y[t], z[t]}, {t, 0, Pi},
	ViewPoint->{2.663, -1.879, 0.910}
]
Clear[n]
^*)