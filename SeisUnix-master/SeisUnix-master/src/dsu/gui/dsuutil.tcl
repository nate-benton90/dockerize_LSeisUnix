#/*********************** self documentation **********************/
#/*
# * dsuutil.tcl - tcl/tk utility commands
# *
# * NOTE: Extracted from the source code of XPVM.
# * See:  XPVM (hhtp://www.netlib.edu/...)
# *
# */
#/**************** end self doc ********************************/
#
#/*
# * Original AUTHOR:  J. A. Khol PVM team
# * CSM AUTHOR:  Alejandro E. Murillo, Colorado School of Mines, 03/03/96
# *
#*/

#
# DSU TCL Utility Procs
#

proc scrollInit \
	{ canvas sb max_size canvas_size win_size vvar orient loc incr } \
{
	#debug_proc scrollInit entry

	global $vvar

	if { $loc == "low" } \
	{
		set view 0

		set scroll_view 0
	} \

	elseif { $loc == "center" } \
	{
		set view [ scrollCenterValue $max_size $win_size $incr ]

		if { $canvas_size < $win_size } \
			{ set scroll_view 0 } \

		else \
		{
			set scroll_view [ expr $view \
				- [ scrollCenterValue $max_size $canvas_size $incr ] ]
		}
	} \

	elseif { $loc == "high" } \
	{
		set view [ expr ( $max_size - $win_size ) / $incr ]

		if { $canvas_size < $win_size } \
			{ set scroll_view 0 } \

		else \
			{ set scroll_view $view }
	}

	scrollSet $sb $canvas_size $win_size $incr $scroll_view

	if { $orient == "HORIZ" } \
	{
		$canvas xview moveto 0.0
        	$canvas xview scroll $view units
		set $vvar $view
	} \

	else \
	{
		$canvas yview moveto 0.0
                $canvas yview scroll $view units
		set $vvar $view
	}

	#debug_proc scrollInit exit
}

proc scrollSet { sb canvas_size win_size incr first } \
{
	#debug_proc scrollSet entry

	set csize [ expr ( $canvas_size + ($incr - 1) ) / $incr ]

	set wsize [ expr $win_size / $incr ]

	set last [ expr $first + $wsize - 1 ]

	$sb set $csize $wsize $first $last

	#debug_proc scrollSet exit
}

proc scrollViews { view incr value } \
{
	#debug_proc scrollViews entry

	global ST_SBH
	global UT_SBH
	global ST_C
	global UT_C

	global frame_border
	global st_active
	global ut_active
	global st_cwidth
	global ut_cwidth
	global tmp_csize
	global timex

	global FRAME_OFFSET

	set fo [ expr $FRAME_OFFSET * $frame_border ]

	set tmp_csize [ expr $timex + (2 * $frame_border) + $fo ]

	#
	# Note: Really need to set canvas view separately from
	# scrollbar in this case, as new TCL/TK causes major
	# hassle with extra frame border.
	#
	# $fo should be included in canvas view, but not in scrollSet{}.
	#

	if { $view == "ST" } \
	{
		scrollCanvas $ST_C $ST_SBH -1 tmp_csize st_cwidth st_xview \
			HORIZ low $incr $value

		if { $ut_active == "TRUE" } \
		{
			set utxv [ expr $value - \
				( $value * ( $ut_cwidth - $st_cwidth) \
					/ ( $tmp_csize - $st_cwidth ) ) ]

			scrollCanvas $UT_C $UT_SBH -1 tmp_csize ut_cwidth ut_xview \
				HORIZ low $incr $utxv
		}
	} \

	elseif { $view == "UT" } \
	{
		scrollCanvas $UT_C $UT_SBH -1 tmp_csize ut_cwidth ut_xview \
			HORIZ low $incr $value

		if { $st_active == "TRUE" } \
		{
			set stxv [ expr $value + \
				( $value * ( $ut_cwidth - $st_cwidth) \
					/ ( $tmp_csize - $ut_cwidth ) ) ]

			scrollCanvas $ST_C $ST_SBH -1 tmp_csize st_cwidth st_xview \
				HORIZ low $incr $stxv
		}
	}

	#debug_proc scrollViews exit
}

