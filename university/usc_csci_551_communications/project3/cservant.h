/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CSERVANT_H
#define CSERVANT_H

#include "ciservant.h"
#include "cmutexlocker.h"
#include "cwaitcondition.h"
#include "ctcpserverthread.h"
#include "ckbhandler.h"
#include "ceventdispatcher.h"
#include "ciniparser.h"
#include "clog.h"
#include "cneighbormap.h"
#include "cuoidmap.h"
#include "cjoindata.h"
#include "cstatuswriter.h"
#include "cfilemgr.h"

class CNode;
                                               
class CServant : public IServant, public ITcpListener
{
public:
  static CServant *getInstance();
  static void releaseInstance();
  virtual ~CServant();

  void start(bool reset, const string &iniFile, bool restarting);
  void handleSignal(int type);
  bool isRestartEnabled();

  //-----------------------------------------------------------------------------
  // IServant interface
  //-----------------------------------------------------------------------------
  CIniParser *getIniParser();
  CLog *getLog();
  CUoidMap *getUoidMap();
  bool isNodeJoining() const;
  string getNborFilePath() const;
  string getHostName() const;
  string getNodeId() const;
  string getNodeInstanceId() const;
  void stop();
  void setLastError(const string &s);
  void setSigMask(sigset_t *set, bool block);
  bool replaceNborNode(const string &curNodeId, const string &newNodeId);
  void addEvent(CEvent *e);
  void notifyResponse(const CMessage *m);
  bool handleUserInput(const string &input);
  UCharBuffer getNewUoid(const string &type) const;
  bool isMsgSource(uint8_t type, const UCharBuffer &reqUoid);
  void setCheckRequired();
  uint getDefTimeout();
  bool isPositiveProb(double v);
  CFileMgr *getFileMgr();
  
  //-----------------------------------------------------------------------------
  // ITcpServer Interface
  //-----------------------------------------------------------------------------
  void notifyNewConn(
    int newSocket, struct sockaddr_in &addr, sigset_t *sigMask);  

private:
  CServant();
  CServant(const CServant &other);
  CServant &operator=(const CServant &other);

  //-----------------------------------------------------------------------------
  // start and stop threads
  //-----------------------------------------------------------------------------
  void startThreadFromMain(CThread *t);
  template<typename T> void stopThread(T *t, const string &name);

  void startTcpServer();
  void stopTcpServer();
  void startKbHandler();
  void stopKbHandler();
  void startEventDispatcher();
  void stopEventDispatcher();
  void stopNeighborThreads();

  bool isStopFlagEnabled();
  
  //-----------------------------------------------------------------------------
  // support methods
  //-----------------------------------------------------------------------------
  void start_p();
  void setMainThreadSigMask(bool block);

  uint getCurTime();
  uint incrCurTime();
  void setMsgProcessing(bool flag);
  bool isMsgProcessingEnabled();
  void setUserInputTime(uint t);
  uint getUserInputTime();
  void setCheckRequired(bool flag);
  bool isCheckRequired();
  void setCheckInProgress(bool flag);
  bool isCheckInProgress();
  void setCheckTime(uint t);
  uint getCheckTime();
  void setStatusUoid(const UCharBuffer &uoid);
  UCharBuffer getStatusUoid();
  void setCheckUoid(const UCharBuffer &uoid);
  UCharBuffer getCheckUoid();
  void setSearchUoid(const UCharBuffer &uoid);
  UCharBuffer getSearchUoid();
  void setGetUoid(const UCharBuffer &uoid);
  UCharBuffer getGetUoid();

  void setStatusType(uint8_t t);
  uint8_t getStatusType();
  
  uint addSearchData(const CMetaData &m);
  CMetaData getSearchData(uint index);  
  void clearSearchData();
  void displaySearchResults(const MetaDataVector &mVec);
  void displayGetResult();
    
  void saveNodesToFile(const JoinDataList &list);
  NodeDataList getNodesFromFile();
  void errorIfInvalidSigCode(int code, const string &method) const;
  
  //-----------------------------------------------------------------------------
  // Timer and Timeout events
  //-----------------------------------------------------------------------------
  void timerTick(uint curTime);
  void timerBeaconRefresh(uint curTime);
  void timerUoidRefresh(uint curTime);
  void timerNodeJoin(uint curTime);
  void timerCheck(uint curTime);
  void timerUserInput(uint curTime);

  //-----------------------------------------------------------------------------
  // Initialization methods
  //-----------------------------------------------------------------------------
  void initRandom();
  void initSigHandler();
  void initNode();
  void initBeaconNode();
  void initRegularNode();
  void initJoin();
  void initParticipate();
  
  void connectToBeacons();
  void connectToBeacon(const CNodeData &data);
  CNode *getNewNode(const string &rmtHost, uint16_t rmtPort);

  //-----------------------------------------------------------------------------
  // User input methods
  //-----------------------------------------------------------------------------
  void displayOpInProgress();
  void completeUserRequest();
  void handleStatusInput(const string &input);
  void handleStoreInput(const string &input);
  void handleSearchInput(const string &input);
  bool handleGetInput(const string &input);
  void handleDeleteInput(const string &input);
  
  StringVector getStoreKwrdsFromInput(const string &s);
  string getSearchKwrdsFromInput(const string &s);

  //-----------------------------------------------------------------------------
  // Member variables
  //-----------------------------------------------------------------------------
  bool _stopFlag;
  bool _reset;
  bool _isNodeJoining;
  bool _msgProcessing;
  bool _restarting;
  bool _restartEnabled;
  bool _checkRequired;
  bool _checkInProgress;

  //times and timeouts
  uint _curTime;
  uint _beaconRfshTime;
  uint _joinTime;
  uint _userInputTime;
  uint _checkTime;

  //uoid's
  UCharBuffer _statusUoid;
  UCharBuffer _checkUoid;
  UCharBuffer _searchUoid;
  UCharBuffer _getUoid;

  uint8_t _statusType;
  string _iniFile;
  string _hostname;
  string _nodeid;
  string _nodeInstanceid;
  string _nborFile;
  string _lastError;
  sigset_t _signalMask;
  struct sigaction _sigAct;

  CLog _log;
  CIniParser _iniParser;
  CUoidMap _uoidMap;
  JoinDataList _joinList;
  CStatusWriter _statusWriter;
  CFileMgr _fileMgr;

  string _getFile;
  std::vector<CMetaData> _searchVec;

  //mutex
  CMutex _mutex;
  CWaitCondition _waitCond;
  CMutex _kbMutex;

  //threads
  CKbHandler _kbHandlerT;
  CTcpServerThread _tcpServerT;
  CNeighborMap _neighborMap;
  CEventDispatcher _eventDispatcherT;

  static uint START_TIME;
  static CServant *_instance;
};

#endif //CSERVANT_H
