# Undo.tcl --
#
#	Tksu procedures to implement the Undo and Redo commands.
#
# Overview of Undo/Redo actions:
#
#	Each time the canvas is modified, tcl script to undo (and restore) the
#	modification is added to the array CanvasUndo. An entry in CanvasUndo
#	is a two-member list with the first member the undo script and the
#	second member the redo script.  Groups of entries in CanvasUndo are
#	separated by marks.  A "mark" is the special entry `MARK' that
#	delimits a group of entries.  The first and last elements of CanvasUndo
#	are always marks, and CanvasUndo is initialized with a single mark.
#	The variable CanvasLevel indexes CanvasUndo, and represents the current
#	undo/redo level of the canvas.  CanvasLevel is always positioned at a
#	mark before the user initiates a modification to the canvas, an undo
#	action, or a redo action.
#
#	When the Undo button is hit, CanvasLevel is decremented and the undo
#	script in CanvasUndo($CanvasLevel) is executed.  This is repeated
#	until a mark is encountered.  The mark identifies the level that the
#	Undo command should roll the canvas back to.  CanvasLevel finishes up
#	pointing at the mark.
#
#	When the Redo button is hit, CanvasLevel is incremented and the redo
#	script in CanvasUndo($CanvasLevel) is executed.  This is repeated
#	until a mark is encountered.  The mark identifies the level that the
#	Redo command should roll the canvas forward to.  CanvasLevel finishes
#	up pointing at the mark.
#
#	When the user performs an action that modifies the canvas, CanvasLevel
#	is incremented and new entries are added to CanvasUndo via calls to
#	AddUndo.  After all entries stemming from a single user action have
#	been added to CanvasUndo, a new mark is added (via a call to AddMark).
#	CanvasLevel finishes up pointing at the new mark.  Any elements of
#	CanvasUndo with an index greater than CanvasLevel are then deleted,
#	eliminating the possibility of "redoing" the canvas from this level.
#	In addition, keep only the old levels that are within UndoLimit
#	marks of the CanvasLevel, to limit the size of the undo list.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Undo.tcl,v 1.1.1.1 2002/06/07 23:43:30 jeff Exp $

#-------------------------------------------------------------------------------
# ResetUndo -- initialize CanvasUndo array.
#
# Args:     none
# Returns:  null
#
# SavedLevel is also reset, such that canvas is assumed to be up to date with
# its saved file.
#-------------------------------------------------------------------------------

proc tksu::ResetUndo {} {
    variable CanvasUndo
    variable CanvasLevel
    variable SavedLevel

    if [array exists CanvasUndo] {
    	unset CanvasUndo
    }
    set CanvasUndo(0) MARK
    set CanvasLevel 0
    set SavedLevel 0
    return
}

#-------------------------------------------------------------------------------
# AddUndo -- add an entry to CanvasUndo.
#
# Args:     undoScript	Tcl undo script.
#	    redoScript	Tcl redo script.
# Returns:  null
#
# CanvasLevel is incremented before the scripts are added.
#-------------------------------------------------------------------------------

proc tksu::AddUndo {undoScript redoScript} {
    variable CanvasUndo
    variable CanvasLevel
    variable SavedLevel

    set oldMatch [expr $CanvasLevel == $SavedLevel]
    incr CanvasLevel
    set CanvasUndo($CanvasLevel) [list $undoScript $redoScript]

# If necessary, update the canvas title, which shows if the flowsheet has
# been modified (i.e. CanvasLevel is different than SavedLevel).

    set newMatch [expr $CanvasLevel == $SavedLevel]
    if {$oldMatch != $newMatch} SetCanvasTitle
    return
}

#-------------------------------------------------------------------------------
# AddMark -- add a mark to CanvasUndo.
#
# Args:     none
# Returns:  null
#
# CanvasLevel is incremented before the mark is added.  CanvasLevel is left
# pointing at the new mark.  Normal usage:  when responding to a user action
# that modifies the canvas, make one or more calls to AddUndo.  After all
# modifications are done, make one call to AddMark to set a new mark.
#-------------------------------------------------------------------------------

