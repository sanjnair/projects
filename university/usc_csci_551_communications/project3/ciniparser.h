/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CINIPARSER_H
#define CINIPARSER_H

#include "cglobal.h"
#include "cnodedata.h"

class CIniParser
{
public:
  CIniParser();
  ~CIniParser();

  void parse(const string &file, const string &hostname);

  bool isBeaconNode() const;
  string getFile() const;
  uint16_t getPort() const;
  uint getLocation() const;
  string getHomeDir() const;
  string getLogFile() const;
  uint getAutoShutdown() const;
  uint getTtl() const;
  uint getMsgLifeTime() const;
  uint getInitNeighbors() const;
  uint getJoinTimeout() const;
  uint getKeepAliveTimeout() const;
  uint getMinNeighbors() const;
  uint getNoCheck() const;
  double getCacheProb() const;
  double getStoreProb() const;
  double getNeighborStoreProb() const;
  uint getCacheSize() const;
  uint getPermSize() const;
  uint getBeaconRetry() const;
  const NodeDataList *getBeaconList() const;

  string getHomePath(const string &file) const;

private:
  void init();
  void processField(const string &s, const string &header);
  void processInitField(const string &name, const string &value);
  void processBeaconField(const string &name, const string &value);
  void processBeaconNode(const string &node);

  bool isSectionHeader(const string &s) const;
  bool isValidLine(const string &s) const;
  string getSectionHeader(const string &s) const;
  void checkBeacon(const string &name);
  void validateFields() const;
  void validateField(const string &s) const;


  bool _isBeacon;
  string _file;
  uint16_t _port;
  uint _location;
  string _homeDir;
  string _logFile;
  uint _autoShutdown;
  uint _ttl;
  uint _msgLifeTime;
  uint _initNeighbors;
  uint _joinTimeout;
  uint _keepAliveTimeout;
  uint _minNeighbors;
  uint _noCheck;
  double _cacheProb;
  double _storeProb;
  double _neighborStoreProb;
  uint _cacheSize;
  uint _permSize;
  uint _beaconRetry;

  StringMap _sectionMap;
  StringMap _fieldMap;
  NodeDataList _beaconList;

  //---------------------------------------------------------------------------
  static const string DEF_LOGFILENAME;
  static const uint DEF_AUTOSHUTDOWN;
  static const uint DEF_TTL;
  static const uint DEF_MSGLIFETIME;
  static const uint DEF_INITNEIGHBORS;
  static const uint DEF_JOINTIMEOUT;
  static const uint DEF_KEEPALIVETIMEOUT;
  static const uint DEF_MINNEIGHBORS;
  static const uint DEF_NOCHECK;
  static const double DEF_CACHEPROB;
  static const double DEF_STOREPROB;
  static const double DEF_NEIGHBORSTOREPROB;
  static const uint DEF_CACHESIZE;
  static const uint DEF_PERMSIZE;
  static const uint DEF_BEACONRETRY;

  static const string FLD_PORT;
  static const string FLD_LOCATION;
  static const string FLD_HOMEDIR;      
  static const string FLD_LOGFILENAME;
  static const string FLD_AUTOSHUTDOWN;
  static const string FLD_TTL;
  static const string FLD_MSGLIFETIME;
  static const string FLD_INITNEIGHBORS;
  static const string FLD_JOINTIMEOUT;
  static const string FLD_KEEPALIVETIMEOUT;
  static const string FLD_MINNEIGHBORS;
  static const string FLD_NOCHECK;
  static const string FLD_CACHEPROB;
  static const string FLD_STOREPROB;
  static const string FLD_NEIGHBORSTOREPROB;
  static const string FLD_CACHESIZE;
  static const string FLD_PERMSIZE;
  static const string FLD_BEACONRETRY;

  static const string SEP_HEADERSTART;
  static const string SEP_HEADEREND;
  static const string SEP_COMMENT;
  static const string SEP_FIELD;
  static const string SEP_BEACON;  
  
  static const string SECTION_INIT;
  static const string SECTION_BEACONS;  
};


#endif


