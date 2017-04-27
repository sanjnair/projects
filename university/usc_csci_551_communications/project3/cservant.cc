/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <time.h>
#include <unistd.h>
#include "cservant.h"
#include "ctcpsocket.h"
#include "cconst.h"
#include "cutil.h"
#include "ckbhandler.h"
#include "cnode.h"
#include "cautoptr.h"
#include "cfile.h"
#include "cofstream.h"
#include "cifstream.h"
#include "cutil.h"
#include "ccert.h"
#include "csha1data.h"
#include "cfilespec.h"

//-----------------------------------------------------------------------------
/*!
  C Signal handlers
*/
void c_sighandler(int t)
{
  CServant::getInstance()->handleSignal(t);
}
//-----------------------------------------------------------------------------

/*!
  Gets the singleton instance
*/
CServant* CServant::getInstance()
{
  if (NULL == _instance) {
    _instance = new CServant(); assert(_instance);
  }
  return _instance;
}

/*!
  Releases the singleton instance
*/
void CServant::releaseInstance()
{
  if (NULL != _instance) {
    delete _instance;
    _instance = NULL;
  }
}

/*!
  Constructs the Servant
*/
CServant::CServant()
{
  _stopFlag = false;
  _reset = false;
  _isNodeJoining = false;
  _msgProcessing = false;
  _restartEnabled = false;
  _checkRequired = false;
  _checkInProgress = false;

  _statusType = 0;
  
  _curTime = START_TIME;
  _beaconRfshTime = 0;
  _joinTime = 0;
  _userInputTime = 0;
  _checkTime = 0;
}

/*!
  Deletes the Servant
*/
CServant::~CServant()
{
}

CServant::CServant(const CServant &other) {}
CServant& CServant::operator=(const CServant &other)
{
  return *this;
}
//-----------------------------------------------------------------------------

/*!
  Starts the servant
*/
void CServant::start(
  bool reset, const string &iniFile, bool restarting)
{
  _reset = reset;
  _iniFile = iniFile;
  _restarting = restarting;

  alarm(0);
  start_p();
  CUtil::writeOutput("\r\n");
}

/*!
  \internal
  Starst the servant
*/
void CServant::start_p()
{
  _hostname = CTcpSocket::getHostName();
  _iniParser.parse(_iniFile, _hostname);
  _nborFile = _iniParser.getHomePath(CConst::INIT_NBORFILE);

  _nodeid = CNode::getNodeId(_hostname, _iniParser.getPort());
  _nodeInstanceid = CNode::getNodeInstanceId(_nodeid);

  if ((! _iniParser.isBeaconNode()) && _reset) {
    if (CFile::exists(_nborFile)) {
      CFile::deleteFile(_nborFile);
    }
  }

  string path = _iniParser.getHomePath(_iniParser.getLogFile());
  _log.open(path, _restarting);

  if (_reset) {
    CCert::reset(_iniParser.getHomeDir());
    _fileMgr.reset(this, &_iniParser, &_log);
  }

  if (! CCert::isCertExist(_iniParser.getHomeDir())) {
    CUtil::writeOutputLn("Generating certificate...");
    CCert::genCert(_iniParser.getHomeDir());
  }
  _fileMgr.initialize(this, &_iniParser, &_log);

  initRandom();
  initSigHandler();
  initNode();
}

/*!
  Initializes the random generator
*/
void CServant::initRandom()
{
  time_t localtime=(time_t)0;
  time(&localtime);
  srand48((long)localtime);
}

/*!
  Initializes the signal handler
*/
void CServant::initSigHandler()
{
  int ret = sigemptyset(&_signalMask);
  errorIfInvalidSigCode(ret, "initSigHandler::sigemptyset");
  ret = sigfillset(&_signalMask);
  errorIfInvalidSigCode(ret, "initSigHandler::sigfillset");
  setSigMask(&_signalMask, true);
    
  ret = sigemptyset(&(_sigAct.sa_mask));
  errorIfInvalidSigCode(ret, "initSigHandler::sigemptyset(sigact)");
  _sigAct.sa_flags = 0;
  _sigAct.sa_handler = c_sighandler;

  sigaction(SIGINT,  &_sigAct, NULL);
  sigaction(SIGALRM, &_sigAct, NULL);
  sigaction(SIGPIPE, &_sigAct, NULL);
  sigaction(SIGUSR1, &_sigAct, NULL);
  sigaction(SIGUSR2, &_sigAct, NULL);

  setMainThreadSigMask(true);
}

/*!
  Handles the signals
*/
void CServant::handleSignal(int type)
{
  if (SIGINT == type) {
      completeUserRequest();
      
  } else if(SIGALRM == type) {
    timerTick(incrCurTime());
    
  } else {
    string msg("signal received = ");
    msg += CUtil::getString(type);
    _log.logDebug(msg);
  }
}

/*!
  Sets the signal mask. Main thread only handles the
  signal from within.
  \WARNING. As the name suggests, only call this method
  from main thread.
*/
void CServant::setMainThreadSigMask(bool block)
{
  int ret = 0;
  if (! block) {
    //remove SIGUSR1 and SIGUSR2 from the mask
    ret  = sigdelset(&_signalMask, SIGUSR1);
    errorIfInvalidSigCode(ret, "setSigMask::sigdelset");
    ret  = sigdelset(&_signalMask, SIGUSR2);
    errorIfInvalidSigCode(ret, "setSigMask::sigdelset");
  }

  setSigMask(&_signalMask, block);

  if (! block) {
    //remove SIGUSR1 and SIGUSR2 from the mask
    ret  = sigaddset(&_signalMask, SIGUSR1);
    errorIfInvalidSigCode(ret, "setSigMask::sigaddset");
    ret  = sigaddset(&_signalMask, SIGUSR2);
    errorIfInvalidSigCode(ret, "setSigMask::sigaddset");
  }
}

