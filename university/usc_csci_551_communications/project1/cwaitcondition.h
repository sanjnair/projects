/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CWAITCONDITION_H
#define CWAITCONDITION_H

#include <pthread.h>
#include "cglobal.h"
#include "cmutex.h"

class CWaitCondition {

public:
    CWaitCondition();
    ~CWaitCondition();

    void wait(CMutex *mutex, unsigned long milli = ULONG_MAX);
    void notify();
    void notifyAll();
    
private:
  void errorIfInvalidCode(int code, const string &msg) const;

  pthread_cond_t _condition;
};


#endif //CWAITCONDITION_H

