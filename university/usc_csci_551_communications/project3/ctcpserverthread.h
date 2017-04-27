/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CTCPSERVERTHREAD_H
#define CTCPSERVERTHREAD_H

#include <signal.h>
#include "cthread.h"
#include "cmutex.h"
#include "ciservant.h"
#include "ctcpserver.h"

class CTcpServerThread : public CThread
{
public:
  CTcpServerThread();
  ~CTcpServerThread();

  void setServantHandle(IServant *s);
  void setTcpListener(ITcpListener *l);
  
  void run();
  void stop();

private:
  void initSigHandler();
  void errorIfInvalidSigCode(int code, const string &method) const;

  IServant *_servant;
  ITcpListener *_listener;
  CTcpServer _tcpServer;
  struct sigaction _sigAct;
  sigset_t _sigmask;
  CMutex _mutex;
};

#endif //CTCPSERVERTHREAD_H

