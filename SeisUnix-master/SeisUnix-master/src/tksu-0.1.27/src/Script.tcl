# Script.tcl --
#
#	Tksu procedures to write runnable shell scripts.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Script.tcl,v 1.10 2002/10/14 04:55:03 jeff Exp $

#-------------------------------------------------------------------------------
# WriteScriptHeader -- write first part of script common to all module groups.
#
# Args:     fid		File id of open script file.
#	    filename	Name of script file.
# Returns:  success	Returns 1 on success, 0 on write failure.
#
# Follow this with a call to WriteScript for each module group in order
# to create the Bourne shell script file.
#-------------------------------------------------------------------------------

proc tksu::WriteScriptHeader {fid filename} {
    variable TempDir

    if [catch {
	puts $fid "#!/bin/sh"
	puts $fid "#----------------------------------------------------------"
	puts -nonewline $fid {echo 1>&2 }
	puts $fid "Tksu flowsheet $filename"
	puts -nonewline $fid {echo 1>&2 }
	puts $fid "Created [exec date]"
	puts $fid {echo 1>&2 Started `date`}
	puts $fid {echo 1>&2 Process ID = $$}
	puts $fid "#----------------------------------------------------------"
	puts $fid ""
	puts $fid {trap abort SIGTERM SIGINT SIGHUP SIGQUIT}
	puts $fid ""
	puts $fid {pid=$$}
	puts $fid "mkdir -p $TempDir"
	puts $fid "jobpid=$TempDir/tksu-\$pid-run"
	puts $fid "modpids=$TempDir/tksu-\$pid-pids"
	puts $fid "echo $filename > \$jobpid"
	puts $fid ""
	puts $fid "cleanup () \{"
	puts $fid {    rm -f $fifos $modpids}
	puts $fid "\}"
	puts $fid ""
	puts $fid "abort () \{"
	puts $fid {    kill `cat $modpids`}
	puts $fid {    cleanup}
	puts $fid {    exit 1}
	puts $fid "\}"
	puts $fid ""
	puts $fid "waitfor () \{"
	puts $fid {    echo -n "$! " >> $modpids}
	puts $fid {    wait $!}
	puts $fid {    status=$?}
	puts $fid {    if [ $1 = tee ]; then return 0; fi}
	puts $fid {    if [ $status -ge 128 ]}
	puts $fid {    then}
	puts $fid {        signum=`expr $status - 128`}
	puts $fid {        signame="(`kill -l $signum`)"}
	puts $fid {    else}
	puts $fid {        signame=""}
	puts $fid {    fi}
	puts -nonewline $fid {    echo 1>&2 }
	puts $fid {"Module $1 exited with status $status $signame"}
	puts $fid "\}"

    } errorMessage] {
    	tk_messageBox -icon error -type ok -title "File write failure" \
	    -message "Could not write command script to file `$filename':\
	    	$errorMessage"
	return 0
    }
    return 1
}

#-------------------------------------------------------------------------------
# WriteScript -- write Bourne shell script for running a module group.
#
# Args:     fid		File id of open file.
#	    group	Group of modules that are to execute simultaneously.
#			Any module referenced by a link that is not in this
#			group is considered disabled, and links to it will
#			be ignored.
# Returns:  success	Returns 1 on success, 0 on write failure.
#
# All ports in the group must be set to a file or a connected pipe.  If
# there remain disconnected pipes, or other problems with parameters,
# WriteScript arranges for an error message to display on stderr when the
# script is run, and the modules will not execute.
#-------------------------------------------------------------------------------

