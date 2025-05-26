#! /bin/sh
# news - add pagination and option -l (list all topics) to "news"
# Usage: news [-option] [news_item]
# Copyright 1985 by Jack K. Cohen

PATH=/bin:/usr/bin
PAGE_PROGRAM=more

for i
do
	case $i in
	-l)	# List all topics option.
		echo Available news topics are:
		ls /usr/news
		exit 0
	;;
	*)	# Accumulate args for the REAL news program
		args="$args $i"
	;;
	esac
done
news $args  | exec PAGE_PROGRAM
