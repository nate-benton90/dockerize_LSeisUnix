# Enter.tcl --
#
#	Tksu procedures to handle the Enter and Default buttons in the
#	parameter and port dialogs.  Also handles parameter history (which
#	is currently disabled).
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Enter.tcl,v 1.3 2002/07/09 22:42:35 jeff Exp $

#-------------------------------------------------------------------------------
# ParamEnter -- handler for `enter' button in parameter or port dialog.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ParamEnter instance {
    variable CurrentParam
    variable CurrentValue

# Save dialog contents in history.

    PushDialog $instance

# Create new vlist, then let UpdateValue handle the rest.

    set vlist [GetDialogValue $instance]
    UpdateValue $instance $vlist

# Update thumbwheel value.

    set type [lindex $CurrentParam($instance) 1]
    if {$type == "int" || $type == "float"} {
	set w .param$instance.param.v2.thumb
	set range [lindex $CurrentParam($instance) 2]
	set value [lindex $CurrentValue($instance) 3]
	if {$value == ""} {
	    set value [lindex $CurrentParam($instance) 3]
	}
	InitThumb $w $range $value $type
    }
    return
}

#-------------------------------------------------------------------------------
# UpdateValue -- save the given value to the parameter list.
#
# Args:     instance	Instance number of the parameter list window.
#	    newvlist	New vlist value to be stored in Values.
# Returns:  null
#
# Before the new value is stored, the value is verified.  If it fails the
# verify test, nothing happens other than saving the value in the history
# list.  After Values list is updated, the parameter list is redisplayed.
#-------------------------------------------------------------------------------

proc tksu::UpdateValue {instance newvlist} {
    variable CurrentModule
    variable CurrentParam
    variable CurrentValue
    variable Links

    set genericName [lindex $CurrentValue($instance) 1]
    set oldName [lindex $CurrentValue($instance) 2]

# Verify new name & value.

    set name [lindex $newvlist 2]
    if {$name == ""} return

    set newValue [lindex $newvlist 3]
    set oldValue [lindex $CurrentValue($instance) 3]
    if {$name == $oldName && $newValue == $oldValue} return

    set w .param$instance
    if {![VerifyValue $w $CurrentParam($instance) $newvlist]} return

# Port update logic:

    set linkChanged 0
    if {[lindex $newvlist 0] == "port"} {

	set thisModule $CurrentModule($instance)
	set thisPort   [lindex $CurrentParam($instance) 0]
	set thisPort   [PortIndex $thisModule $thisPort]

# Link add/modify logic:

	if {[lindex $newValue 0] == "pipe"} {

# Try to link to {module iport} given in the port value.  If the link already
# exists, do nothing.  Otherwise, delete the topmost existing link from the
# CurrentValue.  Then if {module iport} are both non-null, try to link.

	    set thatModule [lindex $newValue 1]
	    set thatPort   [lindex $newValue 2]
	    set thatPort   [PortIndex $thatModule $thatPort]

# If same link already exists, noop.

	    set linkid [GetLink $thisModule $thisPort $thatModule $thatPort]
	    if {$linkid != ""} return

# If a link already exists to this port, delete it.  Any further links to
# to the port are retained.

	    set oldValue [lindex $CurrentValue($instance) 3]

	    if {[lindex $oldValue 0] == "pipe"} {
		set linkid [lindex $oldValue 1]
		if [info exists Links($linkid)] {
		    UndoLink $linkid delete
	    	    DeleteLink $linkid
		    ExpandCanvas
		    set linkChanged 1
		}
	    }

# Create new link if module and iport are non-null.  LinkPort does not
# deactivate the port if it fails, so deactivate it here.

	    if {$thatModule != "" && $thatPort != ""} {
		ActivatePort $thisModule $thisPort
		set linkid [LinkPort $thatModule $thatPort]
		if {$linkid != ""} {
		    UndoLink $linkid add
		    set linkChanged 1
		} else {
		    DeactivatePort
		}
	    }
	    if $linkChanged {
	    	AddMark
		return
	    }

# Pipe is specified but no link was added or deleted:  just update
# current value with `pipe'.

	    set newvlist [lreplace $newvlist 3 3 pipe]

# File specification logic:  Delete all links currently attached to port.

	} else {
	    set oldValue [lindex $CurrentValue($instance) 3]

	    if {[lindex $oldValue 0] == "pipe"} {
	    	foreach linkid [lrange $oldValue 1 end] {
		    if [info exists Links($linkid)] {
			UndoLink $linkid delete
	    		DeleteLink $linkid
			set linkChanged 1
		    }
		}
	    }
	    if $linkChanged ExpandCanvas
	}
    }

# Copy newvlist to CurrentValue and Values, without overwriting history.

    set vlist "[lrange $newvlist 0 3]\
	       [lrange $CurrentValue($instance) 4 5]"
    set CurrentValue($instance) $vlist
    SaveCurrentValue $instance 1
    AddMark

# For ports, make final update to port dialog and port box.

    if {[lindex $newvlist 0] == "port"} {
	LoadPortDialog $instance $CurrentParam($instance) $vlist
    	PortBox $thisModule $thisPort
    }

# Reload parameter list.

    set sellist [SaveSelection $w.list]
    LoadParamList $instance $CurrentModule($instance)
    RestoreSelection $w.list $sellist
    return
}

