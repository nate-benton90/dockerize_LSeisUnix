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
A Mathematica Tutorial for Calculus
;[s]
4:0,0;2,1;13,2;14,3;35,-1;
4:1,21,16,Times,1,24,0,0,0;1,21,16,Times,2,24,0,0,0;1,14,11,Times,0,16,0,0,0;1,21,16,Times,1,24,0,0,0;
:[font = section; inactive; preserveAspect; startGroup; ]
Introduction
:[font = text; inactive; preserveAspect; endGroup; ]
You need to know the material in the ``Front End Notes'' for your platform before starting this notebook (these notebooks are called MacFrontEnd.ma, etc.).   A small amount of material from that notebook is repeated in this one for the sake of completeness.  You are expected to ``execute'' the commands given and to make sure you understand the output.

 If you've gotten into this file by mistake, select ``quit'' in the menu to escape.

After reading this notebook, you should briefly examine the notebook Commands.ma which is a reference file containinga summary of the commands that you will be using this year .
;[s]
4:0,0;356,1;437,2;527,3;618,-1;
4:1,11,8,Times,0,12,0,0,0;1,13,10,Times,0,14,0,0,0;1,11,8,Times,0,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
What is Mathematica?
;[s]
3:0,0;8,1;19,2;21,-1;
3:1,16,12,Times,1,18,0,0,0;1,16,12,Times,2,18,0,0,0;1,12,9,Times,1,14,0,0,0;
:[font = text; inactive; preserveAspect; endGroup; ]
 Mathematica is a state-of-the-art and remarkably powerful system for doing mathematics by computer.  One can use Mathematica in many different ways, but in your calculus courses you will use only a small portion of its capabilities.   (You will use more advanced features in your later course work and,  very likely,  in your professional career).  The following tutorial sections will show you how to use to do numerical, symbolic and graphical calculations.   You will get a sense of the graphical power in this tutorial.
 
     One of the remarkable features of Mathematica  is its `symbolic' capabiltity;  i.e.  it ability to very quickly do messy algrebraic computations and simplifications.   Such packages can thus save us many tedious hours of (error-prone) work.   Similarly, you will see later this term that Mathematica can also perform calculus operations such as  differentiation and integration) .  These time saving features will not take all the work out of the calculus course,  rather they allow us to probe more deeply into central issues of the subject.
;[s]
10:0,0;1,1;12,2;114,3;125,4;566,5;577,6;820,7;831,8;1073,9;1075,-1;
10:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Basic elements of Mathematica
:[font = text; inactive; preserveAspect; ]
In this section we give you examples of Mathematica usage for some familiar and elementary operations. 
;[s]
3:0,0;40,1;51,2;106,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = subsection; inactive; preserveAspect; startGroup; ]
Executing a Mathematica command
;[s]
3:0,0;12,1;23,2;32,-1;
3:1,12,9,Times,1,14,0,0,0;1,13,10,Times,2,14,0,0,0;1,12,9,Times,1,14,0,0,0;
:[font = text; inactive; preserveAspect; ]
In the first example, we illustrate some of the algebraic and graphical capabilities of Mathematica.   Execute the command:
;[s]
4:0,0;88,1;99,2;103,3;123,-1;
4:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; startGroup; ]
y = x^3 - x^2 - 9 x + 9
:[font = output; output; inactive; preserveAspect; endGroup; ]
9 - 9*x - x^2 + x^3
;[o]
           2    3
9 - 9 x - x  + x
:[font = text; inactive; preserveAspect; endGroup; ]
The input line above is an example of writing the definition of  y  as a cubic polynomial in  x.  There are some items you should notice in the above command that are peculiar to the  Mathematica  program:
   �  "x cube"and "x square" are typed using the carat (^ ) . 
   �   product  "9 x" does not need a multiplication symbol.  Two safe 
        forms are:   with a space as  in "9 x" or  with an asterisk as in "9*x".
   �   The form 9x (no space) also works, but be aware that, in contrast, the symbol ax
        does NOT mean a*x!  It denotes a new variable with a two letter name.
        If we mean `a times x', we MUST write either a x (space) or a*x (asterisk) as in the
        preceeding paragraph.  Thus, we tend to put spaces  in something like 9 x, as
        a good habit, even though it is not necessary.
