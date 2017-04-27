/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CSERVER_H
#define CSERVER_H

#include <vector>
#include "cglobal.h"
#include "ctcpserver.h"
#include "cthread.h"
#include "cmutex.h"
#include "cwaitcondition.h"

class CServerConn;

class CServer : public ITcpListener
{
public:
  static CServer *getInstance();
  static void releaseInstance();
  virtual ~CServer();

  void start(int timeout, int delay, uint16_t port);
  void stop();
  virtual void notifyNewConn(int newSocket, struct sockaddr_in &addr);

private:
  CServer();
  CServer(const CServer &other);
  CServer &operator=(const CServer &other);
  void cleanup();
  void outputBanner(string host);

  int _tout;
  int _delay;
  uint16_t _port;
  bool _stopFlag;
  
  CTcpServer _tcpServer;
  CMutex _mutex;
  std::vector<CServerConn *> _connList;

  static CServer *_instance;
};

class CServerConn : public CThread
{
public:
  CServerConn(int socket, int delay);
  virtual ~CServerConn();

  virtual void run();
  void stop();

private:
  void getHeaderData(CTcpSocket &sock, uint16_t &req, uint32_t &size) const;
  void handleAddrReq(CTcpSocket &sock, uint32_t size);
  void handleFileSizeReq(CTcpSocket &sock, uint32_t size);
  void handleGetReq(CTcpSocket &sock, uint32_t size);
  void errorIfInvalidRequest(uint16_t req) const;
  void convertToBuffer(const char *array, int arrayLen, CharBuffer &buf) const;
  void sendErrorReply(CTcpSocket &sock);
  bool isStopflagSet();
  void waitForDelay();
  
  int _socket;
  int _delay;
  CMutex _mutex;
  CWaitCondition _waitCond;
  bool _stopFlag;
};

#endif //CSERVER_H

