# ModList.tcl --
#
#	Tksu procedures for displaying the module selection list.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: ModList.tcl,v 1.2 2002/06/21 02:52:20 jeff Exp $

#-------------------------------------------------------------------------------
# ModListCreate -- create module list window.
#
# Args:     none
# Returns:  null
#
# A single instance of the module list window is supported.  It should not
# be destroyed until the application exits.
#-------------------------------------------------------------------------------

proc tksu::ModListCreate {} {
    variable Color
    variable CurrentMod
    variable CurrentCat

    if [winfo exists .modlist] {
	RaiseWindow .modlist
	return
    }
    toplevel .modlist
    wm title .modlist "Module List"
    wm protocol .modlist WM_DELETE_WINDOW tksu::ModListHide

    if ![info exists CurrentCat] {set CurrentCat ""}
    if ![info exists CurrentMod] {set CurrentMod ""}

# Button bar at top:

    set f .modlist.buttons
    frame $f
    label $f.label -text "Selected module:" -underline 9 -width 16 -anchor e
    entry $f.entry -relief sunken -bg white -width 20
    button $f.add -text Add -underline 0 -width 5 \
    		  -command "tksu::NoRepeat Press ModListAdd"
    button $f.param -text Param -underline 0 -width 5 \
    		    -command tksu::ModListParam
    button $f.help -text Help -underline 0 -width 5 \
    		   -command tksu::ModListHelp
    button $f.close -text Close -underline 0 -width 5 \
    		    -command tksu::ModListHide
    pack $f.label $f.entry $f.add $f.param -side left
    pack $f.close $f.help -side right
    pack $f -side top -fill x

# Search button bar:

    set f .modlist.search
    frame $f
    label $f.label -text "Search for:" -underline 0 -width 16 -anchor e
    entry $f.entry -relief sunken -bg white -width 20
    button $f.next -text Next -underline 0 -width 5 \
    		   -command "tksu::Search next"
    button $f.prev -text Prev -underline 1 -width 5 \
    		   -command "tksu::Search prev"
    pack $f.label $f.entry $f.next $f.prev -side left
    pack $f -side top -fill x

# Category list and scroll bar:

    set f .modlist.cat
    frame $f
    label $f.label -text "Categories"
    listbox $f.list -yscrollcommand "$f.scroll set" -width 18 -bg white \
    		    -exportselection 0 -selectbackground $Color(selected)
    scrollbar $f.scroll -command "$f.list yview"
    pack $f.label -side top -anchor w
    pack $f.scroll -side right -fill y
    pack $f.list -fill y -expand 1
    pack $f -side left -fill y

# Module list and scroll bar:

    set f .modlist.mod
    frame $f
    label $f.label -text "Modules"
    listbox $f.list -yscrollcommand "$f.scroll set" -width 80 \
    		    -bg white -font FixFont -exportselection 0 \
		    -selectbackground $Color(selected)
    scrollbar $f.scroll -command "$f.list yview" -takefocus 0
    pack $f.label -side top -anchor w
    pack $f.scroll -side right -fill y
    pack $f.list -fill both -expand 1
    pack $f -fill both -expand 1

# Listbox bindings:

    set taglist [bindtags .modlist.cat.list]
    lappend taglist catTag
    bindtags .modlist.cat.list $taglist

    set taglist [bindtags .modlist.mod.list]
    lappend taglist modTag
    bindtags .modlist.mod.list $taglist

    bind catTag <ButtonRelease-1>  {tksu::ModListUpdate [tksu::GetCat] 0}
    bind catTag <KeyRelease-Up>	   {tksu::ModListUpdate [tksu::GetCat] 0}
    bind catTag <KeyRelease-Down>  {tksu::ModListUpdate [tksu::GetCat] 0}
    bind catTag <KeyRelease-space> {tksu::ModListUpdate [tksu::GetCat] 0}

    bind modTag <ButtonRelease-1>  {tksu::ModSelect [tksu::GetMod]}
    bind modTag <KeyRelease-Up>	   {tksu::ModSelect [tksu::GetMod]}
    bind modTag <KeyRelease-Down>  {tksu::ModSelect [tksu::GetMod]}
    bind modTag <KeyRelease-space> {tksu::ModSelect [tksu::GetMod]}

# Accelerator bindings:

    bind .modlist <Alt-a> "tksu::NoRepeat %K ModListAdd"
    bind .modlist <Alt-h>  tksu::ModListHelp
    bind .modlist <Alt-p>  tksu::ModListParam
    bind .modlist <Alt-c>  tksu::ModListHide
    bind .modlist <Escape> tksu::ModListHide
    bind .modlist <Alt-m> "focus .modlist.buttons.entry"
    bind .modlist <Alt-s> "focus .modlist.search.entry"
    bind .modlist <Alt-n> "tksu::Search next"
    bind .modlist <Alt-r> "tksu::Search prev"
    bind .modlist.search.entry <Return> "tksu::Search next"
    bind .modlist.search.entry <Shift-Return> "tksu::Search prev"
    bind .modlist.buttons.entry <Return> "tksu::NoRepeat %K ModListAdd"

    set f .modlist.mod.list
    bind .modlist <KeyRelease-Up>    "tksu::MoveSelection $f -1; \
    				      tksu::ModSelect \[tksu::GetMod\]"
    bind .modlist <KeyRelease-Down>  "tksu::MoveSelection $f 1; \
    				      tksu::ModSelect \[tksu::GetMod\]"
    bind .modlist <KeyRelease-Prior> "tksu::MoveSelection $f -10; \
    				      tksu::ModSelect \[tksu::GetMod\]"
    bind .modlist <KeyRelease-Next>  "tksu::MoveSelection $f 10; \
    				      tksu::ModSelect \[tksu::GetMod\]"

# Forward traversal with <Tab> is already on by default.
# Establish reverse traversal by binding to Shift-Tab.

    set f .modlist.buttons
    set tabList "$f.entry $f.add $f.param $f.help $f.close"
    set f .modlist.search
    lappend tabList $f.entry $f.next $f.prev
    lappend tabList .modlist.cat.list .modlist.cat.scroll
    lappend tabList .modlist.mod.list .modlist.mod.scroll

#   bind .modlist <Tab> "tksu::TabForward [list $tabList]"
    bind .modlist <ISO_Left_Tab> "tksu::TabReverse [list $tabList]"
    focus .modlist.buttons.entry

# Set incremental search bindings.

    IncSearchBind
    AddDragBind
    RaiseWindow .modlist
    return
}

