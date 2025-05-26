
#=======================================================================
#                        Executable Makefile
#-----------------------------------------------------------------------
# Define the source and documentation files for this software component
#-----------------------------------------------------------------------

RULES = /home/rbeards/rules

PRODUCT = cwp

COMPONENT = cmovie

HDR = axis.h            \
      color.h           \
      colorbar.h        \
      data.h            \
      draw.h            \
      edit.h            \
      grunge.h          \
      help.h            \
      main.h            \
      map.h             \
      movie.h           \
      pick.h            \
      pik.h             \
      plane.h           \
      rargs.h           \
      region.h          \
      render.h          \
      section.h         \
      ui.h              \
      ui_canvas.h       \
      ui_menu.h         \
      ui_panel.h        \
      ui_window.h       \
      view.h

SRC = axis.c            \
      color.c           \
      colorbar.c        \
      data.c            \
      draw.c            \
      edit.c            \
      help.c            \
      main.c            \
      map.c             \
      movie.c           \
      pick.c            \
      pik.c             \
      plane.c           \
      region.c          \
      render.c          \
      section.c         \
      ui_canvas.c       \
      ui_menu.c         \
      ui_panel.c        \
      ui_window.c       \
      view.c 

DOC = Makefile          \
      Logfile           \
      ReleaseNotes

EXE = $(COMPONENT) 

all: 	$(HDR) $(SRC) $(DOC)
	make $(EXE)
	make clean

CFLAGS = $(COPT) $(CWP_FLAGS) $(IPATH)
FFLAGS = $(FOPT)

#-----------------------------------------------------------------------
# Add the /app/SU include files to resolve gridhdr.h.  This should be
# removed once CWP/SU and Unocal/SU are merged.
#-----------------------------------------------------------------------

IPATH  = -I$(CWP_INC) -I/app/SU/slt/include -I$(MOTIF_INC) -I$(X11_INC)
LPATH  = -L$(CWP_LIB) -L$(MOTIF_LIB) -L$(X11_LIB) 

LDLIBS  = -lpar -lcwp -lXm -lXt -lXmu -lXext -lX11 -lm
LDFLAGS = $(LPATH) $(LDLIBS) 

#-----------------------------------------------------------------------
# Include the rulesets appropriate to this system and product
#-----------------------------------------------------------------------

include $(RULES)/standard.rules
include $(RULES)/$(SYSTEM_RULE)
include $(RULES)/$(PRODUCT).rules

