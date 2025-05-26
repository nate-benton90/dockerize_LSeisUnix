# ParamList.tcl --
#
#	Tksu procedures to handle the parameter list window.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: ParamList.tcl,v 1.4 2002/07/10 19:52:38 jeff Exp $

#-------------------------------------------------------------------------------
# ParamList -- display parameter list window.
#
# Args:     module	Display parameters for this module.
#	    raise	If 1, raise and deiconify the window.  Otherwise, do
#			not raise or deiconify.  The argument is optional --
#			default value is 1.
# Returns:  instance	Instance number of window (1 or greater).
#
# The single argument defines the module to display parameters for.  If the
# module name ends in `-n', where n is an integer, the parameters refer to
# the nth instance of the module in the canvas and are unique to that
# instance.  The value list accessed is Values($module).  If the module
# name does not have an instance number appended, the "preset" value list is
# accessed, which contains initial parameter settings to be applied to a new
# instance of the module when created.
#
# Multiple copies of the top level parameter window may be displayed at
# once.  The copies are named .param1, .param2, etc.  The corresponding
# variables KeepParam(1), KeepParam(2), etc. are bound to the `Keep'
# checkbuttons.  The KeepParam variable, if true, specifies that the window
# is to retain its contents; if false, the window's contents are replaced
# when a new module is selected.
#
# Value list format:
#
# A value list defines parameter settings for a module.  Each entry in the
# list defines the setting for one parameter.  An entry must exist for each
# line of the parameter list.  A value entry is a list having the format
#
# { <port-or-param> <generic-name> <specific-name> <value> <undo> <redo> }
#
# where the elements are:
#
#   <port-or-param>	`port' indicates a port parameter,
#			`param' indicates a regular parameter.
#
#   <generic-name>	May be one of the following forms:
#			<name>		a regular parameter or port name,
#			dup-<name>	a parameter that may be duplicated,
#			enum-<name>	a parameter whose name may be one
#					of an enumeration.
#			The generic name is the name used to look up the
#			parameter in the Params(module) list.
#
#   <specific-name>	The actual name of the parameter.  For the `dup'
#			and `enum' generic name types, the specific name
#			has an index number appended to it, in the form
#			<name>(<index>), for example `ref(3)'.  The index
#			number is removed from the parameter when it is
#			added to the command line in the flowsheet script.
#
#   <value>		Current value for the parameter.  If `{}', no
#			value is set -- the default value (if any) will be
#			used.
#
#   <undo>		The undo history list, containing past values of the
#			dialog entry window.  For a variable-name parameter,
#			the {name value} pair is stored in the list.  For a
#			port parameter, the stored value has a special format.
#			NOTE:  currently unused.
#
#   <redo>		The redo history list, containing past values of the
#			dialog entry window put there by `undo'.
#			NOTE:  currently unused.
#
# A regular parameter has a unique name and appears once in the parameter
# list.  Its generic name equals its specific name, being the actual name
# that is to be used on the command line.  Port names are always regular
# in this sense.
#
# A parameter with a generic name starting with `dup-' is one that can
# appear more than once on the command line.  The specific name is the actual
# parameter name with an index in parens appended to it.  The user may add
# any number of duplicated parameters to the parameter list with the Dup
# button.  Each duplicated parameter has a unique index number assigned to
# its name.  The index number is dropped when the parameter name and value
# are written to the command line in the flowsheet script.
#
# A parameter with a generic name starting with `enum-' acts like a
# duplicated parameter, except that its name may be chosen from any listed
# in the enumeration defined by the generic name.  An arbitrary number of
# variable-name parameters may be added to the parameter list by the user
# with the Dup button.  Each newly added parameter has a unique index
# number assigned to its specific name, even when the selected names are
# all chosen to be different.
#
# When a new module is displayed, any missing members in the value list are
# initialized, so that there is a member for each line in the listbox.
# Ports are always listed first in the value list.
#
# The value lists in Values are updated when the user hits `Enter' from
# the Param or Port dialog subwindow.
#-------------------------------------------------------------------------------

