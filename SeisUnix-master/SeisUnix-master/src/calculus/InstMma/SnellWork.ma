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
Snell's Law and Other Applications
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
f[x_] := 3 x^4 - 24 x^3 + 51 x^2 - 32 x + 64
:[font = input; preserveAspect; ]
Plot[f[x], {x, -10, 10}]
:[font = input; preserveAspect; ]
Plot[f[x], {x, 3, 6}]
:[font = input; preserveAspect; ]
signif = 6;
niter = 8;
xn = 3.4;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, signif] ],
{niter}]
:[font = input; preserveAspect; endGroup; ]
a = 3.4;
tangent = f[a] + f'[a](x - a);
Plot[{f[x], tangent}, {x, 3, 4}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
Clear[a, b, L, c1, c2]
d1 = Sqrt[a^2 + x^2];
d2 = Sqrt[(L-x)^2 + b^2];
eqn = (c2 x d2)^2 - (c1 (L - x) d1)^2 //Expand
:[font = input; preserveAspect; ]
Collect[eqn, x]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Check on equation for example 3
:[font = input; preserveAspect; endGroup; endGroup; ]
values = {a -> 1, b -> 1, L -> 4, c1 -> 1, c2 -> 1/2};
4 eqn /. values //Expand
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
s = 400; h = 1; R = 80000;
F[x_] := Cos[s/(R + x)] - (R + x)/(R + x + h)
:[font = input; preserveAspect; endGroup; ]
signif = 4;
niter = 8;
xn = 0.0;
Do[
    xn = xn - F[xn]/F'[xn];
    Print[ N[xn, signif] ],
{niter}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
s = 400; h = 1;
f[r_] := Cos[s/r] - r/(r + h)
:[font = input; preserveAspect; ]
Plot[f[r], {r, 10, 400}]
:[font = input; preserveAspect; ]
Plot[f[r], {r, 60, 70}]
:[font = input; preserveAspect; ]
signif = 6;
niter = 8;
xn = 62.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, signif] ],
{niter}]
:[font = input; preserveAspect; ]
signif = 6;
niter = 8;
xn = 65.5;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, signif] ],
{niter}]
:[font = text; inactive; preserveAspect; fontSize = 16; ]
Note: if r = 65.5, then circumference is:
:[font = input; preserveAspect; ]
2 Pi 65.5 //N
:[font = text; inactive; preserveAspect; fontSize = 16; ]
And sure enough, in walking 400m the traveling astronaut is only 11.5m BEHIND the starting point.
:[font = input; preserveAspect; ]
2 Pi 61.9 //N
:[font = text; inactive; preserveAspect; fontSize = 16; endGroup; endGroup; ]
And now she has tramped all the way around the asteroid PLUS an additional 11.1 m!
^*)