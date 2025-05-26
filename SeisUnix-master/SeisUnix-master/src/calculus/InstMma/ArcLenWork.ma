(*^

::[paletteColors = 128; automaticGrouping; currentKernel; 
	fontset = title, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e8,  24, "Times"; ;
	fontset = subtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e6,  18, "Times"; ;
	fontset = subsubtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, italic, L1, e6,  14, "Times"; ;
	fontset = section, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, grayBox, M22, bold, L1, a20,  18, "Times"; ;
	fontset = subsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, blackBox, M19, bold, L1, a15,  14, "Times"; ;
	fontset = subsubsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, whiteBox, M18, bold, L1, a12,  12, "Times"; ;
	fontset = text, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  16, "Times"; ;
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
ArcLength Problems
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Introduction
:[font = input; initialization; preserveAspect; ]
*)
ds[y_, x_] := Sqrt[1 + y'[x]^2]
(*
:[font = input; preserveAspect; ]
y[x_] := x^2
:[font = input; preserveAspect; ]
dsParabola = ds[y, x]
:[font = input; preserveAspect; ]
sParabola[x_] = Integrate[dsParabola, x]
:[font = input; preserveAspect; ]
sParabola[1] - sParabola[0]
:[font = input; preserveAspect; ]
sParabola[1] - sParabola[0] //N
:[font = input; preserveAspect; endGroup; ]
y[x_] := x^3
Integrate[ds[y,x], x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = input; preserveAspect; ]
y[x_] := x^p
ds[y, x]
:[font = input; preserveAspect; endGroup; ]
Table[Solve[2p-2==n, p], {n, -2, 2}]//TableForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
y[x_] := x^3/3 + 1/(4x)
ds[y, x]^2 //Expand
:[font = input; preserveAspect; endGroup; ]
y[x_] := Log[x] - x^2/8
ds[y, x]^2 //Expand
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
f[x_] := x^2
Integrate[f[x] - 1/(4f[x]), x]		(* y *)
Integrate[f[x] + 1/(4f[x]), x]		(* s *)
:[font = input; preserveAspect; endGroup; ]
f[x_] := 1/x
Integrate[f[x] - 1/(4f[x]), x]		(* y *)
Integrate[f[x] + 1/(4f[x]), x]		(* s *)
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Arch of Sine Curve
:[font = input; preserveAspect; ]
y[x_] := Sin[x]
ds[y, x]
:[font = input; preserveAspect; endGroup; ]
Integrate[ds[y, x], x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
y[x_] := x^4
Integrate[ds[y,x], x]
:[font = input; preserveAspect; ]
y[x_] := Sqrt[x]
Integrate[ds[y,x], x]
:[font = input; preserveAspect; endGroup; ]
y[x_] := x^(1/4)
Integrate[ds[y,x], x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
y[x_] := x^3/3 + 1/(4x)
Integrate[ds[y,x], x] //Simplify
:[font = input; preserveAspect; ]
y[x_] := Log[x] - x^2/8
Integrate[ds[y,x], x] //Simplify
:[font = input; preserveAspect; endGroup; ]
x^2/8 + Integrate[1/x, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 7
:[font = input; preserveAspect; endGroup; ]
y[x_] := Sin[x]
NIntegrate[ds[y, x], {x, 0, Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 8
:[font = input; preserveAspect; endGroup; ]
y[x_] := Sin[x]
Table[{N[b], NIntegrate[ds[y,x], {x, 0, b}]},
{b, 0, Pi, Pi/8}] //TableForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 9
:[font = input; preserveAspect; endGroup; endGroup; ]
y[x_] := Sin[x]
Plot[NIntegrate[ds[y,x], {x, 0, b}], {b, 0, Pi}]
^*)