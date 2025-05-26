#! /bin/sh
# locks - list rcs files that are locked
# Usage: locks
# Jack K. Cohen, 1987

PATH=/bin:/usr/bin

exec grep "^locks.*:" RCS/*,v
