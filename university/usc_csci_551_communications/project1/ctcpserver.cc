/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <signal.h>
#include "ctcpserver.h"
#include "cutil.h"
#include "cexception.h"
#include "cmutexlocker.h"

/*!
  Signal handler.
*/
void handleTcpServerSignal(int sig_num)
{
  return;
}

/*!
    Constructs the TCP Server
*/
CTcpServer::CTcpServer() : _listener(0), _stopAccept(false) {}

/*!
    Deletes the TCP socket
*/
CTcpServer::~CTcpServer() {}




/*!
  Starts the TCP server
*/
void CTcpServer::start(
  const string &ip, int port, ITcpListener *listener, bool verbose )
{
  if (verbose) {
    string msg = string("Starting server at IP ");
    if (ip.length() >0) {
      msg +=  ip;
    } else {
      msg += string("<All addresses>");
    }
    msg += string(" and port ") + CUtil::getString(port);
    CUtil::writeOutputLn(msg);
  }

  openServer();

  if (ip.length() >0) {
    _sock.bindTo(ip, port);
  } else {
    _sock.bindTo(port);
  }

  if (verbose) {
    CUtil::writeOutputLn("Successully completed bind operation");
  }
  
  startListen();
  acceptFrom(listener);
}
  
/*!
  Stops the TCP server
*/
void CTcpServer::stop()
{
  CMutexLocker locker(&_serverLock);
  _stopAccept = true;
  try {
    signal(SIGINT, handleTcpServerSignal);
    _sock.closeSock();
  } catch (CException &) {
    //ignore exceptions
  }
}

/*!
    Opens the TCP server
*/
void CTcpServer::openServer()
{
  _sock.openSock();

  int yes = 1;
  int status = setsockopt(_sock.getSocket(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  CAbstractSocket::errorIfInalidCode("unable set socket options", status);
}

/*!
    Server starts listening
*/
void CTcpServer::startListen()
{
  int status = listen(_sock.getSocket(), BACKLOG);
  CAbstractSocket::errorIfInalidCode("unable to listen on a TCP port", status);
}

/*!
    Server accepts connection from clients
*/
void CTcpServer::acceptFrom(ITcpListener *listener)
{
  in_addr_t socket = _sock.getSocket();

  _listener = listener;
  struct sockaddr_in dAddr;
  memset(&dAddr, 0, sizeof(dAddr));
  in_addr_t dSocket = 0;

  socklen_t addrSize = sizeof(sockaddr_in);
  bool stop = false;
   
  {
    CMutexLocker locker(&_serverLock);
    stop = _stopAccept;
  }

  if( ! stop) {
    do {
      CUtil::writeOutputLn("Waiting for clients to connect...");
      CUtil::writeOutputLn("-----------------------------");

      dSocket = accept(socket, (struct sockaddr *)&dAddr, &addrSize);
      {
        CMutexLocker locker(&_serverLock);
        stop = _stopAccept;
      }

      if( ! stop) {
        CAbstractSocket::errorIfInalidCode("Accept returned invalid socket", dSocket);
        _listener->notifyNewConn(dSocket, dAddr);
        {
          CMutexLocker locker(&_serverLock);
          stop = _stopAccept;
        }
      }
    } while(! stop);
  }
}
