/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CJOINDATA_H
#define CJOINDATA_H

#include "cglobal.h"

class CJoinData
{
public:
  CJoinData();
  CJoinData(const string &host, uint16_t port, uint32_t distance);
  ~CJoinData();

  string getHost() const;
  uint16_t getPort() const;
  uint32_t getDistance() const;

  bool operator < (const CJoinData &rhs);
  string toString() const;
  
private:
  string _host;
  uint16_t _port;
  uint32_t _distance;
};

typedef std::list<CJoinData> JoinDataList;


#endif


