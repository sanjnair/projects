/**
 * Utilities
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CTCPPSOCKET_H__
#define __CTCPPSOCKET_H__

#include "cabstractsocket.h"

#define BACKLOG 10

class ITcpListener;

class CTcpSocket : public CAbstractSocket {

public:
    explicit CTcpSocket();
    CTcpSocket(int socket);
    virtual ~CTcpSocket();
    
    void openServer();
    void startListen();
    void acceptFrom(ITcpListener *listener);
    void connectTo(const string &ip, int port);

    int sendTo(const char *data, int maxSize);
    int sendTo(const string &str);
    int receiveFrom(char *data, int maxSize);
    string receiveFrom(); 

    void stopAccept();

private:
    CTcpSocket(const CTcpSocket &other);
    CTcpSocket &operator=(const CTcpSocket &other);

    ITcpListener *_listener;
    bool _stopAccept;
};

class ITcpListener {
public:
    virtual void notifyConnect(int newSocket, struct sockaddr_in &addr) = 0;
};


#endif //__CTCPPSOCKET_H__

