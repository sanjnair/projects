/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CABSTRACTSOCKET_H
#define CABSTRACTSOCKET_H

#define SOCK_BUF_SIZE 1024

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include "cglobal.h"
#include "cutil.h"

typedef std::vector<char> CharBuffer;

class CAbstractSocket {

public:
  virtual ~CAbstractSocket();

  virtual void openSock();
  virtual void closeSock();

  void bindTo(const string &ip, uint16_t port);
  void bindTo(uint16_t port);
  
  in_addr_t getSocket() const;
  int getType() const;
  uint16_t getBoundPort() const;
  string getBoundHost() const;

  static string getHostName();
  static string getIp(const string &hostName);
  static string getSystemIp();
  static string getIp(const struct sockaddr_in &a);
  static uint16_t getPort(const struct sockaddr_in &a);
  static in_addr_t ipStrToAddr(const string &s);
  static void errorIfInalidCode(const string &s, int c);

  static void addToBuffer(CharBuffer &buf, const string &s);
  static void addToBuffer(CharBuffer &buf, const char c);
  static void addToBuffer(CharBuffer &buf, uint16_t num, bool nw = true);
  static void addToBuffer(CharBuffer &buf, uint32_t num, bool nw = true);
  static void printBuffer(const CharBuffer &buf);

  template <typename T>
  static void getFromBuffer(const CharBuffer &buf, T &num, bool nw = true);
  
protected:
    CAbstractSocket(int type);
    void bindTo_p(sockaddr_in &sAddr, uint16_t port);
    
    void initSockAddr(sockaddr_in &sockAddr, in_addr_t addr, uint16_t port);
    void errorIfNotOpen() const;

    in_addr_t _socket;
    bool _isOpen;
    int _type;
    uint16_t _boundPort;
    struct sockaddr_in _sAddr;
    string _boundHost;

private:
    CAbstractSocket(const CAbstractSocket &other);
    CAbstractSocket &operator=(const CAbstractSocket &other);
};

template <typename T>
void CAbstractSocket::getFromBuffer(const CharBuffer &buf, T &num, bool nw)
{
  uint sizeT = sizeof(T);

  if (buf.size() < sizeT) {
    string msg("Buffer size is less than ");
    msg += CUtil::getString(sizeT);
    C_THROW(msg);
  }

  uint8_t arr[sizeof(T)];
  for (uint i=0; i <sizeT; i++) {
    arr[i] = uint8_t(buf[i]);
  }

  T t = 0;
  memcpy(&t, arr, sizeT);

  if (sizeT == sizeof(uint16_t)) {
    num = (nw) ? ntohs(t) : t;
  } else {
    num = (nw) ? ntohl(t) : t;
  }


  /*uint sizeT = sizeof(T);

  if (buf.size() < sizeT) {
    string msg("Buffer size is less than ");
    msg += CUtil::getString(sizeT);
    C_THROW(msg);
  }

  T t = 0;
  for (uint i=0; i<sizeT; i++) {
    CUtil::errorIfNotDigit(buf[i]);
    T temp = uint8_t(buf[i]) << (i * 8);
    t |= temp;
  }

  if (sizeT == sizeof(uint16_t)) {
    num = (nw) ? ntohs(t) : t;
  } else {
    num = (nw) ? ntohl(t) : t;
  }*/
}

#endif


