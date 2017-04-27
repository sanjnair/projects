/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include "cnode.h"
#include "cexception.h"
#include "cconst.h"
#include "cautoptr.h"
#include "cfile.h"
#include "cmutexlocker.h"
#include "cmessagefactory.h"
#include "cmessage.h"
#include "clog.h"
#include "cifstream.h"
#include "ciniparser.h"
#include "cfilemgr.h"
#include "cuoidmap.h"
#include "cevent.h"

/*!
  Constructs a Node
*/
CNode::CNode(
  const string &srcHost,
  uint16_t srcPort,
  const string &rmtHost,
  uint16_t rmtPort,
  bool connInitiated,
  CTcpSocket *sockP)
{
  assert(sockP);
  
  _stopFlag = false;
  _helloRcvd = false;
  _helloSent = false;
  _checkRequired = false;
  _notifyReceived = false;

  _keepAliveTime = 0;
  _lastMsgRcvTime  = 0;
  _curTime = 0;
  
  _servant = NULL;
  _nodeType = NodeRegular;

  _socketP = sockP;
  _srcHost = srcHost;
  _srcPort = srcPort;
  _rmtHost = rmtHost;
  _rmtPort = rmtPort;
  _connInitiated = connInitiated;
}

/*!
  Deletes a Node
*/
CNode::~CNode()
{
  cleanup();
}

/*!
  Private Copy constructor and assignment operator
*/
CNode::CNode(const CNode &rhs) {}
CNode& CNode::operator =(const CNode &rhs)
{
  return *this;
}

/*!
  Sets the server handle
*/
void CNode::setServantHandle(IServant *s)
{
  _servant = s;
}

/*!
  Returns true if the connection is initiated
  by this node
*/
bool CNode::isConnInitiated()
{
  return _connInitiated;
}

/*!
  Returns true if the node is neighbor to
  the current node. A node is a neighbor only
  if it exchanged hello with this node.
*/
bool CNode::isNeighbor()
{
  CMutexLocker m(&_mutex);
  return _helloRcvd;
}

/*!
  Stops Node
*/
void CNode::stop()
{
  if (! isStopFlagEnabled()) {
    {
      CMutexLocker m(&_mutex);
      _stopFlag = true;
    }

    if (_socketP) {
      if (isNeighbor() && (! isNotifyReceived()) &&
         _nodeSendThread.isRunning()) {
        _nodeSendThread.setSendNotify(
            getNodeId(), CConst::NOTIFY_SHUTDOWN);

        _nodeSendThread.stop();
      }  else {
        _socketP->stop();
      }
    }
  }
}

/*!
  Returns true if the stop flag is enabled
*/
bool CNode::isStopFlagEnabled()
{
  CMutexLocker m(&_mutex);
  return _stopFlag;
}

/*!
  Returns the const pointer to the socket
*/
const CTcpSocket* CNode::getSocket() const
{
  if (NULL == _socketP) {
    C_THROW("getSocket: Socket is NULL");
  }
  return _socketP;
}

/*!
  Returns the pointer to the socket
*/
CTcpSocket* CNode::getSocket()
{
  if (NULL == _socketP) {
    C_THROW("getSocket: Socket is NULL");
  }
  return _socketP;
}

/*!
  Returns the source host
*/
string CNode::getSrcHost()
{
  return _srcHost;
}

/*!
  Returns the source port
*/
uint16_t CNode::getSrcPort()
{
  CMutexLocker m(&_mutex);
  return _srcPort;
}

/*!
  \internal
  Sets the source port
*/
void CNode::setSrcPort(uint16_t port)
{
  CMutexLocker m(&_mutex);
  _srcPort = port;
}

/*!
  Returns the remote host
*/
string CNode::getRmtHost()
{
  return _rmtHost;
}

/*!
  Returns the remote port
*/
uint16_t CNode::getRmtPort()
{
  return _rmtPort;
}

/*!
  Returns the node id
*/
string CNode::getNodeId()
{
  if(_connInitiated) {
    return CNode::getNodeId(_rmtHost, _rmtPort);
  }
  return CNode::getNodeId(_srcHost, _srcPort);
}