proc scrollTwoCanvases \
	{ c1 c2 sb1 max csize wsize vvar1 vvar2 orient loc \
		incr1 incr2 value } \
{
	#debug_proc scrollTwoCanvases entry

	scrollCanvas $c1 $sb1 $max $csize $wsize $vvar1 $orient $loc \
		$incr1 $value

	set fix_csize 0

	if { $incr1 != $incr2 } \
	{
		global $csize
		global $wsize

		set cs [ expr $$csize ]
		set ws [ expr $$wsize ]

		set maxvalue1 [ scrollMaxValue $cs $ws $incr1 ]

		if { $value >= $maxvalue1 } \
			{ set value2 [ expr ( $maxvalue1 * $incr1 ) / $incr2 ] } \

		else \
			{ set value2 [ expr ( $value * $incr1 ) / $incr2 ] }

		set maxvalue2 [ scrollMaxValue $cs $ws $incr2 ]

		if { $value2 > $maxvalue2 } \
		{
			set tmp [ expr $cs + 1 ]

			while { $value2 > [ scrollMaxValue $tmp $ws $incr2 ] } \
				{ set tmp [ expr $tmp + 1 ] }

			set $csize $tmp

			set fix_csize 1
		}
	} \

	else \
		{ set value2 $value }

	scrollCanvas $c2 none $max $csize $wsize $vvar2 $orient $loc \
		$incr2 $value2

	if { $fix_csize == 1 } \
		{ set $csize $cs }

	#debug_proc scrollTwoCanvases exit
}

proc scrollCanvas \
	{ canvas sb max_size canvas_size win_size vvar orient loc \
		incr value } \
{
	#debug_proc scrollCanvas entry

	global $canvas_size
	global $win_size
	global $vvar

	#
	# Limit Scroll Value
	#

	set csize [ expr $$canvas_size ]

	set wsize [ expr $$win_size ]

	if { $value < 0 } { set value 0 }

	set maxvalue [ scrollMaxValue $csize $wsize $incr ]

	if { $maxvalue > 0 } \
	{
		if { $value > $maxvalue } \
			{ set value $maxvalue }
		
		set scroll_value $value

		if { $loc == "center" } \
		{
			set value [ expr $value \
				+ [ scrollCenterValue $max_size $csize $incr ] ]
		} \

		elseif { $loc == "high" } \
		{
			set value [ expr ( ( $max_size - $wsize ) / $incr ) \
				- $value ]
		}
	} \

	else \
	{
		if { $loc == "low" } \
			{ set value 0 } \
	
		elseif { $loc == "center" } \
			{ set value [ scrollCenterValue $max_size $wsize $incr ] } \
	
		elseif { $loc == "high" } \
			{ set value [ expr ( $max_size - $wsize ) / $incr ] }
	
		set scroll_value 0
	}

	#
	# Set Scrollbar
	#

	if { $sb != "none" } \
		{ scrollSet $sb $csize $wsize $incr $scroll_value }

	#
	# Scroll Canvas View
	#

	if { $orient == "HORIZ" } \
	{
		#AEMdo_xview $canvas $value
		$canvas xview moveto 0.0
        	$canvas xview scroll $value units

		set $vvar $value
	} \

	else \
	{
		#AEMdo_yview $canvas $value
		$canvas yview moveto 0.0
        	$canvas yview scroll $value units

		set $vvar $value
	}

	#debug_proc scrollCanvas exit
}

proc scrollMaxValue { csize wsize incr } \
{
	return [ expr ( ($csize + ($incr - 1)) / $incr ) \
		- ( $wsize / $incr ) ]
}

