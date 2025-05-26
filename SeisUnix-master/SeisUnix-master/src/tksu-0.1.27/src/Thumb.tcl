# Thumb.tcl --
#
#	Procedures to support a thumbwheel composite widget.  The thumbwheel
#	allows the user to set an arbitrary-precision integer or float value
#	with just mouse actions.
#
# Copyright (C) 2002 Henry Thorson Consulting.  All rights reserved.
#
# This program is licensed under the terms of the GNU General Public License,
# version 2 (or any later version).  See the file `COPYING' for full details,
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# CVS: $Id: Thumb.tcl,v 1.2 2002/06/08 04:19:04 jeff Exp $

#-------------------------------------------------------------------------------
# CreateThumb -- create frame containing thumbwheel components.
#
# Args:     w		Desired thumbwheel frame name.
#	    digits	Requested number of digits in the mantissa of the
#			thumbwheel.  This does not count the characters
#			making up the sign, decimal point, or exponent of the
#			number.
#	    command	Command to invoke when a thumbwheel value is to be
#			delivered to the application.
# Returns:  digits	Returns the actual number of digits assigned to the
#			mantissa (between 2 and 12).  An IEEE double precision
#			floating-point value can store no more than 12
#			decimal digits in its mantissa.
#
# The thumbwheel frame contains the following single-character cell (button)
# widgets in order from left to right:
#
#   sign	Contains `+' or `-', the sign of the number.
#   d1 ... dn	One cell for each digit in the number.
#   point	Contains the decimal point.  The decimal point's position may
#		float among the digit boxes, depending on the type of number
#		displayed.  For an integer, it is to the right of all digits
#		and shaded.  For a fixed-precision float, it may be in any
#		position among the digits.  For a float in exponential
#		notation, it lies to the right of d1.
#   exp		Contains `E' to signify start of exponent.  For an integer,
#		it and the other exponent characters are shaded out.
#   esign	Contains the exponent sign `+' or `-'.
#   e1 e2	Labels displaying two exponent digits.
#
# The signs may be toggled with a mouse click.  The digits may be "spun" by
# dragging them in a vertical direction.  More than one digit is spun at once.
# The farther to the left that the drag starts, the greater the spin rate
# (essentially on a logarithmic scale).  The number formats supported are:
# `d' (decimal), `e' (exponential), `f' (fixed float).  In the `f' format,
# the decimal point may be dragged horizontally to a new location, thus
# changing the number of significant digits to the right of the decimal point.
#-------------------------------------------------------------------------------

proc tksu::CreateThumb {w digits command} {
    variable ThumbState

    if {$digits < 2} {
    	set digits 2
    } elseif {$digits > 12} {
    	set digits 12
    }
    set n [expr $digits + 6]

    frame $w
    for {set i 0} {$i < $n} {incr i} {

	set cell $w.t$i
	button $cell \
	    -padx 2 -width 1 -borderwidth 2 -highlightthickness 0 \
	    -relief sunken -takefocus 0
	ColorThumb $w $i disabled
	pack $cell -side left -padx 0 -pady 0 -ipadx 0 -ipady 0

	bind $cell <ButtonPress>   "tksu::DragThumbStart $w $i %X %Y"
	bind $cell <Motion>	   "tksu::DragThumbMotion $w %X %Y"
	bind $cell <ButtonRelease> "tksu::DragThumbFinish $w %X %Y"
    }

    keylset ThumbState($w) digits $digits command $command state disabled
    return $digits
}

#-------------------------------------------------------------------------------
# InitThumb -- initialize thumbwheel for a given parameter.
#
# Args:     w		Thumbwheel frame widget containing the components.
#	    range	Range string in nlist format.
#	    value	Initial value to load in thumbwheel.  If it is null
#			or garbage, an initial value is automatically picked.
#	    type	Value type: `int' or `float'.  If any other type is
#			given, the entire thumbwheel is disabled.
# Returns:  null
#
# Variable ThumbState($w) keeps track of the state of the thumbwheel $w.
# Keyed items in ThumbState are:
#
#   digits	Number of digits in mantissa.
#   rdigits	Number of digits to the right of the decimal point.  This
#		controls the position of the decimal point.  For an integer,
#		rdigits = 0.  For an exponential format, rdigits = digits-1.
#   ftype	Type of format being displayed: `d' for decimal, `e' for
#		exponential, and `f' for fixed-point float.
#   min		Minimum range value, or null if no minimum.
#   max		Maximum range value, or null if no maximum.
#   attained	Boolean indicating if value is allowed to  attain the
#		minimum or maximum.
#   state	State of thumbwheel:  `normal' or `disabled'.
#   command	Stores callback command.
#
#-------------------------------------------------------------------------------

