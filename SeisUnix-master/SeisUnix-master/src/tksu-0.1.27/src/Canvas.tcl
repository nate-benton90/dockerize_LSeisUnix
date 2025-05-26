# Canvas.tcl --
#
#	Tksu procedures supporting canvas window and menu bar.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Canvas.tcl,v 1.3 2002/06/21 02:52:20 jeff Exp $

#-------------------------------------------------------------------------------
# InitCanvas -- initialize canvas window.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::InitCanvas {} {
    variable CanvasDim
    variable Geometry
    variable Version

    SetCanvasTitle
    wm protocol . WM_DELETE_WINDOW tksu::ExitCanvas

# Initialize menu bar.

    menu .menu
    .menu add cascade -label File -underline 0 -menu .menu.file
    .menu add cascade -label Edit -underline 0 -menu .menu.edit
    .menu add cascade -label Go -underline 0 -menu .menu.run
    .menu add cascade -label Lists -underline 0 -menu .menu.lists
    .menu add cascade -label Help -underline 0 -menu .menu.help
#   .menu add cascade -image LogoHT32 -hidemargin 1 -menu .menu.help

# File menu.

    menu .menu.file -tearoff 0
    .menu.file add command -label New -underline 0 -accelerator Alt-N \
    	-command tksu::ClearCanvas
    .menu.file add command -label Open -underline 0 -accelerator Alt-O \
    	-command tksu::OpenCanvas
    .menu.file add command -label Save -underline 0 -accelerator Alt-S \
    	-command tksu::SaveCanvas
    .menu.file add command -label "Save As ..." -underline 5 \
    	-accelerator Alt-A -command "tksu::SaveCanvas {}"
    .menu.file add command -label "Print ..." -underline 0 \
    	-accelerator Alt-P -command tksu::PrintCanvas
    .menu.file add command -label Exit -underline 1 -accelerator Alt-X \
    	-command tksu::ExitCanvas

# Edit menu.

    menu .menu.edit -tearoff 0
    .menu.edit add command -label Undo -underline 0 -accelerator Alt-U \
    	-command tksu::Undo
    .menu.edit add command -label Redo -underline 0 -accelerator Alt-R \
    	-command tksu::Redo
    .menu.edit add separator
    .menu.edit add command -label Enable -underline 0 -accelerator PageUp \
    	-command tksu::EnableSelected
    .menu.edit add command -label Disable -underline 2 -accelerator PageDn \
    	-command tksu::DisableSelected
    .menu.edit add separator
    .menu.edit add command -label Add -underline 0 -accelerator Insert \
	-command {tksu::ModListUpdate [tksu::GetCat]}
    .menu.edit add command -label Delete -underline 0 -accelerator Delete \
    	-command tksu::DeleteSelected

# Run menu.

    menu .menu.run -tearoff 0
    .menu.run add command -label "Check Flow" -underline 0 \
    	-command "tksu::RunFlow -check"
    .menu.run add command -label "Run Flow" -underline 0 \
    	-command tksu::RunFlow
    .menu.run add cascade -label "Kill Flow" -underline 0 -menu .menu.run.kill
    .menu.run add command -label "Flowsheets ..." -underline 0 \
    	-command "tksu::RunFlow -file"
    .menu.run add command -label "Log Files ..." -underline 0 \
    	-command "tksu::ShowLog -file"
    .menu.run add separator
    .menu.run add checkbutton -label "Show log during run" \
    	-variable tksu::ShowLogRun
    .menu.run add checkbutton -label "Save log to file" \
    	-variable tksu::SaveLogFile

    menu .menu.run.kill -tearoff 0 -postcommand tksu::KillFlows

# Lists menu.

    menu .menu.lists -tearoff 0
    .menu.lists add command -label "Module List" -underline 0 \
	-command {tksu::ModListUpdate [tksu::GetCat]}
    .menu.lists add command -label "Binary Header List" -underline 0 \
	-command {tksu::EnumList enum-bhed {} "No selection active" {}}
    .menu.lists add command -label "Trace Header List" -underline 0 \
	-command {tksu::EnumList enum-thed {} "No selection active" {}}