proc scrollCenterValue { max_size wsize incr } \
{
	return [ expr ( ($max_size / 2) - ($wsize / 2) ) / $incr ]
}


proc makeMenu { name cmd info_list close leavesubs prefix } \
{
	#debug_proc makeMenu entry

	global active_fg_color
	global selector_color
	global frame_border
	global fg_color

	global SELECTOR

	frame $name -bd $frame_border -relief raised

	restrict_bindings $name ""

	set root [ rootName $name ]

	set mil "$root\_list"

	global $mil

	set LAST none

	set width 0

	foreach i $info_list \
	{
		set label	[ lindex $i 0 ]
		set type	[ lindex $i 1 ]
		set index	[ lindex $i 2 ]

		#
		# Create Button / Label
		#

		set fix_label [ strip_label_cmd $label ]
		#set fix_label $label

		if { $type == "checkbutton" } \
		{
			#
			# Get State From Info List
			#

			set state	[ lindex $i 3 ]
			set help	[ lindex $i 4 ]

			#
			# Create checkbutton state var
			#

			if { $prefix == "none" } \
				{ set prefix $root }

			set menuvar "$prefix\_state_$fix_label"

			global $menuvar

			if { $state == "ON" } \
				{ set $menuvar ON } \

			else \
				{ set $menuvar OFF }

			#
			# Set checkbutton command / create
			#

			set command "$cmd $index \"$label\" $menuvar FALSE"

			set NAME "$name.ckbutt_$fix_label"

			checkbutton $NAME -text $label -command $command \
				-padx 1 -pady 1 \
				-bd 1 -relief raised -anchor nw \
				-onvalue ON -offvalue OFF -variable $menuvar \
				$SELECTOR $selector_color -foreground $fg_color \
				-activeforeground $active_fg_color

			restrict_bindings $NAME ""
		} \

		elseif { $type == "command" } \
		{
			set help	[ lindex $i 3 ]

			set command "$cmd $index \"$label\" none FALSE"

			set NAME "$name.butt_$fix_label"

			button $NAME -text $label -command $command \
				-padx 1 -pady 1 \
				-bd 1 -relief raised \
				-foreground $fg_color -activeforeground $active_fg_color

			restrict_bindings $NAME ""
		} \

		elseif { $type == "submenu" } \
		{
			set focus_entry [ lindex $i 3 ]
			set subs		[ lindex $i 4 ]
			set help		[ lindex $i 5 ]

			set NAME "$name.butt_$fix_label"

			set command "raiseSubMenu $index $name $NAME \
				$focus_entry $subs"

			button $NAME -text $label -command $command \
				-padx 1 -pady 1 \
				-bd 1 -relief raised -foreground $fg_color \
				-activeforeground $active_fg_color

			restrict_bindings $NAME ""
		} \

		elseif { $type == "exchange" } \
		{
			set button	[ lindex $i 3 ]
			set parent	[ lindex $i 4 ]
			set subs	[ lindex $i 5 ]
			set help	[ lindex $i 6 ]

			set command \
				"exchangeMenu $index $name $button $parent $subs"

			set NAME "$name.butt_$fix_label"

			button $NAME -text $label -command $command \
				-padx 1 -pady 1 \
				-bd 1 -relief raised -foreground $fg_color \
				-activeforeground $active_fg_color

			restrict_bindings $NAME ""
		} \

		elseif { $type == "exchangeIndirect" } \
		{
			set bname	[ lindex $i 3 ]
			set parent	[ lindex $i 4 ]
			set subs	[ lindex $i 5 ]
			set help	[ lindex $i 6 ]

			set command \
				"exchangeMenuIndirect $index $name $bname $parent $subs"

			set NAME "$name.butt_$fix_label"

			button $NAME -text $label -command $command \
				-padx 1 -pady 1 \
				-bd 1 -relief raised -foreground $fg_color \
				-activeforeground $active_fg_color

			restrict_bindings $NAME ""
		} \

		else \
		{
			set NAME "$name.label_$fix_label"

			label $NAME -text $label -foreground $fg_color
		}

		if { $type != "label" } \
			{ butt_help $NAME button $help }

		#
		# Place Button / Label on Menu Frame
		#

		if { $LAST != "none" } \
			{ set mby [ below $LAST ] } \

		else \
			{ set mby $frame_border }

		place $NAME -x $frame_border -y $mby

		update

		#
		# Process Button Width
		#

		set wt [ winfo width $NAME ]

		if { $wt > $width } \
			{ set width $wt }

		#
		# Save Button Name
		#

		set LAST $NAME

		lappend $mil $NAME
	}

	#
	# Create "Done" Button
	#

	set NAME $name.butt_Done

	if { "$close" == "lower" } \
		{ set cmd "lowerMenu $name" } \

	else \
		{ set cmd "$close" }

	button $NAME -text "Done" \
		-padx 1 -pady 1 \
		-bd 1 -relief raised -command $cmd \
		-foreground $fg_color -activeforeground $active_fg_color

	bind $name <Leave> "leaveMenu $name %x %y $leavesubs {$cmd}"

	if { $LAST != "none" } \
		{ set dy [ below $LAST ] } \

	else \
		{ set dy $frame_border }

	place $NAME -x $frame_border -y $dy

	butt_help $NAME close "Lower Menu"

	lappend $mil $NAME

	update

	#
	# Process Final Button Width
	#

	set wt [ winfo width $NAME ]

	if { $wt > $width } \
		{ set width $wt }

	#
	# Set Main Menu Frame Size
	#

	set menu_height [ expr [ below $NAME ] + 4 ]

	set menu_width [ expr $width + (4 * $frame_border) ]

	$name configure -width $menu_width -height $menu_height

	place forget $name

	#
	# Adjust Menu Button Widths
	#

	foreach i [ expr $$mil ] \
		{ place $i -width $width }

	#debug_proc makeMenu exit
}

