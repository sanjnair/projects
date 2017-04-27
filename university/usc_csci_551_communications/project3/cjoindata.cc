/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cjoindata.h"
#include "cutil.h"
#include "cconst.h"

/*!
  Creates the JoinData object
*/
CJoinData::CJoinData()
  : _port(0), _distance(0) {}


/*!
  Creates the JoinData object
*/
CJoinData::CJoinData(
  const string &host, uint16_t port, uint32_t distance)
  : _host(host), _port(port), _distance(distance) {}

/*!
  Deletes the JoinData object
*/
CJoinData::~CJoinData() {}

/*!
  Returns the host
*/
string CJoinData::getHost() const
{
  return _host;
}

/*!
  Returns the port
*/
uint16_t CJoinData::getPort() const
{
  return _port;
}

/*!
  Returns the Distance
*/
uint32_t CJoinData::getDistance() const
{
  return _distance;
}

/*!
  Operator helps in sorting the data
*/
bool CJoinData::operator < (const CJoinData& rhs)
{
  return _distance < rhs._distance;
}

/*!
  Returns the string format of this object that can be
  use to save to file
*/
string CJoinData::toString() const
{
  return (_host + CConst::SEP_NODE + CUtil::getString(_port));
}
