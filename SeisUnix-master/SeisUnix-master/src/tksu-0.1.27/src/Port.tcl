# Port.tcl --
#
#	Tksu procedures to handle the port dialog window.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Port.tcl,v 1.1.1.1 2002/06/07 23:43:30 jeff Exp $

#-------------------------------------------------------------------------------
# CreatePortDialog -- create port dialog.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# The parameter and port dialogs share the space below the parameter listbox
# in the parameter list window.
#-------------------------------------------------------------------------------

proc tksu::CreatePortDialog instance {
    variable PortAction

# Variable PortAction($instance) is referenced by the radio buttons.
# It may take on the values {pipe file temp}.

    set PortAction($instance) pipe
    set bwid 5
    set lwid 10

    set w .param$instance.port
    frame $w
    label $w.title

# Port action: pipe.

    set f $w.pipe
    frame $f
    label $f.label -text " 1" -underline 1
    radiobutton $f.button -anchor w \
    			  -command "tksu::PortToggle $instance" \
    			  -variable tksu::PortAction($instance) -value pipe
    entry $f.module -relief sunken -bg white -width 14 -exportselection 0
    button $f.moduleB -image DownArrow \
    		      -command "tksu::PipeModules $instance"

    label $f.portlabel -text " port:"
    entry $f.port -relief sunken -bg white -width 10 -exportselection 0
    button $f.portB -image DownArrow \
    		    -command "tksu::PipePorts $instance"

# Port action: file.

    set f $w.file
    frame $f
    label $f.label -text " 2" -underline 1
    radiobutton $f.button -anchor w \
    			  -command "tksu::PortToggle $instance" \
    			  -variable tksu::PortAction($instance) -value file
    entry $f.entry -relief sunken -bg white -exportselection 0

# Port action: temporary or null I/O.

    set f $w.temp
    frame $f
    label $f.label -text " 3" -underline 1
    radiobutton $f.button -anchor w \
    			  -command "tksu::PortToggle $instance" \
    			  -variable tksu::PortAction($instance) -value temp
    button $w.fileDialog -text File -underline 0 -width 3 \
			 -command "tksu::PortFile $instance"

# Port default:

    set f $w.default
    frame $f
    label $f.label -text Default: -width $lwid -anchor w
    label $f.value -anchor w -justify left -wraplength 12c

# Port data type:

    set f $w.data
    frame $f
    label $f.label -text "File Type:" -width $lwid -anchor w
    label $f.value -anchor w
    button $f.color -relief sunken -foreground white -width $bwid \
    		    -takefocus 0

# Port description:

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

# Pack up all objects.

    pack $w.title -side top -fill x

    pack $w.pipe.label $w.pipe.button -side left
    pack $w.enter -padx 10 -side right -in $w.pipe
    pack $w.pipe.portB $w.pipe.port $w.pipe.portlabel -side right
    pack $w.pipe.moduleB -side right
    pack $w.pipe.module -fill x -pady 2
    pack $w.pipe -side top -fill x

    pack $w.file.label $w.file.button -side left
    pack $w.undo -padx 10 -side right -in $w.file
    pack $w.file.entry -fill x -pady 2
    pack $w.file -side top -fill x

    pack $w.temp.label $w.temp.button -side left
    pack $w.redo -padx 10 -side right -in $w.temp
    pack $w.fileDialog -side right -in $w.temp
    pack $w.temp -side top -fill x

    pack $w.default.label -side left -padx 2
    pack $w.clear -padx 10 -side right -in $w.default
    pack $w.default.value -side left -padx 2 -anchor w -fill x -expand 1
    pack $w.default -side top -fill x

    pack $w.data.label $w.data.value -side left -padx 2
    pack $w.data.color -padx 10 -side right
    pack $w.data -side top -fill x

    pack $w.desc.label -side left -padx 2 -anchor nw
    pack $w.desc.value -side left -padx 2 -anchor nw -fill x -expand 1
    pack $w.desc -pady 4 -side top -anchor n -fill x

    pack $w -side bottom -fill x

# Accelerator bindings:

    set f .param$instance
    bind $f <Alt-KeyPress-1> "$w.pipe.button invoke"
    bind $f <Alt-KeyPress-2> "$w.file.button invoke"
    bind $f <Alt-KeyPress-3> "$w.temp.button invoke"
    bind $f <Alt-f> "$w.fileDialog invoke"

# These bindings apply to both param and port dialogs:

    bind $f <Alt-e> "tksu::ParamEnter $instance"
    bind $f <Alt-u> tksu::Undo
    bind $f <Alt-r> tksu::Redo
    bind $f <Alt-d> "tksu::ParamClear $instance"
    return
}