/*!
  Sets the Cur time
*/
void CNode::setCurTime(uint curTime)
{
  CMutexLocker locker(&_mutex);
  _curTime = curTime;
}

/*!
  Sets the Cur time
*/
uint CNode::getCurTime()
{
  CMutexLocker locker(&_mutex);
  return _curTime;
}

/*!
  Sets the Last Msg receive time
*/
void CNode::setLastMsgRecvTime(uint t)
{
  CMutexLocker locker(&_mutex);
  _lastMsgRcvTime = t;
}

/*!
  Returns the Last Msg receive time
*/
uint CNode::getLastMsgRecvTime()
{
  CMutexLocker locker(&_mutex);
  return _lastMsgRcvTime;
}

/*!
  Sets the keep alive time
*/
void CNode::setKeepAliveTime(uint t)
{
  CMutexLocker locker(&_mutex);
  _keepAliveTime = t;
}

/*!
  Returns the keep alive time
*/
uint CNode::getKeepAliveTime()
{
  CMutexLocker locker(&_mutex);
  return _keepAliveTime;
}


/*!
  Timer tick event called by main thread's timer
*/
void CNode::timerTick(uint curTime)
{
  if (isStopFlagEnabled()) {
    return;
  }

  if (0 == getCurTime()) {
    setKeepAliveTime(curTime);
    setLastMsgRecvTime(curTime);
  }

  setCurTime(curTime);

  if (isNeighbor()) {
    CIniParser *iniP = _servant->getIniParser();
    uint kAliveTimeout = iniP->getKeepAliveTimeout();
    uint kAliveInterval = static_cast<uint>(0.5 * kAliveTimeout);

    bool checkReq = false;

    if (CUtil::isElapsed(getLastMsgRecvTime(), curTime, kAliveTimeout)) {
      checkReq = true;

    } else if (CUtil::isElapsed(getKeepAliveTime(), curTime, kAliveInterval)) {
      try {
        sendNewKeepAliveMsg(); //TODO - REMOVE THIS COMMENT
        setKeepAliveTime(curTime);

      } catch (CException &e) {
        _servant->getLog()->logError(e.toString());
      }
    }

    if (checkReq) {
      string msg = string("Keep Alive timeout occurred. Node is ") + getNodeId();
      _servant->getLog()->logDebug(msg); //TODO - REMOVE THIS COMMENT
      //printf("%s\n", msg.c_str());
      setCheckRequired(true);
      stop(); //TODO - REMOVE THIS COMMENT
    }
  }
}

/*!
  Sets the initCheck flag
*/
void CNode::setCheckRequired(bool flag)
{
  if (NodeRegular == _nodeType) {
    CMutexLocker locker(&_mutex);
    _checkRequired = flag;
  }
}

/*!
  Returns true if check is required
*/
bool CNode::isCheckRequired()
{
  if (NodeRegular == _nodeType) {

    CMutexLocker locker(&_mutex);
    return _checkRequired;
  }
  return false;
}

/*!
  Sets the notify received flag
*/
void CNode::setNotifyReceived(bool flag)
{
  CMutexLocker m(&_mutex);
  _notifyReceived = flag;
}

/*!
  Returns true of the nofify us received
*/
bool CNode::isNotifyReceived()
{
  CMutexLocker m(&_mutex);
  return _notifyReceived;
}

/*!
  Starts running the thread
*/
void CNode::run()
{
  assert(_servant);

  try {
    if (_servant->getIniParser()->isBeaconNode()) {
      _nodeType = NodeBeacon;
    } else if(_servant->isNodeJoining()) {
      _nodeType = NodeJoin;
    } else {
      _nodeType = NodeRegular;
    }

    _nodeSendThread.setTcpSock(_socketP);
    _nodeSendThread.setServantHandle(_servant);
    _nodeSendThread.start();
    initConn();

  } catch (CException &e) {
    string msg = string("node id: ") + getNodeId();
    msg += string(", ") + e.toString();
    _servant->getLog()->logError(msg);
    //printf("%s\n", msg.c_str());

    if (! isStopFlagEnabled()) {
      setCheckRequired(true);
    }
  }

  _stopFlag = true;

  if (isCheckRequired()) {
    _servant->setCheckRequired();
  }

  if (_nodeSendThread.isRunning()) {
    _nodeSendThread.stop();
    _nodeSendThread.join();
  }
  _servant->getLog()->logDebug(getNodeId() + " finished processing ...");
}

