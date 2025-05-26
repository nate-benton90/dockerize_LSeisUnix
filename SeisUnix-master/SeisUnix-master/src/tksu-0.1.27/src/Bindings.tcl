# Bindings.tcl --
#
#	Tksu procedures bound to canvas objects.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Bindings.tcl,v 1.2 2002/07/30 20:50:44 jeff Exp $

#-------------------------------------------------------------------------------
# DragStart -- start drag in canvas.
#
# Args:     x y		Window (not canvas) coordinates of start of drag.
#	    type	`canvas', `module', `port', or `link'.
#	    args	If present, first element is module name, second
#			element is detail (see below).
# Returns:  null
#
# Place the drag information in variable DragActive.  Procedure DragMotion is
# called during the drag, and DragFinish at the end.  The contents of
# DragActive may be any of the following:
#
#   Canvas drag:  { triggered x y canvas }
#   Module drag:  { triggered x y module name select }
#   Port drag:    { triggered x y port name iport }
#   Link drag:    { triggered x y link id select }
#
# where:
#
#   triggered	Boolean, indicates that mouse has moved far enough to start
#		drag operation, and cursor has changed.  For module drag, if
#		triggered is 0, a select operation is performed.
#   x y		Canvas coordinates of the start of the drag.
#   canvas	Literal.
#   module	Literal.
#   port	Literal.
#   link	Literal.
#   name	Instantiated name of module.
#   id		Canvas id of link.
#   select	Selection choice: `single' or `toggle' (multiple).
#   iport	Port index.
#
#-------------------------------------------------------------------------------

proc tksu::DragStart {x y type args} {
    variable DragActive
    variable SavedCursor
    variable SelItems

# Convert to canvas coordinates.

    if {$DragActive != ""} return
    set x [.canvas canvasx $x]
    set y [.canvas canvasy $y]
    set DragActive "0 $x $y $type $args"

    set SavedCursor [.canvas cget -cursor]
    bind .canvas <Motion> "tksu::DragMotion %x %y"

# For moving modules, append to DragActive x, y and the list of modules to be
# moved.  If the module is selected, it and all other selected modules will
# move.  If the module is not selected, only it will move.

    if {$type == "module"} {
    	lappend DragActive $x $y
	set module [lindex $DragActive 4]
	set index [lsearch -exact $SelItems $module]
	if {$index >= 0} {
	    lappend DragActive [SelectedModules]
	} else {
	    lappend DragActive $module
	}

# For dragging link segment, initialize for DragLink.  Append to DragActive
# x, y, ix and iy.

    } elseif {$type == "link"} {
	set id [lindex $DragActive 4]
    	set ixiy [DragLinkStart $id $x $y]
	eval lappend DragActive $x $y $ixiy
    }
    return
}

#-------------------------------------------------------------------------------
# DragMotion -- called from motion events during a drag.
#
# Args:     x y		Window (not canvas) coordinates of motion point.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::DragMotion {x y} {
    variable DragActive
    variable Ports
    variable Color
    variable Threshold

# Convert to canvas coordinates.

    if {$DragActive == ""} return
    set type [lindex $DragActive 3]
    set x [.canvas canvasx $x]
    set y [.canvas canvasy $y]

# First determine if mouse has moved enough to trigger a drag operation (as
# opposed to a selection operation).  First element of DragActive is drag
# flag.

    if {![lindex $DragActive 0]} {
    	set x0 [lindex $DragActive 1]
	set y0 [lindex $DragActive 2]
	if {abs($x - $x0) < $Threshold && abs($y - $y0) < $Threshold} {
	    return
	}

	# Set drag triggered flag
	set DragActive [lreplace $DragActive 0 0 1]

# Change cursor if dragging initiated.

	switch -- $type {
 
	    canvas {
	    }
	    module {
		.canvas configure -cursor {fleur black}
	    }
	    link {
		set ix [lindex $DragActive 8]
		set iy [lindex $DragActive 9]

		if {$ix > 0 && $iy > 0} {
		    # Corner drag
		    .canvas configure -cursor {fleur black}

		} elseif {$ix > 0} {
		    # Horizontal drag
		    .canvas configure -cursor {sb_h_double_arrow black white}

		} elseif {$iy > 0} {
		    # Vertical drag
		    .canvas configure -cursor {sb_v_double_arrow black white}
		}
	    }
	    port {

# Start dragging new link from port.

		set module [lindex $DragActive 4]
		set iport [lindex $DragActive 5]

		set modbase [ModBase $module]
		set nlist [lindex $Ports($modbase) $iport]
		set type [lindex $nlist 1]
		set rw [lindex $nlist 2]

		set x1y1 [PortEdge $module $iport]
		set x1 [lindex $x1y1 0]
		set y1 [lindex $x1y1 1]

		set id1 [.canvas create line $x1 $y1 $x1 $y1 \
		    -fill black -width 6 -tags portdrag \
		    -capstyle butt -joinstyle miter]
		set id2 [.canvas create line $x1 $y1 $x1 $y1 \
		    -fill $Color($type) -width 4 -tags portdrag \
		    -capstyle butt -joinstyle miter]
		lappend DragActive $x1 $y1 $id1 $id2 $rw
	    	.canvas configure -cursor {pencil black white}
		PushInfo portdrag " Drag cursor to connecting port."
	    }
	}
    }

