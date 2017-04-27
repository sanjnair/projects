/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "ciniparser.h"
#include "cfile.h"
#include "cifstream.h"
#include "cexception.h"
#include "cutil.h"
#include "cdir.h"

/*!
  Constructs the ini parser
*/
CIniParser::CIniParser() : _isBeacon(false) {}

/*!
  Deletes the Ini parser
*/
CIniParser::~CIniParser() {}

/*!
  Returns true if the node is becon node
*/
bool CIniParser::isBeaconNode() const
{
  return _isBeacon;
}

/*!
  Returns the ini file path
*/
string CIniParser::getFile() const
{
  return _file;
}

/*!
  Returns the port
*/
uint16_t CIniParser::getPort() const
{
  return _port;
}

/*!
  Returns location
*/
uint CIniParser::getLocation() const
{
  return _location;
}

/*!
  Returns home dir
*/
string CIniParser::getHomeDir() const
{
  return _homeDir;
}

/*!
  Returns log file
*/
string CIniParser::getLogFile() const
{
  return _logFile;
}

/*!
  Returns autoshutdown
*/
uint CIniParser::getAutoShutdown() const
{
  return _autoShutdown;
}

/*!
  Returns ttl
*/
uint CIniParser::getTtl() const
{
  return _ttl;
}

/*!
  Returns msglifetime
*/
uint CIniParser::getMsgLifeTime() const
{
  return _msgLifeTime;
}

/*!
  Returns initNeighbors
*/
uint CIniParser::getInitNeighbors() const
{
  return _initNeighbors;
}

/*!
  Returns jointimeout
*/
uint CIniParser::getJoinTimeout() const
{
  return _joinTimeout;
}

/*!
  Returns keepalivetimeout
*/
uint CIniParser::getKeepAliveTimeout() const
{
  return _keepAliveTimeout;
}

/*!
  Returns minneighbors
*/
uint CIniParser::getMinNeighbors() const
{
  return _minNeighbors;
}

/*!
  Returns Nocheck
*/
uint CIniParser::getNoCheck() const
{
  return _noCheck;
}

/*!
  Returns cacheprob
*/
double CIniParser::getCacheProb() const
{
  return _cacheProb;
}

/*!
  Returns storeprob
*/
double CIniParser::getStoreProb() const
{
  return _storeProb;
}

/*!
  Returns neighborstoreprob
*/
double CIniParser::getNeighborStoreProb() const
{
  return _neighborStoreProb;
}

/*!
  Returns cachesize
*/
uint CIniParser::getCacheSize() const
{
  return _cacheSize;
}

/*!
  Returns permsize
*/
uint CIniParser::getPermSize() const
{
  return _permSize;
}

/*!
  Returns beaconretry
*/
uint CIniParser::getBeaconRetry() const
{
  return _beaconRetry;
}

/*!
  Returns beaconlist
*/
const NodeDataList* CIniParser::getBeaconList() const
{
  return &_beaconList;
}

/*!
  Returns the home path for the given file
*/
string CIniParser::getHomePath(const string &file) const
{
  return (_homeDir + CDir::separator + file);
}

/*!
  Parses the ini file
*/
void CIniParser::parse(const string &file, const string &hostname)
{
  _file = file;
  init();
  CFile::errorIfNotExist(_file);

  bool cont = true;
  string header;
  string line;

  CFile f(_file);
  CIfStream stream(&f, false);
  stream.open();

  try {
    while (stream.isGood() && (! stream.isEof())) {

      line = CUtil::trim(stream.readLine());
      if (isValidLine(line)) {
        bool isHeader = isSectionHeader(line);

        if ((!isHeader) && (_sectionMap.size() <= 0)) {
          string msg = "Invalid Ini format. File should start with section header";
          C_THROW(msg);
        }

        if (isHeader) {
          header = getSectionHeader(line);
          string key = CUtil::toLower(header);
          if (_sectionMap.find(key) != _sectionMap.end()) {
            cont = false;
          } else {
            _sectionMap[key] = header;
            cont = true;
          }
        }

        if ((! isHeader) &&  cont) {
          processField(line, header);
        }
      }
    }
  } catch (CException &e) {
    stream.close();
    throw e;
  }
  validateFields();
  checkBeacon(hostname);
}

/*!
  Process the field line read from the file
*/
void CIniParser::processField(const string &s, const string &header)
{
  string name;
  string value;
  CUtil::getNvp(s, SEP_FIELD, name, value);

  string key = CUtil::toLower(name);
  if (_fieldMap.find(key) == _fieldMap.end()) {

    if (CUtil::isEqual(header, SECTION_INIT)) {
      processInitField(name, value);

    } else if (CUtil::isEqual(header, SECTION_BEACONS)) {
      processBeaconField(name, value);

    } else {
      string msg = string("Unknown section header ") + header;
      C_THROW(msg);
    }
    _fieldMap[key] = value;
  }
}


