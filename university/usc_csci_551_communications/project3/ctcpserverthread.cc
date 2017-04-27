/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <unistd.h>
#include "ctcpserverthread.h"
#include "ciniparser.h"
#include "clog.h"
#include "cexception.h"
#include "cutil.h"

/*!
  Constructs a TCP Server Thread
*/
CTcpServerThread::CTcpServerThread()
{
   _servant = NULL;
   _listener = NULL;
}

/*!
  Deletes a TCP server thread
*/
CTcpServerThread::~CTcpServerThread() {}

/*!
  Sets the server handle
*/
void CTcpServerThread::setServantHandle(IServant *s)
{
  _servant = s;
}

/*!
  Sets the TCP listener
*/
void CTcpServerThread::setTcpListener(ITcpListener *l)
{
  _listener = l;
}

/*!
  Starts running the thread
*/
void CTcpServerThread::run()
{
  assert(_servant);
  assert(_listener);

  try {
    initSigHandler();
    _tcpServer.start(
      _servant->getIniParser()->getPort(),
      _listener, &_sigmask);

  } catch (CException &e) {
    _servant->getLog()->logError(e.toString());
    _servant->setLastError(e.getMessage());    
    _servant->stop();    
  }
}

/*!
  Initialize the signal handler
*/
void CTcpServerThread::initSigHandler()
{
  int ret = sigemptyset(&_sigmask);
  errorIfInvalidSigCode(ret, "CTcpServerThread::initSigHandler::sigemptyset");

  ret = sigaddset(&_sigmask, SIGUSR1);
  errorIfInvalidSigCode(ret, "CTcpServerThread::initSigHandler::sigaddset");
  _servant->setSigMask(&_sigmask, false);
}

/*!
  Throws exception if the signal error code is invalid
*/
void CTcpServerThread::errorIfInvalidSigCode(
  int code, const string &method) const
{
  if (code < 0) {
    string msg = string("Call to ") + method;
    msg += string(" returned error. code = ") + CUtil::getString(code);
    msg += string(", errno = ") + CUtil::getString(errno);
  }
}

/*!
  Stops the TcpServer
*/
void CTcpServerThread::stop()
{
  _tcpServer.stop();

  if (kill(getpid(), SIGUSR1) < 0) {
    C_THROW("Unable to issue the kill call on SIGUSR1");
  }
}