# Specific dragging actions.

    switch -- $type {

    	canvas {
	}
	module {
	    set xprev [lindex $DragActive 6]
	    set yprev [lindex $DragActive 7]
	    set modules [lindex $DragActive 8]
	    MoveModule [expr $x - $xprev] [expr $y - $yprev] $modules
	    set DragActive [lreplace $DragActive 6 7 $x $y]
	}
	port {
	    set x1  [lindex $DragActive 6]
	    set y1  [lindex $DragActive 7]
	    set id1 [lindex $DragActive 8]
	    set id2 [lindex $DragActive 9]
	    set rw  [lindex $DragActive 10]

	    switch -- $rw {
	    	r {set coords [LinkPath $x $y $x1 $y1]}
		w {set coords [LinkPath $x1 $y1 $x $y]}
	    }
	    eval .canvas coords $id1 $coords
	    eval .canvas coords $id2 $coords
	}
	link {
	    set id    [lindex $DragActive 4]
	    set xprev [lindex $DragActive 6]
	    set yprev [lindex $DragActive 7]
	    DragLink $id [expr $x - $xprev] [expr $y - $yprev]
	    set DragActive [lreplace $DragActive 6 7 $x $y]
	}
    }
    return
}

#-------------------------------------------------------------------------------
# DragFinish -- complete drag in canvas.
#
# Args:     x y		Window (not canvas) coordinate of finish point.
# Returns:  null
#
# Reset cursor and remove motion binding.
#-------------------------------------------------------------------------------

proc tksu::DragFinish {x y} {
    variable DragActive
    variable SavedCursor
    variable Instances
    variable ActivePort

# Convert to canvas coordinates.

    if {$DragActive == ""} return
    set x [.canvas canvasx $x]
    set y [.canvas canvasy $y]

    bind .canvas <Motion> {}
    .canvas configure -cursor $SavedCursor

    set dragged [lindex $DragActive 0]
    set x0      [lindex $DragActive 1]
    set y0      [lindex $DragActive 2]
    set type    [lindex $DragActive 3]
    set module  [lindex $DragActive 4]
    set select  [lindex $DragActive 5]

# Specific actions.

    switch -- $type {

    	canvas {
	    SingleSelect {}
	    DeactivatePort
	}
	module {

# Moving module: finish up by moving module corner down to next whole grid
# interval.  Resize and scroll the canvas to the new location.

	    if $dragged {
		MoveModuleFinish
		MoveLinkFinish all
		AddMark

		set module [lindex $DragActive 4]
		set id [lindex $Instances($module) 0]
		set bounds [.canvas coords $id]
		ExpandCanvas
		ScrollCanvas $bounds

# Selecting module: single select or toggle selection.

	    } elseif {$select == "toggle"} {
		ToggleSelect $module
	    } else {
	    	SingleSelect $module
	    }
	}
	port {
	    set iport [lindex $DragActive 5]

# Creating link by dragging:  determine ending port (must be visible, on top),
# then create link between starting and ending ports.

	    if $dragged {
		.canvas delete portdrag
		PopInfo portdrag
		set modport [FindPortAt $x $y]
		if {$modport != ""} {
		    eval ActivatePort [lrange $DragActive 4 5]
		    set linkid [eval LinkPort $modport]
		    if {$linkid != ""} {
		    	UndoLink $linkid add
			AddMark
		    }
		}

# Button release in port.  If no active port, activate it.  If this port was
# already active, deactivate it.  Otherwise, another port is active:  attempt
# to link this port with the active port.

	    } elseif {$ActivePort == ""} {
	    	ActivatePort $module $iport
	    } elseif {$ActivePort == [list $module $iport]} {
	    	DeactivatePort
	    } else {
	    	set linkid [LinkPort $module $iport]
		if {$linkid != ""} {
		    UndoLink $linkid add
		    AddMark
		}
	    }
	}
	link {
	    set linkid [lindex $DragActive 4]

	    if $dragged {
		DragLinkFinish $linkid
		AddMark
		ExpandCanvas

# Selecting link: single select or toggle selection.

	    } elseif {$select == "toggle"} {
		ToggleSelect $module;	# module is actually link id
		MoveLinkFinish $linkid
	    } else {
	    	SingleSelect $module;	# module is actually link id
		MoveLinkFinish $linkid
	    }
	}
    }
    set DragActive {}
    return
}