proc raiseMenu { name button parent subs } \
{
	#debug_proc raiseMenu entry

	foreach i $subs \
		{ place forget $i }

	set ckm [ winfo ismapped $name ]

	if { $ckm == 1 } \
		{ place forget $name } \

	else \
		{ placeMenu $name $button $parent }

	#debug_proc raiseMenu exit
}

proc raiseMenuIndirect { name bname button parent subs } \
{
	#debug_proc raiseMenuIndirect entry

	global $bname

	foreach i $subs \
		{ place forget $i }

	set ckm [ winfo ismapped $name ]

	if { $ckm == 1 } \
		{ place forget $name } \

	else \
	{
		set $bname $button

		placeMenu $name $button $parent
	}

	#debug_proc raiseMenuIndirect exit
}

proc placeMenu { name button parent } \
{
	#debug_proc placeMenu entry

	set px [ winfo rootx $parent ]
	set py [ winfo rooty $parent ]

	set x [ expr [ winfo rootx $button ] - $px ]

	set y [ expr [ winfo rooty $button ] \
		+ [ winfo height $button ] - $py ]

	place $name -x $x -y $y

	raise $name

	update

	check_in_main $name $x $y

	update

	#debug_proc placeMenu exit
}

proc replaceMenu { name button parent } \
{
	#debug_proc replaceMenu entry

	set ckup [ winfo ismapped $name ]

	if { $ckup == 1 } \
		{ placeMenu $name $button $parent }

	#debug_proc replaceMenu exit
}

proc raiseSubMenu { submenu menu button focus_entry subs } \
{
	#debug_proc raiseSubMenu entry

	foreach i $subs \
		{ place forget $i }

	set ckup [ winfo ismapped $submenu ]

	if { $ckup == 1 } \
		{ place forget $submenu } \

	else \
		{ placeSubMenu $submenu $menu $button $focus_entry }

	#debug_proc raiseSubMenu exit
}

