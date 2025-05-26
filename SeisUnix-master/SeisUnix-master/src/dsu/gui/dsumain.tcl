#/*********************** self documentation **********************/
#/*
# * dsumain.tcl - tcl/tk main script (create window interface widgets)
# *
# * NOTE: Portions were extracted from the source code of XPVM.
# * See:  XPVM (hhtp://www.netlib.edu/...)
# *
# */
#/**************** end self doc ********************************/
#
#/*
# * AUTHOR:  Alejandro E. Murillo, Colorado School of Mines, 03/03/96
# *
#*/
#

# Main TCL Source File
#

#
# Hide Main Window
#

wm withdraw .

#
# Setup Main DSU-GUI Directory
#

set dsu_directory $env(CWPROOT)/src/dsu/gui

#
# Source graph related TCL Procs
#

set ckdir_procs [ file exists dsugraph.tcl ]

if { $ckdir_procs == 1 } \
	{ source dsugraph.tcl } \
else \
	{ source $dsu_directory/dsugraph.tcl }

#
# Source Main TCL Procs
#

set ckdir_procs [ file exists dsuprocs.tcl ]

if { $ckdir_procs == 1 } \
	{ source dsuprocs.tcl } \
else \
	{ source $dsu_directory/dsuprocs.tcl }

#
# Source TCL Utility Procs
#

set ckdir_util [ file exists dsuutil.tcl ]

if { $ckdir_util == 1 } \
	{ source dsuutil.tcl } \
else \
	{ source $dsu_directory/dsuutil.tcl }

#
# Startup Message
#

puts ""

puts -nonewline "Initializing DSU ..."
flush stdout

#
# Proc Debug Flag
#

set proc_debug FALSE
#set proc_debug TRUE

#
# Get User Name
#

set user [ get_user_name ]

#
# Set up Spacing Constants
#

set main_height 800
set main_width 700
#set main_height 950
#set main_width 900

set row_height 20
set col_width 12
set scroll_width 20
set border_space 10
set frame_border 2

#
# General View Constants / Globals
#

set scroll_canvas_width 8000
set graph_active TRUE

set graph_scroll 10
set graph_size 2000
set graph_space 24


set min_graph_height [ expr (2 * $row_height) + (3 * $scroll_width) \
        + (2 * $border_space) ]
#
# Compatibility Globals
#
set XSCROLLINCREMENT -xscrollincrement
set YSCROLLINCREMENT -yscrollincrement
set FRAME_OFFSET 1
set SELECTOR -selectcolor

#030296set nnodes 0

#
# Create Toplevel Window
#

set main_geom "[ expr $main_width ]x[ expr $main_height ]+10+10"

toplevel .dsu
wm geometry .dsu $main_geom
wm withdraw .dsu

set min_main_height [ expr (3 * $row_height) + (3 * $border_space) \
	+ $min_graph_height ]

set min_main_width 400

wm minsize .dsu $min_main_width $min_main_height

set depth [ winfo depth .dsu ]

#
# Set up Colors
#

if { $depth == 1 } \
{
	set active_fg_color white
	set selector_color black
	set fg_color black
	set graph_running_color black
        set graph_idle_color white
        set graph_fg_color black

        set task_active_color black
        set task_nosetup_color white
        set task_setup_color white

} \
else \
{
	set active_fg_color purple
	set selector_color purple
	set fg_color blue
	set graph_running_color green
        set graph_idle_color yellow
        set graph_fg_color blue

        set task_active_color green
        set task_setup_color yellow
        set task_nosetup_color white
}

#
# Create Title & Version Number
#

#set title_str "D S U 1  [ title_info ]"
set title_str "CWP Distributed Seismic Unix (DSU) V1.0"
label .dsu.title -text $title_str -foreground $fg_color

#
# Create Info Message Label
#

label .dsu.message -text "Status:   Welcome to DSU" \
	-foreground $fg_color -relief sunken -anchor nw

set msg_list ""

#
# Place Title & Message
#

place .dsu.title -relx 0.5 -y $border_space -anchor n

set msg_y [ expr $row_height + (3 * $border_space / 2) ]
set msg_wt [ expr $main_width - (2 * $border_space) ]

place .dsu.message -x $border_space -y $msg_y -width $msg_wt

update

#
# Get Main Font for Entry Widgets
#

set main_font [ lindex [ .dsu.title configure -font ] 3 ]

#
# Create Console Buttons
#

button .dsu.dsusave	-text "Save" \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color \
	-command "savingproc"
restrict_bindings .dsu.dsusave ""

button .dsu.print -text "PRINT" -command "printGraphInfo" \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color
restrict_bindings .dsu.print ""

