/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <math.h>
#include "carrival.h"
#include "cutil.h"

/*!
  Creates a new Arrival Thread
*/
CArrival::CArrival(CQueue *q) : CThread()
{
  _stopFlag = false;
   _queue = q;
  _avgInter = 0.0;
  _avgService = 0.0;
  _avgSystem = 0.0;
  _avgQ1 = 0.0;
  _avgS1 = 0.0;
  _avgS2 = 0.0;
  _dvSystem = 0.0;
  _dropProb = 0.0;
}

CArrival::~CArrival()
{
  cleanup();
}

/*!
  Stops the connection
*/
void CArrival::stop()
{
    CMutexLocker m(&_mutex);
    _stopFlag = true;
}

/*!
  Returns true if the stop flag is set
*/
bool CArrival::isStopflagSet()
{
    CMutexLocker m(&_mutex);
    return _stopFlag;
}

/*!
  Starts the connection activities
*/
void CArrival::run()
{
  int64_t stime = 0;
  int64_t etime = 0;

  try {
    bool isExp = (DistExp == _queue->getDistType());
    int nCustomer = _queue->getNumCustomer();

    string msg = "emulation begins";
    _queue->printTimeline(0, 0, msg);
    int64_t startTime = CUtil::getCurrentMillis();
    _queue->setSimStartTime(startTime);

    for (int i=1; i<=nCustomer; i++) {
      if (! isStopflagSet()) {
        stime = CUtil::getCurrentMillis();
        int waitTime = _queue->getInterval(isExp, _queue->getLambda());
        CCustomer *c = new CCustomer(i);
        assert(c);
        _customerList.push_back(c);
        c->setSimStartTime(startTime);
        etime = CUtil::getCurrentMillis();

        //just for safety
        if (waitTime > (etime - stime)) {
          waitTime -= (etime - stime);
        }
        
        stime = CUtil::getCurrentMillis();
        CUtil::sleepFor(waitTime);
        etime = CUtil::getCurrentMillis();
        c->setArrivalTime(etime);
        c->setInterArrivalTime(etime - stime);

        if (! isStopflagSet()) {
          msg = c->getName() + string(" arrives, inter-arrival time = ");
          msg += CUtil::getString(etime - stime) + string("ms");
          _queue->printTimeline(startTime, etime, msg);
          _queue->insertCustomer(c);
        } else {
          if (_customerList.size() > 0) {
            c = _customerList.back();
            if (NULL != c) {
              delete _customerList.back();
              _customerList.pop_back();
            }
          }
        }
      }
    }
  } catch (CException &e) {
    CUtil::writeError(e.toString());
  }
}

/*!
  Cleans up the members
*/
void CArrival::cleanup()
{
  CustomerList::iterator it;
  for (it=_customerList.begin(); it != _customerList.end(); it++) {
    delete *it;
  }
  _customerList.clear();
}

/*!
  Caculates the statistics
*/
void CArrival::calcStatistics()
{
  uint size = _customerList.size();
  uint nServed = 0;

  if (size > 0) {
    CustomerList::iterator it;
    for (it=_customerList.begin(); it != _customerList.end(); it++) {
      CCustomer *c = *it;
      if (! c->isDropped()) {
        nServed++;

        _avgService += c->getServiceTime();
        _avgQ1 += c->getQtime();
        
        if (1 == c->getServerId()) {
          _avgS1 += c->getServiceTime();
        } else if (2 == c->getServerId()) {
          _avgS2 += c->getServiceTime();
        }
        
        _avgSystem += (c->getSystemTime());
      }
      _avgInter += c->getInterArrivalTime();
    }

    _avgInter /= size;

    if (nServed > 0) {
      _avgService /= nServed;
      _avgSystem /= nServed;
      _dropProb = (double(size - nServed))/double(size);
    }
    _dvSystem = getSystemStdDeviation(_avgSystem, nServed);
  }

  double simInterval = _queue->getSimEndTime() - _queue->getSimStartTime();
  if (simInterval > 0.0) {
    _avgS1 /= simInterval;
    _avgS2 /= simInterval;
    _avgQ1 /= simInterval;
  }
}

/*!
  Returns the standard deviation for the system time
*/
double CArrival::getSystemStdDeviation(double avg, uint nServed)
{
  double deviation = 0.0;
  for (CustomerList::iterator it=_customerList.begin();
       it != _customerList.end(); it++) {

    CCustomer *c = *it;
    if (! c->isDropped()) {
      double diff = (double(c->getSystemTime()) / 1000.0) - (avg/1000.0);
      diff = pow(diff, 2.0);
      deviation += diff;
    }
  }

  if (deviation > 0.0) {
    deviation = sqrt(deviation) /double(nServed-1);
    //covert to milliseconds
    deviation *= 1000.0;
  }

  return deviation;
}

/*!
  Returns Average Interarrival time
*/
double CArrival::getAvgInterArrivalTime() const
{
  return _avgInter;
}

/*!
  Returns Average Service time
*/
double CArrival::getAvgServiceTime() const
{
   return _avgService;
}

/*!
  Returns average Q1 time
*/
double CArrival::getAvgQ1() const
{
  return _avgQ1;
}

/*!
  Returns average S1 time
*/
double CArrival::getAvgS1() const
{
  return _avgS1;
}

/*!
  Returns average S2 time
*/
double CArrival::getAvgS2() const
{
  return _avgS2;
}


/*!
  Returns the average time spent in the system
*/
double CArrival::getAvgSystemTime() const
{
  return _avgSystem;
}

double CArrival::getSystemDeviation() const
{
  return _dvSystem;
}

/*!
  Returns customer drop probability
*/
double CArrival::getDropProbability() const
{
  return _dropProb;
}
