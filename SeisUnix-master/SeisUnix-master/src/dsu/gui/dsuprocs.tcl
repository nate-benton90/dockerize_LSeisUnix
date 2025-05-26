#/*********************** self documentation **********************/
#/*
# * dsuprocs.c - tcl/tk script commands
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

# 	Interactive graph editor procedures
#

#
# Layouts
#
proc layout_main_panel { } \
{
	#debug_proc layout_main_panel entry

	global border_space
	global scroll_width
	global main_height
	global main_width
	global graph_active
	global graph_height
	global graph_width
	global graph_y

	#
	# Status Message Label
	#

	set msg_wt [ expr $main_width - (2 * $border_space) ]

	place .dsu.message -width $msg_wt

	#
	# Layout graph View
	#

	if { $graph_active == "TRUE" } \
	{
		layout_graph_panel
		setGraphView

	} \

	else \
		{ place forget .dsu.graph }

	update

	#debug_proc layout_main_panel exit
}

proc layout_graph_panel { } \
{
	#debug_proc layout_graph_panel entry

	global frame_border
	global border_space
	global scroll_width
	global graph_cheight
	global graph_hheight
	global graph_cwidth
	global graph_hwidth
	global graph_scroll
	global graph_active
	global graph_height
	global graph_width
	global graph_xview
	global graph_yview
	global graph_size
	global graph_x
	global graph_y

	set GRAPH_C .dsu.graph.canvas
	set GRAPH_SBH .dsu.graph.horiz_sb
	set GRAPH_SBV .dsu.graph.vert_sb

	#
	# Top Titles
	#

	place .dsu.graph -x $graph_x -y $graph_y \
		-width $graph_width -height $graph_height

	place .dsu.graph.title -relx 0.5 -y 0 -anchor n

	#
	# Bottom Buttons
	#

	set kly [ expr $graph_height - ($border_space / 2) ]

	place .dsu.graph.quit -x $border_space -y $kly -anchor sw

	set kky [ expr $kly - $frame_border ]

	place .dsu.graph.active_label -relx 0.21 -y $kky -anchor sw
	place .dsu.graph.setup_label -relx 0.44 -y $kky -anchor sw
	place .dsu.graph.empty_label -relx 0.70 -y $kky -anchor sw

	update

	set sz [ expr [ winfo height .dsu.graph.active_label ] - 2 ]

	set kky [ expr $kky - 2 ]

	place .dsu.graph.active_box \
		-x [ right .dsu.graph.active_label ] -y $kky -anchor sw \
		-width $sz -height $sz

	place .dsu.graph.setup_box \
		-x [ right .dsu.graph.setup_label ] -y $kky -anchor sw \
		-width $sz -height $sz

	place .dsu.graph.empty_box \
		-x [ right .dsu.graph.empty_label ] -y $kky -anchor sw \
		-width $sz -height $sz

	update

	#
	# Canvas & Scrollbars
	#

	set graph_cy [ below .dsu.graph.title ]

	set graph_cheight [ expr [ above .dsu.graph.quit ] - $graph_cy \
		- $scroll_width - ($border_space / 2) ]

	set graph_cwidth [ expr $graph_width - $scroll_width \
		- (2 * $border_space)]

	set graph_sbx $border_space

	place $GRAPH_SBV -x $graph_sbx -y $graph_cy \
		-width $scroll_width -height $graph_cheight

	if { $graph_hheight < $graph_cheight } \
		{ set yview 0 } \
	
	else \
	{
		set yview [ expr $graph_yview \
			- ( ( ($graph_size / 2) - ($graph_hheight / 2) ) \
				/ $graph_scroll ) ]

		if { $yview < 0 } { set yview 0 }
	}

	scrollSet $GRAPH_SBV $graph_hheight $graph_cheight $graph_scroll $yview

	set graph_cx [ expr $graph_sbx + $scroll_width ]

	place $GRAPH_C -x $graph_cx -y $graph_cy \
		-width $graph_cwidth -height $graph_cheight

	set graph_sby [ expr $graph_cy + $graph_cheight ]

	place $GRAPH_SBH -x $graph_cx -y $graph_sby \
		-width $graph_cwidth -height $scroll_width

	if { $graph_hheight < $graph_cheight } \
		{ set xview 0 } \
	
	else \
	{
		set xview [ expr $graph_xview \
			- ( ( ($graph_size / 2) - ($graph_hwidth / 2) ) \
				/ $graph_scroll ) ]

		if { $xview < 0 } { set xview 0 }
	}

	scrollSet $GRAPH_SBH $graph_hwidth $graph_cwidth $graph_scroll $xview

	update

	#debug_proc layout_graph_panel exit
}

#
# Start Proc grahpview
#

