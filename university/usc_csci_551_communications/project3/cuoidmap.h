/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CUOIDMAP_H
#define CUOIDMAP_H

#include "cglobal.h"
#include "cmutex.h"

class CUoidData;
class CUoidMap
{
public:
  CUoidMap();
  ~CUoidMap();

  bool add(const UCharBuffer &b, const string &nodeid, uint curTime);
  bool add(const string &uoid, const string &nodeid, uint curTime);

  bool contains(const UCharBuffer &b);
  bool contains(const string &uoid);
  string getNodeId(const UCharBuffer &b);
  string getNodeId(const string &uoid);
  void cleanupExpired(uint curTime, uint duration);
  void clear();

private:
  void add_p(const string &uoid, const string &nodeid, uint curTime);
  bool contains_p(const string &uoid) const;
  void cleanupExpired_p(const StringVector &p);
  
  void errorIfItemPresent(const string &uoid) const;
  void errorIfItemNotPresent(const string &uoid) const;
  void errorIfInvalidBuffer(const UCharBuffer &b) const;

  typedef std::map<uint, StringVector> TimeUoidMap;
  
  StringMap _uMap;
  TimeUoidMap _tMap;
  CMutex _mutex;
};

#endif //CUOIDMAP_H
