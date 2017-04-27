/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CTCPPSOCKET_H
#define CTCPPSOCKET_H

#include <sys/socket.h>
#include "cglobal.h"
#include "cmutex.h"

#define SOCK_BUF_SIZE 1024

class CTcpSocket
{
public:
  explicit CTcpSocket();
  ~CTcpSocket();

  void stop();
  bool isStopFlagEnabled();
  
  void openSock();
  void closeSock();
  bool isOpened() const;
  in_addr_t getSocket() const;

  void connectTo(in_addr_t socket, const struct sockaddr_in &dAddr);
  void connectTo(const string &host, uint16_t port);
  bool isConnected() const;

  uint16_t getLocalPort() const;
  uint16_t getRemotePort() const;
  string getRemoteHost() const;

  string getPeerHostName() const;
  uint16_t getPeerPort() const;

  int sendTo(const UCharBuffer &buf);
  int receiveFrom(uchar *data, int maxSize);
  int receiveFrom(UCharBuffer &buf, int maxSize);

  static string getHostName();
  static string getIp(const string &hostName);
  static string getSystemIp();
  static string getHostName(const struct sockaddr_in &a);
  static string getIp(const struct sockaddr_in &a);
  static uint16_t getPort(const struct sockaddr_in &a);
  static in_addr_t ipStrToAddr(const string &s);
  static void initSockAddr(
    sockaddr_in &sockAddr, in_addr_t addr,uint16_t port);

  static void errorIfInalidCode(const string &s, int c);

private:
    CTcpSocket(const CTcpSocket &other);
    CTcpSocket &operator=(const CTcpSocket &other);

    void getPeerNameStruct(struct sockaddr_in *addr, socklen_t *t) const;
    sockaddr_in getSockName() const;

    void errorIfNotOpen() const;
    void errorIfNotConnected() const;

    bool _isOpen;
    bool _stopFlag;
    bool _isConnected;
    in_addr_t _socket;
    uint16_t _localPort;
    uint16_t _remotePort;
    string _remoteHost;
    sockaddr_in _sockAddr;
    CMutex _mutex;
};

#endif //CTCPPSOCKET_H