# Help menu.

    menu .menu.help -tearoff 0
    .menu.help add command -label "Quick Reference" -underline 0 \
    	-command "tksu::Help \{[list Tksu $Version Quick Reference]\} \
		  [file join doc QuickRef]"
    .menu.help add command -label "About Tksu ..." -underline 0 \
    	-command tksu::About

    . configure -menu .menu

# Canvas, vertical scroll bar, status bar, horizontal scroll bar.

    canvas .canvas -relief sunken -borderwidth 2 -takefocus 1 \
		   -scrollregion "0 0 $CanvasDim(x) $CanvasDim(y)" \
    		   -yscrollcommand ".yscroll set" \
    		   -xscrollcommand ".xscroll set"
    scrollbar .yscroll -orient vertical -command ".canvas yview"
    scrollbar .xscroll -orient horizontal -command ".canvas xview"
    entry .status -relief sunken -bg white -font VarFont \
    		  -state disabled -takefocus 1

    pack .status -side bottom -fill x
    pack .xscroll -side bottom -fill x
    pack .yscroll -side right -fill y
    pack .canvas -fill both -expand 1

# Canvas-wide bindings.

    bind .canvas <ButtonPress-1> "tksu::DragStart %x %y canvas"
    bind .canvas <Any-ButtonRelease> "tksu::DragFinish %x %y"
    bind . <Return> "tksu::SelectedParamList"

# Keyboard accelerators.

    bind . <Alt-n>  ".menu.file invoke 0"
    bind . <Alt-o>  ".menu.file invoke 1"
    bind . <Alt-s>  ".menu.file invoke 2"
    bind . <Alt-a>  ".menu.file invoke 3"
    bind . <Alt-p>  ".menu.file invoke 4"
    bind . <Alt-x>  ".menu.file invoke 5"

    bind . <Alt-u>  ".menu.edit invoke 0"
    bind . <Alt-r>  ".menu.edit invoke 1"
    bind . <Prior>  ".menu.edit invoke 3"
    bind . <Next>   ".menu.edit invoke 4"
    bind . <Insert> ".menu.edit invoke 6"
    bind . <Delete> ".menu.edit invoke 7"

# Tab traversal.

    set tabList {.canvas .yscroll .xscroll .status}
    bind . <Tab>	  "tksu::TabCanvas [list $tabList] forward; break"
    bind . <ISO_Left_Tab> "tksu::TabCanvas [list $tabList] reverse; break"

# Restore window geometry.  This should be the unframed window geometry.

    if [info exists Geometry(.)] {
    	wm geometry . $Geometry(.)
	unset Geometry(.)
    }
    return
}

#-------------------------------------------------------------------------------
# AddModule -- add instance of module to canvas.
#
# Args:     modbase	Module name.  Special case: if an instance number is
#			included (`modbase-1'), then an attempt is made to
#			allocate and return this module name.
#	    x		Canvas x coordinate (optional).
#	    y		Canvas y coordinate (optional).
# Returns:  module	Module name with new instance number appended.  If
#			the module's specs are incomplete (no ports or no
#			description), return null.
#
# If arguments x and y are supplied, the new module will be placed at or near
# the canvas coordinate (x,y).  Otherwise the new module is placed at the
# bottom of the canvas.
#
# The Instances variable array holds the canvas object id's for each
# instantiated module.  Instances(module) is a list with the format:
#
#   { module-id { port-id0 port-id1 ... }}
#
# where the port-id list matches the port members in Values() and Ports().
# If port n in Ports is read-write, the corresponding (nth) entry in the
# port-id list is null.
#-------------------------------------------------------------------------------