proc tksu::InitThumb {w range value type} {
    variable ThumbState

    if {$type != "int" && $type != "float"} {
	DisableThumb $w
	return
    }
    keylget ThumbState($w) digits digits

    set crange   [CanonizeRange $range $type $digits]
    set min	 [lindex $crange 0]
    set max	 [lindex $crange 1]
    set attained [lindex $crange 2]

    set value [CanonizeValue $value $type $digits]

# If value is missing, pick it from min or max.

    if {$value == ""} {
    	if {$min != ""} {
	    set value $min
	} elseif {$max != ""} {
	    set value $max
	} else {
	    set value 0
	}
	set value [CanonizeValue $value $type $digits]
    }

# Limit value to lie between min and max.  If attained is 0, move value
# slightly away from the min or max value by calling BumpValue.  If min and
# max are so close together that no value can be set between them, give up
# and disable the thumbwheel.  This also happens if min is greater than max.
# Fix these cases by correcting the erroneous range entry in the spec file.

    if {$min != "" && $value <= $min} {
    	set value [CanonizeValue $min $type $digits]
	if {!$attained} {
	    set value [BumpValue up $value]
	}
    }
    if {$max != "" && $value >= $max} {
    	set value [CanonizeValue $max $type $digits]
	if {!$attained} {
	    set value [BumpValue down $value]
	}
    }
    if {$min != "" && $value <= $min} {
    	if {$value < $min || (!$attained && $value == $min)} {
    	    DisableThumb $w
	    return
    	}
    }

# Got a loadable value.

    keylset ThumbState($w) \
    	rdigits	 [CanonRight $value] \
	ftype	 [CanonType  $value] \
    	min	 $min \
	max	 $max \
	attained $attained \
	state	 normal

    LoadThumb $w $value
    return
}

#-------------------------------------------------------------------------------
# LoadThumb -- load thumbwheel with given canonical value.
#
# Args:     w		Thumbwheel frame widget.
#	    value	A value returned from CanonValue.  The thumbwheel
#			will not operate correctly unless the value loaded is
#			in this special "canonical" format.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::LoadThumb {w value} {
    variable ThumbState

    set state	[keylget ThumbState($w) state]
    set ftype   [keylget ThumbState($w) ftype]
    set digits  [keylget ThumbState($w) digits]
    set rdigits [keylget ThumbState($w) rdigits]
    set ldigits [expr $digits - $rdigits]

    if {$state == "disabled" || $value == ""} {
    	return
    }
    switch -- $ftype {
    	d {
	    regexp {( *)([+-])([0-9]+)} $value dummy lzeros sign lnons
	    regsub -all { } $lzeros 0 lzeros
	    set value $sign$lzeros$lnons.E+00
	}
	f {
	    append value E+00
	}
    }

# Form shade string that determines which digits/components are to be shaded
# out.  Generally, leading zeros to the left, and trailing zeros to the right
# of the decimal point are shaded.  The exponent is shaded when ftype = d or f,
# and so on.

    set normal   00000000000000000000
    set shaded   11111111111111111111
    set disabled 22222222222222222222

    regexp {(0*)[0-9]+\.} $value dummy lzeros
    set lzeros [string length $lzeros]
    set lnons [expr $ldigits - $lzeros]

    if {[string index $value 0] == "-"} {
	set shade 0;					# negative sign
    } else {
    	set shade 1;					# positive sign shaded
    }
    append shade [string range $shaded 1 $lzeros];	# leading zeros
    append shade [string range $normal 1 $lnons];	# nonzeros

    if {$ftype == "d"} {
    	append shade [string range $disabled 1 5];	# remainder of value

    } else {
    	regexp {(0*)E} $value dummy rzeros
	set rzeros [string length $rzeros]
	if {$rzeros == $rdigits} {
	    incr rzeros -1
	}
	set rnons [expr $rdigits - $rzeros]

	append shade [string range $normal 0 $rnons];	# decimal & nonzeros
	append shade [string range $shaded 1 $rzeros];	# trailing zeros

	if {$ftype == "e"} {
	    append shade [string range $normal 1 4];	# exponent for e
	} else {
	    append shade [string range $shaded 1 1];	# exponent for f
	    append shade [string range $disabled 1 3];
	}
    }
    keylset ThumbState($w) shade $shade