/*!
  Sets the signal mask for the given signal set
*/
void CServant::setSigMask(sigset_t *set, bool block)
{
  int rc = 0;
  if (block) {
    rc = pthread_sigmask(SIG_BLOCK, set, NULL);
  } else {
    rc = pthread_sigmask(SIG_UNBLOCK, set, NULL);
  }

  errorIfInvalidSigCode(rc, "setSigMask::pthread_sigmask");
}

/*!
  Throws exception if the signal error code is invalid
*/
void CServant::errorIfInvalidSigCode(
  int code, const string &method) const
{
  if (code < 0) {
    string msg = string("Call to ") + method;
    msg += string(" returned error. code = ") + CUtil::getString(code);
    msg += string(", errno = ") + CUtil::getString(errno);
  }
}

//-----------------------------------------------------------------------------
// start and stop threads
//-----------------------------------------------------------------------------

/*!
  Starts a thread by masking all required signals.
  \WARNING...
  This method MUST be called when the calling thread is the main
  thread. Otherwise the signal mask for the calling thread
  will be mixed up with the main thread signal mask.
*/
void CServant::startThreadFromMain(CThread *t)
{
  setMainThreadSigMask(true);
  t->start();
  setMainThreadSigMask(false);
}

/*!
  Stops the thread and wait for the thread to join.
  Also, logs the message in to the log file.
*/
template<typename T>
void CServant::stopThread(T *t, const string &name)
{
  _log.logDebug(string("Stopping ") + name);
  t->stop();
  _log.logDebug(string("Waiting for ") + name + string(" to join"));
  t->join();
  _log.logDebug(name + string(" stopped"));
}

/*!
  Starts the TCP server
*/
void CServant::startTcpServer()
{
  _tcpServerT.setServantHandle(this);
  _tcpServerT.setTcpListener(this);
  startThreadFromMain(&_tcpServerT);
}

/*!
  Stops the TCP server
*/
void CServant::stopTcpServer()
{
  stopThread(&_tcpServerT, "Tcp Server");
}

/*!
  Starts the Keyboard handler
*/
void CServant::startKbHandler()
{
  _kbHandlerT.setServantHandle(this);
  startThreadFromMain(&_kbHandlerT);
}

/*!
  Stops the Keyboard handler
*/
void CServant::stopKbHandler()
{
  stopThread(&_kbHandlerT, "Keyboard Handler");
}

/*!
  Starts the Event Dispatcher
*/
void CServant::startEventDispatcher()
{
  _eventDispatcherT.setServantHandle(this);
  _eventDispatcherT.setNeighborMap(&_neighborMap);
  startThreadFromMain(&_eventDispatcherT);
}

/*!
  Stops the Event Dispatcher
*/
void CServant::stopEventDispatcher()
{
  stopThread(&_eventDispatcherT, "Event Dispatcher");
}

/*!
  Stops all neighbor threads
*/
void CServant::stopNeighborThreads()
{
  //this will waits for all threads in the map to finish
  //and join the calling thread
  _log.logDebug("Stopping Neighbors");
  _neighborMap.clear();
  _log.logDebug("Neighbors stopped");
}

/*!
  Stops all neighbor threads
*/
bool CServant::isStopFlagEnabled()
{
  CMutexLocker locker(&_mutex);
  return _stopFlag;
}

//-----------------------------------------------------------------------------
//IServant interface
//-----------------------------------------------------------------------------

/*!
  Returns pointer to ini parser
*/
CIniParser* CServant::getIniParser()
{
  return &_iniParser;
}

/*!
  Returns pointer to log
*/
CLog* CServant::getLog()
{
  return &_log;
}

/*!
  Returns pointer to Uoid Map
*/
CUoidMap* CServant::getUoidMap()
{
  return &_uoidMap;
}

/*!
  Returns true if the node is the joining node
*/
bool CServant::isNodeJoining() const
{
  return _isNodeJoining;
}

/*!
  Returns the neighbor file path
*/
string CServant::getNborFilePath() const
{
  return _nborFile;
}

/*!
  Returns the hostname
*/
string CServant::getHostName() const
{
  return _hostname;
}

/*!
  Returns the node id
*/
string CServant::getNodeId() const
{
  return _nodeid;
}

/*!
  Replaces the neighbor node if it can be replaced.
  If successfully replaced with newNode id, this method
  will return true.
*/
bool CServant::replaceNborNode(
  const string &curNodeId,
  const string &newNodeId)
{
  return _neighborMap.replace(curNodeId, newNodeId);
}

/*!
  Returns the node instance id
*/
string CServant::getNodeInstanceId() const
{
  return _nodeInstanceid;
}

/*!
  Stops the servant.
*/
void CServant::stop()
{
  alarm(0);

  if (! isStopFlagEnabled()) {
    _log.logDebug("Shutting down the node");
    {
      CMutexLocker m(&_mutex);
      _stopFlag = true;
      _waitCond.notify();
    }
  }
}

/*!
  Sets the last error
*/
void CServant::setLastError(const string &s)
{
  CMutexLocker m(&_mutex);
  _lastError = s;
}

/*!
  Adds an event to the event queue
*/
void CServant::addEvent(CEvent *e)
{
  if (! _eventDispatcherT.isRunning()) {
    CAutoPtr<CEvent>(e);
    C_THROW("Event dispatcher is not running");
  }
  _eventDispatcherT.addEvent(e);
}

