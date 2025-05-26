h27158
s 00000/00000/00009
d D 1.3 88/11/15 14:03:12 shuki 3 2
c 
e
s 00000/00000/00009
d D 1.2 88/05/25 14:54:15 shemer 2 1
c with SccsId[]
e
s 00009/00000/00000
d D 1.1 88/04/14 13:55:15 shuki 1 0
c date and time created 88/04/14 13:55:15 by shuki
e
u
U
f e 0
t
T
I 1
#!/bin/csh
#if(`isatty 0`) then
#if($#argv == 0) then
#echo "Usage: chart <stdin"
#exit
#endif
#endif
#alias tube hpen300
suplot -v $* | tube
E 1
