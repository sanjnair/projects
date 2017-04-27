/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CMUTEX_H
#define CMUTEX_H

#include <pthread.h>
#include "cglobal.h"

class CMutex {

public:
    CMutex();
    ~CMutex();

    void lock();
    void unlock();
    pthread_mutex_t *getMutex();    

private:
  void errorIfInvalidCode(int code, const string &msg) const;
  pthread_mutex_t _mutex;

};


#endif //CMUTEX_H