# Load each cell box:

    set n [string length $value]
    for {set i 0} {$i < $n} {incr i} {

	switch -- [string range $shade $i $i] {
	    0 {set cellState normal}
	    1 {set cellState shaded}
	    2 {set cellState disabled}
	}
	ColorThumb $w $i $cellState
	set digit [string range $value $i $i]
	$w.t$i configure -text $digit
    }
    return
}

#-------------------------------------------------------------------------------
# ColorThumb -- set colors for given digit of thumbwheel.
#
# Args:     w		Thumbwheel frame widget.
#	    idigit	Index of the thumbwheel digit to set.
#	    state	New state of digit: `normal', `shaded' or `disabled'.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::ColorThumb {w idigit state} {
    variable Color

    switch -- $state {
	normal {
	    set fg black
	    set bg white
	    set activefg black
	    set activebg $Color(selected)
	}
	shaded {
	    set fg $Color(shadedForeground)
	    set bg $Color(shadedBackground)
	    set activefg black
	    set activebg $Color(selected)
	}
    	disabled {
	    set fg $Color(shadedForeground)
	    set bg $Color(shadedBackground)
	    set activefg $Color(shadedForeground)
	    set activebg $Color(shadedBackground)
	}
    }
    $w.t$idigit configure -foreground $fg -background $bg \
    	-activeforeground $activefg -activebackground $activebg
    return
}

#-------------------------------------------------------------------------------
# GetThumbValue -- return value from thumbwheel.
#
# Args:     w		Thumbwheel frame widget.
# Returns:  value	Value of thumbwheel suitable for an entry widget.
#			The value is not in canonical form, since the shaded
#			and disabled cells are not copied over.  No precision
#			is lost in the process.
#-------------------------------------------------------------------------------

proc tksu::GetThumbValue w {
    variable ThumbState

    keylget ThumbState($w) digits digits
    keylget ThumbState($w) shade shade

    set n [expr $digits + 6]
    set value ""
    for {set i 0} {$i < $n} {incr i} {
	if {[string index $shade $i] == 0} {
	    append value [$w.t$i cget -text]
	}
    }
    return $value
}

#-------------------------------------------------------------------------------
# DisableThumb -- display thumbwheel in disabled state.
#
# Args:     w		Thumbwheel frame widget.
# Returns:  null
#-------------------------------------------------------------------------------

proc tksu::DisableThumb w {
    variable ThumbState

    keylset ThumbState($w) state disabled
    keylget ThumbState($w) digits digits
    set value [CanonValue 0 e $digits]

# Load each cell box:

    set n [string length $value]
    set shade ""
    for {set i 0} {$i < $n} {incr i} {
	set digit [string range $value $i $i]
	$w.t$i configure -text $digit
	ColorThumb $w $i disabled
	append shade 2
    }
    keylset ThumbState($w) shade $shade
    return
}

#-------------------------------------------------------------------------------
# DragThumbStart -- begin drag operation on any digit in thumbwheel.
#
# Args:     w		Thumbwheel frame widget.
#	    idigit	Index of the thumbwheel digit clicked on.
#	    x y		Screen coordinates of start of drag.
# Returns:  null
#
# The starting coordinates of the drag are stored in ThumbState, in keyed
# entries whose existence serves to indicate that a drag is taking place.
#
# The sign digits and E digit do not support dragging -- toggle them at this
# time.  If toggled, invoke the command callback to notify any listeners on
# the thumbwheel.
#-------------------------------------------------------------------------------

