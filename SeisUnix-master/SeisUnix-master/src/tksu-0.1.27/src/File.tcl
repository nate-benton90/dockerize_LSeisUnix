# File.tcl --
#
#	Tksu procedures implementing the file menu.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: File.tcl,v 1.7 2002/08/04 19:39:46 jeff Exp $

#-------------------------------------------------------------------------------
# SaveCanvas -- write canvas to file.
#
# Args:     filename	Optional argument names file to save canvas to.
# Returns:  success	Returns 1 for success, 0 for failure/cancel by user.
#
# If the argument is not given, variable CurrentFlow is used for the filename.
# If filename (or CurrentFlow) is null, a file dialog is raised to prompt
# user for filename.
#
# If the canvas is empty, a flowsheet file is still created in order to save
# preset parameters from Values(modbase).
#-------------------------------------------------------------------------------

proc tksu::SaveCanvas args {
    variable Instances
    variable CurrentFlow
    variable SavedLevel
    variable CanvasLevel
    variable FlowDir

    if {[llength $args] > 0} {
    	set filename [lindex $args 0]
    } else {
    	set filename $CurrentFlow
    }

# Prompt for filename.

    if {$filename == ""} {
	set types {
	    {"Flowsheet Files" .flo}
	    {"Shell Scripts" .sh}
	    {"All Files" *}
	}
	set filename [tk_getSaveFile -defaultextension .flo -filetypes $types \
	    -initialdir $FlowDir -title "Save Flowsheet"]
	if {$filename == ""} {
	    return 0
	}
	set FlowDir [file dirname $filename]
    }

# Open filename for writing.

    if [catch {open $filename w} fid] {
	tk_messageBox -icon error -type ok -title "File open failure" \
	    -message "Could not open file `$filename' for writing: $fid"
	return 0
    }

# Write header and timestamp.

    if {![WriteScriptHeader $fid $filename]} {
	catch {close $fid}
	return 0
    }

# Write separate block of script for each enabled module group.  The module
# groups are ordered by their position in the canvas, which defines the
# order in which they will be executed.

    set groups [GetEnabledGroups]
    set leaders ""
    foreach group $groups {
	if {![WriteScript $fid $group]} {
	    catch {close $fid}
	    return 0
	}
	lappend leaders [lindex $group 0]
    }

# Add script to invoke each group function in sequence.

    if {![WriteScriptTrailer $fid $leaders]} {
	catch {close $fid}
	return 0
    }

# Write canvas to file.

    if {![WriteCanvas $fid]} {
    	catch {close $fid}
	return 0
    }

# All done:

    if [catch {close $fid} message] {
    	tk_messageBox -icon error -type ok -title "File save failure" \
	    -message "Could not save to file `$filename': $message"
	return 0
    }
    catch {exec chmod +x $filename}

    set CurrentFlow $filename
    set SavedLevel $CanvasLevel
    SetCanvasTitle
    return 1
}

#-------------------------------------------------------------------------------
# WriteCanvas -- write canvas contents to file.
#
# Args:     fid		File id of open file.
# Returns:  success	Returns 1 on success, 0 on write failure.
#
# All persistent information about the current canvas is written to file in
# the form of a tcl script.  The script can be read back by ReadCanvas, and
# restored with RestoreCanvas.
#-------------------------------------------------------------------------------

