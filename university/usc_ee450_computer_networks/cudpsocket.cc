#include "cudpsocket.h"
#include "cutil.h"
#include "cexception.h"

/*!
    Constructs UDP socket
*/
CUdpSocket::CUdpSocket() : CAbstractSocket(SOCK_DGRAM) {}

/*!
    Deletes UDP socket
*/
CUdpSocket::~CUdpSocket() {}

/*!
    Reads the datagram and returns the data and address
 */
int CUdpSocket::readDatagram(char *data, int maxSize, struct sockaddr_in &dAddr, int timeout)
{
    int status = 0;
    errorIfNotOpen();

    bool isTimeout = false;

    /*if (timeout > 0) {
        //setting a 10 sec timeout
        struct fd_set dsmask;
        FD_ZERO(&dsmask); 
        FD_SET(_socket, &dsmask); 
        struct timeval tout;
        tout.tv_sec = timeout;
        tout.tv_sec = 0;

        int n = select(FD_SETSIZE, &dsmask, (fd_set *)NULL,  (fd_set *)NULL, &tout); 
        isTimeout = (n >= 0 && FD_ISSET(_socket, &dsmask)) ? false : true;
    }*/
    
    #if defined WIN32
      int addrLen = sizeof(dAddr);
    #else
      socklen_t addrLen = sizeof(dAddr);
    #endif
      
    if( ! isTimeout) {
        status = recvfrom(_socket, data, maxSize, 0, (struct sockaddr *) &dAddr, &addrLen);
        errorIfInalidCode("unable to read datagram", status);
    } else {
        status = -1;
        string msg = "Udp connection timed out. (time out set was ";
        msg += CUtil::getString(timeout);
        msg += " seconds).";

        errorIfInalidCode(msg, status);
    }

    return status;
}

/*!
    Just like above function. Provided for convenience.
*/
string CUdpSocket::readDatagram(struct sockaddr_in &dAddr, int timeout)
{
    char buf[SOCK_BUF_SIZE];
    int size = readDatagram(buf, SOCK_BUF_SIZE-1, dAddr, timeout);
    string s(buf, size);
    return s;
}

/*
    writes the datagrarm
*/
int CUdpSocket::writeDatagram(const char *data, int maxSize, struct sockaddr_in &dAddr)
{
    int addrLen = sizeof(dAddr);
    int status = sendto(_socket, data, maxSize, 0, (struct sockaddr *) &dAddr, addrLen);
    errorIfInalidCode("unable to read datagram", status);

    return status;
}

/*
    writes the datagrarm
*/
int CUdpSocket::writeDatagram(const string &data, struct sockaddr_in &dAddr)
{
    return writeDatagram(data.c_str(), data.length(), dAddr);
}

/*!
    Writes the datagram
*/
int CUdpSocket::writeDatagram(const char *data, int maxSize, ulong addr, int port)
{
    errorIfNotOpen();

    struct sockaddr_in sAddr;
    initSockAddr(sAddr, addr, port);

    return writeDatagram(data, maxSize, sAddr);
}


/*!
    Writes the datagram
*/
int CUdpSocket::writeDatagram(const string &data,ulong addr, int port)
{
    return writeDatagram(data.c_str(), data.length(), addr, port);
}


/*!
    Gets the socket information
*/
string CUdpSocket::getSockName()
{
    errorIfNotOpen();
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    
    #if defined WIN32 
      int addrLen = sizeof(addr);
    #else
      socklen_t addrLen = sizeof(addr);
    #endif

    int status = getsockname(_socket, (struct sockaddr *) &addr, &addrLen);
    errorIfInalidCode("Unable to get socket information", status);

    string s = "Address ";
    s += inet_ntoa(addr.sin_addr);
    s += " and port ";
    s += CUtil::getString(addr.sin_port);
    
    return s;
}


