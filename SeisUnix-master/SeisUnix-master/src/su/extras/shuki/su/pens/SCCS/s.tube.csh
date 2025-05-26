h50899
s 00007/00001/00015
d D 1.3 88/11/15 14:07:36 shuki 3 2
c 
e
s 00002/00000/00014
d D 1.2 88/04/19 15:46:44 shuki 2 1
c 
e
s 00014/00000/00000
d D 1.1 88/04/14 13:59:33 shuki 1 0
c date and time created 88/04/14 13:59:33 by shuki
e
u
U
f e 0
t
T
I 1
#!/bin/csh
D 3
if($TERM =~ vt100) then
E 3
I 3
if($TERM =~ visual) then
E 3
vipen $* 
I 3
else if($TERM =~ falco) then
falpen $* 
else if($TERM =~ tek100) then
tpen $*
else if($TERM =~ masscomp) then
apen $*
E 3
I 2
else if($TERM =~ sun) then
sunpen $*
E 2
else if($TERM =~ 2397) then
hppen $* 
else if($TERM =~ 300h*) then
hpen300 $* 
else if($TERM =~ tek4105*) then
ctekpen $*
else if($TERM =~ grif*) then
grifpen $*
else
echo "tube: No Graphics available on that device\!\!"
endif
E 1
