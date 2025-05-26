#! /bin/sh
# system news program
# based in part on the news program in Kernighan and Pike, 1984

PATH=/bin:/usr/bin:/usr/ucb
NEWSDIR=/usr/local/news
# check for news arguments
for i
do
	case $i in
	-)	echo "Usage: news   ---- view news list "
		echo "Usage: news old --- view old news list "
		echo "Usage: news item  --- view particular news item" 1>&2; \
 			exit 1
	;;
	old) 	for k in ` ls $NEWSDIR`
		do
			echo "old news: $k"
		done
		exit 0
	;;
	*)	more $NEWSDIR/$i
		touch $HOME/.news_time
	;;
	esac
done
cd $NEWSDIR
for j in ` /bin/ls -t * $HOME/.news_time`
do
	case $j in
	*' not found')
	;;
	*/.news_time)	break
	;;
	*) echo " news: $j  "
	esac
done
exit 0
