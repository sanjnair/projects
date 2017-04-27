/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CTHREAD_H
#define CTHREAD_H

#include <pthread.h>
#include "cglobal.h"
#include "cmutex.h"

class CThread {

public:
    CThread();
    virtual ~CThread();

    void start();
    virtual void run();
    bool isRunning();
    pthread_t getThreadId() const;
    void exit();
    void join();
    
private:
  static void *start_p(void *arg);
  void threadDone();
  
  bool _isRunning;
  CMutex _mutex;
  pthread_t _threadId;
};


#endif //CTHREAD_H

