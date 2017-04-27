/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <sys/time.h>
#include <errno.h>
#include "cwaitcondition.h"
#include "cexception.h"

/*
  \Attention Code Reuse

  Some logic and code taken from Trolltech QT
  http://www.trolltech.com
*/

/*!
    Constructs the Mutex
*/
CWaitCondition::CWaitCondition()
{
  int status = pthread_cond_init(&_condition, NULL);
  errorIfInvalidCode(status, "Unable to initialize the wait condition");
}

/*!
    Deletes the mutex
*/
CWaitCondition::~CWaitCondition()
{
  pthread_cond_destroy(&_condition);
  
}

/*!
  Waits on the mutex for the specified time.
  \Attention Code Reuse
  Some logic and code taken from Trolltech QT
  http://www.trolltech.com
*/
void CWaitCondition::wait(CMutex *mutex, unsigned long milli)
{
  int status = 0;

  if (ULONG_MAX == milli) {
    status = pthread_cond_wait(&_condition, mutex->getMutex());
    
  } else {
    struct timeval tv;
    struct timespec ts;

    if (gettimeofday(&tv, NULL) < 0) {
      C_THROW("Unable to get time of the day");
    }
    
    ts.tv_nsec = (tv.tv_usec + (milli % 100) * 1000) * 1000;
    ts.tv_sec = tv.tv_sec + (milli / 1000) + (ts.tv_nsec / 1000000000);
    ts.tv_nsec %= 1000000000;
    status = pthread_cond_timedwait(&_condition, mutex->getMutex(), &ts);
    if (status == ETIMEDOUT) {
      status = 0;
    }
  }

  errorIfInvalidCode(status, "Error during condition wait");
}
  
/*!
  Notifies one thread waiting on this condition variable
*/
void CWaitCondition::notify()
{
  int status = pthread_cond_signal(&_condition);
  errorIfInvalidCode(status, "Error during notify");
}

/*!
  Notifies all threads waiting on this condition variable
*/  
void CWaitCondition::notifyAll()
{
  int status = pthread_cond_broadcast(&_condition);
  errorIfInvalidCode(status, "Error during notifyAll");
}
             
/*!
    Error if the code is not valid
*/
void CWaitCondition::errorIfInvalidCode(int code, const string &msg) const
{
  if (code != 0) {
    C_THROW(msg);
  }
}
