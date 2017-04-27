/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "clog.h"
#include "cglobal.h"
#include "cexception.h"
#include "cutil.h"
#include "cconst.h"
#include "cmutexlocker.h"
#include "cmessage.h"

/*!
  Construts the log with the given file path
*/
CLog::CLog()
{
  _fileP = NULL;
  _streamP = NULL;
}

/*!
  Deletes the log
*/
CLog::~CLog()
{
  close();
}

/*!
  Private copy constructor
*/
CLog::CLog(const CLog &rhs) {}

/*!
  Private assignment operator
*/
CLog& CLog::operator=(const CLog &rhs)
{
  return *this;
}

/*!
  Initializes the log
*/
void CLog::open(const string &path, bool append)
{
  close();
  _path = path;

  if (! append) {
    if (CFile::exists(path)) {
      CFile::deleteFile(path);
    }
  }

  _fileP = new CFile(_path);
  assert(_fileP);
  _streamP = new COfStream(_fileP, append);
  assert(_streamP);
  _streamP->open();
}

/*!
  Returns the file path
*/
string CLog::getFilePath() const
{
  return _path;
}

/*!
  Close the log
*/
void CLog::close()
{
  if (NULL != _streamP) {
     if (_streamP->isOpened()) {
        _streamP->close();
     }
     delete _streamP;
     _streamP = NULL;
  }
  if (NULL != _fileP) {
    delete _fileP;
    _fileP = NULL;
  }
}

/*!
  Logs the error
*/
void CLog::logError(const string &s)
{
  log(string("**") + s);
  //printf("Error: %s\n", s.c_str());
}

/*!
  Logs the debug information
*/
void CLog::logDebug(const string &s)
{
  log(string("//") + s);
}

void CLog::logMessage(
  MsgType t, const string &nodeid, const CMessage *m)
{
  assert(m);
  string msg = getTypeStr(t) + string(" ");
  msg += getTimeStr() + string(" ");
  msg += nodeid + string(" ");
  msg += m->toString();  

  log(msg);  
}

/*!
  Logs the message to the log file
*/
void CLog::log(const string &s)
{
  if (NULL == _streamP){
    C_THROW("Log is not initlaized");
  }
  string str = s + CConst::NEWLINE;

  CMutexLocker m(&_mutex);
  _streamP->write(str.c_str(), str.length());
  _streamP->flush();

  //printf("%s\n", s.c_str());
}

/*!
  Returns the string format for the MsgType
*/
string CLog::getTypeStr(MsgType c) const
{
  string s;
  switch(c) {
    case MsgRcvd:
      s= "r";
      break;
    case MsgSent:
      s= "s";
      break;
    case MsgFrwd:
      s= "f";
      break;
    default:
      C_THROW("Invalid message type");
  }
  return s;
}

/*!
  Returns the string format for the time
*/
string CLog::getTimeStr() const
{
  struct timeval tv = CUtil::getCurrentTime();
  long sec = tv.tv_sec;
  int msec = (tv.tv_usec / 1000L);
  char buf[100];

  sprintf(buf, "%10ld.%03d", sec, msec);
  return string(buf);
}