#-------------------------------------------------------------------------------
# LoadPortDialog -- load settings for given port into parameter dialog.
#
# Args:     instance	Instance number of parameter list window.
#	    nlist	Port name structure, a list in the format
#			{ genericName dataType rw default description }.
#			See LoadSpecs for more details.  nlist is usually
#			stored in the Ports array.
#	    vlist	Port value structure, a list in the format
#			{ port-or-param genericName name value undo redo }.
#			See ParamList for more details.  vlist is usually
#			stored in the Values array.
# Returns:  null
#
# The port value may be in one of the following formats:
#
#   pipe		An unconnected pipe.
#   {pipe id ...}	The port has one or more pipes connected to it.
#			The id's are indexes into the Links array.
#   {pipe module port}	After the user has made changes in the port dialog,
#			and before the value has been verified and saved to
#			Values, this form holds the request to
#			create a link to the given module and port name.
#   file		An unconnected file port.
#   {file filename}	A file port, read or write to filename.
#   temp		A temp port, read DevZero or write DevNull.
#
#-------------------------------------------------------------------------------

proc tksu::LoadPortDialog {instance nlist vlist} {
    variable CurrentModule
    variable CurrentParam
    variable CurrentValue
    variable LongDefault
    variable LongDesc
    variable PortAction
    variable Instances
    variable Color

    set portName    [lindex $nlist 0]
    set portType    [lindex $nlist 1]
    set portDir     [lindex $nlist 2]
    set portDefault [lindex $nlist 3]
    set portDesc    [lrange $nlist 4 end]

# If entry widgets are linked to an EnumList window, unlink them.

    UnlinkEntries $instance
    set CurrentParam($instance) $nlist
    set CurrentValue($instance) $vlist

    set w .param$instance.port

# Set initial focus to pipe module button or file entry.

    set module $CurrentModule($instance)
    set modbase [ModBase $module]
    set f [focus]

    if {$f != ".param$instance.list" && $f != ".param$instance.scroll"} {
	if [info exists Instances($module)] {
	    set focusee $w.pipe.moduleB
	} else {
	    set focusee $w.file.entry
	}
    	focus $focusee
	if {[focus] != $focusee} update
    }
    $w.file.entry configure -state normal
    $w.file.entry delete 0 end

# Set label titles according to dir: `r' for read-only, `w' for write-only,
# or `rw' for file read-write.

    switch -- $portDir {

	"r" {

# Input port:

	    $w.title configure -text "Input port `$portName'"
	    $w.pipe.button configure -text "Pipe from module:" -state normal
	    $w.file.button configure -text "Read from file:"
	    $w.temp.button configure -text "Read zeros"
	}
	"w" {

# Output port:

	    $w.title configure -text "Output port `$portName'"
	    $w.pipe.button configure -text "Pipe to module:" -state normal
	    $w.file.button configure -text "Write to file:"
	    $w.temp.button configure -text "Discard output"
	}
	"rw" {

# Read-write port:

	    $w.title configure -text "Random access port `$portName'"
	    $w.pipe.button configure -text "Pipe to module:" -state disabled
	    $w.file.button configure -text "Read/write file:"
	    $w.temp.button configure -text "Read/write temporary file"
	}
    }

# Load port entry widgets and set radiobuttons.

    set value [lindex $vlist 3]
    LoadPortValue $instance $value

# File type description.

    set desc [EnumDesc enum-file $portType]
    if {[llength $desc] <= 0} {
	set desc "unknown"
    }
    $w.data.value configure -text [format "%s (type `%s')" $desc $portType]
    $w.data.color configure -background $Color($portType) -text $portType \
    			    -activebackground $Color($portType)

# Default description.

    if [info exists LongDefault($modbase,$portName)] {
	set desc $LongDefault($modbase,$portName)
    } else {
	set desc [DefaultValue long $portDefault]
    }
    set wraplength [expr [winfo width $w.default.value] - 40]
    if {$wraplength < 100} {
    	set wraplength 12c
    }
    $w.default.value configure -text $desc -wraplength $wraplength

# Port description.

    if [info exists LongDesc($modbase,$portName)] {
	set desc $LongDesc($modbase,$portName)
    } else {
	set desc $portDesc
    }
    set wraplength [expr [winfo width $w.desc.value] - 40]
    if {$wraplength < 100} {
    	set wraplength 12c
    }
    $w.desc.value configure -text $desc -wraplength $wraplength
    return
}

#-------------------------------------------------------------------------------
# GetPortValue -- retrieve correctly formatted value from port dialog entries.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  value	A port value in one of the following forms:
#			(a) pipe ?module port? (but NOT pipe ?id ...?)
#			(b) file ?filename?
#			(c) temp
#-------------------------------------------------------------------------------