;[s]
3:0,0;184,1;195,2;821,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = subsection; inactive; preserveAspect; startGroup; ]
Plotting a function
:[font = text; inactive; preserveAspect; ]
The command below is a very common plotting format that will soon become familiar to you.  (Note: this plot command assumes the above definition of  y;  if you have not executed the command defining  y,  scroll back and do it now.)
:[font = input; preserveAspect; ]
Plot[y, {x,-7,7}, PlotRange -> All]
:[font = text; inactive; preserveAspect; ]
There are some items to note in this plot command that are peculiar to Mathematica:
   �   Notice the square brackets,  [  and  ],  around the arguments of the Plot 
        command.  Also note {x,-7,7}   specifies the domain of  x.   It is 
        important to remember when to use the different types of brackets.  
        Mathematica is very `narrow minded' about this.
   �  "PlotRange -> All" is a plotting option, telling Mathematica that---"yes, 
        we do want to see the entire graph".  (Other plotting options will be 
        introduced as we need them.)   The arrow (->) is created with the 
        "hyphen" followed by the  "greater than" keys on the keyboard.
   �  Notice the upper case letters in  Plot  and  PlotRange.  This is typical of 
       all Mathematica  functions and options;  so get used to this!  In contrast, 
       if  YOU define a new function, like  y  above,  you are free in the use of 
       upper and lower case letters.
;[s]
9:0,0;71,1;82,2;327,3;338,4;430,5;441,6;775,7;786,8;1500,-1;
9:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = text; inactive; preserveAspect; ]
As a contrast, examine the default Mathematica plot by executing:
;[s]
3:0,0;35,1;46,2;66,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
Plot[y, {x,-7,7}]
:[font = text; inactive; preserveAspect; endGroup; ]
You can see that the graph depicts the essential character of function y and would often--but not always--be a more valuable plot  than the one above.   
:[font = subsection; inactive; preserveAspect; startGroup; ]
Modifying a command
:[font = text; inactive; preserveAspect; ]
You can now practice modifying a Mathematica command and, at the same time, get a closer look at the graph:  in the following command line, change the plotting range to -4 to 4 (in place of -7 to 7):
;[s]
3:0,0;33,1;45,2;199,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; endGroup; ]
Plot[y, {x,-7,7}]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Factoring polynomials
:[font = text; inactive; preserveAspect; ]
In the above graph it looks to the eye as if the function y is equal to 0 somewhere around x = -3,  x = 1, and  x = 3.  How to find where  y=0 is an important question that we will pursue from several directions this semester.   But in this case, the question is easily answered because Mathematica is good at factoring polynomials.   To see this, execute the following command.
;[s]
3:0,0;287,1;298,2;455,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; startGroup; ]
Factor[y]
:[font = output; output; inactive; preserveAspect; endGroup; ]
(-3 + x)*(-1 + x)*(3 + x)
;[o]
(-3 + x) (-1 + x) (3 + x)
:[font = text; inactive; preserveAspect; endGroup; ]
It is now clear that y = 0 precisely at  x=-3,  x=1, and  x=3.
:[font = subsection; inactive; preserveAspect; startGroup; ]
Zooming in on a graph
:[font = text; inactive; preserveAspect; ]
Let's return to the graph of y and get into an issue that will be very important throughout our study of calculus.   Again,  execute:
:[font = input; preserveAspect; ]
Plot[y, {x,-4,4}]
:[font = text; inactive; preserveAspect; ]
A recuring theme in calculus can be paraphrased:  ``most functions are `almost linear' if you look closely enough.''  To illustrate this point, you can "zoom" in on the graph at  x  near  2: 
    1.  Modify the plot command below so that the domain of x is 1.9  to  2.1.   
    2.  Execute the plot command.
