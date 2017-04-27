/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CCUSTOMER_H
#define CCUSTOMER_H

#include "cglobal.h"
#include <list>

class CCustomer
{
public:
  explicit CCustomer(uint id);
  ~CCustomer();

  uint getId() const;
  string getName() const;

  void setSimStartTime(int64_t t);
  int64_t getSimStartTime() const;
  void setArrivalTime(int64_t t);
  int64_t getArrivalTime() const;
  void setInterArrivalTime(int64_t t);
  int64_t getInterArrivalTime() const;
  void setDepartureTime(int64_t t);
  int64_t getDepartureTime() const;
  void setQEnterTime(int64_t t);
  int64_t getQEnterTime() const;
  void setQDepartureTime(int64_t t);
  int64_t getQDepartureTime() const;
  void setServiceTime(int64_t t);
  int64_t getServiceTime() const;
  
  void dropped();
  bool isDropped() const;
  void setServerId(uint id);
  uint getServerId() const;

  uint getQtime() const;
  uint getSystemTime() const;


private:
  CCustomer(const CCustomer &rhs);
  CCustomer &operator=(const CCustomer &rhs);

  uint _id;
  int64_t _simStartTime;
  int64_t _arrivalTime;
  int64_t _interArrivalTime;
  int64_t _departureTime;
  int64_t _qEnterTime;
  int64_t _qDepartureTime;
  int64_t _serviceTime;
  bool _dropped;
  uint _serverId;
};

typedef std::list<CCustomer *> CustomerList;


#endif //CCUSTOMER_H

