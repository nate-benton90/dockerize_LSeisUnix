#include <stdio.h>
#include <strings.h>
#include "lists.h"


DrawTop(header)
struct info2 *header;
{	int x,y;
	unsigned int length;

	x =0;
	y= 3;
	erase();
	clear();

	while(header != NULL) {
		length = strlen(header->stuff);
		x = (int)length;
		x = ((col/2) - (x/2));
		move(y, x);
		printw(" %s \n",header->stuff);
		y++;
		header = header->next;
	}
	refresh();
	return(y);
}


WINDOW *MakeWindow(top)
int top;
{	WINDOW *win;
	int lines, cols;

	top +=2;
	lines = lin - top;
	cols = col-2;
	move(top-1, 5);
	printw("i - replace value,   j - down,   k - up,   x - continue,   q - quit");
	refresh();
	win = newwin(lines, cols, top, 1);

	box(win, '*', '*');
	wrefresh(win);
	win = newwin(lines-2, cols-2, top+1, 2);

	return(win);
}

SetRequired(top, head, win)
int top;
struct list *head;
WINDOW *win;
{	int x, y, i, flag, rflag;
	int ScreenLength;
	int spots[10];
	struct list *current;

	ScreenLength = lin -1-top;

	current = head;
	rflag = 1;
	x = (col/2) - 10;
	y = 1;
	wmove(win, y, x);
	wstandout(win);
	wprintw(win, "Required Parameters");
	wstandend(win);
	FillWindow(win, current, ScreenLength, spots);

	GetInfo(win, head, spots, rflag, ScreenLength);

}


SetOptional(top, head, win)
int top;
struct list *head;
WINDOW *win;
{	int x, y, i, flag, rflag;
	int ScreenLength;
	int spots[10];
	struct list *current;

	ScreenLength = lin -1-top;

/* initialize changed field to 0 */

	current = head;
	while (current != NULL) {
		current->changed = '0';
		current=current->next;
	}
	current = head;
	rflag = 0;
	x = (col/2) - 10;
	y = 1;
	wmove(win, y, x);
	wstandout(win);
	wprintw(win, "Optional Parameters");
	wstandend(win);
	FillWindow(win, current, ScreenLength, spots);

	GetInfo(win, head, spots, rflag, ScreenLength);

}


/************************************************************************
 *		Fills the window with as many variables as are		*
 *		left, or will fit.					*
 ************************************************************************/
FillWindow(win, head, ScreenLength, spots)
WINDOW *win;
struct list *head;
int ScreenLength, spots[10];
{	int x, y, count, i, flag;
	struct info *desc;
	struct list *current;

	x = (col/16);
	y = 4; 
	current = head;
	for (count=0;count < 10; count++) spots[count] =0;
	count = 0;
	flag = 0;
	wrefresh(win);
	while ((current != NULL) && (flag == 0)) {
		wmove(win, y, x);
		desc = current->description;
		i = 0;
		while (desc != NULL) {
			desc = desc->next;
			i++;
		}
		if ((y +i) < ScreenLength) {
			wprintw(win, " %s", current->variable);
			x = (7*col/16)-1;
			wmove(win, y, x);
			wprintw(win, "%s", current->description->stuff);
			wrefresh(win);
			spots[count++] = y;
			desc = current->description;
			while(desc->next != NULL) {
				desc = desc->next;
				y++;
				x = (7*col/16)-1;
				wmove (win, y, x);
				wprintw(win, "%s ",desc->stuff);
				wrefresh(win);
			}
			y += 2;
			x = (col/16);
			current = current->next;
			wrefresh(win);
		}
		else {
			flag = 1;
		}
	}
	count = 0;
	current = head;
	while(spots[count]) {
		clearbar(win, spots[count], current->value);
		current = current->next;
		count++;
	}
}


char movement(win, x, y)
WINDOW *win;
int x, y;
{	char a;

	wmove(win, 1,1);
	wrefresh(win);
	a = wgetch(win);
	wmove(win, y, x);
	wrefresh(win);
	switch(a) {
		case 'k' : return('u');
				break;
		case 'j' : return('d');
			   break;
		default : return(a);
	}
}


setbar(win, y, str)
WINDOW *win;
int y;
char str[StringLength];
{	int x;

	x = (3*col/16);
	wrefresh(win);
	wmove(win, y+1, x );
	wprintw(win, "               ");
	wrefresh(win);
	wmove(win, y-1, x );
	wprintw(win, "               ");
	wrefresh(win);
	wmove(win, y, x );
	wstandout(win);
	wprintw(win, "              =");
	wrefresh(win);
	wmove(win, y, x );
	wprintw(win, "%s", str);
	wmove(win, y, x );
	wstandend(win);
	wrefresh(win);
}


