# LinkPort.tcl --
#
#	Tksu procedures to support linking/unlinking ports.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: LinkPort.tcl,v 1.2 2002/06/15 22:54:31 jeff Exp $

#-------------------------------------------------------------------------------
# ActivatePort -- select first port of two to link together.
#
# Args:     module	Instantiated module name.
#	    iport	Index of port in Values($module).
# Returns:  null
#
# ActivePort is set to the given port, and the port box is set blinking.
# A subsequent call to LinkPort will try to connect this port with the port
# given in the call.
#-------------------------------------------------------------------------------

proc tksu::ActivatePort {module iport} {
    variable ActivePort

    DeactivatePort
    set ActivePort [list $module $iport]
    PortBox $module $iport
    PushInfo activate \
    	" Click on a second port to link, or click elsewhere to cancel."
    return
}

#-------------------------------------------------------------------------------
# DeactivatePort -- cancel the link process started by ActivatePort.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::DeactivatePort {} {
    variable ActivePort

    set module [lindex $ActivePort 0]
    set iport [lindex $ActivePort 1]
    set ActivePort {}

    if {$module != "" && $iport != ""} {
    	PortBox $module $iport
	PopInfo activate
    }
    return
}

#-------------------------------------------------------------------------------
# LinkPort -- link given port with the active port.
#
# Args:     module	Instantiated module name.
#	    iport	Index of port in Values($module).
#	    ShowWarn	If 1, warning dialogs will be raised.  If 0, the
#			warning dialogs are suppressed and the link is
#			accepted.  Optional:  default value is 1.
# Returns:  linkid	Returns index into Links array if successful, or
#			null if unsuccessful.
#
# Tests are perform to verify that the two ports may be connected together.
# If it is impossible to connect them, an error dialog is raised, and 0 is
# returned after the user acknowledges the error.  Other tests may result in
# a warning dialog, giving the user a choice of accepting or canceling the
# operation.
#
# Upon passing the verification tests, an appropriate entry is made to Links,
# and a new link item is added to the canvas.  The format of an entry in the
# Links list is
#
#   Links(linkid) = { outputModule outputPort inputModule inputPort }
#
# where
#
#   linkid	  = "link:id1:id2" where id1 and id2 are canvas line ids.
#   outputModule  = Output module name
#   outputPort    = Output port index
#   inputModule   = Input module name
#   inputPort     = Input port index
#
#-------------------------------------------------------------------------------

proc tksu::LinkPort {module iport {showWarn 1}} {
    variable ActivePort
    variable Ports
    variable Values
    variable Links
    variable DeselItems
    variable Color
    variable Disabled

    set modbase [ModBase $module]
    if {$ActivePort == ""} return

# Determine which port is the input port.

    set nlist [lindex $Ports($modbase) $iport]
    if {[lindex $nlist 2] == "r"} {
    	set mi $module
	set pi $iport
	set mo [lindex $ActivePort 0]
	set po [lindex $ActivePort 1]
    } else {
	set mi [lindex $ActivePort 0]
	set pi [lindex $ActivePort 1]
    	set mo $module
	set po $iport
    }

# Input port specs in ni, vi, and output port specs in no, vo.

    set ni [lindex $Ports([ModBase $mi]) $pi]
    set vi [lindex $Values($mi) $pi]
    set no [lindex $Ports([ModBase $mo]) $po]
    set vo [lindex $Values($mo) $po]

# Verify ports.

    if {[lindex $ni 2] != "r" || [lindex $no 2] != "w"} {
	LinkError "A connection may be made only between an input port\
		   and an output port."
	return
    }
    set value [lindex $vi 3]
    if {[lindex $value 0] == "pipe" && [lindex $value 1] != ""} {
    	LinkError "The input port is already connected.  Delete the\
		   old connection first."
	return
    }
    set typei [lindex $ni 1]
    set typeo [lindex $no 1]
    if {$typei != $typeo && $showWarn} {
    	set ok [LinkWarning "The input data type ($typei) does not match\
			     the output data type ($typeo).  Is that OK?"]
	if {!$ok} return
    }
    if {$mi == $mo && $showWarnings} {
    	set ok [LinkWarning "The input and output ports are on the same\
			     module.  Is that OK?"]
	if {!$ok} return
    }

# Determine link coordinates.  Create canvas line item.

    set x1y1 [PortEdge $mo $po]
    set xnyn [PortEdge $mi $pi]
    set coords [eval LinkPath $x1y1 $xnyn]

    set id1 [eval .canvas create line $coords -fill black -width 6 \
    	-capstyle butt -joinstyle miter]
    set id2 [eval .canvas create line $coords -fill $Color($typeo) -width 4 \
    	-capstyle butt -joinstyle miter]

    set linkid "link:$id1:$id2"
    .canvas addtag $linkid withtag $id1
    .canvas addtag $linkid withtag $id2
    .canvas lower $linkid

# Add entry to Links.  Update entries in Values.  The output port may have
# more than one link attached:  append the new link id to its value.

    set value [lindex $vo 3]
    if {[lindex $value 0] == "pipe"} {
    	lappend value $linkid
    } else {
    	set value "pipe $linkid"
    }
    set vo [lreplace $vo 3 3 $value]
    set vi [lreplace $vi 3 3 "pipe $linkid"]
    set Values($mo) [lreplace $Values($mo) $po $po $vo]
    set Values($mi) [lreplace $Values($mi) $pi $pi $vi]

    set Links($linkid) "$mo $po $mi $pi"
    lappend DeselItems $linkid

# Update port box appearances:  solid color.

    PopInfo activate
    DeactivatePort
    ExpandCanvas
    PortBox $mi $pi
    PortBox $mo $po

# If either of the modules is disabled, propagate the disabled state.

    if {$Disabled($mi) > 0 || $Disabled($mo) > 0} {
    	EnableModules
    }

# Link bindings.

    .canvas bind $linkid <ButtonPress-1> \
    	"tksu::DragStart %x %y link $linkid single"
    .canvas bind $linkid <Control-ButtonPress-1> \
    	"tksu::DragStart %x %y link $linkid toggle"
    .canvas bind $linkid <ButtonRelease-3> "tksu::LinkNode $linkid %x %y"
    .canvas bind $linkid <Enter> "tksu::LinkInfo $linkid"
    .canvas bind $linkid <Leave> "tksu::PopInfo $linkid"

# Update any visible port dialogs showing these ports.

    UpdateLinkDialogs $mi $pi
    UpdateLinkDialogs $mo $po
    return $linkid
}

