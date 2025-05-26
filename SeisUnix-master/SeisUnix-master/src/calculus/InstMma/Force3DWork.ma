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
Force Applications in 3D
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Fpoint definition
:[font = input; preserveAspect; endGroup; ]
Fpoint = -G M m/q^2
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = text; inactive; preserveAspect; ]
Comment: Notice that evenness-oddness considerations imply that the z-term integrates to zero in all 3 of these.  That's the mathematical implication of the symmetry.
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Rectangular
:[font = input; preserveAspect; endGroup; ]
rho g Integrate[(z + h + R),
	{z, -R, R},
	{y, -Sqrt[R^2 - z^2], Sqrt[R^2 - z^2]},
	{x, -Sqrt[R^2 - y^2 - z^2], Sqrt[R^2 - y^2 - z^2]}]//
Simplify
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Cross sections
:[font = input; preserveAspect; endGroup; ]
Pi rho g Integrate[(z + h + R) (R^2 - z^2), {z, -R, R}]//
Simplify
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Spherical
:[font = input; preserveAspect; endGroup; endGroup; ]
z = r Cos[phi];
rho g Integrate[(z + h + R) r^2 Sin[phi],
	{r, 0, R}, {phi, 0, Pi}, {theta, 0, 2Pi}]//Simplify
Clear[z]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Expansions about infinity
:[font = input; preserveAspect; endGroup; ]
Series[a/Sqrt[a^2+q^2], {q, Infinity, 3}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 8: Square plate
:[font = input; preserveAspect; ]
R = Sqrt[x^2 + y^2 + q^2];
F = - G M m q/a^2 Integrate[1/R^3,
			{x, -a/2, a/2}, {y, -a/2, a/2}]
:[font = input; preserveAspect; ]
F + O[q, Infinity]^5
:[font = input; preserveAspect; ]
Series[F, {q, Infinity, 3}]
:[font = input; preserveAspect; endGroup; ]
Table[{q, F/Fpoint} /. {a -> 1, q -> 2^k}, {k, 0, 8}]//
N//TableForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 9: Circular disk
:[font = input; preserveAspect; ]
Clear[R, F]
:[font = input; preserveAspect; ]
R = Sqrt[r^2 + q^2];
F = - G M m q/(Pi a^2) Integrate[r/R^3,
				{r, 0, a}, {theta, 0, 2Pi}]
:[font = input; preserveAspect; ]
F + O[q, Infinity]^5
:[font = input; preserveAspect; ]
Series[F, {q, Infinity, 3}]
:[font = input; preserveAspect; endGroup; endGroup; ]
Table[{q, F/Fpoint} /. {a ->1, q -> 2^k}, {k, 0, 8}]//
N//TableForm
^*)