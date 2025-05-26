# LoadSpecs.tcl --
#
#	Tksu procedures for initializating module arrays:  Desc, Ports,
#	Params, Enum, Members.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: LoadSpecs.tcl,v 1.9 2002/07/30 20:50:44 jeff Exp $

#-------------------------------------------------------------------------------
# LoadSpecs -- read module specs from a spec file.
#
# Args:     filename	Name of spec file.  See README.spec for a detailed
#			description of the spec file format.
# Returns:  null
#
# The contents of the spec file are parsed and appended to the following
# variable arrays (note that the `module' name varies):
#
#   Desc(module)	Short description of module
#
#   Ports(module)	List of ports used by module:
#			{ {name type rw default description} ... }
#
#   Params(module)	Command line parameters accepted by module:
#			{ {name type range default description} ... }
#
#   Trans(module)	A value given on a `Trans: value' line is stored
#			in Trans(module).  It is interpreted to be a
#			procedure name that does special translation of the
#			command argument list before it is written to the
#			flow script.
#
#   DocCommand(module)	Stores the value given on a `DocCmd:' line.
#			The value should be an executable command that
#			delivers documentation on the module to stdout
#			(or delivers a warning that there is no help for
#			the module).  DocCommand is optional and rarely used;
#			the default action for self-documentation is to run
#			sudoc first, then try running the module without
#			arguments.
#
#   Enum(enum-type)	Enumeration list for given enum-type:
#			{ {Desc: description} {value description} ... }
#			First member is overall description.
#
#   Members(category)	Member modules of a category:
#			{ module ... }
#
#   Shared(section)	Parameters in a section whose name begins with
#			`shared-' are added to Shared(section).  Later, if
#			a line in the format `Shared: shared-<section>' is
#			encountered, the parameters from the shared section
#			are added to those of the module being loaded.
#
#-------------------------------------------------------------------------------

