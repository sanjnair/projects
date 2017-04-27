TARGET =	prj3
LIBS =		-lsimple -lsuif
OBJS =		main.o bitvector.o print.o bb1.o mysimple.o expression.o vertex.o digraph.o myprint.o prjutil.o duchain.o web.o regspec.o adjmatrix.o prj3.o 
HEADERS =       digraph.h bb1.h prj3.h vertex.h prjutil.h mysimple.h expression.h myprint.h bitvector.h prjsupport.h duchain.h web.h regspec.h adjmatrix.h

INCLDIR =	$(SUIFHOME)/include
CC =		gcc 
C++ =		g++ 
CFLAGS =	-g -I$(INCLDIR)
SHELL =		/bin/sh
LIBDIR =	$(SUIFHOME)/$(MACHINE)/lib

.cc.o:
		$(C++) -c $(CFLAGS) -DDEF_VERSION $<
.c.o:
		$(CC) -c $(CFLAGS) $<
all:		$(TARGET) 


$(TARGET):	$(OBJS)
		$(C++) -o $(TARGET) $(CFLAGS) $(OBJS) -L$(LIBDIR) $(LIBS)
clean:
		-rm -f $(OBJS) $(TARGET)

