# Param.tcl --
#
#	Tksu procedures to handle the parameter dialog.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Param.tcl,v 1.4 2002/07/16 23:31:08 jeff Exp $

#-------------------------------------------------------------------------------
# CreateParamDialog -- create parameter dialog.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# The parameter and port dialogs share the space below the parameter listbox
# in the parameter list window.
#-------------------------------------------------------------------------------

proc tksu::CreateParamDialog instance {
    set awid 5
    set bwid 5
    set lwid 11

    set w .param$instance.param
    frame $w
    label $w.title -text "Set parameter"

# Parameter name:

    set f $w.name
    frame $f
    label $f.label -text Name: -anchor w -width $awid
    entry $f.entry -relief sunken -bg white
    button $f.dup -text Dup -width $bwid -padx 4 \
    		  -command "tksu::ParamNameDup $instance"
    button $f.enum -text List -underline 1 -width $bwid -padx 4 \
    		   -command "tksu::ParamNameEnum $instance"
    button $f.clear -text Clear -width $bwid -padx 4 \
    		    -command "tksu::ParamNameClear $instance"

# Parameter value:

    set f $w.value
    frame $f
    label $f.label -text Value: -anchor w -width $awid
    entry $f.entry -relief sunken -bg white
    button $f.enum -text List -underline 0 -width $bwid -padx 4 \
    		   -command "tksu::ParamEnum $instance"
    button $f.left -text << -underline 0 -width 2 -padx 2 \
    		   -command "tksu::ShiftEntry $f.entry -1"
    button $f.right -text >> -underline 0 -width 2 -padx 2 \
    		   -command "tksu::ShiftEntry $f.entry 1"

# Parameter thumbwheel and 0/1 buttons:

    set f $w.v2
    set cmd "LoadEntry $w.value.entry \[GetThumbValue $w.v2.thumb\]"
    frame $f
    label $f.label -anchor w -text Thumbwheel: -width $lwid
    CreateThumb $f.thumb 12 $cmd
    button $f.zero -text 0 -width 2 -padx 2 -takefocus 0 -command \
    	"tksu::LoadEntry $w.value.entry 0; tksu::ParamEnter $instance"
    button $f.one -text 1 -width 2 -padx 2 -takefocus 0 -command \
    	"tksu::LoadEntry $w.value.entry 1; tksu::ParamEnter $instance"

# Parameter range:

    set f $w.range
    frame $f
    label $f.label -text Range: -width $lwid -anchor w
    label $f.value -anchor w

# Parameter default:

    set f $w.default
    frame $f
    label $f.label -text Default: -width $lwid -anchor w
    label $f.value -anchor w -justify left -wraplength 12c

# Parameter description:

    set f $w.desc
    frame $f
    label $f.label -text Description: -width $lwid -anchor w
    label $f.value -anchor w -justify left -wraplength 14c

# Side buttons:
#
# DISABLED 5/3/02:  "-command ParamUndo $instance" replaced with
# "-command Undo", likewise for "-command ParamRedo $instance".

    button $w.enter -text Enter -underline 0 -width $bwid \
    		    -command "tksu::ParamEnter $instance"
    button $w.undo -text Undo -underline 0 -width $bwid \
    		   -command tksu::Undo
    button $w.redo -text Redo -underline 0 -width $bwid \
    		   -command tksu::Redo
    button $w.clear -text Default -underline 0 -width $bwid \
		    -command "tksu::ParamClear $instance"
# Pack up objects.

    pack $w.title -side top -fill x

    pack $w.name.label -side left -padx 2
    pack $w.enter -padx 10 -side right -in $w.name
    pack $w.name.clear $w.name.enum $w.name.dup -side right
    pack $w.name.entry -fill x -padx 2 -pady 2
    pack $w.name -side top -fill x

    pack $w.value.label -side left -padx 2
    pack $w.undo -padx 10 -side right -in $w.value
    pack $w.value.right $w.value.left $w.value.enum -side right
    pack $w.value.entry -fill x -padx 2 -pady 2
    pack $w.value -side top -fill x

    pack $w.v2.label $w.v2.thumb -side left -padx 2
    pack $w.redo -padx 10 -side right -in $w.v2
    pack $w.v2.one $w.v2.zero -side right
    pack $w.v2 -side top -fill x

    pack $w.range.label $w.range.value -side left -padx 2
    pack $w.clear -padx 10 -side right -in $w.range
    pack $w.range -side top -fill x

    pack $w.default.label -side left -padx 2
    pack $w.default.value -side left -padx 2 -anchor w -fill x -expand 1
    pack $w.default -side top -fill x

    pack $w.desc.label -side left -padx 2 -anchor nw
    pack $w.desc.value -side left -padx 2 -anchor nw -fill x -expand 1
    pack $w.desc -pady 4 -side top -anchor n -fill x

    pack $w -side bottom -fill x

# Accelerator bindings:

    set f .param$instance
    bind $f <Alt-i> "$w.name.enum invoke"
    bind $f <Alt-l> "$w.value.enum invoke"
    bind $f <Alt-comma> "$w.value.left invoke"
    bind $f <Alt-period> "$w.value.right invoke"

# These bindings apply to both param and port dialogs:

    bind $f <Alt-e> "tksu::ParamEnter $instance"
    bind $f <Alt-u> tksu::Undo
    bind $f <Alt-r> tksu::Redo
    bind $f <Alt-d> "tksu::ParamClear $instance"
    return
}