proc tksu::LoadSpecs filename {
    variable Desc
    variable Ports
    variable Params
    variable Trans
    variable DocCommand
    variable Enum
    variable Members
    variable LongDefault
    variable LongDesc
    variable Shared

    set enumSection 0
    set sharedSection 0
    set section undefined
    set fd [open $filename r]

    while {[gets $fd line] >= 0} {

# Skip comment lines and empty lines.

    	if [regexp ^# $line] continue
	if {[llength $line] < 1} continue

# Start of section is defined by `[module]'.
# Enumerations are identified by section name starting with `enum-'.
# The default category for a module is `Miscellaneous'.

	if [regsub {^ *\[(.*)\] *$} "$line" {\1} word] {
	    set section $word
	    set enumSection [regexp ^enum- $word]
	    set sharedSection [regexp ^shared- $word]
	    set category Miscellaneous
	    continue
	}

	set word [lindex "$line" 0]
	set line [lrange "$line" 1 end]
	switch -- $word {

	    Desc: {

# Module description goes in Desc(section).
# Enum description becomes first member in Enum(section) list.

		if $enumSection {
		    lappend Enum($section) [list $word $line]
		    continue
		}
		if $sharedSection continue
		set Desc($section) $line
		continue
	    }

	    Cat: {

# Place module in named category.

		if {$enumSection || $sharedSection} continue
		set category "$line"
		lappend Members($category) $section
		continue
	    }

	    Port: {

# Module port definitions go in Ports(module) list.  Quietly enforce
# restrictions for stdin, stdout ports:  no `rw', and default may be only
# `req' or `-'.

		if {$enumSection || $sharedSection} continue
		set name [lindex $line 0]
		set stddef [lindex $line 3]
		if {$stddef != "req" && $stddef != "-"} {
		    set stddef req
		}
		if {$name == "stdin"} {
		    lappend Ports($section) [lreplace $line 2 3 r $stddef]
		} elseif {$name == "stdout"} {
		    lappend Ports($section) [lreplace $line 2 3 w $stddef]
		} else {
		    lappend Ports($section) $line
		}
		continue
	    }

    	    Param: {

# Module command line parameters go in Params(module) list.

		if $enumSection continue
		set name [lindex $line 0]
		if $sharedSection {
		    lappend Shared($section) $line
		} else {
		    lappend Params($section) $line
		}
		continue
	    }

	    Trans: {

# Parameter translation procedures to be invoked are stored in Trans(module).

		if $enumSection continue
		lappend Trans($section) $line
		continue
	    }

	    DocCmd: {

# Use given command for documenting module in help window.

		if $enumSection continue
		set DocCommand($section) $line
		continue
	    }

	    Shared: {

# Load all lines of shared section into Params(module).

		if $enumSection continue
		set name [lindex $line 0]
		if {![info exists Shared($name)]} continue
		if $sharedSection {
		    eval lappend Shared($section) $Shared($name)
		} else {
		    eval lappend Params($section) $Shared($name)
		}
		continue
	    }

	    LDef: {

# Long default description goes in LongDefault(module,name).

		if {$enumSection} continue
		if {![info exists name]} continue
		eval "lappend LongDefault($section,$name) $line"
		continue
	    }

	    LDesc: {

# Long description goes in LongDesc(module,name).

		if {$enumSection} continue
		if {![info exists name]} continue
		eval "lappend LongDesc($section,$name) $line"
		continue
	    }

	    default {

# Enumeration section.

		if {$enumSection} {
		    lappend Enum($section) [list $word $line]
		    continue
		}

# Anything else:  issue a warning.

		puts "Tksu detected bad specification:"
		puts "    File:     $filename"
		puts "    Section:  \[$section\]"
		puts "    Line:     $word $line"
	    }
	}
    }
    close $fd
    return
}

#-------------------------------------------------------------------------------
# SaveLayout -- save preferences and window geometry to file.
#
# Args:     filename	Layout file to write commands to.
# Returns:  null
#
# The layout file consists of tcl script to restore geometry information into
# the Geometry array, and to restore other preferences.
#-------------------------------------------------------------------------------

proc tksu::SaveLayout filename {
    variable FlowDir
    variable DataDir
    variable TempDir
    variable UndoLimit
    variable ShowLogRun
    variable SaveLogFile
    variable SpecPath
    variable Color
    variable Geometry
    variable CurrentCat

    set format {%dx%d+%d+%d}
    catch {
    	set fid [open $filename w]

	puts $fid "set tksu::FlowDir \"$FlowDir\""
	puts $fid "set tksu::DataDir \"$DataDir\""
	puts $fid "set tksu::TempDir \"$TempDir\""
	puts $fid "set tksu::UndoLimit $UndoLimit"
	puts $fid "set tksu::ShowLogRun $ShowLogRun"
	puts $fid "set tksu::SaveLogFile $SaveLogFile"
#	puts $fid "set tksu::SpecPath \"$SpecPath\""

	foreach cname [array names Color] {
	    puts $fid "set tksu::Color($cname) $Color($cname)"
	}

# Window geometries.

	foreach w ". [winfo children .]" {
	    if {$w != "." && $w != ".modlist" \
	    &&  ![regexp {^\.param} $w] \
	    &&  ![regexp {^\.enum} $w] \
	    &&  ![regexp {^\.help} $w]} continue

# Save geometry of toplevel windows.  Adjust up and left to frame corner.

	    set innerGeom [Geometry $w]
	    set frameGeom [FrameGeometry $w]
	    if {[scan $innerGeom $format wi hi xi yi] != 4} continue
	    if {[scan $frameGeom $format wf hf xf yf] != 4} continue
	    set Geometry($w) ${wi}x${hi}+${xf}+${yf}
	}

# Write geometry array to layout file.  It may include entries for windows
# not instantiated in this session.

	foreach w [array names Geometry] {
	    puts $fid "set tksu::Geometry($w) $Geometry($w)"
	}

# If modlist window is currently up, arrange for it to be raised on restart.

	if {[winfo exists .modlist] \
	&&  [wm state .modlist] == "normal" \
	&&  $CurrentCat != ""} {
	    puts $fid "set tksu::CurrentCat \"$CurrentCat\""
	}
	close $fid
    }
    return
}

#-------------------------------------------------------------------------------
# RestoreLayout -- read tcl commands from given initialization file.
#
# Args:     filename	File to read from.
# Returns:  null
#
# The layout file consists of tcl script to restore geometry information into
# the variable Geometry array.
#-------------------------------------------------------------------------------

proc tksu::RestoreLayout filename {

    catch {
    	set fid [open $filename r]
	while {[gets $fid line] >= 0} {
	    eval $line
	}
	close $fid
    }
    return
}
