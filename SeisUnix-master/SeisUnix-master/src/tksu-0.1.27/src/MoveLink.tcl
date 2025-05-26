# MoveLink.tcl --
#
#	Tksu procedures to support moving/editing links.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: MoveLink.tcl,v 1.1.1.1 2002/06/07 23:43:30 jeff Exp $

#-------------------------------------------------------------------------------
# MoveLinkStart -- initialize state for MoveLink calls.
#
# Args:     linkid	Id of link (an index into Links array).
# Returns:  null
#
# The routine initializes the MoveLinkState variable, which allows MoveLink
# to be a reversible operation.  It is automatically called by MoveLink at
# the start of a series of movements.  MoveLinkState is a list with the
# following elements:
#
#   {x2 x4 ... xn}	Snapshot of original link x coordinates.
#   {a2 a4 ... an}	Movement weights for x coordinates.
#   {y1 y2 y4 ... yn}	Snapshot of original link y coordinates.
#   {b1 b2 b4 ... bn}	Movement weights for y coordinates.
#   {dx1 dy1 dxn dyn}	Cumulative shift of endpoints since initialization.
#			Initialized to {0 0 0 0}.
#
# A link consists of an even number of points (n).  Endpoint 1 is anchored on
# an output port and endpoint n is anchored on an input port.  The segments
# connecting the points alternate between vertical and horizontal -- no
# obliquely running segments are allowed.  There are n/2 vertical segments
# defined by the x coordinates {x2 x4 ... xn}.  The first and last segments
# are vertical and are constrained to have a minimum directed length of
# Stub(x).  Not counting the two y endpoints {y1 yn}, there are n/2 - 1
# horizontal segments defined by the y coordinates {y2 y4 ... y(n-2)}.
#
# When the endpoints of a link are moved, the intermediate segments are
# moved proportionally.  Movements in x and y are done independently of
# each other.  For movement in x, the vertical segments are moved according
# to the weights {a2 a4 ... an} which are numbers between 0 and 1.  Weights
# a2 = 1 and an = 0.  The rest are assigned proportional to the cumulative
# horizontal link length.  For movement in y, weights b1 = 1 and bn = 0.
#-------------------------------------------------------------------------------

proc tksu::MoveLinkStart linkid {
    variable MoveLinkState

# Fetch horizontal and vertical link coordinates.

    set MoveLinkState($linkid) ""
    set coords [GetLinkCoords $linkid]

# Do x, y segments independently.

    foreach j {0 1} {
	set xcoords [lindex $coords $j]
	lappend MoveLinkState($linkid) $xcoords

	if {[llength $xcoords] == 1} {
	    lappend MoveLinkState($linkid) 1.0
	    continue
	}

# Accumulate link length.

	set lengths ""
	set lengthsum 0.0
	set xprev [lindex $xcoords 0]

	foreach x $xcoords {
	    set lengthsum [expr $lengthsum + abs($x - $xprev)]
	    lappend lengths $lengthsum
	    set xprev $x
	}

# For degenerate case where all x coordinates are the same, arrange for
# lengths to increase uniformly.

	if {$lengthsum <= 1.e-6} {
	    set lengths ""
	    set lengthsum -1.0
	    foreach x $xcoords {
		set lengthsum [expr $lengthsum + 1.0]
	    	lappend lengths $lengthsum
	    }
	}

# Set alpha = normalized lengths, so that alpha1 = 0.0 and alphan = 1.0.

	set inverse [expr 1.0/$lengthsum]
	set alpha ""
	foreach length $lengths {
    	    lappend alpha [expr 1.0 - $length*$inverse]
	}
	lappend MoveLinkState($linkid) $alpha
    }
    lappend MoveLinkState($linkid) {0 0 0 0}
    return
}

#-------------------------------------------------------------------------------
# MoveLink -- move endpoints of link by given increments.
#
# Args:	    linkid	Id of link.
#	    dx1 dy1	Increment to move first link endpoint by (write port).
#	    dxn dyn	Increment to move last link endpoint by (read port).
# Returns:  null
#
# The two endpoints are shifted by the given amounts.  The intermediate
# points of the line are proportionally shifted, depending on the settings
# in the MoveLinkState variable.  If this variable does not exist,
# MoveLinkStart is called to set it up.  It may be reset with a call to
# MoveLinkFinish.
#
# In the course of moving the link, constraints may force new points to be
# added to the link.  The constraints are that the first and last segments
# must be vertical with a minimum positive length of Stub(y).
#-------------------------------------------------------------------------------