#-------------------------------------------------------------------------------
# LoadParamDialog -- load settings for given parameter into parameter dialog.
#
# Args:     instance	Instance number of parameter list window.
#	    nlist	Parameter name structure, a list in the format
#			{ genericName dataType range default description }.
#			See LoadSpecs for more details.  nlist is usually
#			stored in the Params array.
#	    vlist	Parameter value structure, a list in the format
#			{ port-or-param genericName name value undo redo }.
#			See ParamList for more details.  vlist is usually
#			stored in the Values array.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::LoadParamDialog {instance nlist vlist} {
    variable CurrentModule
    variable CurrentParam
    variable CurrentValue
    variable LongDefault
    variable LongDesc

    set genericName  [lindex $vlist 1]
    set specificName [lindex $vlist 2]
    set paramType    [lindex $nlist 1]
    set paramRange   [lindex $nlist 2]
    set paramDefault [lindex $nlist 3]
    set paramDesc    [lrange $nlist 4 end]
    set modbase [ModBase $CurrentModule($instance)]

# If entry widgets are linked to an EnumList window, unlink them.

    if {$specificName != [lindex $CurrentValue($instance) 2]} {
	UnlinkEntries $instance
    }
    set CurrentParam($instance) $nlist
    set CurrentValue($instance) $vlist
    set w .param$instance.param

# Special case:  if nlist is null, clear all entries.

    if {$nlist == ""} {
	$w.title configure -text "Set parameter"
	$w.name.entry configure -state normal
	$w.name.entry delete 0 end
	$w.name.entry configure -state disabled
	$w.name.dup configure -state disabled
	$w.name.enum configure -state disabled
	$w.name.clear configure -state disabled

	$w.value.entry delete 0 end
    	$w.value.enum configure -state disabled
    	$w.value.left configure -state disabled
    	$w.value.right configure -state disabled

	$w.v2.zero configure -state disabled
	$w.v2.one configure -state disabled

	$w.range.value configure -text ""
	$w.default.value configure -text ""
	$w.desc.value configure -text ""
    	return
    }

    $w.title configure -text "Parameter `$specificName'"
    $w.name.dup configure -state disabled
    $w.name.enum configure -state disabled
    $w.name.clear configure -state disabled

# If parameter can be duplicated, strip the index before loading name into
# entry, and enable the Dup and Clear buttons.

    regsub -- {\(.*\)$} $specificName {} stripped
    if [regexp -- {^dup-} $genericName] {
    	$w.name.dup configure -state normal
    	$w.name.clear configure -state normal
	$w.title configure -text "Parameter `$stripped' (may be duplicated)"
    }
    $w.name.entry configure -state normal
    $w.name.entry delete 0 end
    $w.name.entry insert end $stripped
    $w.name.entry configure -state disabled

