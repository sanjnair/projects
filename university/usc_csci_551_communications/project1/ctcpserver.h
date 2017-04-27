/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CTCPPSERVER_H
#define CTCPPSERVER_H

#include "ctcpsocket.h"
#include "cmutex.h"

#define BACKLOG 10

class ITcpListener;

class CTcpServer {

public:
    explicit CTcpServer();
    virtual ~CTcpServer();

    void start(
      const string &ip, int port, ITcpListener *listener, bool verbose = true);
    void stop();

private:
    CTcpServer(const CTcpServer &other);
    CTcpServer &operator=(const CTcpServer &other);

    void openServer();
    void startListen();
    void acceptFrom(ITcpListener *listener);

    CTcpSocket _sock;
    ITcpListener *_listener;
    bool _stopAccept;
    CMutex _serverLock;
};

class ITcpListener {
public:
    virtual void notifyNewConn(int newSocket, struct sockaddr_in &addr) = 0;
};

#endif //CTCPPSERVER_H