/*!
  Nodes sends the notify messages.
  This method processes them accordingly
*/
void CServant::notifyResponse(const CMessage *m)
{
  if (CConst::TYPE_JOIN_RES == m->getType()) {
    if (isMsgProcessingEnabled()) {
      const CMessageJnrs *mj = dynamic_cast<const CMessageJnrs *>(m);
      CJoinData data(mj->getHostName(), mj->getPort(), mj->getDistance());
      _joinList.push_back(data);

    }
    
  } else if(CConst::TYPE_CHECK_RES == m->getType()) {
    if (isCheckInProgress()) {
      setCheckRequired(false);
      setCheckInProgress(false);
      setCheckTime(0);
    }
    
  } else if (CConst::TYPE_STATUS_RES == m->getType()) {
    uint t = getUserInputTime();
    if (t > 0) {

      if (CConst::STATUS_NEIGHBORS == getStatusType()) {
        const CMessageStatusRes *ms = dynamic_cast<const CMessageStatusRes *>(m);
        _statusWriter.writeNode(ms->getHostName(), ms->getPort(), ms->getNeighborList());
        
      } else if (CConst::STATUS_FILES == getStatusType()) {
        const CMessageStatusRes *ms = dynamic_cast<const CMessageStatusRes *>(m);
        _statusWriter.writeMetaData(ms->getHostName(), ms->getPort(), ms->getMetaDataVec());
        
      } else {
        string msg = "Invalid status response type ";
        msg += CUtil::getString(getStatusType());
        _log.logError(msg);
      }
    }
    
  } else if (CConst::TYPE_SEARCH_RES == m->getType()) {
    uint t = getUserInputTime();
    if (t > 0) {
      const CMessageSearchRes *ms = dynamic_cast<const CMessageSearchRes *>(m);
      displaySearchResults(ms->getMetaDataVec());
    }
    
  } else if (CConst::TYPE_GET_RES == m->getType()) {
    uint t = getUserInputTime();
    if (t > 0) {
      const CMessageGetRes *ms = dynamic_cast<const CMessageGetRes *>(m);
      try {
        CFileMgr::copyFile(ms->getFilePath(), _getFile);
        displayGetResult();

        _fileMgr.storeFile(ms);
        
      } catch (CException &e) {
        _log.logError(e.toString());
      }

      completeUserRequest();
    }
  }
}

/*!
  Handles the user input. Returns false if the operation
  requires to wait.
*/
bool CServant::handleUserInput(const string &input)
{
  bool completed = true;

  if (CUtil::startsWith(input, CConst::CMD_STATUS, true)) {
    clearSearchData();
    handleStatusInput(input);
    completed = false;

  } else if(CUtil::startsWith(input, CConst::CMD_STORE, true)) {
    clearSearchData();
    handleStoreInput(input);

  } else if(CUtil::startsWith(input, CConst::CMD_SEARCH, true)) {
    clearSearchData();
    handleSearchInput(input);
    completed = false;

  } else if(CUtil::startsWith(input, CConst::CMD_GET, true)) {
    completed = handleGetInput(input);

  } else if(CUtil::startsWith(input, CConst::CMD_DELETE, true)) {
    handleDeleteInput(input);
    completed = true;
    
  } else {
    string msg = string("Invalid option ") + input;
    C_THROW(msg);
  }

  if (! completed) {
    setUserInputTime(getCurTime());
  }

  return completed;
}

/*!
  Returns the new Uoid for the object type
*/
UCharBuffer CServant::getNewUoid(const string &type) const
{
  return CMessage::getNewUoid(getNodeInstanceId(), type);
}

/*!
  Returns true if the request id is originated from
  this node
*/
bool CServant::isMsgSource(
  uint8_t type, const UCharBuffer &reqUoid)
{
  bool flag = false;
  if (type == CConst::TYPE_STATUS_RES) {
    flag = (reqUoid == getStatusUoid());

  } else if (type == CConst::TYPE_CHECK_RES) {
    flag = (reqUoid == getCheckUoid());
    
  } else if (type == CConst::TYPE_SEARCH_RES) {
    flag = (reqUoid == getSearchUoid());
    
  } else if (type == CConst::TYPE_GET_RES) {
    flag = (reqUoid == getGetUoid());
    
  } 

  return flag;
}

/*!
  Sets the check required flag to true
*/
void CServant::setCheckRequired()
{
  if ((! _iniParser.isBeaconNode()) &&
      (0 == _iniParser.getNoCheck())) {

    _log.logDebug("Check initiated...");
    setCheckRequired(true);
  }
}

/*!
  Returns the default timeout.
  This should be used for operations where the default timeout
  is not specified.
*/
uint CServant::getDefTimeout()
{
  uint t = static_cast<uint>(_iniParser.getMsgLifeTime() * 1.2);
  return t;
}

/*!
  Returns true if the probability is greater than v
*/
bool CServant::isPositiveProb(double v)
{
  double r = drand48();
  if ( r < v) {
    return true;
  }
  return false;
}

/*!
  Returns the pointer to the file manager
*/
CFileMgr* CServant::getFileMgr()
{
  return &_fileMgr;
}

//-----------------------------------------------------------------------------
// support methods
//-----------------------------------------------------------------------------
/*!
  Returns the current time
  \warning Mutex protcted
*/
uint CServant::getCurTime() {
  CMutexLocker locker(&_mutex);
  return _curTime;
}

/*!
  Increments the current time
  \warning Mutex protcted
*/
uint CServant::incrCurTime() {
  CMutexLocker locker(&_mutex);
  _curTime++;
  return _curTime;
}

/*!
  Returns true if the restart flag is enabled
*/
bool CServant::isRestartEnabled()
{
  return _restartEnabled;
}

/*!
  Sets the message processing flag enabled
  WARNING - MUTEX LOCKED
*/
void CServant::setMsgProcessing(bool flag)
{
  CMutexLocker m(&_mutex);
  _msgProcessing = flag;
}

/*!
  Returns true if message processing flag enabled
  WARNING - MUTEX LOCKED
*/
bool CServant::isMsgProcessingEnabled()
{
  CMutexLocker m(&_mutex);
  return _msgProcessing;
}

/*!
  Sets the user input time
  WARNING - MUTEX LOCKED
*/
void CServant::setUserInputTime(uint t)
{
  CMutexLocker m(&_mutex);
  _userInputTime = t;
}

