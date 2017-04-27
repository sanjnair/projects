################################################################
# MAKE FILE for HW1
# @author - sanjay nair - sanjaysn@usc.edu
################################################################

TARGET =	hw1
OBJS = 		hw1.o hwconstant.o hwutil.o basedump.o base64dump.o hexdump.o base64enc.o base64dec.o 
C++ = 		g++
CFLAGS =	-g -Wall
.cc.o:
		$(C++) -c $(CFLAGS) $<
all:		$(TARGET)

$(TARGET):	$(OBJS)
		$(C++) -o $(TARGET) $(CFLAGS) $(OBJS) 
clean:
		-rm -f ${OBJS} $(TARGET)