#-------------------------------------------------------------------------------
# ParamUndo -- handler for `undo' button in parameter or port dialog.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# The current dialog value is pushed onto the redo stack, and a value is
# popped off the undo stack and restored to the dialog.
#-------------------------------------------------------------------------------

proc tksu::ParamUndo instance {
    variable CurrentValue
    
    set class [lindex $CurrentValue($instance) 0]
    switch -- $class {

	param {
	    set value [GetParamValue $instance]
	    PushHistory $instance $value redo

	    set value [PopHistory $instance $value undo]
	    if {$value != ""} {
	    	LoadParamValue $instance $value
	    }
	}
	port {
	    set value [GetPortValue $instance]
	    PushHistory $instance $value redo

	    set value [PopHistory $instance $value undo]
	    if {$value != ""} {
	    	LoadPortValue $instance $value
	    }
	}
    }
    return
}

#-------------------------------------------------------------------------------
# ParamRedo -- handler for `redo' button in parameter or port dialog.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# Similar to ParamUndo, only the history list access is swapped.
#-------------------------------------------------------------------------------

proc tksu::ParamRedo instance {
    variable CurrentValue
    
    set class [lindex $CurrentValue($instance) 0]
    switch -- $class {

	param {
	    set value [GetParamValue $instance]
	    PushHistory $instance $value undo

	    set value [PopHistory $instance $value redo]
	    if {$value != ""} {
	    	LoadParamValue $instance $value
	    }
	}
	port {
	    set value [GetPortValue $instance]
	    PushHistory $instance $value undo

	    set value [PopHistory $instance $value redo]
	    if {$value != ""} {
	    	LoadPortValue $instance $value
	    }
	}
    }
    return
}

#-------------------------------------------------------------------------------
# ParamClear -- handler for `default' button in parameter or port dialog.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# Save current settings in history (undo) list, then clear value to reenable
# the default.
#-------------------------------------------------------------------------------

proc tksu::ParamClear instance {
    variable CurrentValue
    
    set class [lindex $CurrentValue($instance) 0]
    set w .param$instance.$class

    switch -- $class {

	param {
	    set value [GetParamValue $instance]
	    PushHistory $instance $value undo
	    $w.value.entry delete 0 end
	}
	port {
	    set value [GetPortValue $instance]
	    PushHistory $instance $value undo
	    LoadPortValue $instance {}
	}
    }

# Experimental:  pushing the Default button implies an Enter operation.

    ParamEnter $instance
    return
}

#-------------------------------------------------------------------------------
# PushDialog -- push contents of dialog onto history list in CurrentValue.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::PushDialog instance {
    variable CurrentValue

    set class [lindex $CurrentValue($instance) 0]
    switch -- $class {
	param {
	    set value [GetParamValue $instance]
	    PushHistory $instance $value undo
	}
	port {
	    set value [GetPortValue $instance]
	    PushHistory $instance $value undo
	}
    }
    return
}

