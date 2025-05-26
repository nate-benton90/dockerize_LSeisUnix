#! /bin/sh
# precedence - give table of C precedences
# Usage: precedence

PATH=/bin:/usr/bin

exec cat <<'END'
                    TABLE of PRECEDENCES
                (Kernighan & Ritchie, page 49)


    ()   []   ->   .
    !  ~  ++  --  -  (type)  *  &  sizeof     {right to left}
    *   /   %
    +   -
    <<   >>
    <    <=    >    >=
    ==   !=
    &
    ^
    |
    &&
    ||
    ?   :    {right to left}
    =   +=   -=   etc.    {right to left}
    ,
END