proc tksu::MoveLink {linkid dx1 dy1 dxn dyn} {
    variable MoveLinkState
    variable Stub

    if {![info exists MoveLinkState($linkid)]} {
    	MoveLinkStart $linkid
    }
    set xcoords [lindex $MoveLinkState($linkid) 0]
    set alpha   [lindex $MoveLinkState($linkid) 1]
    set ycoords [lindex $MoveLinkState($linkid) 2]
    set beta    [lindex $MoveLinkState($linkid) 3]
    set ends    [lindex $MoveLinkState($linkid) 4]

# Accumulate the shift into endpoints.

    set dx1 [expr $dx1 + [lindex $ends 0]]
    set dy1 [expr $dy1 + [lindex $ends 1]]
    set dxn [expr $dxn + [lindex $ends 2]]
    set dyn [expr $dyn + [lindex $ends 3]]
    set ends "$dx1 $dy1 $dxn $dyn"
    set MoveLinkState($linkid) [lreplace $MoveLinkState($linkid) 4 4 $ends]

# Two-point link:  must make it a four-point link if dx1 != dxn.

    if {[llength $xcoords] == 1 && $dx1 != $dxn} {
	set x2 $xcoords
	set y1 [lindex $ycoords 0]
	set y4 [lindex $ycoords 1]
	set y2 [expr 0.5*($y1 + $y4)]

	set xcoords "$x2 $x2"
	set alpha   "1.0 0.0"
	set ycoords "$y1 $y2 $y4"
	set beta    "1.0 0.5 0.0"
    }

# Calculate new coords from original points and cumulative shift.

    set i 0
    foreach a $alpha {
	set x [lindex $xcoords $i]
	lappend xnew [expr $x + $a*$dx1 + (1.0 - $a)*$dxn]
	incr i
    }

    set i 0
    foreach b $beta {
	set y [lindex $ycoords $i]
	lappend ynew [expr $y + $b*$dy1 + (1.0 - $b)*$dyn]
	incr i
    }

# Adjust end segments so that they are at least Stub(y) long.

    set n [llength $xnew]
    set ymin [expr [lindex $ynew 0] + $Stub(y)]
    set ymax [expr [lindex $ynew $n] - $Stub(y)]

# Four-point link:

    if {$n == 2} {

# If four-point link violates end segment constraints, divide it into a
# six-point link.

    	if {$ymin > $ymax} {
	    set x2 [lindex $xnew 0]
	    set x4 [lindex $xnew 1]
	    set xnew [linsert $xnew 1 [expr 0.5*($x2 + $x4)]]
	    set ynew [lreplace $ynew 1 1 $ymin $ymax]

# Otherwise limit y2 between ymin and ymax.

	} else {
	    set y2 [lindex $ynew 1]
	    if {$y2 < $ymin} {set y2 $ymin}
	    if {$y2 > $ymax} {set y2 $ymax}
	    set ynew [lreplace $ynew 1 1 $y2]
	}

# Six points or more:

    } elseif {$n > 2} {
	incr n -1
	set y2 [lindex $ynew 1]
	set yn [lindex $ynew $n]
	if {$y2 < $ymin} {set ynew [lreplace $ynew 1 1 $ymin]}
	if {$yn > $ymax} {set ynew [lreplace $ynew $n $n $ymax]}
    }
    SetLinkCoords $linkid $xnew $ynew
    return
}

#-------------------------------------------------------------------------------
# MoveLinkFinish -- clear state for MoveLink.
#
# Args:	    linkid	Id of link, or special value `all'.
# Returns:  null
#
# Normally, MoveLinkState($linkid) is unset.  If the special value `all' is
# given in the argument, all elements of MoveLinkState are unset.
#-------------------------------------------------------------------------------

proc tksu::MoveLinkFinish linkid {
    variable MoveLinkState

    if {$linkid == "all"} {
    	foreach linkid [array names MoveLinkState] {
	    unset MoveLinkState($linkid)
	}

    } elseif [info exists MoveLinkState($linkid)] {
    	unset MoveLinkState($linkid)
    }
    return
}

