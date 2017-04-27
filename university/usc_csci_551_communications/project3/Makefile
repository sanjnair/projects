################################################################
# MAKE FILE for CSCI551 Project
# @author - sanjay nair - sanjaysn@usc.edu
################################################################
                                                                                
LIBS            = -lpthread -lsocket -lnsl -lcrypto
LIBDIR          = .
INCLDIR         = .
SSL_DIR         = /home/scf-22/csci551b/openssl
SSL_LIBDIR      = $(SSL_DIR)/lib
SSL_INCLDIR     = $(SSL_DIR)/include
  
C++             = g++
CC              = gcc
CFLAGS          = -g -Wall -I$(INCLDIR) -I$(SSL_INCLDIR)
LD_FLAGS        = -L$(LIBDIR) -L/home/scf-22/csci551b/openssl/lib $(LIBS)
                                                                                
SERVANT         = sv_node
SERVANT_OBJS	= cmutex.o cconst.o cutil.o cexception.o cwaitcondition.o cmutexlocker.o cthread.o cabstractfstream.o cfile.o cifstream.o cofstream.o cdir.o ctcpsocket.o ctcpserver.o csha1data.o cmd5data.o cnodedata.o cjoindata.o ciniparser.o clog.o cmessage.o cmessagefactory.o ctcpserverthread.o cnode.o cneighbormap.o cuoidmap.o cevent.o ceventdispatcher.o ckbhandler.o cstatuswriter.o ccert.o cbitvector.o cfileindexmap.o cmetadata.o cfilespec.o cfilemgr.o cservant.o servantmain.o

all:            $(SERVANT)
                                                                                
$(SERVANT):    	$(SERVANT_OBJS) 
		$(C++) -o $(SERVANT) $(CFLAGS) $(SERVANT_OBJS) $(LD_FLAGS) -ldl
                                                                                
.cc.o:
	$(C++) -c $(CFLAGS) $<
                                                                                
.c.o:
	$(CC) -c $(CFLAGS) $<
                                                                                
clean:
	rm -f *.o $(SERVANT)