# If parameter name is variable, enable controls for editing name.

    if [regexp -- {^enum-(.+)} $genericName dummy enum] {
    	$w.name.entry configure -state normal
    	$w.name.dup configure -state normal
	$w.name.enum configure -state normal
    	$w.name.clear configure -state normal
	$w.title configure -text "Select parameter name from list `$enum'"
    }

# Load parameter value.

    set enum ""
    set listable [regsub -- {-list$} $paramType {} type]
    regexp -- {^enum-(.+)$} $type {} enum

    $w.value.entry configure -state normal
    $w.value.entry delete 0 end
    $w.value.enum configure -state disabled
    $w.value.left configure -state disabled
    $w.value.right configure -state disabled

    DisableThumb $w.v2.thumb
    $w.v2.zero configure -state disabled
    $w.v2.one configure -state disabled

    set value [lindex $vlist 3]
    $w.value.entry insert end $value

    if {$enum != ""} {
	$w.value.enum configure -state normal
    }
    if $listable {
	$w.value.left configure -state normal
	$w.value.right configure -state normal
    }
    if {$type == "int" || $type == "float"} {
	if {$value != ""} {
	    set thumbValue $value
	} else {
	    set thumbValue $paramDefault
	}
	InitThumb $w.v2.thumb $paramRange $thumbValue $type
	$w.v2.zero configure -state normal
	$w.v2.one configure -state normal
    }

# Parameter range description.

    if {$enum != ""} {
    	set rangeDesc "select a value from list `$enum'"

    } else {
    	switch -- $type {
	    int     {set rangeDesc "enter an integer"}
	    float   {set rangeDesc "enter a floating value"}
	    string  {set rangeDesc "enter a string"}
	    default {set rangeDesc "enter a value"}
	}
	if [regexp {(.+)<=(.+)} $paramRange dummy lo hi] {
	    append rangeDesc " from $lo to $hi"

	} elseif [regexp {<=(.+)} $paramRange dummy hi] {
	    append rangeDesc " less than or equal to $hi"

	} elseif [regexp {(.+)<=} $paramRange dummy lo] {
	    append rangeDesc " greater than or equal to $lo"

	} elseif [regexp {(.+)<(.+)} $paramRange dummy lo hi] {
	    append rangeDesc " between $lo and $hi (exclusive)"

	} elseif [regexp {<(.+)} $paramRange dummy hi] {
	    append rangeDesc " less than $hi"

	} elseif [regexp {(.+)<} $paramRange dummy lo] {
	    append rangeDesc " greater than $lo"

	} else {
	    append rangeDesc " over any range"
	}
    }
    $w.range.value configure -text $rangeDesc

# Default description.

    if [info exists LongDefault($modbase,$genericName)] {
	set desc $LongDefault($modbase,$genericName)
    } else {
	set desc [DefaultValue long $paramDefault]
    }
    set wraplength [expr [winfo width $w.default.value] - 40]
    if {$wraplength < 100} {
    	set wraplength 12c
    }
    $w.default.value configure -text $desc -wraplength $wraplength

# Parameter description.

    if [info exists LongDesc($modbase,$genericName)] {
	set desc $LongDesc($modbase,$genericName)
    } else {
	set desc $paramDesc
    }
    set wraplength [expr [winfo width $w.desc.value] - 40]
    if {$wraplength < 100} {
    	set wraplength 12c
    }
    $w.desc.value configure -text $desc -wraplength $wraplength

# Set focus to value entry.

    set f [focus]
    if {$f != ".param$instance.list" && $f != ".param$instance.scroll"} {
    	focus $w.value.entry
    }
    return
}

#-------------------------------------------------------------------------------
# LoadEntry -- load entry widget with given value.
#
# Args:     entry	Entry widget name.
#	    newValue	Value string to be loaded.
# Returns:  null
#
# Use this procedure to load the entry widget if a comma-delimited list of
# values is to be supported.  The new value replaces only the value that
# overlies the current insertion point.
#-------------------------------------------------------------------------------

