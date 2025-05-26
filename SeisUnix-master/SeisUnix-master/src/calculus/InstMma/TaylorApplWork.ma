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
Taylor Series--Advanced Usage
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Introduction
:[font = input; preserveAspect; ]
Series[ArcSin[x], {x, 0, 8}]
:[font = input; preserveAspect; ]
ArcSin[x] + O[x]^9
:[font = input; preserveAspect; ]
Series[Sin[x], {x, Pi/4, 2}]
:[font = input; preserveAspect; endGroup; ]
Sin[x] + O[x, Pi/4]^3
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = text; inactive; preserveAspect; ]
Remember that for functions of a single variable, f@x is the same as  f[x] is the same as x//f.
:[font = input; preserveAspect; ]
ArcSin@ArcTan@x + O[x]^9
:[font = input; preserveAspect; endGroup; ]
ArcTan@ArcSin@x + O[x]^9
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Horner Function
:[font = input; preserveAspect; ]
Horner[p_?PolynomialQ, x_] :=
	Fold[x #1 + #2 &, 0, Reverse@CoefficientList[p, x]]
:[font = input; preserveAspect; ]
p = 3 + 2 x + 4 x^2 + 7 x^3 + 2 x^4;
p //FortranForm
:[font = input; preserveAspect; endGroup; ]
Horner[p, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Getting Fortran or C Code
:[font = input; preserveAspect; ]
p[x_] = ArcSin[x] + O[x]^9 //Normal
:[font = input; preserveAspect; ]
arcsin7 = Horner[p[x], x]
:[font = text; inactive; preserveAspect; ]
Wow!  Note the continuation character in column 6!
:[font = input; preserveAspect; ]
N[arcsin7]//FortranForm
:[font = input; preserveAspect; endGroup; ]
p = ArcSin[x] + O[x]^9 //Normal;
Horner[p, x] //N//FortranForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
p = ArcSin@ArcTan@x + O[x]^9 //Normal;
Horner[p, x] //N//FortranForm
:[font = input; preserveAspect; endGroup; ]
Horner[p, x] //N//CForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Limits by Series
:[font = input; preserveAspect; ]
(Sin[x] - x + x^3/6)/x^5 + O[x]^4
:[font = input; preserveAspect; endGroup; ]
Limit[(Sin[x] - x + x^3/6)/x^5, x -> 0]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
(Cos[x] - 1 + x^2/2)/x^4 + O[x]^4
:[font = input; preserveAspect; ]
Limit[(Cos[x] - 1 + x^2/2)/x^4, x -> 0]
:[font = input; preserveAspect; ]
(Sin[x^3] - Sin[x]^3)/x^5 + O[x]^4
:[font = input; preserveAspect; ]
Limit[(Sin[x^3] - Sin[x]^3)/x^5, x -> 0]
:[font = input; preserveAspect; ]
(Sin[Tan[x]] - Tan[Sin[x]])/x^7 + O[x]^4
:[font = input; preserveAspect; endGroup; ]
Limit[(Sin[Tan[x]] - Tan[Sin[x]])/x^7, x -> 0]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exponential DE
:[font = input; preserveAspect; ]
y = 1 + Sum[a[k] x^k, {k, 1, 3}] + O[x]^4
:[font = input; preserveAspect; ]
D[y, x] == y
:[font = input; preserveAspect; ]
Solve[%]
:[font = input; preserveAspect; ]
y /. %
:[font = text; inactive; preserveAspect; fontSize = 16; fontName = "Times"; ]
Let's collapse our procedure:
:[font = input; preserveAspect; endGroup; ]
Clear[y, a]
order = 3;
y = 1 + Sum[a[k] x^k, {k, 1, order}] + O[x]^(order+1);
y /. Solve[D[y, x] == y]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
Clear[y, a]
order = 3;
y = 1 + Sum[a[k] x^k, {k, 1, order}] + O[x]^(order+1);
y /. Solve[D[y, x] == y + x]
:[font = input; preserveAspect; ]
Clear[y];
DSolve[{y'[x] == y[x] + x, y[0] == 1}, y[x], x]
:[font = input; preserveAspect; ]
Clear[y, a]
order = 3;
y = 1 + Sum[a[k] x^k, {k, 1, order}] + O[x]^(order+1);
y /. Solve[D[y, x] == y^2 + x^2]
:[font = input; preserveAspect; endGroup; endGroup; ]
Clear[y];
DSolve[{y'[x] == y[x]^2 + x^2, y[0] == 1}, y[x], x]
^*)