TARGET =	prj1
LIBS =		-lsimple -lsuif
OBJS =		main.o prj1.o bb1.o prjutil.o print.o digraph.o vertex.o
HEADERS =       digraph.h bb1.h prj1.h vertex.h prjutil.h

INCLDIR =	$(SUIFHOME)/include
CC =		gcc # -H
C++ =		g++ # -H
CFLAGS =	-g -I$(INCLDIR)
SHELL =		/bin/sh
LIBDIR =	$(SUIFHOME)/$(MACHINE)/lib

.c.o:
		$(CC) -c $(CFLAGS) $<
all:		$(TARGET) 

main.o:		main.cc
		$(C++) -c $(CFLAGS) -DDEF_VERSION main.cc

digraph.o:	digraph.cc
		$(C++) -c $(CFLAGS) -DDEF_VERSION digraph.cc

vertex.o:	vertex.cc
		$(C++) -c $(CFLAGS) -DDEF_VERSION vertex.cc

prj1.o:      	prj1.cc
		$(C++) -c $(CFLAGS) -DDEF_VERSION prj1.cc

prjutil.o:     	prjutil.cc
		$(C++) -c $(CFLAGS) -DDEF_VERSION prjutil.cc

$(TARGET):	$(OBJS)
		$(C++) -o $(TARGET) $(CFLAGS) $(OBJS) -L$(LIBDIR) $(LIBS)

clean:
		-rm -f $(OBJS) $(TARGET)

bb1.o:		bb1.c
print.o:        print.c print.h