:[font = text; inactive; preserveAspect; ]
You should see a smaller section of the graph and note that it is starting to straighten out a bit.  To further illustrate the point,  modify the x domain in the plot command below to 1.99 to 2.01 and execute the command, and notice that the graph is almost linear. 
:[font = input; preserveAspect; ]
Plot[y, {x, 1.99, 2.01}]
:[font = text; inactive; preserveAspect; endGroup; ]
Is there anything special about the point  (2, -5)?  No.   You may want to experiment on your own by modifying the command with any value for the domain of  x that you choose, and "zoom in" as we have done.  Remember the fundamental point being made:  ``most functions are `almost linear' if you look closely enough.''  We will revisit this notion often in Calculus.
:[font = subsection; inactive; preserveAspect; startGroup; ]
Numerical Values
:[font = text; inactive; preserveAspect; ]
Mathematica tries to do EXACT arithmetic when possible.  Contrast the output of the following three commands:
;[s]
2:0,0;11,1;109,-1;
2:1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
Sqrt[4]
:[font = input; preserveAspect; ]
Sqrt[3]
:[font = input; preserveAspect; ]
Pi
:[font = text; inactive; preserveAspect; ]
Notice that as with all Mathematica built-in quantities Pi is spelled with a capital first letter!

To get numerical values, execute
;[s]
3:0,0;24,1;35,2;132,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
N[Sqrt[3]]
:[font = text; inactive; preserveAspect; ]
and
:[font = input; preserveAspect; ]
N[Pi]
:[font = text; inactive; preserveAspect; ]
You can control the precision of the result like this:
:[font = input; preserveAspect; ]
N[Pi, 25]
:[font = text; inactive; preserveAspect; ]
To avoid the ``missing bracket'' syndrome, we sometimes use the alternate notation:
:[font = input; preserveAspect; ]
Sqrt[3] //N
:[font = text; inactive; preserveAspect; ]
which you can think of as saying as an afterthought: ``Oh, and make that numerical.''  A final   example:
:[font = input; preserveAspect; ]
Pi/2 + 5 Sqrt[17] //N
:[font = text; inactive; preserveAspect; ]
is equivalent to:
:[font = input; preserveAspect; endGroup; ]
N[Pi/2 + 5 Sqrt[17]]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Expand, Factor, Rules
:[font = text; inactive; preserveAspect; ]
Below we point out a few more characteristics in some  Mathematica  commands that you will use during the year.  Execute each command, notice its output, and read the explanation that follows.
;[s]
3:0,0;55,1;66,2;806,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
y
:[font = text; inactive; preserveAspect; ]
Here we assume you excuted the definition  y given above.   Now consider
:[font = input; preserveAspect; ]
xy
:[font = text; inactive; preserveAspect; ]
Mathematica .  As discussed earlier,  xy is treated as a new variable with two letter name, since we `forgot' the space between  x  and  y.   Now execute:
;[s]
2:0,0;11,1;155,-1;
2:1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
x y 
:[font = text; inactive; preserveAspect; ]
Mathematica  remembers the definition of  y  and substitutes it in. 
;[s]
4:0,0;11,1;13,2;66,3;69,-1;
4:1,10,8,Times,2,12,0,0,0;1,13,10,Times,0,14,0,0,0;1,11,8,Times,0,12,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = input; preserveAspect; ]
Expand[%]
:[font = text; inactive; preserveAspect; ]
In this  Expand  command, the percent sign (%) stands for  `the last result'.  Therefore, the product x y  is expanded here to get a fourth degree polynomial.
:[font = input; preserveAspect; ]
Factor[%]
:[font = text; inactive; preserveAspect; ]
Likewise,  this factors the last result.   Finally, a bit more subtle command:
:[font = input; preserveAspect; ]
y /. x -> 2
:[font = text; inactive; preserveAspect; endGroup; ]
The effect of this command is to evaluate  y  at  x=2.  It illustrates a very important feature of  Mathematica.   You should read this command as "evaluate  y  subject to the rule:  (x  is set to 2)".  The operator  /.  is made up by a slash  /  followed by a period  (.);  and the arrow is a "hyphen" followed by a "greater than"  sign.   The spaces after the  y, before the  x  and before the  2  are not necessary here, but they help in the readability of the command.
;[s]
4:0,0;100,1;115,2;471,3;473,-1;
4:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = subsection; inactive; preserveAspect; startGroup; ]
Defining a  function
:[font = text; inactive; preserveAspect; ]
As you probably noticed, the last example is an awkward way to evaluate  y  at  x = 2.  An easier way to do such evaluations it is to use the "function" notation.  The following is an example of a function definition for  f :
:[font = input; preserveAspect; ]
f[x_] := x^2 + 1
:[font = text; inactive; preserveAspect; endGroup; ]
There are several items to note in the above command that are peculiar to  Mathematica  and that you must include to make  f  a legitimate function:  
    �  square brackets must be used to enclose the argument of  f,
    �  an underscore (shift on the hyphen) follows the argument inside the 
        brackets, and
    �  the assignment operator  :=  replaces the usual equal sign.   
;[s]
4:0,0;75,1;88,2;381,3;387,-1;
4:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = subsection; inactive; preserveAspect; startGroup; ]
Examples using a defined function
:[font = text; inactive; preserveAspect; ]
Some advantages of the function notation are shown in the  examples below.  These examples give you an idea of the power you have in Mathematica  when  f  is defined as an actual function. The original argument,  x, can be replaced by any numerical value or symbol.   EXECUTE the following commands, one at a time, starting with the definition of  f .
;[s]
3:0,0;133,1;144,2;354,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
f[x_] := x^2 + 1
:[font = input; preserveAspect; ]
f[x]
:[font = input; preserveAspect; ]
f[3]
:[font = input; preserveAspect; ]
f[-3]
:[font = input; preserveAspect; ]
f[-12]
:[font = input; preserveAspect; ]
f[p]
:[font = input; preserveAspect; ]
f[cat]
:[font = input; preserveAspect; ]
f[t^3+1]
:[font = input; preserveAspect; ]
Expand[%]
:[font = text; inactive; preserveAspect; endGroup; ]
As you can see, the original argument,  x, can be replaced by ANYTHING!  For example, with the argument  t^3 + 1  we obtained a sixth degree polynomial (the `composition' of the functions  x^2 + 5 and t^3 + 1).  
:[font = subsection; inactive; preserveAspect; startGroup; ]
Plotting two functions on a single graph
:[font = text; inactive; preserveAspect; ]
You can form a so-called ``rational function'' by taking the quotient of y and f[x].   Execute the following command to define the ratio of  y and  f[x], and then execute the second command to plot the function.
:[font = input; preserveAspect; ]
r = y/f[x]
:[font = input; preserveAspect; ]
Plot[r, {x, -20, 20}, PlotRange -> All]
:[font = text; inactive; preserveAspect; ]
You can see here that  r  has the same general shape,  for "small" x, as did  y.  (Can you tell why?)  Also notice that for "large x" the graph of  r  looks linear.  (Can you tell why?)  To explore the second question graphically we plot functions  r and  x-1  on the same graph as follows.  Execute the command:
:[font = input; preserveAspect; ]
Plot[{r, x-1}, {x, -40, 40}, PlotRange -> All]
:[font = text; inactive; preserveAspect; endGroup; endGroup; endGroup; ]
In this tutorial, you have seen demonstrated two phenomena that are quite common:
    1.  Most functions look linear if you zoom in very close; that is, if you look 
          at the function over a very small interval.
    2.  Some (but not most) functions look linear if you look at them from a 
          large distance.  
The concepts of "very close" and "very far" play central roles in calculus, as you will see this semester.
^*)