clearbar(win, y, str)
WINDOW *win;
int y;
char str[StringLength];
{	int x;

	x = (3*col/16);
	wstandend(win);
	wmove(win, y, x );
	wrefresh(win);
	wmove(win, y, x );
	wprintw(win, "..   .   ..   .");
	wrefresh(win);   	
	wmove(win, y, x );
	wprintw(win, "              =");
	wrefresh(win);
	wmove(win, y, x );
	wprintw(win, "%s", str);
	wmove(win, y, x );
	wrefresh(win);
}



backscroll(win, current, spots, ScreenLength)
WINDOW *win;
struct list *current;
int spots[10], ScreenLength;
{
	current = current->prev;
	wclear(win);
	wrefresh(win);
	FillWindow(win, current, ScreenLength, spots);
}

wscroll(win, current, spots, ScreenLength, y)
WINDOW *win;
struct list *current;
int spots[10], ScreenLength, y;
{	struct list *next;
	struct info *desc;
	int x, count, len1, len2, toscroll;

	

	next = current->next;
	desc = next->description;
	len1 = 0;
	while(desc != NULL) {
		len1++;
		desc = desc->next;
	}
	len2 = 0;
	desc = current->description;
	while (desc != NULL) {
		len2++;
		desc = desc->next;
	}
	toscroll = len1 + 2 - (ScreenLength - y - len2);
	x = 1;
	len1 = 1;	
	while(spots[x]) {
		x++;
		len1++;
		current = current->prev;
	}
	count = 0;
	while(count < toscroll) {
		desc = current->description;
		while(desc != NULL) {
			count++;
			desc = desc->next;
		}
		count++;
		len1--;
		current = current->next;
	}
	wclear(win);
	wrefresh(win);
	FillWindow(win, current, ScreenLength, spots);
	return(len1);
}

/************************************************************************
 *		Check looks to see that all the required parameters	*
 *		have had values set.					*
 ************************************************************************/
check(head)
struct list *head;
{	struct list *current;

	current = head;
	while(current !=NULL) {
		if ((int)strlen(current->value) <= 0) return(0);
		current = current->next;
	}
	return(1);
}


GetInfo(win, head, spots, rflag, ScreenLength)
WINDOW *win;
struct list *head;
int spots[10], rflag, ScreenLength;
{	char a, str[StringLength];
	int count, flag, len;
	struct list *current;

	count = 0;
	current = head;

	setbar(win, spots[count], current->value);

	flag = 1;
	while (flag) {
		a = movement(win, (3*col/16), spots[count]);
		switch(a) {
			case 'u' : if (current->prev == NULL)
					wprintw(win,"\007");
				   else if (count == 0) {
				 	clearbar(win, spots[count],								current->value); 
					backscroll(win, current, spots,
						ScreenLength);
					if (rflag) {
						wmove(win, 1, ((col/2)-10));
						wstandout(win);
					wprintw(win, "Required Parameters");
						wstandend(win);
					}
					else {
						wmove(win, 1, ((col/2)-10));
						wstandout(win);
					wprintw(win, "Optional Parameters");
						wstandend(win);
					}
					current = current->prev;
					setbar(win, spots[count], 
						current->value);
					}
				   else {
					clearbar(win,spots[count],
						current->value);
					current = current->prev;
					count--;
					setbar(win, spots[count],
						current->value);
				   }
				   break;
			case 'd' : if (current->next == NULL)
					wprintw(win, "\007");
				   else if (spots[count+1] == 0){
					clearbar(win, spots[count],
						current->value);
					count = wscroll(win, current, spots, 
						ScreenLength, spots[count]);
					current = current->next;
					setbar(win, spots[count],
						current->value);
					if (rflag) {
						wmove(win, 1, ((col/2)-10));
						wstandout(win);
					wprintw(win, "Required Parameters");
						wstandend(win);
					}
					else {
						wmove(win, 1, ((col/2)-10));
						wstandout(win);
					wprintw(win, "Optional Parameters");
						wstandend(win);
					}
				   }
				   else {
					clearbar(win, spots[count],
						current->value);
					current = current->next;
					count++;
					setbar(win, spots[count],
						current->value);
				   }
				   break;
			case 'i' : winput(win, str);
				   strcpy(current->value, str);
			           current->changed = 'y';	
				   setbar(win, spots[count], str);
				   break;
			case 'x' : if (((rflag) && (check(head)))
					|| (rflag == 0))
					flag = 0;
				   else {
					wmove(win, 1, 4);
					wprintw(win, "must set ALL parms ");
					wmove(win, 1, 1);
					wprintw(win, " ");
					wrefresh(win);
					wmove(win, 1, 1);
					wrefresh(win);
				   }
				   break;
			case 'q' : wmove(win, 1, 4);
					wprintw(win, "are you sure?     ");
					wmove(win, 1, 17);
					wrefresh(win);
					a = wgetch(win);
					wmove(win, 1, 1);
					wprintw(win, " ");
					wrefresh(win);
					if (a == 'y') {
						clear();
						refresh();
						endwin();
						exit();
					}
					wmove(win, 1, 4);
					wprintw(win, "                 ");
					wrefresh(win);
				break;
			default : wprintw(win, "\007");
		}
	}
}


