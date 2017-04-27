/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CEVENTDISPATCHER_H
#define CEVENTDISPATCHER_H

#include "cglobal.h"
#include "cthread.h"
#include "cmutex.h"
#include "cwaitcondition.h"
#include "cevent.h"
#include "ciservant.h"

class CNeighborMap;

class CEventDispatcher : public CThread
{
public:
  CEventDispatcher();
  ~CEventDispatcher();

  void setServantHandle(IServant *s);
  void setNeighborMap(CNeighborMap *m);
  
  void addEvent(CEvent *event);
  void stop();
  void run();

private:
  bool isStopFlagEnabled();
  void cleanup();
  CEvent *getEvent_p();

  void handleEvent(CEvent *e);
  void handleMsgEvent(CMsgEvent *e);
  void handleStatusMsgEvent(CStatusMsgEvent *e);
  
  void floodMsg(CMsgEvent *e);
  void routeMsg(CMsgEvent *e);
      
  typedef std::list<CEvent*> EventQueue;

  bool _stopFlag;
  IServant *_servantP;
  CNeighborMap *_nMapP;
  
  EventQueue _eventQueue;
  CMutex _mutex;
  CWaitCondition _waitCond;
};

#endif //CEVENTDISPATCHER_H

