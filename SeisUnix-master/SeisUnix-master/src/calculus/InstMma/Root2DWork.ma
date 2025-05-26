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
FindRoot & Friends in 2D
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Illustrative Example
:[font = input; preserveAspect; ]
p = 4; q = 1; r = 0;
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Can't Solve with Solve
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
vars = {a, b};
Solve[eqns, vars]
:[font = input; preserveAspect; endGroup; ]
N[%]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Could Solve with NSolve
:[font = input; preserveAspect; endGroup; ]
Clear[a,b,f,g]
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
vars = {a,b};
NSolve[eqns, vars]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
FindRoot[eqns, {a,p/2}, {b,q/2}]
:[font = input; preserveAspect; endGroup; ]
{f[a,b], g[a,b]} /. % //Chop
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
FindMinimum on related distance function
:[font = input; preserveAspect; ]
h[x_,y_] := x^2 + 4y^2
d[x_,y_,p_,q_,r_] :=
	Sqrt[(x-p)^2 + (y-q)^2 + (h[x,y]-r)^2]

FindMinimum[d[x,y,p,q,r], {x,p/2}, {y,q/2}]
:[font = input; preserveAspect; ]
{min, pointrule} =
	FindMinimum[d[x,y,p,q,r], {x,p/2}, {y,q/2}];
:[font = input; preserveAspect; ]
min
:[font = input; preserveAspect; endGroup; endGroup; ]
d[x,y,p,q,r] /. pointrule
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = text; inactive; preserveAspect; ]
The point is outside the paraboloid and "low", so expect unique root.
:[font = input; preserveAspect; ]
p = 2; q = 2; r = 0;
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
NSolve
:[font = input; preserveAspect; endGroup; ]
Clear[a,b,f,g]
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
vars = {a,b};
NSolve[eqns, vars]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
FindRoot[eqns, {a,p/2}, {b,q/2}]
:[font = input; preserveAspect; endGroup; ]
{f[a,b], g[a,b]} /. % //Chop
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
FindMinimum on related distance function
:[font = input; preserveAspect; endGroup; endGroup; ]
h[x_,y_] := x^2 + 4y^2
d[x_,y_,p_,q_,r_] :=
	Sqrt[(x-p)^2 + (y-q)^2 + (h[x,y]-r)^2]

FindMinimum[d[x,y,p,q,r], {x,p/2}, {y,q/2}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = text; inactive; preserveAspect; ]
The point is outside the paraboloid and "high", so expect several roots.
:[font = input; preserveAspect; ]
p = 4; q = 1; r = 4;
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
NSolve
:[font = input; preserveAspect; endGroup; ]
Clear[a,b,f,g]
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
vars = {a,b};
NSolve[eqns, vars]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot
:[font = input; preserveAspect; ]
Clear[a,b,f,g]
f[a_, b_] :=  2 a^3 + 8 a b^2 + (1 - 2 r) a - p
g[a_, b_] := 32 b^3 + 8 b a^2 + (1 - 8 r) b - q

eqns = {f[a,b]==0,g[a,b]==0};
FindRoot[eqns, {a,p/2}, {b,q/2}]
:[font = input; preserveAspect; endGroup; ]
{f[a,b], g[a,b]} /. % //Chop
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
FindMinimum on related distance function
:[font = input; preserveAspect; endGroup; endGroup; ]
h[x_,y_] := x^2 + 4y^2
d[x_,y_,p_,q_,r_] :=
	Sqrt[(x-p)^2 + (y-q)^2 + (h[x,y]-r)^2]

