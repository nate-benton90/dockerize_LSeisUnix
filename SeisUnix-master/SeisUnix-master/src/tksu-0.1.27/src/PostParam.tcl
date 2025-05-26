# PostParam.tcl --
#
#	Tksu procedures to post a menu beside a module box, with the menu
#	showing the currently set parameters.  Clicking on an entry in the
#	posted menu raises the dialog for that parameter.  Ports and
#	defaulted parameters are not shown.  Port values are accessed via
#	the port boxes.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: PostParam.tcl,v 1.3 2002/07/09 22:42:35 jeff Exp $

#-------------------------------------------------------------------------------
# PostParam -- post the parameter popup menu.
#
# Args:     module	Module whose parameters are to be shown.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::PostParam module {
    variable Color
    variable Ports
    variable Params
    variable Values
    variable Instances

    if {![info exists Instances($module)]} return

# Create menu if necessary.  First menu item is always available, and raises
# the parameter list window with no specific parameter shown in the dialog.

    if {![winfo exists .ppmenu]} {
	menu .ppmenu -tearoff 0 -font VarFont \
	    -background white -borderwidth 2 \
	    -activebackground $Color(selected) -activeborderwidth 2
    }
    .ppmenu delete 0 end
    .ppmenu add command -label Parameters: -command "tksu::ParamList $module"
#   .ppmenu add command -label Parameters: -command ".ppmenu unpost"

# Subsequent items are parameters that have been explicitly set, i.e.
# parameters with non-null values.

    set iparam 0
    foreach vlist $Values($module) {
    	if {[lindex $vlist 0] == "param"} {
	    set name  [lindex $vlist 2]
	    set value [lindex $vlist 3]
	    regsub -- {\(.+\)$} $name {} name
	    if {$value != ""} {
		set label "$name=$value"
		if {[string length $label] > 24} {
		    set label "[string range $label 0 24] ..."
		}
	    	.ppmenu add command -label $label \
		    -command "tksu::ParamListParam $module $iparam"
	    }
	}
	incr iparam
    }

# Post the menu adjacent to the right edge of the module box.  Coordinates
# have to be converted from canvas values to root screen values.

    set coords [.canvas coords [lindex $Instances($module) 0]]
    set xshift [expr [winfo rootx .canvas] - [.canvas canvasx 0]]
    set yshift [expr [winfo rooty .canvas] - [.canvas canvasy 0]]
    set x [expr int([lindex $coords 2] + $xshift + 2)]
    set y [expr int([lindex $coords 1] + $yshift)]

    tk_popup .ppmenu $x $y
    return
}

#-------------------------------------------------------------------------------
# AddPopupButton -- add button to module box for posting the parameter menu.
#
# Args:     module	Instantiated module name.
# Returns:  null
#
# Position the button (actually a canvas image object) centered inside the
# right edge of the module box.
#-------------------------------------------------------------------------------

proc tksu::AddPopupButton module {
    variable Instances

    set coords [.canvas coords [lindex $Instances($module) 0]]
    set x [expr [lindex $coords 2] - 2]
    set y [expr 0.5*([lindex $coords 1] + [lindex $coords 3])]

    set id [.canvas create image $x $y -anchor e -image RightArrow \
    	-tags "$module modules"]

# Bind popup command to button.

    .canvas bind $id <ButtonPress> \
    	"tksu::PostParam $module"
    .canvas bind $id <Enter> {
    	tksu::PushInfo popup "Click arrow to show currently set parameters"
    }
    .canvas bind $id <Leave> "tksu::PopInfo popup"
    return
}
