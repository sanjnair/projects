/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cglobal.h"
#include <openssl/sha.h>
#include "cevent.h"

/*!
  C'tor
*/
CEvent::CEvent() {}

/*!
  D'tor
*/
CEvent::~CEvent() {}

/*!
  Private copy constructor and assignment operator
*/
CEvent::CEvent(const CEvent &rhs) {}
CEvent& CEvent::operator=(const CEvent &rhs)
{
  return *this;
}

//-----------------------------------------------------------------------------
                                                                               
/*!
  Constructs an event with a message for flooding
*/
CMsgEvent::CMsgEvent(
  CMessage *m,
  const string &srcNodeId,
  CLog::MsgType logType)
{
  assert(m);
  _message = m;
  _srcNodeId = srcNodeId;
  _logType = logType;
  _probabilistic = false;
}

/*!
  Constructs an event with a message for routing
*/
CMsgEvent::CMsgEvent(
  CMessage *m,
  const string &srcNodeId,
  const UCharBuffer &uoid,
  CLog::MsgType logType)
{
  assert(m);
  assert(uoid.size() == SHA_DIGEST_LENGTH);
  _message = m;
  _srcNodeId = srcNodeId;
  _uoid = uoid;
  _logType = logType;
  _probabilistic = false;
}

/*
  Deletes the event
*/
CMsgEvent::~CMsgEvent()
{
  if (_message) {
    delete _message;
    _message = NULL;
  }
}

/*!
  Returns the event type
*/
EventType CMsgEvent::getType()
{
  return EventMsg;
}
 
/*!
  Returns the message
*/
CMessage* CMsgEvent::getMessage()
{
  return _message;
}

/*!
  Returns the source node id
*/
string CMsgEvent::getSrcNodeId() const
{
  return _srcNodeId;
}

/*!
  Return the log Msg
*/
CLog::MsgType CMsgEvent::getLogType() const
{
  return _logType;
}

/*!
  Returns true if the message needs to be flooded.
*/
bool CMsgEvent::isFlood() const
{
  return (_uoid.size() != SHA_DIGEST_LENGTH);
}

/*!
  Returns true if the message needs to be flooded.
*/
UCharBuffer CMsgEvent::getUoid() const
{
  return _uoid;
}

/*!
  Sets the probabilistic flooding enabled
*/
void CMsgEvent::setProbEnabled(bool flag)
{
  _probabilistic = flag;
}

/*!
  Returns true if the probabilistic flooding is enabled
*/
bool CMsgEvent::isProbEnabled() const
{
  return _probabilistic;
}

//-----------------------------------------------------------------------------

/*!
  C'tor
*/
CStatusMsgEvent::CStatusMsgEvent(
  CMessageStatusRes *m,
  const string &srcNodeId,
  const UCharBuffer &uoid,
  CLog::MsgType logType)
  : CMsgEvent(m, srcNodeId, uoid, logType)  {}

/*!
  D'tor
*/
CStatusMsgEvent::~CStatusMsgEvent() {}

/*!
  Returns the event type
*/
EventType CStatusMsgEvent::getType()
{
  return EventStatusMsg;
}

/*!
  Returns the status response message
*/
CMessageStatusRes* CStatusMsgEvent::getMessage()
{
  return dynamic_cast<CMessageStatusRes*>(_message);
}