proc tksu::WriteScript {fid group} {
    variable Params
    variable Ports
    variable Values
    variable Links
    variable DevNull
    variable DevZero
    variable Trans
    variable TempDir

# For each module, format each argument that will be put on the command line.

    if {$group == ""} return
    set errors ""
    set warnings ""
    set teeCount 0

    foreach module $group {
	set modbase [ModBase $module]
	set args($module) ""
	set iparam 0
	set iport 0

# If the par file parameter is present, missing a required parameter is not
# a fatal error, since it may be in the par file.

	set vlist [GetVlistByName $module par]
	set gotParFile [expr {[lindex $vlist 3] != ""}]

# Strip index off name, if present.

	foreach vlist $Values($module) {
	    set name [lindex $vlist 2]
	    regsub -- {\(.+\)$} $name {} name
	    set value [lindex $vlist 3]
	    set nvalue [llength $value]

# Parameter:  a defaulted value is not placed on the command line.
# However, an error is logged if it is required.

	    if {[lindex $vlist 0] == "param"} {
		set nlist [lindex $Params($modbase) $iparam]
		incr iparam
		set default [lindex $nlist 3]

		if {$nvalue == 0} {
		    if {$default == "req" && !$gotParFile} {
		    	lappend errors "$module: parameter $name is missing"
		    }

# If value has embedded whitespace, quote it.

		} elseif {$nvalue > 1} {
		    lappend args($module) $name=\"$value\"
		} else {
		    lappend args($module) $name=$value
		}
		continue
	    }

# Port:  load target with appropriate filename.  A null value defaults to
# a null target.

	    set nlist [lindex $Ports($modbase) $iport]
	    incr iport
	    set type [lindex $value 0]
	    set rw [lindex $nlist 2]
	    set default [lindex $nlist 3]
	    set target ""

# Temp port:  depending on rw flag, set DevNull, DevZero or temp file.

	    if {$type == "temp"} {
		if {$rw == "r"} {
		    set target $DevZero
		} elseif {$rw == "w"} {
		    set target $DevNull
		} else {
		    set target [TempPidName]
		}

# File port:  if value has embedded whitespace, quote it.

	    } elseif {$type == "file"} {
		if {$nvalue > 2} {
		    set target \"[lrange $value 1 end]\"
		} elseif {$nvalue == 2} {
		    set target [lindex $value 1]

# Defaulted file port:  if it is required input, log an error and set target
# to DevZero.  If it is required output, log a warning and set target to
# DevNull.  Thus a script can run even if there is a broken output port.
# Defaults for stdin and stdout are explicitly placed on the command line:
# DevZero, DevNull, or the default value.  Otherwise do not place a
# defaulted file port on the command line.

		} elseif {$nvalue == 1} {
		    if {$rw == "r"} {
		        if {$default == "req" && !$gotParFile} {
			    lappend errors "$module: input port $name\
			    	is missing"
			    set target $DevZero

			} elseif {$name != "stdin"} {
			    # null target

			} elseif {$default == "-"} {
			    set target $DevNull

			} else {
			    set target $default
			}
		    } elseif {$rw == "w"} {
		        if {$default == "req" && !$gotParFile} {
			    lappend warnings "$module: output port $name\
			    	is missing"
			    set target $DevNull

			} elseif {$name != "stdout"} {
			    # null target
			
			} elseif {$default == "-"} {
			    set target $DevNull

			} else {
			    set target $default
			}
		    } else {
			if {$name == "stdin" || $name == "stdout"} {
			    lappend errors "$module: port $name cannot\
			    	be a read-write port"
			    set target $DevNull

		    	} elseif {$default == "req" && !$gotParFile} {
			    lappend errors "$module: port $name\
			    	is missing"
			    set target $DevNull
			}
		    }
		}

# Pipe:  retrieve connected links from value.  Include only those links that
# are to other modules of this group.

	    } elseif {$type == "pipe"} {
		set linkids ""
		foreach linkid [lrange $value 1 end] {
		    if {$rw == "r"} {
		    	set otherModule [lindex $Links($linkid) 0]
		    } else {
		    	set otherModule [lindex $Links($linkid) 2]
		    }
		    if {[lsearch -exact $group $otherModule] < 0} continue
		    lappend linkids $linkid
		}
		set nlinks [llength $linkids]

# Log an error (for read port) or warning (for write port) if pipe is
# disconnected.  Wire a disconnected write port to DevNull.

		if {$nlinks < 1} {
		    if {$rw == "r"} {
		    	lappend errors "$module: input port $name\
			    is not connected"
			set target $DevZero
		    } else {
		    	lappend warnings "$module: output port $name\
			    is not connected"
			set target $DevNull
		    }

# If an output port has more than one pipe, allocate a tee to do the splitting.
# The first element of list tee is the pipe for input and the subsequent
# elements are the output pipes.

		} elseif {$nlinks > 1} {
		    set target [TempPidName]
		    incr teeCount
		    set tees($teeCount) $target
		    set namedPipes($teeCount) $target

		    foreach linkid $linkids {
		    	if {![info exists namedPipes($linkid)]} {
			    set namedPipes($linkid) [TempPidName]
			}
			lappend tees($teeCount) $namedPipes($linkid)
		    }

# Normal connected pipe:  look up named pipe for this linkid.  If it hasn't
# been assigned yet, assign it.

		} else {
		    set linkid [lindex $linkids 0]
		    if {![info exists namedPipes($linkid)]} {
			set namedPipes($linkid) [TempPidName]
		    }
		    set target $namedPipes($linkid)
		}
	    }

# Got target for port:  format the argument, depending on the port name.
# An empty target at this point means skip this port.

	    if {$target != ""} {
		if {$name == "stdin"} {
		    lappend args($module) "<$target"
		} elseif {$name == "stdout"} {
		    lappend args($module) ">$target"
		} else {
		    lappend args($module) "$name=$target"
		}
	    }
	}

# Do special argument translation for this module if necessary.
# The translation procedure named in Trans($module) takes args($module)
# as an argument and returns a modified args list to replace args($module).

	if [info exists Trans($modbase)] {
	    set args($module) [$Trans($modbase) $args($module)]
	}
    }

# Wrap the I/O calls in a catch statement.  The script for a group is placed
# in a shell function named after the first module of the group.  The shell
# function reports errors and warnings, and if argument `go' is provided,
# will proceed to execute the script (barring any reported errors).

    if [catch {

	set leader [lindex $group 0]
	regsub -all -- - $leader _ leaderfunction

	puts $fid ""
	puts $fid "#----------------------------------------------------------"
	puts $fid "# Group $leader:"
	puts $fid "#----------------------------------------------------------"
	puts $fid "\nfunction $leaderfunction () \{"
	puts -nonewline $fid {    echo 1>&2 }
	puts $fid "Group $leader:"
	puts $fid {    if [ "$1" = "-check" ]; then}

# During -check stage, report errors and warnings.  Errors are fatal for the
# function.

	if {$warnings != ""} {
	    foreach warning $warnings {
		puts -nonewline $fid {        echo 1>&2 Warning: }
		puts $fid $warning
	    }
	}
	if {$errors != ""} {
	    foreach error $errors {
		puts -nonewline $fid {        echo 1>&2 Error: }
		puts $fid $error
	    }
	    puts $fid {        echo 1>&2 Script cannot be run until\
	    	errors are corrected}
	    puts $fid {        return 1}
	} else {
	    puts $fid {        echo 1>&2 No obvious errors detected}
	    puts $fid {        return 0}
	}
	puts $fid {    fi}
	puts $fid ""
	puts $fid {    echo -n " " > $modpids}
	puts $fid {    status=$?}
	puts $fid {    if [ $status != 0 ]; then exit $status; fi}

# Create list of named pipes.

	puts $fid "\n#   Named pipes:"
	puts $fid "    fifos=\"\\"
	foreach linkid [array names namedPipes] {
	    puts $fid "        $namedPipes($linkid) \\"
	}
	puts $fid {    "}
	if {[array size namedPipes] > 0} {
	    puts $fid {    mkfifo $fifos}
	}

# Write tee commands.

	if {$teeCount > 0} {
	    puts $fid "\n#   Output port tees:"

	    foreach index [array names tees] {
		set tee $tees($index)
		puts $fid {    (}
	    	puts -nonewline $fid {        tee <}
		set n [llength $tee]
		set i 0
		foreach fname $tee {
		    incr i
		    if {$i == $n} {puts -nonewline $fid " >"}
		    puts -nonewline $fid "$fname "
		}
		# Finish tee line with ampersand.
		puts $fid {&}
		puts $fid {        waitfor tee}
		puts $fid {    )&}
	    }
	}

# Write module commands.

	foreach module $group {
	    set modbase [ModBase $module]
	    puts $fid "\n#   Module $module:"
	    puts $fid "    ("
	    puts -nonewline $fid "        $modbase"
	    set column [string length $modbase]
	    incr column 8

	    foreach arg $args($module) {
		set n [expr 1 + [string length $arg]]
		if {$column + $n > 72} {
		    puts -nonewline $fid " \\\n           "
		    set column 11
		}
	    	puts -nonewline $fid " $arg"
		incr column $n
	    }
	    puts $fid " &\n        waitfor $module"
	    puts $fid {    )&}
	}

# Script cleanup code.

	puts $fid "\n#   Wait for modules to finish, then report exit status"
	puts $fid "    wait"
	puts $fid "    echo 1>&2 Completed group $leader"
	puts $fid "    echo 1>&2"
	puts $fid {    cleanup}
	puts $fid {    rm -f $jobpid}
	puts $fid {    return 0}
	puts $fid "\}"

# End of catch statement:

    } errorMessage] {
    	tk_messageBox -icon error -type ok -title "File write failure" \
	    -message "Could not write command script to file: $errorMessage"
	return 0
    }
    return 1
}