/*!
  Returns user input time
  WARNING - MUTEX LOCKED
*/
uint CServant::getUserInputTime()
{
  CMutexLocker m(&_mutex);
  return _userInputTime;
}

/*!
  Sets the CheckRequired flag
  WARNING - MUTEX LOCKED
*/
void CServant::setCheckRequired(bool flag)
{
  CMutexLocker m(&_mutex);
  _checkRequired = flag;
}

/*!
  Returns the CheckRequired flag
  WARNING - MUTEX LOCKED
*/
bool CServant::isCheckRequired()
{
  CMutexLocker m(&_mutex);
  return _checkRequired;
}

/*!
  Sets the CheckInProgress flag
  WARNING - MUTEX LOCKED
*/
void CServant::setCheckInProgress(bool flag)
{
  CMutexLocker m(&_mutex);
  _checkInProgress = flag;
}

/*!
  Returns the CheckInProgress flag
  WARNING - MUTEX LOCKED
*/
bool CServant::isCheckInProgress()
{
  CMutexLocker m(&_mutex);
  return _checkInProgress;
}

/*!
  Sets the check time
  WARNING - MUTEX LOCKED
*/
void CServant::setCheckTime(uint t)
{
  CMutexLocker m(&_mutex);
  _checkTime = t;
}

/*!
  Returns check time
  WARNING - MUTEX LOCKED
*/
uint CServant::getCheckTime()
{
  CMutexLocker m(&_mutex);
  return _checkTime;
}

/*!
  Sets the status uoid
  WARNING - MUTEX LOCKED
*/
void CServant::setStatusUoid(const UCharBuffer &uoid)
{
  CMutexLocker m(&_mutex);
  _statusUoid = uoid;
}

/*!
  Returns status uoid
  WARNING - MUTEX LOCKED
*/
UCharBuffer CServant::getStatusUoid()
{
  CMutexLocker m(&_mutex);
  return _statusUoid;
}

/*!
  Sets the check uoid
  WARNING - MUTEX LOCKED
*/
void CServant::setCheckUoid(const UCharBuffer &uoid)
{
  CMutexLocker m(&_mutex);
  _checkUoid = uoid;
}

/*!
  Returns check uoid
  WARNING - MUTEX LOCKED
*/
UCharBuffer CServant::getCheckUoid()
{
  CMutexLocker m(&_mutex);
  return _checkUoid;
}

/*!
  Sets the search uoid
  WARNING - MUTEX LOCKED
*/
void CServant::setSearchUoid(const UCharBuffer &uoid)
{
  CMutexLocker m(&_mutex);
  _searchUoid = uoid;
}

/*!
  Returns search uoid
  WARNING - MUTEX LOCKED
*/
UCharBuffer CServant::getSearchUoid()
{
  CMutexLocker m(&_mutex);
  return _searchUoid;  
}

/*!
  Sets the get uoid
  WARNING - MUTEX LOCKED
*/
void CServant::setGetUoid(const UCharBuffer &uoid)
{
  CMutexLocker m(&_mutex);
  _getUoid = uoid;
}

/*!
  Returns the get uoid
  WARNING - MUTEX LOCKED
*/
UCharBuffer CServant::getGetUoid()
{
  CMutexLocker m(&_mutex);
  return _getUoid;
}

/*!
  Sets the status Type
  WARNING - MUTEX LOCKED
*/
void CServant::setStatusType(uint8_t t)
{
  CMutexLocker m(&_mutex);
  _statusType = t;
}

/*!
  Sets the status Type
  WARNING - MUTEX LOCKED
*/
uint8_t CServant::getStatusType()
{
  CMutexLocker m(&_mutex);
  return _statusType;
}
 
/*!
  Adds the search data and returns the fileid index
*/
uint CServant::addSearchData(const CMetaData &m)
{
  assert(m.getFileId().size() > 0);
  
  _searchVec.push_back(m);
  return _searchVec.size();
}

/*!
  Returns the search fileid for the given index
*/
CMetaData CServant::getSearchData(uint index)
{
  CMutexLocker m(&_mutex);

  if (0 == index) {
    C_THROW("Invalid index 0");
  }

  uint idIndex = index-1;

  if (idIndex >= _searchVec.size()) {
    string msg = "No file id found for index ";
    msg += CUtil::getString(index);
    C_THROW(msg);
  }

  return _searchVec.at(idIndex);
}


/*!
  Clears the search data
*/
void CServant::clearSearchData()
{
  CMutexLocker m(&_mutex);
  _searchVec.clear();
}

/*
  Displays the search results
  \WARNING - Mutex protected
*/
void CServant::displaySearchResults(
  const MetaDataVector &mVec)
{
  CMutexLocker locker(&_mutex);
  
  string s;

  if (mVec.size() > 0) {
    for (uint i=0; i<mVec.size(); i++) {
      const CMetaData &m = mVec.at(i);
      uint fileIndex = addSearchData(m);

      s += string("[") + CUtil::getString(fileIndex) + string("]") + CConst::SPACE_SEP;
      s += string("\tFileID=") + CUtil::getHexStr(m.getFileId()) + CConst::CRLF;
      s += string("\tFileName=") + m.getFileName() + CConst::CRLF;
      s += string("\tFileSize=") + CUtil::getString(m.getFileSize()) + CConst::CRLF;
      s += string("\tSHA1=") + CUtil::getHexStr(m.getSha1()) + CConst::CRLF;
      s += string("\tNonce=") + CUtil::getHexStr(m.getNonce()) + CConst::CRLF;
      s += string("\tKeywords=");

      StringVector v = m.getKeywords();
      if (v.size() > 0) {
        for (uint j=0; j<v.size(); j++) {
          s +=v.at(j) + CConst::SPACE_SEP;
        }
      }
      s += CConst::CRLF;
    }

    CUtil::writeOutputLn(s);
  }
}

