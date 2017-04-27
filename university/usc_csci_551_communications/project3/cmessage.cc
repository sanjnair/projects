/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cglobal.h"
#include <openssl/sha.h>
#include <assert.h>
#include "cmessage.h"
#include "cconst.h"
#include "cexception.h"
#include "cutil.h"
#include "cautoptr.h"
#include "csha1data.h"
#include "cfile.h"

/*!
  Creates the message object
*/
CMessage::CMessage(
  const UCharBuffer &uoid, uint8_t ttl)
  : _uoid(uoid), _ttl(ttl)
{
  assert(_uoid.size() == SHA_DIGEST_LENGTH);
}

/*!
  Deletes the message object
*/  
CMessage::~CMessage() {}

/*!
  Gets the Uoid
*/
UCharBuffer CMessage::getUoid() const
{
  return _uoid;
}

/*!
  Gets the TTL
*/
uchar CMessage::getTtl() const
{
  return _ttl;
}

/*!
  Decrements the TTL
*/
void CMessage::decrTtl()
{
  if (_ttl > 0) {
    _ttl--;
  }
}

/*!
  Returns the string represenation of the message object
  used for logging.
*/
string CMessage::toString_p() const
{
  string str;
  UCharBuffer b = toUcharBuffer();
  
  str  = getTypeStr() + string(" ");
  str += CUtil::getString(b.size()) + string(" ");
  str += CUtil::getString(uint16_t(_ttl)) + string(" ");
  str += getFormattedId(_uoid) + string(" ");

  return str; 
}

/*!
  Returns the Byte format of the message
*/
void CMessage::toUcharBuffer_p(UCharBuffer &b) const
{
  addToBuffer(b, getType());
  addToBuffer(b, _uoid);
  addToBuffer(b, _ttl);
  addToBuffer(b, uint8_t(0));
  addToBuffer(b, getByteLen());
}

/*!
  Get type from the buffer
*/
uint8_t CMessage::getTypeFromBuffer(const UCharBuffer &b)
{
  errorIfInvalidBuffer(b);
  return b.at(0);
}

/*!
  Returns TTL value from Buffer
*/
uint8_t CMessage::getTtlFromBuffer(const UCharBuffer &b)
{
  errorIfInvalidBuffer(b);
  return b.at(21);
}

/*!
  Returns UOID from Buffer
*/
UCharBuffer CMessage::getUoidFromBuffer(const UCharBuffer &b)
{
  errorIfInvalidBuffer(b);
  return CMessage::getBuffer(b, 1, SHA_DIGEST_LENGTH);
}

/*!
  Returns Datalen from Buffer
*/
uint32_t CMessage::getDataLenFromBuffer(const UCharBuffer &b)
{
  uint32_t len = 0;
  CMessage::getOrdData(b, 23, len);
  return len;
}

/*!
  Adds the byte to the buffer
*/
void CMessage::addToBuffer(UCharBuffer &b, uint8_t num)
{
  b.push_back(num);
}

/*!
  Adds the number to Buffer.
*/
void CMessage::addToBuffer(UCharBuffer &buf, uint16_t num)
{
  uint16_t t = htons(num);
  unsigned char arr[2];
  memcpy(arr, &t, sizeof(t));

  for (int i=0; i<2; i++) {
    buf.push_back(arr[i]);
  }
}

/*!
  Adds the number to Buffer.
*/
void CMessage::addToBuffer(UCharBuffer &buf, uint32_t num)
{
  uint32_t t = htonl(num);
  unsigned char arr[4];
  memcpy(arr, &t, sizeof(t));
  for (int i=0; i<4; i++) {
    buf.push_back(arr[i]);
  }
}

/*!
  Adds the Buffer
*/
void CMessage::addToBuffer(UCharBuffer &b, const string &str)
{
  const char *s = str.c_str();
  for (uint i=0; i <str.length(); i++) {
    b.push_back((uint8_t) s[i]);
  }
}

/*!
  Adds the Buffer
*/
void CMessage::addToBuffer(UCharBuffer &b, const UCharBuffer &b1)
{
  for (uint i=0; i<b1.size(); i++) {
    b.push_back(b1.at(i));
  }
}

/*!
  Return number of bytes from the \a from position
*/
UCharBuffer CMessage::getBuffer(const UCharBuffer &b, uint from, uint nBytes)
{
  errorIfInvalidDataReq(b, from, nBytes);

  UCharBuffer buf;
  for (uint i=from; i < (from+nBytes); i++) {
    buf.push_back(b.at(i));
  }

  return buf;
}

/*!
  Return the string from the buffer
*/
string CMessage::getString(const UCharBuffer &b, uint from, uint nBytes)
{
  errorIfInvalidDataReq(b, from, nBytes);
  string s;

  for (uint i=from; i< (from + nBytes); i++) {
    s.push_back((char)b.at(i));
  }

  return s;
}

/*!
  Returns ordinal data from the message buffer
*/
void CMessage::getOrdData(const UCharBuffer &b, uint from, uint32_t &t)
{
  t = getOrdData<uint32_t>(b, from, sizeof(uint32_t));
  t = htonl(t);
}

/*!
  Returns ordinal data from the message buffer
*/
void CMessage::getOrdData(const UCharBuffer &b, uint from, uint16_t &t)
{
  t = getOrdData<uint16_t>(b, from, sizeof(uint16_t));
  t = htons(t);
}

/*!
  Throws exception if the data requested is invalid
*/
void CMessage::errorIfInvalidDataReq(
    const UCharBuffer &b, uint from, uint nBytes)
{
  assert(from >=0);
  assert(nBytes >0);

  if ((from + nBytes) > b.size()) {
    string msg;
    msg += "Unable to return the requesed data from Buffer. Buffer size = ";
    msg += CUtil::getString(b.size());
    msg += string(" From  = ") + CUtil::getString(from);
    msg += string(" nBytes  = ") + CUtil::getString(nBytes);
  }
}

