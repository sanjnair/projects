/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CCLIENT_H
#define CCLIENT_H

#include "cglobal.h"
#include "ctcpsocket.h"

enum ReqType {
  ReqAddr,
  ReqFileSize,
  ReqGet
};

class CClient {

public:
  explicit CClient(ReqType type, const string &serverHost,
                   uint16_t port, const string &option);

  ~CClient();
  void connect();

private:
  CClient(const CClient &other);
  CClient &operator=(const CClient &other);

  void sendRequest(CTcpSocket &sock, uint16_t type);
  
  void handleAddrReq(CTcpSocket &sock);
  void handleFileSizeReq(CTcpSocket &sock);
  void handleGetReq(CTcpSocket &sock);
  void getHeaderData(CTcpSocket &sock, uint16_t &reply, uint32_t &size) const;
  void errorIfInvalidReply(uint16_t reply) const;
  void writeOutput(const string &s) const;
  void outputBanner() const;

  ReqType _type;
  string _serverHost;
  uint16_t _port;
  string _option;
};

#endif //CCLIENT_H

