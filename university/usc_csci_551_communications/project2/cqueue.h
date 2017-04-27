/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CQUEUE_H
#define CQUEUE_H

#include "cglobal.h"
#include "ccustomer.h"

enum DistType
{
  DistExp,
  DistDet
};

class CQueue
{
public:
  virtual void insertCustomer(CCustomer *c) = 0;
  virtual CCustomer *getCustomer() = 0;
  virtual int getInterval(bool exponential, double rate) = 0;
  virtual void printTimeline(int64_t sTime, int64_t eTime, const string &msg) = 0;
  virtual void printMsg(const string &msg) = 0;
 
  virtual double getLambda() const = 0;
  virtual double getMu() const = 0;
  virtual long getSeed() const = 0;
  virtual uint getQSize() const = 0;
  virtual uint getNumCustomer() const = 0;
  virtual DistType getDistType() const = 0;
  virtual bool isSingleServer() const = 0;

  virtual void setSimStartTime(int64_t t) = 0;
  virtual int64_t getSimStartTime() = 0;
  virtual void setSimEndTime(int64_t t) = 0;
  virtual int64_t getSimEndTime() = 0;
};

#endif //CQUEUE_H
