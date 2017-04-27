/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CARRIVAL_H
#define CARRIVAL_H

#include "cglobal.h"
#include "cqueue.h"
#include "cthread.h"
#include "cmutexlocker.h"
#include "cwaitcondition.h"

class CArrival : public CThread
{
public:
  CArrival(CQueue *q);
  virtual ~CArrival();

  virtual void run();
  void stop();

  void calcStatistics();
  double getAvgInterArrivalTime() const;
  double getAvgServiceTime() const;
  double getAvgQ1() const;
  double getAvgS1() const;
  double getAvgS2() const;
  double getAvgSystemTime() const;
  double getSystemDeviation() const;
  double getDropProbability() const; 

private:
  void cleanup();
  double getSystemStdDeviation(double avg, uint nServed);
  
  bool isStopflagSet();
  bool _stopFlag;
  CQueue *_queue;
  CMutex _mutex;
  CustomerList _customerList;

  double _avgInter;
  double _avgService;
  double _avgQ1;
  double _avgS1;
  double _avgS2;  
  double _avgSystem;
  double _dvSystem;
  double _dropProb;
};

#endif //CARRIVAL_H