proc tksu::LoadEntry {entry newValue} {

    set valueList [split [$entry get] ,]
    set ipoint [$entry index insert]

# Find index i of value in list that the insertion point (ipoint) lies in.

    set value {}
    set icomma 0
    set i 0
    foreach value $valueList {
    	incr icomma [string length $value]
	if {$ipoint <= $icomma} break
	incr icomma
	incr i
    }

# Replace ith value with newValue.

    if {$i < [llength $valueList]} {
    	set valueList [lreplace $valueList $i $i $newValue]
    } else {
    	lappend valueList newValue
    }

# New insertion point is at end of newValue.

    set ibeg [expr $icomma - [string length $value]]
    set iend [expr $ibeg + [string length $newValue]]

# Load entry widget with new list, set insertion point at iend.
# Optionally, set selection to (ibeg,iend).

    $entry delete $ibeg $icomma
    $entry insert $ibeg $newValue
    $entry icursor $iend
#   $entry selection range $ibeg $iend
    return
}

#-------------------------------------------------------------------------------
# ShiftEntry -- shift insertion point left or right by whole fields.
#
# Args:     entry	Entry widget name.
#	    direction	Direction and amount to shift insertion point by:
#			1 to move right, -1 to move left.
# Returns:  null
#
# For those entry widgets that support a comma-delimited list of values,
# use this procedure to position the insertion point over another value.
#-------------------------------------------------------------------------------

proc tksu::ShiftEntry {entry direction} {

    set valueList [split [$entry get] ,]
    set ipoint [$entry index insert]

# Find index i of value in list that the insertion point (ipoint) lies in.

    set value {}
    set icomma 0
    set i 0
    foreach value $valueList {
    	incr icomma [string length $value]
	if {$ipoint <= $icomma} break
	incr icomma
	incr i
    }

# Advance index i.  Create new empty elements at either end of list, if
# i goes out of range.

    incr i $direction
    while {$i < 0} {
    	set valueList [linsert $valueList 0 {}]
	incr i
    }
    while {$i >= [llength $valueList]} {
    	lappend valueList {}
    }

# Now automatically trim empty elements off each end of the list, up
# to but not including index i.

    set nlo 0
    while {$nlo < $i} {
    	if {[lindex $valueList $nlo] != ""} break
	incr nlo
    }
    set nhi [expr [llength $valueList] - 1]
    while {$nhi > $i} {
    	if {[lindex $valueList $nhi] != ""} break
	incr nhi -1
    }
    set valueList [lrange $valueList $nlo $nhi]
    incr i -$nlo

# Compute new insertion point.

    set ibeg 0
    foreach value $valueList {
    	set iend [expr $ibeg + [string length $value]]
	incr i -1
	if {$i < 0} break
	set ibeg [expr $iend + 1]
    }

# Load entry widget with new list, set insertion point at iend and
# set selection to (ibeg,iend).

    $entry delete 0 end
    $entry insert 0 [join $valueList ,]
    $entry icursor $iend
    $entry selection range $ibeg $iend
    return
}

#-------------------------------------------------------------------------------
# ParamEnum -- handler for button to raise the enumeration list window.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# The `list' button next to the value entry widget in the parameter dialog
# is a toggle.  If relief is raised, then it will be toggled to sunken relief
# and an EnumList window will be raised.  If relief is sunken, the EnumList
# window associated with the entry is hidden.
#-------------------------------------------------------------------------------

