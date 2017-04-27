/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CMESSAGE_H
#define CMESSAGE_H

#include "cglobal.h"
#include "cutil.h"
#include "cexception.h"
#include "cnodedata.h"
#include "cmetadata.h"

class CMessage
{
public:
  CMessage(const UCharBuffer &uoid, uint8_t ttl);
  virtual ~CMessage();

  virtual CMessage *clone() const = 0;
  
  virtual uint8_t getType() const = 0;
  virtual string getTypeStr() const = 0;
  virtual string toString() const = 0;
  virtual UCharBuffer toUcharBuffer() const = 0;

  UCharBuffer getUoid() const;
  uint8_t getTtl() const;
  void decrTtl();
    
  void printBuffer() const;
  static void printBuffer(const UCharBuffer &buf);
  static UCharBuffer getNewUoid(const string &nodeInstId, const string &objType);
  static uint8_t getTypeFromBuffer(const UCharBuffer &b);
  static uint8_t getTtlFromBuffer(const UCharBuffer &b);
  static UCharBuffer getUoidFromBuffer(const UCharBuffer &b);
  static uint32_t getDataLenFromBuffer(const UCharBuffer &b);

  static void addToBuffer(UCharBuffer &b, uint8_t num);
  static void addToBuffer(UCharBuffer &b, uint16_t num);
  static void addToBuffer(UCharBuffer &b, uint32_t num);
  static void addToBuffer(UCharBuffer &b, const string &str);
  static void addToBuffer(UCharBuffer &b, const UCharBuffer &b1);
  static UCharBuffer getBuffer(const UCharBuffer &b, uint from, uint nBytes);
  static string getString(const UCharBuffer &b, uint from, uint nBytes);
  static void getOrdData(const UCharBuffer &b, uint from, uint32_t &t);
  static void getOrdData(const UCharBuffer &b, uint from, uint16_t &t);

  template<typename T>
  static T getOrdData(const UCharBuffer &b, uint from, uint nBytes);
  static void errorIfInvalidDataReq(const UCharBuffer &b, uint from, uint nBytes);

  static const uint32_t HEADER_LEN;
protected:
  virtual uint32_t getByteLen() const = 0;
  string toString_p() const;
  void toUcharBuffer_p(UCharBuffer &b) const;
  string getFormattedId(const UCharBuffer &b) const;

  static void errorIfInvalidBuffer(const UCharBuffer &b);
  
  UCharBuffer _uoid;
  uint8_t _ttl;
};

/*!
  Returns the ordinal data from the given buffer
*/
template<typename T>
T CMessage::getOrdData(const UCharBuffer &b, uint from, uint nBytes)
{
  T t = 0;

  if (nBytes > 0) {
    errorIfInvalidDataReq(b, from, nBytes);

    if ((from + nBytes) > sizeof(T)) {
      string msg;
      msg += "Data size is less than requested. Data size = ";
      msg += CUtil::getString(sizeof(T));
      msg += string(" From  = ") + CUtil::getString(from);
      msg += string(" nBytes  = ") + CUtil::getString(nBytes);
    }

    memcpy(&t, &b[from], nBytes);
  }

  return t;
}

//-----------------------------------------------------------------------------
class CMessageJnrq : public CMessage
{
public:
  CMessageJnrq(
    const UCharBuffer &uoid, uint8_t ttl,
    uint32_t hostLoc, uint16_t port, string hostname);

  virtual ~CMessageJnrq();
  CMessageJnrq *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;
  uint32_t getHostLoc() const;
  uint16_t getPort() const;
  string getHostName() const;

  static CMessageJnrq *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:

  uint32_t _hostLoc;
  uint16_t _port;
  string _hostname;
};

//-----------------------------------------------------------------------------
class CMessageJnrs : public CMessage
{
public:
  CMessageJnrs(
    const UCharBuffer &uoid,
    const UCharBuffer &requoid, uint32_t distance,
    uint16_t port, string hostname);

  virtual ~CMessageJnrs();
  CMessageJnrs *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  UCharBuffer getReqUoid() const;
  uint32_t getDistance() const;
  uint16_t getPort() const;
  string getHostName() const;

  static CMessageJnrs *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  UCharBuffer _reqUoid;
  uint32_t _distance;
  uint16_t _port;
  string _hostname;
};

//-----------------------------------------------------------------------------
class CMessageHello : public CMessage
{
public:
  CMessageHello(
    const UCharBuffer &uoid,
    uint16_t port, string hostname);

  virtual ~CMessageHello();
  CMessageHello *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  uint16_t getPort() const;
  string getHostName() const;

  static CMessageHello *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  uint16_t _port;
  string _hostname;
};

//-----------------------------------------------------------------------------
class CMessageKeepAlive : public CMessage
{
public:
  CMessageKeepAlive(const UCharBuffer &uoid);

  virtual ~CMessageKeepAlive();
  CMessageKeepAlive *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  static CMessageKeepAlive *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
};

//-----------------------------------------------------------------------------
class CMessageNotify : public CMessage
{
public:
  CMessageNotify(
    const UCharBuffer &uoid, uint8_t errorCode);

  virtual ~CMessageNotify();
  CMessageNotify *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  uint8_t getErrorCode() const;

  static CMessageNotify *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  uint8_t _errorCode;
};

//-----------------------------------------------------------------------------
class CMessageCheckReq : public CMessage
{
public:
  CMessageCheckReq(
    const UCharBuffer &uoid, uint8_t ttl);

