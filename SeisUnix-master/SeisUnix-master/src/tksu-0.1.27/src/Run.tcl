# Run.tcl --
#
#	Tksu procedures implementing the run menu.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Run.tcl,v 1.4 2002/07/30 20:50:44 jeff Exp $

#-------------------------------------------------------------------------------
# RunFlow -- execute flowsheet script.
#
# Args:     -file	Optional:  if `-file' is present, raise the open file
#			dialog and prompt the user for a flowsheet to run.
#			Otherwise the current flowsheet in the canvas is run.
#	    -check	Optional:  if `-check' is present, report warnings
#			and errors to log output, but don't run the script.
#			Otherwise if `-check' is missing and there are no
#			errors reported, the flowsheet is run.
#
# If the current flowsheet is to be run, it is first saved to file.  The
# save file dialog may raise if the current flowsheet has not been named yet.
#-------------------------------------------------------------------------------

proc tksu::RunFlow args {
    variable SavedLevel
    variable CanvasLevel
    variable CurrentFlow
    variable SaveLogFile
    variable ShowLogRun
    variable DevNull
    variable FlowDir

# Get flowsheet to run.

    set flowsheet ""
    if {[lsearch -exact $args "-file"] >= 0} {
	set types {
	    {"Flowsheet Files" .flo}
	    {"Shell Scripts" .sh}
	    {"All Files" *}
	}
	set flowsheet [tk_getOpenFile -defaultextension .flo \
	    -initialdir $FlowDir -filetypes $types -title "Run Flowsheet"]

	if {$flowsheet != ""} {
	    set FlowDir [file dirname $flowsheet]
	}

    } else {
	if {$SavedLevel != $CanvasLevel} {
	    SaveCanvas
	}
	set flowsheet $CurrentFlow
    }
    if {$flowsheet == ""} return

# Arrange for stderr to go to a log file.  If SaveLogFile is on, the log file
# has the same path and prefix as the flowsheet.  If necessary, a numeric
# suffix is incremented to guarantee that the log file name is unique.

    if $SaveLogFile {
	set n 1
	while 1 {
	    set logfile [file rootname $flowsheet]-$n.log
	    if {![file exists $logfile]} break
	    incr n
	}

# If SaveLogFile is off, stderr is directed to a temporary file that the
# log window will read.

    } elseif $ShowLogRun {
    	set logfile [TempName]
    } else {
    	set logfile $DevNull
    }

# If ShowLogRun is on, raise the log window now.  If ShowLogRun is off, the
# log file is still saved unless SaveLogFile is also off.

    if $ShowLogRun {
	if $SaveLogFile {
    	    ShowLog -running $logfile
	} else {
    	    ShowLog -running -delete $logfile
	}
    }

# Start the run.  If the CurrentFlow is run, copy the flowsheet to a hidden
# file in TempDir and run from it.  This allows the user to modify the
# CurrentFlow and submit concurrent runs.

    set check ""
    if {[lsearch -exact $args "-check"] >= 0} {
    	set check "-check"
    }
    if {$flowsheet == $CurrentFlow} {
	set flowsheet [TempName]
	exec cp $CurrentFlow $flowsheet
	exec sh $flowsheet $check >& $logfile &
	after 5000 exec rm $flowsheet
    } else {
	exec sh $flowsheet $check >& $logfile &
    }
    return
}

#-------------------------------------------------------------------------------
# KillFlows -- terminate running flows.
#
# Args:     none
# Returns:  null
#
# Look in TempDir for files with the name `tksu-[pid]-run'.  For each such
# file, add a button to the Kill submenu (the menu cascaded off the Kill
# button).  Clicking on a button will kill the associated run.  KillFlows is
# designed to be the post command for the Kill submenu, and the menu is
# dynamically updated each time it is posted.
#-------------------------------------------------------------------------------

proc tksu::KillFlows {} {
    variable TempDir

    if [catch {
    	exec ls $TempDir
    } files] {
    	set files ""
    }
    set menu .menu.run.kill
    $menu delete 0 end

    foreach file $files {
    	if {![regexp -- {tksu-([0-9]+)-run} $file dummy pid]} continue
	set flowsheet [exec cat [file join $TempDir $file]]
	set flowsheet [file tail [file rootname $flowsheet]]
	if {$flowsheet == "" || $pid == ""} continue

	set label [format "%5d %s" $pid $flowsheet]
	$menu add command -label $label \
			  -command "tksu::KillFlow $pid"
    }
    if {[$menu index end] == "none"} {
    	$menu add command -label "No running flows found"
    }
    return
}

#-------------------------------------------------------------------------------
# KillFlow -- kill individual flow.
#
# Args:     pid		Process ID of shell running flowsheet script.
# Returns:  null
#
# This command is bound to each item in the Kill menu.
#-------------------------------------------------------------------------------

proc tksu::KillFlow pid {
    variable TempDir

    exec kill $pid
#   after 1000 exec kill -9 $pid
    exec rm -f $TempDir/tksu-$pid-run
    return
}

#-------------------------------------------------------------------------------
# ShowLog -- raise window displaying log file.
#
# Args:     -file	Optional:  if `-file' is present, raise the open file
#			dialog and prompt the user for a log file.
#	    -running	Optional:  if `-running' is present, scroll to the
#			end of the log file and display the lines as they are
#			being written to the file.  This is the `F' command
#			in less.
#	    -delete	Optional: if `-delete' is present, delete the given
#			log file when the window is closed.
#	    logfile	If -file is not given, this argument should be given,
#			naming the log file to be displayed.
#
# The log file is browsed with the `less' command in an xterm window.
#-------------------------------------------------------------------------------

proc tksu::ShowLog args {
    variable Color
    variable XtermFont
    variable FlowDir

# Scan args, get logfile name.

    set logfile ""
    set running ""
    set delcmd 0
    foreach arg $args {
    	if {$arg == "-file" && $logfile == ""} {
	    set types {
		{"Log Files" .log}
		{"All Files" *}
	    }
	    set logfile [tk_getOpenFile -defaultextension .flo \
		-initialdir $FlowDir -filetypes $types -title "Open Log File"]

	    if {$logfile != ""} {
		set FlowDir [file dirname $logfile]
	    }

	} elseif {$arg == "-running"} {
	    set running "+F"

	} elseif {$arg == "-delete"} {
	    set delcmd 1

	} elseif {$logfile == ""} {
	    set logfile $arg
	}
    }
    if {$logfile == ""} return

# Bring up less command in xterm window.

    set basename [file tail $logfile]
    if $delcmd {
    	set title "Log Output"
    } else {
	set title "less $basename"
    }
    set command "xterm +sb -bg white -fg black -fn $XtermFont \
    	-hc '$Color(selected)' -T '$title' -n '$title' \
	-e less -m $running $logfile"

    if $delcmd {
    	append command {; rm -f }
	append command $logfile
    }
    catch {
	exec sh -c $command &
    }
    return
}
