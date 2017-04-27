/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CTCPPSERVER_H
#define CTCPPSERVER_H

#include <signal.h>
#include "ctcpsocket.h"
#include "cmutex.h"

#define BACKLOG 10

class ITcpListener;

class CTcpServer {

public:
  explicit CTcpServer();
  virtual ~CTcpServer();

  void start(int port, ITcpListener *listener, sigset_t *sigMask);
  void stop();

  void bindTo(uint16_t port);
  uint16_t getBoundPort() const;

private:
    CTcpServer(const CTcpServer &other);
    CTcpServer &operator=(const CTcpServer &other);

    void startListen();
    void acceptFrom(ITcpListener *listener, sigset_t *sigMask);
    bool isStopFlagEnabled();

    bool _stopFlag;
    uint16_t _boundPort;
    
    CTcpSocket _sock;
    CMutex _serverLock;
};

class ITcpListener {
public:
    virtual void notifyNewConn(
      int newSocket, struct sockaddr_in &addr, sigset_t *sigMask) = 0;
};

#endif //CTCPPSERVER_H