proc tksu::AddModule {modbase args} {
    variable Desc
    variable Ports
    variable Params
    variable Values
    variable Instances
    variable DeselItems
    variable Disabled
    variable Color
    variable ModDim
    variable PortDim
    variable GridInc

# If instance number is provided, try to honor the request.  This is important
# for the Undo/Redo commands.  Otherwise start searching from instance = 1.

    if [regexp -- {^(.+)-([0-9]+)$} $modbase dummy newbase instance] {
    	set modbase $newbase
    } else {
    	set instance 1
    }
    if {![info exists Desc($modbase)]
    || ![info exists Ports($modbase)]} return

# Pick unique instance number for this module.

    while {[info exists Instances($modbase-$instance)]} {
    	incr instance
    }

# Create Values entry for new instance.

    set module $modbase-$instance
    InitValues $module

# If coordinates are given, position the module, rounded to the nearest
# GridInc multiple.

    if {[llength $args] == 2} {
	set x1 [Round [lindex $args 0]]
	set y1 [Round [lindex $args 1]]
	set x2 [expr $x1 + $ModDim(x)]
	set y2 [expr $y1 + $ModDim(y)]

# The module's default position is first column, below all other modules.

    } else {
	set bounds [ModuleBounds]
	set x1 [RoundUp [lindex $bounds 0]]
	if {$x1 < $GridInc} {set x1 $GridInc}
	set y1 [RoundUp [lindex $bounds 3]]
	set y1 [expr $y1 + $GridInc]
	set x2 [expr $x1 + $ModDim(x)]
	set y2 [expr $y1 + $ModDim(y)]
    }

# Create composite object for module:  rectangle, text label, input port
# plugs along top edge, and output port plugs along bottom edge.
# The elements of the composite object share the module instance as a tag.

    set id [.canvas create rectangle $x1 $y1 $x2 $y2 \
    	-fill $Color(module) -outline black -width 2 \
	-tags "$module $module-body modules"]

    set x [expr $x1 + 0.5 * $PortDim(x)]
    set y [expr 0.55*$y1 + 0.45*$y2]
    .canvas create text $x $y -anchor w -font VarFont \
    	-text $module -tags "$module $module-body modules"

# Module bindings.

    set nport 0
    if {[info exists Ports($modbase)] \
    &&  [info exists Params($modbase)]} {
	set nport [llength $Ports($modbase)]
    }
    .canvas bind $module-body <ButtonPress-1> \
    	"tksu::DragStart %x %y module $module single"
    .canvas bind $module-body <Control-ButtonPress-1> \
    	"tksu::DragStart %x %y module $module toggle"
    .canvas bind $module-body <ButtonRelease-3> \
    	"tksu::ParamListParam $module $nport 1"
#   .canvas bind $module-body <Double-ButtonRelease-3> \
#   	"tksu::ParamListParam $module $nport 1"
    .canvas bind $module-body <Enter> "tksu::ModuleInfo $module"
    .canvas bind $module-body <Leave> "tksu::PopInfo $module"

# Place port rectangles.

    set xi1 [expr $x1 + 0.5 * $PortDim(x)]
    set xi2 [expr $xi1 + $PortDim(x)]
    set yi1 $y1
    set yi2 [expr $yi1 + $PortDim(y)]

    set xo1 $xi1
    set xo2 $xi2
    set yo1 [expr $y2 - $PortDim(y)]
    set yo2 $y2

    set xinc [expr 1.5 * $PortDim(x)]

    set iport -1
    foreach nlist $Ports($modbase) {
    	incr iport
	set type [lindex $nlist 1]
	set rw [lindex $nlist 2]
	if {![info exists Color($type)]} {
	    lappend portids {}
	    continue
	}

# Port rectangle coordinates are xp1,yp1,xp2,xp2.

	if {$rw == "r"} {
	    set xp1 $xi1; set yp1 $yi1
	    set xp2 $xi2; set yp2 $yi2
	    set xi1 [expr $xi1 + $xinc]
	    set xi2 [expr $xi2 + $xinc]

	} elseif {$rw == "w"} {
	    set xp1 $xo1; set yp1 $yo1
	    set xp2 $xo2; set yp2 $yo2
	    set xo1 [expr $xo1 + $xinc]
	    set xo2 [expr $xo2 + $xinc]

	} else {
	    lappend portids {}
	    continue
	}
	set xcenter [expr 0.5*($xp1 + $xp2)]
	set xq1 [expr $xcenter - 1]
	set xq2 [expr $xcenter + 1]
	if {$rw == "r"} {
	    set yq1 $yp1; set yq2 [expr $yp1 + 2]
	} else {
	    set yq2 $yp2; set yq1 [expr $yp2 - 2]
	}

# For each port, store in portids the entry port:id1:id2 where id1 is the
# lower rectangle, id2 is the upper rectangle.

	set id1 [.canvas create rectangle $xp1 $yp1 $xp2 $yp2 \
	    -fill $Color($type) -outline black -width 2 \
	    -tags "$module modules"]

	set id2 [.canvas create rectangle $xq1 $yq1 $xq2 $yq2 \
	    -fill $Color($type) -outline $Color($type) -width 2 \
	    -tags "$module modules"]

	set portid "port:$id1:$id2"
	.canvas addtag $portid withtag $id1
	.canvas addtag $portid withtag $id2
	lappend portids $portid

# Port rectangle bindings.

	.canvas bind $portid <ButtonPress-1> \
	    "tksu::DragStart %x %y port $module $iport"
	.canvas bind $portid <ButtonRelease-3> \
	    "tksu::ParamListParam $module $iport 1"
#	.canvas bind $portid <Double-ButtonRelease-3> \
#	    "tksu::ParamListParam $module $iport 1"
	.canvas bind $portid <Enter> "tksu::PortInfo $module $iport"
	.canvas bind $portid <Leave> "tksu::PopInfo $module"
    }

# Increase width of module if necessary to enclose all port rectangles.

    if {$x2 < $xi1} {set x2 $xi1}
    if {$x2 < $xo1} {set x2 $xo1}
    .canvas coords $id $x1 $y1 $x2 $y2

# Add module entry to Instances array.  Add it enabled, deselected.

    set Instances($module) [list $id $portids]
    set Disabled($module) 0
    lappend DeselItems $module
    AddPopupButton $module

# Display current state of each port.

    set nport [llength $Ports($modbase)]
    for {set iport 0} {$iport < $nport} {incr iport} {
	PortBox $module $iport
    }

# If necessary, expand and/or scroll canvas to show new module.

    ExpandCanvas
    set bounds [ExpandBox [.canvas coords $id] $GridInc]
    ScrollCanvas $bounds
    return $module
}

