################################################################
# MAKE FILE for HW2
# @author - sanjay nair - sanjaysn@usc.edu
################################################################

OPENSSL_DIR =    /home/scf-22/csci551b/openssl

TARGET =	hw2
LIBS =          -lcrypto -lsocket
LIBDIR =        $(OPENSSL_DIR)/lib
INCLDIR =       $(OPENSSL_DIR)/include
OBJS = 		hw2.o hwconstant.o hwutil.o basedump.o  base64encdump.o base64decdump.o sha1dump.o md5dump.o desbasedump.o desencdump.o desdecdump.o sha1data.o md5data.o
C++ = 		g++
CFLAGS =	-g -Wall -I$(INCLDIR)
.cc.o:
		$(C++) -c $(CFLAGS) $<
all:		$(TARGET)

$(TARGET):	$(OBJS)
		$(C++) -o $(TARGET) $(CFLAGS) $(OBJS) -L$(LIBDIR) $(LIBS) 
clean:
		-rm -f ${OBJS} $(TARGET)

