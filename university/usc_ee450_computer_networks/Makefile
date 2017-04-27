################################################################
# MAKE FILE for EE450 Project
# @author - sanjay nair - sanjaysn@usc.edu
################################################################

LIBS            = -lsocket -lnsl
LIBDIR          = . 
INCLDIR         = . 
C++             = g++
CC              = gcc
CFLAGS          = -g -Wall -I$(INCLDIR)
LD_FLAGS        = -L$(LIBDIR) $(LIBS)

UTIL_LIB        = libutil.a
UTIL            = util
UTIL_OBJS       = cabstractfstream.o cabstractsocket.o cabstractsocket.o cconstant.o cexception.o cfile.o cifstream.o cofstream.o ctcpsocket.o cudpsocket.o cutil.o
UTIL_LD_FLAGS   = -l$(UTIL)

ASERVER         = aserver
ASERVER_OBJS    = caserver.o caservermain.o

USERVER         = userver
USERVER_OBJS    = cuserver.o cuservermain.o

CLIENT          = client
CLIENT_OBJS     = cclient.o cclientmain.o

all:            $(UTIL) $(ASERVER) $(USERVER) $(CLIENT)


$(UTIL):        $(UTIL_OBJS)
		ar rc $(UTIL_LIB) $(UTIL_OBJS)

$(ASERVER):     $(ASERVER_OBJS) 
		$(C++) -o $(ASERVER) $(CFLAGS) $(ASERVER_OBJS) $(LD_FLAGS) $(UTIL_LD_FLAGS) -ldl 
 
$(USERVER):     $(USERVER_OBJS) 
		$(C++) -o $(USERVER) $(CFLAGS) $(USERVER_OBJS) $(LD_FLAGS) $(UTIL_LD_FLAGS) -ldl 

$(CLIENT):      $(CLIENT_OBJS) 
		$(C++) -o $(CLIENT) $(CFLAGS) $(CLIENT_OBJS) $(LD_FLAGS) $(UTIL_LD_FLAGS) -ldl 

.cc.o:
	$(C++) -c $(CFLAGS) $<

.c.o:
	$(CC) -c $(CFLAGS) $<


clean:
	rm -f *.o $(UTIL_LIB) $(ASERVER) $(USERVER) $(CLIENT)