/*!
  Initialize the connection
*/
void CNode::initConn()
{
  CMessage *messageP = NULL;

  switch (_nodeType) {
    case NodeBeacon:
      if (isConnInitiated()) {
        sendNewHelloMsg();
      }
      break;
    case NodeRegular:
      sendNewHelloMsg();
      break;
    case NodeJoin:
      sendNewJoinReqMsg();
      break;
  }

  while ((! isStopFlagEnabled()) && (! isNotifyReceived()))   {
    messageP = CMessageFactory::createMsg(_socketP);

    if (! isStopFlagEnabled()) {
      setLastMsgRecvTime(getCurTime());
      handleMsg(messageP);
    }
  }
}

/*!
  Cleans up the object
*/
void CNode::cleanup()
{
  if (_socketP) {
    delete _socketP;
    _socketP = NULL;
  }
}

//---------------------------------------------------------------------------
// Message handling helpers
//---------------------------------------------------------------------------

/*!
  Handles the message
*/
void CNode::handleMsg(CMessage *m)
{
  assert(m);

  if (! isNotifyReceived()) {
    if (CConst::TYPE_HELLO == m->getType()) {
      handleHelloMsg(m);

    } else if(CConst::TYPE_JOIN_REQ == m->getType()) {
      handleJnrqMsg(m);

    } else if (CConst::TYPE_JOIN_RES == m->getType()) {
      handleJnrsMsg(m);

    } else if (CConst::TYPE_KEEPALIVE == m->getType()) {
      handleKeepAliveMsg(m);

    } else if (CConst::TYPE_NOTIFY == m->getType()) {
      handleNotifyMsg(m);

    } else if (CConst::TYPE_STATUS_REQ == m->getType()) {
      handleStatusReqMsg(m);

    } else if (CConst::TYPE_STATUS_RES == m->getType()) {
      handleStatusResMsg(m);

    } else if (CConst::TYPE_CHECK_REQ == m->getType()) {
      handleCheckReqMsg(m);

    } else if (CConst::TYPE_CHECK_RES == m->getType()) {
      handleCheckResMsg(m);

    } else if (CConst::TYPE_STORE == m->getType()) {
      handleStoreMsg(m);
      
    } else if (CConst::TYPE_SEARCH_REQ == m->getType()) {
      handleSearchReqMsg(m);

    } else if (CConst::TYPE_SEARCH_RES == m->getType()) {
      handleSearchResMsg(m);

    }  else if (CConst::TYPE_GET_REQ == m->getType()) {
      handleGetReqMsg(m);

    } else if (CConst::TYPE_GET_RES == m->getType()) {
      handleGetResMsg(m);

    } else if (CConst::TYPE_DELETE == m->getType()) {
      handleDeleteMsg(m);
      
    } else {
      string msg = string("Message type ") + CUtil::getString(m->getType());
      msg += " Not supported";
      C_THROW(msg);
    }
  }
}

/*!
  Handles the hello message
*/
void CNode::handleHelloMsg(CMessage *m)
{
  CAutoPtr<CMessageHello> messageP(dynamic_cast<CMessageHello*>(m));
  assert(messageP.get());

  string host = messageP->getHostName();
  uint16_t port = messageP->getPort();

  string nodeid = CNode::getNodeId(host, port);
  _servant->getLog()->logMessage(CLog::MsgRcvd, nodeid, m);

  if (! isConnInitiated()) {
    if (getSrcPort() != messageP->getPort()) {
      if ( ! replaceNodeId(host, port)) {
        stop();
      }
    }
  }

  if (! isStopFlagEnabled()) {
    _helloRcvd = true;

    if (!_helloSent) {
      sendNewHelloMsg();
    }
  }
}

