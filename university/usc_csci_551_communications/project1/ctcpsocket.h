/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CTCPPSOCKET_H
#define CTCPPSOCKET_H

#include "cabstractsocket.h"

class CTcpSocket : public CAbstractSocket {

public:
    explicit CTcpSocket();
    CTcpSocket(in_addr_t socket);
    virtual ~CTcpSocket();

    virtual void openSock();
    void connectTo(const string &ip, uint16_t port);
    int sendTo(const char *data, int maxSize);
    int sendTo(const string &str);
    int sendTo(const CharBuffer &buf);
    int receiveFrom(char *data, int maxSize, int secDelay);
    string receiveFrom(int secDelay);
    CharBuffer receiveFrom(uint numChars, int secDelay);
    string receiveStrFrom(uint numChars, int secDelay);

private:
    CTcpSocket(const CTcpSocket &other);
    CTcpSocket &operator=(const CTcpSocket &other);
};

#endif //CTCPPSOCKET_H

