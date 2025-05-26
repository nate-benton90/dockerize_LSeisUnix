# Help.tcl --
#
#	Tksu procedures implementing help windows.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Help.tcl,v 1.5 2002/08/04 15:23:17 jeff Exp $

#-------------------------------------------------------------------------------
# Help -- raise text window displaying module help.
#
# Args:     module	Module to show help for.  SU auto-documentation is
#			fetched and placed in a text widget in a window.
#	    filename	Optional:  if present, names a file (relative to
#			TksuDir) that should be displayed in the help window.
#			In this case, the module arg contains the title to
#			slap on the help window.
# Returns:  null
#
# More than one copy of the top level help window may be displayed at once.
# The copies are named .help1, .help2, etc.  The corresponding variables
# KeepHelp(1), KeepHelp(2), etc. are bound to the windows' checkbuttons.
# The KeepHelp variable, if true, specifies that the window is to retain its
# help text; if false, the contents of the window may be overwritten with new
# help text.
#-------------------------------------------------------------------------------

proc tksu::Help {module args} {
    variable KeepHelp
    variable TksuDir
    variable DocCommand

    set filename [lindex $args 0]
    if {$filename != ""} {
    	set filename [file join $TksuDir $filename]
	set title $module
	set modbase $filename

# Strip instance suffix from module name if necessary.

    } else {
    	if {$module == ""} return
	set modbase [ModBase $module]
    }

# Find an existing help window that is willing to display the helpText.

    set instance 0
    set jmax 0
    foreach w [winfo children .] {

    	if {[scan $w ".help%d" j] != 1} continue
	if {$jmax < $j} {set jmax $j}

# Fetch base module name from existing window.

	set modbasej [$w.buttons.label cget -text]
	regexp -- {`(.+)'} $modbasej dummy modbasej

	if {$modbase == $modbasej} {
	    # Found exact match
	    set instance $j
	    break
	}
	if {$KeepHelp($j) == 0} {
	    # Found available
	    set instance $j
	}
    }
    set w .help$instance

# If no help window exists that is willing to display, create a new one.

    if {$instance == 0} {
	set instance [expr $jmax + 1]
	set KeepHelp($instance) 0

	set w .help$instance
	toplevel $w
	wm protocol $w WM_DELETE_WINDOW "tksu::HideHelp $instance"

# Button bar at top:

	set f $w.buttons
	frame $f
	checkbutton $f.keep -text Keep -underline 0 \
			    -variable tksu::KeepHelp($instance)
	label $f.label -pady 2m
	button $f.close -text Close -underline 0 \
			-command "tksu::HideHelp $instance"
	pack $f.keep -side left
	pack $f.close -side right
	pack $f.label -fill x
	pack $f -side top -fill x

# Text widget (read-only) and scroll bar:

	text $w.text -yscrollcommand "$w.scroll set" -font FixFont -bg white
	scrollbar $w.scroll -command "$w.text yview" -takefocus 0
	pack $w.scroll -side right -fill y
	pack $w.text -fill both -expand 1

# Forward scrolling events to the scroll bar:

	bind $w <KeyRelease-Up>    "$w.text yview scroll -1 units"
	bind $w <KeyRelease-Down>  "$w.text yview scroll  1 units"
	bind $w <KeyRelease-Prior> "$w.text yview scroll -1 pages"
	bind $w <KeyRelease-Next>  "$w.text yview scroll  1 pages"

# Accelerator bindings:

	bind $w <Alt-k>  "$w.buttons.keep invoke"
	bind $w <Alt-c>  "tksu::HideHelp $instance"
	bind $w <Escape> "tksu::HideHelp $instance"

# Set up forward and reverse traversal.

	set tabList "$w.buttons.keep $w.buttons.close $w.scroll"
#	bind $w <Tab> "tksu::TabForward [list $tabList]"
	bind $w <ISO_Left_Tab> "tksu::TabReverse [list $tabList]"
    }
    RaiseWindow $w

# Window $w is available for help display.

    if {$filename != ""} {
    	wm title $w $title
	$w.buttons.label configure -text $title
	set KeepHelp($instance) 1

    } else {
	wm title $w "Help for $modbase"
	$w.buttons.label configure -text "Module `$modbase'"
    }

# Store the output of the module's self-documentation in helpText.
# Catch any problem with the execution of the module and append the error
# message to the text.

    set message ""
    set helpText ""
    catch {
	if {$filename != ""} {
	    set helpText [exec cat $filename]

	} elseif [info exists DocCommand($modbase)] {
	    set helpText [eval exec $DocCommand($modbase)]

	} else {
	    set helpText [exec sudoc $modbase]
	}
    } message
    if {$helpText == "" && $message != ""} {
    	set helpText $message
    }

# Some modules are unknown to sudoc.  However, they all seem to have
# self-documentation, so if sudoc fails, try running the module with
# no arguments and capture stderr.

    if [regexp -- "There is no entry in the docs.*" $helpText] {
	set helpFile [TempName]
    	catch {
	    exec $modbase > /dev/tty 2> $helpFile
	} message
	catch {
	    set helpText [exec cat $helpFile]
	}
	catch {
	    exec rm -f $helpFile
	}
	if {$helpText == "" && $message != ""} {
	    set helpText $message
	}
    }

# Place text in window $w.  First line is normally trimmed.

    $w.text configure -state normal
    $w.text delete 1.0 end
    $w.text insert end $helpText
    if [regexp ^In $helpText] {
    	$w.text delete 1.0 1.end
    }
    $w.text configure -state disabled
    return
}

#-------------------------------------------------------------------------------
# HideHelp -- put away help window.
#
# Args:     instance	Instance number of help window.
# Returns:  null
#
# Do not destroy the window.  It remains available for future help text.
#-------------------------------------------------------------------------------

proc tksu::HideHelp instance {
    variable KeepHelp

    HideWindow .help$instance
    set KeepHelp($instance) 0
    return
}

#-------------------------------------------------------------------------------
# HideWindow -- put away a top level window.
#
# Args:     w		Top level window.
# Returns:  null
#
# The geometry of the window is saved in variable array Geometry, so that
# RaiseWindow can restore its position.  The window is withdrawn regardless
# of its window manager state:  iconified or deiconified.
#-------------------------------------------------------------------------------

proc tksu::HideWindow w {
    variable Geometry
    variable Border

    if {[wm state $w] != "withdrawn"} {
	set Geometry($w) [Geometry $w]
	wm deiconify $w
	wm withdraw $w
    }
    return
}

#-------------------------------------------------------------------------------
# RaiseWindow -- deiconify a top level window.
#
# Args:     w		Top level window.
# Returns:  null
#
# If the window was withdrawn with HideWindow, its geometry can be restored.
# If the window hasn't previously appeared on the screen, the Geometry (if
# any) to be passed should be the unframed window geometry.
#-------------------------------------------------------------------------------

proc tksu::RaiseWindow w {
    variable Geometry

    if [info exists Geometry($w)] {
	wm geometry $w $Geometry($w)
	unset Geometry($w)
    }
    wm iconify $w
    wm deiconify $w
    return
}

#-------------------------------------------------------------------------------
# RaiseWindowBelow -- place a top level window just below another window.
#
# Args:     w		Top level window to be placed.
#	    wref	Top level window that $w will be placed next to.
# Returns:  null
#
# Attempt to juxtapose window $w next to and below $wref on the screen by
# setting the geometry of window $w.  Since the geometry is set before the
# window is raised, the window manager may overrule it.
#-------------------------------------------------------------------------------

proc tksu::RaiseWindowBelow {w wref} {
    variable Geometry

    set gframe [FrameGeometry $wref]
    if {[scan $gframe %*dx%d+%d+%d height x y] != 3} return
    set y [expr $y + $height]
    set Geometry($w) "+$x+$y"
    return
}

#-------------------------------------------------------------------------------
# Geometry -- return geometry for window.
#
# Args:     w		Top level window to query.
# Returns:  geometry	The geometry string that `wm geometry' should normally
#			return.
#
# Run xwininfo directly to get the true geometry of the window.  Tk seems to
# have a problem getting the geometry under the enlightenment window manager.
# The routine is not necessary if `wm geometry' works correctly.  The coords
# of the window enclosed by the window manager border are returned.
#-------------------------------------------------------------------------------

proc tksu::Geometry w {

#   set info [exec xwininfo -id [winfo id $w]]
#   set n [lsearch $info -geometry]
#   incr n
#   return [lindex $info $n]
    return [winfo geometry $w]
}

#-------------------------------------------------------------------------------
# FrameGeometry -- return geometry of bordered window.
#
# Args:     w		Top level window to query.
# Returns:  geometry	Hopefully the geometry string for the window that
#			includes the window manager frame.  If not successful,
#			returns null.
#-------------------------------------------------------------------------------

proc tksu::FrameGeometry w {

    set rootMatch   {Root window id: (0x[0-9a-f]+)}
    set parentMatch {Parent window id: (0x[0-9a-f]+)}
    set geomMatch   {-geometry ([0-9]+x[0-9]+)}
    set cornerMatch {Corners: *([+][0-9]+[+][0-9]+)}

    set id {}
    set root {}
    set parent [winfo id $w]

# Find enclosing window whose parent is root.  Presumably this window
# contains the wm borders.

    while {$parent != $root} {
	set id $parent
    	set info [exec xwininfo -id $id -children -stats]

	if {![regexp -- $rootMatch $info match root]} return
	if {![regexp -- $parentMatch $info match parent]} return
    }
    if {$id == {}} return

# Fetch geometry from info string.

    if {![regexp -- $geomMatch $info match geometry]} return
    if {![regexp -- $cornerMatch $info match corner]} return
    return ${geometry}$corner
}