#-------------------------------------------------------------------------------
# SelectedModules -- return list of selected modules.
#
# Args:     none
# Returns:  list	List of items in SelItems which are modules.
#-------------------------------------------------------------------------------

proc tksu::SelectedModules {} {
    variable SelItems
    variable Instances

    set list ""
    foreach item $SelItems {
    	if [info exists Instances($item)] {
	    lappend list $item
	}
    }
    return $list
}

#-------------------------------------------------------------------------------
# ShowSelect -- update selection state of objects in canvas.
#
# Args:     none
# Returns:  null
#
# Update items in canvas as selected or deselected, depending on the current
# contents of SelItems and DeselItems.  SelItems is a list of selected
# objects in the canvas, and DeselItems is the list of deselected objects.
#-------------------------------------------------------------------------------

proc tksu::ShowSelect {} {
    variable Color
    variable Instances
    variable SelItems
    variable DeselItems
    variable Links

# Configure selected items.

    foreach item $SelItems {
	if [info exists Instances($item)] {
	    # Item is a module
    	    set id [lindex $Instances($item) 0]
	    .canvas itemconfigure $id -fill $Color(selected) -outline white

	} else {
	    # Item is a link
	    set id [lindex [split $item :] 1]
	    .canvas itemconfigure $id -fill white
	    eval PortBox [lrange $Links($item) 0 1]
	    eval PortBox [lrange $Links($item) 2 3]
	}
    }

# Configure deselected items.

    foreach item $DeselItems {
	if [info exists Instances($item)] {
	    # Item is a module
    	    set id [lindex $Instances($item) 0]
	    .canvas itemconfigure $id -fill $Color(module) -outline black

	} else {
	    # Item is a link
	    set id [lindex [split $item :] 1]
	    .canvas itemconfigure $id -fill black
	    eval PortBox [lrange $Links($item) 0 1]
	    eval PortBox [lrange $Links($item) 2 3]
	}
    }
    return
}

#-------------------------------------------------------------------------------
# SingleSelect -- select single item, deselecting all others.
#
# Args:     item	Canvas item to select.  If null, causes all items
#			to be deselected.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::SingleSelect item {
    variable SelItems
    variable DeselItems
    variable Instances

# Deselect all:  lower selected links.  Modules don't need to be lowered.

    foreach selectedItem $SelItems {
	if [info exists Instances($selectedItem)] {
	    # Item is module
	} else {
	    # Item is link
	    .canvas lower $selectedItem
	}
    }
    set DeselItems [concat $SelItems $DeselItems]
    set SelItems {}

# Select single item:

    set index [lsearch -exact $DeselItems $item]
    if {$index >= 0} {
    	set DeselItems [lreplace $DeselItems $index $index]
	set SelItems $item

	if [info exists Instances($item)] {
	    # Item is module
	    .canvas raise $item modules
	} else {
	    # Item is link
	    .canvas raise $item
	}
    }
    ShowSelect
    return
}

#-------------------------------------------------------------------------------
# ToggleSelect -- toggle selection state of item, don't deselect others.
#
# Args:     item	Canvas item to select/deselect.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ToggleSelect item {
    variable SelItems
    variable DeselItems
    variable Instances

    set i1 [lsearch -exact $SelItems $item]
    set i2 [lsearch -exact $DeselItems $item]