proc tksu::ParamEnum instance {
    variable CurrentModule
    variable CurrentParam
    variable Color

    set entry .param$instance.param.value.entry
    set button .param$instance.param.value.enum
    set callback "$button configure -relief raised -background $Color(normal)"

# Disconnect from list:

    if {[$button cget -relief] == "sunken"} {
	set i [GetEnumInstance $entry]
	if {$i != ""} {
	    ResetEnumEntry $i
	}
    	eval $callback
	return
    }

# Bring list up:

    set module $CurrentModule($instance)
    set nlist $CurrentParam($instance)
    set name [lindex $nlist 0]
    set enum [lindex $nlist 1]
    regsub -- {-list$} $enum {} enum

    if {$module == "" || $name == "" || ![regexp {^enum-} $enum]} return

    set status "Select parameter `$name' in module `$module'"
    set i [EnumList $enum $entry $status $callback]
    if {$i != ""} {
    	$button configure -relief sunken -background $Color(trough)
    }
    return
}

#-------------------------------------------------------------------------------
# UnlinkEntries -- disassociate `list' buttons from enumeration windows.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# When the `list' button in the parameter dialog is toggled down (see
# ParamEnum), the neighboring entry widget becomes associated with a new
# enumeration list window.  If a selection is made in the enumeration, the
# entry widget is loaded with the selected value.  This procedure removes
# the association of the enumeration list with the entry, while letting the
# enumeration window stay up on the screen.
#-------------------------------------------------------------------------------

proc tksu::UnlinkEntries instance {

    set entries {}
    set w .param$instance.param
    lappend entries $w.name.entry $w.value.entry

    foreach entry $entries {
    	set i [GetEnumInstance $entry]
	if {$i != ""} {
	    ResetEnumEntry $i
	}
    }
    return
}

#-------------------------------------------------------------------------------
# ParamNameEnum -- parameter name `list' button handler.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# Works like ParamEnum, but for the name entry widget and list button.
#-------------------------------------------------------------------------------

proc tksu::ParamNameEnum instance {
    variable CurrentModule
    variable CurrentValue
    variable Color

    set entry .param$instance.param.name.entry
    set button .param$instance.param.name.enum
    set callback "$button configure -relief raised -background $Color(normal)"

# Disconnect from list:

    if {[$button cget -relief] == "sunken"} {
	set i [GetEnumInstance $entry]
	if {$i != ""} {
	    ResetEnumEntry $i
	}
    	eval $callback
	return
    }

# Bring list up:

    set module $CurrentModule($instance)
    set vlist $CurrentValue($instance)
    set enum [lindex $vlist 1]

    if {$module == "" || $enum == "enum-" || ![regexp {^enum-} $enum]} {
    	return
    }
    set status "Choose new parameter name in module `$module'"
    set i [EnumList $enum $entry $status $callback]
    if {$i != ""} {
    	$button configure -relief sunken -background $Color(trough)
    }
    return
}

#-------------------------------------------------------------------------------
# ParamNameClear -- handler for parameter name `clear' button.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# The current duplicate or variable-name parameter is removed from the
# parameter list, unless it is the only one left of the set.  The selection
# is changed to the previous duplicate parameter of the set.  The indices
# of the remaining parameters of the set are preserved.
#-------------------------------------------------------------------------------

proc tksu::ParamNameClear instance {
    variable Values
    variable CurrentValue
    variable CurrentModule

# The only parameter that may be removed is one whose name is indexed.

    set w .param$instance
    set genericName [lindex $CurrentValue($instance) 1]
    set specificName [lindex $CurrentValue($instance) 2]
    if {![regexp -- {\(.+\)$} $specificName]} return

# Append indices of all entries with the same generic name to igen.
# If there is only one entry, it cannot be deleted.  Set ispec to the
# specific entry index.

    set module $CurrentModule($instance)
    set i 0
    set igen ""
    foreach vlist $Values($module) {
    	if {[lindex $vlist 1] == $genericName} {
	    lappend igen $i
	    if {[lindex $vlist 2] == $specificName} {
	    	set ispec $i
	    }
	}
	incr i
    }
    if {[llength $igen] <= 1} return

# Delete the specific entry from Values.

    UndoName $module $ispec delete
    AddMark
    set Values($module) [lreplace $Values($module) $ispec $ispec]

# Reload parameter list, select the previous entry and load the dialog.

    set sellist [SaveSelection $w.list]
    LoadParamList $instance $module
    RestoreSelection $w.list $sellist
    if {$ispec > 0} {
    	incr ispec -1
    }
    SetSelection $w.list $ispec
    ParamSelected $instance
    return
}

