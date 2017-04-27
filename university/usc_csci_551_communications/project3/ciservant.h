/***************************************************************************
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CISERVANT_H
#define CISERVANT_H

#include <signal.h>
#include "cglobal.h"

class CIniParser;
class CLog;
class CUoidMap;
class CEvent;
class CMessage;
class CFileMgr;

class IServant
{
public:
  virtual CIniParser *getIniParser() = 0;
  virtual CLog *getLog() = 0;
  virtual CUoidMap *getUoidMap() = 0;
  virtual string getNborFilePath() const = 0;
  virtual bool isNodeJoining() const = 0;
  virtual string getHostName() const = 0;
  virtual string getNodeId() const = 0;
  virtual string getNodeInstanceId() const = 0;
  virtual void stop() = 0;
  virtual void setLastError(const string &s) = 0;
  virtual void setSigMask(sigset_t *set, bool block) = 0;
  virtual bool replaceNborNode(const string &curNodeId, const string &newNodeId) = 0;
  virtual void addEvent(CEvent *e) = 0;
  virtual void notifyResponse(const CMessage *m) = 0;
  virtual bool handleUserInput(const string &input) = 0;
  virtual UCharBuffer getNewUoid(const string &type) const = 0;
  virtual bool isMsgSource(uint8_t type, const UCharBuffer &reqUoid) = 0;
  virtual void setCheckRequired() = 0;
  virtual uint getDefTimeout() = 0;
  virtual bool isPositiveProb(double v) = 0;
  virtual CFileMgr *getFileMgr() = 0;
};

#endif //CISERVANT_H
