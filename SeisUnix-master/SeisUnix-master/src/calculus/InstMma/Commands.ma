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
 Some Mathematica commands
;[s]
4:0,0;6,1;17,2;18,3;26,-1;
4:1,21,16,Times,1,24,0,0,0;1,21,16,Times,2,24,0,0,0;1,14,11,Times,1,16,0,0,0;1,16,12,Times,1,18,0,0,0;
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Introduction
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
Below we give a list of Mathematica  commands that you will use this year.   But first,  recall that there are two basic ways to define a function of  x.   The quick way is, for example:
:[font = input; preserveAspect; ]
y = 3 x + 1
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
The lengthier way, but which allows more flexibility, is:
:[font = input; preserveAspect; ]
f[x_] := x^3 + x^2 - 5x - 5
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
Note:  both the x _  symbol on the left,  and the  :=  defining symbol.   Defining  f  this way allows you to easily evaluate, say,  f[2] ,  f[5 t + 3], etc.  In contrast,  to evaluate the above   y  at  x = 2   you must type:

:[font = input; preserveAspect; ]
y /. x -> 2
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
Remember that Mathematica tends to do exact arithmetic;  for example, carrying the fraction  2358463 / 78932749.   This is time-consuming for the computer and one usually prefers the decimal approximation.   One way  to get the decimal form is to use the  N  command; e.g. 
:[font = input; preserveAspect; ]
N[2358463/78932749]
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
Or, you can simply put a decimal point after one of the integers.  Try it. 
If you really need lots of decimal places of p you can type, e.g.:
:[font = input; preserveAspect; endGroup; ]
goodPi = N[Pi, 50]
:[font = section; inactive; preserveAspect; startGroup; ]
Getting Help during a  Mathematica  session
;[s]
4:0,0;23,1;34,2;36,3;43,-1;
4:1,16,12,Times,1,18,0,0,0;1,16,12,Times,2,18,0,0,0;1,13,10,Times,0,14,0,0,0;1,16,12,Times,1,18,0,0,0;
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
Suppose that you have used a command before, but have forgotten its exact format.  You can simple type,  for example:

:[font = input; preserveAspect; ]
?Plot
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; endGroup; ]
Or if you want more detail, type   ??Plot .   Try it.

