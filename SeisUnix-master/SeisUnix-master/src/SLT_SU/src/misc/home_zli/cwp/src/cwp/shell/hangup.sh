#! /bin/sh
# hangup - disconnect line
# Usage: reply "hangup" to the login prompt
# Author: Shuki
# Caveat: Do NOT use directly from the shell--the line will not disconnect!

PATH=/bin:/usr/bin

stty 0