button .dsu.dsuload	-text "Load" \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color \
	-command "loadingproc"
restrict_bindings .dsu.dsuload ""

button .dsu.forkNode -text "Fork" -command "AddNode .forkadd 1" \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color
restrict_bindings .dsu.forkNode ""

button .dsu.addNode -text "Add" \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color \
	-command "AddNode .winadd 0"
restrict_bindings .dsu.addNode ""

button .dsu.delNode	-text "Delete" \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color \
	-command "delNodeC"
restrict_bindings .dsu.delNode ""

button .dsu.dsurun	-text "Execute" \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color \
	-command "raiseMenu .dsu.run_menu .dsu.dsurun .dsu {}"
#restrict_bindings .dsu.dsurun ""

button .dsu.pvm -text "PVM..." \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color \
	-command "raiseMenu .dsu.pvm_menu .dsu.pvm .dsu {}"

restrict_bindings .dsu.pvm ""

button .dsu.help -text "Help" \
	-padx 1 -pady 1 \
	-foreground $fg_color -activeforeground $active_fg_color \
	-command "buildHelp .winhelp"

restrict_bindings .dsu.help ""

#
# Place Console Command Buttons
#

set cons_y [ expr $msg_y + $row_height + $border_space ]

place .dsu.dsusave	-relx 0.025 -y $cons_y
place .dsu.print	-relx 0.100 -y $cons_y
place .dsu.dsuload	-relx 0.250 -y $cons_y
place .dsu.forkNode	-relx 0.350 -y $cons_y
place .dsu.addNode	-relx 0.450 -y $cons_y
place .dsu.delNode	-relx 0.525 -y $cons_y
place .dsu.dsurun	-relx 0.625 -y $cons_y
place .dsu.pvm		-relx 0.775 -y $cons_y
place .dsu.help		-relx 0.875 -y $cons_y

update

#
# Set Up Initial Main Panel Constants
#

set start_main_y [ expr [ below .dsu.dsurun ] + ($border_space / 2) ]

set graph_y $start_main_y
set graph_width $main_width
set graph_x 0
set height_left [ expr $main_height - $start_main_y ]
#set graph_height [ expr $height_left / 2 ] #AEM 12/03/95
set graph_height [ expr 3 * $height_left / 4 ]


#
# Tick
#

puts -nonewline "1"
flush stdout

#
# Create graph Canvas
#

set GRAPH_C .dsu.graph.canvas

set GRAPH_SBH .dsu.graph.horiz_sb
set GRAPH_SBV .dsu.graph.vert_sb

set graph_cheight [ expr $graph_height - $scroll_width \
	- (2 * $row_height) - (2 * $border_space) ]

set graph_cwidth [expr $graph_width - $scroll_width - (2 * $border_space)]

set graph_hheight $graph_size
set graph_hwidth $graph_size

frame .dsu.graph

restrict_bindings .dsu.graph ""

label .dsu.graph.title -text "DSU Sequence Graph" -foreground $fg_color

set pcoord [ expr $graph_size / 2 ]
set ncoord [ expr -1 * ( $graph_size / 2 ) ]

canvas $GRAPH_C -bd $frame_border -relief sunken \
	$XSCROLLINCREMENT $graph_scroll $YSCROLLINCREMENT $graph_scroll \
	-confine 0 -scrollregion "$ncoord $ncoord $pcoord $pcoord"

set bg_color [ $GRAPH_C config -background ]

set cmd "scrollCanvas $GRAPH_C $GRAPH_SBH \
        $graph_size graph_hwidth graph_cwidth graph_xview HORIZ center $graph_scroll"

scrollbar $GRAPH_SBH -orient horiz -bd $frame_border -relief sunken \
        -width $scroll_width -command $cmd

scrollInit $GRAPH_C $GRAPH_SBH $graph_size $graph_hwidth $graph_cwidth \
        graph_xview HORIZ low $graph_scroll

set cmd "scrollCanvas $GRAPH_C $GRAPH_SBV \
        $graph_size graph_hheight graph_cheight graph_yview VERT center $graph_scroll"

scrollbar $GRAPH_SBV -orient vert -bd $frame_border -relief sunken \
        -width $scroll_width -command $cmd

scrollInit $GRAPH_C $GRAPH_SBV $graph_size $graph_hheight $graph_cheight \
        graph_yview VERT center $graph_scroll

set cmd "runPvmC halt"

button .dsu.graph.quit -text "QUIT" -command $cmd \
        -padx 1 -pady 1 \
        -foreground $fg_color -activeforeground $active_fg_color


#
# Create graph Key
#