/*!
  Process the init field
*/
void CIniParser::processInitField(const string &name, const string &value)
{
  if (value.length() <= 0) {
    string msg = string("Invalid value in the NVP for name ") + name;
    C_THROW(msg);                    
  }
  if (CUtil::isEqual(FLD_PORT, name)) {
    _port = CUtil::getInteger<uint16_t>(value, true);

  } else if (CUtil::isEqual(FLD_LOCATION, name)) {
    _location = CUtil::getUInteger<ulong>(value);

  } else if (CUtil::isEqual(FLD_HOMEDIR, name)) {
    _homeDir = value;

  } else if (CUtil::isEqual(FLD_LOGFILENAME, name)) {
    _logFile = value;

  } else if (CUtil::isEqual(FLD_AUTOSHUTDOWN, name)) {
    _autoShutdown = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_TTL, name)) {
    _ttl = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_MSGLIFETIME, name)) {
    _msgLifeTime = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_INITNEIGHBORS, name)) {
    _initNeighbors = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_JOINTIMEOUT, name)) {
    _joinTimeout = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_KEEPALIVETIMEOUT, name)) {
    _keepAliveTimeout = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_MINNEIGHBORS, name)) {
    _minNeighbors = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_NOCHECK, name)) {
    _noCheck = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_CACHEPROB, name)) {
    _cacheProb = CUtil::getFloat<double>(value, true);

  } else if (CUtil::isEqual(FLD_STOREPROB, name)) {
    _storeProb = CUtil::getFloat<double>(value, true);

  } else if (CUtil::isEqual(FLD_NEIGHBORSTOREPROB, name)) {
    _neighborStoreProb = CUtil::getFloat<double>(value, true);

  } else if (CUtil::isEqual(FLD_CACHESIZE, name)) {
    _cacheSize = CUtil::getInteger<uint>(value, true);

  } else if (CUtil::isEqual(FLD_PERMSIZE, name)) {
    _permSize = CUtil::getInteger<uint>(value, true);

  } else {
    string msg = string("Unknown field ") + name + string(" found in the ini file.");
    C_THROW(msg);
  }
}

void CIniParser::processBeaconField(
      const string &name, const string &value)
{
  if (CUtil::isEqual(FLD_BEACONRETRY, name)) {
    _beaconRetry = CUtil::getInteger<uint>(value, true);

  } else {
    processBeaconNode(name);
  }
}

/*!
  Process the beacon node
*/
void CIniParser::processBeaconNode(const string &node)
{
  string sHost;
  string sPort;
  uint16_t port = 0;

  CUtil::getNvp(node, CIniParser::SEP_BEACON, sHost, sPort);
  port = CUtil::getInteger<uint16_t>(sPort, true);

  NodeDataList::const_iterator it;
  for( it= _beaconList.begin(); it != _beaconList.end(); it++) {
    if (CUtil::isEqual(it->getHost(), sHost)) {
      if (it->getPort() == port) {
        string msg = "Beacon list alrady contains host ";
        msg += sHost + string(" and port ") + sPort;
        C_THROW(msg);
      }
    }
  }
  CNodeData beaconData(sHost, port);
  _beaconList.push_back(beaconData);
}

/*!
  Returns true if the string is the section header
*/
bool CIniParser::isSectionHeader(const string &s) const
{
  if (CUtil::startsWith(s,SEP_HEADERSTART) &&
      CUtil::endsWith(s, SEP_HEADEREND)) {

    return true;
  }
  return false;
}

/*!
  Returns the section header
*/
string CIniParser::getSectionHeader(const string &s) const
{
  string str;

  string::size_type start = s.find(SEP_HEADERSTART);
  string::size_type end = s.find(SEP_HEADEREND);

  if ((string::npos != start) && (string::npos != end)) {
    if ((end - start) > 1) {
      str = CUtil::trim(s.substr(start+1, end-1));
    }
  }

  if (str.length() <= 0) {
    string msg = string("Invalid section header ") + s;
    C_THROW(msg);
  }

  if ((! CUtil::isEqual(str,SECTION_INIT)) &&
      (! CUtil::isEqual(str,SECTION_BEACONS))) {

    string msg = string("Invalid section header ") + str;
    C_THROW(msg);
  }

  return str;
}

/*!
  Returns true if the line is valid
*/

bool CIniParser::isValidLine(const string &s) const
{
  if (s.length() > 0) {
    if (! CUtil::startsWith(s, SEP_COMMENT)) {
      return true;
    }
  }
  return false;
}

