# README.spec --
#
#	This file defines the format of the tksu module specification file.
#
# Copyright (c) 2002 Henry Thorson Consulting.  All Rights Reserved.
# CVS: $id$


#--------------------------
# Module Specification File
#--------------------------

# A module specification (or `spec') file provides all the information tksu
# needs to know about a module in order to use it in a processing flowsheet.
# Each command-line parameter available to the module must be defined:  its
# name, type and the valid range for the assigned value.  Port specifications
# for stdin, stdout, and files named on the command line must also be defined.

# When tksu is started, it reads in as many spec files as it can find.  Each
# spec file must have a name ending in `.spec', and must be in a directory
# that tksu searches.  The default search directory is $TksuDir/specs, where
# TksuDir is tksu's installation directory.  More search directories may be
# specified by the user in two ways:  1) append them to the tcl variable
# `SpecPath' in the layout file `.tksu', or 2) name the directories in the
# environment variable TKSU_PATH, which has the same format as the shell
# PATH variable.

# A spec file is an ascii file consisting of one or more `sections'.  Each
# section is headed by a name enclosed in brackets `[]'.  Currently, sections
# may be one of two types:  module sections and enumeration sections.
# Comment lines begin with a pound sign `#` and may be placed anywhere in
# the file.


#---------------
# Module Section
#---------------

# A module section defines the parameters and ports for one module.  The
# section header must contain the module name enclosed in brackets.  The
# module name is the actual name of the executable file, without any leading
# directory path.  It is assumed that the executable can be found in one of
# the directories named in the PATH environment variable.  The module section
# may contain the following lines:

# Description Line
#
#   Synopsis:	Desc: <one-line description of the module>
#
#   The description line is required.  The description following the `Desc:'
#   keyword will be placed in the Module List, so it should be a short one-
#   line description that does not exceed 80 characters.

# Category Line
#
#   Synopsis:	Cat: <category>
#
#   Add a category line for each category that the module is to be listed
#   under.  The <category> may be a multi-word description.  The line is
#   optional; if none is given, the module will be added to the
#   `Miscellaneous' category.

# Trans Line
#
#   Synopsis:	Trans: <translation-procedure>
#
#   There may optionally be one Trans line in a module section.  It names
#   a Tcl/Tk procedure that should be invoked to edit the command line
#   parameters for the module if they need special treatment.  Currently the
#   following procedures are supported:
#
#	PositionalArgs	Any parameter in the form `name-n=value' (n is an
#			integer) is taken to be a positional parameter.
#			The argument is converted to `value' before being
#			written to the command line.
#
#	SortArgs	Special handling for susort arguments.

# DocCmd Line
#
#   Synopsis:	DocCmd: <documentation command>
#
#   The DocCmd value should be an executable command that delivers
#   documentation on the module to stdout (or delivers a warning that there
#   is no help for the module).  DocCommand is optional and rarely used.
#   The default action for self-documentation is to try sudoc first, and if
#   that fails, run the module without arguments.

# Port Line
#
#   Synopsis:	Port: <name> <type> <rw> <default> <desc>
#
#   Add a port line for each input and output port defined for the module.
#   A port is one of the unnamed pipes stdin or stdout, or it is file input/
#   output defined by a parameter name.
#
#   <name>	Must be stdin, stdout, or a parameter that is to name a file.
#
#   <type>	Defines the type of data passed through the port, or the
#		contents of the file.  The <type> must be one of the known
#		file types defined in the `enum-file' enumeration list.  It
#		is safe to add new types to enum-file if necessary.  See
#		file enum.spec for the definition of enum-file.
#
#   <rw>	Defines the I/O direction for the port:
#
#		`r'	Read
#		`w'	Write
#		`rw'	Read/write or random access
#
#		A random access port can only have a filename assigned to it;
#		it cannot be linked to another port.
#
#   <default>	A word or phrase that describes the default value to use for
#		the port.  If it is a phrase, it must be enclosed in quotes
#		(e.g. "aaa bbb").  A user specifies the default by leaving
#		or setting the port value blank.  For ports, the <default>
#		value is usually a filename.  However, the following special
#		settings for <default> are recognized:
#
#		`-'	The port is optional and will not appear on the
#			command line if not set.
#		`req'	A value for this port is REQUIRED:  the user must
#			either provide a filename or link it to another port.
#
#   <desc>	A brief one-line description for the port that appears in
#		the Parameter List.