FindMinimum[d[x,y,p,q,r], {x,p/2}, {y,q/2}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
2D minimum problem: y = x^2
:[font = input; preserveAspect; ]
Clear[h,d]
h[x_] := x^2   (* parabola *)
p = 2; q = 1;
d[x_,p_,q_] :=
	Sqrt[(x-p)^2 + (h[x]-q)^2]

{min, pointrule} =
	FindMinimum[d[x,p,q], {x,p/2}]
:[font = input; preserveAspect; endGroup; ]
h[x] /. pointrule
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
3D minimum problem: z = x^2 + 4 y^2
:[font = input; preserveAspect; ]
Clear[h,d]
h[x_,y_] := x^2 + 4y^2 (* paraboloid *)
p = 3; q = 2; r = 1;
d[x_,y_,p_,q_,r_] :=
	Sqrt[(x-p)^2 + (y-q)^2 + (h[x,y]-r)^2]

{min, pointrule} =
	FindMinimum[d[x,y,p,q,r], {x,p/2}, {y,q/2}]
:[font = input; preserveAspect; endGroup; ]
h[x, y] /. pointrule
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
4D minimum problem: w = x^2 + 4 y^2 + 9 z^2
:[font = input; preserveAspect; ]
Clear[h,d]
h[x_,y_,z_] := x^2 + 4y^2 + 9z^2  (* hyper-paraboloid *)
p = 4; q = 3; r = 2; s = 1;
d[x_,y_,z_,p_,q_,r_,s_] :=
	Sqrt[(x-p)^2 + (y-q)^2 + (z-r)^2 + (h[x,y,z]-s)^2]

{min, pointrule} =
	FindMinimum[d[x,y,z,p,q,r,s], {x,p/2}, {y,q/2}, {z,r/2}]
:[font = input; preserveAspect; endGroup; ]
h[x, y, z] /. pointrule
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
5D minimum problem: v = x^2 + 4 y^2 + 9 z^2 + 16 w^2
:[font = input; preserveAspect; ]
Clear[h,d]
h[x_,y_,z_,w_] := x^2 + 4y^2 + 9z^2 + 16w^2
p = 5; q = 4; r = 3; s = 2; t = 1;
d[x_,y_,z_,w_,p_,q_,r_,s_,t_] :=
	Sqrt[(x-p)^2 + (y-q)^2 + (z-r)^2 + 
		(w-s)^2 + (h[x,y,z,w]-t)^2]

{min, pointrule} =
	FindMinimum[d[x,y,z,w,p,q,r,s,t], {x,p/2}, {y,q/2},
		{z,r/2}, {w,s/2}]
:[font = input; preserveAspect; endGroup; endGroup; ]
h[x, y, z, w] /. pointrule
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
NSolve
:[font = input; preserveAspect; endGroup; ]
Clear[x,y,f,g]
f[x_, y_] :=  x^2 - 2x + 2y^2 - 4y - 4
g[x_, y_] := 10 - 15 Sin[(x + 2y)/5]

eqns = {f[x,y]==0,g[x,y]==0};
vars = {x,y};
NSolve[eqns, vars]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Solve by FindRoot--use starting point (3.0, 0.1)
:[font = input; preserveAspect; ]
Clear[x,y,f,g]
f[x_, y_] :=  x^2 - 2x + 2y^2 - 4y - 4
g[x_, y_] := 10 - 15 Sin[(x + 2y)/5]

eqns = {f[x,y]==0,g[x,y]==0};
FindRoot[eqns, {x, 3.0}, {y, 0.1}]
:[font = input; preserveAspect; endGroup; endGroup; ]
{f[x,y], g[x,y]} /. % //Chop
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
Clear[h,x,y]
h[x_,y_] := Exp[-9x + x^2 + y^2 + y/2]

FindMinimum[h[x,y], {x,4.6}, {y,0}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Sensitivity
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,5}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,10}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,11}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,12}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,4}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,1}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,-1}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,-2}, {y,0}]
:[font = input; preserveAspect; endGroup; endGroup; ]
FindMinimum[h[x,y], {x,-3}, {y,0}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5a (not in project)
:[font = input; preserveAspect; ]
Clear[h,x,y]
h[x_,y_] := -Exp[9x - x^2 - y^2 - y/2]

FindMinimum[h[x,y], {x,4.6}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,5}, {y,0}]
:[font = text; inactive; preserveAspect; ]
So no problem?
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,10}, {y,0}]
:[font = input; preserveAspect; endGroup; ]
FindMinimum[h[x,y], {x,11}, {y,0}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
Clear[h,x,y]
h[x_,y_] := -Exp[2x - x^2 - y^2] -
				0.1(x^2 + y^2 - 2y + 1)

FindMinimum[h[x,y], {x,1}, {y,0}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Sensitivity
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,0}, {y,0}]
:[font = input; preserveAspect; ]
FindMinimum[h[x,y], {x,2}, {y,0}]
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
FindMinimum[h[x,y], {x,3}, {y,0}]
^*)