#-------------------------------------------------------------------------------
# WriteScriptTrailer -- finish writing Bourne shell script.
#
# Args:     fid		File id of open file.
#	    leaders	List of module groups to be run.
# Returns:  success	Returns 1 on success, 0 on write failure.
#
# A Bourne shell function has been created for each entry in the leaders list.
# The function name is the leader name with `-' replaced with `_', so that
# the Bourne shell function name is valid.
#-------------------------------------------------------------------------------

proc tksu::WriteScriptTrailer {fid leaders} {

    foreach leader $leaders {
	regsub -all -- - $leader _ leaderfunction
	lappend leaderfunctions $leaderfunction
    }

    if [catch {
	puts $fid ""
	puts $fid "#----------------------------------------------------------"
	puts $fid "echo 1>&2\necho 1>&2 Check flow:"
	puts $fid "#----------------------------------------------------------"
	puts $fid "\nstatus=0"
	if {$leaders != ""} {
	    puts $fid "for leader in $leaderfunctions; do"
	    puts $fid {    $leader -check}
	    puts $fid {    status=`expr $status + $?`}
	    puts $fid {done}
	}
	puts $fid {if [ "$1" = "-check" -o $status -ne 0 ]; then}
	puts $fid {    exit $status}
	puts $fid {fi}

	puts $fid ""
	puts $fid "#----------------------------------------------------------"
	puts $fid "echo 1>&2\necho 1>&2 Execute flow:"
	puts $fid "#----------------------------------------------------------"
	if {$leaders != ""} {
	    puts $fid "\nfor leader in $leaderfunctions; do"
	    puts $fid {    $leader}
	    puts $fid {    status=`expr $status + $?`}
	    puts $fid {    if [ $status -ne 0 ]; then break; fi}
	    puts $fid {done}
    	}
	puts $fid {if [ $status = 0 ]; then sdesc=normal; else sdesc=error; fi}
	puts $fid {echo 1>&2 "Flow exited with $sdesc status ($status)"}
	puts $fid "exit \$status\n"

    } errorMessage] {
    	tk_messageBox -icon error -type ok -title "File write failure" \
	    -message "Could not write command script to file: $errorMessage"
	return 0
    }
    return 1
}

