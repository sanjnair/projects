/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/
#include "cmserver.h"
#include "cutil.h"
#include "ccustomer.h"


/*!
  Creates a new Server Connection
*/
CMserver::CMserver(uint id, CQueue *q)
  : CThread(), _id(id), _queue(q) {}

CMserver::~CMserver() {}

/*!
  Returns server id
*/
uint CMserver::getId() const
{
  return _id;
}

/*!
  Starts the connection activities
*/
void CMserver::run()
{
  bool isExp = (DistExp == _queue->getDistType());
  int64_t stime = 0;
  int64_t etime = 0;  
  int waitTime = 0;
  bool abort = false;
  string msg;

  try {
    while (! abort) {
      CCustomer *c = _queue->getCustomer();
      if (NULL == c) {
        abort = true;
      } else {
        stime = CUtil::getCurrentMillis();
        waitTime = _queue->getInterval(isExp, _queue->getMu());
        c->setServerId(_id);
        msg = c->getName() + string(" begin service");
        _queue->printTimeline(c->getSimStartTime(), stime, msg);
        etime = CUtil::getCurrentMillis();

        //just for safety
        if (waitTime > (etime - stime)) {
          waitTime -= (etime - stime);
        }

        stime = CUtil::getCurrentMillis();
        CUtil::sleepFor(waitTime);
        etime = CUtil::getCurrentMillis();
        
        c->setServiceTime(etime - stime);
        c->setDepartureTime(etime);
        
        msg = c->getName() + string(" departs, service time = ");
        msg += CUtil::getString(waitTime) + string("ms, time in system = ");
        msg += CUtil::getString(c->getSystemTime()) + string("ms");
        _queue->printTimeline(c->getSimStartTime(), etime, msg);
      }
    }
  } catch (CException &e) {
    CUtil::writeError(e.toString());
  }
}   