proc tksu::ParamList {module args} {
    variable Params
    variable Ports
    variable Values
    variable KeepParam
    variable Color
    variable CurrentModule
    variable CurrentParam
    variable CurrentValue
    variable Geometry

# Initialize the Values($module) list.  For each line in the listbox, a
# corresponding element exists in Values($module), in the same order.

    if {$module == ""} {return {}}
    set modbase [ModBase $module]
    if {![info exists Values($module)]} {
	InitValues $module
    }

# Find an existing window that is willing to display.  If a window is
# already displaying the module, use it.

    set instance 0
    set jmax 0
    foreach w [winfo children .] {

    	if {[scan $w ".param%d" j] != 1} continue
	if {$jmax < $j} {set jmax $j}

	if {$module == $CurrentModule($j)} {
	    # Found exact match
	    set instance $j
	    break
	}
	if {$KeepParam($j) == 0} {
	    # Found available
	    set instance $j
	}
    }
    set w .param$instance

# If no window exists that is willing to display, create a new one.

    if {$instance == 0} {
	set instance [expr $jmax + 1]
	set KeepParam($instance) 0
	set CurrentModule($instance) {}
	set CurrentParam($instance) {}
	set CurrentValue($instance) {}

	set w .param$instance
	toplevel $w
	wm protocol $w WM_DELETE_WINDOW "tksu::HideParamList $instance"

# Button bar at top:

	set f $w.buttons
	frame $f
	checkbutton $f.keep -text Keep -underline 0 \
			    -variable tksu::KeepParam($instance)
	label $f.label -pady 2m
	button $f.help -text Help -underline 0 -width 4 \
		       -command "tksu::ParamHelp $instance"
	button $f.close -text Close -underline 0 -width 4 \
			-command "tksu::HideParamList $instance"
	pack $f.keep -side left
	pack $f.close $f.help -side right
	pack $f.label -fill x
	pack $f -side top -fill x

# Parameter listbox and scroll bar:

	set f $w.listScroll
	frame $f
	listbox $w.list -bg white -font FixFont -width 80 -height 5 \
			-exportselection 0 -selectbackground $Color(selected) \
			-yscrollcommand "$w.scroll set"
	scrollbar $w.scroll -command "$w.list yview"
	pack $w.scroll -side right -fill y -in $f
	pack $w.list -fill both -expand 1 -in $f

# Parameter and port dialogs:

	CreateParamDialog $instance
	CreatePortDialog $instance
	pack $f -fill both -expand 1

# Listbox bindings:  make sure tag is unique for this window.

	set tag param${instance}Tag
	set taglist [bindtags $w.list]
	lappend taglist $tag
	bindtags $w.list $taglist

	bind $tag <ButtonRelease-1>  "tksu::ParamSelected $instance"
	bind $tag <KeyRelease-Up>    "tksu::ParamSelected $instance"
	bind $tag <KeyRelease-Down>  "tksu::ParamSelected $instance"
	bind $tag <KeyRelease-space> "tksu::ParamSelected $instance"

# Accelerator bindings:

	bind $w <Alt-k>		    "$w.buttons.keep invoke"
	bind $w <Alt-h>		    "tksu::ParamHelp $instance"
	bind $w <Alt-c>		    "tksu::HideParamList $instance"
	bind $w <Escape>	    "tksu::HideParamList $instance"
	bind $w <KeyRelease-Up>     "tksu::MoveSelection $w.list -1; \
				     tksu::ParamSelected $instance"
	bind $w <KeyRelease-Down>   "tksu::MoveSelection $w.list 1; \
				     tksu::ParamSelected $instance"
	bind $w <KeyRelease-Prior>  "tksu::MoveSelection $w.list -10; \
				     tksu::ParamSelected $instance"
	bind $w <KeyRelease-Next>   "tksu::MoveSelection $w.list 10; \
				     tksu::ParamSelected $instance"
	bind $w <KeyRelease-Return> "tksu::ParamEnter $instance"

# Set up forward and reverse traversal.

	set tabList ""
	lappend tabList $w.buttons.keep $w.buttons.help $w.buttons.close
	lappend tabList $w.list $w.scroll

	set f $w.param
	lappend tabList $f.name.entry $f.name.dup $f.name.enum $f.name.clear
	lappend tabList $f.value.entry $f.value.enum
	lappend tabList $f.value.left $f.value.right
	lappend tabList $f.enter $f.undo $f.redo $f.clear

	set f $w.port
	lappend tabList $f.pipe.button $f.pipe.moduleB $f.pipe.portB
	lappend tabList $f.file.button $f.file.entry
	lappend tabList $f.temp.button $f.fileDialog
	lappend tabList $f.enter $f.undo $f.redo $f.clear

#	bind $w <Tab> "tksu::TabForward [list $tabList]"
	bind $w <ISO_Left_Tab> "tksu::TabReverse [list $tabList]"

# Initialize some color variables.

	set Color(normal) [$w.scroll cget -background]
	set Color(trough) [$w.scroll cget -troughcolor]
    }
    set raise [lindex $args 0]
    if {$raise != "0"} {
	if [info exists Geometry($w)] {
	    $w.list configure -height 1
	}
    	RaiseWindow $w
    }

# Save previous dialog contents (if any) in history.
#
#   if {$CurrentModule($instance) != ""} {
#	PushDialog $instance
#	SaveCurrentValue $instance
#   }

# Set module titles.

    set title "Module `$module'"
    if {$module == $modbase} {lappend title preset}
    lappend title parameters
    $w.buttons.label configure -text $title

    wm title $w "Parameters for $module"
    set oldModule $CurrentModule($instance)
    set CurrentModule($instance) $module

# Fill the listbox.  Each line of the listbox describes one parameter:
# name, value (if any), and one-line description.

    LoadParamList $instance $module

# Reset and display parameter dialog.

    if {$module != $oldModule} {
	UnlinkEntries $instance
	LoadParamDialog $instance {} {}
	SwitchDialog $instance param
    }
    return $instance
}