#-------------------------------------------------------------------------------
# MoveModule -- move modules in canvas by given increment.
#
# Args:     dx		Amount to move horizontally in pixels.
#	    dy		Amount to move vertically in pixels.
#	    modules	List of instantiated modules.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::MoveModule {dx dy modules} {
    variable Values

    foreach module $modules {
	.canvas move $module $dx $dy

	foreach linkEndpoint [GetLinks $module] {
	    set id [lindex $linkEndpoint 0]
	    if {[lindex $linkEndpoint 1] == "w"} {
	    	MoveLink $id $dx $dy 0 0
	    } else {
	    	MoveLink $id 0 0 $dx $dy
	    }
	}
    }
    return
}

#-------------------------------------------------------------------------------
# MoveModuleFinish -- finish up dragging modules.
#
# Args:     none
# Returns:  null
#
# Finish up the module drag operation by moving module corner down to next
# whole grid interval.  Record undo and redo commands.  Relies on DragActive
# properly set by DragStart and DragMotion.
#-------------------------------------------------------------------------------

proc tksu::MoveModuleFinish {} {
    variable DragActive
    variable Instances

    set module [lindex $DragActive 4]
    set id [lindex $Instances($module) 0]
    set bounds [.canvas coords $id]
    set x0 [lindex $bounds 0]
    set y0 [lindex $bounds 1]
    set dx [expr [Round $x0] - $x0]
    set dy [expr [Round $y0] - $y0]
    set modules [lindex $DragActive 8]
    MoveModule $dx $dy $modules

# Set x0,y0 to original undragged location, and x,y to final dragged location:

    set x0 [lindex $DragActive 1]
    set y0 [lindex $DragActive 2]
    set x  [expr [lindex $DragActive 6] + $dx]
    set y  [expr [lindex $DragActive 7] + $dy]
    set undo [list MoveModule [expr $x0 - $x] [expr $y0 - $y] $modules]
    set redo [list MoveModule [expr $x - $x0] [expr $y - $y0] $modules]
    AddUndo $undo $redo
    return
}