/*!
  Throws exception if the buffer is invalid
*/
void CMessage::errorIfInvalidBuffer(const UCharBuffer &b)
{
  if (b.size() < HEADER_LEN) {
    string msg = string("Message length ") + CUtil::getString(b.size());
    msg += " is less than the message header size ";
    msg += CUtil::getString(HEADER_LEN);
    C_THROW(msg);
  }
}

/*!
  Print the buffer in Both Hex and Ascii format
*/
void CMessage::printBuffer() const
{
  CMessage::printBuffer(toUcharBuffer());;
}

/*!
  Returns new Uoid for the given node instance id
  and object type
*/
UCharBuffer CMessage::getNewUoid(
  const string &nodeInstId,
  const string &objType)
{
  static ulong seqNo = (ulong) 1;
  char str_buf[104];

  snprintf(
    str_buf, sizeof(str_buf), "%s_%s_%1ld",
    nodeInstId.c_str(), objType.c_str(), (long)seqNo++);

  UCharBuffer uoid = CSha1Data::getSha1(str_buf);
  return uoid;
}

/*!
  Returns the formatted string for the buffer.
  The first 4 bytes of the
*/
string CMessage::getFormattedId(const UCharBuffer &b) const
{
  if (b.size() != SHA_DIGEST_LENGTH) {
    string msg = string("Message length must be ");
    msg += CUtil::getString(SHA_DIGEST_LENGTH);
    msg += string("Given = ") + CUtil::getString(b.size());
    C_THROW(msg);
  }

  string id = CUtil::getHexStr(b);
  id = string("0x") + id.substr((SHA_DIGEST_LENGTH - 8), 8);

  return id;
}

/*!
  Prints the contents of the buffer
*/
void CMessage::printBuffer(const UCharBuffer &buf)
{
  if (buf.size() > 0) {
    for (uint i=0; i<buf.size(); i++) {
      printf("%d) 0x%02x \t%c\n", i, (uint8_t)buf.at(i), buf.at(i));
    }
  } else {
    printf("Buffer is empty\n");
  }
}

const uint32_t CMessage::HEADER_LEN     = 27;

//-----------------------------------------------------------------------------
/*!
  Constructs the join message request
*/
CMessageJnrq::CMessageJnrq(
    const UCharBuffer &uoid, uint8_t ttl,
    uint32_t hostLoc, uint16_t port, string hostname)
    : CMessage(uoid, ttl)
{
  _hostLoc = hostLoc;
  _port = port;
  _hostname = hostname;
}

/*!
  Deletes the join message request
*/
CMessageJnrq::~CMessageJnrq() {}

