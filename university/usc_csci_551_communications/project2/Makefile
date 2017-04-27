################################################################
# MAKE FILE for CSCI551 Project
# @author - sanjay nair - sanjaysn@usc.edu
################################################################

LIBS            = -lpthread
LIBDIR          = . 
INCLDIR         = . 

C++             = g++
CC              = gcc
CFLAGS          = -g -Wall -I$(INCLDIR)
LD_FLAGS        = -L$(LIBDIR) $(LIBS)

UTIL_LIB        = libutil.a
UTIL            = util
UTIL_OBJS       = cmutex.o cconst.o cutil.o cexception.o cwaitcondition.o cmutexlocker.o cthread.o ccustomer.o 
UTIL_LD_FLAGS   = -l$(UTIL)

MM2		= mm2
MM2_OBJS	= cmm2.o carrival.o cmserver.o ccustomer.o mm2main.o 

all:            $(MM2)

$(UTIL):        $(UTIL_OBJS)
		ar rc $(UTIL_LIB) $(UTIL_OBJS)

$(MM2):     	$(UTIL) $(MM2_OBJS) 
		$(C++) -o $(MM2) $(CFLAGS) $(MM2_OBJS) $(LD_FLAGS) $(UTIL_LD_FLAGS) -ldl 

.cc.o:
	$(C++) -c $(CFLAGS) $<

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o $(UTIL_LIB) $(MM2)