proc tksu::DragThumbStart {w idigit x y} {
    variable ThumbState
    variable BitmapDir

    keylget ThumbState($w) state state
    keylget ThumbState($w) digits digits
    keylget ThumbState($w) rdigits rdigits
    keylget ThumbState($w) ftype ftype

    if {$state == "disabled"} {
    	return
    }

# dx = width of cell in pixels.
# x0 = screen x coord of left edge of cell clicked in (index idigit).
# xe = screen x coord of left edge of cell `E' (index edigit).

    set cell $w.t$idigit
    set char [$cell cget -text]
    set edigit [expr $digits + 2]
    set xe [winfo rootx $w.t$edigit]
    incr edigit
    set dx [winfo rootx $w.t$edigit]
    incr edigit -1
    incr dx -$xe
    set x0 [winfo rootx $cell]

# For an integer, the decimal point and exponent are disabled.

    if {$ftype == "d"} {
    	if {$idigit > $digits} return

# For fixed point, the exponent sign and digits are disabled.

    } elseif {$ftype == "f"} {
    	if {$idigit > $edigit} return
    }

# Sign digits:  toggle between +/-.

    if {$idigit == 0} {
	keylget ThumbState($w) shade shade
    	if {$char == "+"} {
	    $cell configure -text "-"
	    ColorThumb $w 0 normal
	    set shade 0[string range $shade 1 end]
	} else {
	    $cell configure -text "+"
	    ColorThumb $w 0 shaded
	    set shade 1[string range $shade 1 end]
	}
	keylset ThumbState($w) shade $shade
	eval [keylget ThumbState($w) command]
	return
    }

    if {$idigit == [expr $edigit + 1]} {
    	if {$char == "+"} {
	    $cell configure -text "-"
	} else {
	    $cell configure -text "+"
	}
	eval [keylget ThumbState($w) command]
	return
    }

# Clicking on the E digit toggles ftype between `f' and `e'.  When going from
# f to e, the value may have to be renormalized.  When going from e to f,
# if the value overflows, replace it with the maximum value.  The value will
# not exceed the min, max settings in ThumbState.

    if {$idigit == $edigit} {

	set value [GetThumbValue $w]
    	if {$ftype == "f"} {
	    set newValue [CanonValue $value e $digits]
	    set newFtype e
	} else {
	    set newValue [CanonizeValue $value float $digits]
	    if {$newValue == "" || [CanonType $newValue] == "e"} {
		set newValue [format %+${digits}f $value]
		regexp {([0-9]*)\.} $newValue dummy ldigits
		set ldigits [max 1 [string length $ldigits]]
		set rdigits [max 1 [expr $digits - $ldigits]]
	    	set newValue [CanonValue $value f $digits $rdigits]
	    }
	    set newFtype f
	}
	keylset ThumbState($w) rdigits [CanonRight $newValue] ftype $newFtype
	LoadThumb $w $newValue
	return
    }

# Drag cases now follow.  Change to thumb cursor.  Save existing cursor in
# ThumbState and restore it in DragThumbFinish.

    keylset ThumbState($w) cell $cell cursor [$cell cget -cursor]
    $cell configure -cursor [list @[file join $BitmapDir Thumb.xbm] black]

# Begin drag on decimal point (type f only).  Items stored in ThumbState:
#
#   dragType	`decimal', defining this type of drag.
#   dragValue	Original value in thumbwheel at start of drag.
#   xe		Window x coord of left edge of E digit.
#   dx		Width of a digit cell, for calculating which digit the
#		dragged cursor currently is in.

    if {$ftype == "f" && $char == "."} {
	set value [CanonValue [GetThumbValue $w] f $digits $rdigits]
	keylset ThumbState($w) dragType decimal dragValue $value \
	    xe $xe.0 dx $dx.0
	return
    }

# Begin drag on digit in mantissa.  Construct mask which has 1 in idigit
# and 0's elsewhere in mantissa.  Convert this into a drag rate of value
# units per pixel.

    set dy 10.0
    if {$idigit < $edigit && $char != "."} {
	set value [CanonValue [GetThumbValue $w] e $digits $rdigits]

	set idecimal [expr $digits - $rdigits + 1]
	if {$ftype == "e"} {
	    regexp {E(.+)} $value dummy expo
	    set expo [expr [BlankPad $expo] + $idecimal - $idigit]
	} else {
	    set expo [expr $idecimal - $idigit]
	}
	if {$idigit < $idecimal} {
	    incr expo -1
	}
	set mask 1.E$expo
	set rate [expr $mask/$dy]

# Rate is initially set for the center of the dragged cell.  Adjust the rate
# smoothly with x (on an exponential scale), such that if the mouse were
# centered in x on the cell to the left, the rate would be 10 times higher.

	set alpha [expr ($x.0 - $x0.0)/$dx.0]
	if {$alpha < 0.0} {set alpha 0.0}
	if {$alpha > 1.0} {set alpha 1.0}
	set rate [expr $rate * pow(10.0, 0.5 - $alpha)]

# Items stored in ThumbState for mantissa drag:
#
#   dragType	`mantissa', defining this type of drag.
#   dragValue	Original value in thumbwheel at start of drag.
#   dragRate	Drag rate value, in units per pixel, in cell at x0.
#   dx		Cell width in pixels.
#   x0		X coord of left edge of clicked cell.
#   y0		Initial y coord of cursor.
#   mask	mask for preserving digits less significant than idigit.

	keylset ThumbState($w) dragType mantissa dragValue $value \
	    dragRate $rate dx $dx x0 $x0 y0 $y mask $mask
    	return
    }

# Begin drag on exponent digit.  Similar to mantissa drag, but only two
# digits are involved.

    if {$idigit > $edigit} {
	set value [CanonValue [GetThumbValue $w] $ftype $digits $rdigits]
	if {$idigit == [expr $edigit + 2]} {
	    set rate 10.0
	} else {
	    set rate 1.0
	}
	set rate [expr $rate/$dy]

# Items stored in ThumbState for exponent drag:
#
#   dragType	`exponent', defining this type of drag.
#   dragValue	Original value in thumbwheel at start of drag.
#   dragRate	Drag rate value, in units per pixel.
#   dx		Cell width in pixels.
#   x0		X coord of left edge of clicked cell.
#   y0		Initial y position of cursor.

	keylset ThumbState($w) dragType exponent dragValue $value \
	    dragRate $rate dx $dx x0 $x0 y0 $y
    	return
    }
    return
}

