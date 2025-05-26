# Search.tcl --
#
#	Tksu module search methods.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Search.tcl,v 1.1.1.1 2002/06/07 23:43:30 jeff Exp $

#-------------------------------------------------------------------------------
# IncSearch -- incremental search for module.
#
# Args:     pattern	Pattern to search for.
# Returns:  string	Module name that first matches the pattern.
#
# Given starting pattern, look for and return first module matching that
# pattern.  If no module is found, return null.  In looking for module,
# first examine current category, then examine all modules.
#-------------------------------------------------------------------------------

proc tksu::IncSearch pattern {
    variable Desc
    variable Members
    variable CurrentCat

    set imatch -1

# First search current category for a match with pattern.

    if [info exists Members($CurrentCat)] {
    	set modlist [lsort $Members($CurrentCat)]
	set imatch [lsearch -regexp $modlist ^$pattern]
    }

# If not found in current category, search all modules.

    if {$imatch < 0} {
    	set modlist [lsort [array names Desc]]
	set imatch [lsearch -regexp $modlist ^$pattern]
    }
    if {$imatch < 0} {
    	return {}
    }
    return [lindex $modlist $imatch]
}

#-------------------------------------------------------------------------------
# IncSearchEntry -- perform incremental search from modlist entry widget.
#
# Args:     keysym	Keysym of latest keystroke.
# Returns:  null
#
# This procedure is bound to a KeyPress event in the entry widget, and
# initiates an incremental search.
#-------------------------------------------------------------------------------

proc tksu::IncSearchEntry keysym {
    variable CurrentMod
    variable PreviousPattern

    set w .modlist.buttons.entry
    set pattern [string trim [$w get]]

# If the entry widget string is equal to the current module, it indicates
# that a keystroke was received that did not modify the pattern (such as
# Tab, Left, Home, etc).  Forget the incremental search in this case.

    if {$pattern == $CurrentMod} return
    set module [IncSearch $pattern]

    if {![info exists PreviousPattern]} {
    	set PreviousPattern ""
    }

# No module matching pattern:

    if {$module == ""} {
	set PreviousPattern $pattern
    	ModSelect $pattern
	return
    }

# Trim one character off end of pattern if backspace or delete was hit,
# and pattern is no different from previous pattern.  This indicates that
# the backspace deleted the selection instead of trimming the pattern by
# one character.

    if {($keysym == "BackSpace" || $keysym == "Delete") \
    && [info exists PreviousPattern] && $pattern == $PreviousPattern} {

	set n [string length $pattern]
	if {$n >= 2} {
	    incr n -2
	    set pattern [string range $pattern 0 $n]
	} else {
	    set pattern ""
	}
	set module [IncSearch $pattern]
    }

# Module is found that matches pattern:  the trailing part of the module
# name after the pattern is selected in the entry widget.

    set PreviousPattern $pattern
    ModSelect $module
    set i [string length $pattern]
    $w icursor $i
    $w selection range $i end
    return
}

#-------------------------------------------------------------------------------
# IncSearchBind -- bind incremental search to module list entry widget.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::IncSearchBind {} {

    set w .modlist.buttons.entry
    set taglist [bindtags $w]
    lappend taglist entryTag
    bindtags $w $taglist
    bind entryTag <KeyPress> {tksu::IncSearchEntry %K}
    return
}

#-------------------------------------------------------------------------------
# Search -- search categories for a module.
#
# Args:     direction	Direction to perform the search in:
#			`next'     Search forward.
#			`prev'     Search backward.
#			`continue' Search in the previously given direction.
# Returns:  null
#
# The search (regexp) pattern is fetched from the search entry widget.
# Categories are searched for a match in a module's name or description.
# When a match is found, the CurrentCat and CurrentMod are updated.
#-------------------------------------------------------------------------------

proc tksu::Search direction {
    variable Desc
    variable Members
    variable CurrentCat
    variable CurrentMod
    variable SearchDirection

    if {! [info exists SearchDirection]} {
    	set SearchDirection next
    }
    if {$direction == "continue"} {
    	set direction $SearchDirection
    } else {
    	set SearchDirection $direction
    }
    set sortOrder [expr {$direction == "prev" ? "-decreasing" : "-increasing"}]

# Get pattern from search entry widget.

    set pattern [string trim [.modlist.search.entry get]]
    if {$pattern == ""} return

# Set up list of categories to search over.  Begin and end search at current
# category.  If current category is "All Modules", restrict search to just
# that category.

    set startCat $CurrentCat
    if {$startCat == ""} {
    	set startCat "All Modules"
    }
    if {$startCat == "All Modules"} {
    	set cats [list $startCat $startCat]

    } else {
	set cats [lsort $sortOrder [array names Members]]
	set icat [lsearch -exact $cats $startCat]
	if {$icat < 0} return
	set cats "[lrange $cats $icat end] [lrange $cats 0 $icat]"
    }

# Loop over categories.

    set icat 0
    set ecat [expr [llength $cats] - 1]
    foreach cat $cats {

	if {$cat == "All Modules"} {
	    set modules [lsort $sortOrder [array names Desc]]
	} else {
	    set modules [lsort $sortOrder $Members($cat)]
	}

# For starting category, trim module list up to and including CurrentMod.

	if {$icat == 0} {
	    set imod [lsearch -exact $modules $CurrentMod]
	    if {$imod >= 0} {
	    	set modules [lreplace $modules 0 $imod]
	    }

# For ending category, trim module list from CurrentMod to end.

	} elseif {$icat == $ecat} {
	    set imod [lsearch -exact $modules $CurrentMod]
	    if {$imod >= 0} {
	    	set modules [lreplace $modules $imod end]
	    }
	}

# Loop over modules in category.  Search for pattern in module name, then
# in description.

	foreach module $modules {
	    if {[regexp -nocase $pattern $module] \
	    ||  [regexp -nocase $pattern $Desc($module)]} {

# Found match:  update current category and module to new match.

		if {$cat != $CurrentCat} {
		    ModListUpdate $cat 0
		}
		ModSelect $module
		return
	    }
	}
	incr icat
    }
    return
}