#-------------------------------------------------------------------------------
# TabForward -- traverse widgets in forward order.
#
# Args:     list	List of widgets.  They will be traversed in the order
#			given in the list.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::TabForward list {

    set i [lsearch -exact $list [focus]]
    if {$i < 0} {
    	focus [lindex $list 0]
	return
    }
    for {set j [expr $i + 1]} {$j != $i} {incr j} {
    	if {$j >= [llength $list]} {
	    set j 0
	}
	set w [lindex $list $j]
	if [tkFocusOK $w] {
	    focus $w
	    return
	}
    }
    return
}

#-------------------------------------------------------------------------------
# TabReverse -- traverse widgets in reverse order.
#
# Args:     list	List of widgets.  They will be traversed in the order
#			opposite to their order in the list.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::TabReverse list {

    set i [lsearch -exact $list [focus]]
    if {$i < 0} {
    	focus [lindex $list 0]
	return
    }
    for {set j [expr $i - 1]} {$j != $i} {incr j -1} {
    	if {$j < 0} {
	    set j [expr [llength $list] - 1]
	}
	set w [lindex $list $j]
	if [tkFocusOK $w] {
	    focus $w
	    return
	}
    }
    return
}

#-------------------------------------------------------------------------------
# ModListAdd -- create new instance of current module, add to canvas.
#
# Args:     args	Optional arguments passed directly to AddModule.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ModListAdd args {
    variable CurrentMod
    variable Desc

    set modbase [ModBase $CurrentMod]
    if {$CurrentMod == ""} {
    	return
    }
    if {![info exists Desc($modbase)]} {
    	tk_messageBox -icon error -parent .modlist -type ok \
		      -title "Nonexistent module" \
		      -message "Module `$modbase' does not exist."
	return
    }
    if {$CurrentMod != $modbase} {
    	tk_messageBox -icon error -parent .modlist -type ok \
		      -title "Instance number present" \
		      -message "If module `$modbase' is to be added to the\
		      	        canvas, leave off the instance number."
	return
    }
    set module [eval AddModule $modbase $args]
    if {$module == ""} {
    	tk_messageBox -icon error -parent .modlist -type ok \
		      -title "Bad module spec" \
		      -message "Module `$modbase' is missing a Port or\
		      		a Desc line in its specification.  Correct\
				the entry for $modbase in its spec file."
    	return
    }
    UndoModule $module add
    AddMark
    return
}