proc setGraphView { } \
{
	#debug_proc setNetworkView entry

	global graph_cheight
	global graph_hheight
	global graph_cwidth
	global graph_hwidth
	global graph_scroll
	global graph_xview
	global graph_yview
	global graph_size

	set GRAPH_C .dsu.graph.canvas

	#
	# Set Graph View Location
	#

	set scroll_yview [ expr $graph_yview \
		- [ scrollCenterValue $graph_size $graph_hheight $graph_scroll ] ]

	set max_yview \
		[ scrollMaxValue $graph_hheight $graph_cheight $graph_scroll ]

	if { $scroll_yview < 0 || $max_yview < 0 || \
		( $max_yview >= 0 && $scroll_yview > $max_yview ) } \
	{
		set graph_yview \
			[ scrollCenterValue $graph_size $graph_cheight $graph_scroll ]

		#AEMdo_yview $GRAPH_C $graph_yview
		$canvas yview moveto 0.0
        	$canvas yview scroll $graph_yview units
	}

	set scroll_xview [ expr $graph_xview \
		- [ scrollCenterValue $graph_size $graph_hwidth $graph_scroll ] ]

	set max_xview \
		[ scrollMaxValue $graph_hwidth $graph_cwidth $graph_scroll ]

	if { $scroll_xview < 0 || $max_xview < 0 || \
		( $max_xview >= 0 && $scroll_xview > $max_xview ) } \
	{
		set graph_xview \
			[ scrollCenterValue $graph_size $graph_cwidth $graph_scroll ]

		#AEMdo_xview $GRAPH_C $graph_xview
		$canvas xview moveto 0.0
        	$canvas xview scroll $graph_xview units
	}

	#
	# Set Network Scrollbars
	#

	if { $graph_hheight < $graph_cheight } \
		{ set yview 0 } \
	
	else \
	{
		set yview [ expr $graph_yview \
			- ( ( ($graph_size / 2) - ($graph_hheight / 2) ) \
				/ $graph_scroll ) ]

		if { $yview < 0 } { set yview 0 }
	}

	if { $graph_hwidth < $graph_cwidth } \
		{ set xview 0 } \
	
	else \
	{
		set xview [ expr $graph_xview \
			- ( ( ($graph_size / 2) - ($graph_hwidth / 2) ) \
				/ $graph_scroll ) ]

		if { $xview < 0 } { set xview 0 }
	}

	scrollSet .dsu.graph.vert_sb \
		$graph_hheight $graph_cheight $graph_scroll $yview

	scrollSet .dsu.graph.horiz_sb \
		$graph_hwidth $graph_cwidth $graph_scroll $xview

	#debug_proc setNetworkView exit
}
#
# End Proc graphview
#


proc fix_menus_resize { } \
{
	#debug_proc fix_menus_resize entry

	replaceMenu .dsu.hosts_menu .dsu.hosts .dsu

	replaceMenu .dsu.tasks_menu .dsu.tasks .dsu

	replaceMenu .dsu.views_menu .dsu.views .dsu

	replaceMenu .dsu.reset_menu .dsu.reset .dsu

	replaceMenu .dsu.help_menu .dsu.help .dsu
	replaceMenu .dsu.tasks_help_menu .dsu.help .dsu
	replaceMenu .dsu.views_help_menu .dsu.help .dsu

	update

	replaceSubMenu .dsu.spawn .dsu.tasks_menu \
		.dsu.tasks_menu.butt_SPAWN .dsu.spawn.cmd_entry

	replaceSubMenu .dsu.systasks_menu .dsu.tasks_menu \
		.dsu.tasks_menu.butt_SYS_TASKS none

	update

	#debug_proc fix_menus_resize exit
}

#
# Compatibility Procs
#

proc restrict_bindings { win extra } \
{
        #debug_proc restrict_bindings entry

        bindtags $win "$win $extra"

        #debug_proc restrict_bindings exit
}

#
# temporary procs
#
proc quit_but {} {
  exit
}


#
# Added 04/11/96
#

proc onlyAdd {} {
  global hname_field
  do_add_delC $hname_field add
}

proc onlyDel {} {
  global hname_field
  do_add_delC $hname_field delete
}

proc adddelhost {what theproc} {

  # what: add, delete

  global hname_field

# Set up a window to get values for hostname 

  set w .adddel
  catch {destroy $w}
  toplevel $w
  wm title $w "Enter the name of a host to $what"
  wm iconname $w "AddDelHost"
  set hname_field ""

  label $w.msg -wraplength 4i -justify left \
         -text "Enter hostname to $what"
  pack $w.msg -side top

  frame $w.buttons
  pack  $w.buttons -side bottom -expand y -fill x -pady 2m
  button $w.buttons.dismiss -text Dismiss -command "destroy $w"
  button $w.buttons.ok -text "Ok" -command "$theproc"
        
  pack $w.buttons.dismiss $w.buttons.ok -side left -expand 1

  foreach i {hname_field} {
    frame $w.$i -bd 2
    entry $w.$i.entry -relief sunken -width 40 -textvar $i
    label $w.$i.label
    pack $w.$i.entry -side right
    pack $w.$i.label -side left
  }

  $w.hname_field.label config -text Hostname:
  pack $w.msg $w.hname_field -side top -fill x

  bind $w <Return> "$theproc"

}

proc pvmMenuProc { arg1 arg2 arg3 arg4} {

  if { ($arg1 == "reset") || ($arg1 == "halt") } {runPvmC $arg1}
  if { ($arg1 == "delete") } 	{adddelhost DELETE onlyDel}
  if { ($arg1 == "add") } 	{adddelhost ADD onlyAdd}

}

