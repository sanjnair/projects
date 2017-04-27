/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CMSERVER_H
#define CMSERVER_H

#include "cglobal.h"
#include "cqueue.h"
#include "cthread.h"
#include "cthread.h"
#include "cmutexlocker.h"
#include "cwaitcondition.h"

class CMserver : public CThread
{
public:
  CMserver(uint id, CQueue *q);
  virtual ~CMserver();

  virtual void run();
  uint getId() const;

private:
  uint _id;
  CQueue *_queue;
};

#endif //CMSERVER_H