:[font = section; inactive; preserveAspect; startGroup; ]
A Warning about variable names
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; endGroup; ]
Should you have another Mathematica notebook open, the two notebooks will share the same variables.   For example, suppose in another notebook you had defined a variable  y  differently that we have above.  Then you come to this notebook and execute the above  y  definition.   When you return to the other notebook  y  would be changed to the above. 
    
      However,  if you simply open this notebook to refresh your memory or to copy a command, but do no executions in this notebook,  then there is no problem.   
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
A Partial List of  Mathematica Commands.
;[s]
5:0,0;19,1;30,2;31,3;39,4;40,-1;
5:1,16,12,Times,1,18,0,0,0;1,16,12,Times,2,18,0,0,0;1,13,10,Times,0,14,0,0,0;1,16,12,Times,1,18,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
The  following commands are in alphabetical order.  If the command is on a separate line, as in the example for  D  below, you can execute it if you wish; ASSUMING the above  y  and  f[x]  have been executed.
:[font = subsubsection; inactive; preserveAspect; fontLeading = 9; startGroup; ]
Abs---take the absolute value of its argument; e.g.
:[font = input; preserveAspect; endGroup; ]
Abs[f[2]]
:[font = subsubsection; inactive; preserveAspect; ]
Clear---is used to "undefine" variables or functions.  This is useful when you are changing the definition of f,  or things have gotten confused.  For example, Clear[f,  y] clears above functions.
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
D---will take the derivative of specified function with respect to specfied variable.  E.g.,  
:[font = input; preserveAspect; endGroup; ]
D[f[x], x]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Do---allows you to set up a "loop" in order to perform the same (or similar) commands numerous times.  E.g.,  the following does 6 Newton iterations on function  t^2 - 5,  to approximate  Sqrt[5]:
:[font = input; preserveAspect; ]
t = 3.0
Do[t = t - (t^2 - 5.0) / (2.0 t), {k, 1, 6}]
t
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; endGroup; ]
The variable  k  is a `counter' that here takes on values 1,2,3,4,5,6.
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Expand---expands out in powers of x (or some other expression).
E.g., consider these two examples:
:[font = input; preserveAspect; ]
Expand[y f[x]]
:[font = input; preserveAspect; endGroup; ]
Expand[Cos[x] (3 - Cos[x]^2)]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Integrate---the first form of  Integrate  attempts to find an antiderivative of the first argument, with respect to the second.  
:[font = input; preserveAspect; ]
Integrate[y, x]
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
The second form is for doing a definite integral;  e.g. 
:[font = input; preserveAspect; ]
Integrate[y, {x, 0, 3}]
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; endGroup; ]
(Should this fail,  see  NIntegrate  below).
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Factor---will attempt to factor the argument.  In polynomials, it only looks for  integer factors.  E.g.,
:[font = input; preserveAspect; endGroup; ]
Factor[f[x]]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Limit---will attempt to take the specified limit.  
:[font = input; preserveAspect; ]
Limit[y, x -> 3]
:[font = text; inactive; preserveAspect; ]
The following takes the limit from the right  (use  +1  to get the limit from the left):
:[font = input; preserveAspect; endGroup; ]
Limit[f[x], x -> 0, Direction -> -1]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
N---converts the argument to decimal form and shows  6  figures.   More figures are shown if requested;  e.g.,
:[font = input; preserveAspect; ]
N[Pi]
:[font = input; preserveAspect; endGroup; ]
N[Pi/2, 16]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
NIntegrate---attempts to do a numerical integration of the first argument.   Typically, the result is good to 16 figures.  E.g.,
:[font = input; preserveAspect; ]
NIntegrate[y, {x, 0, 3}]
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; endGroup; ]
(Also see Integrate  above).
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Plot---plots one or more functions over the specified interval.  There are many options (try ??Plot), the most common is perhaps PlotRange.  E.g.  try this with,  and without,  the option:
:[font = input; preserveAspect; ]
Plot[{y, f[x]}, {x,-4,4}, PlotRange -> All]

:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
To get a label on a plot we can do e.g.:
:[font = input; preserveAspect; ]
Plot[{y, f[x]}, {x,-4,4}, PlotRange -> All,
      PlotLabel -> "J. Jacobs Project No. 6"]
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; endGroup; ]
where a `Return' (not `Enter') was used to go to a new line.
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Sign---produces the  sign  (+1  or  -1)  of the argument;  e.g.
:[font = input; preserveAspect; ]
Sign[-Pi/2]
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
This function is useful in producing "step functions";  e.g.,   
:[font = input; preserveAspect; endGroup; ]
g[x_] := Sign[x - 1] - Sign[x - 3]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Simplify---attempts to clean up the expression by looking for common factors, etc.  E.g., 
:[font = input; preserveAspect; endGroup; ]
Simplify[(f[x]- y) / (x + 3)]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Solve---attempts to solve the specified equation (or equations) for the specified variables.  Solve is effective primarily for polyomial equations.  E.g. this finds x-values at which f[x] and y are equal:
:[font = input; preserveAspect; ]
ourRules = Solve[y == f[x], x]
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
The result of this is a set of "rules", saved in a variable we called `ourRules', which can be used as follows to evaluate y at these x values:
:[font = input; preserveAspect; ]
y /. ourRules
:[font = help; inactive; preserveAspect; fontSize = 12; fontName = "Times"; ]
Next we solve a pair of equations for unknown s, t:
:[font = input; preserveAspect; endGroup; ]
Solve[{3 s + t == 7,  2 s - 3 t == 1}, {s,t}]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Sum---this command allows you to conveniently perform a  sum,  as a `counter ' (k  below) takes on its range of values.   E.g.  to compute the sum of squares of the first six integers: 
:[font = input; preserveAspect; endGroup; ]
sumsq = Sum[k^2, {k, 1, 6}]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Table and TableForm---Table is used, for example, to generate a list of order pairs; and
TableForm puts them in a more readable form;  e.g.
:[font = input; preserveAspect; endGroup; ]
Table[{n, n^2}, {n, 1, 5}] //TableForm
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Together---puts terms in a sum over a common denominator, and does some simplifying.   E.g.,
:[font = input; preserveAspect; ]
 (1 + x) y + f[x] / (1 - x)

:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
Together[%]
^*)