#-------------------------------------------------------------------------------
# ParamListParam -- like ParamList, but also with given parameter selected.
#
# Args:     module	Module to be shown.
#	    index	Index of parameter/port in list to show.
#	    raise	If 1, raise and deiconify the window.
#			The argument is optional -- default value is 1.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ParamListParam {module index args} {

    set instance [ParamList $module $args]
    set listbox .param$instance.list
    SetSelection $listbox $index
    ParamSelected $instance
    return
}

#-------------------------------------------------------------------------------
# LoadParamList -- load parameter listbox.
#
# Args:     instance	Instance of this parameter window.
#	    module	The parameters for this module are displayed.  The
#			module name usually has an instance number `-n'
#			appended to it.  If not, the preset (shared) module
#			parameters are displayed.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::LoadParamList {instance module} {
    variable Values

    set w .param$instance.list
    $w delete 0 end
    set modbase [ModBase $module]

    foreach vlist $Values($module) {

	set class [lindex $vlist 0]
	set genericName [lindex $vlist 1]
	set name [lindex $vlist 2]
	set value [lindex $vlist 3]
	set nlist [ParamName $modbase $genericName]
	set desc [join [lrange $nlist 4 end]]

	if {$class == "port"} {
	    set value [FormatPortValue [lindex $nlist 2] $value]
	}
	if {$value == ""} {
	    # Use default value
	    set value [DefaultValue short [lindex $nlist 3]]
	}
	$w insert end [FormatParamList $genericName $name $value $desc]
    }
    return
}

#-------------------------------------------------------------------------------
# FormatParamList -- format parameter list line.
#
# Args:     genericName	Generic parameter name.
#	    name	Specific parameter name.
#	    value	Parameter value, already formatted.
#	    desc	Parameter description string.
# Returns:  string	Formatted string to be shown in listbox.
#
# Defaulted parameter values have angle brackets surrounding them.  The other
# parameters have been manually set.  Flag them with a symbol in the first
# column so that they stand out from the defaults a bit better.
#-------------------------------------------------------------------------------

