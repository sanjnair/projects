/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CMM2_H
#define CMM2_H

#include <list>
#include <signal.h>

#include "cglobal.h"
#include "ccustomer.h"
#include "cmutexlocker.h"
#include "cwaitcondition.h"
#include "carrival.h"
#include "cmserver.h"
#include "cqueue.h"
                 
class CMm2 : public CQueue
{
public:
  static CMm2 *getInstance();
  static void releaseInstance();
  virtual ~CMm2();

  void start(
    double lambda, double mu, long seed, uint qsize,
    uint ncustomer, DistType dtype, bool singleserver);
  void stop();

  virtual void insertCustomer(CCustomer *c);
  virtual CCustomer *getCustomer();
  virtual int getInterval(bool exponential, double rate);
  virtual void printTimeline(int64_t sTime, int64_t eTime, const string &msg);
  virtual void printMsg(const string &msg);

  virtual double getLambda() const;
  virtual double getMu() const;
  virtual long getSeed() const;
  virtual uint getQSize() const;
  virtual uint getNumCustomer() const;
  virtual DistType getDistType() const;
  virtual bool isSingleServer() const;

  virtual void setSimStartTime(int64_t t);
  virtual int64_t getSimStartTime();
  virtual void setSimEndTime(int64_t t);
  virtual int64_t getSimEndTime();
  
private:
  CMm2();
  CMm2(const CMm2 &other);
  CMm2 &operator=(const CMm2 &other);

  void cleanup();
  void startSimulation();
  void printParams() const;
  void printStatistics();
  void printStat(const string &msg, double val, bool isTime = true);
  void initRandom(long l_seed);
  int exponentialInterval(double dval, double rate);
  void initSignalMask();
  void setSignalMask(bool block) const;
  bool isStopFlagSet();
  
  bool _stopFlag;
  double _lambda;
  double _mu;
  long _seed;
  uint _qsize;
  uint _ncustomer;
  DistType _dtype;
  bool _singleserver;
  int64_t _simStartTime;
  int64_t _simEndTime;

  CArrival *_arrival;
  CMserver *_s1;
  CMserver *_s2;
  CustomerList _customerList;

  CWaitCondition _initThreadCond;
  CMutex _mutex;
  CWaitCondition _cond;
  sigset_t _signal_mask;
  CMutex _printMutex;

  static CMm2 *_instance;
  
};

#endif //CMM2_H