proc tksu::WriteCanvas fid {
    variable Values
    variable Instances
    variable Links
    variable Disabled

# Wrap this routine in a catch statement, since it is doing I/O.

    if [catch {

	puts $fid "#-----------------------------------------------------------"
	puts $fid "# Tksu canvas definition -- please do not edit this section."
	puts $fid "#-----------------------------------------------------------"
	puts $fid "<< TksuCanvasEnd"

# Preset parameters:  any values for uninstantiated modules set by the user.

	foreach module [array names Values] {
	    set modbase [ModBase $module]
	    if {$modbase != $module} continue

	    set empty 1
	    foreach vlist $Values($modbase) {
		set class	 [lindex $vlist 0]
		set genericName	 [lindex $vlist 1]
		set specificName [lindex $vlist 2]
		set value	 [lindex $vlist 3]

		if {$value == ""} {
		    if {$specificName == $genericName || $specificName == ""} {
		    	continue
		    }
		}
		if {$class == "port" \
		&& [lindex $value 0] != "temp" \
		&& [lindex $value 1] == ""} {
		    continue
		}
		if $empty {
		    puts $fid "set ModuleValues($module) \{"
		    set empty 0
		}
		set vlist [lreplace $vlist 3 5 $value {} {}]
		puts $fid "    [list $vlist]"
	    }
	    if {!$empty} {
		puts $fid "\};"
	    }
	}

# Values array:  just save values that are different from the default (null).
# Don't save linked port values here -- those are saved in LinkValues.

	foreach module [array names Instances] {
	    puts $fid "set ModuleValues($module) \{"

	    foreach vlist $Values($module) {
		set class	 [lindex $vlist 0]
		set genericName	 [lindex $vlist 1]
		set specificName [lindex $vlist 2]
		set value	 [lindex $vlist 3]

		if {$value == ""} {
		    if {$specificName == $genericName || $specificName == ""} {
		    	continue
		    }
		}
		if {$class == "port" \
		&& [lindex $value 0] == "pipe" \
		&& [lindex $value 1] != ""} {
		    continue
		}

# Don't bother saving the history lists.

		set vlist [lreplace $vlist 3 5 $value {} {}]
		puts $fid "    [list $vlist]"
	    }
	    puts $fid "\};"
	}

# The ModuleCoords array stores each module's upper left canvas coordinate.

	foreach module [array names Instances] {
	    set id [lindex $Instances($module) 0]
	    set coords [.canvas coords $id]
	    puts $fid "set ModuleCoords($module) [list [lrange $coords 0 1]];"
	}

# Links array:  replace linkid with simple integer.  Also write the port
# name instead of the port index.

	set ilink 1
	foreach linkid [array names Links] {
	    set mpw [GetModPort $linkid w]
	    set mpr [GetModPort $linkid r]
	    puts $fid "set LinkValues($ilink) [list [concat $mpr $mpw]];"
	    incr ilink
	}

# The LinkCoords array holds link canvas coordinates.

	set ilink 1
	foreach linkid [array names Links] {
	    set coords [GetLinkCoords $linkid]
	    puts $fid "set LinkCoords($ilink) [list $coords];"
	    incr ilink
	}

# The HardDisabled list holds all hard disabled modules.

	set hardDisabled ""
	foreach module [array names Instances] {
	    if {$Disabled($module) == 1} {
	    	lappend hardDisabled $module
	    }
	}
	puts $fid "set HardDisabled [list $hardDisabled]"
	puts $fid "TksuCanvasEnd"
	puts $fid "#-----------------------------------------------------------"

# End of catch statement:

    } errorMessage] {
    	tk_messageBox -icon error -type ok -title "File write failure" \
	    -message "Could not write flowsheet to file: $errorMessage"
	return 0
    }
    return 1
}

#-------------------------------------------------------------------------------
# SetCanvasTitle -- set title to canvas window.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::SetCanvasTitle {} {
    variable CurrentFlow
    variable SavedLevel
    variable CanvasLevel

    set basename [file tail $CurrentFlow]
    if {$basename == ""} {
    	set title "Tksu Flowsheet (untitled)"
    } else {
    	set title "Flowsheet  `$basename'"
	if {$SavedLevel != $CanvasLevel} {
	    append title " (modified)"
	}
    }
    wm title . $title
    return
}

#-------------------------------------------------------------------------------
# ClearCanvas -- clear everything out of current canvas.
#
# Args:     none
# Returns:  success	Returns 1 if successful, 0 if not.
#
# The user is given a chance to save the current canvas if it was modified.
# If the response is no, the canvas is cleared and 1 is returned.  If yes,
# the file dialog is raised.  If the user cancels in the file dialog, or if
# SaveCanvas fails to write the file, the canvas is not cleared and 0 is
# returned.  Otherwise the canvas is cleared and 1 is returned.
#
# If the canvas is empty, the user may still be asked to save modifications
# made to the preset parameters.
#-------------------------------------------------------------------------------

proc tksu::ClearCanvas {} {
    variable CurrentModule
    variable CurrentFlow
    variable SelItems
    variable DeselItems
    variable SavedLevel
    variable CanvasLevel
    variable Instances
    variable Values

    if {$SavedLevel != $CanvasLevel && [info exists Values]} {

    	set response [tk_messageBox -icon question -type yesnocancel \
	    -default yes -title "Flowsheet has changed" \
	    -message "The current flowsheet has not been saved.  Should\
	    	it be written to file?"]
	if {$response == "cancel"} {
	    return 0
	}
	if {$response == "yes"} {
	    if {![SaveCanvas]} {return 0}
	}
    }

# Go ahead and clear the canvas.

    set SelItems [concat $SelItems $DeselItems]
    set DeselItems {}
    DeleteSelected
    ResetUndo
    set CurrentFlow ""
    SetCanvasTitle

# Clear preset parameters too.

    foreach instance [array names CurrentModule] {
    	if {$CurrentModule($instance) != ""} {
	    HideParamList $instance
	}
    }
    catch {
    	unset Values
    }
    return 1
}

#-------------------------------------------------------------------------------
# OpenCanvas -- clear current canvas and read one in from file.
#
# Args:     filename	Optional:  the flowsheet file to open.  If not given,	
#			the user is prompted for an input file.
# Returns:  success	If successful, returns 1.  Otherwise a return of 0
#			indicates the user canceled the operation, the new
#			canvas could not be read, or the old canvas could
#			not be saved.
#-------------------------------------------------------------------------------