# Parameter Line
#
#   Synopsis:	Param: <name> <type> <range> <default> <desc>
#
#   Add a parameter line for each possible parameter that may appear on the
#   command line.  The standard format for a command-line parameter in SU
#   is a `name=value' pair, as dictated by getpar.
#
#   <name>	Defines the parameter name.  For some SU modules, a parameter
#		name may be one of any in an enumeration (such as a trace
#		header field).  In this special case, set <name> to the
#		enumeration name (e.g. `enum-thed'), and if necessary, define
#		the enumeration in a new Enumeration Section.
#
#		If parameter name is prefixed with `dup-', multiple
#		instances of the parameter may appear on the command line.
#		The `Dup' button in the parameter dialog is enabled so that
#		multiple copies of the parameter may be created and edited.
#
#   <type>	The type of value the parameter may take:
#
#		`int'	  Integer
#		`float'	  Floating point value
#		`string'  Arbitrary string.  If the user enters a multi-word
#			  string, it will be enclosed in quotes.
#		`enum-X'  Any enumeration (replace `enum-X' with the name of
#			  the enumeration).  The parameter value may take on
#			  any one of the enumerated values.  If necessary,
#			  define the enumeration in a new Enumeration Section.
#
#		If <type> has `-list' appended to it, then this parameter
#		may consist of a multi-valued list, such as `freq=0,10,50,60'.
#		The value list will be comma-delimited.  Each member of the
#		list is to have the specified <type>.
#
#   <range>	Defines the valid range for a parameter of type int or float.
#		A range has the format <low><sep><high>, where
#
#		<low>	= lower limit, or null if no lower limit,
#		<sep>	= separator: `<=' indicates the value may equal the
#			  lower or upper limit, and `<' indicates the value
#			  may not attain the lower or upper limit.
#		<high>	= upper limit, or null if no upper limit.
#
#		The special range `-' indicates no limits (aside from the
#		maximum range for an integer or float).  Use `-' for
#		parameters that are not of int or float type.  For example:
#
#		`1<=5'	indicates 1 to 5 inclusive,
#		`1<='	indicates the value must be 1 or greater, 
#		`<=-2'	indicates the value must be -2 or less,
#		`0<'	indicates the value must be greater than 0,
#		`-'	indicates no limit to the value.
#
#   <default>	A word or phrase that describes the default value to use for
#		the parameter.  If it is a phrase, it must be enclosed in
#		quotes (e.g. "from header").  A user specifies the default by
#		leaving or setting the parameter value blank, in which case
#		the parameter will NOT appear on the module's command line.
#		However, the following special settings for <default> are
#		recognized:
#
#		`-'	The parameter is optional and will be ignored if not
#			set.
#		`req'	A value for the parameter is not given but it is
#			REQUIRED:  the user must provide a value.
#
#   <desc>	A brief one-line description for the parameter that appears
#		in the Parameter List.

# Shared Line
#
#   Synopsis:	Shared: <shared-section>
#
#   If a Shared line is encountered, all parameters defined in the given
#   shared section are inserted at this point.  The shared section must have
#   been defined previous to this, and has a name that begins with `shared-'.

# LDef Line
#
#   Synopsis:	LDef: <long description of default value>
#
#   The LDef line provides an expanded description of the default value
#   for the preceding parameter or port.  The `short' default value
#   defined in the Param or Port line is displayed in the Parameter List,
#   whereas the description defined here is displayed in the Parameter/Port
#   Dialog where room is available for an expanded explanation.  This line is
#   optional.  If not given, the short default value is displayed in both
#   places.

# LDesc Line
#
#   Synopsis:	LDesc: <long description of parameter/port>
#
#   The LDesc line provides a longer description of the preceding parameter
#   or port.  The `short' description defined in the Param or Port line is
#   displayed in the Parameter List, whereas the description defined here is
#   displayed in the Parameter/Port Dialog where there is room for more
#   detailed documentation on the parameter.  This line is optional.  More
#   than one line may be given for the same parameter -- the multi-line
#   description is concatenated together.  If no detail lines are present,
#   the parameter's short description is displayed in both places of
#   the Parameter List Window.


#-----------------------
# Module Section Example
#-----------------------

[segyread]

Cat:	Conversion
Desc:	read SEGY tape
Port:	tape segy r req		SEGY file input
Port:	stdout su w req 	trace output
Port:	hfile ahed w header	ascii header output
Port:	bfile bhed w binary	binary header output
Port:	par par r -		optional par file input
Param:	verbose int 0<=1 0	1 = report every vblock traces
Param:	vblock int 0<= 50	trace report interval
Param:	buff int 0<=1 1		1 = buffered device
Param:	over int 0<=1 0		1 = override bad bhed format value
Param:	format int - 0		format value to use when over=1
Param:	conv int 0<=1 1		1 = convert samples to IEEE format
Param:	ns int 1<= "from header"  samples per trace
Param:	trmin int 1<= 1		first trace to read
Param:	trmax int 1<= -		last trace to read
Param:	endian int 0<=1 1	0 = little-endian machine
Param:	errmax int 0<= 0	number of consecutive tape IO errors allowed
Param:	remap enum-thed-list - -  header fields to remap to
Param:	byte string-list - -	header fields (and formats) to remap from


#--------------------
# Enumeration Section
#--------------------

# An enumeration list is defined by the special section `[enum-<name>]'
# where <name> is the name of the enumeration.  The list should begin with
# one description line:
#
# Desc:    <a short description of the enumeration>
#
# Subsequent lines have the format:
#
# <value>  <a short description of the enumeration value>
#
# The <value> and <description> fields appear in a listbox when the
# enumeration is displayed.
#
# If a parameter type is specified as an enumeration, the possible values the
# parameter may take are defined by the enumeration.  An enumeration need not
# be defined in just one section -- all Enumeration Sections with the same
# name are concatenated together to form the list of possible values.
#
# See file enum.spec for an example of some important enumerations.


#---------------
# Shared Section
#---------------

# A shared section is defined with the name `[shared-<name>]'.  Only parameter
# lines (or other shared lines) may be added to a shared section.  When a
# Shared line is encountered in a module section or another shared section,
# the contents of the referenced shared section are inserted.

