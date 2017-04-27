/***************************************************************************
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CLOG_H
#define CLOG_H

#include "cmutex.h"
#include "cfile.h"
#include "cofstream.h"

class CMessage;

class CLog {
public:

  enum MsgType {
    MsgRcvd,
    MsgSent,
    MsgFrwd
  };

    CLog();
    ~CLog();

    void open(const string &path, bool append);
    void close();
    string getFilePath() const;

    void logMessage(MsgType t, const string &nodeid, const CMessage *m);    
    void logError(const string &s);
    void logDebug(const string &s);

private:
  CLog(const CLog &rhs);
  CLog &operator=(const CLog &rhs);
  void log(const string &s);
  string getTypeStr(MsgType t) const;
  string getTimeStr() const;

  string _path;
  CMutex _mutex;
  CFile *_fileP;
  COfStream *_streamP;
};

#endif //CLOG_H

