/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cnodedata.h"

/*!
  Creates the Nodedata object
*/
CNodeData::CNodeData(const string &host, uint16_t port)
  : _host(host), _port(port) {}

/*!
  Deletes the Nodedata object
*/
CNodeData::~CNodeData() {}

/*!
  Returns the host
*/
string CNodeData::getHost() const
{
  return _host;
}

/*!
  Returns the port
*/
uint16_t CNodeData::getPort() const
{
  return _port;
}

