/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CEVENT_H
#define CEVENT_H

#include "cglobal.h"
#include "cmessage.h"
#include "clog.h"

enum EventType {
  EventMsg,
  EventStatusMsg
};


class CEvent
{
public:
  CEvent();
  virtual ~CEvent();
  virtual EventType getType() = 0;

private:
  CEvent(const CEvent &rhs);
  CEvent &operator=(const CEvent &rhs);
};
//-----------------------------------------------------------------------------

class CMsgEvent : public CEvent
{
public:
  CMsgEvent(
    CMessage *m,
    const string &srcNodeId,
    CLog::MsgType logType);

  CMsgEvent(
    CMessage *m,
    const string &srcNodeId,
    const UCharBuffer &uoid,
    CLog::MsgType logType);

  virtual ~CMsgEvent();
  virtual EventType getType();

  virtual CMessage *getMessage();
  string getSrcNodeId() const;
  UCharBuffer getUoid() const;
  CLog::MsgType getLogType() const;

  bool isFlood() const;
  void setProbEnabled(bool flag);
  bool isProbEnabled() const;

protected:
  CMessage *_message;
  string _srcNodeId;
  UCharBuffer _uoid;
  CLog::MsgType _logType;
  bool _probabilistic;
};

//-----------------------------------------------------------------------------
class CStatusMsgEvent : public CMsgEvent
{
public:
  CStatusMsgEvent(
    CMessageStatusRes *m,
    const string &srcNodeId,
    const UCharBuffer &uoid,
    CLog::MsgType logType);
    
  ~CStatusMsgEvent();

  virtual EventType getType();

  CMessageStatusRes *getMessage();
private:
};

#endif //CEVENT_H