proc tksu::FormatParamList {genericName name value desc} {

# Strip the index number from the parameter name (if present).

    if [regexp -- {(.+)\(.+\)} $name dummy stripped] {
    	set nameValue "$stripped=$value"
    } else {
	set nameValue "$name=$value"
    }
    set char [string index $value 0]
    if {$char != "<" && $char != ""} {
    	set pointer {[X]}
    } else {
    	set pointer {   }
    }
    set n [string length $nameValue]
    set n [expr (($n/8) + 1)*8]
    if {$n < 24} {set n 24}

    return [format "%s %-${n}s%s" $pointer $nameValue $desc]
}

#-------------------------------------------------------------------------------
# ParamName -- return parameter name list.
#
# Args:     module	Module name, may be an instance name.
#	    name	Name to look up in Ports or Params.
# Returns:  nlist	Entry in Ports(module) or Params(module), or {}.
#
# The return value is the list {name type range-or-rw default desc}.
# If not found, null is returned.
#-------------------------------------------------------------------------------

proc tksu::ParamName {module name} {
    variable Ports
    variable Params

    set modbase [ModBase $module]

# Search Ports.

    if [info exists Ports($modbase)] {
    	foreach nlist $Ports($modbase) {
	    if {[lindex $nlist 0] == $name} {
	    	return $nlist
	    }
	}
    }

# Search Params.

    if [info exists Params($modbase)] {
    	foreach nlist $Params($modbase) {
	    if {[lindex $nlist 0] == $name} {
	    	return $nlist
	    }
	}
    }
    return
}

#-------------------------------------------------------------------------------
# NewValue -- return newly initialized parameter value object.
#
# Args:     class	Parameter class: `param' or `port'.
#	    nlist	Name list for this parameter or port.
# Returns:  vlist	Initialized value object.
#
# See ParamList for a description of the value (vlist) object.
#-------------------------------------------------------------------------------

proc tksu::NewValue {class nlist} {
    variable Enum

    set name [lindex $nlist 0]
    if {$class == "port"} {
    	if {$name == "stdin" || $name == "stdout"} {
	    if {[lindex $nlist 3] == "req"} {
	    	set value pipe
	    } else {
	    	set value temp
	    }
	} else {
	    set value file
	}
	return "$class $name $name $value {} {}"
    }
    if [regexp -- {^dup-(.*)} $name dummy specificName] {
	append specificName (1)
	return "$class $name $specificName {} {} {}"

    } elseif [regexp -- {^enum-} $name] {
	set specificName [lindex [lindex $Enum($name) 1] 0]
	append specificName (1)
	return "$class $name $specificName {} {} {}"

    } else {
	return "$class $name $name {} {} {}"
    }
}

#-------------------------------------------------------------------------------
# DefaultValue -- return formatted description of default value.
#
# Args:     format	Choice of description: `long' or `short'.
#	    default	Default entry in name object: `req', `-', `pipe' or
#			a string describing default value for parameter.
# Returns:  string	Formatted string for parameter list or dialog.
#-------------------------------------------------------------------------------

proc tksu::DefaultValue {format default} {
    set str ""

# Short format is for parameter list:

    if {$format == "short"} {
    	switch -- $default {
	    "-"     {}
	    "req"   {set str <REQUIRED>}
	    default {set str <$default>}
	}

# Long format is for parameter/port dialog:

    } else {
    	switch -- $default {
	    "-"     {set str "value is optional - ignored if not set"}
	    "req"   {set str "value is REQUIRED - no default"}
	    "pipe"  {set str "pipe to another module"}
	    default {set str $default}
	}
    }
    return $str
}