/*!
  Handles the join request message
*/
void CNode::handleJnrqMsg(CMessage *m)
{
  CAutoPtr<CMessageJnrq> messageP(dynamic_cast<CMessageJnrq*>(m));
  assert(messageP.get());

    string host = messageP->getHostName();
    uint16_t port = messageP->getPort();
    string reqNodeid = CNode::getNodeId(host, port);

  _servant->getLog()->logMessage(CLog::MsgRcvd, reqNodeid, m);

  if (_servant->getUoidMap()->add(
          m->getUoid(), getNodeId(), getCurTime())) {

    if (! isNeighbor()) {
      _jnrqNodeId = reqNodeid;
    }
    sendNewJoinResMsg(messageP.get());
    dispatchMsg(messageP.release());
  }
}

/*!
  Handles the join response message
*/
void CNode::handleJnrsMsg(CMessage *m)
{
  CAutoPtr<CMessageJnrs> messageP(dynamic_cast<CMessageJnrs*>(m));
  assert(messageP.get());

  string nodeid = CNode::getNodeId(messageP->getHostName(), messageP->getPort());
  _servant->getLog()->logMessage(CLog::MsgRcvd, nodeid, m);

  if (_servant->isNodeJoining()) {
    _servant->notifyResponse(messageP.get());
  } else {
    CMessageJnrs *mj = messageP.release();
    dispatchMsg(mj, mj->getReqUoid());
  }
}

/*!
  Handles the keep alive message
*/
void CNode::handleKeepAliveMsg(CMessage *m)
{
  CAutoPtr<CMessageKeepAlive> messageP(dynamic_cast<CMessageKeepAlive*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);
}

/*!
  Handles the Notify message
*/
void CNode::handleNotifyMsg(CMessage *m)
{
  CAutoPtr<CMessageNotify> messageP(dynamic_cast<CMessageNotify*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);
  setNotifyReceived(true);
  setCheckRequired(true);
  stop();
}

/*!
  Handles the status request message
*/
void CNode::handleStatusReqMsg(CMessage *m)
{
  CAutoPtr<CMessageStatusReq> messageP(dynamic_cast<CMessageStatusReq*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->getUoidMap()->add(
        m->getUoid(), getNodeId(), getCurTime())) {

    sendNewStatusResMsg(messageP.get());
    dispatchMsg(messageP.release());
  }
}

/*
  Handles the status response message
*/
void CNode::handleStatusResMsg(CMessage *m)
{
  CAutoPtr<CMessageStatusRes> messageP(dynamic_cast<CMessageStatusRes*>(m));
  assert(messageP.get());

  string nodeid = CNode::getNodeId(messageP->getHostName(), messageP->getPort());
  _servant->getLog()->logMessage(CLog::MsgRcvd, nodeid, m);

  if (_servant->isMsgSource(messageP->getType(), messageP->getReqUoid())) {
    _servant->notifyResponse(messageP.get());

  } else {
    CMessageStatusRes *mj = messageP.release();
    dispatchMsg(mj, mj->getReqUoid());
  }
}

/*
  Handles the check request message
*/
void CNode::handleCheckReqMsg(CMessage *m)
{
  CAutoPtr<CMessageCheckReq> messageP(dynamic_cast<CMessageCheckReq*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->getUoidMap()->add(
          m->getUoid(), getNodeId(), getCurTime())) {

    if (_servant->getIniParser()->isBeaconNode()) {
      sendNewCheckResMsg(messageP.get());
    }
    dispatchMsg(messageP.release());
  }
}

/*
  Handles the check response message
*/
void CNode::handleCheckResMsg(CMessage *m)
{
  CAutoPtr<CMessageCheckRes> messageP(dynamic_cast<CMessageCheckRes*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->isMsgSource(messageP->getType(), messageP->getReqUoid())) {
    _servant->notifyResponse(messageP.get());

  } else {
    CMessageCheckRes *mj = messageP.release();
    dispatchMsg(mj, mj->getReqUoid());
  }
}