proc placeSubMenu { submenu menu button focus_entry } \
{
	#debug_proc placeSubMenu entry

	set x [ winfo x $menu ]
	set y [ winfo y $menu ]

	set x [ expr $x + [ left $button ] ]
	set y [ expr $y + [ below $button ] ]

	place $submenu -x $x -y $y

	raise $submenu

	update

	check_in_main $submenu $x $y

	if { $focus_entry != "none" } \
		{ focus $focus_entry }

	update

	#debug_proc placeSubMenu exit
}

proc replaceSubMenu { submenu menu button focus_entry } \
{
	#debug_proc replaceSubMenu entry

	set ckup [ winfo ismapped $submenu ]

	if { $ckup == 1 } \
		{ placeSubMenu $submenu $menu $button $focus_entry }

	#debug_proc replaceSubMenu exit
}

proc exchangeMenu { submenu menu button parent subs } \
{
	#debug_proc exchangeMenu entry

	set cksm [ winfo ismapped $submenu ]

	if { $cksm == 1 } \
	{
		place forget $submenu

		raiseMenu $menu $button $parent $subs
	} \

	else \
	{
		place forget $menu

		raiseMenu $submenu $button $parent $subs
	}

	#debug_proc exchangeMenu exit
}

proc exchangeMenuIndirect { submenu menu bname parent subs } \
{
	#debug_proc exchangeMenuIndirect entry

	global $bname

	set cksm [ winfo ismapped $submenu ]

	if { $cksm == 1 } \
	{
		place forget $submenu

		raiseMenu $menu [ expr $$bname ] $parent $subs
	} \

	else \
	{
		place forget $menu

		raiseMenu $submenu [ expr $$bname ] $parent $subs
	}

	#debug_proc exchangeMenuIndirect exit
}

proc check_in_main { name x y } \
{
	#debug_proc check_in_main entry

	global main_height
	global main_width

	set wt [ winfo width $name ]

	set ckx [ expr $x + $wt ]

	if { $ckx > $main_width } \
	{
		set x [ expr $main_width - $wt ]

		if { $x < 0 } { set x 0 }

		place $name -x $x
	}

	set ht [ winfo height $name ]

	set cky [ expr $y + $ht ]

	if { $cky > $main_height } \
	{
		set y [ expr $main_height - $ht ]

		if { $y < 0 } { set y 0 }

		place $name -y $y
	}

	#debug_proc check_in_main exit
}

proc leaveMenu { name x y subs cmd } \
{
	#debug_proc leaveMenu entry

	set ht [ winfo height $name ]
	set wt [ winfo width $name ]

	if { $x < 0 || $y < 0 || $x >= $wt || $y >= $ht } \
	{
		foreach w $subs \
		{
			set ckw [ winfo ismapped $w ]

			if { $ckw == 1 } \
				{ return }
		}

		eval $cmd
	}

	#debug_proc leaveMenu exit
}

proc lowerMenu { name } \
{
	#debug_proc lowerMenu entry

	place forget $name

	#debug_proc lowerMenu exit
}


proc setMsg { text } \
{
	#debug_proc setMsg entry

	global msg_list

	.dsu.message configure -text "Status:   $text"

	set msg_list [ linsert $msg_list 0 [ list 0 $text ] ]

	update

	#debug_proc setMsg exit
}

proc setTmpMsg { hdr text } \
{
	#debug_proc setMsg entry

	global msg_list

	.dsu.message configure -text "$hdr:   $text"

	set msg_list [ linsert $msg_list 0 [ list 1 $hdr $text ] ]

	update

	#debug_proc setMsg exit
}

