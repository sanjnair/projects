/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "ccustomer.h"
#include "cutil.h"

/*!
  Creates customer
*/
CCustomer::CCustomer(uint id)
  : _id(id), _simStartTime(-1), _arrivalTime(-1), _interArrivalTime(-1),
    _departureTime(-1), _qEnterTime(-1), _qDepartureTime(-1),
    _serviceTime(0), _dropped(false), _serverId(0) {}

/*!
  Deletes Customer
*/
CCustomer::~CCustomer() {}

/*!
  Copy constructor
*/
CCustomer::CCustomer(const CCustomer &rhs) {}

/*!
  Assigment operator
*/
CCustomer& CCustomer::operator=(const CCustomer &rhs)
{
  return *this;
}


/*!
  Gets the customer id
*/
uint CCustomer::getId() const
{
  return _id;
}

/*!
  Returns the customer name
*/
string CCustomer::getName() const
{
  return (string("c") + CUtil::getString(_id));
}

void CCustomer::setSimStartTime(int64_t t)
{
  _simStartTime = t;
}

int64_t CCustomer::getSimStartTime() const
{
  return _simStartTime;
}

void CCustomer::setArrivalTime(int64_t t)
{
  _arrivalTime = t;
}

int64_t CCustomer::getArrivalTime() const
{
  return _arrivalTime;
}

void CCustomer::setInterArrivalTime(int64_t t)
{
  _interArrivalTime = t;
}

int64_t CCustomer::getInterArrivalTime() const
{
   return _interArrivalTime;
}

void CCustomer::setDepartureTime(int64_t t)
{
  _departureTime = t;
}

int64_t CCustomer::getDepartureTime() const
{
  return _departureTime;
}

void CCustomer::setQEnterTime(int64_t t)
{
  _qEnterTime = t;
}

int64_t CCustomer::getQEnterTime() const
{
  return _qEnterTime;
}

void CCustomer::setQDepartureTime(int64_t t)
{
  _qDepartureTime = t;
}

int64_t CCustomer::getQDepartureTime() const
{
  return _qDepartureTime;
}

void CCustomer::setServiceTime(int64_t t)
{
  _serviceTime = t;
}

int64_t CCustomer::getServiceTime() const
{
  return _serviceTime;
}

/*!
    Sets if customer is dropped
*/
void CCustomer::dropped()
{
  _dropped = true;
}

bool CCustomer::isDropped() const
{
  return _dropped;
}

void CCustomer::setServerId(uint id)
{
  _serverId = id;
}

uint CCustomer::getServerId() const
{
  return _serverId;
}
                      
/*!
  Returns the time spent on the Queue in milliseconds
*/
uint CCustomer::getQtime() const
{
  if ((-1 == _qEnterTime) || (-1 == _qDepartureTime)) {
    C_THROW("Queue Arrival/Departure time is not initialized");
  }
  int64_t t = _qDepartureTime - _qEnterTime;
  return static_cast<uint>(t);
}

/*!
  Returns the time spent on the System
*/
uint CCustomer::getSystemTime() const
{
  if ((-1 == _arrivalTime) || (-1 == _departureTime)) {
    C_THROW("Arrival/Departure time is not initialized");
  }
  int64_t t = _departureTime - _arrivalTime;
  return static_cast<uint>(t);
}