/*!
  Display the search results
*/
void CServant::displayGetResult()
{
  string s = string("Transfer of file  ") + _getFile;
  s += " completed successfully.";
  CUtil::writeOutputLn(s);  
}

//-----------------------------------------------------------------------------

/*!
  Saves the join data to file
*/
void CServant::saveNodesToFile(const JoinDataList &list)
{
  uint initSize = _iniParser.getInitNeighbors();
  assert(list.size() >= initSize);
  uint count = 0;

  CFile file(_iniParser.getHomePath(CConst::INIT_NBORFILE));
  COfStream ostream(&file);
  ostream.open();

  JoinDataList::const_iterator it;
  try {
    for (it = list.begin(); it != list.end(); it++) {
      const CJoinData &jn = *it;
      string s = jn.toString() + "\n";
      ostream.write(s);

      count++;
      if (count >= initSize) {
        break;
      }
    }
    ostream.close();

  } catch (CException &e) {
    ostream.close();
    throw e;
  }
}

/*!
  Returns the node data from the file
*/
NodeDataList CServant::getNodesFromFile()
{
  string path = _iniParser.getHomePath(CConst::INIT_NBORFILE);
  CFile::errorIfNotExist(path);

  CFile file(path);
  CIfStream stream(&file, false);
  stream.open();

  string line;
  string host;
  string value;
  uint16_t port;
  NodeDataList list;

  try {
    while (stream.isGood() && (! stream.isEof())) {
      line = CUtil::trim(stream.readLine());

      if (line.length() > 0) {
        CUtil::getNvp(line, CConst::SEP_NODE, host, value);
        port = CUtil::getUInteger<uint16_t>(value);
        CNodeData data(host, port);
        list.push_back(data);
      }
    }
  } catch (CException &e) {
    stream.close();
    throw e;
  }

  return list;
}

//-----------------------------------------------------------------------------
// Timer and Timeout events
//-----------------------------------------------------------------------------

/*!
  Time ticks - ticks in every second
*/
void CServant::timerTick(uint curTime)
{
  //no need to protect the members with mutex as only the main thread
  //accesses them.
  try {
    if (CUtil::isElapsed(
          START_TIME, curTime, _iniParser.getAutoShutdown())) {

      CUtil::writeOutputLn("\nAuto Shutdown...");
      stop();

    } else {
      if (! isStopFlagEnabled()) {
        timerBeaconRefresh(curTime);
      }

      if (! isStopFlagEnabled()) {
        timerUoidRefresh(curTime);
      }

      if (! isStopFlagEnabled()) {
        timerNodeJoin(curTime);
      }

      if (! isStopFlagEnabled()) {
        timerCheck(curTime);
      }

      if (! isStopFlagEnabled()) {
        timerUserInput(curTime);
      }
      //Calls timer tick on all nodes.
      if (! isStopFlagEnabled()) {
        _neighborMap.timerTick(curTime);
      }

      if (! isStopFlagEnabled()) {
        alarm(1);
      }
    }
  } catch (CException &e) {
    setLastError(e.getMessage());
    stop();
  }
}

/*!
  timer for beacon refresh
*/
void CServant::timerBeaconRefresh(uint curTime)
{
  if (_iniParser.isBeaconNode()) {
    if ((_beaconRfshTime > 0) &&
      CUtil::isElapsed(_beaconRfshTime, curTime, _iniParser.getBeaconRetry())) {
      connectToBeacons();
    }
  }
}

/*!
  timer for UOID refresh
*/
void CServant::timerUoidRefresh(uint curTime)
{
  if (CUtil::isElapsed(
        START_TIME, curTime, _iniParser.getMsgLifeTime())) {

    _uoidMap.cleanupExpired(curTime, _iniParser.getMsgLifeTime());
  }
}

/*!
  timer for beacon refresh
*/
void CServant::timerNodeJoin(uint curTime)
{
  if (isNodeJoining()) {
    if ((_joinTime > 0) &&
         CUtil::isElapsed(_joinTime, curTime, _iniParser.getJoinTimeout())) {

      setMsgProcessing(false);
      if (_joinList.size() < _iniParser.getInitNeighbors()) {
        string msg("Not enough nodes responded to join request (responded = ");
        msg += CUtil::getString(_joinList.size()) + string(" ,expected = ");
        msg += CUtil::getString(_iniParser.getInitNeighbors()) + string(").");

        setLastError(msg);

      } else {
        _restartEnabled = true;
        _joinList.sort();
        saveNodesToFile(_joinList);

        CUtil::writeOutputLn("Join operation completed.");
      }
      stop();
    }
  }
}

/*!
  Timer for handling check events
*/
void CServant::timerCheck(uint curTime)
{
  if ((! _iniParser.isBeaconNode()) &&
      (0 == _iniParser.getNoCheck()) &&
      isCheckRequired()) {

    if (_checkInProgress) {
      if (CUtil::isElapsed(getCheckTime(), curTime, getDefTimeout())) {
        setLastError("Check failed. No Beacons responded.");
        _restartEnabled = true;
        stop();
      }
    } else {
      _log.logDebug("Initiating check...");
      setCheckUoid(getNewUoid(CConst::OBJ_TYPE_MSG));

      //ttl + 1 because the dispatcher decrements it
      CMessageCheckReq *mP = new CMessageCheckReq(
                                getCheckUoid(),
                                _iniParser.getTtl() + 1);

      CMsgEvent *eP = new CMsgEvent(mP, "", CLog::MsgSent);
      _eventDispatcherT.addEvent(eP);

      setCheckInProgress(true);
      setCheckTime(getCurTime());
    }
  }
}

/*!
  timer for beacon refresh
*/
void CServant::timerUserInput(uint curTime)
{
  uint t = getUserInputTime();
  if (t > 0) {
    if (CUtil::isElapsed(t, curTime, getDefTimeout())) {
      completeUserRequest();
    }
  }
}

//-----------------------------------------------------------------------------
// User input methods
//-----------------------------------------------------------------------------

