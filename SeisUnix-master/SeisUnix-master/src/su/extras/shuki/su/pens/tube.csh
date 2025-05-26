#!/bin/csh
if($TERM =~ visual) then
vipen $* 
else if($TERM =~ falco) then
falpen $* 
else if($TERM =~ tek100) then
tpen $*
else if($TERM =~ masscomp) then
apen $*
else if($TERM =~ sun) then
sunpen $*
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
