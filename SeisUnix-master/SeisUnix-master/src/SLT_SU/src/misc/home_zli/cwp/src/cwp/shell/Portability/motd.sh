#! /bin/sh
# motd - echo the current motd
# Usage: motd
# Author: Shuki

PATH=/bin:/usr/bin
PAGE_PROGRAM=more

exec PAGE_PROGRAM /etc/motd
