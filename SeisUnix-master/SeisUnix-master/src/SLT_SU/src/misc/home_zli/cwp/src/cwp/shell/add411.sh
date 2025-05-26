#! /bin/sh
# add411 - edit cwp phonebook
# Usage: add411
# Jack K. Cohen, 1985

PATH=/bin:/usr/bin:/usr/ucb

phonedir=/usr/local/lib

if
	cp $phonedir/phonebook $phonedir/phonebook.bak
then
	echo "phonebook backed up as phonebook.bak"
fi
sleep 1

exec vi $phonedir/phonebook