/*!
  Initializes the Object
*/
void CIniParser::init() {
  _homeDir.clear();
  _port               = 0;
  _location           = 0;
  _logFile            = DEF_LOGFILENAME;
  _autoShutdown       = DEF_AUTOSHUTDOWN;
  _ttl                = DEF_TTL;
  _msgLifeTime        = DEF_MSGLIFETIME;
  _initNeighbors     = DEF_INITNEIGHBORS;
  _joinTimeout        = DEF_JOINTIMEOUT;
  _keepAliveTimeout   = DEF_KEEPALIVETIMEOUT;
  _minNeighbors       = DEF_MINNEIGHBORS;
  _noCheck            = DEF_NOCHECK;
  _cacheProb          = DEF_CACHEPROB;
  _storeProb          = DEF_STOREPROB;
  _neighborStoreProb  = DEF_NEIGHBORSTOREPROB;
  _cacheSize          = DEF_CACHESIZE;
  _permSize           = DEF_PERMSIZE;
  _beaconRetry        = DEF_BEACONRETRY;
}

/*!
  Checks to see if the node is a beacon node
*/
void CIniParser::checkBeacon(const string &hostname)
{
  NodeDataList::iterator it;
  for( it= _beaconList.begin(); it != _beaconList.end(); it++) {
    if (CUtil::isEqual(it->getHost(), hostname, true) &&
       (it->getPort() == _port)) {
      _isBeacon = true;
      _beaconList.erase(it);
      break;
    }
  }
}

/*!
  Validates the fields
*/
void CIniParser::validateFields() const
{
  validateField(FLD_PORT);
  validateField(FLD_LOCATION);
  validateField(FLD_HOMEDIR);
  CFile::errorIfNotExist(_homeDir);
}

/*!
  Validate the given field
*/
void CIniParser::validateField(const string &s) const
{
  string key = CUtil::toLower(s);
  if (_fieldMap.find(key) == _fieldMap.end()) {
    string msg = string("Field ") + s + string(" Not found in ini file");
    C_THROW(msg);
  }
}

//-----------------------------------------------------------------------------
const string CIniParser::DEF_LOGFILENAME        = "servant.log";
const uint CIniParser::DEF_AUTOSHUTDOWN         = 900;
const uint CIniParser::DEF_TTL                  = 30;
const uint CIniParser::DEF_MSGLIFETIME          = 30;
const uint CIniParser::DEF_INITNEIGHBORS        = 3;
const uint CIniParser::DEF_JOINTIMEOUT          = 15;
const uint CIniParser::DEF_KEEPALIVETIMEOUT     = 60;
const uint CIniParser::DEF_MINNEIGHBORS         = 2;
const uint CIniParser::DEF_NOCHECK              = 0;
const double CIniParser::DEF_CACHEPROB          = 0.1;
const double CIniParser::DEF_STOREPROB          = 0.1;
const double CIniParser::DEF_NEIGHBORSTOREPROB  = 0.2;
const uint CIniParser::DEF_CACHESIZE            = 500;
const uint CIniParser::DEF_PERMSIZE             = 500;
const uint CIniParser::DEF_BEACONRETRY          = 30;

const string CIniParser::FLD_PORT               = "Port";
const string CIniParser::FLD_LOCATION           = "Location";
const string CIniParser::FLD_HOMEDIR            = "HomeDir";
const string CIniParser::FLD_LOGFILENAME        = "LogFileName";
const string CIniParser::FLD_AUTOSHUTDOWN       = "AutoShutdown";
const string CIniParser::FLD_TTL                = "TTL";
const string CIniParser::FLD_MSGLIFETIME        = "MsgLifeTime";
const string CIniParser::FLD_INITNEIGHBORS      = "InitNeighbors";
const string CIniParser::FLD_JOINTIMEOUT        = "JoinTimeout";
const string CIniParser::FLD_KEEPALIVETIMEOUT   = "KeepAliveTimeout";
const string CIniParser::FLD_MINNEIGHBORS       = "MinNeighbors";
const string CIniParser::FLD_NOCHECK            = "NoCheck";
const string CIniParser::FLD_CACHEPROB          = "CacheProb";
const string CIniParser::FLD_STOREPROB          = "StoreProb";
const string CIniParser::FLD_NEIGHBORSTOREPROB  = "NeighborStoreProb";
const string CIniParser::FLD_CACHESIZE          = "CacheSize";
const string CIniParser::FLD_PERMSIZE           = "PermSize";
const string CIniParser::FLD_BEACONRETRY        = "Retry";

const string CIniParser::SEP_HEADERSTART        = "[";
const string CIniParser::SEP_HEADEREND          = "]";
const string CIniParser::SEP_COMMENT            = ";";
const string CIniParser::SEP_FIELD              = "=";
const string CIniParser::SEP_BEACON             = ":";

const string CIniParser::SECTION_INIT           = "init";
const string CIniParser::SECTION_BEACONS        = "beacons";