#-------------------------------------------------------------------------------
# DragLinkStart -- initialize dragging of individual link segment.
#
# Args:     linkid	Id of link.
#	    x y		Canvas coordinates to select which link(s) to drag.
# Returns:  {ix iy}	Indices of segments to be dragged.  If ix or iy is 0,
#			indicates that segment (x or y) will not be dragged.
#
# If x,y is along a segment edge, that segment is dragged.  If x,y is at the
# corner of two segments, both will be dragged.  The variable MoveLinkState
# is used to store the DragLink state.  The only difference in usage is that
# element {dx1 dy1 dxn dyn} holds the following information:
#
#   dx1 dy1	Cumulative motion of the mouse since initialization, starts
#		as 0,0.
#   ix iy	Indices into xcoords, ycoords (respectively) of segment that
#		is being dragged.  If ix or iy is 0, no x or y segment is
#		dragged.  The first and last x segments (x2 and xn) cannot be
#		dragged, neither can the endpoints y1 and yn.
#
# The alpha and beta lists in MoveLinkState are not used.
#-------------------------------------------------------------------------------

proc tksu::DragLinkStart {linkid x y} {
    variable MoveLinkState

    set MoveLinkState($linkid) ""
    set coords [GetLinkCoords $linkid]
    set xcoords [lindex $coords 0]
    set ycoords [lindex $coords 1]

# Get closest segments in x and y.

    set ixiy [ClosestSegment $xcoords $ycoords $x $y]
    set ix [lindex $ixiy 0]
    set iy [lindex $ixiy 1]

# Prevent dragging first and last segments of xcoords, ycoords.

    set n [llength $xcoords]
    if {$iy < 1 || $iy >= $n} {set iy 0}
    incr n -1
    if {$ix < 1 || $ix >= $n} {set ix 0}

    set ends "0 0 $ix $iy"
    set MoveLinkState($linkid) [list $xcoords {} $ycoords {} $ends]
    return "$ix $iy"
}

#-------------------------------------------------------------------------------
# DragLink -- drag individual segment of link.
#
# Args:     linkid	Id of link.
#	    dx dy	Amount in x or y to drag segments.
# Returns:  null
#
# See DragLinkStart for details on MoveLinkState usage.
#-------------------------------------------------------------------------------

proc tksu::DragLink {linkid dx dy} {
    variable MoveLinkState
    variable Stub

    if {![info exists MoveLinkState($linkid)]} return

    set xcoords [lindex $MoveLinkState($linkid) 0]
    set ycoords [lindex $MoveLinkState($linkid) 2]
    set ends	[lindex $MoveLinkState($linkid) 4]
    set ix	[lindex $ends 2]
    set iy	[lindex $ends 3]

# Update MoveLinkState.

    set dx [expr $dx + [lindex $ends 0]]
    set dy [expr $dy + [lindex $ends 1]]
    set ends [lreplace $ends 0 1 $dx $dy]
    set MoveLinkState($linkid) [lreplace $MoveLinkState($linkid) 4 4 $ends]

# Move segment.

    if {$ix > 0} {
    	set x [expr $dx + [lindex $xcoords $ix]]
	set xcoords [lreplace $xcoords $ix $ix $x]
    }
    if {$iy > 0} {
    	set y [expr $dy + [lindex $ycoords $iy]]
	set ycoords [lreplace $ycoords $iy $iy $y]
    }

# Adjust end segments so that they are at least Stub(y) long.

    if {$iy == 1} {
	set ymin [expr [lindex $ycoords 0] + $Stub(y)]
	set y [lindex $ycoords 1]
	if {$y < $ymin} {
	    set ycoords [lreplace $ycoords 1 1 $ymin]
	}
    }

    set n [expr [llength $ycoords] - 2]
    if {$iy == $n} {
	incr n
	set ymax [expr [lindex $ycoords $n] - $Stub(y)]
	incr n -1
	set y [lindex $ycoords $n]
	if {$y > $ymax} {
	    set ycoords [lreplace $ycoords $n $n $ymax]
	}
    }
    SetLinkCoords $linkid $xcoords $ycoords
    return
}

#-------------------------------------------------------------------------------
# DragLinkFinish -- clear state for DragLink.
#
# Args:	    linkid	Id of link.
# Returns:  null
#
# Sets undo, redo commands, then erases MoveLinkState.
#-------------------------------------------------------------------------------

