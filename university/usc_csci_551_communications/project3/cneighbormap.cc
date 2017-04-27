/***************************************************************************
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cneighbormap.h"
#include "cmutexlocker.h"
#include "cexception.h"
#include "cutil.h"

/*!
  Constructs a neighbor map
*/
CNeighborMap::CNeighborMap()
{
  _deleteId = 0;;
}

/*!
  Deletes the neighbor map
*/
CNeighborMap::~CNeighborMap()
{
  //cleanup();
}

/*!
  Returns the size of the map
*/
size_t CNeighborMap::size()
{
  CMutexLocker m(&_mutex);
  return _nMap.size();
}

/*!
  Adds or replaces an entry from the map.
  If the nodeid already exists, then this method will
  replace the entry if the node is no longer running.
  It'll rename the entry as deleted_<id>.

  Returns true, if the node can be added / replaced. Otherwise
  returns false.
*/
bool CNeighborMap::addOrReplace(
  const string &nodeid, CNode *n)
{
  assert(n);

  CMutexLocker locker(&_mutex);
  bool status = canAddOrReplace_p(nodeid);

  if (status) {
    if (contains_p(nodeid)) {
      markDelete_p(nodeid);
    }
    add_p(nodeid, n);
  }

  return status;
}

/*!
  Returns true if the node can be replaced.
*/
bool CNeighborMap::canAddOrReplace(const string &nodeid)
{
  CMutexLocker m(&_mutex);
  return canAddOrReplace_p(nodeid);
}

/*!
  Replaces the neighbor node if it can be replaced.
  If successfully replaced with newNode id, this method
  will return true.
*/
bool CNeighborMap::replace(
  const string &curNodeId,
  const string &newNodeId)
{
  CMutexLocker m(&_mutex);
  bool status = canAddOrReplace_p(newNodeId);
  
  if (status) {
    if (contains_p(newNodeId)) {
      markDelete_p(newNodeId);
    }
    CNode *n = take_p(curNodeId);
    add_p(newNodeId, n);
  }

  return status;
}

/*!
  Mark the node as deleted.
  And remove the node id from the map
*/
void CNeighborMap::markDelete_p(
  const string &nodeid)
{
  CNode *node = take_p(nodeid);
  string s = getDeletedId();
  add_p(s, node);
}

/*!
  Returns true if the node can be replaced.
*/
bool CNeighborMap::canAddOrReplace_p(
  const string &nodeid)
{
  bool status = true;
  if (contains_p(nodeid)) {
    CNode *node = _nMap[nodeid];

    if (node->isRunning()) {
      status = false;
    }
  }
  return status;
}
 
/*!
  \internal
*/
void CNeighborMap::add_p(const string &nodeid, CNode *n)
{
  errorIfItemPresent(nodeid);
  _nMap[nodeid] = n;
}

/*!
  Deletes a neighbor from the neighbor list.
  If not int the list, it doesn't do anything.
*/
void CNeighborMap::remove(const string &nodeid)
{
  CMutexLocker m(&_mutex);
  remove_p(nodeid);
}

/*!
  Removes the item from the map.
*/
void CNeighborMap::remove_p(const string &nodeid)
{
  if (contains_p(nodeid)) {
    delete_p(nodeid);
    _nMap.erase(_nMap.find(nodeid));
  }
}

/*!
  Deltes the item from the map. It does not remove
  the item from the map
*/
void CNeighborMap::delete_p(const string &nodeid)
{
  CNode *n = _nMap[nodeid];
  assert(n);
  if (n->isRunning()) {
    n->stop();
    n->join();
  }
  delete n;
}

/*!
  Returns true if the list contains the neighbor
*/
bool CNeighborMap::contains(const string &nodeid)
{
  CMutexLocker m(&_mutex);
  return contains_p(nodeid);
}

/*!
  Returns neighbor if it is in the list.
  Otherwise throws exception
*/
CNode* CNeighborMap::get(const string &nodeid)
{
  CMutexLocker m(&_mutex);

  errorIfItemNotPresent(nodeid);
  return _nMap[nodeid];
}

/*!
  Clears the map
*/
void CNeighborMap::clear()
{
  CMutexLocker m(&_mutex);
  cleanup();
}

/*!
  Returns the pointer stored without deleting the pointer.
  Caller need to make sure the pointer is deleted.
*/
CNode* CNeighborMap::take(const string &nodeid)
{
  CMutexLocker m(&_mutex);
  return take_p(nodeid);
}

/*!
  \Internal
*/
CNode* CNeighborMap::take_p(const string &nodeid)
{
  errorIfItemNotPresent(nodeid);
  CNode *node = _nMap[nodeid];
  _nMap.erase(_nMap.find(nodeid));

  return node;
}

/*!
  Sends the timer ticks to nodes
*/
void CNeighborMap::timerTick(uint curTime)
{
  if (_nMap.size() > 0) {
    NeighborMap::const_iterator it;

    for (it = _nMap.begin(); it != _nMap.end(); it++) {
      string nodeid = it->first;
      CNode *n = it->second;

      {
        CMutexLocker m(&_mutex);
        if (n && n->isRunning()) {
          n->timerTick(curTime);
        }
      }
    }
  }
}

/*!
  Returns the node id of list of neighbors
*/
StringList CNeighborMap::getNeighborList()
{
  StringList list;

  CMutexLocker m(&_mutex);
  if (_nMap.size() > 0) {
    NeighborMap::const_iterator it;

    for (it = _nMap.begin(); it != _nMap.end(); it++) {
      string nodeid = it->first;
      CNode *n = it->second;
      assert(n);

      if (n->isNeighbor() && n->isRunning()) {
        list.push_back(nodeid);
      }
    }
  }

  return list;
}

/*!
  Cleans up the map
*/
void CNeighborMap::cleanup()
{
  NeighborMap::const_iterator it;
  for (it = _nMap.begin(); it != _nMap.end(); it++) {
    CNode *n = it->second;

    if(n) {
      if (n->isRunning()) {
        //printf("stopping node %s\n", n->getNodeId().c_str());
        n->stop();
        //printf("stopped node. Waiting for join %s\n", n->getNodeId().c_str());
        n->join();
        //printf("Node joined %s\n", n->getNodeId().c_str());
      }
      delete n;
    }
  }
  _nMap.clear();

}

/*!
  \internal
  Will not lock the object
*/
bool CNeighborMap::contains_p(const string &nodeid) const
{
  return (_nMap.find(nodeid) != _nMap.end());
}

/*!
  Returns the deleted id
*/
string CNeighborMap::getDeletedId()
{
  string s = DELETE_PREFIX + CUtil::getString(_deleteId);
  _deleteId++;

  return s;
}

/*!
  Error if Item Present
*/
void CNeighborMap::errorIfItemPresent(const string &nodeid) const
{
  if (contains_p(nodeid)) {
    string msg = string("Item ") + nodeid + string(" is already in the list");
    C_THROW(msg);
  }
}

/*!
  Error if Item Not Present
*/
void CNeighborMap::errorIfItemNotPresent(const string &nodeid) const
{
  if (! contains_p(nodeid)) {
    string msg = string("Item ") + nodeid + string(" is not in the list");
    C_THROW(msg);
  }
}

//-----------------------------------------------------------------------------
const string CNeighborMap::DELETE_PREFIX  = "deleted_";
