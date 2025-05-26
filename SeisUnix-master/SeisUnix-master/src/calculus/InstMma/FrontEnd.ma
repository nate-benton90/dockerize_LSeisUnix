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
Mathematica Front End Notes for the NeXT
;[s]
3:0,0;11,1;12,2;40,-1;
3:1,21,16,Times,2,24,0,0,0;1,14,11,Times,0,16,0,0,0;1,21,16,Times,1,24,0,0,0;
:[font = section; inactive; preserveAspect; startGroup; ]
Working with this ``notebook''
:[font = text; inactive; preserveAspect; ]
Before you can read through this notebook,  you need to know several things about "getting around" inside the notebook window.  This section shows you how to use the mouse to `scroll',  and how to select, change and delete text. 
:[font = subsection; inactive; preserveAspect; startGroup; ]
Learning how to `scroll' through this notebook
:[font = text; inactive; preserveAspect; ]
The mouse is the small rectangular box that is attached to the keyboard.  By sliding the mouse on a smooth surface (such as a  pad), you can control the movement of the cursor on the screen.  You move the cursor to point to items you want to manipulate.   There are two "buttons" on the mouse, either of which will  (unless altered)  work for your purposes.
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Scrolling by using the arrows
:[font = text; inactive; preserveAspect; endGroup; ]
You can use the mouse and the direction arrows at the bottom left of the window to scroll up or down through the tutorial:
   1.  Point the cursor on the `down' arrow that appears at the extreme lower 
         left corner of  this notebook window,
   2.  Click the mouse button once (the text will move up one line in the 
         window);  or simply
   3.  Press (and hold down) the button until the desired amount of new text 
         appears.
You can similarly scroll up,  or backwards,  by using the `up' arrow.
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
Scrolling by using the `scroll bar'
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
 There is a faster way to move up and down.  Inside the grey margin strip at the left of the window is a short vertical bar with a small circle in the center (the scroll bar).  You can use this bar to scroll quickly down, or up,  the page:
    1.  Point the cursor on the scroll bar, 
    2.  Press the button (hold it down), and 
    3.  Drag the scroll bar down or up to correctly position the text.
:[font = subsection; inactive; preserveAspect; startGroup; ]
More about the mouse and the cursor
:[font = text; inactive; preserveAspect; ]
You will notice that the cursor changes shape as it moves about the screen.  If you move the cursor from a position inside this window to a border of the page, or off the page,  the cursor will change from a vertical I-beam shape to an arrow.   Or,  if you slowly move down the page the cursor will occasionally change, briefly,  to a horizontal I-beam shape.   (This occurs between "cells"s,  where a cell is a block of text defined by the bracket,  ] ,  to the right---e.g. see the  ]  for this paragraph).   Move the cursor around and see it change. 
:[font = text; inactive; preserveAspect; endGroup; ]
The cursor has three basic shapes:
   �  a vertical I-beam, for selecting text in the window; 
   �  a horizontal I-beam, for opening more work space between paragraphs 
       (or cells);
   �  the arrow,  for pointing to items bordering,  or off,  the window.
You will have occasion soon to see the implications of these various shapes.
:[font = subsection; inactive; preserveAspect; startGroup; ]
Adding new text
:[font = text; inactive; preserveAspect; endGroup; ]
 You can change (edit) text by using the mouse.  You can practice adding text right now: 
    1.  Move the cursor to the end of a  nearby paragraph, 
    2.  Click the mouse -- a blinking vertical bar will appear, 
    3.  Add new text there by typing something;  e.g. type:   `Some new stuff' 
          after this sentence.   