#-------------------------------------------------------------------------------
# GetEnabledGroups -- return list of module groups for WriteScript.
#
# Args:     none
# Returns:  groups	List of module groups.
#
# A module group is a list of modules (enabled modules only) that are linked
# together with pipes, and that must be run simultaneously.  See WriteScript
# for details.  The returned list of groups is in the same order that the
# modules are arranged in the canvas, thus defining the order in which the
# groups will be executed.
#-------------------------------------------------------------------------------

proc tksu::GetEnabledGroups {} {
    variable Values
    variable Instances
    variable Links
    variable Disabled

# First collect all enabled modules in their canvas order into a list.

    set groups ""
    set modules ""
    set item ""
    set items [array names Instances]

    while 1 {
    	set item [NextItem $item $items forward]
	if {$item == ""} break
	if {$Disabled($item) > 0} continue
	lappend modules $item
    }

# While entries remain in modules, pull out leading entry, trace its links
# and include the connecting modules in the new group.

    while {[set leader [lindex $modules 0]] != ""} {
	set modules [lrange $modules 1 end]
	set group $leader
	set imod 0

	while {[set module [lindex $group $imod]] != ""} {
	    incr imod
	    foreach vlist $Values($module) {
		if {[lindex $vlist 0] != "port"} break
		set value [lindex $vlist 3]
		if {[lindex $value 0] != "pipe"} continue

		foreach linkid [lrange $value 1 end] {
		    set otherModule [lindex $Links($linkid) 0]
		    if {$otherModule == $module} {
		    	set otherModule [lindex $Links($linkid) 2]
		    }
		    if {[lsearch -exact $group $otherModule] >= 0} continue
		    set jmod [lsearch -exact $modules $otherModule]
		    if {$jmod < 0} continue
		    lappend group [lindex $modules $jmod]
		    set modules [lreplace $modules $jmod $jmod]
		}
	    }
	}
	lappend groups $group
    }
    return $groups
}

