/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CNEIGHBORMAP_H
#define CNEIGHBORMAP_H

#include "cglobal.h"
#include "cmutex.h"
#include "cnode.h"

typedef std::map<string, CNode*> NeighborMap;

class CNeighborMap
{
public:
    CNeighborMap();
    ~CNeighborMap();

  size_t size();

  bool addOrReplace(const string &nodeid, CNode *n);
  bool canAddOrReplace(const string &nodeid);
  bool replace(const string &curNodeId, const string &newNodeId);

  void remove(const string &nodeid);
  bool contains(const string &nodeid);
  CNode *get(const string &nodeid);
  CNode *take(const string &nodeid);
  void clear();

  void timerTick(uint curTime);
  StringList getNeighborList();
  
private:
  void cleanup();
  string getDeletedId();

  void add_p(const string &nodeid, CNode *n);
  bool contains_p(const string &nodeid) const;
  void remove_p(const string &nodeid);
  void delete_p(const string &nodeid);
  CNode *take_p(const string &nodeid);
  bool canAddOrReplace_p(const string &nodeid);
  void markDelete_p(const string &nodeid);

  void errorIfItemPresent(const string &nodeid) const;
  void errorIfItemNotPresent(const string &nodeid) const;

  uint _deleteId;
  NeighborMap _nMap;
  CMutex _mutex;

  static const string DELETE_PREFIX;
};

#endif //CNEIGHBORMAP_H