:[font = subsection; inactive; preserveAspect; startGroup; ]
Deleting text
:[font = text; inactive; preserveAspect; ]
Here is how to delete the text added above:
   1.  Move the cursor to the beginning of your added text,  just before the "S" 
         in Some , 
   2.  `Select' the text  (i.e.  press and drag the mouse to the right until the  
         `Some new stuff'  is completely shaded in grey), 
   3.  Hit the Delete key (located on the upper right of the main keyboard, with 
         the left arrow).  The shaded text will disappear.
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
By combining the above features, moving about, adding and deleting text, one can change the text as desired.   You will get more practice below.
:[font = section; inactive; preserveAspect; startGroup; ]
How to continue this Tutorial---or Quit
:[font = text; inactive; preserveAspect; endGroup; ]
Below you will see four section titles which serve as a `table of contents' for the rest of this tutorial.  These sections are "closed up" for convenience in finding the section of interest.   For example,  if you needed to get off the computer now,  you would want to "open up" the sections  "How to exit this tutorial"  and  "Logging off the NeXT".   But,  let's assume that you want to open the next section: Important Techniques: ``Copy and Paste'', ``Cut''.  To do this:
  1.  Put the cursor on the small rectangle to the right of the section title;
  2.  "double-click"---that is,  make two short clicks of the mouse.  
       This  (short) section should open up for you.   Try it.
  
If you want to close up that section,  double-click on the  ]  to the right, enclosing the entire section,  including the title.  Try it.

Notice that the section,  "How to exit this tutorial", has a  relatively small rectangle following it--this is because it is a smaller section.

This tutorial covers the basics of using Mathematica on the NeXT computer, your next step is to read the notebook Tutorial.ma which provides an introduction to the use of Mathematica itself.
;[s]
5:0,0;1017,1;1028,2;1147,3;1158,4;1167,-1;
5:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Executing a Mathematica  command
;[s]
4:0,0;12,1;23,2;25,3;33,-1;
4:1,16,12,Times,1,18,0,0,0;1,16,12,Times,2,18,0,0,0;1,12,9,Times,1,14,0,0,0;1,16,12,Times,1,18,0,0,0;
:[font = text; inactive; preserveAspect; ]
Here is an example of an expression that defines  y, a function of  x:
:[font = input; preserveAspect; ]
y = x^3 - x^2 - 9 x + 9
:[font = text; inactive; preserveAspect; ]
The above line is called a command, or "input".  When you enter such an input, Mathematica processes it and returns "output" (or a result). To obtain the result you must first "execute" the command by following these steps:
   1.  Move the cursor to the end of the command;  in this case, after the 
         second  9.   
        (Alternatively,  put the cursor on the  ]  to the right of the command),
   2.  Click the mouse to select the command,
   3.  Hit the Enter key,  located on the extreme lower right of the keyboard.  
        (Note: this is  NOT the same as the Return key that you have been 
         using.)
	
The above command is repeated below.   Follow the directions above to execute it now.  There will be a short delay while Mathematica performs the command. Once the command is completed, you will see an input line labeled  In[1]  that contains the original command, and an output line labeled  Out[1].  Notice that the result in the output line is written differently than the way we typed it;  Mathematica  prefers to write polynomials in increasing powers of  x.
;[s]
8:0,0;79,1;90,2;746,3;757,4;1019,5;1032,6;1087,7;1089,-1;
8:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = input; preserveAspect; ]
y = x^3 - x^2 - 9 x + 9
:[font = subsection; inactive; preserveAspect; startGroup; ]
Writing a Mathematica command
;[s]
3:0,0;10,1;21,2;29,-1;
3:1,12,9,Times,1,14,0,0,0;1,13,10,Times,2,14,0,0,0;1,12,9,Times,1,14,0,0,0;
:[font = text; inactive; preserveAspect; endGroup; ]
The input line above is an example of writing the definition of  y  as the cubic polynomial in  x.  There are two items you should notice in the above command that are peculiar to the  Mathematica  program:
   �  "x cube"and "x square" are typed using the carat (^ ) . 
   �   product  "9 x" does not need a multiplication symbol.  Two possible 
        forms are:   with a space as in "9 x" or  with an asterisk as in "9*x".
 
;[s]
7:0,0;185,1;196,2;198,3;206,4;207,5;425,6;428,-1;
7:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = subsection; inactive; preserveAspect; startGroup; ]
Opening Mathematica work space
;[s]
3:0,0;8,1;19,2;31,-1;
3:1,12,9,Times,1,14,0,0,0;1,13,10,Times,2,14,0,0,0;1,12,9,Times,1,14,0,0,0;
:[font = text; inactive; preserveAspect; ]
Now it's time for you to practice writing a command in Mathematica.  
However, first you must `open' a line to type in as follows:   
   1.  Move the cursor slowly down between two paragraphs  (e.g. between 
         this paragraph and the next one)  until the cursor becomes a  horizontal 
         I-beam.
   2.  Click the mouse;  a grey line will appear across the window. 
