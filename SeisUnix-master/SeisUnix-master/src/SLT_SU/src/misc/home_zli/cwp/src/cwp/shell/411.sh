#! /bin/sh
# 411 - Access phone book as a database (after Kernighan and Pike)
# Usage: 411 pattern
# Note: ignoring case in searches
# A feature (or bug) is that 411 sans args prints the whole phonebook.
# Jack K. Cohen, 1985

PATH=/bin:/usr/bin

phonedir=/usr/local/lib

exec grep -i "$*" $phonedir/phonebook
