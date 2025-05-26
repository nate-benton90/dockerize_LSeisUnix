# ModalList.tcl --
#
#	Tksu procedure to make a modal selection from popup list.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: ModalList.tcl,v 1.1.1.1 2002/06/07 23:43:30 jeff Exp $

#-------------------------------------------------------------------------------
# ModalList -- make selection from a list of values.
#
# Args:     entry	Entry widget that the list is to attach to.
#	    list	List to be displayed in a listbox.
# Returns:  null
#
# A listbox and scrollbar are packed into a frame, which is then attached to
# the given entry widget by the placer.  The entry's current contents define
# the default selected element in the list.  The list grabs the keyboard and
# the mouse, and is retired when any of the following events is received:
# <Tab>, <Shift-Tab>, <Return>, <Esc> or <ButtonRelease>.  The new selected
# value is then loaded into the entry widget.  The focus is restored to the
# window that originally had it.
#-------------------------------------------------------------------------------

proc tksu::ModalList {entry list} {
    variable ModalListDone
    variable Color

    if {$list == ""} return

# Use the same font and width of entry widget.

    set font [$entry cget -font]
    set width [$entry cget -width]

# Set number of lines in listbox.

    set height [llength $list]
    if {$height > 6} {set height 6}

# Restore the entry widget's state, normal or disabled, upon exit.

    set oldFocus [focus]
    set entryState [$entry cget -state]
    $entry configure -state normal

# Create the frame off the toplevel of the entry.

    set toplevel [winfo toplevel $entry]
    if {$toplevel == "."} {
    	set w .modal
    } else {
    	set w $toplevel.modal
    }
    frame $w -relief raised

# Listbox and scroll bar:

    listbox $w.list -font $font -bg white -height $height -width $width \
    		    -takefocus 0 -highlightthickness 0 -exportselection 0 \
		    -selectbackground $Color(selected) \
		    -yscrollcommand "$w.scroll set"
    scrollbar $w.scroll -takefocus 0 -command "$w.list yview"
    pack $w.scroll -side right -fill y
    pack $w.list -fill both -expand 1

# Window bindings:

    bind $w.list <Tab>		 "set tksu::ModalListDone Accept"
    bind $w.list <ISO_Left_Tab>	 "set tksu::ModalListDone Accept"
    bind $w.list <Return>	 "set tksu::ModalListDone Accept"
    bind $w.list <space>	 "set tksu::ModalListDone Accept"
    bind $w.list <ButtonRelease> "set tksu::ModalListDone Accept"
    bind $w.list <Escape>	 "set tksu::ModalListDone Ignore"
    bind $w <ButtonRelease>	 "set tksu::ModalListDone Ignore"

# It is important that all keyboard events stop at the class level and do
# not get delivered to the top level window.  Insert a tag after the class
# tag to intercept such events.

    set tags [bindtags $w.list]
    bindtags $w.list [linsert $tags 2 tagModal]
    bind tagModal <KeyPress>	"break"
    bind tagModal <KeyRelease>	"break"

# Fill the listbox.

    $w.list delete 0 end
    eval $w.list insert 0 $list

    set default [string trim [$entry get]]
    set index [lsearch -exact $list $default]
    if {$index < 0} {set index 0}

    $w.list selection clear 0 end
    $w.list selection set $index
    $w.list activate $index
    $w.list see $index

# Raise the frame, using the placer geometry manager.  Make it modal.

    place $w -in $entry -relx 0.0 -rely 1.0 -anchor nw -bordermode outside
    grab set $w
    focus $w.list

# Wait for ModalListDone to signal end.

    set ModalListDone Ignore
    tkwait variable tksu::ModalListDone

# Store new selected entry in entry widget.

    if {$ModalListDone != "Ignore"} {
	set index [$w.list curselection]
	if {$index < 0} {set index 0}
	set value [$w.list get $index]
	$entry delete 0 end
	$entry insert 0 $value
    }

# Retire frame.

    grab release $w
    place forget $w
    destroy $w

# Restore entry widget state and focus.

    $entry configure -state $entryState
    focus $oldFocus
    return
}