#-------------------------------------------------------------------------------
# DragThumbMotion -- called as cursor moves during a drag operation.
#
# Args:     w		Thumbwheel frame widget.
#	    x y		Current screen coordinates of cursor.
# Returns:  null
#
# The thumbwheel value is dynamically updated as the drag proceeds.
#-------------------------------------------------------------------------------

proc tksu::DragThumbMotion {w x y} {
    variable ThumbState

# No drag is active if dragType key is not present.

    if {![keylget ThumbState($w) dragType dragType]} {
    	return
    }
    keylget ThumbState($w) digits digits
    keylget ThumbState($w) rdigits rdigits
    keylget ThumbState($w) ftype ftype

    if {$dragType == "decimal"} {
	keylget ThumbState($w) dragValue value
	keylget ThumbState($w) xe xe
	keylget ThumbState($w) dx dx

# For decimal drag, rdigits is the only setting that changes.  Determine new
# rdigits within the limits 1 to digits-1.

	set rdigitsOld $rdigits
	set rdigits [expr int(($xe - $x)/$dx)]
	if {$rdigits == $rdigitsOld} {
	    return
	}
	if {$rdigits < 1} {
	    set rdigits 1
	} elseif {$rdigits >= $digits} {
	    set rdigits [expr $digits - 1]
	}
	keylset ThumbState($w) rdigits $rdigits
	set value [CanonValue $value f $digits $rdigits]
	LoadThumb $w $value
	return
    }

# Mantissa and exponent drags:

    if {$dragType == "mantissa" || $dragType == "exponent"} {
	keylget ThumbState($w) dragValue value
	keylget ThumbState($w) dragRate rate
	keylget ThumbState($w) dx dx
	keylget ThumbState($w) x0 x0
	keylget ThumbState($w) y0 y0
	keylget ThumbState($w) min min
	keylget ThumbState($w) max max
	keylget ThumbState($w) attained attained

# For mantissa drag, compute new value by multiplying the pixel shift by the
# drag rate.  The digits less significant than idigit are preserved by
# truncating delta with the use of mask.

	if {$dragType == "mantissa"} {
	    keylget ThumbState($w) mask mask
	    set delta [expr ($y0 - $y) * $rate]
	    set delta [expr int($delta/$mask) * $mask]
	    set value [expr $delta + $value]

# Exponent drag is similar, except that only the exponent digits are changed.
# Pad the exponent with blanks to avoid octal arithmetic.

	} else {
	    regexp {(.+)E(.+)} $value dummy mant expo
	    set expo [BlankPad $expo]
	    set expo [expr ($y0 - $y) * $rate + $expo]
	    if {$expo < -99.0} {set expo -99.0}
	    if {$expo >  99.0} {set expo  99.0}
	    set expo [format %+03d [expr int($expo)]]
	    set value ${mant}E${expo}
	}

# If value is out of (min,max) range, either clip it or reject it.

	if {$min != "" && $value < $min} {
	    if {!$attained} return
	    set value $min
	}
	if {$max != "" && $value > $max} {
	    if {!$attained} return
	    set value $max
	}
	set value [CanonValue $value $ftype $digits $rdigits]
	LoadThumb $w $value
	return
    }
    return
}

