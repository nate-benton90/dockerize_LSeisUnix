#! /bin/sh
# dirtree - show names of directories in current tree
# Usage: dirtree
# Jack K. Cohen, 1988

echo "."
ls -lR |
sed -n '
	/^d/s/.* /	/p
	/^\./p
'
