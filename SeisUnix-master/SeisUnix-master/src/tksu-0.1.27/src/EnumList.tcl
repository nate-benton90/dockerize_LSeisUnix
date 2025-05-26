# EnumList.tcl --
#
#	Tksu procedures supporting enumeration list windows.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: EnumList.tcl,v 1.1.1.1 2002/06/07 23:43:30 jeff Exp $

#-------------------------------------------------------------------------------
# EnumList -- raise enumeration list window.
#
# Args:     enum	Enumeration list to display, e.g. `enum-thed'.
#			Enumeration list names always have the prefix `enum-'.
#			The lists are stored in variable array Enum.
#	    entry	The requesting entry widget name, which is stored in
#			variable EnumEntry($i), where i is the instance of the
#			enum window.  As long as EnumEntry($i) is non-null,
#			any selection made in the enum window is immediately
#			used to update the named entry widget.  It is OK to
#			call EnumList with a null entry.
#	    status	Status line to display for this selection -- e.g.
#			"Selecting parameter `XXX' in module `YYY'".
#	    callback	Script to run when this window is disconnected from
#			the entry widget.  The callback script is stored in
#			EnumEntry.
# Returns:  instance	The instance number of the window is returned.
#
# Multiple top-level enumeration windows may be displayed at once.  The copies
# are named .enum1, .enum2, etc.  Variables KeepEnum(1), KeepEnum(2), etc.
# determine if each window is to retain its contents.  If KeepEnum is false,
# the contents of the window may be replaced when a new enumeration list is
# to be displayed.
#-------------------------------------------------------------------------------

