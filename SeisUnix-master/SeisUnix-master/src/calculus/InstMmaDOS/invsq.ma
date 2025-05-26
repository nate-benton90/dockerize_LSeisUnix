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
Inverse Square Law Problems
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Illustrative Problem: particle outside a sphere
:[font = input; preserveAspect; ]
R = Sqrt[q^2 + r^2 - 2 q r Cos[phi]];
F = -3 G M m/(4 Pi a^3) Integrate[(q - r Cos[phi])/R^3
	r^2 Sin[phi], {theta, 0, 2Pi}, {r, 0, b}, {phi, 0, Pi}]
:[font = text; inactive; preserveAspect; ]
Watch out--we need Sqrt [ (a - q)^2] = q- a, so PowerExpand is not the answer here.   Could do by hand, but here's a little artifice to get what we want.
:[font = input; preserveAspect; ]
% /. a -> q - e //PowerExpand//Simplify
:[font = text; inactive; preserveAspect; ]
Verify that PowerExpand is wrong.  However, this is the correct result for Exercise 2!
:[font = input; preserveAspect; endGroup; ]
F//PowerExpand//Simplify
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Hollow Sphere
:[font = input; preserveAspect; endGroup; ]
R = Sqrt[q^2 + r^2 - 2 q r Cos[phi]];
F = -3 G M m/(4 Pi (b^3 - a^3)) *
	 Integrate[(q - r Cos[phi])/R^3 r^2 Sin[phi],
	 {theta, 0, 2Pi}, {r, a, b}, {phi, 0, Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Cylinder
:[font = input; preserveAspect; ]
R = Sqrt[r^2 + (q - z)^2];
F = - G M m/(Pi a^2 2h) Integrate[r (q - z)/R^3,
		{theta, 0, 2Pi},
		{z, -h, h},
		{r, 0, a}
]
:[font = input; preserveAspect; ]
F = % /. q -> h + d//PowerExpand
:[font = input; preserveAspect; ]
F = % /. d -> q - h//Simplify
:[font = input; preserveAspect; ]
F + O[q, Infinity]^4
:[font = input; preserveAspect; ]
Series[F, {q, Infinity, 3}]
:[font = input; preserveAspect; ]
Fpoint = -G M m/q^2;
:[font = input; preserveAspect; endGroup; ]
Table[{q, F/Fpoint} /. {a -> 1, h -> 1/2, q -> 2^k},
{k, 0, 8}]//N//TableForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Infinite plate
:[font = input; preserveAspect; endGroup; endGroup; ]
R = Sqrt[x^2 + y^2 + q^2];
F = - G rho m q Integrate[1/R^3,
			{x, -Infinity, Infinity},
			{y, -Infinity, Infinity}
]//PowerExpand
^*)