proc tksu::GetPortValue instance {
    variable PortAction

    set w .param$instance.port
    set value $PortAction($instance)

    switch -- $value {
    	
	pipe {
	    set module [string trim [$w.pipe.module get]]
	    set port   [string trim [$w.pipe.port get]]

	    if {$module != "" && $module != "NONE" && $port != ""} {
		set value "pipe $module $port"
	    }
	}
	file {
	    set filename [string trim [$w.file.entry get]]
	    set value "file $filename"
	}
	temp {
	}
    }
    return $value
}

#-------------------------------------------------------------------------------
# FormatPortValue -- return formatted port value.
#
# Args:     rw		I/O direction of port: `r', `w', or `rw'.
#	    value	Port value (see LoadPortDialog for format).
# Returns:  string	A formatted string suitable for the parameter list.
#-------------------------------------------------------------------------------

proc tksu::FormatPortValue {rw value} {
    variable DevNull
    variable DevZero

    set action [lindex $value 0]
    set target [lindex $value 1]

    switch -- $action {
    	pipe {
	    if {$target == "" || $rw == "rw"} {
	    	return <pipe>
	    }

# Pipe value is a list of link ids:

	    switch -- $rw {
	    	r {set direction w}
		w {set direction r}
	    }
	    if {[GetModPort $target $direction] != ""} {
	    	foreach id [lrange $value 1 end] {
		    set modport [join [GetModPort $id $direction] :]
		    if {$id != $target} {append str ,}
		    append str $modport
		}

# Pipe value is {module port}:

	    } else {
		set str [join [lrange $value 1 2] :]
	    }
	    return $str
	}

	file {
	    return $target
	}

	temp {
	    switch -- $rw {
	    	r  {return $DevZero}
		w  {return $DevNull}
		rw {return "temp file"}
	    }
	}
    }
    return
}

#-------------------------------------------------------------------------------
# LoadPortValue -- load given port value to dialog entries.
#
# Args:     instance	Instance number of parameter list window.
#	    value	A port value (see LoadPortDialog for details).
# Returns:  null
#
# Dialog widgets are filled and radiobuttons are set.
#-------------------------------------------------------------------------------

proc tksu::LoadPortValue {instance value} {
    variable CurrentParam
    variable CurrentModule
    variable PortAction

    set w .param$instance.port

# If the focus is in one of the following widgets, change it to the new
# active entry.

    set changeFocus 0
    foreach f "$w.pipe.button $w.pipe.moduleB $w.pipe.portB $w.file.button \
    	       $w.file.entry $w.temp.button $w.fileDialog" {
	if {[focus] == $f} {
	    set changeFocus 1
	}
    }
    set action [lindex $value 0]
    set target [lindex $value 1]
    set rw [lindex $CurrentParam($instance) 2]

# If null, set action from default.

    if {$action == ""} {
	set action [lindex [NewValue port $CurrentParam($instance)] 3]
	set target ""
    }
    set PortAction($instance) $action

    $w.pipe.module configure -state normal
    $w.pipe.module delete 0 end
    $w.pipe.module configure -state disabled
    $w.pipe.moduleB configure -state disabled

    $w.pipe.port configure -state normal
    $w.pipe.port delete 0 end
    $w.pipe.port configure -state disabled
    $w.pipe.portB configure -state disabled

    $w.file.entry configure -state normal
    $w.file.entry delete 0 end
    $w.file.entry configure -state disabled
    $w.fileDialog configure -state disabled

    switch -- $action {
        pipe {

# For preset parameter dialogs, disable the module and port entries.

	    set module $CurrentModule($instance)
	    if {$module == [ModBase $module]} {
		if $changeFocus {focus $w.pipe.button}
		return
	    }

	    switch -- $rw {
	    	r  {set direction w}
		w  {set direction r}
		rw {return}
	    }

# If a link is present, show the module and port of the link.

	    set modport [GetModPort $target $direction]
	    if {$modport != ""} {
	    	set module [lindex $modport 0]
		set port [lindex $modport 1]
	    } else {
	    	set module [lindex $value 1]
		set port [lindex $value 2]
	    }
	    $w.pipe.moduleB configure -state normal
	    $w.pipe.module configure -state normal
	    $w.pipe.module insert 0 $module
	    $w.pipe.module configure -state disabled

	    $w.pipe.portB  configure -state normal
	    $w.pipe.port   configure -state normal
	    $w.pipe.port   insert 0 $port
	    $w.pipe.port   configure -state disabled

	    if $changeFocus {focus $w.pipe.moduleB}
	}

	file {
	    $w.fileDialog configure -state normal
	    $w.file.entry configure -state normal
	    $w.file.entry insert 0 $target
	    if $changeFocus {focus $w.file.entry}
	}

	temp {
	    if $changeFocus {focus $w.temp.button}
	}
    }
    return
}