proc tksu::DragLinkFinish linkid {
    variable MoveLinkState
    variable Links

    if {![info exists MoveLinkState($linkid)]} return

    set xcoords [lindex $MoveLinkState($linkid) 0]
    set ycoords [lindex $MoveLinkState($linkid) 2]
    set link $Links($linkid)
    set undo "SetLinkCoords \[GetLink $link\] [list $xcoords $ycoords]"
    set redo "SetLinkCoords \[GetLink $link\] [GetLinkCoords $linkid]"
    AddUndo $undo $redo

    unset MoveLinkState($linkid)
    return
}

#-------------------------------------------------------------------------------
# LinkNode -- add to or delete nodes from link.
#
# Args:     linkid	Id of link.
#	    x y		Window (not canvas) coordinates close to a point on
#			the link, used to select a segment to split, or a
#			node (corner) to delete.
# Returns:  null
#
# If x,y is within a threshold of an existing corner of the link, the corner
# will be deleted by removing the horizontal (y) and vertical (x) segments
# comprising that corner.  If either segment cannot be removed, no change
# takes place.  Segments that cannot be removed are the first and last x
# segments (x2 and xn), and the endpoints (y1 and yn).
#
# If x,y is within a threshold of an existing segment, but away from a corner,
# the segment will be divided at that point and two new nodes created.
# Subdivision will not happen on the first and last vertical segments (x2 and
# xn) if within Stub(y) of the link endpoints.  The y endpoints (y1 and yn)
# also are not considered for subdivision.
#-------------------------------------------------------------------------------

proc tksu::LinkNode {linkid x y} {
    variable Stub
    variable Links

# Convert to canvas coordinates.

    set x [.canvas canvasx $x]
    set y [.canvas canvasy $y]

    set coords [GetLinkCoords $linkid]
    set xcoords [lindex $coords 0]
    set ycoords [lindex $coords 1]
    set changed 0

# Get closest segments in x and y.

    set ixiy [ClosestSegment $xcoords $ycoords $x $y]
    set ix [lindex $ixiy 0]
    set iy [lindex $ixiy 1]
    set ny [llength $xcoords];		# Last point in ycoords
    set nx [expr $ny - 1];		# Last point in xcoords

# Remove node at x,y:

    if {$ix >= 0 && $iy >= 0} {

    	if {$ix == 0 || $ix == $nx || $iy == 0 || $iy == $ny} {

	    PushInfo failedNode " This corner cannot be deleted"
	    after 5000 "tksu::PopInfo failedNode"
	    return
	}
	set xcoords [lreplace $xcoords $ix $ix]
	set ycoords [lreplace $ycoords $iy $iy]
	incr ny -1
	incr nx -1

# Disallow removal if resulting first or last vertical segment violates
# the minimum Stub(y) rule.

	if {[lindex $ycoords 1] - [lindex $ycoords 0] < $Stub(y) \
	||  [lindex $ycoords $ny] - [lindex $ycoords $nx] < $Stub(y)} {

	    PushInfo failedNode " This corner cannot be deleted"
	    after 5000 "tksu::PopInfo failedNode"
	    return
	}
	set changed 1
	PushInfo deletedNode " Deleted pipe corner"
	after 5000 "tksu::PopInfo deletedNode"

# Split x segment at y, but not within Stub(y) of endpoints.

    } elseif {$ix >= 0 && $iy < 0} {

	if {$ix == 0} {
	    if {$y < [expr [lindex $ycoords 0] + $Stub(y)]} return
	}
	if {$ix == $nx} {
	    if {$y > [expr [lindex $ycoords $ny] - $Stub(y)]} return
	}
	set ixp [expr $ix + 1]
	set xcoords [linsert $xcoords $ix [lindex $xcoords $ix]]
	set ycoords [linsert $ycoords $ixp $y]
	set changed 1

	PushInfo splitVertical " Vertical segment split at cursor"
	after 5000 "tksu::PopInfo splitVertical"

# Split y segment at x:

    } elseif {$ix < 0 && $iy > 0 && $iy < $ny} {

	set xcoords [linsert $xcoords $iy $x]
	set ycoords [linsert $ycoords $iy [lindex $ycoords $iy]]
	set changed 1

	PushInfo splitHorizontal " Horizontal segment split at cursor"
	after 5000 "tksu::PopInfo splitHorizontal"
    }
    if {$changed} {
	SetLinkCoords $linkid $xcoords $ycoords
	ExpandCanvas

	set link $Links($linkid)
	set undo "SetLinkCoords \[GetLink $link\] $coords"
	set redo "SetLinkCoords \[GetLink $link\] [list $xcoords $ycoords]"
	AddUndo $undo $redo
	AddMark
    }
    return
}