/*!
  Handles the store message
*/
void CNode::handleStoreMsg(CMessage *m)
{
  CAutoPtr<CMessageStore> messageP(dynamic_cast<CMessageStore*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->getUoidMap()->add(
        m->getUoid(), getNodeId(), getCurTime())) {

    if (_servant->isPositiveProb(
          _servant->getIniParser()->getStoreProb())) {

      try {          
        _servant->getFileMgr()->cacheFile(messageP.get());
        
      } catch (CException &e) {
        _servant->getLog()->logError(e.toString());
      }
    }
    dispatchMsg(messageP.release(), true);
  }
}

/*!
  Handles the search request message
*/
void CNode::handleSearchReqMsg(CMessage *m)
{
  CAutoPtr<CMessageSearchReq> messageP(dynamic_cast<CMessageSearchReq*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->getUoidMap()->add(
        m->getUoid(), getNodeId(), getCurTime())) {

    
    uint8_t searchType = messageP->getSearchType();
    string query = messageP->getQuery();
    MetaDataVector vct = _servant->getFileMgr()->search(
                            searchType, query);

    if (vct.size() > 0) {
      sendNewSearchResMsg(messageP.get(), vct);
    }
    dispatchMsg(messageP.release());
  }
}

/*!
  Handles the search response message
*/
void CNode::handleSearchResMsg(CMessage *m)
{
  CAutoPtr<CMessageSearchRes> messageP(dynamic_cast<CMessageSearchRes*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->isMsgSource(messageP->getType(), messageP->getReqUoid())) {
    _servant->notifyResponse(messageP.get());

  } else {
    CMessageSearchRes *mj = messageP.release();
    dispatchMsg(mj, mj->getReqUoid());
  }
}

/*!
  Handles the Get request message
*/
void CNode::handleGetReqMsg(CMessage *m)
{
  CAutoPtr<CMessageGetReq> messageP(dynamic_cast<CMessageGetReq*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->getUoidMap()->add(
        m->getUoid(), getNodeId(), getCurTime())) {

    UCharBuffer fileId = messageP->getFileId();

    if (_servant->getFileMgr()->isFileIdPresent(fileId)) {
      sendNewGetResMsg(messageP.get(), fileId);
      
    } else {
      dispatchMsg(messageP.release());
    }
  }
}

/*!
  Handles the Get response message
*/
void CNode::handleGetResMsg(CMessage *m)
{
  CAutoPtr<CMessageGetRes> messageP(dynamic_cast<CMessageGetRes*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->isMsgSource(messageP->getType(), messageP->getReqUoid())) {
    _servant->notifyResponse(messageP.get());

  } else {
    //cache the file if the probability is positive
    CIniParser *iniP = _servant->getIniParser();
    
    if (_servant->isPositiveProb(iniP->getCacheProb())) {
      try {
        _servant->getFileMgr()->cacheFile(messageP.get());

      } catch (CException &e) {
        _servant->getLog()->logError(e.toString());
      }
    }

    CMessageGetRes *mj = messageP.release();
    dispatchMsg(mj, mj->getReqUoid());
  }
}

/*!
  Handles delete message
*/
void CNode::handleDeleteMsg(CMessage *m)
{
  CAutoPtr<CMessageDelete> messageP(dynamic_cast<CMessageDelete*>(m));
  assert(messageP.get());

  _servant->getLog()->logMessage(CLog::MsgRcvd, getNodeId(), m);

  if (_servant->getUoidMap()->add(
        m->getUoid(), getNodeId(), getCurTime())) {

    _servant->getFileMgr()->deleteFile(messageP->getFileSpec());
    dispatchMsg(messageP.release(), true);
  }
}

//-----------------------------------------------------------------------------  

