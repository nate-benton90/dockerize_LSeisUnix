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
Derivatives, Slopes and Tangents
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = text; inactive; preserveAspect; ]
Loop for figures 1 and 2.
:[font = input; preserveAspect; ]
a  = 2.0; h = 4.0;
Do[
	h = h/2;
	Plot[x^2, {x, a-h, a+h}],
{5}]
:[font = text; inactive; preserveAspect; ]
Near an extremum, the curve doesn't get linear as you zoom (not in the project).
:[font = input; preserveAspect; endGroup; ]
a  = 0.0; h = 4.0;
Do[
	h = h/2;
	Plot[x^2, {x, a-h, a+h}],
{5}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
Clear[h];  (* given a value in exercise 1 *)
f[x_] := x^3
Expand[(f[x+h]-f[x])/h]
:[font = input; preserveAspect; ]
% /. h -> 0
:[font = input; preserveAspect; ]
f[x_] := x^4
Expand[(f[x+h]-f[x])/h]
:[font = input; preserveAspect; endGroup; ]
% /. h -> 0
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
f[x_] := (x^3 - 5)(x^2 - 1) / (x^2 + 1)
:[font = input; preserveAspect; ]
b = 2.1;
slope = (f[b] - f[2]) / (b - 2.0);
secant = f[2.0] + slope (x - 2.0);
Print["slope is ", slope]
Plot[{secant, f[x]}, {x, 1.5, 3.0},
        PlotStyle -> {GrayLevel[0.0], GrayLevel[0.5]}
]
:[font = input; preserveAspect; endGroup; ]
f'[x] /. x -> 2 //N
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
x^3
:[font = input; preserveAspect; ]
a  = 2.0; h = 4.0;
Do[
	h = h/2;
	Plot[x^3, {x, a-h, a+h}],
{5}]
:[font = text; inactive; preserveAspect; ]
Using front end, picked the points on the right side of the following equation and then edited in the left side to get values stored in variables.
:[font = input; preserveAspect; ]
{{x1,y1},{x2,y2}} =
	{{1.983211, 7.817194}, {2.016789, 8.234265}}
:[font = input; preserveAspect; ]
approxSlope = (y2-y1)/(x2-x1)
:[font = input; preserveAspect; endGroup; ]
trueSlope = D[x^3, x] /. x -> 2
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
x^4
:[font = input; preserveAspect; ]
a  = 2.0; h = 4.0;
Do[
	h = h/2;
	Plot[x^4, {x, a-h, a+h}],
{5}]
:[font = input; preserveAspect; ]
{{x1,y1},{x2,y2}} =
	{{1.985077, 15.635866}, {2.018654, 16.702468}};
:[font = input; preserveAspect; ]
approxSlope = (y2-y1)/(x2-x1)
:[font = input; preserveAspect; endGroup; endGroup; ]
trueSlope = D[x^4, x] /. x -> 2
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
f[x_] := (x^3 - 5)(x^2 - 1) / (x^2 + 1)
a  = 2.0; h = 4.0;
Do[
	h = h/2;
	Plot[f[x], {x, a-h, a+h}],
{5}]
:[font = input; preserveAspect; endGroup; ]
{{x1,y1},{x2,y2}} =
	{{1.986942, 1.719979}, {2.012125, 1.917256}};
approxSlope = (y2-y1)/(x2-x1)
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
g[x_] := (x - 2)^(2/3) + 2 x^3
:[font = input; preserveAspect; ]
h = 4;
Do[
	h = h/2;
	Plot[g[x], {x, 2-h, 2+h},
		PlotLabel ->
		FontForm[2 - Log[2,h], {"Times-Bold", 16}]
	],
{16}]
:[font = text; inactive; preserveAspect; ]
Calculations for n=4
:[font = input; preserveAspect; ]
{{x1,y1},{x2,y2}} =
	{{1.95063, 16.976447}, {1.997943, 18.10063}};
leftslope = (y2-y1)/(x2-x1)
:[font = input; preserveAspect; ]
{{x1,y1},{x2,y2}} =
	{{2, 18.10063}, {2.026742, 18.743021}};
rightslope = (y2-y1)/(x2-x1)
:[font = text; inactive; preserveAspect; ]
Calculations for n=8
:[font = input; preserveAspect; ]
{{x1,y1},{x2,y2}} =
	{{1.996917, 16.080606}, {1.99988, 16.12563}};
leftslope = (y2-y1)/(x2-x1)
:[font = input; preserveAspect; ]
{{x1,y1},{x2,y2}} =
	{{1.99988, 16.12563}, {2.001683, 16.180659}};
rightslope = (y2-y1)/(x2-x1)
:[font = text; inactive; preserveAspect; ]
Calculations for n=12
:[font = input; preserveAspect; ]
{{x1,y1},{x2,y2}} =
	{{1.999985, 16.007944}, {2.000113, 16.012937}};
rightslope = (y2-y1)/(x2-x1)
:[font = text; inactive; preserveAspect; ]
Just playing around with Mathematica
:[font = input; preserveAspect; ]
Limit[g'[x], x -> 2, Direction -> 1]
:[font = input; preserveAspect; ]
Limit[g'[x], x -> 2, Direction -> -1]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Table relative size of derivatives
:[font = input; preserveAspect; ]
d1 = D[(2-x)^(2/3), x]
:[font = input; preserveAspect; ]
d2 = D[2x^3, x]
:[font = input; preserveAspect; ]
Table[{k+2, -d1, d2} /. x -> 2.0 + 1/2^k, {k, -1, 20}]//
TableForm
:[font = input; preserveAspect; endGroup; ]
Table[{k+2, -d1, d2} /. x -> 2.0 - 1/2^k, {k, -1, 20}]//
TableForm
:[font = subsection; inactive; preserveAspect; startGroup; ]
Figure 5
:[font = input; preserveAspect; endGroup; endGroup; ]
Plot[(2-x)^(2/3), {x, 1, 3}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 7
:[font = input; preserveAspect; ]
f[x_] := Sin[x]
a  = Pi/3; h = 4.0;
Do[
	h = h/2;
	Plot[f[x], {x, a-h, a+h}],
{7}]
:[font = input; preserveAspect; ]
{{x1,y1},{x2,y2}} =
	{{1.020453, 0.852666}, {1.023706, 0.854487}};
approxSlope = (y2-y1)/(x2-x1)
trueSlope = Cos[Pi/3]//N
:[font = input; preserveAspect; ]
f[x_] := Sin[x]
a  = Pi/6; eps = 4.0;
Do[
	eps = eps/2;
	Plot[f[x], {x, a-eps, a+eps}],
{6}]
:[font = input; preserveAspect; endGroup; ]
{{x1,y1},{x2,y2}} =
	{{0.499633, 0.480143}, {0.523972, 0.501529}};
approxSlope = (y2-y1)/(x2-x1)
trueSlope = Cos[Pi/6]//N
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Slope Function Examples (Instructors Only)
:[font = text; inactive; preserveAspect; ]
The following examples show that we can get a graphical derivative function that is sometimes very revealing.
:[font = input; preserveAspect; ]
f[x_] := x^2; dx = .01;
fs[x_] := (f[x + dx] - f[x])/dx /; dx != 0.0
Plot[{f[x], fs[x]}, {x, -2, 2},
	AspectRatio -> Automatic,
	PlotStyle -> {GrayLevel[0], GrayLevel[0.5]}
]
:[font = input; preserveAspect; ]
Table[{x, fs[x]}, {x, -2, 2, .5}] //TableForm
:[font = input; preserveAspect; ]
f[x_] := x^3; dx = .01;
fs[x_] := (f[x + dx] - f[x])/dx /; dx != 0.0
Plot[{f[x], fs[x]}, {x, -2, 2},
	PlotStyle -> {Dashing[{.05, .05}], {}}]
:[font = input; preserveAspect; ]
TableForm[Table[{x, fs[x]}, {x, -4, 4, .5}],
	TableHeadings -> {None, {"f", "fs"}}]
:[font = input; preserveAspect; ]
f[x_] := Sin[x]; dx = .01;
fs[x_] := (f[x + dx] - f[x])/dx /; dx != 0.0
Plot[{f[x], fs[x]}, {x, -2Pi, 2Pi},
	PlotStyle -> {Dashing[{.05, .05}], {}}]
:[font = input; preserveAspect; ]
{shift, amp} = {1.547149, 1.023194}
:[font = input; preserveAspect; ]
Plot[{f[x], fs[x-shift]}, {x, -2Pi, 2Pi},
	PlotStyle -> {Dashing[{.05, .05}], {}}]
:[font = input; preserveAspect; ]
f[x_] := 2^x; dx = .01;
fs[x_] := (f[x + dx] - f[x])/dx /; dx != 0.0
Plot[{f[x], fs[x]}, {x, -2, 2},
	PlotStyle -> {Dashing[{.05, .05}], {}}]
:[font = input; preserveAspect; ]
Table[fs[x]/f[x], {x, -2, 2, .5}] //TableForm
:[font = input; preserveAspect; ]
f[x_] := 3^x; dx = .01;
fs[x_] := (f[x + dx] - f[x])/dx /; dx != 0.0
Plot[{f[x], fs[x]}, {x, -2, 2},
	PlotStyle -> {Dashing[{.05, .05}], {}}]
:[font = input; preserveAspect; ]
Table[fs[x]/f[x], {x, -2, 2, .5}] //TableForm
:[font = input; preserveAspect; ]
f[x_] := 2.7^x; dx = .01;
fs[x_] := (f[x + dx] - f[x])/dx /; dx != 0.0
Plot[{f[x], fs[x]}, {x, -2, 2},
	PlotStyle -> {Dashing[{.05, .05}], {}}]
:[font = input; preserveAspect; endGroup; endGroup; ]
Table[fs[x]/f[x], {x, -2, 2, .5}] //TableForm
^*)