/*!
  Clones the object
*/
CMessageJnrq* CMessageJnrq::clone() const
{
  return new CMessageJnrq(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageJnrq::getType() const
{
  return CConst::TYPE_JOIN_REQ;
}

/*!
  Returns the string format of message type
*/
string CMessageJnrq::getTypeStr() const
{
  return CConst::TYPE_JOIN_REQ_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageJnrq::getByteLen() const
{
  return (sizeof(_hostLoc) + sizeof(_port) +
          _hostname.length());
}

/*!
  Returns string represntation of the object
*/
string CMessageJnrq::toString() const
{
  string s = toString_p();
  s += CUtil::getString(_port) + string(" ");
  s += _hostname;

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageJnrq::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);
  addToBuffer(b, _hostLoc);
  addToBuffer(b, _port);
  addToBuffer(b, _hostname);

  return b;
}

/*!
  Returns host location
*/
uint32_t CMessageJnrq::getHostLoc() const
{
  return _hostLoc;
}

/*!
  Returns port
*/
uint16_t CMessageJnrq::getPort() const
{
  return _port;
}

/*!
  Returns host name
*/
string CMessageJnrq::getHostName() const
{
  return _hostname;
}

/*!
  Returns a new Message from the buffer
*/
CMessageJnrq* CMessageJnrq::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint8_t ttl = getTtlFromBuffer(b);
  //uint32_t dataLen = getDataLenFromBuffer(b);

  uint from = HEADER_LEN;
  uint32_t hostLoc = 0;
  CMessage::getOrdData(b, from, hostLoc);

  from += sizeof(hostLoc);
  uint16_t hostPort = 0;
  CMessage::getOrdData(b, from, hostPort);

  from += sizeof(hostPort);
  uint nBytes = b.size() - from;

  if (nBytes <= 0) {
    C_THROW("Invalid message. Incorrect size");
  }
  if (nBytes <= 0) {
    C_THROW("Invalid message. Incorrect size");
  }

  string hostName = getString(b, from, nBytes);

  CMessageJnrq *m = new CMessageJnrq(uoid, ttl, hostLoc, hostPort, hostName); assert(m);
  return m;
}

//-----------------------------------------------------------------------------
/*!
  Constructs the join message response
*/
CMessageJnrs::CMessageJnrs(
  const UCharBuffer &uoid,
  const UCharBuffer &requoid, uint32_t distance,
  uint16_t port, string hostname)
  : CMessage(uoid, 1)
{
  _reqUoid = requoid;
  _distance = distance;
  _port = port;
  _hostname = hostname;

  assert(_reqUoid.size() == SHA_DIGEST_LENGTH);
}

/*!
  Deletes the join message response
*/
CMessageJnrs::~CMessageJnrs() {}

/*!
  Clones the object
*/
CMessageJnrs* CMessageJnrs::clone() const
{
  return new CMessageJnrs(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageJnrs::getType() const
{
  return CConst::TYPE_JOIN_RES;
}

/*!
  Returns the string format of message type
*/
string CMessageJnrs::getTypeStr() const
{
  return CConst::TYPE_JOIN_RES_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageJnrs::getByteLen() const
{
  return (_reqUoid.size() + sizeof(_distance) +
          sizeof(_port) + _hostname.length());
}

/*!
  Returns string represntation of the object
*/
string CMessageJnrs::toString() const
{
  string s = toString_p();
  s += getFormattedId(_reqUoid) + string(" ");
  s += CUtil::getString(_distance) + string(" ");
  s += CUtil::getString(_port) + string(" ");
  s += _hostname;

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageJnrs::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);
  addToBuffer(b, _reqUoid);
  addToBuffer(b, _distance);
  addToBuffer(b, _port);
  addToBuffer(b, _hostname);

  return b;
}

/*!
  Returns the request uoid
*/
UCharBuffer CMessageJnrs::getReqUoid() const
{
  return _reqUoid;
}

uint32_t CMessageJnrs::getDistance() const
{
  return _distance;
}

/*!
  Returns Port
*/
uint16_t CMessageJnrs::getPort() const
{
  return _port;
}

/*!
  Returns the hostname
*/
string CMessageJnrs::getHostName() const
{
  return _hostname;
}

/*!
  Returns a new Message from the buffer
*/
CMessageJnrs* CMessageJnrs::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);

  uint from = HEADER_LEN;
  UCharBuffer requoid = CMessage::getBuffer(b, HEADER_LEN, SHA_DIGEST_LENGTH);

  from += SHA_DIGEST_LENGTH;
  uint32_t distance = 0;
  CMessage::getOrdData(b, from, distance);

  from += sizeof(distance);
  uint16_t hostPort = 0;
  CMessage::getOrdData(b, from, hostPort);

  from += sizeof(hostPort);
  uint nBytes = b.size() - from;
  if (nBytes <= 0) {
    C_THROW("Invalid message. Incorrect size");
  }

  string hostName = getString(b, from, nBytes);
  CMessageJnrs *m = new CMessageJnrs(
                          uoid, requoid, distance, hostPort, hostName); assert(m);
  return m;
}
//-----------------------------------------------------------------------------
/*!
  Constructs the Hello message
*/
CMessageHello::CMessageHello(
  const UCharBuffer &uoid,
  uint16_t port, string hostname)
  : CMessage(uoid, 1)
{
  _port = port;
  _hostname = hostname;
}

/*!
  Deletes the message response
*/
CMessageHello::~CMessageHello() {}

/*!
  Clones the object
*/
CMessageHello* CMessageHello::clone() const
{
  return new CMessageHello(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageHello::getType() const
{
  return CConst::TYPE_HELLO;
}

/*!
  Returns the string format of message type
*/
string CMessageHello::getTypeStr() const
{
  return CConst::TYPE_HELLO_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageHello::getByteLen() const
{
  return (sizeof(_port) + _hostname.length());
}

/*!
  Returns string represntation of the object
*/
string CMessageHello::toString() const
{
  string s = toString_p();
  s += CUtil::getString(_port) + string(" ");
  s += _hostname;

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageHello::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);
  addToBuffer(b, _port);
  addToBuffer(b, _hostname);

  return b;
}

/*!
  Returns Port
*/
uint16_t CMessageHello::getPort() const
{
  return _port;
}

/*!
  Returns the hostname
*/
string CMessageHello::getHostName() const
{
  return _hostname;
}

/*!
  Returns a new Message from the buffer
*/
CMessageHello* CMessageHello::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint32_t dataLen = getDataLenFromBuffer(b);

  uint16_t hostPort = 0;
  CMessage::getOrdData(b, HEADER_LEN, hostPort);
  uint from = HEADER_LEN + sizeof(hostPort);
  uint nBytes = dataLen - sizeof(hostPort);
  if (nBytes <= 0) {
    C_THROW("Invalid message. Incorrect size");
  }

  string hostName = getString(b, from, nBytes);

  CMessageHello *m = new CMessageHello(uoid, hostPort, hostName); assert(m);
  return m;
}

//-----------------------------------------------------------------------------
/*!
  Constructs the Keepalive message
*/
CMessageKeepAlive::CMessageKeepAlive(
  const UCharBuffer &uoid)
  : CMessage(uoid, uint8_t(1)) { }

/*!
  Deletes the message response
*/
CMessageKeepAlive::~CMessageKeepAlive() {}

/*!
  Clones the object
*/
CMessageKeepAlive* CMessageKeepAlive::clone() const
{
  return new CMessageKeepAlive(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageKeepAlive::getType() const
{
  return CConst::TYPE_KEEPALIVE;
}

/*!
  Returns the string format of message type
*/
string CMessageKeepAlive::getTypeStr() const
{
  return CConst::TYPE_KEEPALIVE_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageKeepAlive::getByteLen() const
{
  return uint32_t(0);
}

/*!
  Returns string represntation of the object
*/
string CMessageKeepAlive::toString() const
{
  return toString_p();
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageKeepAlive::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);

  return b;
}

/*!
  Returns a new Message from the buffer
*/
CMessageKeepAlive* CMessageKeepAlive::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  CMessageKeepAlive *m = new CMessageKeepAlive(uoid); assert(m);
  return m;
}


//-----------------------------------------------------------------------------
/*!
  Constructs the Notify message
*/
CMessageNotify::CMessageNotify(
  const UCharBuffer &uoid, uint8_t errorCode)
  : CMessage(uoid, uint8_t(1))
{
  _errorCode = errorCode;
}

/*!
  Deletes the message
*/
CMessageNotify::~CMessageNotify() {}

/*!
  Clones the object
*/
CMessageNotify* CMessageNotify::clone() const
{
  return new CMessageNotify(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageNotify::getType() const
{
  return CConst::TYPE_NOTIFY;
}

/*!
  Returns the string format of message type
*/
string CMessageNotify::getTypeStr() const
{
  return CConst::TYPE_NOTIFY_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageNotify::getByteLen() const
{
  return (sizeof(_errorCode));
}

/*!
  Returns string represntation of the object
*/
string CMessageNotify::toString() const
{
  string s = toString_p();
  s += CUtil::getString(static_cast<uint>(_errorCode)) + string(" ");

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageNotify::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);
  addToBuffer(b, _errorCode);

  return b;
}

/*!
  Returns ErrorCode
*/
uint8_t CMessageNotify::getErrorCode() const
{
  return _errorCode;
}

/*!
  Returns a new Message from the buffer
*/
CMessageNotify* CMessageNotify::fromBuffer(const UCharBuffer &b)
{
  if (b.size() != (HEADER_LEN + 1)) {
    string msg("Notify: Invalid number of bytes received. Expected = ");
    msg += CUtil::getString(HEADER_LEN + 1) + string(" actual = ");
    msg += b.size();
    C_THROW(msg);
  }

  UCharBuffer uoid = getUoidFromBuffer(b);
  uint8_t errorCode = b.at(HEADER_LEN);

  CMessageNotify *m = new CMessageNotify(uoid, errorCode); assert(m);
  return m;
}

//-----------------------------------------------------------------------------
/*!
  Constructs the CheckReq message
*/
CMessageCheckReq::CMessageCheckReq(
  const UCharBuffer &uoid, uint8_t ttl)
  : CMessage(uoid, ttl) { }

/*!
  Deletes the message response
*/
CMessageCheckReq::~CMessageCheckReq() {}

/*!
  Clones the object
*/
CMessageCheckReq* CMessageCheckReq::clone() const
{
  return new CMessageCheckReq(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageCheckReq::getType() const
{
  return CConst::TYPE_CHECK_REQ;
}

/*!
  Returns the string format of message type
*/
string CMessageCheckReq::getTypeStr() const
{
  return CConst::TYPE_CHECK_REQ_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageCheckReq::getByteLen() const
{
  return uint32_t(0);
}

/*!
  Returns string represntation of the object
*/
string CMessageCheckReq::toString() const
{
  return toString_p();
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageCheckReq::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);

  return b;
}

/*!
  Returns a new Message from the buffer
*/
CMessageCheckReq* CMessageCheckReq::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint8_t ttl = getTtlFromBuffer(b);

  CMessageCheckReq *m = new CMessageCheckReq(uoid, ttl); assert(m);
  return m;
}

//-----------------------------------------------------------------------------
/*!
  Constructs the Check response message
*/
CMessageCheckRes::CMessageCheckRes(
  const UCharBuffer &uoid,
  const UCharBuffer &requoid)
  : CMessage(uoid, 1)
{
  _reqUoid = requoid;
  assert(_reqUoid.size() == SHA_DIGEST_LENGTH);
}

/*!
  Deletes the check response
*/
CMessageCheckRes::~CMessageCheckRes() {}

/*!
  Clones the object
*/
CMessageCheckRes* CMessageCheckRes::clone() const
{
  return new CMessageCheckRes(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageCheckRes::getType() const
{
  return CConst::TYPE_CHECK_RES;
}

/*!
  Returns the string format of message type
*/
string CMessageCheckRes::getTypeStr() const
{
  return CConst::TYPE_CHECK_RES_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageCheckRes::getByteLen() const
{
  return (_reqUoid.size());
}

/*!
  Returns string represntation of the object
*/
string CMessageCheckRes::toString() const
{
  string s = toString_p();
  s += getFormattedId(_reqUoid) + string(" ");

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageCheckRes::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);
  addToBuffer(b, _reqUoid);

  return b;
}

/*!
  Returns the request uoid
*/
UCharBuffer CMessageCheckRes::getReqUoid() const
{
  return _reqUoid;
}

/*!
  Returns a new Message from the buffer
*/
CMessageCheckRes* CMessageCheckRes::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  UCharBuffer requoid = CMessage::getBuffer(b, HEADER_LEN, SHA_DIGEST_LENGTH);

  CMessageCheckRes *m = new CMessageCheckRes(uoid, requoid); assert(m);
  return m;
}

//-----------------------------------------------------------------------------
/*!
  Constructs the Status message
*/
CMessageStatusReq::CMessageStatusReq(
  const UCharBuffer &uoid, uint8_t ttl, uint8_t statusType)
  : CMessage(uoid, ttl)
{
  _statusType = statusType;
}

/*!
  Deletes the message
*/
CMessageStatusReq::~CMessageStatusReq() {}

/*!
  Clones the object
*/
CMessageStatusReq* CMessageStatusReq::clone() const
{
  return new CMessageStatusReq(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageStatusReq::getType() const
{
  return CConst::TYPE_STATUS_REQ;
}

/*!
  Returns the string format of message type
*/
string CMessageStatusReq::getTypeStr() const
{
  return CConst::TYPE_STATUS_REQ_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageStatusReq::getByteLen() const
{
  return (sizeof(_statusType));
}

/*!
  Returns string represntation of the object
*/
string CMessageStatusReq::toString() const
{
  string s = toString_p();
  s += CUtil::getString(static_cast<uint>(_statusType)) + string(" ");

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageStatusReq::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);
  addToBuffer(b, _statusType);

  return b;
}

/*!
  Returns Status Type
*/
uint8_t CMessageStatusReq::getStatusType() const
{
  return _statusType;
}

/*!
  Returns a new Message from the buffer
*/
CMessageStatusReq* CMessageStatusReq::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint8_t ttl = getTtlFromBuffer(b);
  //uint32_t dataLen = getDataLenFromBuffer(b);
  uint8_t statusType = b.at(HEADER_LEN);

  CMessageStatusReq *m = new CMessageStatusReq(uoid, ttl, statusType); assert(m);
  return m;
}

//-----------------------------------------------------------------------------

/*!
  Constructs the status message response
*/
CMessageStatusRes::CMessageStatusRes(
  const UCharBuffer &uoid,
  const UCharBuffer &requoid,
  uint16_t port, string hostname)
  : CMessage(uoid, 1)
{
  _reqUoid = requoid;
  _port = port;
  _hostname = hostname;

  assert(_reqUoid.size() == SHA_DIGEST_LENGTH);
}

/*!
  Deletes the status message response
*/
CMessageStatusRes::~CMessageStatusRes() {}

/*!
  Clones the object
*/
CMessageStatusRes* CMessageStatusRes::clone() const
{
  return new CMessageStatusRes(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageStatusRes::getType() const
{
  return CConst::TYPE_STATUS_RES;
}

/*!
  Returns the string format of message type
*/
string CMessageStatusRes::getTypeStr() const
{
  return CConst::TYPE_STATUS_RES_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageStatusRes::getByteLen() const
{
  uint32_t len = 0;
  uint32_t hostInfoLen = 0;

  len =  _reqUoid.size();
  len += sizeof(hostInfoLen);
  len += sizeof(_port);
  len += _hostname.length();

  for (uint i=0; i<_vctRecord.size(); i++) {
    len += _vctRecord.at(i).size();
  }

  return len;
}

/*!
  Returns string represntation of the object
*/
string CMessageStatusRes::toString() const
{
  string s = toString_p();
  s += getFormattedId(_reqUoid);

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageStatusRes::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);

  uint32_t hInfoLen = sizeof(_port) + _hostname.length();

  addToBuffer(b, _reqUoid);
  addToBuffer(b, hInfoLen);  
  addToBuffer(b, _port);
  addToBuffer(b, _hostname);
  
  for (uint i=0; i < _vctRecord.size(); i++) {
    addToBuffer(b, _vctRecord.at(i));
  }

  return b;
}

/*!
  Returns the request uoid
*/
UCharBuffer CMessageStatusRes::getReqUoid() const
{
  return _reqUoid;
}

/*!
  Returns Port
*/
uint16_t CMessageStatusRes::getPort() const
{
  return _port;
}

/*!
  Returns the hostname
*/
string CMessageStatusRes::getHostName() const
{
  return _hostname;
}

/*!
  Sets the neighbor list
*/
void CMessageStatusRes::setNeighborList(
  const NodeDataList &list)
{
  _vctRecord.clear();

  UCharBuffer b;
  uint32_t rLen = 0;
  NodeDataList::const_iterator it;

  for (it = list.begin(); it != list.end(); it++) {
    b.clear();

    uint16_t port = it->getPort();
    string host = it->getHost();

    rLen = sizeof(port) + host.length();
    addToBuffer(b, rLen);
    addToBuffer(b, port);
    addToBuffer(b, host);
    _vctRecord.push_back(b);
  }

  b.clear();
  rLen = 0;
  addToBuffer(b, rLen);
  _vctRecord.push_back(b);
}
 
/*!
  Returns the hostname
*/
NodeDataList CMessageStatusRes::getNeighborList() const
{
  NodeDataList nList;

  for (uint i=0; i< _vctRecord.size(); i++) {
    uint from = 0;
    uint32_t rLen = 0;

    UCharBuffer b = _vctRecord.at(i);

    CMessage::getOrdData(b, from, rLen);
    from += sizeof(rLen);

    if (rLen > 0) {
      uint16_t port;
      string host;
      
      CMessage::getOrdData(b, from, port);
      from += sizeof(port);
      host = getString(b, from, (rLen -sizeof(port)));
      CNodeData d(host, port);
      nList.push_back(d);
    }
  }
  
  return nList;
}

/*!
  Sets the metadata vector
*/
void CMessageStatusRes::setMetaDataVec(
  const MetaDataVector &v)
{
  _vctRecord.clear();

  uint32_t rLen = 0;

  for (uint i=0; i < v.size(); i++) {
    UCharBuffer mb = v.at(i).toBuffer();
    rLen = mb.size();

    UCharBuffer b;
    addToBuffer(b, rLen);
    addToBuffer(b, mb);
    _vctRecord.push_back(b);
  }
}

/*!
  Returns the metadata vector
*/
MetaDataVector CMessageStatusRes::getMetaDataVec() const
{
  MetaDataVector vct;
  
  for (uint i=0; i< _vctRecord.size(); i++) {
    uint from = 0;
    uint32_t rLen = 0;

    UCharBuffer b = _vctRecord.at(i);
    CMessage::getOrdData(b, from, rLen);
    from += sizeof(rLen);


    UCharBuffer rb = getBuffer(b, from, (rLen -sizeof(rLen)));

    CMetaData m = CMetaData::load(rb);
    vct.push_back(m);
  }
 
  return vct;
}


/*!
  Returns a new Message from the buffer
*/
CMessageStatusRes* CMessageStatusRes::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint32_t dataLen = getDataLenFromBuffer(b);
  uint32_t msgLen  = HEADER_LEN + dataLen;  

  uint from = HEADER_LEN;
  uint32_t hostInfoLen = 0;
  uint16_t port = 0;
  uint32_t recordLen = 0;
  string hostName;
  
  UCharBuffer requoid = CMessage::getBuffer(b, HEADER_LEN, SHA_DIGEST_LENGTH);

  from += SHA_DIGEST_LENGTH;
  CMessage::getOrdData(b, from, hostInfoLen);

  from += sizeof(hostInfoLen);
  CMessage::getOrdData(b, from, port);

  hostInfoLen -= sizeof(port);
  from += sizeof(port);
  hostName = getString(b, from, hostInfoLen);

  CAutoPtr<CMessageStatusRes> m(
    new CMessageStatusRes(uoid, requoid, port, hostName));

  from += hostName.length();

  if(from <  dataLen) {
    std::vector<UCharBuffer> vctR;
    
    while(from <  msgLen) {
      CMessage::getOrdData(b, from, recordLen);

      UCharBuffer rBuf;
      addToBuffer(rBuf, recordLen);
      from += sizeof(recordLen);

      if (recordLen > 0) {
        addToBuffer(rBuf, getBuffer(b, from, recordLen));
        vctR.push_back(rBuf);
        from += recordLen;
      } else {
        vctR.push_back(rBuf);
      }
    }
    m->_vctRecord = vctR; 
  }

  return m.release();
}

//-----------------------------------------------------------------------------

/*!
  Constructs the Store message
*/
CMessageStore::CMessageStore(
  const UCharBuffer &uoid, uint8_t ttl)
  : CMessage(uoid, ttl)
{
  _isTmpFile = false;
}

/*!
  Deletes the message
*/
CMessageStore::~CMessageStore()
{
  if (_isTmpFile) {
    if (CFile::exists(_filePath)) {
      try {
        CFile::deleteFile(_filePath);
      } catch (CException &e) {
        CUtil::writeError(e.getMessage(), true);
      }
    }
  }
}

/*!
  Clones the object
*/
CMessageStore* CMessageStore::clone() const
{
  CMessageStore *m = new CMessageStore(*this);
  const_cast<CMessageStore*>(this)->_isTmpFile = false;
  return m;
}

/*!
  Returns the type of the message
*/
uint8_t CMessageStore::getType() const
{
  return CConst::TYPE_STORE;
}

/*!
  Returns the string format of message type
*/
string CMessageStore::getTypeStr() const
{
  return CConst::TYPE_STORE_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageStore::getByteLen() const
{
  uint32_t len = 0;

  len  = sizeof(uint32_t); //metadata length
  len += _mData.size();
  len += sizeof(uint32_t); //certificate length
  len += _cData.size();
  len += _fDataSize;

  return len;
}

/*!
  Returns string represntation of the object
*/
string CMessageStore::toString() const
{
  string s = toString_p();
  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageStore::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);

  addToBuffer(b, static_cast<uint32_t>(_mData.size()));
  addToBuffer(b, _mData);
  addToBuffer(b, static_cast<uint32_t>(_cData.size()));
  addToBuffer(b, _cData);

  return b;
}

/*!
  Sets the meta data
*/
void CMessageStore::setMetaData(
  const UCharBuffer &d)
{
  _mData = d;
}

/*!
  Returns the meta data
*/
UCharBuffer CMessageStore::getMetaData() const
{
  return _mData;
}

/*!
  Sets the certificate data
*/
void CMessageStore::setCertData(
  const UCharBuffer &d)
{
  _cData = d;
}

/*!
  Returns the certificate data
*/
UCharBuffer CMessageStore::getCertData() const
{
  return _cData;
}

/*!
  Sets the file data size
*/
void CMessageStore::setFileDataSize(uint32_t size)
{
  _fDataSize = size;
}

/*!
  Returns the file data size
*/
uint32_t CMessageStore::getFileDataSize() const
{
  return _fDataSize;
}

/*!
  Sets the file path where the data is stored
*/
void CMessageStore::setFilePath(const string &f)
{
  _filePath = f;
}

/*!
  Returns the file path where the data is stored
*/
string CMessageStore::getFilePath() const
{
  return _filePath;
}

/*!
  Sets true if the file is temporary file
*/
void CMessageStore::setTmpFile(bool flag)
{
  _isTmpFile = flag;
}

/*!
  Returns true if the file is temp
*/
bool CMessageStore::isTempFile() const
{
  return _isTmpFile;
}

/*!
  Returns a new Message from the buffer
*/
CMessageStore* CMessageStore::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint8_t ttl = getTtlFromBuffer(b);

  CAutoPtr<CMessageStore> m(new CMessageStore(uoid, ttl));
  assert(m.get());

  return m.release();
}

//-----------------------------------------------------------------------------

/*!
  Constructs the Search request message
*/
CMessageSearchReq::CMessageSearchReq(
  const UCharBuffer &uoid, uint8_t ttl,
  uint8_t sType, const string &query)
  : CMessage(uoid, ttl)
{
  _searchType = sType;
  _query = query;
}

/*!
  Deletes the message
*/
CMessageSearchReq::~CMessageSearchReq() {}

/*!
  Clones the object
*/
CMessageSearchReq* CMessageSearchReq::clone() const
{
  CMessageSearchReq *m = new CMessageSearchReq(*this);
  return m;
}

/*!
  Returns the type of the message
*/
uint8_t CMessageSearchReq::getType() const
{
  return CConst::TYPE_SEARCH_REQ;
}

/*!
  Returns the string format of message type
*/
string CMessageSearchReq::getTypeStr() const
{
  return CConst::TYPE_SEARCH_REQ_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageSearchReq::getByteLen() const
{
  uint32_t len = 0;

  len += sizeof(_searchType);
  len += _query.length();

  return len;
}

/*!
  Returns string represntation of the object
*/
string CMessageSearchReq::toString() const
{
  string s = toString_p();
  
  if (CConst::SEARCH_FILENAME == _searchType) {
    s += "filename ";
  } else if (CConst::SEARCH_SHA1 == _searchType) {
    s += "sha1hash ";

  } else if (CConst::SEARCH_KWRDS == _searchType) {
    s += "keywords ";
  }
  s += _query;
  
  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageSearchReq::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);

  addToBuffer(b, _searchType);
  addToBuffer(b, _query);

  return b;
}

/*!
  Returns the search type
*/
uint8_t CMessageSearchReq::getSearchType() const
{
  return _searchType;
}

/*!
  Returns the Query
*/
string CMessageSearchReq::getQuery() const
{
  return _query;
}

/*!
  Returns a new Message from the buffer
*/
CMessageSearchReq* CMessageSearchReq::fromBuffer(
  const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint8_t ttl = getTtlFromBuffer(b);

  uint8_t searchType = b.at(HEADER_LEN);
  uint32_t dataLen = getDataLenFromBuffer(b) - sizeof(searchType);
  uint from = HEADER_LEN + 1;

  CAutoPtr<CMessageSearchReq> m;
  
  string query = getString(b, from, dataLen);
  m = new CMessageSearchReq(uoid, ttl, searchType, query); 

  assert(m.get());
  return m.release();
}

//-----------------------------------------------------------------------------

/*!
  Constructs the search response message
*/
CMessageSearchRes::CMessageSearchRes(
  const UCharBuffer &uoid,
  const UCharBuffer &requoid)
  : CMessage(uoid, 1)
{
  _reqUoid = requoid;
  assert(_reqUoid.size() == SHA_DIGEST_LENGTH);
}

/*!
  Deletes the search message response
*/
CMessageSearchRes::~CMessageSearchRes() {}

/*!
  Clones the object
*/
CMessageSearchRes* CMessageSearchRes::clone() const
{
  return new CMessageSearchRes(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageSearchRes::getType() const
{
  return CConst::TYPE_SEARCH_RES;
}

/*!
  Returns the string format of message type
*/
string CMessageSearchRes::getTypeStr() const
{
  return CConst::TYPE_SEARCH_RES_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageSearchRes::getByteLen() const
{
  uint32_t len = 0;

  len = _reqUoid.size();

  if (_mVec.size() <= 0) {
    C_THROW("CMessageSearchRes: Cannot send response with 0 responses in");
  }
  
  for (uint i=0; i<_mVec.size(); i++) {
     const CMetaData &m = _mVec.at(i);

     len += sizeof(uint32_t);
     len += m.getFileId().size();
     len += m.toBuffer().size();
  }

  return len;
}

/*!
  Returns string represntation of the object
*/
string CMessageSearchRes::toString() const
{
  string s = toString_p();
  s += getFormattedId(_reqUoid);

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageSearchRes::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);

  addToBuffer(b, _reqUoid);
  
  for (uint i=0; i<_mVec.size(); i++) {
    const CMetaData &m = _mVec.at(i);

    const UCharBuffer &fileId = m.getFileId();
    const UCharBuffer &meta = m.toBuffer();

    uint32_t len = fileId.size() + meta.size();

    addToBuffer(b, len);
    addToBuffer(b, fileId);
    addToBuffer(b, meta);
  }

  return b;
}

/*!
  Returns the request uoid
*/
UCharBuffer CMessageSearchRes::getReqUoid() const
{
  return _reqUoid;
}

/*!
  Sets the metadata vector
*/
void CMessageSearchRes::setMetaDataVec(
  const MetaDataVector &v)
{
  _mVec = v;
}

const MetaDataVector& CMessageSearchRes::getMetaDataVec() const
{
  return _mVec;
}

/*!
  Returns a new Message from the buffer
*/
CMessageSearchRes* CMessageSearchRes::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint32_t dataLen = getDataLenFromBuffer(b);

  UCharBuffer requoid = CMessage::getBuffer(b, HEADER_LEN, SHA_DIGEST_LENGTH);
  CAutoPtr<CMessageSearchRes> m(new CMessageSearchRes(uoid, requoid));

  uint from = HEADER_LEN + SHA_DIGEST_LENGTH;
  uint32_t len = 0;
  MetaDataVector v;

  do {
    getOrdData(b, from, len);
    from += sizeof(len);
    
    UCharBuffer fileId = getBuffer(b, from, SHA_DIGEST_LENGTH);
    from += SHA_DIGEST_LENGTH;

    UCharBuffer meta = getBuffer(b, from, (len-SHA_DIGEST_LENGTH));
    CMetaData d = CMetaData::load(meta);
    d.setFileId(fileId);
    from += meta.size();

    v.push_back(d);
    
  } while (from < dataLen);

  m->setMetaDataVec(v);

  return m.release();
}

//-----------------------------------------------------------------------------
/*!
  Constructs the Get request message
*/
CMessageGetReq::CMessageGetReq(
  const UCharBuffer &uoid, uint8_t ttl,
  const UCharBuffer &fileId)
  : CMessage(uoid, ttl)
{
  assert(fileId.size() == SHA_DIGEST_LENGTH);
  _fileId = fileId;
}

/*!
  Deletes the message
*/
CMessageGetReq::~CMessageGetReq() {}

/*!
  Clones the object
*/
CMessageGetReq* CMessageGetReq::clone() const
{
  return new CMessageGetReq(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageGetReq::getType() const
{
  return CConst::TYPE_GET_REQ;
}

/*!
  Returns the string format of message type
*/
string CMessageGetReq::getTypeStr() const
{
  return CConst::TYPE_GET_REQ_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageGetReq::getByteLen() const
{
  return _fileId.size();
}

/*!
  Returns string represntation of the object
*/
string CMessageGetReq::toString() const
{
  string s = toString_p();
  s += getFormattedId(_fileId);

  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageGetReq::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);
  addToBuffer(b, _fileId);

  return b;
}

/*!
  Returns Status Type
*/
UCharBuffer CMessageGetReq::getFileId() const
{
  return _fileId;
}

/*!
  Returns a new Message from the buffer
*/
CMessageGetReq* CMessageGetReq::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint8_t ttl = getTtlFromBuffer(b);
  UCharBuffer fileId = getBuffer(b, HEADER_LEN, SHA_DIGEST_LENGTH);

  CMessageGetReq *m = new CMessageGetReq(uoid, ttl, fileId); assert(m);
  return m;
}

//-----------------------------------------------------------------------------

/*!
  Constructs the Store message
*/
CMessageGetRes::CMessageGetRes(
  const UCharBuffer &uoid) : CMessage(uoid, 1)
{
  _isTmpFile = false;
}

/*!
  Deletes the message
*/
CMessageGetRes::~CMessageGetRes()
{
  if (_isTmpFile) {
    if (CFile::exists(_filePath)) {
      try {
        CFile::deleteFile(_filePath);
      } catch (CException &e) {
        CUtil::writeError(e.getMessage(), true);
      }
    }
  }
}

/*!
  Clones the object
*/
CMessageGetRes* CMessageGetRes::clone() const
{
  CMessageGetRes *m = new CMessageGetRes(*this);
  const_cast<CMessageGetRes*>(this)->_isTmpFile = false;
  return m;
}

/*!
  Returns the type of the message
*/
uint8_t CMessageGetRes::getType() const
{
  return CConst::TYPE_GET_RES;
}

/*!
  Returns the string format of message type
*/
string CMessageGetRes::getTypeStr() const
{
  return CConst::TYPE_GET_RES_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageGetRes::getByteLen() const
{
  uint32_t len = 0;

  len = _reqUoid.size();
  len += sizeof(uint32_t); //metadata length
  len += _mData.size();
  len += sizeof(uint32_t); //certificate length
  len += _cData.size();
  len += _fDataSize;

  return len;
}

/*!
  Returns string represntation of the object
*/
string CMessageGetRes::toString() const
{
  string s = toString_p();
  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageGetRes::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);

  addToBuffer(b, _reqUoid); 
  addToBuffer(b, static_cast<uint32_t>(_mData.size()));
  addToBuffer(b, _mData);
  addToBuffer(b, static_cast<uint32_t>(_cData.size()));
  addToBuffer(b, _cData);

  return b;
}

void CMessageGetRes::setReqUoid(const UCharBuffer &d)
{
  _reqUoid = d;
  assert(_reqUoid.size() == SHA_DIGEST_LENGTH);
}

/*!
  Returns the request uoid
*/
UCharBuffer CMessageGetRes::getReqUoid() const
{
  return _reqUoid;
}

/*!
  Sets the meta data
*/
void CMessageGetRes::setMetaData(
  const UCharBuffer &d)
{
  _mData = d;
}

/*!
  Returns the meta data
*/
UCharBuffer CMessageGetRes::getMetaData() const
{
  return _mData;
}

/*!
  Sets the certificate data
*/
void CMessageGetRes::setCertData(
  const UCharBuffer &d)
{
  _cData = d;
}

/*!
  Returns the certificate data
*/
UCharBuffer CMessageGetRes::getCertData() const
{
  return _cData;
}

/*!
  Sets the file data size
*/
void CMessageGetRes::setFileDataSize(uint32_t size)
{
  _fDataSize = size;
}

/*!
  Returns the file data size
*/
uint32_t CMessageGetRes::getFileDataSize() const
{
  return _fDataSize;
}

/*!
  Sets the file path where the data is stored
*/
void CMessageGetRes::setFilePath(const string &f)
{
  _filePath = f;
}

/*!
  Returns the file path where the data is stored
*/
string CMessageGetRes::getFilePath() const
{
  return _filePath;
}

/*!
  Sets true if the file is temporary file
*/
void CMessageGetRes::setTmpFile(bool flag)
{
  _isTmpFile = flag;
}

/*!
  Returns true if the file is temp
*/
bool CMessageGetRes::isTempFile() const
{
  return _isTmpFile;
}

/*!
  Returns a new Message from the buffer
*/
CMessageGetRes* CMessageGetRes::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  
  CMessageGetRes *m = new CMessageGetRes(uoid);
  assert(m);
  return m;
}

//-----------------------------------------------------------------------------
/*!
  Constructs the delete message
*/
CMessageDelete::CMessageDelete(
  const UCharBuffer &uoid, uint8_t ttl,
  const UCharBuffer &fileSpec)
  : CMessage(uoid, ttl)
{
  _fileSpec = fileSpec;
}

/*!
  Deletes the message
*/
CMessageDelete::~CMessageDelete() {}

/*!
  Clones the object
*/
CMessageDelete* CMessageDelete::clone() const
{
  return new CMessageDelete(*this);
}

/*!
  Returns the type of the message
*/
uint8_t CMessageDelete::getType() const
{
  return CConst::TYPE_DELETE;
}

/*!
  Returns the string format of message type
*/
string CMessageDelete::getTypeStr() const
{
  return CConst::TYPE_DELETE_STR;
}

/*!
  Returns the data length
*/
uint32_t CMessageDelete::getByteLen() const
{
  return _fileSpec.size();
}

/*!
  Returns string represntation of the object
*/
string CMessageDelete::toString() const
{
  string s = toString_p();
  return s;
}

/*!
  Returns the Byte format of the message
*/
UCharBuffer CMessageDelete::toUcharBuffer() const
{
  UCharBuffer b;
  toUcharBuffer_p(b);
  addToBuffer(b, _fileSpec);

  return b;
}

/*!
  Returns Status Type
*/
UCharBuffer CMessageDelete::getFileSpec() const
{
  return _fileSpec;
}

/*!
  Returns a new Message from the buffer
*/
CMessageDelete* CMessageDelete::fromBuffer(const UCharBuffer &b)
{
  UCharBuffer uoid = getUoidFromBuffer(b);
  uint8_t ttl = getTtlFromBuffer(b);
  uint32_t dataLen = getDataLenFromBuffer(b);
  UCharBuffer fileSpec = getBuffer(b, HEADER_LEN, dataLen);

  CMessageDelete *m = new CMessageDelete(uoid, ttl, fileSpec); assert(m);
  return m;
}
