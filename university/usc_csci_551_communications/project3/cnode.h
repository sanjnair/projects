/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CNODE_H
#define CNODE_H

#include "cglobal.h"
#include "ciservant.h"
#include "ctcpsocket.h"
#include "cthread.h"
#include "cwaitcondition.h"
#include "cmutex.h"
#include "cmessage.h"
#include "clog.h"

class CNodeSendThread : public CThread
{
public:
  CNodeSendThread();
  ~CNodeSendThread();

  void setTcpSock(CTcpSocket *sock);
  void setServantHandle(IServant *s);
  void addMessage(CMessage *message);
  void setSendNotify(const string &nodeid, uint8_t code);
  
  void stop();
  void run();

private:
  void cleanup();
  bool isStopFlagEnabled();
  CMessage* getMessage_p();
  void sendNewNotifyMsg();
  void sendMsgStore(CMessage *m, CTcpSocket *sock);
  void sendMsgGetRes(CMessage *m, CTcpSocket *sock);

  void sendMsg(
      const string &fPath,
      uint fSize,
      CMessage *m,
      CTcpSocket *sock);

  typedef std::list<CMessage*> MessageQueue;

  bool _stopFlag;
  bool _notifyFlag;
  uint8_t _notifyCode;
  string _nodeid;
  
  MessageQueue _messageQueue;
  CMutex _mutex;
  CWaitCondition _waitCond;
  CTcpSocket *_sock;
  IServant *_servant;
};


class CNode : public CThread
{
public:
  enum NodeType
  {
    NodeBeacon,
    NodeRegular,
    NodeJoin
  };

  CNode(
    const string &srcHost, uint16_t srcPort,
    const string &rmtHost, uint16_t rmtPort,
    bool connInitiated, CTcpSocket *sockP);
    
  ~CNode();

  void run();
  void stop();
  void setServantHandle(IServant *s);
  const CTcpSocket* getSocket() const;
  CTcpSocket* getSocket();
  bool isConnInitiated();
  bool isNeighbor();

  string getSrcHost();
  uint16_t getSrcPort();
  string getRmtHost();
  uint16_t getRmtPort();
  string getNodeId();

  void sendMessage(CMessage *m, CLog::MsgType t);
  void sendNewKeepAliveMsg();

  void timerTick(uint curTime);

  //---------------------------------------------------------------------------
  // Static Helper methods
  //---------------------------------------------------------------------------
  static string getNodeId(const string &name, uint16_t port);
  static string getNodeInstanceId(const string &nodeId);
  static string getHostName(const string &nodeId);
  static uint16_t getPort(const string &nodeId);

private:
  CNode(const CNode &rhs);
  CNode &operator =(const CNode &rhs);

  bool isStopFlagEnabled();
  void setSrcPort(uint16_t port);
  void setCurTime(uint curTime);
  uint getCurTime();
  void initConn();
  void initCheck();
  void cleanup();

  //---------------------------------------------------------------------------
  // Message handling helpers
  //---------------------------------------------------------------------------
  void handleMsg(CMessage *m);
  void handleHelloMsg(CMessage *m);
  void handleJnrqMsg(CMessage *m);
  void handleJnrsMsg(CMessage *m);
  void handleKeepAliveMsg(CMessage *m);
  void handleNotifyMsg(CMessage *m);
  void handleStatusReqMsg(CMessage *m);
  void handleStatusResMsg(CMessage *m);
  void handleCheckReqMsg(CMessage *m);
  void handleCheckResMsg(CMessage *m);
  void handleStoreMsg(CMessage *m);
  void handleSearchReqMsg(CMessage *m);
  void handleSearchResMsg(CMessage *m);  
  void handleGetReqMsg(CMessage *m);
  void handleGetResMsg(CMessage *m);
  void handleDeleteMsg(CMessage *m);
      
  //---------------------------------------------------------------------------
  // Message sending helpers
  //---------------------------------------------------------------------------
  void dispatchMsg(CMessage *m, bool probEnabled=false);
  void dispatchMsg(CMessage *m, const UCharBuffer &uoid);

  void sendNewHelloMsg();
  void sendNewJoinReqMsg();
  void sendNewJoinResMsg(CMessageJnrq *m);
  void sendNewStatusResMsg(CMessageStatusReq *m);
  void sendNewCheckResMsg(CMessageCheckReq *m);
  void sendNewSearchResMsg(CMessageSearchReq *m, const MetaDataVector &vct);
  void sendNewGetResMsg(CMessageGetReq *m, const UCharBuffer &fileId); 
      
  //---------------------------------------------------------------------------
  // Helper methods
  //---------------------------------------------------------------------------
  UCharBuffer getNewUoid() const;
  void setCheckRequired(bool flag);
  bool isCheckRequired();
  void setNotifyReceived(bool flag);
  bool isNotifyReceived();
  bool replaceNodeId(const string &host, uint16_t port);

  void setLastMsgRecvTime(uint t);
  uint getLastMsgRecvTime();
  void setKeepAliveTime(uint t);
  uint getKeepAliveTime();
      
  //---------------------------------------------------------------------------
  // Member variables
  //---------------------------------------------------------------------------
  bool _stopFlag;
  bool _helloRcvd;
  bool _helloSent;
  bool _connInitiated;
  bool _checkRequired;
  bool _notifyReceived;
  
  uint _curTime;
  uint16_t _srcPort;
  uint16_t _rmtPort;
  uint _keepAliveTime;
  uint _lastMsgRcvTime;

  NodeType _nodeType;
  IServant *_servant;
  string _srcHost;
  string _rmtHost;
  string _jnrqNodeId;
  CTcpSocket *_socketP;
  CMutex _mutex;
  CNodeSendThread _nodeSendThread;

  static const string NODEID_SEP;

};

#endif //CNODE_H

