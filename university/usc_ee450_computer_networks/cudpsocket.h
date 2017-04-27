/**
 * Utilities
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CUDPSOCKET_H__
#define __CUDPSOCKET_H__

#include "cabstractsocket.h"

class CUdpSocket : public CAbstractSocket {

public:
    explicit CUdpSocket();
    virtual ~CUdpSocket();

    int readDatagram(char *data, int maxSize, struct sockaddr_in &dAddr, int timeout=0);
    string readDatagram(struct sockaddr_in &dAddr, int timeout=0);
    
    int writeDatagram(const char *data, int maxSize, struct sockaddr_in &dAddr);
    int writeDatagram(const string &data, struct sockaddr_in &dAddr);
    int writeDatagram(const char *data, int maxSize, ulong addr, int port);
    int writeDatagram(const string &data, ulong addr, int port);
    string getSockName();

private:
    CUdpSocket(const CUdpSocket &other);
    CUdpSocket &operator=(const CUdpSocket &other);

};

#endif //__CUDPSOCKET_H__