/*!
  Replaces the Node Id to the actual host and port
*/
bool CNode::replaceNodeId(
  const string &host, uint16_t port)
{
  bool bReplace = false;

  string curNodeId = getNodeId();
  setSrcPort(port);
  string newNodeId = getNodeId();

  try {
    bReplace = _servant->replaceNborNode(curNodeId, newNodeId);

    if (! bReplace) {
      string msg = "Unable to replace node ";
      msg += curNodeId + string("with ");
      msg += newNodeId;
      _servant->getLog()->logDebug(msg);
    }

  } catch (CException &e) {
    _servant->getLog()->logError(e.toString());
  }

  return bReplace;
}

//---------------------------------------------------------------------------
// Message sending calls
//---------------------------------------------------------------------------

/*!
  Sends the given message through the socket.
*/
void CNode::sendMessage(CMessage *m, CLog::MsgType t)
{
  if (! isStopFlagEnabled()) {
    assert(m);
    if (! isNotifyReceived()) {
      _nodeSendThread.addMessage(m->clone());

      string nodeid = getNodeId();
      if (CConst::TYPE_JOIN_RES == m->getType()) {
        if (! isNeighbor()) {
          nodeid = _jnrqNodeId;
        }
      }
      _servant->getLog()->logMessage(t, nodeid, m);
    }
  }
}

/*!
  Dispatches the message to the event
  dispatcher. Event dispatcher will flood this message.
*/
void CNode::dispatchMsg(
  CMessage *m, bool probEnabled)
{
  CMsgEvent *e = new CMsgEvent(m, getNodeId(), CLog::MsgFrwd); assert(e);
  e->setProbEnabled(probEnabled);
  _servant->addEvent(e);
}

/*!
  Dispatches the message to the event
  dispatcher. Event dispatcher will user the
  uoid to route the message.
*/
void CNode::dispatchMsg(CMessage *m, const UCharBuffer &uoid)
{
  CMsgEvent *e = new CMsgEvent(m, getNodeId(), uoid, CLog::MsgFrwd); assert(e);
  _servant->addEvent(e);
}

/*!
  Sends the hello Message
*/
void CNode::sendNewHelloMsg()
{
  CMessageHello m(
    getNewUoid(), _servant->getIniParser()->getPort(),
    _servant->getHostName());

  sendMessage(&m, CLog::MsgSent);
  _helloSent = true;
}

/*!
  Sends the new join request message
*/
void CNode::sendNewJoinReqMsg()
{
  CIniParser *iniP = _servant->getIniParser();

  CMessageJnrq msg(
      getNewUoid(), iniP->getTtl(), iniP->getLocation(),
      iniP->getPort(), _servant->getHostName());

  sendMessage(&msg, CLog::MsgSent);
}

/*!
  Sends New Join Response message
*/
void CNode::sendNewJoinResMsg(CMessageJnrq *m)
{
  CIniParser *iniP = _servant->getIniParser();
  uint distance = labs(iniP->getLocation() - m->getHostLoc());

  CMessageJnrs msg(
    getNewUoid(), m->getUoid(), distance,
    iniP->getPort(), _servant->getHostName());

  sendMessage(&msg, CLog::MsgSent);
}

/*!
  Sends new keep alive message
*/
void CNode::sendNewKeepAliveMsg()
{
  CMessageKeepAlive msg(getNewUoid());
  sendMessage(&msg, CLog::MsgSent);
}

/*!
  Sends New Status Response message
*/
void CNode::sendNewStatusResMsg(CMessageStatusReq *m)
{
  CIniParser *iniP = _servant->getIniParser();
  CMessageStatusRes *msgP = new CMessageStatusRes(
                              getNewUoid(),
                              m->getUoid(),
                              iniP->getPort(),
                              _servant->getHostName());

  if (CConst::STATUS_NEIGHBORS == m->getStatusType()) {
    CStatusMsgEvent * eP = new CStatusMsgEvent(
                              msgP, getNodeId(),
                              m->getUoid(),
                              CLog::MsgSent);
    _servant->addEvent(eP);                        
          
  } else {
    CAutoPtr<CMessageStatusRes> mP(msgP);
    msgP->setMetaDataVec(
      _servant->getFileMgr()->getAllMetaInfo());
      
    sendMessage(msgP, CLog::MsgSent);
  }
}