#-------------------------------------------------------------------------------
# LinkError -- raise linking error dialog.
#
# Args:     message	Error message to display in dialog.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::LinkError message {

    tk_messageBox -icon error -parent .canvas -type ok \
    	-message $message -title "Port connection error"
    return
}

#-------------------------------------------------------------------------------
# LinkWarning -- raise linking warning dialog.
#
# Args:     message	Error message to display in dialog.
# Returns:  choice	User's response: 1 for OK, 0 for Cancel.
#-------------------------------------------------------------------------------

proc tksu::LinkWarning message {

    set answer [tk_messageBox -icon warning -parent .canvas -type okcancel \
    	-default cancel -message $message -title "Port connection warning"]
    if {$answer == "ok"} {return 1}
    return 0
}

#-------------------------------------------------------------------------------
# LinkInfo -- display link info in status line.
#
# Args:     linkid	Id of link and index into Links.
# Returns:  null
#
# To remove the link info from the status line, invoke `PopInfo $linkid'.
#-------------------------------------------------------------------------------

proc tksu::LinkInfo linkid {

    set modport1 [join [GetModPort $linkid w] :]
    set modport2 [join [GetModPort $linkid r] :]
    regsub -- {:} $modport1 " port " modport1
    regsub -- {:} $modport2 " port " modport2
    PushInfo $linkid " Pipe from $modport1 to $modport2"
    return
}

#-------------------------------------------------------------------------------
# PortEdge -- return outside edge coordinate of port rectangle.
#
# Args:     module	Instantiated module.
#	    iport	Index of port in module.
# Returns:  x y		Canvas coordinates at center of outside edge.
#-------------------------------------------------------------------------------

proc tksu::PortEdge {module iport} {
    variable Instances
    variable Ports

    set modbase [ModBase $module]
    set nlist [lindex $Ports($modbase) $iport]
    set rw [lindex $nlist 2]

    set portids [lindex $Instances($module) 1]
    set linkid [lindex $portids $iport]
    set coords [.canvas coords $linkid]

    set x [expr 0.5*([lindex $coords 0] + [lindex $coords 2])]
    switch -- $rw {
    	r {set y [expr [lindex $coords 1] + 1]}
	w {set y [expr [lindex $coords 3] - 1]}
    }
    return "$x $y"
}

#-------------------------------------------------------------------------------
# FindPortAt -- return {module iport} for topmost port at point x,y.
#
# Args:     x y		Canvas coordinates to query.
# Returns:  modport	The list {module iport} which defines the visible
#			port that x,y is inside.  Returns null if x,y is
#			not in a visible port.
#-------------------------------------------------------------------------------

