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
Limits and Continuity
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Instructor Note Material
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Limit Help Examples
:[font = input; preserveAspect; startGroup; ]
?Limit
:[font = info; inactive; preserveAspect; endGroup; ]
Limit[expr, x->x0] finds the limiting value of expr when x
   approaches x0.
:[font = input; preserveAspect; ]
Options[Limit]
:[font = input; preserveAspect; startGroup; ]
?Direction
:[font = info; inactive; preserveAspect; endGroup; endGroup; ]
Direction is an option for Limit which specifies the
   direction towards which to approach the limit.
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
LimitTable (from Calculus.m)
:[font = text; inactive; preserveAspect; ]
This assumes that you've loaded in Calculus.m.
:[font = input; preserveAspect; startGroup; ]
?LimitTable
:[font = info; inactive; preserveAspect; endGroup; ]
LimitTable[f[x], {x, a, (n), (h)}] 

 Tabulate f[x] at n values near a

 a      the point at which the limit is being investigated
 n       the number of points to tabulate, default is 10
 h       the first point is computed as a+h.  Use negative
   values for left approach to limit
          default is 1
 Typically issue the command like this:
 LimitTable[x^2, {x, 2}] //TableForm
:[font = input; preserveAspect; ]
f[x_] := (Sqrt[25 + x] - Sqrt[25 - x])/x
:[font = input; preserveAspect; ]
LimitTable[f[x], {x, 0}]//TableForm
:[font = input; preserveAspect; endGroup; endGroup; ]
LimitTable[f[x], {x, 0, 5}]//TableForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; endGroup; ]
f[x_] :=  x^3 + 3x^2
f[2]
f[x] /. x -> 2
f[t]
f[cat]
Table[f[n], {n, 1, 5}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
Factor[(x^3 - 9x^2 - 45x - 91)/(x - 13)]
Factor[(x^3 - 9x^2 - 39x - 86)/(x - 13)]
Factor[(x^4 - 26x^3 + 178x^2 - 234x + 1521)/(x - 13)]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Behind the scenes: how the problem was constructed
:[font = input; preserveAspect; ]
(x-13)(x^2 + 4x + 7) //Expand
:[font = input; preserveAspect; ]
next = (x-13)(x^2 + 4x + 7)+(5+6x) //Expand
:[font = input; preserveAspect; ]
Factor[next]
:[font = input; preserveAspect; ]
(x-13)^2(x^2 + 9) //Expand
:[font = input; preserveAspect; endGroup; endGroup; ]
Factor[%]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Answer
:[font = input; preserveAspect; ]
f[x_] := (Sqrt[25 + 3x] - Sqrt[25 - 2x])/x
:[font = input; preserveAspect; ]
Plot[f[x], {x, -1, 1}]
:[font = input; preserveAspect; ]
Table[{x, f[x]}, {x, -1.0, 1.0, 0.2}]//TableForm
:[font = input; preserveAspect; ]
Table[{x, f[x]}, {x, -1.0, 1.0, 0.2}]//Chop//TableForm
:[font = input; preserveAspect; ]
a = 0; ntimes = 5;
Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}] //
N // TableForm
Clear[a, ntimes];
:[font = input; preserveAspect; endGroup; ]
Table[{x, f[x]} /. x -> a - 1/2^k, {k, 1, ntimes}] //
N // TableForm
Clear[a, ntimes];
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Instructor Note -- use of LimitTable command from Calculus.m
:[font = input; preserveAspect; endGroup; ]
LimitTable[f[x], {x, 0}]//TableForm
:[font = subsection; inactive; preserveAspect; startGroup; ]
Instructor Note -- use of Series command
:[font = input; preserveAspect; endGroup; endGroup; ]
Series[f[x], {x, 0, 3}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
(x^3 - x^3 - 4x + 4)/(x - 1) /. x -> 0
:[font = input; preserveAspect; ]
f[x_] := Sin[x] / x 
a = 0; ntimes = 10;
Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}] //
N // TableForm
Clear[a, ntimes];
:[font = input; preserveAspect; ]
f[x_] := (1 - Cos[x] )/ x 
a = 0; ntimes = 10;
Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}] //
N // TableForm
Clear[a, ntimes];
:[font = input; preserveAspect; ]
f[x_] := Sin[5 x] / x
a = 0; ntimes = 10;
Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}] //
N // TableForm
Clear[a, ntimes];
:[font = input; preserveAspect; endGroup; ]
f[x_] := (1 + x)^(1/x) 
a = 0; ntimes = 10;
Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}] //
N // TableForm
Clear[a, ntimes];
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = text; inactive; preserveAspect; ]
The student level answer to the second part is to just edit the statement: to:
:[font = input; preserveAspect; ]
f[x_] := x^3 + 2 /; x <= -1
f[x_] := x^4 + 1 /; x >= 1
f[x_] := x^2 + x/2 + 1/2
:[font = text; inactive; preserveAspect; ]
The ``which'' code for f[x]:
:[font = input; preserveAspect; endGroup; ]
Clear[f]
f[x_] :=
    Which[
            (* If *)     x <= -1, (* then *) x^3 + 2,
            (* Elseif *) x >= 1,  (* then *) x^4 + 1,
            (* Else *)   True,    (* then *) x^2 + x + 1
    ]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Coming Attraction Material
:[font = input; preserveAspect; ]
Sin[Tan[x]] - Tan[Sin[x]] + O[x]^11
:[font = input; preserveAspect; ]
Sin[Tan[x]] +O[x]^11 //Normal//TeXForm
:[font = input; preserveAspect; ]
Tan[Sin[x]] +O[x]^11 //Normal//TeXForm
:[font = input; preserveAspect; ]
f[x_] := (Sin[Tan[x]] - Tan[Sin[x]]) /x^7
a = 0; ntimes = 10;
Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}] //
N //TableForm
Clear[a, ntimes];
:[font = input; preserveAspect; ]
f[x_] := (Sin[Tan[x]] - Tan[Sin[x]]) /x^7
a = 0; ntimes = 10;
N[Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}], 24] //
TableForm
Clear[a, ntimes];
:[font = input; preserveAspect; ]
Limit[1/Sin[x]^2 - 1/x^2, x -> 0]
:[font = input; preserveAspect; endGroup; endGroup; ]
f[x_] := (Sin[Tan[x]] - Tan[Sin[x]]) /x^7
Limit[f[x], x -> 0]
^*)