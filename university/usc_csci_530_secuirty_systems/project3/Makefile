################################################################
# MAKE FILE for HW4
# @author - sanjay nair - sanjaysn@usc.edu
################################################################

OPENSSL_DIR     = /home/scf-22/csci551b/openssl
OPENSSL_LIB_DIR = $(OPENSSL_DIR)/lib
OPENSSL_INC_DIR = $(OPENSSL_DIR)/include
LIBS            = -lcrypto -lsocket
LIBDIR          = $(OPENSSL_LIB_DIR)
INCLDIR         = $(OPENSSL_INC_DIR)
C++             = g++
CC              = gcc
CFLAGS          = -g -Wall -I$(INCLDIR)
LD_FLAGS        = -L$(LIBDIR) $(LIBS)
SHLIBLDFLAGS	= -G $(LIBS)


TARGET          = hw4
TARGET_OBJS     = hw4.o hwconstant.o hwutil.o hwlibrary.o sha1data.o smartcardproxy.o basedump.o secretdump.o signdump.o verifydump.o desbase.o desencryptor.o desdecryptor.o desencdump.o desdecdump.o editordump.o

SMARTCARD_LIB   = libhw4sc.so
SMARTCARD       = smartcard
SMARTCARD_OBJS  = smartcard.o 

all:            $(TARGET) $(SMARTCARD)

$(TARGET): $(TARGET_OBJS)
	$(C++) -o $(TARGET) $(CFLAGS) $(TARGET_OBJS) $(LD_FLAGS) -ldl

$(SMARTCARD): $(SMARTCARD_OBJS)
	$(CC) -o $(SMARTCARD_LIB) $(CFLAGS) $(SMARTCARD_OBJS) $(SHLIBLDFLAGS) -lc

.cc.o:
	$(C++) -c $(CFLAGS) $<

.c.o:
	$(CC) -c $(CFLAGS) $<


clean:
	rm -f *.o $(SMARTCARD_LIB) $(TARGET)