proc tksu::FindPortAt {x y} {
    variable Instances

    set ids [.canvas find overlapping $x $y $x $y]

# The topmost item (last in canvas list) should be a port.

    set n [expr [llength $ids] - 1]
    if {$n < 0} return
    set id [lindex $ids $n]
    set tags [.canvas gettags $id]

# However, the dragged line item may overlap the top port.  Ignore it.

    if {$tags == "portdrag"} {
    	incr n -1
	if {$n < 0} return
	set id [lindex $ids $n]
	set tags [.canvas gettags $id]
    }

# Ports have tag `port:id1:id2'.

    set module ""
    set porttag ""
    foreach tag $tags {
	if {[lindex [split $tag :] 0] == "port"} {
	    set porttag $tag
	} elseif [info exists Instances($tag)] {
	    set module $tag
	}
    }
    if {$porttag == "" || $module == ""} return

# Look up iport in that has the given porttag.

    set iport 0
    foreach portid [lindex $Instances($module) 1] {
    	if {$portid == $porttag} {
	    return "$module $iport"
	}
	incr iport
    }
    return
}

#-------------------------------------------------------------------------------
# LinkPath -- return acceptable path between two endpoints.
#
# Args:     x1 y1	Canvas coordinates of output port.
#	    xn yn	Canvas coordinates of input port.
# Returns:  coords	List {x1 y1 x2 y2 ... xn yn} defining segments of
#			a line item to be created from x1,y1 to xn,yn.
#-------------------------------------------------------------------------------

proc tksu::LinkPath {x1 y1 xn yn} {
    variable Stub

    set coords "$x1 $y1 $xn $yn"

# First and last segments must be vertical, directed up.

    if {$x1 == $xn && $y1 <= $yn} {

    } elseif {$yn - $y1 >= $Stub(y)} {
    	set y1 [expr 0.5*($y1 + $yn)]
	set coords [linsert $coords 2 $x1 $y1 $xn $y1]

    } else {
    	set y1 [expr $y1 + $Stub(y)]
	set yn [expr $yn - $Stub(y)]

	if {abs($x1 - $xn) < $Stub(x)} {
	    set x2 [expr $x1 + 2.0*$Stub(x)]
	} else {
	    set x2 [expr 0.5*($x1 + $xn)]
	}
	set coords [linsert $coords 2 $x1 $y1 $x2 $y1 $x2 $yn $xn $yn]
    }
    return $coords
}

#-------------------------------------------------------------------------------
# DeleteLink -- remove link.
#
# Args:     linkid	Id of link object to be removed.
# Returns:  null
#
# If the linkid refers to a nonexistent link (just removed), return quietly.
# Any port dialogs showing this link must be updated.
#-------------------------------------------------------------------------------

proc tksu::DeleteLink linkid {
    variable Links
    variable Values
    variable SelItems
    variable DeselItems
    variable Disabled

    PopInfo $linkid
    if {![info exists Links($linkid)]} return

    set mpr [lrange $Links($linkid) 2 3]
    set mpw [lrange $Links($linkid) 0 1]

    foreach modport [list $mpr $mpw] {
    	set module [lindex $modport 0]
	set iport  [lindex $modport 1]
	set vlist  [lindex $Values($module) $iport]
	set value  [lindex $vlist 3]

# Remove linkid from port value.

	set n [lsearch -exact $value $linkid]
	if {$n > 0} {
	    set value [lreplace $value $n $n]
	}
	set vlist [lreplace $vlist 3 3 $value]
	set Values($module) [lreplace $Values($module) $iport $iport $vlist]
    }

# Remove entry from Links and selected lists.

    .canvas delete $linkid
    unset Links($linkid)

    set index [lsearch -exact $SelItems $linkid]
    if {$index >= 0} {
    	set SelItems [lreplace $SelItems $index $index]
    }
    set index [lsearch -exact $DeselItems $linkid]
    if {$index >= 0} {
    	set DeselItems [lreplace $DeselItems $index $index]
    }

# If either connecting module was disabled, the disabled state of modules
# may change.

    if {$Disabled([lindex $mpr 0]) > 0 \
    ||  $Disabled([lindex $mpw 0]) > 0} {
    	EnableModules
    }

# Update port box appearance, and visible dialogs.

    eval PortBox $mpr
    eval PortBox $mpw
    eval UpdateLinkDialogs $mpr
    eval UpdateLinkDialogs $mpw
    return
}