#-------------------------------------------------------------------------------
# DragThumbFinish -- complete thumbwheel drag operation.
#
# Args:     w		Thumbwheel frame widget.
#	    x y		Screen coordinates of completion of drag.
# Returns:  null
#
# The callback command is evaluated when a drag is successfully completed.
#-------------------------------------------------------------------------------

proc tksu::DragThumbFinish {w x y} {
    variable ThumbState

# Restore original cursor.

    if {[keylget ThumbState($w) cell cell] \
    &&  [keylget ThumbState($w) cursor cursor]} {
	$cell configure -cursor $cursor
    }

# No drag is active if dragType key is not present.

    if {![keylget ThumbState($w) dragType dragType]} {
    	return
    }
    switch -- $dragType {
    	decimal {
	    keyldel ThumbState($w) dragType dragValue xe dx
	}
	mantissa -
	exponent {
	    keyldel ThumbState($w) dragType dragValue dragRate y0
	}
    }
    eval [keylget ThumbState($w) command]
    return
}

#-------------------------------------------------------------------------------
# CanonizeValue -- determine canonical format for given numerical value.
#
# Args:     value	Integer or float value to be canonized.
#	    type	Value type: `int' or `float'.  The `int' type is
#			assumed to be 4 bytes, 2's complement.  The `float'
#			type is assumed to be IEEE 8-byte floating point.
#	    digits	Number of mantissa digits available for value.
#
# Returns:  cvalue	A "canonized" value in one of the following formats:
#
#			1) %+Ld for an integer, where L = digits.
#			2) %+0L.Rf for a float, where L = digits + 2, and
#			   1 <= R <= digits - 1, so that decimal point is
#			   always surrounded on either side by a digit.
#			3) %+L.RE for a float, where L = digits + 6 and
#			   R = digits - 1.
#
#			If the input value is null, garbage, or out of range,
#			the returned value is null.  Any type other than `int'
#			or `float' will also return null.
#-------------------------------------------------------------------------------

proc tksu::CanonizeValue {value type digits} {

    if {$type != "int" && $type != "float"} {
    	return
    }
    if {$digits < 2 || $digits > 12} {
    	return
    }

# Use expr to detect if value is not a number, or if it is outside the range
# of a floating point value.

    if [regexp {^[+-]?[0-9]+$} $value] {
	# Add decimal point to integer
    	append value .
    }
    if [catch {
    	set value [expr $value]
    }] {
    	return
    }

# Integers:  verify that value can fit inside the thumbwheel as an integer.
# Then convert to an integer.

    if {$type == "int"} {

	set maxd [CanonMax d $digits]
	if {$value < -$maxd || $value > $maxd} {
	    return
	}
	set n [expr $digits + 1]
	set value [format %+${n}d [expr int($value)]]
	return $value
    }

# Floats:  verify that value can fit inside the thumbwheel in exponential
# form.

    set maxe [CanonMax e $digits]
    if {$value < [expr {-($maxe)}] || $value > $maxe} {
    	return
    }

# Now determine if value can be expressed in fixed point format without
# losing any digits.  Format the value with %f, trim the trailing zeros,
# and if the resulting number fits, use it.

    set fvalue [string trimright [format %+.${digits}f $value] 0]
    regexp {([0-9]*)\.([0-9]*)} $fvalue dummy fleft fright
    set fleft [string length $fleft]
    set fright [string length $fright]
    if {$fright < 1} {
    	set fright 1
    }
    if {$fleft + $fright <= $digits && $fvalue == $value} {
	set ftotal [expr $digits + 2]
	set value [format %+0${ftotal}.${fright}f $fvalue]
	return $value
    }

# Otherwise format the number with %e.

    set fright [expr $digits - 1]
    set ftotal [expr $digits + 6]
    set value [format %+${ftotal}.${fright}E $value]
    return $value
}