/*!
  Display the operation in progress message
*/
void CServant::displayOpInProgress()
{
  string msg("Performing the requested operation. Press Ctrl-C to cancel...");
  _kbHandlerT.outputStr(msg, true);
}

/*!
  Complete the user request and give prompt
  back to user.
*/
void CServant::completeUserRequest() {
  if (getUserInputTime() > 0) {
    setUserInputTime(0);

    try {
      _statusWriter.clear();
    } catch (CException &e) {
      _log.logError(e.toString());
    }
  }

  if (_kbHandlerT.isRunning()) {
    _kbHandlerT.opInterrupted();
  }
}

void CServant::handleStatusInput(const string &input)
{
  StringVector vct = CUtil::split(input, CConst::SPACE_SEP);

  if (4 != vct.size()) {
    string msg("Invalid status command.");
    C_THROW(msg);
  }
  string option = vct.at(1);
  uint ttl = CUtil::getInteger<uint>(vct.at(2), true);
  string filePath = vct.at(3);

  if (CConst::VAL_NEIGHBORS == option) {
    _statusWriter.initialize(filePath, true);
    setStatusType(CConst::STATUS_NEIGHBORS);
    
    //insert current servant's neighbor first
    StringList nList = _neighborMap.getNeighborList();
    if (nList.size() > 0) {
      string nodeId;
      NodeDataList nodeList;

      StringList::const_iterator it;
      for (it = nList.begin(); it != nList.end(); it++) {
        nodeId = *it;
        CNodeData d(CNode::getHostName(nodeId), CNode::getPort(nodeId));
        nodeList.push_back(d);
      }
      _statusWriter.writeNode(getHostName(), _iniParser.getPort(), nodeList);
    }

    setStatusUoid(getNewUoid(CConst::OBJ_TYPE_MSG));
    //ttl + 1 because the dispatcher decrements it
    CMessageStatusReq *mP = new CMessageStatusReq(
                              getStatusUoid(), ttl + 1,
                              CConst::STATUS_NEIGHBORS);

    CMsgEvent *eP = new CMsgEvent(mP, "", CLog::MsgSent);
    _eventDispatcherT.addEvent(eP);

  } else if (CConst::VAL_FILES == option) {
    _statusWriter.initialize(filePath, false);
    setStatusType(CConst::STATUS_FILES);
    
    MetaDataVector vctM = _fileMgr.getAllMetaInfo();
    _statusWriter.writeMetaData(getHostName(), _iniParser.getPort(), vctM);

    setStatusUoid(getNewUoid(CConst::OBJ_TYPE_MSG));
    getUoidMap()->add(getStatusUoid(), getNodeId(), getCurTime());
    
    //ttl + 1 because the dispatcher decrements it
    CMessageStatusReq *mP = new CMessageStatusReq(
                              getStatusUoid(), ttl + 1,
                              CConst::STATUS_FILES);
        
    CMsgEvent *eP = new CMsgEvent(mP, "", CLog::MsgSent);
    _eventDispatcherT.addEvent(eP);

  } else {
    string msg("Invalid status command. Invalid option");
    msg += option;
    C_THROW(msg);
  }

  displayOpInProgress();
}

/*!
  Handles the store input request
*/
void CServant::handleStoreInput(const string &input)
{
  StringVector vct = CUtil::split(input, CConst::SPACE_SEP);
  uint size = vct.size();
  const uint MIN_SIZE = 3;

  if (size < MIN_SIZE) {
    string msg("Invalid store command. Atleast 3 args expected");
    C_THROW(msg);
  }

  string fPath = vct.at(1);
  uint ttl = CUtil::getUInteger<uint>(vct.at(2));
  StringVector kwrdVct;

  if (size > MIN_SIZE) {
    string s = vct.at(2) + CConst::SPACE_SEP;
    uint pos = input.find(s, 0);

    if (pos != string::npos) {
      s = input.substr(pos + s.length(), input.length());
      kwrdVct = getStoreKwrdsFromInput(s);
    }
  }

  UCharBuffer uoid = getNewUoid(CConst::OBJ_TYPE_MSG);
  CAutoPtr<CMessageStore> mP(new CMessageStore(uoid, ttl+1));

  _fileMgr.storeFile(
      fPath,
      kwrdVct,
      getNewUoid(CConst::OBJ_TYPE_FILE),
      mP.get());

  if (ttl > 0) {
    CMsgEvent *eP = new CMsgEvent(mP.release(), "", CLog::MsgSent);
    eP->setProbEnabled(true);

    getUoidMap()->add(uoid, getNodeId(), getCurTime());
    _eventDispatcherT.addEvent(eP);
  }
}

/*!
  Handles the search input
*/
void CServant::handleSearchInput(const string &input)
{
  StringVector vct = CUtil::split(input, CConst::SPACE_SEP);
  uint size = vct.size();
  const uint MIN_SIZE = 2;

  if (size < MIN_SIZE) {
    string msg("Invalid search command. Atleast 2 args expected");
    C_THROW(msg);
  }
  string name;
  string val;
  CUtil::getNvp(vct.at(1), CConst::NVP_SEP, name, val);

  string query;
  uint8_t searchType = 0;

  if (CConst::OPTION_SEARCH_FILENAME == name) {
    query = val;
    searchType = CConst::SEARCH_FILENAME;

  } else if (CConst::OPTION_SEARCH_SHA1 == name) {
    query = val;
    searchType = CConst::SEARCH_SHA1;

  } else if (CConst::OPTION_SEARCH_KWRDS == name) {
    query = getSearchKwrdsFromInput(input);
    searchType = CConst::SEARCH_KWRDS;

  } else {
    string msg = string("Invalid option ") + name;
    C_THROW(msg);
  }

  MetaDataVector vctMeta = _fileMgr.search(searchType, query);
  displayOpInProgress();

  if (vctMeta.size() > 0) {
    displaySearchResults(vctMeta);
  }

  setSearchUoid(getNewUoid(CConst::OBJ_TYPE_MSG));
  getUoidMap()->add(getSearchUoid(), getNodeId(), getCurTime());

  uint ttl = _iniParser.getTtl() + 1;
  CAutoPtr<CMessageSearchReq> mP;
  mP = new CMessageSearchReq(getSearchUoid(), ttl, searchType, query);
  CMsgEvent *eP = new CMsgEvent(mP.release(), "", CLog::MsgSent);
  _eventDispatcherT.addEvent(eP);
}

