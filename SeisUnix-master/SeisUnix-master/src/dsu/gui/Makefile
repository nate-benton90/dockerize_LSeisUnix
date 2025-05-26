#
# GUI Makefile. 08/20/95. AEM
#               03/06/96. AEM
#

#Root directory for TCL & TK libraries
TCLTKROOT = /usr/local

#Root directory for dsu
DSUROOT	 = $(CWPROOT)/src/dsu

#
# Compiler Options
#

#COMPILEROPTIONS = -O
COMPILEROPTIONS = -g

CC = cc

CFLAGS = $(COMPILEROPTIONS)

#
# Source Directory
#

SRCDIR = $(DSUROOT)/gui
INCDIR = $(DSUROOT)/include

#
# Libraries
#

TCLLIBDIR = -L$(TCLTKROOT)/lib/tcl7.4
TKLIBDIR =  -L$(TCLTKROOT)/lib/tk4.0
PVMLIBDIR = -L$(PVM_ROOT)/lib/$(PVM_ARCH)
DSULIBDIR = -L$(DSUROOT)/lib
XLIBDIR =   -L/usr/X11R6/lib
LIBDIRS =   $(TCLLIBDIR) $(TKLIBDIR) $(PVMLIBDIR) $(XLIBDIR) $(DSULIBDIR)
SYSLIBS =   $(ARCHLIB) -lm
PVMGLIB = -lgpvm3
PVMLIB =  -lpvm3
DSULIB =  -ldsu
TCLLIB =  -ltcl7.4
TKLIB = -ltk4.0
XLIB = -lX11 

LIBS = $(LIBDIRS) $(PVMLIB) $(PVMGLIB) $(DSULIB) \
	$(TKLIB) $(TCLLIB) $(XLIB) $(SYSLIBS)

#
# Include Directories
#

PVMINCL = 	-I$(PVM_ROOT)/include -I$(CWPROOT)/include
TCLINCL = 	-I$(TCLTKROOT)/include
TKINCL 	= 	-I$(TCLTKROOT)/include
XINCL 	= 	-I/usr/X11R6/include/X11
INCL 	= 	-I$(INCDIR) $(PVMINCL) $(TKINCL) $(TCLINCL) $(XINCL)

#
# Object Files
#

OBJS = 	dsu.o commands.o dsuutil.o graph_handle.o graph_draw.o graph_ascii.o graph_exec.o dsuinfo.o dsupvm.o

HEADERS = $(INCDIR)/dsu.h $(INCDIR)/const.h $(PVM_ROOT)/include/pvm3.h \
	  $(INCDIR)/graph_ascii.h $(INCDIR)/hash.h $(CWPROOT)/include/su.h

#
# Targets
#

default: INSTALL
remake:  clean INSTALL

INSTALL: dsu
	@ echo dsu installed in `pwd`
#@ cp dsu $(CWPROOT)/bin
#@ rm dsu
#@ rm dsu $(OBJS)

dsu: $(OBJS)
	$(CC) -o dsu $(CFLAGS) $(OBJS) $(LIBS)

objs: $(OBJS)

clean:
	rm -f *.o dsu

#
# C Dependencies
#

dsu.o: $(SRCDIR)/dsu.c $(HEADERS)
	@ touch dsu.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/dsu.c

commands.o: $(SRCDIR)/commands.c $(HEADERS)
	@ touch commands.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/commands.c

pvm.o: $(SRCDIR)/pvm.c $(HEADERS)
	@ touch pvm.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/pvm.c

dsuutil.o: $(SRCDIR)/dsuutil.c $(HEADERS)
	@ touch dsuutil.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/dsuutil.c

graph_handle.o: $(SRCDIR)/graph_handle.c $(HEADERS)
	@ touch graph_handle.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/graph_handle.c

graph_draw.o: $(SRCDIR)/graph_draw.c $(HEADERS)
	@ touch graph_draw.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/graph_draw.c

graph_ascii.o: $(SRCDIR)/graph_ascii.c $(HEADERS)
	@ touch graph_ascii.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/graph_ascii.c

graph_exec.o: $(SRCDIR)/graph_exec.c $(HEADERS)
	@ touch graph_exec.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/graph_exec.c

dsuinfo.o: $(SRCDIR)/dsuinfo.c $(HEADERS)
	@ touch dsuinfo.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/dsuinfo.c

dsupvm.o: $(SRCDIR)/dsupvm.c $(HEADERS)
	@ touch dsupvm.o
	$(CC) $(CFLAGS) -c $(INCL) $(SRCDIR)/dsupvm.c