#-------------------------------------------------------------------------------
# HideParamList -- put away parameter list window.
#
# Args:     instance	Instance number of the window.
# Returns:  null
#
# This procedure overrides the KeepParam setting for the parameter list window.
# Instead of being destroyed, the window is withdrawn for later use.  This
# routine will also withdraw an iconified window.
#-------------------------------------------------------------------------------

proc tksu::HideParamList instance {
    variable KeepParam
    variable CurrentModule
    variable CurrentParam
    variable CurrentValue

    PushDialog $instance
#   SaveCurrentValue $instance
    HideWindow .param$instance

# Reset parameter window state.

    UnlinkEntries $instance
    set KeepParam($instance) 0
    set CurrentModule($instance) {}
    set CurrentParam($instance) {}
    set CurrentValue($instance) {}
    return
}

#-------------------------------------------------------------------------------
# SwitchDialog -- switch to parameter or port dialog.
#
# Args:     instance	Instance number of the window.
#	    choice	Choice for dialog: `param' or `port'.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::SwitchDialog {instance choice} {

    set w .param$instance

    if {$choice == "port"} {
    	pack forget $w.param
	pack $w.port -side bottom -fill x
    } else {
    	pack forget $w.port
	pack $w.param -side bottom -fill x
    }
    return
}

#-------------------------------------------------------------------------------
# ParamSelected -- respond to a new selection in the parameter list.
#
# Args:     instance	Instance number of the parameter list window.
# Returns:  null
#
# This procedure is called when a new parameter has been selected from the
# parameter list.  Bring up the dialog for the new parameter.  Changes to the
# previous parameter are not automatically saved, however the dialog contents
# are stored in its history list so that they can be restored with `Back'.
#-------------------------------------------------------------------------------

proc tksu::ParamSelected instance {
    variable CurrentModule
    variable CurrentValue
    variable Values

    set w .param$instance
    set module $CurrentModule($instance)
    set modbase [ModBase $module]

# Get Values entry from position of selected item in listbox.

    set i [$w.list curselection]
    if {$i == ""} return
    set vlist [lindex $Values($module) $i]

# If vlist names equal CurrentValue's names, selection hasn't changed.

    set genericName [lindex $vlist 1]
    set name [lindex $vlist 2]
#   if {$genericName == [lindex $CurrentValue($instance) 1] \
#   &&  $name        == [lindex $CurrentValue($instance) 2]} {
#	return
#   }
#   PushDialog $instance
#   SaveCurrentValue $instance

# Bring up dialog for new parameter.

    set nlist [ParamName $modbase $genericName]
    set class [lindex $vlist 0]

    if {$class == "port"} {
    	LoadPortDialog $instance $nlist $vlist
    } else {
    	LoadParamDialog $instance $nlist $vlist
    }
    SwitchDialog $instance $class
    return
}

#-------------------------------------------------------------------------------
# InitValues -- initialize the Values list for the given module.
#
# Args:     module	Module to initialize values for.  All previous values
#			are lost.  A value entry is created for each line in
#			the parameter listbox.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::InitValues module {
    variable Ports
    variable Params
    variable Values

    set modbase [ModBase $module]
    set Values($module) {}

# If preset values exist for this module, use them.

    if {$modbase != $module && [info exists Values($modbase)]} {
    	set Values($module) $Values($modbase)
	return
    }

# Scan port entries.

    if [info exists Ports($modbase)] {
	foreach nlist $Ports($modbase) {
	    lappend Values($module) [NewValue port $nlist]
	}
    }

# Scan param entries.

    if [info exists Params($modbase)] {
	foreach nlist $Params($modbase) {
	    lappend Values($module) [NewValue param $nlist]
	}
    }
    return
}

#-------------------------------------------------------------------------------
# GetDialogValue -- return parameter value from current dialog.
#
# Args:     instance	Instance number of the parameter window.
# Returns:  vlist	Returned value object, constructed from the contents
#			of the current parameter or port dialog.  See
#			ParamList for a description of the vlist object.
#			If CurrentValue is null, return null.
#-------------------------------------------------------------------------------