#-------------------------------------------------------------------------------
# SelectedParamList -- raise parameter list window for selected module.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::SelectedParamList {} {
    variable SelItems
    variable Instances

    set module ""
    foreach item $SelItems {
    	if [info exists Instances($item)] {
	    set module $item
	    break
	}
    }
    if {$module != ""} {
    	ParamListParam $module 0
    }
}

#-------------------------------------------------------------------------------
# PortBox -- update port box appearance in module rectangle.
#
# Args:     module	Instantiated module that port belongs to.
#	    iport	Index of port in Values(module) and Ports(modbase).
# Returns:  null
#
# The Values entry for the given module is examined to determine the current
# state of the port.  If it is a file port, draw the box with a black closed
# border.  If it has a pipe connected, draw a break in the border by filling
# the second rectangle.  If the pipe is disconnected, draw a white border.
# If the port is active (i.e. ready to connect), make the border blink
# white & black.
#-------------------------------------------------------------------------------

proc tksu::PortBox {module iport} {
    variable Values
    variable Instances
    variable ActivePort
    variable SelItems

    if {$module == [ModBase $module]} return

# Get port state: file, pipe or disconnected pipe.

    set vlist [lindex $Values($module) $iport]
    set value [lindex $vlist 3]

    if {[lindex $value 0] != "pipe"} {
    	set state file
    } elseif {[lindex $value 1] != ""} {
    	set state pipe
    } else {
    	set state disconnected
    }
    set active [expr {[list $module $iport] == $ActivePort}]

# Get current outline and fill color of box.

    set portids [lindex $Instances($module) 1]
    set portid [split [lindex $portids $iport] :]
    set id1 [lindex $portid 1]
    set id2 [lindex $portid 2]
    set border [.canvas itemcget $id1 -outline]
    set fill   [.canvas itemcget $id1 -fill]

# If pipe is connected, make upper rectangle visible.

    if {$state == "pipe"} {
    	.canvas itemconfigure $id2 -fill $fill -outline $fill
    } else {
    	.canvas itemconfigure $id2 -fill {} -outline {}
    }

# If active, blink between white and black.

    if $active {
    	BlinkItem $id1 750

# Otherwise, outline color depends on state.  Connected pipe outlines are
# white if link is selected, black otherwise.

    } else {
	BlinkItem $id1 0
	switch -- $state {
	    disconnected {set border white}
    	    file	 {set border black}
	    pipe {
	    	set border black
		foreach linkid [lrange $value 1 end] {
		    if {[lsearch -exact $SelItems $linkid] >= 0} {
		    	set border white
			break
		    }
		}
	    }
	}
	.canvas itemconfigure $id1 -outline $border
    }
    return
}

#-------------------------------------------------------------------------------
# BlinkItem -- start and stop item blinking.
#
# Args:     id		Canvas tag or id of item to blink.  The item must
#			have an `-outline' configuration option.
#	    rate	Blink rate in ms.  If positive, this is a request to
#			start blinking.  If the item is already blinking, the
#			blink rate is changed to this rate.  If rate is zero,
#			this is a request to stop blinking.  If rate is
#			negative, continue blinking while looking for a
#			request to stop from a previous invocation.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::BlinkItem {id rate} {
    variable Blinking

