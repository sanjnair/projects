/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cmutex.h"
#include "cexception.h"

/*
  \Attention Code Reuse

  Some logic and code taken from Trolltech QT
  http://www.trolltech.com
*/


/*!
    Constructs the Mutex
*/
CMutex::CMutex()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  int status = pthread_mutex_init(&_mutex, &attr);

  errorIfInvalidCode(status, "Unable Initialize Mutex");
  pthread_mutexattr_destroy(&attr);  
}

/*!
    Deletes the mutex
*/
CMutex::~CMutex()
{
  pthread_mutex_destroy(&_mutex);
}

/*!
    Locks the Mutex
*/
void CMutex::lock()
{
  int status = pthread_mutex_lock(&_mutex);
  errorIfInvalidCode(status, "Unable to Lock Mutex");
}

/*!
    Unlocks the mutex
*/
void CMutex::unlock()
{
  int status = pthread_mutex_unlock(&_mutex);
  errorIfInvalidCode(status, "Unable to Lock Mutex");
}

pthread_mutex_t* CMutex::getMutex()
{
  return &_mutex;
}

/*!
    Error if the code is not valid
*/
void CMutex::errorIfInvalidCode(int code, const string &msg) const
{
  if (code != 0) {
    C_THROW(msg);
  }
}