proc tksu::GetDialogValue instance {
    variable CurrentValue

    set class [lindex $CurrentValue($instance) 0]
    if {$class == ""} {
    	return {}
    }
    set genericName  [lindex $CurrentValue($instance) 1]
    set specificName [lindex $CurrentValue($instance) 2]

    switch -- $class {
    	param {
	    set w .param$instance.param
	    set name [string trim [$w.name.entry get]]
	    set value [string trim [$w.value.entry get]]

# If specificName has an index, be sure that it is transferred to the
# potentially new name.

	    if [regexp -- {\(.+\)$} $specificName suffix] {
	    	append name $suffix
	    }
	}
	port {
	    set name [lindex $CurrentValue($instance) 2]
	    set value [GetPortValue $instance]
	}
    }
    set vlist [lreplace $CurrentValue($instance) 2 3 $name $value]
    return $vlist
}

#-------------------------------------------------------------------------------
# SaveCurrentValue -- save CurrentValue to Values list.
#
# Args:     instance	Instance number of the parameter list window.
#	    allowUndo	Optional: if 1, add entries to CanvasUndo to
#			allow the save operation to be undone.  If 0, the
#			CanvasUndo array is unchanged.  Default value is 0.
# Returns:  null
#
# Call this not only when a parameter value has changed, but also when the
# history list in CurrentValue is to be saved.
#-------------------------------------------------------------------------------

proc tksu::SaveCurrentValue {instance {allowUndo 0}} {
    variable CurrentModule
    variable CurrentValue
    variable Values

    set module $CurrentModule($instance)
    set genericName  [lindex $CurrentValue($instance) 1]
    set specificName [lindex $CurrentValue($instance) 2]

# Find index i of CurrentValue in Values.

    set foundIndex 0
    set i 0
    foreach vlist $Values($module) {
    	if {$genericName == [lindex $vlist 1]
	&&  $specificName == [lindex $vlist 2]} {
	    set foundIndex 1
	    break;
	}
	incr i
    }

# If index not found, CurrentValue may be a variable-name parameter whose
# name has changed.  If so, get the index from the selection in the listbox.

    if {!$foundIndex} {
    	set i [lindex [.param$instance.list curselection] 0]
	if {$i != ""} {
	    set vlist [lindex $Values($module) $i]
	    if {$genericName == [lindex $vlist 1]} {
	    	set foundIndex 1
	    }
	}
    }

    if $foundIndex {
	if $allowUndo {
	    UndoValue $module $i $CurrentValue($instance)
	}
	set Values($module) \
	    [lreplace $Values($module) $i $i $CurrentValue($instance)]
    }
    return
}

#-------------------------------------------------------------------------------
# GetVlistByName -- return vlist from Values with given specific name.
#
# Args:     module	Module name (instantiated or not).
#	    name	Specific name to look for.
# Returns:  vlist	Entry in Values(module) with given specific name.
#			If none found, returns null.
#-------------------------------------------------------------------------------

proc tksu::GetVlistByName {module name} {
    variable Values

    if {![info exists Values($module)]} {
    	return {}
    }
    foreach vlist $Values($module) {
    	if {[lindex $vlist 2] == $name} {
	    return $vlist
	}
    }
    return {}
}

#-------------------------------------------------------------------------------
# ParamHelp -- raise help window for module shown in parameter list window.
#
# Args:     instance	Instance number for parameter list window.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ParamHelp instance {
    variable CurrentModule

    Help $CurrentModule($instance)
    return
}

#-------------------------------------------------------------------------------
# ModBase -- return base module name given instance module name.
#
# Args:     module	Instantiated module name in the form `modbase-n'
#			where modbase is base module name and n is an
#			integer greater than 0.
# Returns:  modbase	The base part of the name.
#
# Made this into a procedure because it is too easy to mistype the line!
#-------------------------------------------------------------------------------

proc tksu::ModBase module {

    regsub -- {-[0-9]+$} $module {} modbase
    return $modbase
}