# Request to start:

    if {$rate > 0} {
    	if [info exists Blinking($id)] {
	    set Blinking($id) $rate
	    return
	}
	set Blinking($id) $rate

# Stop blinking by requesting next invocation to stop.

    } elseif {$rate == 0} {
    	if [info exists Blinking($id)] {
	    set Blinking($id) 0
	}
	return

# Continuation: check if it is to stop.

    } else {
    	if {![info exists Blinking($id)]} return
	if {$Blinking($id) == 0} {
	    unset Blinking($id)
	    return
	}
    }

# Toggle visual state.  Schedule continuation call.

    set border [.canvas itemcget $id -outline]
    if {$border == "white"} {
	set border black
    } else {
	set border white
    }
    .canvas itemconfigure $id -outline $border
    after $Blinking($id) [list tksu::BlinkItem $id -1]
    return
}

#-------------------------------------------------------------------------------
# ExpandBox -- expand or contract box by given increment.
#
# Args:     coords	List {x1 y1 x2 y2} defining a rectangle.
#	    inc		Amount to shift edges by: negative to shrink, positive
#			to expand.
# Returns:  coords	List {x1 y1 x2 y2} of modified coordinates.
#-------------------------------------------------------------------------------

proc tksu::ExpandBox {coords inc} {

    set x1 [expr [lindex $coords 0] - $inc]
    set y1 [expr [lindex $coords 1] - $inc]
    set x2 [expr [lindex $coords 2] + $inc]
    set y2 [expr [lindex $coords 3] + $inc]

    return "$x1 $y1 $x2 $y2"
}

#-------------------------------------------------------------------------------
# ModuleBounds -- return rectangle bounding all modules in canvas.
#
# Args:     none
# Returns:  bounds	Smallest rectangle {x1 y1 x2 y2} that encloses all
#			modules currently in the canvas.
#-------------------------------------------------------------------------------

proc tksu::ModuleBounds {} {
    variable Instances

    foreach module [array names Instances] {
    	set id [lindex $Instances($module) 0]
	set bounds [.canvas coords $id]

	if [info exists union] {
	    set union [RectUnion $union $bounds]
	} else {
	    set union $bounds
	}
    }
    if [info exists union] {
    	return $union
    } else {
    	return "0 0 0 0"
    }
}

#-------------------------------------------------------------------------------
# LinkBounds -- return rectangle bounding all links in canvas.
#
# Args:     none
# Returns:  bounds	Smallest rectangle {x1 y1 x2 y2} that encloses all
#			link segments currently in the canvas.  If there are
#			no links, returns {0 0 0 0}.
#-------------------------------------------------------------------------------

proc tksu::LinkBounds {} {
    variable Links

    set xmin 0; set ymin 0
    set xmax 0; set ymax 0

    foreach linkid [array names Links] {
	set coords [GetLinkCoords $linkid]

	foreach x [lindex $coords 0] {
	    if {$xmin > $x} {set xmin $x}
	    if {$xmax < $x} {set xmax $x}
	}
	foreach y [lindex $coords 1] {
	    if {$ymin > $y} {set ymin $y}
	    if {$ymax < $y} {set ymax $y}
	}
    }
    return "$xmin $ymin $xmax $ymax"
}

#-------------------------------------------------------------------------------
# RectUnion -- return union of two rectangles.
#
# Args:     a		First rectangle {x1 y1 x2 y2}.
#	    b		Second rectangle {x1 y1 x2 y2}.
# Returns:  union	The smallest rectangle that encloses the two.
#-------------------------------------------------------------------------------