#-------------------------------------------------------------------------------
# CanonizeRange -- determine min and max values of range in canonical form.
#
# Args:     range	Parameter range specification in nlist format (see
#			LoadSpecs for details).  Acceptable forms are
#			${min}-${max} and ${min}<${max} where min and/or max
#			may be null.
#	    type	Parameter value type: `int' or `float'.  Any other
#			type will produce a null return values.
#	    digits	Number of digits available for min and max values.
# Returns:  lrange	A list {min max attained} where
#			min	 = formatted minimum range value, or null,
#			max	 = formatted maximum range value, or null,
#			attained = whether value can equal min or max.
#-------------------------------------------------------------------------------

proc tksu::CanonizeRange {range type digits} {

# Parse range argument into min, max, attained.  If there is anything wrong
# with the range specification, just return a wide open range.

    if {![regexp {(.*)(<=?)(.*)} $range dummy min separator max]} {
    	# Range string not recognized
	return [list {} {} 1]
    }
    set min [CanonizeValue $min $type $digits]
    set max [CanonizeValue $max $type $digits]
    set attained [expr {$separator == "<="}]

    return [list $min $max $attained]
}

#-------------------------------------------------------------------------------
# CanonValue -- canonize value by format type.
#
# Args:     value	Value to be converted.
#	    ftype	Format type: `d', `e', or `f'.
#	    digits	Number of digits in mantissa.
#	    rdigits	Number of digits to right of decimal point.
#			This argument is optional and ignored for ftypes d
#			and e, but required for ftype f.
# Returns:  cvalue	Value in the desired format.  If the value is out of
#			range with respect to the given format, it is
#			replaced with the maximum.
#
# Compare this to CanonizeValue.  This routine formats the value given ftype
# and rdigits, while CanonizeValue automatically determines ftype and rdigits.
# Both return a value in the proper format for storing in the thumbwheel.
#-------------------------------------------------------------------------------

proc tksu::CanonValue {value ftype digits args} {

    set rdigits [lindex $args 0]
    set maxValue [CanonMax $ftype $digits $rdigits]
    set minValue [expr {- $maxValue}]

    if {$value > $maxValue} {
    	set value $maxValue
    } elseif {$value < $minValue} {
    	set value $minValue
    }
    switch -- $ftype {
    	d {
	    set n [expr $digits + 1]
	    return [format %+${n}d [expr int($value)]]
	}
	e {
	    set rdigits [expr $digits - 1]
	    set n [expr $digits + 6]
	    return [format %+${n}.${rdigits}E $value]
	}
	f {
	    set n [expr $digits + 2]
	    return [format %+0${n}.${rdigits}f $value]
	}
    }
    return
}

#-------------------------------------------------------------------------------
# CanonDigits -- return digits from canonized value.
#
# Args:     value	Integer or floating value from CanonizeValue().
# Returns:  digits	Total number of digits in mantissa.
#-------------------------------------------------------------------------------

proc tksu::CanonDigits value {

    set ftype [CanonType $value]
    set n [string length $value]

    switch -- $ftype {
    	d {
	    return [expr $n - 1]
	}
	e {
	    return [expr $n - 6]
	}
	f {
	    return [expr $n - 2]
	}
    }
    return 0
}

#-------------------------------------------------------------------------------
# CanonRight -- return rdigits from canonized value.
#
# Args:     value	Integer or floating value from CanonizeValue().
# Returns:  rdigits	Number of digits to the right of the decimal point.
#-------------------------------------------------------------------------------

proc tksu::CanonRight value {

    if {![regexp {\.([0-9]*)} $value dummy right]} {
    	return 0
    }
    return [string length $right]
}