/*!
  Sends New Check Response message
*/
void CNode::sendNewCheckResMsg(CMessageCheckReq *m)
{
  CMessageCheckRes msg(getNewUoid(), m->getUoid());
  sendMessage(&msg, CLog::MsgSent);
}

/*!
  Sends New Search Response message
*/
void CNode::sendNewSearchResMsg(
  CMessageSearchReq *m, const MetaDataVector &vct)
{
  CMessageSearchRes msg(getNewUoid(), m->getUoid());
  msg.setMetaDataVec(vct);
  sendMessage(&msg, CLog::MsgSent);
}

/*!
  Sends New Get Response message
*/
void CNode::sendNewGetResMsg(
  CMessageGetReq *m, const UCharBuffer &fileId)
{
  CMessageGetRes msg(getNewUoid());
  msg.setReqUoid(m->getUoid());

  _servant->getFileMgr()->getFile(fileId, &msg);
  sendMessage(&msg, CLog::MsgSent);
}

//---------------------------------------------------------------------------
// Helper methods
//---------------------------------------------------------------------------
/*!
  Returns the New UOID
*/
UCharBuffer CNode::getNewUoid() const
{
  return _servant->getNewUoid(CConst::OBJ_TYPE_MSG);
}

//---------------------------------------------------------------------------
// Static Helper methods
//---------------------------------------------------------------------------

/*!
  Returns the node id for the given hostname and port
*/
string CNode::getNodeId(const string &host, uint16_t port)
{
  return (host + NODEID_SEP + CUtil::getString(port));
}

/*!
  Returns the node instance id for the given hostname and port
*/
string CNode::getNodeInstanceId(const string &nodeId)
{
  return (nodeId + NODEID_SEP + CUtil::getString(time(NULL)));
}

/*!
  Returns the hostname part from the node id
*/
string CNode::getHostName(const string &nodeId)
{
  string name;
  string value;
  CUtil::getNvp(nodeId, NODEID_SEP, name, value);
  return name;
}

/*!
  Returns the port part from the node id
*/
uint16_t CNode::getPort(const string &nodeId)
{
  string name;
  string value;
  CUtil::getNvp(nodeId, NODEID_SEP, name, value);
  return CUtil::getInteger<uint16_t>(value);
}

const string CNode::NODEID_SEP  = "_";

//-----------------------------------------------------------------------------

/*!
  Creates Node sender thread
*/
CNodeSendThread::CNodeSendThread()
{
  _stopFlag = false;
  _notifyFlag = false;
  _notifyCode = CConst::NOTIFY_SHUTDOWN;
  _sock = NULL;
  _servant = NULL;
}

/*!
  Deletes Node sender thread
*/
CNodeSendThread::~CNodeSendThread()
{
  cleanup();
}

/*!
  Sets the TCP socket
*/
void CNodeSendThread::setTcpSock(CTcpSocket *sock)
{
  _sock = sock;
}

/*!
  Sets the servant handle
*/
void CNodeSendThread::setServantHandle(IServant *s)
{
  _servant = s;
}

/*!
  Adds an event to the queue
*/
void CNodeSendThread::addMessage(CMessage *message)
{
  CMutexLocker m(&_mutex);
  _messageQueue.push_back(message);
  _waitCond.notifyAll();
}

/*!
  Sets the notification flag and code
*/
void CNodeSendThread::setSendNotify(
  const string &nodeid, uint8_t code)
{
  _nodeid = nodeid;
  _notifyFlag = true;
  _notifyCode = code;
}

/*!
  Stops the Sender thread
*/
void CNodeSendThread::stop()
{
  _stopFlag = true;
  CMutexLocker m(&_mutex);
  _waitCond.notifyAll();
}

/*!
  Returns true if the stop flag is enabled
*/
bool CNodeSendThread::isStopFlagEnabled()
{
  CMutexLocker m(&_mutex);
  return _stopFlag;
}

/*!
  Cleanup the queue and deletes all Events
*/
void CNodeSendThread::cleanup()
{
  if (_messageQueue.size() > 0) {
    MessageQueue::const_iterator it;
    for (it = _messageQueue.begin(); it != _messageQueue.end(); it++) {
      delete *it;
    }
    _messageQueue.clear();
  }
}