proc popMsgs { } \
{
	#debug_proc popMsgs entry

	global msg_list

	set cnt 0

	foreach m $msg_list \
	{
		set type [ lindex $m 0 ]

		if { $type == 1 } \
		{
			set msg_list [ lreplace $msg_list $cnt $cnt ]

			set cnt [ expr $cnt - 1 ]
		}

		set cnt [ expr $cnt + 1 ]
	}

	set pop [ lindex $msg_list 0 ]

	set popstr [ lindex $pop 1 ]

	.dsu.message configure -text "Status:   $popstr"

	update

	#debug_proc popMsgs exit
}

proc topMsg { } \
{
	#debug_proc topMsg entry

	global msg_list

	set top [ lindex $msg_list 0 ]

	set toptype [ lindex $top 0 ]

	if { $toptype == 0 } \
		{ return [ lindex $top 1 ] } \
	
	else \
		{ return [ lindex $top 2 ] }

	#debug_proc topMsg exit
}

proc rootName { path } \
{
	#debug_proc rootName entry

	set tmp [ split $path . ]

	set num [ llength $tmp ]

	set root [ lindex $tmp [ expr $num - 1 ] ]

	return $root

	#debug_proc rootName exit
}

proc above { win } \
{
	#debug_proc above entry

	set y [ winfo y $win ]

	return $y

	#debug_proc above exit
}

proc below { win } \
{
	#debug_proc below entry

	set y [ expr [ winfo y $win ] + [ winfo height $win ] ]

	return $y

	#debug_proc below exit
}

proc left { win } \
{
	#debug_proc left entry

	set x [ winfo x $win ]

	return $x

	#debug_proc left exit
}

proc right { win } \
{
	#debug_proc right entry

	set x [ expr [ winfo x $win ] + [ winfo width $win ] ]

	return $x

	#debug_proc right exit
}

proc get_user_name { } \
{
	#debug_proc get_user_name entry

	set home [ glob ~ ]

	set len [ string length $home ]

	set slash_index [ string last / $home ]

	set user_index [ expr $slash_index + 1 ]

	set user [ string range $home $user_index $len ]

	return $user

	#debug_proc get_user_name exit
}

proc entry_setup { entry cmd } \
{
	bind $entry <Return> $cmd

	bind $entry <2> \
		{ %W insert insert [selection get]; tk_entrySeeCaret %W }

	bind $entry <3> "%W scan mark %x"

	bind $entry <B3-Motion> "%W scan dragto %x"
}

proc butt_help { butt type msg } \
{
	#debug_proc butt_help entry

	bind $butt <Enter> "do_butt_help IN $butt $type {$msg}"

	bind $butt <Motion> "do_butt_help IN $butt $type {$msg}"

	if { $type == "button" } \
		{ bind $butt <ButtonPress> "$butt invoke" } \
	
	elseif { $type == "close" } \
		{ bind $butt <ButtonPress> "popMsgs ; $butt invoke" }

	bind $butt <Leave> "do_butt_help OUT $butt $type {$msg}"

	#debug_proc butt_help exit
}

proc do_butt_help { cmd butt type msg } \
{
	#debug_proc do_butt_help entry

	if { $cmd == "IN" } \
	{
		if { $type == "entry" } \
		{
			set msg [ lindex $msg 1 ]

			focus $butt
		}

		if { $type == "button" } \
			{ $butt configure -state active } \

		elseif { $type == "close" } \
			{ $butt configure -state active }

		if { $msg != [ topMsg ] } \
			{ setTmpMsg "Help" $msg }
	} \

	elseif { $cmd == "OUT" } \
	{
		popMsgs

		if { $type == "entry" } \
		{
			set cmd [ lindex $msg 0 ]

			eval $cmd
		}

		if { $type == "button" } \
			{ $butt configure -state normal } \

		elseif { $type == "close" } \
			{ $butt configure -state normal }
	}

	#debug_proc do_butt_help exit
}