# Deselect item if selected:

    if {$i1 >= 0} {
    	set SelItems [lreplace $SelItems $i1 $i1]
	lappend DeselItems $item

	if [info exists Instances($item)] {
	    # Item is module
	} else {
	    # Item is link
	    .canvas lower $item
	}

# Select item if deselected:

    } elseif {$i2 >= 0} {
    	set DeselItems [lreplace $DeselItems $i2 $i2]
	lappend SelItems $item

	if [info exists Instances($item)] {
	    # Item is module
	    .canvas raise $item modules
	} else {
	    # Item is link
	    .canvas raise $item
	}
    }
    ShowSelect
    return
}

#-------------------------------------------------------------------------------
# ModuleInfo -- display module info in status line.
#
# Args:     module	Instantiated module.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ModuleInfo module {
    variable Desc
    variable Disabled

    set modbase [ModBase $module]
    set str " Module $modbase"
    if {$Disabled($module) > 0} {
    	append str " (DISABLED)"
    }
    append str ":  $Desc($modbase)"
    PushInfo $module $str
    return
}

#-------------------------------------------------------------------------------
# PortInfo -- display port info in status line.
#
# Args:     module	Instantiated module.
#	    iport	Index of port in parameter list.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::PortInfo {module iport} {
    variable Values
    variable Ports
    variable DevNull
    variable DevZero

    set modbase [ModBase $module]
    set nlist [lindex $Ports($modbase) $iport]
    set vlist [lindex $Values($module) $iport]

    set name    [lindex $vlist 2]
    set value   [lindex $vlist 3]
    set choice  [lindex $value 0]
    set linkids [lrange $value 1 end]
    set type    [lindex $nlist 1]
    set default [lindex $nlist 3]
    set desc	" ([lrange $nlist 4 end])"

    if {[lindex $nlist 2] == "r"} {
    	set thisrw  input
	set thisdir from
	set thatrw  w
    } else {
    	set thisrw  output
	set thisdir to
	set thatrw  r
    }
    set text " Port $name (type $type):  $thisrw "

    switch -- $choice {
    	file {
	    set filename [lindex $value 1]
	    if {$filename == ""} {
	    	append text "file [DefaultValue short $default]"
	    } else {
	    	append text "file $filename"
	    }
	    append text $desc
	}
	temp {
	    if {$thisrw == "input"} {
	    	append text "from $DevZero"
	    } else {
	    	append text "to $DevNull"
	    }
	    append text $desc
	}
	pipe {
	    set n [llength $linkids]
	    if {$n > 1} {
	    	append text "pipe connected to multiple ports"

	    } elseif {$n == 1} {
		set modport [GetModPort [lindex $linkids 0] $thatrw]
		set modport "[lindex $modport 0] port [lindex $modport 1]"
	    	append text "pipe $thisdir $modport"

	    } else {
	    	append text "pipe DISCONNECTED"
		append text $desc
	    }
	}
	default {
	    append text "file [DefaultValue short $default]"
	    append text $desc
	}
    }
    PushInfo $module $text
    return
}

#-------------------------------------------------------------------------------
# PushInfo -- push message onto InfoStack, display on status line.
#
# Args:     module	Module associated with message.
#	    message	Message to display.
# Returns:  null
#
# The module and message are pushed onto InfoStack, then the new message is
# displayed in the status line.  Call PopInfo to remove the message and
# restore the previous message to the status line.
#-------------------------------------------------------------------------------

proc tksu::PushInfo {module message} {
    variable InfoStack

    lappend InfoStack [list $module $message]

    .status configure -state normal
    .status delete 0 end
    .status insert 0 $message
    .status selection clear
    .status configure -state disabled
    return
}

#-------------------------------------------------------------------------------
# PopInfo -- pop specific message from InfoStack.
#
# Args:     module	Identifies which message is to be removed.
# Returns:  message	The message removed from InfoStack (if any).
#
# Scan InfoStack from top to bottom, and remove the module/message pair that
# matches the module argument.  If it is the top pair, the new top message
# in InfoStack is displayed in the status line.
#-------------------------------------------------------------------------------