/*!
  Takes a message out from the queue
*/
CMessage* CNodeSendThread::getMessage_p()
{
  CMutexLocker m(&_mutex);
  while ((! _stopFlag) && (_messageQueue.empty())) {
     _waitCond.wait(&_mutex);
  }

  CMessage *msg = NULL;

  if (! _stopFlag) {
    msg = _messageQueue.front();
    _messageQueue.pop_front();
  }

  return msg;
}

/*!
  Starts running the thread
*/
void CNodeSendThread::run()
{
  assert(_sock);
  assert(_servant);
  
  CMessage *m = NULL;
  while (! isStopFlagEnabled()) {
    m = getMessage_p();

    if (NULL != m) {
      try {
        if (CConst::TYPE_STORE == m->getType()) {
          sendMsgStore(m, _sock);

        } else if (CConst::TYPE_GET_RES == m->getType()) {
          sendMsgGetRes(m, _sock);
          
        } else {
          _sock->sendTo(m->toUcharBuffer());
          
        }
      } catch (CException &e) {
        _servant->getLog()->logError(e.toString());
      }
    }
  }

  if (_notifyFlag) {
    sendNewNotifyMsg();
    _sock->stop();
  }
}

/*!
  Sends new keep Notify message
*/
void CNodeSendThread::sendNewNotifyMsg()
{
  try {
    UCharBuffer uoid = _servant->getNewUoid(CConst::OBJ_TYPE_MSG);
    CMessageNotify m(uoid, _notifyCode);
    _servant->getLog()->logMessage(CLog::MsgSent, _nodeid, &m);
    _sock->sendTo(m.toUcharBuffer());
    
  } catch (CException &e) {
    _servant->getLog()->logError(e.toString());
  }
}

/*!
  Sends the msg store over the socket
*/
void CNodeSendThread::sendMsgStore(
  CMessage *m, CTcpSocket *sock)
{
  CMessageStore *mP = dynamic_cast<CMessageStore *>(m);
  assert(mP);

  sendMsg(
    mP->getFilePath(),
    mP->getFileDataSize(),
    m,
    sock);
}

/*!
  Sends the msg Get Response the socket
*/
void CNodeSendThread::sendMsgGetRes(
  CMessage *m, CTcpSocket *sock)
{
  CMessageGetRes *mP = dynamic_cast<CMessageGetRes *>(m);
  assert(mP);
  
  sendMsg(
    mP->getFilePath(),
    mP->getFileDataSize(),
    m,
    sock);
}

/*!
  Reads the file contents from file and sends the message content
  and file content over the socket.
*/
void CNodeSendThread::sendMsg(
  const string &fPath, uint fSize,
  CMessage *m, CTcpSocket *sock)
{
  CFile::errorIfNotExist(fPath);
  CFile f(fPath);
  uint fileSize = f.size();

  if (fileSize != fSize) {
    string msg = string("Size of file ") + fPath;
    msg += string(" (") + CUtil::getString(fileSize) + string(")");
    msg += string(" is not same as message file size (");
    msg += string(" (") + CUtil::getString(fSize) + string(")");    

    C_THROW(msg);
  }

  CIfStream stream(&f, true);
  stream.open();

  try {
    //first send the message buffer over the socket.
    sock->sendTo(m->toUcharBuffer());
    
    UCharBuffer b;
    b.resize(FILE_BUF_SIZE);
    char *buf = reinterpret_cast<char*>(&(b[0]));

    //printf("file size %d \n", fSize);

    while (stream.isGood() && ! stream.isEof()) {
      uint len = stream.read(buf, CConst::SOCK_RW_SIZE);
      UCharBuffer messageBuf = CMessage::getBuffer(b, 0, len);
      //printf("sending %d bytes over socket\n", len);
      sock->sendTo(messageBuf);
    }
    stream.close();
  } catch (CException &e) {
    stream.close();
    throw e;
  }
}

