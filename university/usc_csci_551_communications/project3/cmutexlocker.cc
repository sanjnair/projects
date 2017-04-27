/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cmutexlocker.h"

/*!
  Creates a mutex locker that locks the given mutex
*/
CMutexLocker::CMutexLocker(CMutex *m) : _mutex(m), _locked(false)
{
  _mutex->lock();
  _locked = true;
}
             
CMutexLocker::~CMutexLocker()
{
  if (_locked) {
    _mutex->unlock();
  }
}

CMutex* CMutexLocker::getMutex()
{
  return _mutex;
}