proc tksu::PopInfo module {
    variable InfoStack

    set message ""
    set n [expr [llength $InfoStack] - 1]

    for {set i $n} {$i >= 0} {incr i -1} {
	set modmess [lindex $InfoStack $i]
	if {$module == [lindex $modmess 0]} {
	    set InfoStack [lreplace $InfoStack $i $i]
	    set message [lindex $modmess 1]
	    break;
	}
    }
    if {$i == $n} {
	incr i -1
	set modmess [lindex $InfoStack $i]
	.status configure -state normal
	.status delete 0 end
	.status insert 0 [lindex $modmess 1]
	.status selection clear
	.status configure -state disabled
    }
    return $message
}

#-------------------------------------------------------------------------------
# DeleteSelected -- delete all selected items.
#
# Args:     none
# Returns:  null
#
# Readjust the size of the canvas after deleting any items.
#-------------------------------------------------------------------------------

proc tksu::DeleteSelected {} {
    variable Instances
    variable Links
    variable SelItems

    set changed 0

# Delete selected links first.

    foreach item $SelItems {
	if [info exists Links($item)] {
	    UndoLink $item delete
	    DeleteLink $item
	    set changed 1
	}
    }

# The remaining selected items must be modules.

    foreach item $SelItems {
	DeleteModule $item
	set changed 1
    }
    if $changed AddMark
    ExpandCanvas
    return
}

#-------------------------------------------------------------------------------
# DeleteModule -- delete module from canvas.
#
# Args:     module	Named instance of module.
#	    allowUndo	Optional: if 1, add entries to CanvasUndo to
#			allow the delete operation to be undone.  If 0, the
#			CanvasUndo array is unchanged.  During an undo/
#			redo operation, DeleteModule should be called with
#			allowUndo = 0 in order to prevent a nasty recursion.
#			Default value is 1.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::DeleteModule {module {allowUndo 1}} {
    variable Values
    variable Instances
    variable SelItems
    variable DeselItems
    variable CurrentModule
    variable ActivePort
    variable InfoStack
    variable Disabled

# Put away any parameter list window showing this module.

    set instance 1
    while {[info exists CurrentModule($instance)]} {
    	if {$module == $CurrentModule($instance)} {
	    HideParamList $instance
	}
	incr instance
    }

# If one of its ports is active, deactivate.
# Clear status line if it shows info about the module to be deleted.

    if {$module == [lindex $ActivePort 0]} {
    	DeactivatePort
    }
    PopInfo $module

# Delete all links to this module.

    set hadLinks 0
    foreach vlist $Values($module) {
    	if {[lindex $vlist 0] != "port"} continue
	set value [lindex $vlist 3]
	if {[lindex $value 0] != "pipe"} continue
	set linkids [lrange $value 1 end]
	foreach linkid $linkids {
	    if ($allowUndo) {
	    	UndoLink $linkid delete
	    }
	    DeleteLink $linkid
	    set hadLinks 1
	}
    }

# Remove canvas items making up module.  Remove from Values, Instances and
# the selection lists.

    if ($allowUndo) {
    	UndoModule $module delete
    }
    .canvas delete $module
    unset Values($module)
    unset Instances($module)

    set index [lsearch -exact $SelItems $module]
    if {$index >= 0} {
    	set SelItems [lreplace $SelItems $index $index]
    }
    set index [lsearch -exact $DeselItems $module]
    if {$index >= 0} {
    	set DeselItems [lreplace $DeselItems $index $index]
    }

# If the module was disabled, deleting it may reenable downstream modules.

    set wasDisabled $Disabled($module)
    unset Disabled($module)
    if {$wasDisabled > 0 && $hadLinks} {
    	EnableModules
    }
    return
}

#-------------------------------------------------------------------------------
# NextItem -- return next item in canvas to traverse to.
#
# Args:     refItem	Reference item in canvas, or {} if the first or
#			last item is to be returned.  Selectable items are
#			presently modules and links.
#	    items	Pick the next item from this list.
#	    direction	`forward' or `reverse', defining the direction to
#			traverse.
# Returns:  next	If direction is forward, next is either the item after
#			the reference item or the first item in the list.  If
#			there is no next item, return null.
#			If direction is reverse, next is either the item
#			immediately before the reference item or the last
#			item in the list.  If there is no previous item,
#			return null.
#
# For traversal purposes, modules are ordered primarily by the y1 coordinate
# and secondarily by the x1 coordinate.  Links are ordered by their output
# port in the module.  Thus modules are traversed from the top of the canvas
# to bottom, and output ports with links are traversed in each module.
#-------------------------------------------------------------------------------