proc tksu::OpenCanvas {{filename {}}} {
    variable CurrentFlow
    variable FlowDir

# Prompt for filename.

    if {$filename == ""} {
	set types {
	    {"Flowsheet Files" .flo}
	    {"Shell Scripts" .sh}
	    {"All Files" *}
    	}
    	set filename [tk_getOpenFile -defaultextension .flo -filetypes $types \
	    -initialdir $FlowDir -title "Open Flowsheet"]
    	if {$filename == ""} {
	    return 0
    	}
    }
    set FlowDir [file dirname $filename]

# Clear canvas after getting filename.

    if {![ClearCanvas]} {return 0}

# Open filename for reading.

    if [catch {open $filename r} fid] {
	tk_messageBox -icon error -type ok -title "File open failure" \
	    -message "File open failure: $fid"
	return 0
    }

# Read canvas information from file.

    if {![ReadCanvas $fid]} {
    	catch {close $fid}
	return 0
    }
    if [catch {close $fid} message] {
    	tk_messageBox -icon error -type ok -title "File read failure" \
	    -message "Could not read file `$filename': $message"
	return 0
    }

# Instantiate all modules and links from information read in.

    if {![FixupCanvas]} {
    	return 0
    }
    ScrollCanvas [ModuleBounds]
    set CurrentFlow $filename
    SetCanvasTitle
    return 1
}

#-------------------------------------------------------------------------------
# ReadCanvas -- read canvas information from file.
#
# Args:     fid		Open file identifier.
# Returns:  success	Returns 1 if successful, or 0 if the read failed.
#
# Text lines in the file bracketed by `<< TksuCanvasEnd' and `TksCanvasEnd'
# are read and interpreted as a tcl script.  The script loads values into
# the arrays:  ModuleValues, ModuleCoords, LinkValues and LinkCoords.
# These arrays are used by FixupCanvas to instantiate modules and links,
# reconstructing the canvas.  SaveCanvas created the file.
#-------------------------------------------------------------------------------

proc tksu::ReadCanvas fid {
    variable ModuleValues
    variable ModuleCoords
    variable LinkValues
    variable LinkCoords
    variable HardDisabled

# Wrap this routine in a catch statement, since it is doing I/O.

    if [catch {

	set script ""
    	while {[gets $fid line] >= 0} {

# Look for beginning of script.

	    if {$script == ""} {
	    	if {$line == "<< TksuCanvasEnd"} {
		    set script " "
		}
		continue
	    }

# Look for end of script.  Meanwhile accumulate lines in script.

	    if {$line == "TksuCanvasEnd"} break
	    append script $line
	}

# Invoke script to fill arrays.  Errors due to a corrupted script will
# hopefully be caught.

	eval $script

# End of catch statement.

    } errorMessage] {
    	tk_messageBox -icon error -type ok -title "File read failure" \
	    -message "Could not read flowsheet from file: $errorMessage"
	return 0
    }
    return 1
}

#-------------------------------------------------------------------------------
# FixupCanvas -- populate empty canvas from arrays read by ReadCanvas.
#
# Args:     none
# Returns:  success	Returns 1 if successful, or 0 if the arrays from
#			ReadCanvas failed consistency checks.
#
# Arrays ModuleValues, ModuleCoords, LinkValues, LinkCoords should have been
# initialized by ReadCanvas.  Use information within to instantiate modules
# and links.  It is assumed that the canvas is empty to start with.
#-------------------------------------------------------------------------------

