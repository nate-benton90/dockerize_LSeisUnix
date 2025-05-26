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
FindRoot & Friends
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Solving the Quadratic with Solve
:[font = input; preserveAspect; ]
quadeqn = a x^2 + b x + c == 0;
solutions = Solve[quadeqn, x] //Simplify
:[font = input; preserveAspect; endGroup; ]
quadeqn /. solutions //Simplify
:[font = section; inactive; preserveAspect; startGroup; ]
Solving a Cubic with NSolve
:[font = input; preserveAspect; ]
lhs = x^3 - 3x^2 + 1;
solutions = NSolve[lhs == 0, x]
:[font = input; preserveAspect; ]
lhs /. solutions
:[font = input; preserveAspect; ]
lhs /. solutions //Chop
:[font = input; preserveAspect; ]
solutions1 = Solve[lhs == 0, x] //N
:[font = input; preserveAspect; ]
solutions1 //Chop
:[font = input; preserveAspect; endGroup; ]
solutions1 = Solve[lhs == 0.0, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Using FindRoot on the Asteroid Equation
:[font = input; preserveAspect; ]
s = 400; h = 1;
equation = Cos[s/r] == r/(r + h)
NSolve[equation, r]
:[font = input; preserveAspect; endGroup; ]
lhs = Cos[s/r] - r/(r + h);
approxsoln = 80000;
FindRoot[lhs == 0, {r, approxsoln}]
solution = N[ FindRoot[lhs, {r, approxsoln}], 16]
lhs /. solution

:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
equation = x^2 -3x + 4 == 0;
Solve[equation, x]
:[font = input; preserveAspect; endGroup; ]
NSolve[equation, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
equation = x^5 -3x + 4 == 0;
Solve[equation, x]
:[font = input; preserveAspect; endGroup; ]
NSolve[equation, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; endGroup; ]
equation = x^3 - 68x^2 + 1100x - 5000 == 0;
NSolve[equation, x]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; endGroup; ]
equation = x^3 - 2a x^2 - 4 x + 8a == 0
rules = Solve[equation, x]
equation /. rules
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
Plot[Tan[x] + x, {x, 0, 4Pi}]
:[font = input; preserveAspect; ]
lhs = Tan[x] + x;
soln = FindRoot[lhs == 0, {x, 2}]
lhs /. soln
:[font = input; preserveAspect; ]
soln = FindRoot[lhs == 0, {x, 5}]
lhs /. soln
:[font = input; preserveAspect; ]
soln = FindRoot[lhs == 0, {x, 8}]
lhs /. soln
:[font = input; preserveAspect; endGroup; endGroup; ]
soln = FindRoot[lhs == 0, {x, 11}]
lhs /. soln
^*)