proc tksu::AddMark {} {
    variable CanvasUndo
    variable CanvasLevel
    variable SavedLevel
    variable UndoLimit

    incr CanvasLevel
    set CanvasUndo($CanvasLevel) MARK

# Remove any levels higher than CanvasLevel.  If any of the removed levels
# was the SavedLevel, reset SavedLevel to -1.  This prevents the canvas from
# mistakenly being indicated as saved when the CanvasLevel grows back to the
# old SavedLevel.

    foreach level [array names CanvasUndo] {
    	if {$level > $CanvasLevel} {
	    unset CanvasUndo($level)
	    if {$level == $SavedLevel} {
	    	set SavedLevel -1
		SetCanvasTitle
	    }
	}
    }

# Remove old levels that are beyond UndoLimit marks.  Also reset SavedLevel
# to -1 if necessary.

    set level $CanvasLevel
    set imark 0
    while {[info exists CanvasUndo($level)]} {
    	if {$CanvasUndo($level) == "MARK"} {
	    if {$imark <= $UndoLimit} {
	    	incr imark
		incr level -1
		continue
	    }
	}
	if {$imark > $UndoLimit} {
	    unset CanvasUndo($level)
	    if {$level == $SavedLevel} {
	    	set SavedLevel -1
		SetCanvasTitle
	    }
	}
	incr level -1
    }
    return
}

#-------------------------------------------------------------------------------
# Undo -- undo canvas back to mark.
#
# Args:     none
# Returns:  null
#
# Issue warning and reset CanvasUndo if CanvasLevel does not currently
# point to a mark (should never happen).
#-------------------------------------------------------------------------------

proc tksu::Undo {} {
    variable CanvasUndo
    variable CanvasLevel
    variable SavedLevel

    set oldMatch [expr $CanvasLevel == $SavedLevel]

    if {![info exists CanvasUndo($CanvasLevel)] \
    ||  $CanvasUndo($CanvasLevel) != "MARK"} {
    	tk_messageBox -icon error -type ok -title "Undo failure" \
	    -message "Bad CanvasUndo array.  Resetting array."
	ResetUndo
	return
    }
    DeactivatePort

    while 1 {
    	incr CanvasLevel -1
	if {![info exists CanvasUndo($CanvasLevel)]} {
	    incr CanvasLevel
	    break
	}
	if {$CanvasUndo($CanvasLevel) == "MARK"} {
	    break
	}
	eval [lindex $CanvasUndo($CanvasLevel) 0]
    }
    set newMatch [expr $CanvasLevel == $SavedLevel]
    if {$oldMatch != $newMatch} SetCanvasTitle
    return
}

#-------------------------------------------------------------------------------
# Redo -- redo canvas up to mark.
#
# Args:     none
# Returns:  null
#
# Issue warning and reset CanvasUndo if CanvasLevel does not currently
# point to a mark (should never happen).
#-------------------------------------------------------------------------------

proc tksu::Redo {} {
    variable CanvasUndo
    variable CanvasLevel
    variable SavedLevel

    set oldMatch [expr $CanvasLevel == $SavedLevel]

    if {![info exists CanvasUndo($CanvasLevel)] \
    ||  $CanvasUndo($CanvasLevel) != "MARK"} {
    	tk_messageBox -icon error -type ok -title "Redo failure" \
	    -message "Bad CanvasUndo array.  Resetting array."
	ResetUndo
	return
    }
    DeactivatePort

    while 1 {
    	incr CanvasLevel
	if {![info exists CanvasUndo($CanvasLevel)]} {
	    incr CanvasLevel -1
	    break
	}
	if {$CanvasUndo($CanvasLevel) == "MARK"} {
	    break
	}
	eval [lindex $CanvasUndo($CanvasLevel) 1]
    }
    set newMatch [expr $CanvasLevel == $SavedLevel]
    if {$oldMatch != $newMatch} SetCanvasTitle
    return
}

#-------------------------------------------------------------------------------
# UndoLink -- place commands to undo link add/delete in CanvasUndo.
#
# Args:     linkid	An existing link.
#	    action	Either `add' or `delete'.  If `add', the undo command
#			placed in CanvasUndo will delete the link, and
#			redo will restore the link.  If `delete', vice versa.
# Returns:  null
#
# The linkid has a different value every time the link is recreated, so it
# must be looked up with a call to GetLink at redo time.  Preserve the link's
# selected state.
#-------------------------------------------------------------------------------

