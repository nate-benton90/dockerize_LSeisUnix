# tksu.tcl --
#
#	Primary file for tksu application.  Tksu is a graphical front end to
#	the SU seismic processing package.  It allows the user to create shell
#	scripts for running programs (or `modules') in the SU package.
#	Dialogs are available for selecting modules and for setting command
#	line parameters for each module.  Pipes may be established from any
#	output port of module A to any input port of module B, although tksu
#	will verify that the data types of the input and output ports are
#	consistent.  The user may notify tksu of new, customized SU modules
#	by providing a simple specification file that defines the command
#	line arguments and the I/O properties of the new module.  A flowsheet
#	may be graphically built that defines the processing flow within a
#	connected network of SU modules.  After a flowsheet is created and
#	saved, the associated shell script may be interactively run from tksu,
#	with the log output displayed to a window.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: tksu.tcl,v 1.5 2002/09/20 02:44:18 jeff Exp $

#-------------------------------------------------------------------------------
# tksu -- application entry point.
#
# Args:     The command line arguments are all optional.  Synopsis:
#	    tksu [--help] [--version] [-installdir] [--layout file] [flowsheet]
#
#	    --help		Print usage statement and exit.
#	    --version		Print version number and exit.
#	    --installdir	Print installation directory and exit.
#	    --layout <file>	Read <file> for layout instead of ~/.tksu.
#	    <flowsheet>		Open this <flowsheet> to begin.
#
# Returns:  null
#-------------------------------------------------------------------------------

package require Tclx 8.0
namespace eval tksu {
    namespace export tksu
}

