/**
 * Utilities
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CABSTRACTSOCKET_H__
#define __CABSTRACTSOCKET_H__

#define SOCK_BUF_SIZE 1024

#ifdef WIN32
    #include <winsock.h>
    #include <errno.h>
    #include <sys/types.h>
#else
    #include <unistd.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

#include "cglobal.h"

class CAbstractSocket {

public:
    virtual ~CAbstractSocket();
    virtual void openSock();
    virtual void closeSock();
    
    void bindTo(ulong addr, int port);
    int getSocket() const;
    int getType() const;
    int getBoundPort() const;
    string getBoundHost() const;
    string getSystemIp();
    ulong ipStrToUlong(const string &s);
    
    string getIpAddress(const struct sockaddr_in &a);
    int getPort(const struct sockaddr_in &a);

    void errorIfInalidCode(const string &s, int c) const;
    void errorIfNotOpen() const;

protected:
    CAbstractSocket(int type);
    void initSockAddr(struct sockaddr_in &sockAddr, ulong addr, int port);

    int _socket;
    int _type;
    struct sockaddr_in _sAddr;
    int _boundPort;
    string _boundHost;

private:
    CAbstractSocket(const CAbstractSocket &other);
    CAbstractSocket &operator=(const CAbstractSocket &other);
    
};

#endif //__CABSTRACTSOCKET_H__

