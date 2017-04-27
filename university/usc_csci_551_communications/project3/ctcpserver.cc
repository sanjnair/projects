/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "ctcpserver.h"
#include "cutil.h"
#include "cexception.h"
#include "cmutexlocker.h"

/*!
    Constructs the TCP Server
*/
CTcpServer::CTcpServer()
{
  _stopFlag = false;
  _boundPort = 0;
}

/*!
    Deletes the TCP socket
*/
CTcpServer::~CTcpServer() {}

/*!
  Starts the TCP server and binds to all interfaces.
  SigMask is for the listeners that want to spawn
  a new thread for new connections.
  The listeners MUST set back the mask as it is given
  to them.
*/
void CTcpServer::start(
  int port, ITcpListener *listener, sigset_t *sigMask)
{
  _sock.openSock();
  int reuseAddr = 1;
  setsockopt(
    _sock.getSocket(), SOL_SOCKET, SO_REUSEADDR,
    (void*)(&reuseAddr), sizeof(int));
  
  bindTo(port);
  startListen();
  acceptFrom(listener, sigMask);
}

/*!
  Stops the TCP server
*/
void CTcpServer::stop()
{
  CMutexLocker locker(&_serverLock);
  _stopFlag = true;
}

/*!
  Bind to all interfaces
*/
void CTcpServer::bindTo(uint16_t port)
{
  struct sockaddr_in sAddr;
  CTcpSocket::initSockAddr(sAddr, INADDR_ANY, port);
  
  int status = bind(
                _sock.getSocket(),
                (struct sockaddr *) &sAddr,
                sizeof(sAddr));
                
  string msg = string("Unable to bind to port ") + CUtil::getString(port);
  CTcpSocket::errorIfInalidCode(msg, status);

  _boundPort = port;
}

/*!
  Returns the bounded port
*/
uint16_t CTcpServer::getBoundPort() const
{
  return _boundPort;
}

/*!
  Returns true if the stop flag is enabled
*/
bool CTcpServer::isStopFlagEnabled()
{
  CMutexLocker locker(&_serverLock);
  return _stopFlag;
}

/*!
    Server starts listening
*/
void CTcpServer::startListen()
{
  int status = listen(_sock.getSocket(), BACKLOG);
  CTcpSocket::errorIfInalidCode("unable to listen on a TCP port", status);
}

/*!
    Server accepts connection from clients
*/
void CTcpServer::acceptFrom(
  ITcpListener *listener, sigset_t *sigMask)
{
  assert(listener);

  in_addr_t socket = _sock.getSocket();
  struct sockaddr_in dAddr;
  memset(&dAddr, 0, sizeof(dAddr));
  in_addr_t dSocket = 0;

  socklen_t addrSize = sizeof(sockaddr_in);

  while (! isStopFlagEnabled()) {
    dSocket = accept(socket, (struct sockaddr *)&dAddr, &addrSize);

    if (! isStopFlagEnabled()) {
      CTcpSocket::errorIfInalidCode("Accept returned invalid socket", dSocket);
      listener->notifyNewConn(dSocket, dAddr, sigMask);
    }
  }
}