#-------------------------------------------------------------------------------
# ParamNameDup -- duplicate parameter name and value.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# Create a new duplicate parameter, write it to Values array and update
# the parameter list to show the new variable.  Selection advances to the
# new entry.
#-------------------------------------------------------------------------------

proc tksu::ParamNameDup instance {
    variable Values
    variable CurrentValue
    variable CurrentParam
    variable CurrentModule

# CurrentValue must have an indexed parameter name in order to be
# duplicated.

    set w .param$instance
    set genericName [lindex $CurrentValue($instance) 1]
    set name [lindex $CurrentValue($instance) 2]
    if {![regexp -- {\(.+\)$} $name]} return

# Find entry index (ispec) of CurrentValue in Values.
# Find largest name index among all entries with generic name.

    set module $CurrentModule($instance)
    set i 0
    set index 0
    foreach vlist $Values($module) {
    	if {$genericName == [lindex $vlist 1]} {
	    set name1 [lindex $vlist 2]
	    if {$name == $name1} {
		set ispec $i
	    }
	    if [regexp -- {\((.+)\)$} $name1 dummy index1] {
	    	if {$index < $index1} {
		    set index $index1
		}
	    }
	}
	incr i
    }

# Get name and value from entries.

    set nameValue [GetParamValue $instance]
    set name [lindex $nameValue 0]
    set value [lindex $nameValue 1]
    if {$name == ""} return

# Apply new index to name.

    if {$index == 0} return
    incr index
    if {![regsub -- {\(.+\)$} $name ($index) name]} return

# Create new vlist entry.  Verify name and value.

    set vlist [list param $genericName $name $value {} {}]
    if {![VerifyValue $w $CurrentParam($instance) $vlist]} {
	return
    }

# Insert new vlist into Values.

    incr ispec
    set Values($module) [linsert $Values($module) $ispec $vlist]
    UndoName $module $ispec add
    AddMark

# Reload parameter list.  Set CurrentValue to new entry.

    set sellist [SaveSelection $w.list]
    LoadParamList $instance $module
    RestoreSelection $w.list $sellist
    SetSelection $w.list $ispec
    ParamSelected $instance
    return
}

#-------------------------------------------------------------------------------
# GetParamValue -- get name/value from entry widgets in parameter dialog.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  value	If current parameter name is regular, the contents of
#			the value entry widget is returned.  If current
#			parameter name is indexed, a {name(index) value} list
#			is returned, reflecting the contents of the name and
#			value entry widgets.
#-------------------------------------------------------------------------------

proc tksu::GetParamValue instance {
    variable CurrentValue

    set w .param$instance.param
    set genericName [lindex $CurrentValue($instance) 1]
    set name [lindex $CurrentValue($instance) 2]

    set value [string trim [$w.value.entry get]]

    if [regexp -- {\(.+\)$} $name suffix] {
	set name [string trim [$w.name.entry get]]
	set value [list $name$suffix $value]
    }
    return $value
}

#-------------------------------------------------------------------------------
# LoadParamValue -- load entry widgets in parameter dialog with given value.
#
# Args:     instance	Instance number of parameter list window.
#	    value	If current parameter is regular, the value entry
#			widget is loaded with this value.  If current
#			parameter name is indexed, the argument is a
#			{name value} pair, which is loaded into the name and
#			value entry widgets.  The index is stripped off the
#			name before loading.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::LoadParamValue {instance value} {
    variable CurrentValue

    set w .param$instance.param
    set genericName [lindex $CurrentValue($instance) 1]
    set name [lindex $CurrentValue($instance) 2]

    if [regexp -- {\(.+\)$} $name suffix] {
	regsub -- {\(.+\)$} [lindex $value 0] {} name
    	$w.name.entry delete 0 end
	$w.name.entry insert 0 $name
	set value [lindex $value 1]
    }
    $w.value.entry delete 0 end
    $w.value.entry insert 0 $value
    return
}