proc tksu::EnumList {enum entry status callback} {
    variable Enum
    variable EnumEntry
    variable KeepEnum
    variable Color

    if {$enum == "" || ![info exists Enum($enum)]} {
    	return {}
    }

# Find an existing window that is willing to display:  its KeepEnum flag
# is off, OR its current enumeration list matches the enum argument.

    regsub -- {^enum-} $enum {} name
    set instance 0
    set jmax 0

    foreach w [winfo children .] {

    	if {[scan $w ".enum%d" j] != 1} continue
	if {$jmax < $j} {set jmax $j}

# The current enumeration list name is read from the title line.

	set line [$w.buttons.label cget -text]
	regexp -- {`(.+)'} $line dummy namej

	if {$name == $namej} {
	    # Found exact match.
	    set instance $j
	    break
	}
	if {$KeepEnum($j) == 0} {
	    # Found available.
	    set instance $j
	}
    }
    set w .enum$instance

# If no window exists that is willing to display, create a new one.

    if {$instance == 0} {
	set instance [expr $jmax + 1]
	set KeepEnum($instance) 0
	set EnumEntry($instance) {}

	set w .enum$instance
	toplevel $w
	wm protocol $w WM_DELETE_WINDOW \
	    "tksu::HideEnumList $instance"

# Button bar at top:

	set f $w.buttons
	frame $f
	checkbutton $f.keep -text Keep -underline 0 \
			    -variable tksu::KeepEnum($instance)
	label $f.label -pady 2m
	button $f.enter -text Enter -underline 0 -state disabled \
			-command "tksu::EnterAndHide $instance"
	button $f.close -text Close -underline 0 \
			-command "tksu::HideEnumList $instance"
	pack $f.keep -side left
	pack $f.close $f.enter -side right
	pack $f.label
	pack $f -side top -fill x

# Status line at bottom:

	label $w.status
	pack $w.status -side bottom -pady 2

# Enum listbox and scroll bar:

	listbox $w.list -font FixFont -width 80 -bg white \
			-exportselection 0 -yscrollcommand "$w.scroll set" \
			-selectbackground $Color(selected) 
	scrollbar $w.scroll -command "$w.list yview"
	pack $w.scroll -side right -fill y
	pack $w.list -fill both -expand 1

# Listbox bindings:  make sure tag is unique for this window.

	set tag enum${instance}Tag
	set taglist [bindtags $w.list]
	lappend taglist $tag
	bindtags $w.list $taglist

	bind $tag <ButtonRelease-1>  "tksu::EnumSelected $instance"
	bind $tag <KeyRelease-Up>    "tksu::EnumSelected $instance"
	bind $tag <KeyRelease-Down>  "tksu::EnumSelected $instance"
	bind $tag <KeyRelease-space> "tksu::EnumSelected $instance"

# Accelerator bindings:

	bind $w <Alt-k> "$w.buttons.keep invoke"
	bind $w <Alt-c> "tksu::HideEnumList $instance"
	bind $w <Escape> "tksu::HideEnumList $instance"
	bind $w <Alt-e> "$w.buttons.enter invoke"
	bind $w <Return> "$w.buttons.enter invoke"
	bind $w <KeyRelease-Up>    "tksu::MoveSelection $w.list -1; \
				    tksu::EnumSelected $instance"
	bind $w <KeyRelease-Down>  "tksu::MoveSelection $w.list 1; \
				    tksu::EnumSelected $instance"
	bind $w <KeyRelease-Prior> "tksu::MoveSelection $w.list -10; \
				    tksu::EnumSelected $instance"
	bind $w <KeyRelease-Next>  "tksu::MoveSelection $w.list 10; \
				    tksu::EnumSelected $instance"

# Set up forward and reverse traversal.

	set tabList ""
	lappend tabList $w.buttons.keep $w.buttons.enter $w.buttons.close
	lappend tabList $w.list $w.scroll
#	bind $w <Tab> "tksu::TabForward [list $tabList]"
	bind $w <ISO_Left_Tab> "tksu::TabReverse [list $tabList]"

# If associated with an entry widget, try to position the new window directly
# below the parameter list window.  An existing window will retain its user-
# assigned position (via the RaiseWindow call below).

	if {$entry != "" \
	&& [regexp {^(.param[0-9]+).*entry$} $entry match wref]} {
	    RaiseWindowBelow $w $wref
	}
    }
    RaiseWindow $w

# Window .enum$instance is available for display:  fill title and status line.

    set oldName ""
    regexp -- {^List `(.+)'} [$w.buttons.label cget -text] dummy oldName

    wm title $w "List $name"
    $w.buttons.label configure -text "List `$name'"
    $w.status configure -text $status

# Set window state.

    set oldCallback [lindex $EnumEntry($instance) 1]
    if {$oldCallback != ""} {
    	eval $oldCallback
    }
    set EnumEntry($instance) [list $entry $callback]
    if {$entry != ""} {
    	$w.buttons.enter configure -state normal
    }

# Fill the listbox.  Some enumerations (thed and bhed) are formatted
# differently.

    set sellist [SaveSelection $w.list]
    $w.list delete 0 end

    foreach line $Enum($enum) {
    	set value [lindex $line 0]
	set desc [lindex $line 1]

	if {$value == "Desc:"} {
	    $w.buttons.label configure -text "List `$name' - $desc"

	} elseif {$enum == "enum-thed" || $enum == "enum-bhed"} {
	    $w.list insert end [format " %-7s  %-5s  %-7s  %s" $value \
	    	[lindex $desc 0] [lindex $desc 1] [lrange $desc 2 end]]

	} else {
	    $w.list insert end [format " %-10s %s" $value $desc]
	}
    }

# If the enum list is the same as the one previously displayed, restore
# the previous selection.  Otherwise set the selection to the first element.

    if {$oldName == $name && [lindex $sellist 0] != ""} {
    	RestoreSelection $w.list $sellist
    } else {
    	SetSelection $w.list 0
    }
    return $instance
}

#-------------------------------------------------------------------------------
# HideEnumList -- put away enumeration list window.
#
# Args:     instance	Instance number of window.
# Returns:  null
#
# The window is not destroyed.  It remains available for later use.
#-------------------------------------------------------------------------------

proc tksu::HideEnumList instance {
    variable KeepEnum

    HideWindow .enum$instance

# Reset window state.

    ResetEnumEntry $instance
    set KeepEnum($instance) 0
    return
}

#-------------------------------------------------------------------------------
# EnterAndHide -- call ParamEnter and put away enumeration list window.
#
# Args:     instance	Instance number of window.
# Returns:  null
#
# Combines the actions of closing the enumeration window and hitting the
# Enter button in the parameter/port dialog.
#-------------------------------------------------------------------------------

proc tksu::EnterAndHide instance {
    variable EnumEntry

    set entry [lindex $EnumEntry($instance) 0]
    set pattern {^\.param([0-9]+)\.param}
    if [regexp -- $pattern $entry dummy paramInstance] {
    	ParamEnter $paramInstance
    }
    HideEnumList $instance
    return
}

#-------------------------------------------------------------------------------
# ResetEnumEntry -- disassociate enumeration window from parameter entry.
#
# Args:     instance	Instance number of window.
# Returns:  null
#
# Reset the status line and invoke the callback (if any) stored in EnumEntry.
# The callback normally resets the `list' button in the parameter dialog.
#-------------------------------------------------------------------------------

proc tksu::ResetEnumEntry instance {
    variable EnumEntry

    if {$instance == ""} return

# The second element in EnumEntry is a callback (if non-null) which should
# be called whenever EnumEntry changes.

    set callback [lindex $EnumEntry($instance) 1]
    if {$callback != ""} {
    	eval $callback
    }

# Reset status line in enumeration window.

    set w .enum$instance
    $w.buttons.enter configure -state disabled
    $w.status configure -text "No selection active"
    set EnumEntry($instance) {}
    return
}

#-------------------------------------------------------------------------------
# EnumSelected -- respond to new selection made in enumeration list.
#
# Args:     instance	Instance number of window.
# Returns:  null
#
# If associated with an entry widget, load the entry with the selected value.
#-------------------------------------------------------------------------------

proc tksu::EnumSelected instance {
    variable EnumEntry

    set entry [lindex $EnumEntry($instance) 0]
    if {$entry == ""} return

# Get new value from listbox.

    set w .enum$instance
    set i [$w.list curselection]
    if {$i == ""} return
    set line [lindex [$w.list get $i] 0]
    regsub =.* $line {} newValue

    LoadEntry $entry $newValue
    return
}

#-------------------------------------------------------------------------------
# GetEnumInstance -- return window instance linked to entry widget.
#
# Args:     entry	Entry widget name.
# Returns:  instance	Instance of enumeration window that is linked to the
#			the entry (it was found in EnumEntry).  If no instance
#			found, return null.
#-------------------------------------------------------------------------------

proc tksu::GetEnumInstance entry {
    variable EnumEntry

    foreach instance [array names EnumEntry] {
    	if {[lindex $EnumEntry($instance) 0] == $entry} {
	    return $instance
	}
    }
    return
}

#-------------------------------------------------------------------------------
# EnumDesc -- return description of enumeration element.
#
# Args:     enum	Enumeration list `enum-XXX' to examine.
#	    name	Enumeration value in list.
# Returns:  string	Description string, or null if enum/value not found.
#-------------------------------------------------------------------------------

proc tksu::EnumDesc {enum name} {
    variable Enum

    if {![info exists Enum($enum)]} {
    	return {}
    }
    foreach entry $Enum($enum) {
    	if {[lindex $entry 0] == $name} {
	    return [join [lrange $entry 1 end]]
	}
    }
    return
}

#-------------------------------------------------------------------------------
# MoveSelection -- move current selection.
#
# Args:     listbox	Listbox widget containing a list.
#	    amount	Amount and direction to scroll the selection by:
#			N scrolls the selection N lines down, and -N scrolls
#			it N lines up.
# Returns:  null
#
# Do nothing if focus is currently in a listbox -- presume that the default
# listbox key bindings will take care of scrolling the selection.
# Also ignore if focus is inside a scroll bar.
#-------------------------------------------------------------------------------

proc tksu::MoveSelection {listbox amount} {

    if {[regexp {scroll$} [focus]] || [regexp {list$} [focus]]} {
    	return
    }
    set i [$listbox curselection]
    set n [$listbox index end]

    if {$i == ""} {
    	set i 0
    } else {
    	incr i $amount
	if {$i < 0} {
	    set i 0
	} elseif {$i >= $n} {
	    set i [expr $n-1]
	}
    }
    SetSelection $listbox $i
    return
}

#-------------------------------------------------------------------------------
# SetSelection -- set selection in listbox.
#
# Args:     listbox	Listbox widget containing a list.
#	    index	Index of list element to select.  Any other element is
#			deselected, the activation index is moved, and the
#			list is scrolled if necessary to see the selection.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::SetSelection {listbox index} {

    $listbox selection clear 0 end
    $listbox selection set $index
    $listbox activate $index
    $listbox see $index
    return
}

#-------------------------------------------------------------------------------
# SaveSelection -- save selection and scroll position of listbox.
#
# Args:     listbox	Listbox widget containing a list.
# Returns:  sellist	The list {index first} is returned, where index is
#			the selected line, and first is the first visible
#			line in the list.
#-------------------------------------------------------------------------------

proc tksu::SaveSelection listbox {

    set i [$listbox curselection]
    set n [$listbox index end]
    set first [lindex [$listbox yview] 0]
    set first [expr round($first * $n)]
    return [list $i $first]
}

#-------------------------------------------------------------------------------
# RestoreSelection -- restore selection and scroll position of listbox.
#
# Args:     listbox	Listbox widget containing a list.
#	    sellist	The list {index first}, where index is the selected
#			line, and first is the first visible line in the list.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::RestoreSelection {listbox sellist} {

    set n [$listbox index end]
    set first [lindex $sellist 1]
    if {$first >= $n} {
    	set first [expr $n - 1]
    }
    $listbox yview $first

    set index [lindex $sellist 0]
    if {$index == ""} {
    	set index 0
    } elseif {$index >= $n} {
    	set index [expr $n - 1]
    }
    $listbox selection set $index
    $listbox activate $index
    return
}
