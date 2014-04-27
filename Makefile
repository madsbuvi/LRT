CC = g++
CFLAGS = -g3
DEFS = -DDEBUG=10
BFLAGS = --debug
CLFLAGS = -I"C:\Program Files (x86)\AMD APP SDK\2.9\include" -L"C:\Program Files (x86)\AMD APP SDK\2.9\lib\x86"
LFLAGS = -lpthread -lOpenCL
# LGTK = -LD:/usr/dev/lib -lgtk-win32-2.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lgtksourceview-2.0 -lintl
# GTKFLAGS =  -I"D:\usr\dev\include\gtksourceview-2.0" -ID:/usr/dev/include/gtk-2.0 -ID:/usr/dev/lib/gtk-2.0/include -ID:/usr/dev/include/atk-1.0 -ID:/usr/dev/include/cairo -ID:/usr/dev/include/gdk-pixbuf-2.0 -ID:/usr/dev/include/pango-1.0 -ID:/usr/dev/include/glib-2.0 -ID:/usr/dev/lib/glib-2.0/include -ID:/usr/dev/include

TARG = LemmingsRT.exe
OBJS = main.o clstuff.o

all	: $(TARG)

gui : $(GUI_TARG)

run : $(TARG)
	$(TARG)

%.c: %.y
%.c: %.l

.cpp.o	:
	$(CC) $(CFLAGS) $(CLFLAGS) $(DEFS) -c $< 

.S.o	:
	$(CC) $(CFLAGS) $(CLFLAGS) $(DEFS) -c $<

$(TARG): $(OBJS)
	$(CC) $(CFLAGS) $(CLFLAGS) $(DEFS) -o $(TARG) $(OBJS) $(LFLAGS)

clean	:
	rm -f $(OBJS) $(TARG) *.bak