#-------------------------------------------------------------------------------
# DisableSelected -- disable all selected modules.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::DisableSelected {} {
    variable SelItems
    variable Instances

    set modules ""
    foreach item $SelItems {
    	if [info exists Instances($item)] {
	    lappend modules $item
	}
    }
    DisableModules $modules
    set undo "EnableModules [list $modules]"
    set redo "DisableModules [list $modules]"
    AddUndo $undo $redo
    AddMark
    return
}

#-------------------------------------------------------------------------------
# EnableSelected -- enable all selected modules.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::EnableSelected {} {
    variable SelItems
    variable Instances

    set modules ""
    foreach item $SelItems {
    	if [info exists Instances($item)] {
	    lappend modules $item
	}
    }
    EnableModules $modules
    set undo "DisableModules [list $modules]"
    set redo "EnableModules [list $modules]"
    AddUndo $undo $redo
    AddMark
    return
}

#-------------------------------------------------------------------------------
# DisableModules -- disable modules in given list.
#
# Args:     modules	Modules to be disabled.
# Returns:  null
#
# The Disabled array holds the enable/disable state of each module.  Valid
# values are:
#
#   0	The module is enabled (normal state).
#   1	The module was explicitly disabled by the user ("hard" disable).
#   2	The module is disabled because an upstream module connected to it via
#	pipe is disabled ("soft" disable).
#
# Hard disabled modules and links sharing a disabled module on either end are
# drawn in a distinct disabled state.  Disabled modules are ignored as far as
# script generation and running a flowsheet are concerned.  They are modules
# that are temporarily "sidelined" by the user, which can be switched back
# into the flow by reenabling them.
#-------------------------------------------------------------------------------

proc tksu::DisableModules modules {
    variable Disabled
    variable Values
    variable Instances
    variable Links
    variable BitmapDir

    set bitmap @[file join $BitmapDir Diagonal.xbm]
    foreach module $modules {

# Change appearance of module to disabled.

	if {$Disabled($module) != 1} {
	    set id [lindex $Instances($module) 0]
	    .canvas itemconfigure $id -stipple $bitmap
	}

# If module is already disabled, do not need to follow links.

    	if {$Disabled($module) > 0} {
	    set Disabled($module) 1
	    continue
	}
	set Disabled($module) 1
	set stack $module

# Starting module is placed on stack.  For each module on the stack, follow
# its downstream links.  If the downstream module is enabled, place it on the
# stack to recursively check its links.  Continue until stack is empty.

	while {[set thisModule [lindex $stack 0]] != ""} {
	    set stack [lrange $stack 1 end]
	    foreach vlist $Values($thisModule) {
	    	if {[lindex $vlist 0] != "port"} break
		set value [lindex $vlist 3]
		if {[lindex $value 0] != "pipe"} continue
		foreach linkid [lrange $value 1 end] {

# Change appearance of both upstream and downstream links to module.

		    set id [lindex [split $linkid :] 2]
		    .canvas itemconfigure $id -stipple $bitmap

# Examine disabled state of downstream module:  if not, disable it and put
# it on the stack.

		    set nextModule [lindex $Links($linkid) 2]
		    if {$nextModule == $thisModule} continue
		    if {$Disabled($nextModule) == 0} {
		    	set Disabled($nextModule) 2
			lappend stack $nextModule
		    }
		};  # End linkid loop
	    };	# End vlist loop
	};  # End stack loop
    };	# End module loop
    return
}

