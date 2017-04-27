/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cthread.h"
#include "cexception.h"
#include "cmutexlocker.h"
#include <assert.h>

/*
  \Attention Code Reuse

  Some logic and code taken from Trolltech QT
  http://www.trolltech.com
*/

typedef void*(*ThreadCallback)(void*);

/*!
  creates a Cthread
*/
CThread::CThread() : _isRunning(false), _threadId(0)
{
}

/*!
  Deletes the thread
*/
CThread::~CThread() {}

/*!
  starts the thread
*/
void CThread::start()
{
    int ret;
    _threadArgs[0] = this;

    _isRunning = true;
    ret = pthread_create(
            &_threadId, NULL, (ThreadCallback)&CThread::start_p,
            _threadArgs);

    if (0 != ret) {
      _isRunning = false;
      string msg("Unable to start the thread");
      C_THROW(msg);
    }
}

/*!
  Returns true of the thread is running
*/
bool CThread::isRunning()
{
  CMutexLocker m(&_mutex);
  return _isRunning;
}

/*!
  Exits the thread
*/
void CThread::exit()
{
  pthread_exit(0);
}

/*!
  Joins the thread
*/
void CThread::join()
{
  bool running = false;
  {
    CMutexLocker m(&_mutex);
    running = _isRunning;
  }
  if (running) {
    int ret = pthread_join(_threadId, NULL);
    if (0 != ret) {
      string msg("Unable to join the thread");
      C_THROW(msg);
    }
  }
}

/*!
  Indicates the completion of thread execution
*/
void CThread::threadDone()
{
  CMutexLocker m(&_mutex);
  _isRunning = false;
}

/*!
  Returns the thread id of the thrad
*/
pthread_t CThread::getThreadId() const
{
  return _threadId;
}

/*!
  Calls run method on the thread
*/
void* CThread::start_p(void *arg)
{
  void **args = (void **)arg;
  CThread *c = reinterpret_cast<CThread*>(args[0]);

  if (NULL == c) {
    string msg("Null argument for Thread handle");
    C_THROW(msg);
  }
  
  c->run();
  c->threadDone();
  return 0;
}

/*!
  Sub classes must override this
*/
void CThread::run()
{
   assert(0);
   C_THROW("Run on Base class Thread is called. You must derive this method");
}