proc tksu::NextItem {refItem items direction} {

    switch -- $direction {
	forward { set le <= }
    	reverse { set le >= }
    }
    if {$refItem != ""} {
	set refOrder [ItemOrder $refItem]
    }

# Loop over items.

    set nextItem ""
    foreach item $items {
	set order [ItemOrder $item]

# Skip items previous (or equal) to refItem.

	if {$refItem != ""} {
	    if [expr [ItemCompare $order $refOrder] $le 0] continue
	}

# Skip modules beyond next.

	if {$nextItem != ""} {
	    if [expr [ItemCompare $nextOrder $order] $le 0] continue
	}

# Found candidate for next module.

	set nextItem $item
	set nextOrder $order
    }
    return $nextItem
}

#-------------------------------------------------------------------------------
# ItemOrder -- return order indices for item in canvas.
#
# Args:     item	Item in canvas:  either module name or link id.
# Returns:  order	List with 4 elements, { y x module iport }, where:
#			x,y	= upper left corner of module containing link,
#			module  = module instance name,
#			iport   = index of output port for this link, or -1 if
#				  if item is not a link.
#			If item doesn't exist, return null.
#-------------------------------------------------------------------------------

proc tksu::ItemOrder item {
    variable Instances
    variable Links

    if [info exists Instances($item)] {
    	# Item is a module
	set module $item
	set iport -1

    } elseif [info exists Links($item)] {
    	# Item is a link
	set module [lindex $Links($item) 0]
	set iport  [lindex $Links($item) 1]

    } else {
    	return {}
    }

    set id [lindex $Instances($module) 0]
    set coords [.canvas coords $id]
    set x [lindex $coords 0]
    set y [lindex $coords 1]
    return "$y $x $module $iport"
}

#-------------------------------------------------------------------------------
# ItemCompare -- return result of comparing two item orders.
#
# Args:     order1	First item order to compare.
#	    order2	Second item order to compare.
# Returns:  sign	-1 if order1 < order2,
#			+1 if order1 > order2,
#			0 if order1 == order2.
#
# The item orders are objects created by ItemOrder.
#-------------------------------------------------------------------------------

proc tksu::ItemCompare {order1 order2} {
    
    foreach i {0 1 2 3} {
	if {[lindex $order1 $i] < [lindex $order2 $i]} {return -1}
	if {[lindex $order1 $i] > [lindex $order2 $i]} {return 1}
    }
    return 0
}

#-------------------------------------------------------------------------------
# TabCanvas -- traverse to next item in canvas and single-select.
#
# Args:     tablist	List of widgets to tab over, including .canvas.
#	    direction	Direction to traverse in: `forward' or `reverse'.
# Returns:  null
#
# Besides traversing items in canvas, will traverse the other widgets in
# the top-level window, as in TabForward or TabReverse.
#-------------------------------------------------------------------------------

proc tksu::TabCanvas {tablist direction} {
    variable SelItems
    variable DeselItems
    variable Instances
    variable Links

# Traverse widgets outside canvas.

    if {[focus] != ".canvas"} {
    	switch -- $direction {
	    reverse { TabReverse $tablist }
	    default { TabForward $tablist }
	}
    }

# If focus arrives or is in canvas, traverse items in canvas.

    if {[focus] == ".canvas"} {
	set allItems "$SelItems $DeselItems"
	if {[llength $SelItems] > 1} {
	    set refItem [NextItem {} $SelItems $direction]
	} else {
	    set refItem $SelItems
	}
	set next [NextItem $refItem $allItems $direction]
	SingleSelect $next

# If finished traversing modules, advance focus out of canvas.

	if {$next == ""} {
    	    switch -- $direction {
		reverse { TabReverse $tablist }
		default { TabForward $tablist }
	    }

# Otherwise scroll canvas to newly selected module or link output port.

	} else {
	    if [info exists Instances($next)] {
		# Item is a module
		set module $next
		ModuleInfo $module
		after 5000 "tksu::PopInfo $module"
	
	    } else {
	    	# Item is a link
		set module [lindex $Links($next) 0]
		LinkInfo $next
		after 5000 "tksu::PopInfo $next"
	    }
	    set id [lindex $Instances($module) 0]
	    ScrollCanvas [.canvas coords $id]
	}
    }
    return
}