  virtual ~CMessageCheckReq();
  CMessageCheckReq *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  static CMessageCheckReq *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
};

//-----------------------------------------------------------------------------
class CMessageCheckRes : public CMessage
{
public:
  CMessageCheckRes(
    const UCharBuffer &uoid,
    const UCharBuffer &requoid);

  virtual ~CMessageCheckRes();
  CMessageCheckRes *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  UCharBuffer getReqUoid() const;

  static CMessageCheckRes *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  UCharBuffer _reqUoid;
};

//-----------------------------------------------------------------------------
class CMessageStatusReq : public CMessage
{
public:
  CMessageStatusReq(
    const UCharBuffer &uoid, uint8_t ttl,
    uint8_t statusType);

  virtual ~CMessageStatusReq();
  CMessageStatusReq *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  uint8_t getStatusType() const;

  static CMessageStatusReq *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  uint8_t _statusType;
};

//-----------------------------------------------------------------------------
class CMessageStatusRes : public CMessage
{
public:
  CMessageStatusRes(
    const UCharBuffer &uoid,
    const UCharBuffer &requoid,
    uint16_t port, string hostname);

  virtual ~CMessageStatusRes();
  CMessageStatusRes *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  UCharBuffer getReqUoid() const;
  string getHostName() const;
  uint16_t getPort() const;

  void setNeighborList(const NodeDataList &list);
  NodeDataList getNeighborList() const;
  void setMetaDataVec(const MetaDataVector &v);
  MetaDataVector getMetaDataVec() const;
  
  static CMessageStatusRes *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  UCharBuffer _reqUoid;
  string _hostname;
  uint16_t _port;
  std::vector<UCharBuffer> _vctRecord;
};

//-----------------------------------------------------------------------------
class CMessageStore : public CMessage
{
public:
  CMessageStore(
    const UCharBuffer &uoid, uint8_t ttl);

  virtual ~CMessageStore();
  CMessageStore *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  void setMetaData(const UCharBuffer &d);
  UCharBuffer getMetaData() const;
  void setCertData(const UCharBuffer &d);
  UCharBuffer getCertData() const;
  void setFileDataSize(uint32_t size);
  uint32_t getFileDataSize() const;
  void setFilePath(const string &f);
  string getFilePath() const;
  void setTmpFile(bool flag);
  bool isTempFile() const;      

  static CMessageStore *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  UCharBuffer _mData;
  UCharBuffer _cData;
  uint32_t _fDataSize;
  string _filePath;
  bool _isTmpFile;
};

//-----------------------------------------------------------------------------
class CMessageSearchReq : public CMessage
{
public:
  CMessageSearchReq(
    const UCharBuffer &uoid, uint8_t ttl,
    uint8_t sType, const string &query);

  virtual ~CMessageSearchReq();
  CMessageSearchReq *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  uint8_t getSearchType() const;
  string getQuery() const;
  
  static CMessageSearchReq *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  uint8_t _searchType;
  string _query;
};

//-----------------------------------------------------------------------------
class CMessageSearchRes : public CMessage
{
public:
  CMessageSearchRes(
    const UCharBuffer &uoid,
    const UCharBuffer &requoid);

  virtual ~CMessageSearchRes();
  CMessageSearchRes *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  UCharBuffer getReqUoid() const;
  void setMetaDataVec(const MetaDataVector &v);
  const MetaDataVector &getMetaDataVec() const;

  static CMessageSearchRes *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  UCharBuffer _reqUoid;
  MetaDataVector _mVec;
};

//-----------------------------------------------------------------------------
class CMessageGetReq : public CMessage
{
public:
  CMessageGetReq(
    const UCharBuffer &uoid, uint8_t ttl, const UCharBuffer &fileId);

  virtual ~CMessageGetReq();
  CMessageGetReq *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  UCharBuffer getFileId() const;

  static CMessageGetReq *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  UCharBuffer  _fileId;
};

//-----------------------------------------------------------------------------
class CMessageGetRes : public CMessage
{
public:
  CMessageGetRes(const UCharBuffer &uoid);

  virtual ~CMessageGetRes();
  CMessageGetRes *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  void setReqUoid(const UCharBuffer &d);
  UCharBuffer getReqUoid() const;
  void setMetaData(const UCharBuffer &d);
  UCharBuffer getMetaData() const;
  void setCertData(const UCharBuffer &d);
  UCharBuffer getCertData() const;
  void setFileDataSize(uint32_t size);
  uint32_t getFileDataSize() const;
  void setFilePath(const string &f);
  string getFilePath() const;
  void setTmpFile(bool flag);
  bool isTempFile() const;

  static CMessageGetRes *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  UCharBuffer _reqUoid;
  UCharBuffer _mData;
  UCharBuffer _cData;
  uint32_t _fDataSize;
  string _filePath;
  bool _isTmpFile;
};

//-----------------------------------------------------------------------------
class CMessageDelete : public CMessage
{
public:
  CMessageDelete(
    const UCharBuffer &uoid, uint8_t ttl, const UCharBuffer &fileSpec);

  virtual ~CMessageDelete();
  CMessageDelete *clone() const;

  uint8_t getType() const;
  string getTypeStr() const;
  string toString() const;
  UCharBuffer toUcharBuffer() const;

  UCharBuffer getFileSpec() const;

  static CMessageDelete *fromBuffer(const UCharBuffer &b);

protected:
  uint32_t getByteLen() const;

private:
  UCharBuffer  _fileSpec;
};

#endif //CMESSAGE_H
