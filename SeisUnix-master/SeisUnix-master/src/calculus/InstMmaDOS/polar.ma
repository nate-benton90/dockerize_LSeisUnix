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
Plotting in Polar Coordinates
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
PolarAnimation Module 
:[font = input; preserveAspect; ]
Clear[PolarAnimation]
PolarAnimation[r_, {T1_, T2_, dT_}, opts___Rule] :=
Module[
	{q, T},
	Do[
		string = StringForm["`1` Pi", N[T/Pi]];
		label = FontForm[string, {"Times-Bold", 16}];
	
		q = {Cos[T], Sin[T]};
		ParametricPlot[
			{r[t] Cos[t], r[t] Sin[t]}, {t, 0, T},
			opts,
			PlotLabel -> label,
			AspectRatio -> Automatic,
			Axes -> Automatic,
			Epilog ->
				Show[Graphics[{PointSize[0.03], Point@q}],
					DisplayFunction -> Identity
				] [[1]]
		],
	{T, T1, T2, dT}]
]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Example
:[font = input; preserveAspect; endGroup; endGroup; ]
k = 2;
r[t_] := Sin[k t]
PolarAnimation[r, {Pi/4, 2Pi, Pi/4},
	PlotRange -> {{-1.1, 1.1}, {-1.1, 1.1}}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Introductory Example
:[font = input; preserveAspect; ]
<<Graphics`Master`
:[font = input; preserveAspect; ]
PolarPlot[Sin[2t], {t, 0, 2Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[2t], {t, 0, Pi}]
:[font = input; preserveAspect; endGroup; ]
PolarPlot[Sin[3t], {t, 0, Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Sin(k theta), k = 1, 2, 3, 4, 5
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
k = 1, completes in Pi, has 1 leaf
:[font = input; preserveAspect; endGroup; ]
PolarPlot[Sin[t], {t, 0, Pi}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
k = 2, completes in 2Pi, has 4 leaves
:[font = input; preserveAspect; endGroup; ]
PolarPlot[Sin[2t], {t, 0, 2Pi}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
k = 3,  completes in Pi, has 3 leaves
:[font = input; preserveAspect; endGroup; ]
PolarPlot[Sin[3t], {t, 0, Pi}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
k = 4, completes in 2Pi, has 8 leaves
:[font = input; preserveAspect; endGroup; ]
PolarPlot[Sin[4t], {t, 0, 2Pi}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
k = 5, completes in Pi, has 5 leaves
:[font = input; preserveAspect; endGroup; endGroup; ]
PolarPlot[Sin[5t], {t, 0, Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Sin(theta / k), k=2, 3, 4, 5, 6,7
:[font = input; preserveAspect; ]
PolarPlot[Sin[t/2], {t, 0, 4Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[t/3], {t, 0, 3Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[t/4], {t, 0, 8Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[t/5], {t, 0, 5Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[t/6], {t, 0, 12Pi}]
:[font = input; preserveAspect; endGroup; ]
PolarPlot[Sin[t/7], {t, 0, 7Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Sin(b theta), b = m/n, for new cases with n=3,4,5,6,7
:[font = input; preserveAspect; ]
PolarPlot[Sin[2t/3], {t, 0, 6Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[3t/4], {t, 0, 8Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[2t/5], {t, 0, 10Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[3t/5], {t, 0, 5Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[4t/5], {t, 0, 10Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[5t/6], {t, 0, 12Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[2t/7], {t, 0, 14Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[3t/7], {t, 0, 7Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[4t/7], {t, 0, 14Pi}]
:[font = input; preserveAspect; ]
PolarPlot[Sin[5t/7], {t, 0, 7Pi}]
:[font = input; preserveAspect; endGroup; ]
PolarPlot[Sin[6t/7], {t, 0, 14Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Sin(b theta), b = Sqrt[2]
:[font = input; preserveAspect; ]
PolarPlot[Sin[Sqrt[2]t], {t, 0, 12Pi},
	PlotPoints -> 75
]
:[font = input; preserveAspect; ]
PolarPlot[Sin[Sqrt[2]t], {t, 0, 24Pi},
	PlotPoints -> 75
]
:[font = input; preserveAspect; endGroup; ]
PolarPlot[Sin[Sqrt[2]t], {t, 0, 48Pi},
	PlotPoints -> 75
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
1 + p Sin(theta)
:[font = input; preserveAspect; ]
p = 3/4;
PolarPlot[1 + p Sin[t], {t, 0, 2Pi}]

:[font = input; preserveAspect; ]
p = 1;
PolarPlot[1 +p Sin[t], {t, 0, 2Pi}]

:[font = input; preserveAspect; ]
p = 4/3;
PolarPlot[1 + p Sin[t], {t, 0, 2Pi}]

:[font = input; preserveAspect; ]
p = 1/2;
PolarPlot[1 + p Sin[t], {t, 0, 2Pi}]

:[font = input; preserveAspect; ]
p = 1/4;
PolarPlot[1 + p Sin[t], {t, 0, 2Pi}]

:[font = input; preserveAspect; ]
p = 0;
PolarPlot[1 +p Sin[t], {t, 0, 2Pi}]

:[font = input; preserveAspect; ]
p = 4;
PolarPlot[1 + p Sin[t], {t, 0, 2Pi}]

:[font = input; preserveAspect; endGroup; ]
p = 16;
PolarPlot[1 + p Sin[t], {t, 0, 2Pi}]

:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Creative Plots
:[font = input; preserveAspect; ]
niceplot[t_] := 3 Cos[t]^2 - 1
PolarPlot[niceplot[t], {t, 0, 2Pi}]
:[font = input; preserveAspect; ]
niceplot[t_] := 3 Cos[t]^3 - 1
PolarPlot[niceplot[t], {t, 0, 2Pi}]
:[font = input; preserveAspect; endGroup; endGroup; ]
niceplot[t_] := 3 Cos[3t/4]^3 - 1
PolarPlot[niceplot[t], {t, 0, 8Pi},
	PlotPoints -> 75
]
^*)