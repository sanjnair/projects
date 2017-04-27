/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "ceventdispatcher.h"
#include "cmutexlocker.h"
#include "cautoptr.h"
#include "cneighbormap.h"
#include "cconst.h"
#include "cuoidmap.h"
#include "ciniparser.h"

/*!
  Constructs the Event Dispatcher
*/
CEventDispatcher::CEventDispatcher()
{
  _stopFlag = false;
  _servantP = NULL;
  _nMapP = NULL;
}

/*!
  Deletes the event dispatcher
*/
CEventDispatcher::~CEventDispatcher()
{
  cleanup();
}

/*!
  Sets the server handle
*/
void CEventDispatcher::setServantHandle(IServant *s)
{
  _servantP = s;
}

/*!
  Sets the Neighbor map handle
*/
void CEventDispatcher::setNeighborMap(CNeighborMap *m)
{
  _nMapP = m;
}

/*!
  Adds an event to the queue
*/
void CEventDispatcher::addEvent(CEvent *event)
{
  CMutexLocker m(&_mutex);
  _eventQueue.push_back(event);
  _waitCond.notifyAll();
}


/*!
  Stops the Dispatcher thread
*/
void CEventDispatcher::stop()
{
  CMutexLocker m(&_mutex);
  _stopFlag = true;
  _waitCond.notifyAll();
}

/*!
  Returns true if the stop flag is enabled
*/
bool CEventDispatcher::isStopFlagEnabled()
{
  CMutexLocker m(&_mutex);
  return _stopFlag;
}

/*!
  Cleanup the queue and deletes all Events
*/
void CEventDispatcher::cleanup()
{
  if (_eventQueue.size() > 0) {
    EventQueue::const_iterator it;
    for (it = _eventQueue.begin(); it != _eventQueue.end(); it++) {
      delete *it;
    }
    _eventQueue.clear();
  }
}

/*!
  Takes an event out from the queue
*/
CEvent* CEventDispatcher::getEvent_p()
{
  CMutexLocker m(&_mutex);
  while ((! _stopFlag) && (_eventQueue.empty())) {
     _waitCond.wait(&_mutex);
  }
  CEvent *e = NULL;

  if (! _stopFlag) {
    e = _eventQueue.front();
    _eventQueue.pop_front();
  }

  return e;
}

/*!
  Thread Run method
*/
void CEventDispatcher::run()
{
  assert(_servantP);
  assert(_nMapP);
  
  CEvent *e = NULL;
  while (! isStopFlagEnabled()) {
    e = getEvent_p();
    if (NULL != e) {

      try {
        handleEvent(e);
      } catch (CException &e) {
        _servantP->getLog()->logError(e.toString());
      }
    }
  }
}

/*!
  Handles the Event
*/
void CEventDispatcher::handleEvent(CEvent *e)
{
  CAutoPtr<CEvent> eventP(e);

  switch(e->getType()) {
    case EventMsg:
      handleMsgEvent(dynamic_cast<CMsgEvent *>(e));
      break;

    case EventStatusMsg:
      handleStatusMsgEvent(dynamic_cast<CStatusMsgEvent *>(e));    
      break;
            
    default:
    {
      string msg = string("Invalud event type: ");
      msg += CUtil::getString(e->getType());
      C_THROW(msg);
    }
  }
}

/*!
  Handles the message event
*/
void CEventDispatcher::handleMsgEvent(CMsgEvent *e)
{
  e->isFlood() ? floodMsg(e) : routeMsg(e);
}

/*!
  Handles the status message event
*/
void CEventDispatcher::handleStatusMsgEvent(CStatusMsgEvent *e)
{
  CMessageStatusRes *message = e->getMessage(); assert(message);  

  StringList nList = _nMapP->getNeighborList();

  if (nList.size() <= 0) {
    return;
  }

  CNode *nodeP = NULL;
  string nodeId;
  StringList::const_iterator it;

  NodeDataList nborList;
  
  for (it = nList.begin(); it != nList.end(); it++) {
    nodeId = *it;
    nodeP = _nMapP->get(nodeId);

    if (nodeP && nodeP->isRunning()) {
      CNodeData d(CNode::getHostName(nodeId), CNode::getPort(nodeId));
      nborList.push_back(d);
    }
  }

  message->setNeighborList(nborList);
  routeMsg(e);
}

/*!
  Floods the message all the neighbor nodes except
  the one that initiated this request.
*/
void CEventDispatcher::floodMsg(CMsgEvent *e)
{
  CMessage *m = e->getMessage();
  m->decrTtl();
  if (m->getTtl() <= 0) {
    return;
  }

  StringList nList = _nMapP->getNeighborList();
  if (nList.size() <= 0) {
    return;
  }

  CNode *nodeP = NULL;
  string nodeId;
  StringList::const_iterator it;

  for (it = nList.begin(); it != nList.end(); it++) {
    nodeId = *it;
    nodeP = _nMapP->get(nodeId);

    if (nodeP && nodeP->isRunning()) {

      if (e->getSrcNodeId() != nodeId) {
        bool send = true;
        
        if (e->isProbEnabled()) {
          double v = _servantP->getIniParser()->getNeighborStoreProb();
          if (! _servantP->isPositiveProb(v)) {
            send = false;
          }
        }
        
        if (send) {
          nodeP->sendMessage(m, e->getLogType());
        }
      }
    }
  }
}

/*!
  Routes this message to the appropriate requester
*/
void CEventDispatcher::routeMsg(CMsgEvent *e)
{
  CUoidMap *uMap = _servantP->getUoidMap();
  
  if (uMap->contains(e->getUoid())) {
    string nodeId = uMap->getNodeId(e->getUoid());

    if (_nMapP->contains(nodeId)) {
      CNode *nodeP = _nMapP->get(nodeId);
      if (nodeP && nodeP->isRunning()) {
        nodeP->sendMessage(e->getMessage(), e->getLogType());
      }
    }
  }
}