#-------------------------------------------------------------------------------
# EnableModules -- enable given modules, rescan to enable any others.
#
# Args:     modules	Modules to enable.  This argument is optional, and
#			defines modules to explicitly reenable.  Only modules
#			with disabled state = 1 will respond here.  Whether or
#			not the argument is present, EnableModules scans all
#			modules to see if any soft disabled modules may be
#			reenabled.
# Returns:  null
#
# After disabled states have been changed, a list of modules having hard
# disabled states (1) is saved, and the entire module set is reenabled.  Then
# DisableModules is called to reestablish the disabled modules.  The reason
# for rescanning all modules is that when a module or link is removed, a soft
# disabled module may have to be switched back to enabled.
#-------------------------------------------------------------------------------

proc tksu::EnableModules {{modules {}}} {
    variable Disabled
    variable Values
    variable Instances
    variable Links

# Change appearance of modules to be explicitly enabled.

    foreach module $modules {
    	if {$Disabled($module) == 1} {
	    # Hard to soft:
	    set Disabled($module) 2
	    set id [lindex $Instances($module) 0]
	    .canvas itemconfigure $id -stipple {}
	}
    }

# Loop over all modules and reenable.  Save a list of those modules that
# were still hard enabled.

    set hardDisabled ""
    foreach module [array names Instances] {
    	if {$Disabled($module) == 0} continue
	if {$Disabled($module) == 1} {
	    lappend hardDisabled $module
	}
	set Disabled($module) 0

# Change appearance of both upstream and downstream links to module.

	foreach vlist $Values($module) {
	    if {[lindex $vlist 0] != "port"} break
	    set value [lindex $vlist 3]
	    if {[lindex $value 0] != "pipe"} continue

	    foreach linkid [lrange $value 1 end] {
		set id [lindex [split $linkid :] 2]
		.canvas itemconfigure $id -stipple {}
	    }
	}
    }

# Now apply the hard disables again.

    DisableModules $hardDisabled
    return
}

#-------------------------------------------------------------------------------
# PositionalArgs -- procedure to translate arguments to positional form.
#
# Args:     arglist	List of arguments in `name=value' form (some arguments
#			may have other forms, such as `<input' or `>output').
# Returns:  arglist	The argument list modified in the following way.
#			If a name having a suffix `-n' is encountered, it is
#			taken to be a positional argument.  The name is
#			stripped, leaving only the value in the arglist.
#			It is assumed that the argument is already in the
#			correct position in the arglist.
#-------------------------------------------------------------------------------

proc tksu::PositionalArgs arglist {
    
    set newlist ""
    foreach arg $arglist {
    	regsub -- {.+-[0-9]+=} $arg {} arg
	lappend newlist $arg
    }
    return $newlist
}

#-------------------------------------------------------------------------------
# SortArgs -- procedure to convert arguments to form required by susort.
#
# Args:     arglist	List of arguments in `name=value' form (some arguments
#			may have other forms, such as `<input' or `>output').
# Returns:  arglist	The argument list modified in the following way.
#			If a value of `+' or `-' is encountered, `name=[+-]'
#			is converted to `[+-]name'.
#-------------------------------------------------------------------------------

proc tksu::SortArgs arglist {
    
    set newlist ""
    foreach arg $arglist {
	regsub -- {(.+)=([+-])} $arg {\2\1} arg
	lappend newlist $arg
    }
    return $newlist
}