proc tksu::UndoLink {linkid action} {
    variable Links
    variable SelItems

    set coords  [GetLinkCoords $linkid]
    set link    $Links($linkid)
    set undo    "DeleteLink \[GetLink $link\]"
    set redo    "ActivatePort [lrange $link 0 1]; "
    append redo "set linkid \[LinkPort [lrange $link 2 3] 0\]; "
    append redo "SetLinkCoords \$linkid $coords"

    if {[lsearch -exact $SelItems $linkid] >= 0} {
    	append redo "; ToggleSelect \$linkid"
    }
    switch -- $action {
    	add	{AddUndo $undo $redo}
	delete	{AddUndo $redo $undo}
    }
    return
}

#-------------------------------------------------------------------------------
# UndoModule -- place commands to undo module add/delete in CanvasUndo.
#
# Args:     module	An existing module.
#	    action	Either `add' or `delete'.  If `add', the undo command
#			placed in CanvasUndo will delete the module, and
#			redo will restore it.  If `delete', vice versa.
# Returns:  null
#
# The module instance number is preserved between undo and redo actions
# (unlike the linkid for links), so that the module name does not change.
# Preserve the selected state and hard-disabled state of the module as well.
#-------------------------------------------------------------------------------

proc tksu::UndoModule {module action} {
    variable Instances
    variable Values
    variable Disabled
    variable SelItems

    set id	[lindex $Instances($module) 0]
    set coords  [lrange [.canvas coords $id] 0 1]
    set undo    "DeleteModule $module 0"
    set redo    "AddModule $module $coords; "
    append redo	"set tksu::Values($module) "
    append redo "[list $Values($module)]"

    if {$Disabled($module) == 1} {
    	append redo "; DisableModules $module"
    }
    if {[lsearch -exact $SelItems $module] >= 0} {
    	append redo "; ToggleSelect $module"
    }
    switch -- $action {
    	add	{AddUndo $undo $redo}
	delete	{AddUndo $redo $undo}
    }
    return
}

#-------------------------------------------------------------------------------
# UndoName -- schedule commands to undo parameter name add/delete.
#
# Args:     module	An existing module.
#	    iparam	Index in Values($module) list of variable-name param.
#	    action	Either `add' or `delete'.  If `add', the undo command
#			placed in CanvasUndo will delete the parameter, and
#			redo will restore it.  If `delete', vice versa.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::UndoName {module iparam action} {
    variable Values

    set vlist	[lindex $Values($module) $iparam]
    set undo    "set tksu::Values($module) "
    append undo	"\[lreplace \$tksu::Values($module) $iparam $iparam\]; "
    append undo	"UpdateLinkDialogs $module [expr $iparam - 1] 1"

    set redo    "set tksu::Values($module) "
    append redo "\[linsert \$tksu::Values($module) $iparam [list $vlist]\]; "
    append redo	"UpdateLinkDialogs $module $iparam 1"

    switch -- $action {
    	add	{AddUndo $undo $redo}
	delete	{AddUndo $redo $undo}
    }
    return
}

#-------------------------------------------------------------------------------
# UndoValue -- schedule commands to undo parameter value change.
#
# Args:     module	An existing module.
#	    iparam	Index in Values($module) list of parameter.
#	    vnew	New vlist value to replace old value in Values.
#			The undo operation restores the old value.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::UndoValue {module iparam vnew} {
    variable Values

    set vold	[lindex $Values($module) $iparam]
    set undo    "set tksu::Values($module) \[lreplace \$tksu::Values($module) "
    append undo	"$iparam $iparam [list $vold]\]; "
    append undo	"UpdateLinkDialogs $module $iparam 1"

    set redo    "set tksu::Values($module) \[lreplace \$tksu::Values($module) "
    append redo "$iparam $iparam [list $vnew]\]; "
    append redo	"UpdateLinkDialogs $module $iparam 1"

    AddUndo $undo $redo
    return
}
