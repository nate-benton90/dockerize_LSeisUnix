# About.tcl --
#
#	Tksu procedures implementing the `About ...' window.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: About.tcl,v 1.1.1.1 2002/06/07 23:43:30 jeff Exp $

#-------------------------------------------------------------------------------
# About -- raise window containing info on tksu, SU and Tcl/Tk.
#
# Args:     none
# Returns:  null
#
# Information in this window includes:
#
#   Tksu version number, install directory, brief abstract.
#   Henry Thorson logo, copyright, web address, GPL license button.
#   CWP/SU logo, version number, install directory, copyright, web address.
#   Tcl/Tk logo, version number, copyright, web address.
#-------------------------------------------------------------------------------

proc tksu::About {} {
    variable TksuDir
    variable Version
    global auto_path

    if [winfo exists .about] {
    	RaiseWindow .about
	return
    }

# Create window.

    set noticeHT "Tksu is a graphical front end to the SU seismic processing\
	package.  It provides dialogs for setting module parameters, online\
	help, and the ability to build a processing flow from arbitrarily\
	connected modules.  It creates a shell script which may be run\
	interactively or exported for running later.\
	\n\
	\nAlthough designed with SU in mind, Tksu is an independent\
	application.  Since it interfaces with SU via shell scripts, it can\
	manage any module that implements the SU command line protocol.\
	\n\
	\nHenry Thorson Consulting Tksu version $Version\
	\nInstalled in directory $TksuDir\
	\nFor the latest version, visit www.henrythorson.com\
	\nCopyright (c) 2002 by Henry Thorson Consulting.  All Rights Reserved.\
	\nThis software is subject to the GNU General Public License, version 2"

    set noticeCwp "Center for Wave Phenomena SU version [GetCwpVersion]\
	\nInstalled in directory [GetCwpRoot]\
	\nFor the latest version, visit www.cwp.mines.edu\
	\nSU is copyrighted (c) 1992-2002 by the Colorado School of Mines"

    set noticeTcl "This application is powered by Tcl/Tk, version\
    	[info patchlevel]\
	\nInstalled in directory [lindex $auto_path 0]\
	\nFor the latest version, visit www.tcl.tk\
	\nTcl/Tk is copyrighted by the Regents of the University of\
	California, Sun Microsystems Inc., and other parties."

    set w .about
    toplevel $w
    wm title $w "About Tksu"
    wm protocol $w WM_DELETE_WINDOW "tksu::HideWindow $w"

    frame $w.top
    label $w.top.dummy -width 8
    label $w.top.title -text "This is Tksu Version $Version"
    button $w.top.close -text Close -underline 0 -command "tksu::HideWindow $w"

    pack $w.top.dummy -side left
    pack $w.top.close -side right
    pack $w.top.title -expand 1 -anchor s
    pack $w.top -side top -fill x

    frame $w.ht -relief groove -borderwidth 4
    label $w.ht.logo -image LogoHT -width 120
    label $w.ht.notice -font VarFont -text $noticeHT -justify left \
	-wraplength 18c
    button $w.ht.view -text "View General Public License" -underline 0 \
	-command "tksu::Help {GNU General Public License} \
		  [file join $TksuDir COPYING]"

    pack $w.ht.view -side bottom
    pack $w.ht.logo -side left
    pack $w.ht.notice -side left -ipadx 12
    pack $w.ht -side top -padx 8 -pady 8 -fill x

    frame $w.cwp -relief groove -borderwidth 4
    label $w.cwp.logo -image LogoCwp -width 120
    label $w.cwp.notice -font VarFont -text $noticeCwp -justify left \
    	-wraplength 18c

    pack $w.cwp.logo -side left
    pack $w.cwp.notice -side left -ipadx 12
    pack $w.cwp -side top -padx 8 -fill x

    frame $w.tcl -relief groove -borderwidth 4
    label $w.tcl.logo -image LogoTcl -width 120
    label $w.tcl.notice -font VarFont -text $noticeTcl -justify left \
    	-wraplength 18c

    pack $w.tcl.logo -side left
    pack $w.tcl.notice -side left -ipadx 12
    pack $w.tcl -padx 8 -pady 8 -fill x

# Accelerator bindings:

    bind $w <Alt-v> "$w.ht.view invoke"
    bind $w <Alt-c> "$w.top.close invoke"
    bind $w <Escape> "$w.top.close invoke"
    return
}

#-------------------------------------------------------------------------------
# GetCwpRoot -- return CWPROOT directory, if defined.
#
# Args:     none
# Returns:  cwproot	Directory that SU is installed in, or a message
#			declaring that it may not be installed.
#-------------------------------------------------------------------------------

proc tksu::GetCwpRoot {} {
    global env

    if [info exists env(CWPROOT)] {
    	return $env(CWPROOT)
    }
    return "(unknown or uninstalled)"
}

#-------------------------------------------------------------------------------
# GetCwpVersion -- return current CWP/SU version number.
#
# Args:     none
# Returns:  version	Version number from CWPROOT/src/LICENSE_XX where XX is
#			the version number.  If CWPROOT is not defined or the
#			version file is not found, return a message declaring
#			that SU may not be installed.
#-------------------------------------------------------------------------------

proc tksu::GetCwpVersion {} {
    global env

    if [info exists env(CWPROOT)] {
	set files ""
	catch {
	    set files [readdir [file join $env(CWPROOT) src]]
	}
	foreach file $files {
    	    if [regexp {^LICENSE_(.+)$} $file match version] {
	    	return $version
	    }
    	    if [regexp {^MAILHOME_(.+)$} $file match version] {
	    	return $version
	    }
	}
    }
    return "(unknown or uninstalled)"
}