;[s]
3:0,0;55,1;66,2;377,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = text; inactive; preserveAspect; endGroup; ]
For practice, after opening a line,  type the same Mathematica  command that we worked with above, and execute the command.  (Remember:  to execute a command,  first `select' it by clicking behind the command,  and then hit the Enter key.) 
;[s]
3:0,0;51,1;62,2;241,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = subsection; inactive; preserveAspect; startGroup; ]
Modifying a command
:[font = text; inactive; preserveAspect; ]
You can now practice modifying a Mathematica command and, at the same time, get a closer look at the graph.
;[s]
4:0,0;33,1;45,2;106,3;108,-1;
4:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = text; inactive; preserveAspect; ]
Follow these steps to change the domain of  x in the plot command below:
   �  Carefully place the cursor in front of the  -7;
   �  Press and drag the mouse across the -7,7 (it will be shaded grey);
   �  Type  -4,4  (the  -7,7  will be replaced).
Now execute the modified plot command. (If the command does not execute, it's always a good idea to check for typographical errors. If there are no errors  and the graph still does not plot, ask for help.)
:[font = input; preserveAspect; ]
Plot[y, {x,-7,7}, PlotRange -> All]
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
You will now see a more detailed graph of y where it crosses the x-axis.
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Important Techniques:  ``Copy and Paste'', ``Cut''
:[font = subsection; inactive; preserveAspect; startGroup; ]
Copying and Pasting
:[font = text; inactive; preserveAspect; ]
You will find it very convenient in working with Mathematica to``copy and paste'' commands---to avoid retyping the same, or similar, commands.  Let's see how to copy commands from this notebook to the blank Mathematica window behind this window.  Notice that if you click on any portion of the blank window it moves to the front; and to return to this window,  you click on it.   Try it (and notice the blank window has the label ``Untitled-1'' on its top border).   

Consider the following two commands:
;[s]
8:0,0;49,1;60,2;207,3;218,4;219,5;464,6;469,7;507,-1;
8:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,14,11,Times,0,16,0,0,0;1,11,8,Times,0,12,0,0,0;1,14,11,Times,0,16,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
y = x^2 - 9
:[font = input; preserveAspect; ]
Plot[y, {x, 0, 4}]
:[font = text; inactive; preserveAspect; endGroup; ]
To copy these two commands onto the blank window:
  *  Click on the small  ]  to the right of the first command.   (The  ]  
      should be shaded gray),
  *  Hold down a Command key and click on the  ]  for the Plot  
      command.  (Both  brackets  should be shaded gray).
  *  "Copy" the commands (into a buffer) by pressing the Command 
       key and the  c  key.  (Notice the word `Copy', in green, on the 
       front of the  c  key),
  *  Click on the blank window to bring it forward,  and
  *  "Paste" the commands in the new location by pressing the 
      Command key and  v  key.  (Notice the word `Paste' on the front
       of the  v  key).  
Try it.
 
:[font = subsection; inactive; preserveAspect; startGroup; ]
Deleting unwanted cells
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
Suppose you want to clean up the ``Untitled-1'' window by `cutting'
out unwanted cells.  For example, to delete the  y = x^2-9  command:
  *  Click on the  ]  to its right,
  *  Hold a Command key and press the  x  key  (note the word `Cut' 
      in green).
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
How to exit this tutorial
:[font = text; inactive; preserveAspect; endGroup; ]
To exit this tutorial, you move the cursor to the Mathematica menu located at the extreme upper left corner of your screen.  Point the cursor and click on the "Quit" rectangle at the bottom of the  menu .   You will be presented with a  "Quit"  box with some options; click the  "Quit Anyway"  box. 
;[s]
3:0,0;50,1;62,2;299,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Logging off the NeXT
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
Once you have left the tutorial, to log off the computer move the cursor to the  "Log Out" box of the Workspace Menu (upper left of the screen)  and click there.  Then on the Quit menu box, click on  Log Out.
^*)