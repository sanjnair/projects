/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CMUTEXLOCKER_H
#define CMUTEXLOCKER_H

#include "cmutex.h"

class CMutexLocker {

public:
    CMutexLocker(CMutex *m);
    ~CMutexLocker();

    CMutex *getMutex();

private:
  CMutex *_mutex;
  bool _locked;
};

#endif //CMUTEXLOCKER_H