#-------------------------------------------------------------------------------
# ModListHelp -- bring up help window for current module.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ModListHelp {} {
    variable CurrentMod
    Help $CurrentMod
    return
}

#-------------------------------------------------------------------------------
# ModListParam -- bring up parameter window for current module.
#
# Args:     none
# Returns:  null
#
# It is OK for CurrentMod to name an instantiated module.
#-------------------------------------------------------------------------------

proc tksu::ModListParam {} {
    variable CurrentMod
    variable Desc
    variable Instances
    variable Ports
    variable Params

    set modbase [ModBase $CurrentMod]
    if {$CurrentMod == ""} {
    	return

    } elseif {![info exists Desc($modbase)]} {
    	tk_messageBox -icon error -parent .modlist -type ok \
		      -title "Nonexistent module" \
		      -message "Module `$modbase' does not exist."

    } elseif {$CurrentMod != $modbase \
    && ![info exists Instances($CurrentMod)]} {
    	tk_messageBox -icon error -parent .modlist -type ok \
		      -title "Nonexistent instance" \
		      -message "Module instance `$CurrentMod' does not exist."
    } else {
	set nport 0
	if {[info exists Ports($modbase)] \
	&&  [info exists Params($modbase)]} {
	    set nport [llength $Ports($modbase)]
	}
	ParamListParam $CurrentMod $nport
    }
    return
}

#-------------------------------------------------------------------------------
# ModListShow -- raise module list window.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ModListShow {} {
    RaiseWindow .modlist
    return
}

#-------------------------------------------------------------------------------
# ModListHide -- hide module list window.
#
# Args:     none
# Returns:  null
#
# The module list window is not destroyed.  It can be raised again with a call
# to ModListShow.
#-------------------------------------------------------------------------------

proc tksu::ModListHide {} {
    HideWindow .modlist
    return
}

#-------------------------------------------------------------------------------
# CatListUpdate -- update/create contents of category list.
#
# Args:     none
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::CatListUpdate {} {
    variable Members
    variable ActiveMember

    set f .modlist.cat.list
    set catlist [lsort [array names Members]]
    lappend catlist "All Modules"
    $f delete 0 end
    foreach line $catlist {
    	$f insert end $line
	set ActiveMember($line) 0
    }
    return
}

#-------------------------------------------------------------------------------
# ModListUpdate -- update/create contents of module list.
#
# Args:     category	Show modules belonging to this category.  An
#			alphabetical index of all modules is displayed if the
#			special category "All Modules" is given.
#	    raise	Optional boolean value, specifies if module list
#			window should be deiconified and raised.  Default
#			value is 1.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ModListUpdate {category {raise 1}} {
    variable Desc
    variable Members
    variable ActiveMember
    variable CurrentCat
    variable CurrentMod

# Create module list window if necessary.
# Create category list if necessary.

    if {$raise || ![winfo exists .modlist]} {
	ModListCreate
    }
    set f .modlist.cat.list
    if {[$f index end] == 0} CatListUpdate
    set catlist [$f get 0 end]

# Save active member from previous module list in ActiveMember.

    set icat [lsearch -exact $catlist $CurrentCat]
    if {$icat >= 0} {
    	set ActiveMember($CurrentCat) [.modlist.mod.list index active]
    }

# Update category selection.

    set CurrentCat $category
    $f selection clear 0 end
    set icat [lsearch -exact $catlist $CurrentCat]
    if {$icat >= 0} {
	SetSelection $f $icat
    }

# Update module list title.

    set title $CurrentCat
    set n [expr [llength $title] - 1]
    if {[lindex $title $n] != "Modules"} {
    	lappend title "Modules"
    }
    .modlist.mod.label configure -text [join $title]

# Create module list.

    set f .modlist.mod.list
    $f delete 0 end
    if {$icat >= 0} {
	if {$CurrentCat == "All Modules"} {
	    set modlist [lsort [array names Desc]]
	} else {
    	    set modlist [lsort $Members($CurrentCat)]
	}
	foreach mod $modlist {
	    set desc [join $Desc($mod)]
	    $f insert end [format " %-14s %s" $mod $desc]
	}

# Restore active member for new module list.

	set i $ActiveMember($CurrentCat)
	SetSelection $f $i
	ModSelect $CurrentMod
    }
    return
}