#-------------------------------------------------------------------------------
# GetLink -- return link id given modules and iports.
#
# Args:     module1	Module, endpoint 1.
#	    iport1	Port index, endpoint 1 (either read or write).
#	    module2	Module, endpoint 2.
#	    iport2	Port index, endpoint 2 (either read or write).
# Returns:  linkid	Index into Links, or null if not found.
#
# Null is also returned if any of the arguments are null.
#-------------------------------------------------------------------------------

proc tksu::GetLink {module1 iport1 module2 iport2} {
    variable Links

    foreach linkid [array names Links] {

    	if {$module1 == [lindex $Links($linkid) 0]} {
	    if {$iport1  != [lindex $Links($linkid) 1]} continue
	    if {$module2 != [lindex $Links($linkid) 2]} continue
	    if {$iport2  != [lindex $Links($linkid) 3]} continue
	    return $linkid

	} elseif {$module1 == [lindex $Links($linkid) 2]} {
	    if {$iport1  != [lindex $Links($linkid) 3]} continue
	    if {$module2 != [lindex $Links($linkid) 0]} continue
	    if {$iport2  != [lindex $Links($linkid) 1]} continue
	    return $linkid
	}
    }
    return
}

#-------------------------------------------------------------------------------
# GetLinks -- return list of links attached to given module.
#
# Args:     module	Instantiated module.
# Returns:  list	List of links attached to module.  Each element of the
#			list has the format {linkid rw}, where linkid is the
#			id of the link (index into Links), and rw is the I/O
#			sense of the port the link is attached to: r = read
#			endpoint, w = write endpoint.  If there are no links,
#			return null.
#-------------------------------------------------------------------------------

proc tksu::GetLinks module {
    variable Values
    variable Ports

    set modbase [ModBase $module]
    set linkEndpoints ""
    set iport 0

    foreach vlist $Values($module) {
    	if {[lindex $vlist 0] != "port"} break
	set value [lindex $vlist 3]

	if {[lindex $value 0] == "pipe"} {
	    set nlist [lindex $Ports($modbase) $iport]
	    set rw [lindex $nlist 2]

	    foreach linkid [lrange $value 1 end] {
		lappend linkEndpoints [list $linkid $rw]
	    }
	}
	incr iport
    }
    return $linkEndpoints
}

#-------------------------------------------------------------------------------
# GetModPort -- return module and port name, given link id and direction.
#
# Args:     linkid	Link index defining a link.  If no link with this
#			index found, null is returned.
#	    rw		Direction of port: `r' for input, `w' for output.
# Returns:  modport	A list {module port} where module is an instantiated
#			module name and port is a port name (not an index).
#			Null may also be returned.
#-------------------------------------------------------------------------------

proc tksu::GetModPort {linkid rw} {
    variable Links
    variable Values

    if {![info exists Links($linkid)]} {
    	return {}
    }
    switch -- $rw {
    	r {set i 2}
	w {set i 0}
    }
    set module [lindex $Links($linkid) $i]
    incr i
    set iport [lindex $Links($linkid) $i]
    set vlist [lindex $Values($module) $iport]
    set port [lindex $vlist 2]
    return "$module $port"
}

#-------------------------------------------------------------------------------
# UpdateLinkDialogs -- update parameter list window showing module, port.
#
# Args:     module	Search for window displaying this module.
#	    iport	Index of parameter or port to update.  If it is
#			currently being shown in the Parameter/Port dialog,
#			the dialog is updated.  Otherwise, the dialog is
#			unchanged and only the parameter list is updated.
#	    force	Optional:  if on, the Parameter/Port dialog will be
#			changed to show iport.  Default value is off.
# Returns:  null
#
# If there is no parameter list window currently showing the specified module,
# do nothing.  The CurrentValue of the parameter window may be out of date
# because of some action in the canvas -- it must be updated from Values.
#-------------------------------------------------------------------------------

proc tksu::UpdateLinkDialogs {module iport {force 0}} {
    variable CurrentModule
    variable CurrentValue
    variable Values
    
    foreach instance [array names CurrentModule] {
	if {$module != $CurrentModule($instance)} continue
	set vlist [lindex $Values($module) $iport]
	set port [lindex $vlist 2]

# Found active window showing module, port.  OK to update it.

	if {$port == [lindex $CurrentValue($instance) 2] || $force} {
	    ParamListParam $module $iport 0

# If a different port/parameter is in the dialog area of the window, it does
# not have to be updated.  However, the parameter listbox itself must be
# updated.  Preserve the current selection.

	} else {
	    set listbox .param$instance.list
	    set index [lindex [SaveSelection $listbox] 0]
	    LoadParamList $instance $module
	    SetSelection $listbox $index
	}
    }
    return
}
