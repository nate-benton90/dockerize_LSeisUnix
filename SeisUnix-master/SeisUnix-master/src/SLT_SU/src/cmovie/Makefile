CWP_INC = $(CWPROOT)/include
CWP_LIB = $(CWPROOT)/lib

MOTIF_INC = /usr/dt/include
MOTIF_LIB = /usr/dt/lib

X11_INC = /usr/openwin/include
X11_LIB = /usr/openwin/lib

COPT = -g
CFLAGS = $(COPT) -g $(CWP_FLAGS) $(IPATH) 

IPATH  = -I$(CWP_INC) -I$(MOTIF_INC) -I$(X11_INC)
LPATH  = -L$(CWP_LIB) -L$(MOTIF_LIB) -L$(X11_LIB) 

LDLIBS  =  -lpar -lcwp -lXm -lXt -lXmu -lXext -lX11 -lm
LDFLAGS = $(LPATH) $(LDLIBS) 

BIN = cmovie

OBJ = axis.o            \
      color.o           \
      colorbar.o        \
      data.o            \
      draw.o            \
      edit.o            \
      help.o            \
      main.o            \
      map.o             \
      movie.o           \
      pick.o            \
      pik.o             \
      plane.o           \
      region.o          \
      render.o          \
      section.o         \
      ui_canvas.o       \
      ui_menu.o         \
      ui_panel.o        \
      ui_window.o       \
      view.o            \

$(BIN): $(OBJ) 
	$(CC) $(CFLAGS) $(OBJ) -o  cmovie $(LDFLAGS)
	rm -f *.o

install: $(BIN)
	rm -f $(CWPROOT)/bin/$(BIN)
	cp $(BIN) $(CWPROOT)/bin/$(BIN)