label .dsu.graph.active_label -text "Active" \
	-foreground $fg_color

label .dsu.graph.active_box -text "    " \
	-bd 1 -relief sunken -background $graph_running_color

label .dsu.graph.setup_label -text "Setup" \
	-foreground $fg_color

label .dsu.graph.setup_box -text "    " \
	-bd 1 -relief sunken -background $task_setup_color

label .dsu.graph.empty_label -text "No Setup yet" \
	-foreground $fg_color

label .dsu.graph.empty_box -text "    " \
	-bd 1 -relief sunken -background $task_nosetup_color

#
# layout_graph_panel
#

layout_graph_panel

#
# Flush Main DSU Window Creation
#

update

#
# Tick
#

puts -nonewline "-2"
flush stdout

#
# Create PVM Menu
#

set pvm_info_list \
	[ list \
		[ list "Reset PVM"		command	 reset \
			"Reset the PVM Virtual Machine, Kill All Tasks" ] \
		[ list "Halt PVM"	command	 halt \
		"Stop PVM, Kill PVM Tasks " ] \
		[ list "Add Host"		command	 add \
		"Add a host to the PVM Virtual Machine" ] \
		[ list "Delete Host"		command	 delete \
		"Delete a host from the PVM Virtual Machine" ] \
	]

makeMenu .dsu.pvm_menu pvmMenuProc $pvm_info_list lower "{}" none

#
# Create EXEC Menu
#

set run_info_list \
	[ list \
		[ list "Run  Distributed"		command	 distributed \
			"Execute the sequence in a Distributed environment" ] \
		[ list "Run  Local"		command	 local \
			"Execute the sequence in a Local environment" ] \
	]
makeMenu .dsu.run_menu runSeqC $run_info_list lower "{}" none


#
# Tick
#

puts -nonewline "-3"
flush stdout

#
# Add Canvas Event Handle Bindings
#

$GRAPH_C bind node <Double-1> {
    getParsC [$GRAPH_C find withtag current]
#$GRAPH_C itemconfigure current -background $task_setup_color
}

$GRAPH_C bind node <Button-1> {
    set_active_task [$GRAPH_C find withtag current]
}

$GRAPH_C bind node <Button-2> {
    set mcurX %x
    set mcurY %y
}
$GRAPH_C bind node <B2-Motion> {
  moveNodeC [$GRAPH_C find withtag current] [expr %x-$mcurX] \
                [expr %y-$mcurY]
  set mcurX %x
  set mcurY %y
}

$GRAPH_C bind node <Button-3> {
  show_task_help [$GRAPH_C find withtag current]
}

#bind .dsu <Configure>			"resize_main_panel"

#set win_map_list ""

#bind .dsu <Unmap>				"unmap_all"
#bind .dsu <Map>				"remap_all"

bind .dsu <Destroy>			"exit"

#
# Tick
#

puts -nonewline "-4"
flush stdout

#
# Main Panel
#

butt_help .dsu.dsusave button \
	"Save the SU sequence "

butt_help .dsu.print button \
	"Print the sequence of applications starting at the current node"

butt_help .dsu.dsuload button \
	"Load a previous created DSU sequence "

butt_help .dsu.forkNode button \
	"Add an application after the current creating a new branch"

butt_help .dsu.addNode button \
	"Add applications to the current SU sequence "

butt_help .dsu.delNode button \
	"Delete the current (green) application in the current SU sequence "

butt_help .dsu.dsurun button \
	"Executes the DSU sequence "

butt_help .dsu.pvm button \
	"Raise / Lower Reset Menu for handling PVM"

butt_help .dsu.help button \
	"Raise / Lower Help Menu for More Detailed Help Information"

#
# Graph View
#

butt_help .dsu.graph.active_label label \
	"This is the current Task (for deleting, adding, etc ...)"

butt_help .dsu.graph.active_box label \
	"This is the current Task (for deleting, adding, etc ...)"

butt_help .dsu.graph.setup_label label \
	"Parameters has been already setup"

butt_help .dsu.graph.setup_box label \
	"Parameters has been already setup"

butt_help .dsu.graph.empty_label label \
	"Parameters has NOT been setup yet"

butt_help .dsu.graph.empty_box label \
	"Parameters has NOT been setup yet"

butt_help .dsu.graph.quit quit \
	"Quit DSU, leave PVM running"

#
# End of Button Help
#

puts " Done."

#
# Actually Bring Up Main DSU Window
#

wm deiconify .dsu

#
# Initialize C TCL Globals
#

initialize_tcl_globals

#
# Reset Welcome Message
#

setMsg "Welcome to DSU"

update