proc tksu::FixupCanvas {} {
    variable ModuleValues
    variable ModuleCoords
    variable LinkValues
    variable LinkCoords
    variable HardDisabled
    variable Desc
    variable Ports
    variable Params
    variable Values
    variable Instances
    variable Links
    variable Disabled

    set errors ""

# Create modules listed in ModuleValues.

    foreach module [array names ModuleValues] {

    	set modbase [ModBase $module]
	set instantiated [expr {$module != $modbase}]

	if {![info exists Desc($modbase)]} {
	    lappend errors "Module `$modbase' is unknown."
	    continue
	}

	if $instantiated {
	    if {![info exists ModuleCoords($module)]} {
		lappend errors "Module `$module': missing coordinates."
		continue
	    }
	    set xy $ModuleCoords($module)
	    if {[llength $xy] != 2} {
		lappend errors "Module `$module': bad coordinates."
		continue
	    }

# Request the same instance number as before by calling AddModule with the
# module name instead of the base name.

	    set newModule [eval AddModule $module $xy]
	    if {$newModule != $module} {
		lappend errors "Tried to create module $module, but it already\
		    exists, or has bad specs."
		continue
	    }
	}

# Load port values from ModuleValues (if any).

	set Values($module) ""
	if [info exists Ports($modbase)] {
	    set iport 0
	    foreach nlist $Ports($modbase) {
	        set name [lindex $nlist 0]
	        set newVlist [NewValue port $nlist]

	        foreach vlist $ModuleValues($module) {
	    	    if {[lindex $vlist 0] != "port"} break
		    if {[lindex $vlist 1] != $name} continue

		    set newVlist $vlist
		    break
	        }
	        lappend Values($module) $newVlist
		if $instantiated {
		    PortBox $module $iport
		}
		incr iport
	    }
	}

# Load parameter values from ModuleValues (if any).

	if [info exists Params($modbase)] {
	    foreach nlist $Params($modbase) {
	        set name [lindex $nlist 0]
	        set newVlist [NewValue param $nlist]

	        foreach vlist $ModuleValues($module) {
	    	    if {[lindex $vlist 0] != "param"} continue
		    if {[lindex $vlist 1] != $name} continue

		    # Duplicated parameters:
		    if {[lindex $vlist 1] != [lindex $vlist 2]} {
		        lappend Values($module) $vlist
			set newVlist ""
		        continue
		    }
		    # Regular parameter:
		    lappend Values($module) $vlist
		    set newVlist ""
		    break
	        }
		# Initialized parameter:
		if {$newVlist != ""} {
		    lappend Values($module) $newVlist
		}
	    }
	}
    }

# Create links listed in LinkValues.

    foreach ilink [array names LinkValues] {

    	if {![info exists LinkCoords($ilink)]} {
	    lappend errors "Link $ilink: missing coordinates."
	    continue
	}
	set module1 [lindex $LinkValues($ilink) 0]
	set port1   [lindex $LinkValues($ilink) 1]
	set module2 [lindex $LinkValues($ilink) 2]
	set port2   [lindex $LinkValues($ilink) 3]

	if {![info exists Instances($module1)] \
	||  ![info exists Instances($module2)]} {
	    lappend errors "Link $ilink connects to a nonexistent module."
	    continue
	}
	set iport1 [PortIndex $module1 $port1]
	set iport2 [PortIndex $module2 $port2]

	if {$iport1 == "" || $iport2 == ""} {
	    lappend errors "Link $ilink connects to a nonexistent port."
	    continue
	}

# Link passed simple tests: create it.  Any warnings from LinkPort should be
# suppressed at this stage.

	ActivatePort $module1 $iport1
	set linkid [LinkPort $module2 $iport2 0]
	DeactivatePort

# Set its coordinates.

	if {$linkid != ""} {
	    eval SetLinkCoords $linkid $LinkCoords($ilink)
	}
    }

# Restore disabled state to modules.
# Done with variables set by ReadCanvas:  remove them.

    DisableModules $HardDisabled
    catch {
	unset ModuleValues ModuleCoords LinkValues LinkCoords HardDisabled
    }
    ResetUndo

# Report accumulated errors in a dialog.  Limit number of messages.

    if {$errors != ""} {
	set message ""
	set i 0
	foreach error [lrange $errors 0 3] {
	    incr i
	    append message "$i) $error\n"
	}
    	tk_messageBox -icon error -type ok -title "Errors loading flowsheet" \
	    -message $message
	return 0
    }
    return 1
}

#-------------------------------------------------------------------------------
# ExitCanvas -- exit program.
#
# Args:     none
# Returns:  null	If successful, it doesn't return.  The normal exit
#			status is 0.  If it does return, it is because the
#			user has canceled the operation or SaveCanvas failed
#			to write the current canvas.
#-------------------------------------------------------------------------------

proc tksu::ExitCanvas {} {
    variable LayoutFile

    SaveLayout $LayoutFile
    if [ClearCanvas] {
    	exit 0
    }
    return
}

#-------------------------------------------------------------------------------
# TempName -- return a temporary file name.
#
# Args:     none
# Returns:  filename	Fully qualified temporary file name.
#-------------------------------------------------------------------------------

proc tksu::TempName {} {
    variable SeqNumber
    variable TempDir

    if {![info exists SeqNumber]} {
    	set SeqNumber 0
    }
    incr SeqNumber
    return $TempDir/tksu-[pid]-$SeqNumber
}

#-------------------------------------------------------------------------------
# TempPidName -- return temporary file name with embedded process id.
#
# Args:     none
# Returns:  filename	Fully qualified file name, with embedded `$pid'.
#
# The process id in the filename returned by TempName is replaced with `$pid'
# so that each invocation of the shell puts its own pid in the filename,
# guaranteeing uniqueness.
#-------------------------------------------------------------------------------

proc tksu::TempPidName {} {

    regsub -- "-[pid]-" [TempName] {-$pid-} pidName
    return $pidName
}