/*!
  Handles the get input
*/
bool CServant::handleGetInput(const string &input)
{
  bool completed = false;
  
  StringVector vct = CUtil::split(input, CConst::SPACE_SEP);
  const uint MIN_SIZE = 2;

  if (vct.size() < MIN_SIZE) {
    string msg("Invalid get command. Atleast 2 args expected");
    C_THROW(msg);
  }

  uint index = CUtil::getUInteger<uint>(vct.at(1));
  CMetaData mData = getSearchData(index);
  _getFile = (vct.size() > MIN_SIZE) ? vct.at(2)
                                     : mData.getFileName();

  bool proceed = true;

  if (CFile::exists(_getFile)) {
    string msg = string("File ") + _getFile;
    msg += " exists. Do you want to replace? (y/n): ";
    _kbHandlerT.outputStr(msg);
    string input = CUtil::trim(_kbHandlerT.getUserInput());

    if (! CUtil::isEqual(input, "y", false)) {
      proceed = false;
    }
  }

  if (proceed) {
    if (_fileMgr.isFileIdPresent(mData.getFileId())) {
      _fileMgr.getLocalFile(mData.getFileId(), _getFile);
      displayGetResult();
      completed = true;
      
    } else {
      setGetUoid(getNewUoid(CConst::OBJ_TYPE_MSG));
      getUoidMap()->add(getGetUoid(), getNodeId(), getCurTime());
      uint ttl = _iniParser.getTtl() + 1;
      CAutoPtr<CMessageGetReq> mP;
      mP = new CMessageGetReq(getGetUoid(), ttl, mData.getFileId());
      CMsgEvent *eP = new CMsgEvent(mP.release(), "", CLog::MsgSent);
      _eventDispatcherT.addEvent(eP);
    }
  } else {
    completed = true;
  }

  return completed;
}

/*!
  Handles the delete input
*/
void CServant::handleDeleteInput(const string &input)
{
  CFileSpec fs = CFileSpec::fromUserInput(input);
  UCharBuffer b = _fileMgr.getSignedSpec(fs);
  _fileMgr.deleteFile(b);

  UCharBuffer uoid = getNewUoid(CConst::OBJ_TYPE_MSG);
  getUoidMap()->add(uoid, getNodeId(), getCurTime());
  uint ttl = _iniParser.getTtl() + 1;
  CAutoPtr<CMessageDelete> mP;
  mP = new CMessageDelete(uoid, ttl, b);
  CMsgEvent *eP = new CMsgEvent(mP.release(), "", CLog::MsgSent);
  _eventDispatcherT.addEvent(eP);
}

/*!
  Returns the keyword vector
*/
StringVector CServant::getStoreKwrdsFromInput(const string &s)
{
  StringVector v;
  string val;
  uint offset =0;
  uint index = s.find(CConst::NVP_SEP, offset);

  while (index != string::npos) {
    val = s.substr(offset, index - offset);
    v.push_back(val);
    offset += index - offset + CConst::NVP_SEP.length();

    index = s.find(CConst::DOUBLE_QUOTE, offset);
    if (string::npos == index) {
      C_THROW(string("invalid keyword ") + s);
    }
    offset += 1;

    index = s.find(CConst::DOUBLE_QUOTE, offset);
    if (string::npos == index) {
      C_THROW(string("invalid keyword ") + s);
    }

    val = s.substr(offset, index - offset);
    StringVector tVec = CUtil::split(val, CConst::SPACE_SEP);
    for (uint i=0; i<tVec.size(); i++) {
      v.push_back(tVec.at(i));
    }

    if (s.length() > index +2) {
      offset = index+2;
      index = s.find(CConst::NVP_SEP, offset);      
    } else {
      index = string::npos;
    }
  }

  return v;
}

/*!
  Returns the search keywords from input.
  The keywords will be separated by spaces.
*/
string CServant::getSearchKwrdsFromInput(const string &s)
{
  uint index = s.find(CConst::NVP_SEP, 0);
  if (index == string::npos) {
    C_THROW("Invalid keyword format. Unable to find nvp sep.");
  }

  index += CConst::NVP_SEP.length();
  if (index >= s.length()) {
    C_THROW("Invalid keyword format. no data after nvp sep.");
  }

  string output = s.substr(index, s.length());
  if (CUtil::startsWith(output, CConst::DOUBLE_QUOTE)) {
    index += CConst::DOUBLE_QUOTE.length();

    if (index >= s.length()) {
      C_THROW("Invalid keyword format. no data after quote sep.");
    }
    output = s.substr(index, s.length());    
  }
  if (CUtil::endsWith(output, CConst::DOUBLE_QUOTE)) {
    output = output.substr(0, output.length()-1);
  }

  return output;
}

//-----------------------------------------------------------------------------
// Initialization methods
//-----------------------------------------------------------------------------

/*!
  Initializes the node
*/
void CServant::initNode()
{
  if (_iniParser.isBeaconNode()) {
    initBeaconNode();
  } else {
    initRegularNode();
  }

  {
    CMutexLocker m(&_mutex);
    while(! _stopFlag) {
      alarm(1);
      _waitCond.wait(&_mutex);
    }
    alarm(0);
  }

  //stop all threads
  stopKbHandler();
  stopTcpServer();
  stopEventDispatcher();
  stopNeighborThreads();

  if (_lastError.length() > 0) {
    CUtil::writeError(_lastError);
  }
}