#-------------------------------------------------------------------------------
# ModSelect -- update current module selection.
#
# Args:     module	Name of module to be the new CurrentMod.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ModSelect {module} {
    variable CurrentMod

    set CurrentMod $module

# Activate selected entry in module list (if present).

    set f .modlist.mod.list
    $f selection clear 0 end
    set n [$f index end]
    for {set i 0} {$i < $n} {incr i 1} {
	set moddesc [$f get $i]
	if {[lindex $moddesc 0] == $module} {
	    SetSelection $f $i
	    break
	}
    }

# Place module name in entry window.

    set f .modlist.buttons.entry
    if {$module != [$f get]} {
	$f delete 0 end
	$f insert 0 $module
    }
    return
}

#-------------------------------------------------------------------------------
# GetCat -- return active category.
#
# Args:     none
# Returns:  string	Currently active category in the category list.
#-------------------------------------------------------------------------------

proc tksu::GetCat {} {

    set default "All Modules"
    set f .modlist.cat.list
    if {![winfo exists $f]} {return $default}

    set i [$f index active]
    return [$f get $i]
}

#-------------------------------------------------------------------------------
# GetMod -- return active module.
#
# Args:     none
# Returns:  string	Currently active module in the module list.  Only the
#			module name is returned, not the description.
#-------------------------------------------------------------------------------

proc tksu::GetMod {} {

    set f .modlist.mod.list
    set i [$f index active]
    return [lindex [$f get $i] 0]
}

#-------------------------------------------------------------------------------
# AddDragBind -- set up special drag binding to Add button.
#
# Args:     none
# Returns:  null
#
# Desired behavior for Add button:  while the mouse button remains pressed,
# cursor changes to `add' cursor, which may be dragged into the canvas window.
# If the mouse button is released in the canvas window, the new module will
# be placed at that point.  If the mouse button is released in the Add button,
# the normal button command is invoked.  Otherwise if the mouse button is
# released anywhere else, nothing happens.  We rely here on Tk establishing
# a global grab on the Add button when the mouse button is pressed.
#-------------------------------------------------------------------------------

proc tksu::AddDragBind {} {
    variable SavedCursor
    variable CurrentMod

    set f .modlist.buttons.add
    set taglist [bindtags $f]
    lappend taglist dragTag
    bindtags $f $taglist

# Mouse button press:  save old cursor, change to new cursor.  Set up a
# one-shot binding for canvas <Enter> event.  It should apply only while the
# drag is active.

    bind dragTag <ButtonPress> [namespace code {
	set f .modlist.buttons.add
	set SavedCursor [$f cget -cursor]
	$f configure -cursor {icon black}

	bind .canvas <Enter> [namespace code {
	    bind .canvas <Enter> {}
	    set x [.canvas canvasx %%x]; # Must escape %x
	    set y [.canvas canvasy %%y]; # Must escape %y
	    ModListAdd $x $y
	}]
    }]

# Mouse button release:  restore old cursor.  Wait a bit before disabling the
# canvas binding, in order to allow the <Enter> event to be processed if the
# button is released inside the canvas.

    bind dragTag <ButtonRelease> [namespace code {
	variable SavedCursor
	set f .modlist.buttons.add
	$f configure -cursor $SavedCursor
	after 500 [list bind .canvas <Enter> {}]
    }]
    return
}

#-------------------------------------------------------------------------------
# NoRepeat -- prevent multiple calls to procedure from repeating key.
#
# Args:     keysym	Keysym of key that shouldn't repeat when held down.
#	    command	Procedure that is to be invoked with key press.
# Returns:  null
#
# Holding the accelerator key down on some buttons creates a big mess because
# of multiple calls to the command (e.g. ModListAdd).  Slow down the repeat
# rate by setting variable IgnoreKey to the keysym that should be ignored.
#-------------------------------------------------------------------------------

proc tksu::NoRepeat {keysym command} {
    variable IgnoreKey

    if [info exists IgnoreKey($keysym)] return
    eval $command
    set IgnoreKey($keysym) 1
    after 500 "unset tksu::IgnoreKey($keysym)"
    return
}
