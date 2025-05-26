#! /bin/sh
# filetype - list all files of given type
# Usage: filetype
# Credit: Fiedler and Hunter
#
# Examples:
#	filetype text      - list printable files
#       filetype stripped  - list unstripped files

PATH=/bin:/usr/bin

case $# in
	1) ;;  # OK
	*) echo Usage: filetype string_from_file_output 1>&2; exit 1 ;;
esac

file * | grep $1 | sed 's/:.*$//'