/*!
  Initializes the beacon Node
*/
void CServant::initBeaconNode()
{
  startTcpServer();
  startEventDispatcher();
  connectToBeacons();
  startKbHandler();
}

/*!
  Initializes the regular node
*/
void CServant::initRegularNode()
{
  if (CFile::exists(_nborFile)) {
    _isNodeJoining = false;
    initParticipate();

    if (! _stopFlag) {
      startTcpServer();
      startEventDispatcher();
      startKbHandler();
    }

  } else {
    _isNodeJoining = true;
    setMsgProcessing(true);
    initJoin();
    _joinTime = getCurTime();
  }
}

/*!
  Initializes the join process
*/
void CServant::initJoin()
{
  CUtil::writeOutputLn("Joining the network. Please wait...");

  const NodeDataList *bList = _iniParser.getBeaconList();
  if (bList->size() > 0) {
    CNode *nodeP = NULL;

    NodeDataList::const_iterator it;
    for (it = bList->begin(); it != bList->end(); it++) {
      try {
        nodeP = getNewNode(it->getHost(), it->getPort());
        if (! _neighborMap.addOrReplace(nodeP->getNodeId(), nodeP)) {
          delete nodeP;
          nodeP = NULL;
        } else {
          startThreadFromMain(nodeP);
        }
        break;
      } catch (CException &e) {
        _log.logError(e.toString());
      }
    }
    if (NULL == nodeP) {
      C_THROW("Unable to connect to any beacon nodes");
    }
  } else {
    C_THROW("No beacon nodes found");
  }
}

/*!
  Initializes the participating node
*/
void CServant::initParticipate()
{
  NodeDataList list = getNodesFromFile();

  if (list.size() > 0) {
    CNode *nodeP = NULL;

    NodeDataList::const_iterator it;
    for (it = list.begin(); it != list.end(); it++) {
      try {
        nodeP = getNewNode(it->getHost(), it->getPort());
        if (! _neighborMap.addOrReplace(nodeP->getNodeId(), nodeP)) {
          delete nodeP;
          nodeP = NULL;
        } else {
          startThreadFromMain(nodeP);
        }

      } catch (CException &e) {
        _log.logError(e.toString());
      }
    }

    if (_neighborMap.size() < _iniParser.getMinNeighbors()) {
      CFile::deleteFile(_iniParser.getHomePath(CConst::INIT_NBORFILE));
      string msg("Unable to connect to minimum (");
      msg += CUtil::getString(_iniParser.getMinNeighbors());
      msg += ") number of neighbors. # connected = ";
      msg += CUtil::getString(_neighborMap.size());
      CUtil::writeOutputLn(msg);

      _stopFlag = true;
      _restartEnabled = true;
    }
  } else {
    C_THROW("No Nodes found in the file. Delete the file.");
  }
}

/*!
  Attempts to connect to the remote node and if successful,
  then create a new node return.
*/
CNode* CServant::getNewNode(const string &rmtHost, uint16_t rmtPort)
{
  CAutoPtr<CTcpSocket> sockP(new CTcpSocket());
  sockP->openSock();
  sockP->connectTo(rmtHost, rmtPort);

  CNode *nodeP = new CNode(
                     getHostName(), _iniParser.getPort(),
                     rmtHost, rmtPort, true, sockP.release());
  assert (nodeP);
  nodeP->setServantHandle(this);

  return nodeP;
}

/*!
  Connects to becaon nodes.
*/
void CServant::connectToBeacons()
{
  const NodeDataList *bList = _iniParser.getBeaconList();
  if (bList->size() > 0) {
    NodeDataList::const_iterator it;
    for (it = bList->begin(); it != bList->end(); it++) {
      connectToBeacon(*it);
    }
  }
  _beaconRfshTime = getCurTime();
}

/*!
  Connects to the Beacon Node. If the connection already
  exist and active (that is the connection thread is alive)
  then this method will not do anything. Otherwise, it will
  create a new connection.
*/
void CServant::connectToBeacon(const CNodeData &data)
{
  string host = data.getHost();
  uint16_t port = data.getPort();
  string nodeId = CNode::getNodeId(host, port);

  if (_neighborMap.canAddOrReplace(nodeId)) {
    try {
      CNode *nodeP = getNewNode(host, port);
      //printf("adding / replacing node %s\n", nodeId.c_str());

      if (! _neighborMap.addOrReplace(nodeId, nodeP)) {
        delete nodeP;
        nodeP = NULL;

      } else {
        startThreadFromMain(nodeP);
      }
    } catch (CException &e) {
      _log.logError(e.toString());
    }
  }
}


//-----------------------------------------------------------------------------
//ITcpServer Interface
//-----------------------------------------------------------------------------
void CServant::notifyNewConn(
  int newSocket, struct sockaddr_in &addr, sigset_t *sigMask)
{
  CAutoPtr<CTcpSocket> sockP(new CTcpSocket());

  try {
    sockP->connectTo(newSocket, addr);
    string host = CTcpSocket::getHostName(addr);
    uint16_t port = CTcpSocket::getPort(addr);

    //printf("notifyNewConn: host = %s, port = %d socket = %d\n", host.c_str(), port, newSocket);
    CNode *nodeP = new CNode(
                    host, port, getHostName(),
                    _iniParser.getPort(), false, sockP.release());

    nodeP->setServantHandle(this);

    if (! _neighborMap.addOrReplace(nodeP->getNodeId(), nodeP)) {
      delete nodeP;
      nodeP = NULL;

    } else {
      setSigMask(sigMask, true);
      nodeP->start();
      setSigMask(sigMask, false);
    }

  } catch (CException &e) {
    _log.logError(e.toString());
  }
}

//-----------------------------------------------------------------------------
uint CServant::START_TIME     = 1;
CServant* CServant::_instance = NULL;