#-------------------------------------------------------------------------------
# PipeModules -- select module from a modal list of modules.
#
# Args:     instance	Instance number for this dialog.
# Returns:  null
#
# ModalList is called to display current module instances in a listbox below
# the module entry.  When a selection is made and the listbox is withdrawn,
# the module entry holds the new selection (but remains disabled).
#
# After selection, look for an existing link to the selected module.  If
# found, set the port entry to the connecting port.  Otherwise set the
# port entry blank.
#-------------------------------------------------------------------------------

proc tksu::PipeModules instance {
    variable CurrentParam
    variable CurrentValue
    variable Instances

# Provide blank line at top of list for deleting a link.

    lappend modules {}
    eval lappend modules [lsort [array names Instances]]
    set w .param$instance.port.pipe

    ModalList $w.module $modules

# If a link is present to the shown module, show the link's port.

    set rw [lindex $CurrentParam($instance) 2]
    switch -- $rw {
    	r  {set direction w}
	w  {set direction r}
    }
    set module [string trim [$w.module get]]
    set port ""
    set value [lindex $CurrentValue($instance) 3]

    if {[lindex $value 0] == "pipe"} {
	foreach id [lrange $value 1 end] {
	    set modport [GetModPort $id $direction]

	    # Looking for a module name match ...
	    if {$modport == "" || [lindex $modport 0] != $module} continue

	    # Got one ...
	    set port [lindex $modport 1]
	    break
	}
    }

# Load the port name.

    $w.port configure -state normal
    $w.port delete 0 end
    $w.port insert 0 $port
    $w.port configure -state disabled
    return
}

#-------------------------------------------------------------------------------
# PipePorts -- select port from a modal list of ports.
#
# Args:     instance	Instance number for this dialog.
# Returns:  null
#
# ModalList is called to display current ports in a listbox below the port
# entry.  When a selection is made and the listbox is withdrawn, the port
# entry holds the new selection (but remains disabled).
#
# The ports listed are those that belong to the module currently shown in
# the module entry widget, and have the opposite I/O sense of the current
# port in the dialog window.  If no ports apply, the port list contains a
# single blank entry.
#-------------------------------------------------------------------------------

proc tksu::PipePorts instance {
    variable CurrentParam
    variable Ports

    lappend ports {}
    set rw1 [lindex $CurrentParam($instance) 2]
    set w .param$instance.port.pipe
    set module [string trim [$w.module get]]

    if {$module != ""} {
	set modbase [ModBase $module]
	if [info exists Ports($modbase)] {
	    foreach nlist $Ports($modbase) {
		set rw2 [lindex $nlist 2]
		if {$rw2 == "rw" || $rw2 == $rw1} continue
		lappend ports [lindex $nlist 0]
	    }
	}
    }
    ModalList $w.port $ports
    return
}

#-------------------------------------------------------------------------------
# PortToggle -- handler for port dialog radio buttons.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::PortToggle instance {
    variable PortAction
    variable CurrentModule

    set w .param$instance.port
    set action $PortAction($instance)
    set module $CurrentModule($instance)

    set changeFocus 0
    foreach f "$w.pipe.button $w.pipe.moduleB $w.pipe.portB $w.file.button \
    	       $w.file.entry $w.temp.button $w.fileDialog" {
	if {[focus] == $f} {set changeFocus 1}
    }
    $w.pipe.moduleB configure -state disabled
    $w.pipe.portB   configure -state disabled
    $w.file.entry   configure -state disabled
    $w.fileDialog   configure -state disabled

    switch -- $action {
        pipe {
	    if {$module == [ModBase $module]} {
		if $changeFocus {focus $w.pipe.button}
	    } else {
		$w.pipe.moduleB configure -state normal
		$w.pipe.portB   configure -state normal
		if $changeFocus {focus $w.pipe.moduleB}
	    }
	}
	file {
	    $w.fileDialog configure -state normal
	    $w.file.entry configure -state normal
	    if $changeFocus {focus $w.file.entry}
	}
	temp {
	    if $changeFocus {focus $w.temp.button}
	}
    }
    return
}

#-------------------------------------------------------------------------------
# PortFile -- raise dialog for selecting input or output file.
#
# Args:     instance	Instance number of parameter list window.
# Returns:  null
#
# If non-null filename is returned from dialog, it is stored in the file
# entry widget.
#-------------------------------------------------------------------------------

proc tksu::PortFile instance {
    variable CurrentParam
    variable DataDir

    set w .param$instance.port.file.entry
    set rw [lindex $CurrentParam($instance) 2]

    if {$rw == "r"} {
    	set filename [tk_getOpenFile -initialdir $DataDir]
    } else {
    	set filename [tk_getSaveFile -initialdir $DataDir]
    }

    if {$filename != ""} {
	PushDialog $instance
    	$w delete 0 end
	$w insert 0 $filename
	set DataDir [file dirname $filename]
	ParamEnter $instance
    }
    return
}