proc tksu::RectUnion {a b} {

    set x1 [expr [lindex $a 0] < [lindex $b 0] ? [lindex $a 0] : [lindex $b 0]]
    set y1 [expr [lindex $a 1] < [lindex $b 1] ? [lindex $a 1] : [lindex $b 1]]
    set x2 [expr [lindex $a 2] > [lindex $b 2] ? [lindex $a 2] : [lindex $b 2]]
    set y2 [expr [lindex $a 3] > [lindex $b 3] ? [lindex $a 3] : [lindex $b 3]]

    return "$x1 $y1 $x2 $y2"
}

#-------------------------------------------------------------------------------
# Round -- return x rounded to nearest multiple of GridInc.
#
# Args:     x		Integer or float value.
# Returns:  gridx	Value x rounded.
#-------------------------------------------------------------------------------

proc tksu::Round x {
    variable GridInc

    set i [expr floor($x/$GridInc + 0.5)]
    return [expr $GridInc * $i]
}

#-------------------------------------------------------------------------------
# RoundDown -- return x rounded down to nearest multiple of GridInc.
#
# Args:     x		Integer or float value.
# Returns:  gridx	Value x rounded down.
#-------------------------------------------------------------------------------

proc tksu::RoundDown x {
    variable GridInc

    set i [expr floor($x/$GridInc)]
    return [expr $GridInc * $i]
}

#-------------------------------------------------------------------------------
# RoundUp -- return x rounded up to nearest multiple of GridInc.
#
# Args:     x		Integer or float value.
# Returns:  gridx	Value x rounded up.
#-------------------------------------------------------------------------------

proc tksu::RoundUp x {
    variable GridInc

    set i [expr ceil($x/$GridInc)]
    return [expr $GridInc * $i]
}

#-------------------------------------------------------------------------------
# ExpandCanvas -- expand canvas area to enclose all modules and links.
#
# Args:     none
# Returns:  null
#
# More accurately, the canvas area is shrink-wrapped (with a border padding
# of GridInc) to enclose the modules and links in the canvas.  It is never
# shrunk more than its original size defined in CanvasDim.
#-------------------------------------------------------------------------------

proc tksu::ExpandCanvas {} {
    variable CanvasDim
    variable GridInc

    set original "0 0 $CanvasDim(x) $CanvasDim(y)"
    set region [RectUnion [ModuleBounds] [LinkBounds]]
    set region [ExpandBox $region $GridInc]
    set region [RectUnion $region $original]
    .canvas configure -scrollregion $region
    return
}

#-------------------------------------------------------------------------------
# ScrollCanvas -- scroll canvas area so that given bounds are in view.
#
# Args:     bounds	Rectangle {x1 y1 x2 y2} to be shown in canvas window.
# Returns:  null
#
# If bounds already in view, no change.
#-------------------------------------------------------------------------------

proc tksu::ScrollCanvas bounds {

    set region [.canvas cget -scrollregion]
    set rx0 [lindex $region 0]
    set ry0 [lindex $region 1]
    set rwidth  [expr [lindex $region 2] - $rx0]
    set rheight [expr [lindex $region 3] - $ry0]

    set x1 [expr ([lindex $bounds 0] - $rx0)/$rwidth]
    set y1 [expr ([lindex $bounds 1] - $ry0)/$rheight]
    set x2 [expr ([lindex $bounds 2] - $rx0)/$rwidth]
    set y2 [expr ([lindex $bounds 3] - $ry0)/$rheight]

    set xv1 [lindex [.canvas xview] 0]
    set yv1 [lindex [.canvas yview] 0]
    set xv2 [lindex [.canvas xview] 1]
    set yv2 [lindex [.canvas yview] 1]

    if {$x1 < $xv1} {
    	.canvas xview moveto $x1
    } elseif {$x2 > $xv2} {
    	.canvas xview moveto [expr $xv1 + $x2 - $xv2]
    }

    if {$y1 < $yv1} {
    	.canvas yview moveto $y1
    } elseif {$y2 > $yv2} {
    	.canvas yview moveto [expr $yv1 + $y2 - $yv2]
    }
    return
}
