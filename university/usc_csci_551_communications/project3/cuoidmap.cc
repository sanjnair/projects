/***************************************************************************
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cglobal.h"
#include <openssl/sha.h>
#include "cuoidmap.h"
#include "cmutexlocker.h"
#include "cexception.h"
#include "cutil.h"

/*!
  Constructs a uoid map
*/
CUoidMap::CUoidMap() {}

/*!
  Deletes the uoid map
*/
CUoidMap::~CUoidMap() {}

/*!
  Adds a uoid to the uoid list.
  Returns true if the item is added. Else false.
  If in the list, it throws exception
*/
bool CUoidMap::add(
  const UCharBuffer &b, const string &nodeid, uint curTime)
{
  errorIfInvalidBuffer(b);
  return add(CUtil::getHexStr(b), nodeid, curTime);
}

/*!
  Adds a uoid to the uoid list.
  Returns true if the item is added. Else false.
  If in the list, it throws exception
*/
bool CUoidMap::add(
  const string &uoid, const string &nodeid, uint curTime)
{
  bool bAdded = false;
  CMutexLocker m(&_mutex);
  if (! contains_p(uoid)) {
    add_p(uoid, nodeid, curTime);
    bAdded = true;
  }
  return bAdded;
}

/*!
  \internal
*/
void CUoidMap::add_p(
  const string &uoid, const string &nodeid, uint curTime)
{
  errorIfItemPresent(uoid);
  _uMap[uoid] = nodeid;

  if (_tMap.find(curTime) == _tMap.end()) {
    StringVector v;
    _tMap[curTime] = v;
  }
  StringVector *sv = &(_tMap[curTime]);
  sv->push_back(uoid);
}

/*!
  Returns true if the list contains the uoid
*/
bool CUoidMap::contains(const UCharBuffer &b)
{
  errorIfInvalidBuffer(b);
  return contains(CUtil::getHexStr(b));
}

/*!
  Returns true if the list contains the uoid
*/
bool CUoidMap::contains(const string &uoid)
{
  CMutexLocker m(&_mutex);
  return contains_p(uoid);
}

/*!
  \internal
  Will not lock the object
*/
bool CUoidMap::contains_p(const string &uoid) const
{
  return (_uMap.find(uoid) != _uMap.end());
}

/*!
  Returns nodeid if it is in the list.
  Otherwise throws exception
*/
string CUoidMap::getNodeId(const UCharBuffer &b)
{
  errorIfInvalidBuffer(b);
  return getNodeId(CUtil::getHexStr(b));
}

/*!
  Returns nodeid if it is in the list.
  Otherwise throws exception.
*/
string CUoidMap::getNodeId(const string &uoid)
{
  CMutexLocker m(&_mutex);
  errorIfItemNotPresent(uoid);
  return _uMap[uoid];
}

/*!
  Cleans up the expired items that're
  in the uoid map.
*/
void CUoidMap::cleanupExpired(uint curTime, uint duration)
{
  CMutexLocker m(&_mutex);

  if (_tMap.size() > 0) {
    TimeUoidMap::iterator it;

    for (it=_tMap.begin(); it != _tMap.end(); it++) {
      uint pastTime = it->first;
      if (CUtil::isElapsed(pastTime, curTime, duration)) {
        cleanupExpired_p(it->second);
        _tMap.erase(it);
      } else {
        break;
      }
    }
  }
}

/*!
  Cleans up the items in the time map
*/
void CUoidMap::cleanupExpired_p(const StringVector &p)
{
  if (p.size() > 0) {
    for (uint i=0; i<p.size(); i++) {
      string val = p.at(i);

      if (contains_p(val)) {
        _uMap.erase(val);
        //printf("removed %s from umap \n", val.c_str());
      }
    }
  }
}

/*!
  Clears the map
*/
void CUoidMap::clear()
{
  CMutexLocker m(&_mutex);
  _uMap.clear();
  _tMap.clear();
}

/*!
  Error if Item Present
*/
void CUoidMap::errorIfItemPresent(const string &uoid) const
{
  if (contains_p(uoid)) {
    string msg = string("Item ") + uoid + string(" is already in the list");
    C_THROW(msg);
  }
}

/*!
  Error if Item Not Present
*/
void CUoidMap::errorIfItemNotPresent(const string &uoid) const
{
  if (! contains_p(uoid)) {
    string msg = string("Item ") + uoid + string(" is not in the list");
    C_THROW(msg);
  }
}

/*!
  Throws exception if the Buffer is invalid
*/
void CUoidMap::errorIfInvalidBuffer(const UCharBuffer &b) const
{
  if (b.size() != SHA_DIGEST_LENGTH) {
    string msg = string("Invalid uoid size ") + CUtil::getString(b.size());
    C_THROW(msg);
  }
}
