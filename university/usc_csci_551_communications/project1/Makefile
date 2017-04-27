################################################################
# MAKE FILE for CSCI551 Project
# @author - sanjay nair - sanjaysn@usc.edu
################################################################

LIBS            = -lpthread -lsocket -lnsl -lcrypto
LIBDIR          = . 
INCLDIR         = . 
SSL_DIR 	= /usr/lsd/openssl/0.9.7g
SSL_LIBDIR	= $(SSL_DIR)/lib
SSL_INCLDIR	= $(SSL_DIR)/include

C++             = g++
CC              = gcc
CFLAGS          = -g -Wall -I$(INCLDIR) -I$(SSL_INCLDIR)
LD_FLAGS        = -L$(LIBDIR) $(LIBS)

UTIL_LIB        = libutil.a
UTIL            = util
UTIL_OBJS       = cabstractsocket.o cmutex.o ctcpsocket.o cconst.o cutil.o cexception.o ctcpserver.o cwaitcondition.o cmutexlocker.o cabstractfstream.o cofstream.o cifstream.o cfile.o cmd5data.o cthread.o
UTIL_LD_FLAGS   = -l$(UTIL)

SERVER		= server
SERVER_OBJS	= cserver.o servermain.o

CLIENT		= client
CLIENT_OBJS	= cclient.o clientmain.o

all:            $(SERVER) $(CLIENT)

$(UTIL):        $(UTIL_OBJS)
		ar rc $(UTIL_LIB) $(UTIL_OBJS)

$(SERVER):     	$(UTIL) $(SERVER_OBJS) 
		$(C++) -o $(SERVER) $(CFLAGS) $(SERVER_OBJS) $(LD_FLAGS) $(UTIL_LD_FLAGS) -ldl 

$(CLIENT):     	$(UTIL) $(CLIENT_OBJS) 
		$(C++) -o $(CLIENT) $(CFLAGS) $(CLIENT_OBJS) $(LD_FLAGS) $(UTIL_LD_FLAGS) -ldl 

.cc.o:
	$(C++) -c $(CFLAGS) $<

.c.o:
	$(CC) -c $(CFLAGS) $<


clean:
	rm -f *.o $(UTIL_LIB) $(SERVER) $(CLIENT)

