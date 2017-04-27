/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CNODEDATA_H
#define CNODEDATA_H

#include "cglobal.h"

class CNodeData
{
public:
  CNodeData(const string &host, uint16_t port);
  ~CNodeData();

  string getHost() const;
  uint16_t getPort() const;

private:
  string _host;
  uint16_t _port;
};

typedef std::list<CNodeData> NodeDataList;


#endif