proc unmap_all { } \
{
	#debug_proc unmap_all entry

	global win_map_list

	set win_map_list ""

	set tmp_list ".dsu"

	foreach w { .utilization .task_output } \
	{
		set ckup [ winfo ismapped $w ]

		if { $ckup == 1 } \
			{ lappend tmp_list $w }
	}

	set tmp_list [ sort_map_list $tmp_list ]

	foreach w $tmp_list \
		{ wm iconify $w }

	set win_map_list $tmp_list

	#debug_proc unmap_all exit
}

proc remap_all { } \
{
	#debug_proc remap_all entry

	global win_map_list

	set save_list $win_map_list

	set win_map_list ""

	foreach w $save_list \
		{ wm deiconify $w }

	foreach w $save_list \
		{ raise $w }

	#debug_proc remap_all exit
}

proc sort_map_list { wlist } \
{
	#debug_proc sort_map_list entry

	set num [ llength $wlist ]

	set done 0

	while { $done == 0 } \
	{
		set done 1

		for { set i 0 } { $i < $num - 1 } { set i [ expr $i + 1 ] } \
		{
			set ip1 [ expr $i + 1 ]

			set w [ lindex $wlist $i ]
			set t [ lindex $wlist $ip1 ]

			set tw [ top_window $w $t ]

			if { $tw == $w } \
			{
				set wlist [ lreplace $wlist $i $ip1 $t $w ]

				set done 0
			}
		}
	}

	#debug_proc sort_map_list exit

	return $wlist
}

proc top_window { w1 w2 } \
{
	#debug_proc top_window entry

	set tw [ do_top_window $w1 $w2 ]

	if { $tw == "" } \
		{ set tw [ do_top_window $w2 $w1 ] }

	#debug_proc top_window exit

	return $tw
}

proc do_top_window { w1 w2 } \
{
	#debug_proc do_top_window entry

	set x1 [ winfo x $w1 ]
	set y1 [ winfo y $w1 ]

	set x2 [ winfo x $w2 ]
	set y2 [ winfo y $w2 ]

	set ht1 [ winfo height $w1 ]
	set wt1 [ winfo width $w1 ]

	set ht2 [ winfo height $w2 ]
	set wt2 [ winfo width $w2 ]

	if { $x1 <= $x2 && $x2 <= $x1 + $wt1 \
		&& $y1 <= $y2 && $y2 <= $y1 + $ht1 } \
	{
		#
		# ( x2, y2 ) inside w1
		#

		set tw [ winfo containing $x2 $y2 ]
	} \

	elseif { $x1 <= $x2 + $wt2 && $x2 + $wt2 <= $x1 + $wt1 \
		&& $y1 <= $y2 && $y2 <= $y1 + $ht1 } \
	{
		#
		# ( x2 + wt2, y2 ) inside w1
		#

		set tw [ winfo containing [ expr $x2 + $wt2 ] $y2 ]
	} \

	elseif { $x1 <= $x2 && $x2 <= $x1 + $wt1 \
		&& $y1 <= $y2 + $ht2 && $y2 + $ht2 <= $y1 + $ht1 } \
	{
		#
		# ( x2, y2 + ht2 ) inside w1
		#

		set tw [ winfo containing $x2 [ expr $y2 + $ht2 ] ]
	} \

	elseif { $x1 <= $x2 + $wt2 && $x2 + $wt2 <= $x1 + $wt1 \
		&& $y1 <= $y2 + $ht2 && $y2 + $ht2 <= $y1 + $ht1 } \
	{
		#
		# ( x2 + wt2, y2 + ht2 ) inside w1
		#

		set tw [ winfo containing \
			[ expr $x2 + $wt2 ] [ expr $y2 + $ht2 ] ]
	} \

	else \
		{ set tw "" }

	if { $tw != "" } \
		{ set tw [ winfo toplevel $tw ] }

	#debug_proc do_top_window exit

	return $tw
}

proc debug_proc { routine inout } \
{
	global proc_debug

	if { $proc_debug == "TRUE" } \
		{ puts "(proc debug: $routine{} $inout)" }
}
