#/*********************** self documentation **********************/
#/*
# * dsugraph.tcl - tcl/tk script to handle the graph representing
#                a SU sequence
# *
# */
#/**************** end self doc ********************************/
#
#/*
# * AUTHOR:  Alejandro E. Murillo, Colorado School of Mines, 03/03/96
# *
#*/
#
#/*
#
#Copyright statement:
#Copyright (c) Colorado School of Mines, 1995,1996
#All rights reserved.
#
#*/

# loadList --
# 	This procedure reloads the application 
#	listbox from the file: fn
#
# Arguments:
# w -			Name of the toplevel window of the demo.
# fn -			File Name containing labels

proc loadList {w fn} {

    $w.f.list delete 0 end
    set f [open $fn]
    while { [gets $f line] >= 0} {
        $w.f.list insert end [lindex [split $line .] 0]
    }
    close $f

}

proc loadDir {w directory} {

  $w.f.list delete 0 end
  foreach i [lsort [glob $directory/*]] {
    $w.f.list insert end [file tail $i]
  }
}


proc AddNode {w fork} {

  set GRAPH_C .dsu.graph.canvas
  global wanted
  global curDir
  global env

  catch {destroy $w}
  toplevel $w -class Dialog

  if { $fork == 0 } \
  {
    wm title $w "Add Seismic Unix applications"
    wm iconname $w "AddNode"
  }

  if { $fork == 1 } \
  {
    wm title $w "Add Seismic Unix applications FORK"
    wm iconname $w "FORK"
  }


  #label $w.msg -font $font -wraplength 4i -justify left
  label $w.msg -wraplength 4i -justify left \
	-text "This panel allows you to add applications to the current SU/DSU sequence. To select an application Click twice on it"
  pack $w.msg -side top

  frame $w.buttons
  pack  $w.buttons -side bottom -expand y -fill x -pady 2m
  button $w.buttons.dismiss -text Dismiss -command "destroy $w"

  set wanted  ""
  entry $w.buttons.wanted  -width 20 -textvariable wanted
  bind $w.buttons.wanted <Return> "locString $w"

  button $w.buttons.search -text "Search:" \
    	-command "locString $w"
  pack $w.buttons.dismiss $w.buttons.search $w.buttons.wanted \
	-side left -expand 1

  frame $w.spacer1 -height 3m -width 30
  label $w.fileLabel -text "DSU applications:"
  frame $w.f -borderwidth 10
  pack $w.spacer1 $w.fileLabel $w.f -side top -anchor center

  listbox $w.f.list -width 20 -height 10 -yscrollcommand "$w.f.scroll set" -setgrid 1
  scrollbar $w.f.scroll -command "$w.f.list yview"
  pack $w.f.list $w.f.scroll -side left -fill y -expand 1
 
  loadList $w $env(CWPROOT)/src/dsu/gui/applinfo.list
  
  if { $fork == 0 } \
  {
    bind $w.f.list <Double-1> { 
      set fn [selection get] 
      mkNodeC [lindex [split $fn .] 0] 0 
    } 
  }

  if { $fork != 0 } \
  {
    bind $w.f.list <Double-1> { 
      set fn [selection get] 
      mkNodeC [lindex [split $fn .] 0] 1 
    }
  }

}

#@@@@

proc updateArgv {w i value} {

  set w1 $w.frame.0
  set w2 $w.frame.1

  set w3 $w2
  if { ($i % 2) } {
    set w3 $w1
  }

  $w3.$i.entry insert 0 $value

}

#@@@@

proc showWin  {w} {
  wm deiconify $w
  raise $w
  return
}

proc createArgvWin {w label node args} {

  #
  # Create the window
  #

  catch {destroy $w}
  toplevel $w -class Dialog
  #wm title $w "Get parameters for $label (node $node)"
  wm title $w "Get parameters for $label"
  wm iconname $w "$label$node-Pars"
  wm withdraw $w

  #
  # Put top message
  #

  label $w.msg -wraplength 5i -justify left -text \
  	"Enter parameters for $label. You can type in the various entries and use tabs to move circularly between the entries."
  pack $w.msg -side top

  #
  # Fill button part (in the bottom)
  #

  frame $w.buttons -relief flat -bd 1

  button $w.buttons.dismiss -text Dismiss -command "wm withdraw $w"
  button $w.buttons.code -text "Save" -command "savePars $w $node"
  pack  $w.buttons.dismiss $w.buttons.code -side left -expand 1 

  button $w.buttons.stdinp -text "stdin"  -command "setstdio $node 0"
  button $w.buttons.stdout -text "stdout" -command "setstdio $node 1"
  button $w.buttons.hostname -text "Hostname" -command "setafield $node"
  button $w.buttons.options -text "Options" -command "setafield $node"
  pack $w.buttons.stdinp  $w.buttons.stdout -side left -expand 1
  pack $w.buttons.hostname  $w.buttons.options -side left -expand 1

  pack $w.buttons -side bottom -fill both

  #
  # Fill top part
  #

  frame $w.frame

  set w1 $w.frame.0
  set w2 $w.frame.1
  frame $w1 -width 30
  frame $w2 -width 30
  pack $w1 $w2 -side right -fill both

  set i 0
  foreach j $args {

    set par [split $j  = ]
    set lbl [lindex $par 0]
    set val [lindex $par 1]

    set w3 $w2
    if { ($i % 2) } {
      set w3 $w1
    }
    frame $w3.$i -bd 2
    entry $w3.$i.entry -relief sunken -width 30
    $w3.$i.entry insert 0 $val
    label $w3.$i.label -text "$lbl:" -width 10 
    pack $w3.$i.label $w3.$i.entry -side left -fill x -expand 1
    pack  $w3.$i -side top -fill x

    incr i

  }

  pack $w.frame -side top -expand yes -fill both

  bind $w <Return> "savePars $w $node"

}

proc savePars {w node} {

  set plist ""

  getafieldC $node 4 
  set npars $myresult

  for { set i 0 } {$i < $npars} {incr i}  {

    set w1 $w.frame.1
    if { ($i % 2) } {
      set w1 $w.frame.0
    }

    set parval [$w1.$i.entry get]
    if { $parval == ""} {set parval "-@-"}
    lappend plist $parval
    
  }
  saveParsC $node $plist
}
  
#
# Help stuff
#

proc buildHelp {w} {

  global env 
  catch {destroy $w}
  toplevel $w -class Dialog
  wm title $w "Seismic Unix Help"
  wm iconname $w "DSU Help"

  #label $w.msg -font $font -wraplength 4i -justify left
  label $w.msg -wraplength 4i -justify left \
        -text "This panel allows you to get help from DSU applications in general. Click twice on the application you need help with..."
  pack $w.msg -side top

  frame $w.buttons
  pack  $w.buttons -side bottom -expand y -fill x -pady 2m
  button $w.buttons.dismiss -text Dismiss -command "destroy $w"
  
  set wanted  "String to search"
  entry $w.buttons.wanted  -width 20 -textvariable wanted 
  bind $w.buttons.wanted <Return> "locString $w"

  button $w.buttons.search -text "Search:" -command "locString $w"
  pack $w.buttons.dismiss $w.buttons.search $w.buttons.wanted -side left -expand 1

  frame $w.spacer1 -height 3m -width 30
  label $w.fileLabel -text "Select an DSU application:"
  frame $w.f -borderwidth 10
  pack $w.spacer1 $w.fileLabel $w.f -side top -anchor center

  listbox $w.f.list -width 20 -height 10 -yscrollcommand "$w.f.scroll set" -setgrid 1
  scrollbar $w.f.scroll -command "$w.f.list yview"
  pack $w.f.list $w.f.scroll -side left -fill y -expand 1

  loadDir $w $env(CWPROOT)/src/doc/Stripped

  bind $w.f.list <Double-1> {
	set fn [selection get]
        set label [lindex [split $fn .] 0]
        showHelp .help$label $fn $label
  }

}

proc loadFile {w file} {

  $w delete 1.0 end
  set f [open $file]
  while {![eof $f]} {
    $w insert end [read $f 1000]
  }
  close $f
}
  
proc showCode {w label} {

  global env

  catch {destroy $w}
  toplevel $w -class Dialog
  wm title $w "$label Source Code"
  wm iconname $w "$label Source Code"

  label $w.msg -wraplength 4i -justify left \
        -text "Source Code window for $label"
  pack $w.msg -side top

  frame $w.buttons
  pack  $w.buttons -side bottom -expand y -fill x -pady 2m
  button $w.buttons.dismiss -text Dismiss -command "destroy $w"
  button $w.buttons.help -text "Help" -command "destroy $w"
  pack $w.buttons.dismiss $w.buttons.help -side left -expand 1

  text $w.text -relief sunken -bd 2 \
    -yscrollcommand "$w.scroll set" \
    -font -*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*
  
  scrollbar $w.scroll -command "$w.text yview"
  pack $w.scroll -side right -fill y
  pack $w.text -side left

  getsrcdirC $label
  set srcdir $myresult

  loadFile $w.text $env(CWPROOT)/src/$srcdir/$label.c
}

proc showHelp {w fn label} {

  global env

  catch {destroy $w}
  toplevel $w -class Dialog
  wm title $w "$label Help"
  wm iconname $w "$label Help"

  label $w.msg -wraplength 4i -justify left \
        -text "Help window for $label"
  pack $w.msg -side top

  frame $w.buttons
  pack  $w.buttons -side bottom -expand y -fill x -pady 2m
  button $w.buttons.dismiss -text Dismiss -command "destroy $w"
  button $w.buttons.help -text "See Code" -command "showCode .code$label $label"
  pack $w.buttons.dismiss $w.buttons.help -side left -expand 1

  text $w.text -relief sunken -bd 2 \
    -yscrollcommand "$w.scroll set" \
    -font -*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*
  
  scrollbar $w.scroll -command "$w.text yview"
  pack $w.scroll -side right -fill y
  pack $w.text -side left

  loadFile $w.text $env(CWPROOT)/src/doc/Stripped/$fn
}

proc locString {w} {
global wanted

  for {set i 0} {$i < [$w.f.list size]} {incr i} {
    set j [$w.f.list get $i] 
    if { [string first $wanted $j] != -1} {
      $w.f.list yview $i 
      return
    }
  }
}


# loadorsave -- Create a dialog window to obtain the name 
#		of the file to be used to load or save
# 		a sequence of SU applications

proc loadorsave {w what dirVar fileVar} {

  global wanted loadingDir savingDir

  set msg $what

  catch {destroy $w}
  toplevel $w
  wm title $w "$msg"
  wm iconname $w "$msg"

  label $w.msg  -wraplength 4i -justify left -text "This panel allows you to $what. Click twice on the filename/directory you want to use to $what"

  pack $w.msg -side top

  frame $w.buttons
  pack  $w.buttons -side bottom -expand y -fill x -pady 2m
  button $w.buttons.dismiss -text Dismiss -command "destroy $w"

  set wanted  "String to search"
  entry $w.buttons.wanted  -width 20 -textvariable wanted 
  bind $w.buttons.wanted <Return> "locString $w"

  button $w.buttons.search -text "Search:" -command "locString $w"
  pack $w.buttons.dismiss $w.buttons.search $w.buttons.wanted \
	-side left -expand 1

  label $w.dirLabel -text "Directory:"
  entry $w.dirName -width 30 -textvariable $dirVar

  frame $w.spacer0 -height 3m -width 20

  label $w.curFileL -text "Current file:"
  entry $w.curFileE -width 30 -textvariable $fileVar

  frame $w.spacer1 -height 3m -width 20
  label $w.fileLabel -text "File:"
  frame $w.f
  pack $w.dirLabel $w.dirName $w.curFileL $w.curFileE $w.spacer1 $w.fileLabel $w.f -side top -anchor w

  listbox $w.f.list -width 20 -height 10 -yscrollcommand "$w.f.scroll set"
  scrollbar $w.f.scroll -command "$w.f.list yview"
  pack $w.f.list $w.f.scroll -side left -fill y -expand 1
  
}

proc savingproc {} {

  global savingDir env curFile

  set w .savewin
  loadorsave $w "Save an DSU sequence" savingDir curFile

  if { $savingDir == "" } {set savingDir $env(CWPROOT)/src/dsu/demos}
  loadDir $w $savingDir

  bind $w.dirName <Return> {loadDir .savewin $savingDir}

  bind $w.curFileE <Return> {
    set absfn $savingDir/$curFile
    if { [confirm $absfn 2] }\
	{ 
	  saveGraphC $absfn;
#setMsg "Sequence saved in file: $absfn ";
	}
  }

  bind $w.f.list <Double-1> { 
    set absfn $savingDir/[selection get]
    if { [file type $absfn] == "directory" } {
        set savingDir $absfn
	loadDir .savewin $savingDir
    } else {
      set curFile [file tail $absfn]
      if { [confirm $absfn 2] }\
	{ 
	  saveGraphC $absfn;
#setMsg "Sequence saved in file: $absfn ";
	}
    }
  }

}

proc loadingproc {} {

  global loadingDir env

  set w .loadwin
  loadorsave $w "load a DSU sequence" loadingDir curFile

  if { $loadingDir == "" } { set loadingDir $env(CWPROOT)/src/dsu/demos}
  loadDir $w $loadingDir

  bind $w.dirName <Return> {loadDir .loadwin $loadingDir}

  bind $w.f.list <Double-1> {
    set absfn $loadingDir/[selection get]
    if { [file type $absfn] == "directory" } {
        set loadingDir $absfn
	loadDir .loadwin $loadingDir
    } else {
      set curFile [file tail $absfn]
      loadGraphC $absfn
    }
  }
}

proc setafield {node} { 

  # field: 2 - hostname, 3 - options

  global active_node hname_field option_field

  set active_node $node
  
  getafieldC $node 2
  set hname_field $myresult
  getafieldC $node 3
  set option_field $myresult
  getafieldC $node 5
  set appl_name $myresult

# Set up a window to get values for hostname and options

  set w .options
  catch {destroy $w}
  toplevel $w
  wm title $w "Enter options"
  wm iconname $w "$appl_name"

  label $w.msg -wraplength 4i -justify left \
	 -text "Enter hostname/options for: $appl_name"
  pack $w.msg -side top

  frame $w.buttons
  pack  $w.buttons -side bottom -expand y -fill x -pady 2m
  button $w.buttons.dismiss -text Dismiss -command "destroy $w"
  button $w.buttons.save -text "Save" \
  	-command {\
	   setafieldC $active_node 2 $hname_field;
	   setafieldC $active_node 3 "$option_field"
       	}
  pack $w.buttons.dismiss $w.buttons.save -side left -expand 1

  foreach i {hname_field option_field} {
    frame $w.$i -bd 2
    entry $w.$i.entry -relief sunken -width 40 -textvar $i
    label $w.$i.label
    pack $w.$i.entry -side right
    pack $w.$i.label -side left
  }

  $w.hname_field.label config -text Hostname:
  $w.option_field.label config -text Options:
  pack $w.msg $w.hname_field $w.option_field -side top -fill x

  bind $w <Return> { \
#setafieldC $active_node 2 $hname_field ;
#setafieldC $active_node 3 "\"$option_field\""
     setafieldC $active_node 2 $hname_field;
     setafieldC $active_node 3 "$option_field"
  }

}

proc setstdio {node field} { 

# field: 0 - stdin, 1 - stdout

  set w .stdiowin

  global stdioDir active_node active_field env curFile

#puts " Setting stdio $field - (0 stdin - 1 stdout) for node # $node"

  set active_node $node
  set active_field $field

  getafieldC $active_node $active_field
  set curFile [file tail $myresult]

  set what "INPUT"
  if { $active_field == 1 } {set what "OUTPUT"}

  loadorsave $w "Set standard $what" stdioDir curFile

  if { $stdioDir == "" } { set stdioDir $env(HOME) }
  loadDir $w $stdioDir

  bind $w.curFileE <Return> {
    if { $curFile == "" }\
      { 
         setafieldC $active_node $active_field "";
         prtTopMsg "nothing" $active_field;
      }\
    else \
     {
       if {$active_field == 1}\
        {
          set absfn $stdioDir/$curFile
          if { [confirm $absfn 1] }\
	    {
	      setafieldC $active_node $active_field $absfn;
              setMsg "Standard OUTPUT filename set to: $absfn ";
	    }
        }\
       else { prtwarning "Select an existent file for STDINP"}
     }
  }

  bind $w.dirName <Return> {loadDir .stdiowin $stdioDir}

  bind $w.f.list <Double-1> {
    set absfn $stdioDir/[selection get]
    if { [file type $absfn] == "directory" } {
        set stdioDir $absfn
	loadDir .stdiowin $stdioDir
    } else {
      if { [confirm $absfn $active_field] }\
	  { 
	    setafieldC $active_node $active_field $absfn;
	    set curFile [file tail $absfn]
            prtTopMsg $absfn $active_field;
	  }
    }
  }
}

#
# Dialogo procedure
#

proc confirm {filename option} { 

  switch $option {
    0 {set confirmsg  "Set standard input file name to: $filename ?"}
    1 {set confirmsg  "Set standard output file name to: $filename ?"}
    2 {set confirmsg  "Save the sequence in: $filename ?"}
    3 {set confirmsg  "Load a sequence from: $filename ?"}
  }
  after idle {.dialog1.msg configure -wraplength 7i}
  set i [tk_dialog .dialog1 "CONFIRM YOUR SELECTION" "$confirmsg"\
  info 0 OK Cancel]

  switch $i {
    0 {puts "You pressed OK"; return 1}
    1 {puts "You pressed Cancel"; return 0}
  }

#return $i
}

proc prtTopMsg {filename option} { 

  switch $option {
    0 {setMsg "STDINP filename set to: $filename"}
    1 {setMsg "STDOUT filename set to: $filename"}
  }

}

proc prtwarning {msg} { 

  set w .warningmsg
  catch {destroy $w}
  toplevel $w
  wm title $w "MESSAGE STATUS WINDOW"
  wm iconname $w "Status"
  
  frame $w.buttons
  pack  $w.buttons -side bottom -expand y -fill x -pady 2m
  button $w.buttons.dismiss -text Dismiss -command "destroy $w"
  pack $w.buttons.dismiss -side left -expand 1

  text $w.text -yscrollcommand "$w.scroll set" -setgrid true \
	-width 50 -height 5 -wrap word
  scrollbar $w.scroll -command "$w.text yview"
  pack $w.scroll -side right -fill y
  pack $w.text -expand yes -fill both

  # Set up display styles

  $w.text tag configure bold -font -*-Courier-Bold-O-Normal--*-120-*-*-*-*-*-*
  $w.text tag configure big -font -*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*
  $w.text tag configure verybig -font \
	  -*-Helvetica-Bold-R-Normal--*-240-*-*-*-*-*-*

  $w.text insert end "$msg. \n" center 

}
