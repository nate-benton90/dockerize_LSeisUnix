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
Work Along a Curve
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part a
:[font = input; preserveAspect; endGroup; ]
Sqrt[(3-1)^2 + (3-2)^2] +
	Sqrt[(4-3)^2 + (5-3)^2] +
		Sqrt[(5-4)^2 + (5-5)^2] +
			Sqrt[(6-5)^2 + (8-5)^2] //N
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part c
:[font = input; preserveAspect; ]
f[x_] := x^2 - 4
:[font = input; preserveAspect; endGroup; endGroup; ]
Sqrt[(.5-0)^2 + (f[.5]-f[0])^2] +
	Sqrt[(1-.5)^2 + (f[1]-f[.5])^2] +
		Sqrt[(1.5-1)^2 + (f[1.5]-f[1])^2] +
			Sqrt[(2-1.5)^2 + (f[2]-f[1.5])^2] //N
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part a
:[font = input; preserveAspect; endGroup; ]
Sqrt[(3-1)^2 + (3-2)^2] (3-2)/(3-1) +
	Sqrt[(4-3)^2 + (5-3)^2] (5-3)/(4-3) +
		Sqrt[(5-4)^2 + (5-5)^2] (5-5)/(5-4) +
			Sqrt[(6-5)^2 + (8-5)^2] (8-5)/(6-5) //N
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part c
:[font = input; preserveAspect; endGroup; endGroup; ]
2(Sqrt[(.5-0)^2 + (f[.5]-f[0])^2] (f[.5]-f[0])+
Sqrt[(1-.5)^2 + (f[1]-f[.5])^2] (f[1]-f[.5]) +
	Sqrt[(1.5-1)^2 + (f[1.5]-f[1])^2] (f[1.5]-f[1]) +
	Sqrt[(2-1.5)^2 + (f[2]-f[1.5])^2] (f[2]-f[1.5])) //N
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part a
:[font = input; preserveAspect; ]
h[x_] := Sqrt[4 - x^2]
a = -2;  b = 2;
ArcLength = Integrate[Sqrt[1 + h'[x]^2], {x, a, b}]
:[font = input; preserveAspect; endGroup; ]
h[x_] := Sqrt[4 - x^2]
a = -2;  b = 2;
ArcLength = NIntegrate[Sqrt[1 + h'[x]^2], {x, a, b}]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part b
:[font = input; preserveAspect; ]
h[x_] := x^2 - 4
a = 0;  b = 2;
ArcLength = Integrate[Sqrt[1 + h'[x]^2], {x, a, b}]
:[font = input; preserveAspect; endGroup; endGroup; ]
N[ArcLength]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part a
:[font = input; preserveAspect; ]
h[x_] := x^2 - 4
F[x_] := 3x
a = 0;  b = 2;
Work = Integrate[F[x] Sqrt[1 + h'[x]^2], {x, a, b}]
:[font = input; preserveAspect; endGroup; ]
N[Work]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part b
:[font = input; preserveAspect; endGroup; ]
h[x_] := Sin[3x] / (1 + x^2)
F[x_] := 2 x^2
a = 0;  b = Pi;
ArcLength = NIntegrate[Sqrt[1 + h'[x]^2], {x, a, b}]
Work = NIntegrate[F[x] Sqrt[1 + h'[x]^2], {x, a, b}]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Part c
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
F[x_] := 3 h'[x]
Work = NIntegrate[F[x] Sqrt[1 + h'[x]^2], {x, a, b}]
^*)