#-------------------------------------------------------------------------------
# ClosestSegment -- return x or y segment in link that is closest to x,y.
#
# Args:     xcoords	X segment coordinates.
#	    ycoords	Y segment coordinates.
#	    x y		Reference coordinate.
# Returns:  {ix iy}	Indices in xcoords and ycoords (may be -1 if none).
#
# The point x,y is tested against each x and y segment in turn.  If the
# point is within a threshold distance of the segment, the segment is a
# candidate to be returned.  Both an x and y segment index may be returned if
# the x,y point is near a corner.  The indices of the closest segments are
# returned.  A return index of -1 indicates no segment was within the
# threshold distance.
#-------------------------------------------------------------------------------

proc tksu::ClosestSegment {xcoords ycoords x y} {
    variable Threshold

    set nx [llength $xcoords]
    set ix -1
    set iy -1
    set jx 0
    set jy 0
    set x1 [lindex $xcoords 0]
    set y1 [lindex $ycoords 0]

    while 1 {

# X (vertical) segment:

	set x0 $x1
	set y0 $y1
	incr jy
	set y1 [lindex $ycoords $jy]
	set dx [expr abs($x - $x0)]

	if {$y0 < $y1} {
	    set ymin [expr $y0 - $Threshold]
	    set ymax [expr $y1 + $Threshold]
	} else {
	    set ymin [expr $y1 - $Threshold]
	    set ymax [expr $y0 + $Threshold]
	}
	if {$dx <= $Threshold && $y >= $ymin && $y <= $ymax} {
	    if {$ix < 0 || $dx < $dxmin} {
		set ix $jx
		set dxmin $dx
	    }
	}

# Y (horizontal) segment:

	incr jx
	if {$jx == $nx} break
	set x1 [lindex $xcoords $jx]
	set dy [expr abs($y - $y1)]

	if {$x0 < $x1} {
	    set xmin [expr $x0 - $Threshold]
	    set xmax [expr $x1 + $Threshold]
	} else {
	    set xmin [expr $x1 - $Threshold]
	    set xmax [expr $x0 + $Threshold]
	}
	if {$dy <= $Threshold && $x >= $xmin && $x <= $xmax} {
	    if {$iy < 0 || $dy < $dymin} {
		set iy $jy
		set dymin $dy
	    }
	}
    }
    return "$ix $iy"
}

#-------------------------------------------------------------------------------
# GetLinkCoords -- get x or y segment coordinates of given link.
#
# Args:     linkid	Id of link.
# Returns:  lists	Two lists:  x coordinates {x2 x4 ... xn} followed by
#			y coordinates {y1 y2 y4 ... yn}.  See MoveLinkStart
#			for a description of the lists.
#-------------------------------------------------------------------------------

proc tksu::GetLinkCoords linkid {

    set coords [.canvas coords $linkid]
    set n [llength $coords]

# xcoords:

    set i 2
    while {$i < $n} {
	lappend xcoords [lindex $coords $i]
	incr i 4
    }

# ycoords:

    lappend ycoords [lindex $coords 1]
    set i 3
    while {$i < $n} {
    	lappend ycoords [lindex $coords $i]
	incr i 4
    }
    return [list $xcoords $ycoords]
}

#-------------------------------------------------------------------------------
# SetLinkCoords -- update canvas coordinates of given link.
#
# Args:     linkid	Id of link.
#	    xcoords	X coordinates {x2 x4 ... xn}.
#	    ycoords	Y coordinates {y1 y2 y4 ... yn}, in the same format
#			as returned from GetLinkCoords.
# Returns:  null
#
# The individual line members making up the link share $linkid as a tag.
# The format for linkid is `link:id1:id2' where id1 is the lower line and
# id2 is the upper line.
#-------------------------------------------------------------------------------

proc tksu::SetLinkCoords {linkid xcoords ycoords} {

    set nx [llength $xcoords]
    set ix 0
    set iy 0
    while {$ix < $nx} {
    	lappend coords [lindex $xcoords $ix]
	lappend coords [lindex $ycoords $iy]
	incr iy
    	lappend coords [lindex $xcoords $ix]
	lappend coords [lindex $ycoords $iy]
	incr ix
    }

    foreach id [lrange [split $linkid :] 1 2] {
	eval .canvas coords $id $coords
    }
    return
}