#-------------------------------------------------------------------------------
# CanonType -- return type of canonized value.
#
# Args:     value	Integer or floating value from CanonizeValue().
# Returns:  ftype	Type of formatted value: `d', `e' or `f'.
#-------------------------------------------------------------------------------

proc tksu::CanonType value {

    if [regexp {[Ee]} $value] {
    	return "e"
    } elseif [regexp {\.} $value] {
    	return "f"
    } else {
    	return "d"
    }
}

#-------------------------------------------------------------------------------
# CanonMax -- return the maximum canonical value with given ftype.
#
# Args:     ftype	Type of canonical format: `d', `e' or `f'.
#	    digits	Total number of digits in mantissa.
#	    rdigits	Number of digits to the right of the decimal point.
#			This arg is optional and ignored for ftypes d and e,
#			but required for ftype f.
# Returns:  value	Positive signed maximum value for specified ftype.
#-------------------------------------------------------------------------------

proc tksu::CanonMax {ftype digits args} {

    switch -- $ftype {
    	d {
	    set value +
	    for {set i 0} {$i < $digits} {incr i} {
	    	append value 9
	    }
	    set maxLong 2147483647
	    if "$value.0 > $maxLong.0" {
		set n [expr $digits + 1]
		set value [format %+${n}d $maxLong]
	    }
	}
	e {
	    set value +9.
	    set rdigits [expr $digits - 1]
	    for {set i 0} {$i < $rdigits} {incr i} {
	    	append value 9
	    }
	    append value E+99
	}
	f {
	    set rdigits [lindex $args 0]
	    set ldigits [expr $digits - $rdigits]
	    set value +
	    for {set i 0} {$i < $ldigits} {incr i} {
	    	append value 9
	    }
	    append value .
	    for {set i 0} {$i < $rdigits} {incr i} {
	    	append value 9
	    }
	}
    }
    return $value
}

#-------------------------------------------------------------------------------
# BumpValue -- return canonized value incremented by smallest significant amt.
#
# Args:     direction	Either `up' (increase) or `down' (decrease value).
#	    value	Integer or floating value from CanonizeValue().
# Returns:  newValue	Canonized value that is greater or less than the input
#			value by the least significant digit.
#-------------------------------------------------------------------------------

proc tksu::BumpValue {direction value} {

    set decrease [expr {$direction == "down"}]
    set ftype    [CanonType $value]
    set digits   [CanonDigits $value]
    set rdigits  [CanonRight $value]
    set maxValue [CanonMax $ftype $digits $rdigits]

# Don't let the value overflow.

    if $decrease {
	if {$value <= -$maxValue} {
	    return -$maxValue
	}
    } else {
	if {$value >= $maxValue} {
	    return $maxValue
	}
    }

    if {$ftype == "d"} {
	set increment 1

    } else {
	set increment 0.
	set n [expr $rdigits - 1]
	for {set i 0} {$i < $n} {incr i} {
	    append increment 0
	}
	append increment 1

	if {$ftype == "e"} {
	    regexp {(E.+)} $value dummy exponent
	    append increment $exponent
	}
    }

    if $decrease {
    	set increment -$increment
    }
    set newValue [expr $value + $increment]
    return [CanonValue $newValue $ftype $digits $rdigits]
}

#-------------------------------------------------------------------------------
# ZeroPad -- replace leading blanks in canonized integer with zeros.
#
# Args:     value	Canonized value.
# Returns:  newValue	Value with zeros replacing blanks, repositioned sign.
#-------------------------------------------------------------------------------

proc tksu::ZeroPad value {

    regexp {( *)([+-])(.*)} $value dummy blanks sign nonblanks
    regsub -all { } $blanks 0 zeros
    return $sign$zeros$nonblanks
}

#-------------------------------------------------------------------------------
# BlankPad -- replace leading zeros in canonized integer with blanks.
#
# Args:     value	Canonized value.
# Returns:  newValue	Value with blanks replacing zeros, repositioned sign.
#			Retain last digit however.
#-------------------------------------------------------------------------------

proc tksu::BlankPad value {

    regexp {([+-])(0*)(.*[0-9])$} $value dummy sign zeros nonblanks
    regsub -all 0 $zeros { } blanks
    return $blanks$sign$nonblanks
}