#-------------------------------------------------------------------------------
# PushHistory -- push value onto history list in CurrentValue.
#
# Args:     instance	Instance number of parameter list window.
#	    value	Value to be pushed.
#	    choice	Which list to push onto: `undo' or `redo'.
# Returns:  null
#
# Push the given value onto the top of the stack.  In addition, remove any
# other entries in the stack equal to the pushed value, to remove redundancy.
# The stack is limited to 10 entries.
#-------------------------------------------------------------------------------

proc tksu::PushHistory {instance value choice} {
    variable CurrentValue

# DISABLED 5/3/02:  vlist history may be obsolete.  It's function has been
# taken over by the Undo/Redo commands and CanvasUndo.  Don't save history
# for now, and consider removing it completely later.
    return

    if {$choice == "undo"} {
    	set index 4
    } else {
    	set index 5
    }
    set oldStack [lindex $CurrentValue($instance) $index]
    set newStack [list $value]

    set n 1
    foreach oldValue $oldStack {
	if {$oldValue == $value} continue
	lappend newStack $oldValue
	incr n
	if {$n >= 10} break
    }
    set CurrentValue($instance) [lreplace $CurrentValue($instance) \
    				 $index $index $newStack]
    return
}

#-------------------------------------------------------------------------------
# PopHistory -- pop value from history list in CurrentValue.
#
# Args:     instance	Instance number of parameter list window.
#	    value	Value to compare top of list with.
#	    choice	Which list to pop from: `undo' or `redo'.
# Returns:  value	New popped value, or null if list is empty.
#
# If the top element of the stack matches the value argument, pop and discard
# it.  The next element in the stack is always different or null (PushHistory
# guarantees this).  Return this element and retain it on the stack.
#-------------------------------------------------------------------------------

proc tksu::PopHistory {instance value choice} {
    variable CurrentValue

    if {$choice == "undo"} {
    	set index 4
    } else {
    	set index 5
    }
    set oldStack [lindex $CurrentValue($instance) $index]
    set newStack {}

    set oldValue [lindex $oldStack 0]
    if {$oldValue != $value} {
    	return $oldValue
    }
    foreach oldValue $oldStack {
    	if {$oldValue == $value} continue
	lappend newStack $oldValue
    }
    set CurrentValue($instance) [lreplace $CurrentValue($instance) \
    				 $index $index $newStack]
    return [lindex $newStack 0]
}

#-------------------------------------------------------------------------------
# VerifyValue -- test if value is acceptable.
#
# Args:     w		Parent of modal dialog.
#	    nlist	Parameter name object.
#	    vlist	Parameter value object, containing value to be tested.
# Returns:  boolean	0 or 1, depending on whether parameter value passes
#			all tests.
#
# If test fails, a modal dialog is raised reporting the problem.
#-------------------------------------------------------------------------------