proc tksu::tksu args {
    global env auto_path

# Begin by initializing various namespace variables.

    variable GridInc 32;	# Granularity in canvas for moving modules.
    variable ModDim;		# Module rectangle dimensions.
    variable PortDim;		# Port rectangle dimensions.
    variable CanvasDim;		# Original canvas region dimensions.
    variable Stub;		# Minimum link stub dimensions.
    variable Threshold 6.0;	# Pixel threshold to activate drag.
    variable Enum;		# Enumeration definitions.
    variable Color;		# Array of color definitions.
    variable Geometry;		# Persistent memory for window positions.
    variable CurrentCat;	# Current category shown in modlist.
    variable SelItems {};	# Selected items list.
    variable DeselItems {};	# Deselected items list.
    variable DragActive {};	# See DragStart.
    variable ActivePort {};	# See DragStart.
    variable InfoStack {bottom ""}; # See PushInfo.
    variable CurrentFlow "";	# Flowsheet file currently shown in canvas.
    variable ShowLogRun 1;	# Boolean: show log file during run.
    variable SaveLogFile 1;	# Boolean: save log output to file.
    variable UndoLimit 25;	# Number of user actions that can be undone.
    variable LayoutFile "";	# File where preferences are stored.
    variable TksuDir "";	# Tksu installation directory.
    variable SrcDir "";		# Tksu source directory.
    variable BitmapDir "";	# Tksu bitmap directory.
    variable SpecPath "";	# Directory path for spec files.
    variable Version;		# Program version number.

    variable FlowDir;		# Current directory for flowsheets, log files.
    variable DataDir;		# Current directory for data files.
    variable TempDir /tmp/tksu; # Unix directory for FIFOs, temp files.
    variable DevNull /dev/null;	# Unix null device.
    variable DevZero /dev/zero;	# Unix zero device.
    variable XtermFont;		# An X-style font to use in xterm.

    array set ModDim	{x 128 y  44};	# Module box size in pixels.
    array set PortDim	{x  16 y  12};	# Port box size in pixels.
    array set CanvasDim	{x 500 y 500};	# Original canvas dimensions.
    array set Stub	{x  32 y  16};	# Min length of link seg at port endpt.

    set Version [package provide tksu]

# Process command line args.

    set showHelp 0
    set showTksuDir 0
    set layoutFile ""
    set flowsheet ""

    while {[llength $args] > 0} {
	switch -- [lindex $args 0] {
	    
	    --help {
		set showHelp 1
	    }
	    --version {
		puts "tksu version $Version"
		exit 0
	    }
	    --installdir {
	    	set showTksuDir 1
	    }
	    --layout {
	    	set layoutFile [lindex $args 1]
		set args [lrange $args 1 end]
	    }
	    default {
	    	set flowsheet [lindex $args 0]
		if {[string index $flowsheet 0] == "-"} {
		    set showHelp 1
		}
	    }
	}
	set args [lrange $args 1 end]
    }
    if $showHelp {
    	puts -nonewline {Usage:  tksu [--help] [--version] [--installdir] }
	puts {[--layout file] [flowsheet]}
	exit 0
    }

# Find tksu installation directory.  It should be one of the directories
# listed in auto_path.

    foreach dir $auto_path {
	set app [file join $dir src tksu.tcl]
	if [file exists $app] {
	    set TksuDir $dir
	    break
	}
    }
    if {$TksuDir == ""} {
    	puts "tksu: cannot find installation directory"
	return
    }
    if $showTksuDir {
    	puts $TksuDir
	exit 0
    }
    set BitmapDir	[file join $TksuDir bitmaps]
    set SrcDir		[file join $TksuDir src]
    set SpecPath	[file join $TksuDir specs]
    lappend SpecPath	[file join $TksuDir localspecs]
    set FlowDir		[pwd]
    set DataDir		[pwd]

# If environment variable TKSU_TMPDIR is defined, use it for TempDir.

    if [info exists env(TKSU_TMPDIR)] {
    	set TempDir $env(TKSU_TMPDIR)
    }

# Restore preferences from previous run.

    set LayoutFile $layoutFile
    if {$LayoutFile == ""} {
	if [info exists env(TKSU_LAYOUT)] {
	    set LayoutFile $env(TKSU_LAYOUT)
	} elseif [info exists env(HOME)] {
	    set LayoutFile [file join $env(HOME) .tksu]
	}
    }
    if {$LayoutFile != ""} {
	RestoreLayout $LayoutFile
    }

# Add to SpecPath the directories named in the environment variable
# TKSU_PATH (if it exists).  By defining TKSU_PATH, the user may add custom
# modules to the application.  However, note that the standard modules and
# any custom modules all share the same namespace with respect to module
# names and enumerations.

    if [info exists env(TKSU_PATH)] {
	eval lappend SpecPath [split $env(TKSU_PATH) :]
    }

# For each directory in SpecPath, look for spec (*.spec) files and load them.
# Before scanning a directory, check that it exists and that it has not been
# previously scanned.

    set currentDir [pwd]
    set newPath ""
    foreach dir $SpecPath {
	if {![file exists $dir]} continue
	if {![file isdirectory $dir]} continue

	cd $dir
	set dir [pwd]
	if {![info exists gottenDir($dir)]} {
	    set gottenDir($dir) 1
	    lappend newPath $dir

	    foreach file [readdir .] {
		if {[file extension $file] == ".spec"} {
		    LoadSpecs $file
	    	}
	    }
	}
	cd $currentDir
    }

# Update SpecPath to the edited list of directories.

    set SpecPath $newPath
    unset gottenDir

# Load color preferences.

    array set Color {
	module			#d0e8f0
	selected		#e8f0d0
	shadedForeground	#a0a0a0
	shadedBackground	#d9d9d9
    }
    foreach enum $Enum(enum-color) {
    	set name [lindex $enum 0]
	set value [lindex $enum 1]
	if {$name == "Desc:"} continue
	if [info exists Color($name)] continue
	set Color($name) $value
    }

# Assign fonts to FixFont (fixed width) and VarFont (variable width).
# Preferred fixed width font is:

    set XtermFont -misc-fixed-medium-r-semicondensed-*-*-120-*

# Try preferred fixed font, and fall back to Courier if necessary.

    set preferred LucidaTypewriter
    set fontSize -12
    font create VarFont -family Helvetica -size $fontSize
    font create FixFont -family $preferred -size $fontSize
    if {[string tolower [font actual FixFont -family]] \
     != [string tolower $preferred]} {
	font create FixFont -family courier -size $fontSize
    }

# Create images.

    foreach bitmap {LeftArrow RightArrow UpArrow DownArrow} {
	image create bitmap $bitmap -foreground black -background {} \
	    -file [file join $BitmapDir $bitmap.xbm]
    }
    foreach gif {LogoHT LogoHT32 LogoTcl LogoCwp} {
	image create photo $gif -file [file join $BitmapDir $gif.gif]
    }

# Initialize Undo/Redo stacks.  Raise canvas and read initial flowsheet.

    ResetUndo
    InitCanvas
    update
    if {$flowsheet != ""} {
	OpenCanvas $flowsheet
    }
    if [info exists CurrentCat] {
    	ModListUpdate $CurrentCat
    }
    return
}