proc tksu::VerifyValue {w nlist vlist} {

    set type [lindex $nlist 1]
    set multiple [regsub -- {-list$} $type {} type]
    set range [lindex $nlist 2]
    set rw $range
    set default [lindex $nlist 3]

    set class [lindex $vlist 0]
    set genericName [lindex $vlist 1]
    set name [lindex $vlist 2]

# Verify name.

    regsub -- {\(.+\)$} $name {} name
    if {$name == ""} {
    	VerifyFail $w name "name is empty."
	return 0
    }
    if {[regexp -- {[^_a-zA-z0-9-]} $name]} {
    	VerifyFail $w $name "parameter name has inappropriate characters."
	return 0
    }

# Variable-name parameter.

    if [regexp -- {^enum-} $name] {
    	VerifyFail $w $name "name cannot start with `enum-'."
	return 0
    }
    if [regexp -- {^enum-(.*)} $genericName dummy suffix] {
    	if {$suffix == ""} {
	    VerifyFail $w $name "`$genericName' is not an enumeration."
	    return 0
	}
	if {[EnumDesc $genericName $name] == ""} {
	    VerifyFail $w $name "`$name' is not a member of list `$suffix'."
	    return 0
	}
    }

# Port value tests.

    if {$class == "port"} {

	set value [lindex $vlist 3]
	if {$value == ""} {
	    return 1
	}
	if {[llength $value] > 3} {
	    VerifyFail $w $name "port value `$value' is badly formatted."
	    return 0
	}
	set target [lindex $value 1]
	switch -- [lindex $value 0] {
	    pipe {
		if {$rw == "rw"} {
		    VerifyFail $w $name "cannot use a pipe for a\
		    			 random-access (rw) port."
		    return 0
		}
	    }
	    file {}
	    temp {}
	    default {
	    	VerifyFail $w $name "port value `$value' is badly formatted."
		return 0
	    }
	}
	return 1;	# Accept value.
    }

# For integer and floating values, get lo and hi limits (null if no limit),
# and limit test (<= or <).

    if {$type == "int" || $type == "float"} {
    	set crange [CanonizeRange $range $type 12]
	set lo       [lindex $crange 0]
	set hi       [lindex $crange 1]
	set attained [lindex $crange 2]
	if $attained {
	    set test "<="
	} else {
	    set test "<"
	}
    }

# For a multiple-value entry, split into list of values.

    set values [split [lindex $vlist 3] ,]

    if {[llength $values] > 1 && ! $multiple} {
	VerifyFail $w $name "this parameter does not accept a multiple-value\
			     string."
	return 0
    }
    foreach value $values {

# Trim leading and trailing white space.  Null value is OK -- causes default
# to be applied.

	set value [string trim $value]
	if {$value == ""} continue

# Integer value may be hexadecimal.

	if {$type == "int"} {
	    if [regexp {^0x} $value] {
		set format %x%s
	    } else {
		set format %d%s
	    }
	    if {[scan $value $format val dummy] != 1} {
		VerifyFail $w $name "value `$value' is not an integer."
		return 0
	    }

# Floating point type.

	} elseif {$type == "float"} {
	    if {[scan $value %f%s val dummy] != 1} {
		VerifyFail $w $name "value `$value' is not a float."
		return 0
	    }

# Enumeration type.

	} elseif [regsub -- {^enum-} $type {} suffix] {
	    if {[EnumDesc $type $value] == ""} {
	    	VerifyFail $w $name "value `$value' is not a member of\
				     list `$suffix'."
		return 0
	    }

# String type -- accept anything.

	} elseif {$type == "string"} {
	    continue

# Unknown type.

	} else {
	    VerifyFail $w $name "value type `$type' is unknown."
	    return 0
	}

# Check that int and float values are in range.

	if {$type == "int" || $type == "float"} {
	    if {$lo != ""} {
	    	if [expr !($lo $test $val)] {
	    	    VerifyFail $w $name "value `$value' is out of range."
		    return 0
		}
	    }
	    if {$hi != ""} {
	    	if [expr !($val $test $hi)] {
	    	    VerifyFail $w $name "value `$value' is out of range."
		    return 0
		}
	    }
	}
    }
    return 1
}

#-------------------------------------------------------------------------------
# VerifyFail -- raise modal dialog, report verification failure.
#
# Args:     w		Parent of modal dialog.
#	    name	Name of parameter being verified.
#	    desc	Description of problem with parameter name or value.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::VerifyFail {w name desc} {

    tk_messageBox -icon error -parent $w -type ok \
    		  -title "Bad Parameter" \
		  -message "Parameter `$name':  $desc"
    return
}

#-------------------------------------------------------------------------------
# PortIndex -- return port index given port name.
#
# Args:     module	Module name (instantiated or basename).
#	    port	Port name from Ports(module) list.
# Returns:  iport	Index into Ports and Values for this port.  Returns
#			null if module or port name are null, or if port is
#			not found.
#-------------------------------------------------------------------------------

proc tksu::PortIndex {module port} {
    variable Ports

    if {$module == "" || $port == ""} return

    set modbase [ModBase $module]
    set iport 0
    foreach nlist $Ports($modbase) {
    	if {$port == [lindex $nlist 0]} {
	    return $iport
	}
	incr iport
    }
    return
}
