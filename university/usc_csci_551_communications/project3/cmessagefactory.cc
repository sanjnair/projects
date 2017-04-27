/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include "cmessagefactory.h"
#include "cmessage.h"
#include "cexception.h"
#include "ctcpsocket.h"
#include "cconst.h"
#include "cautoptr.h"
#include "csha1data.h"
#include "cfilemgr.h"


/*!
  Constructs the message factory
*/
CMessageFactory::CMessageFactory() {}

/*!
  Deletes the message factory
*/
CMessageFactory::~CMessageFactory() {}

/*!
  Returns a message from socket
*/
CMessage* CMessageFactory::createMsg(CTcpSocket *sockP)
{
  assert(sockP);

  CMessage *messageP = NULL;
  UCharBuffer buf;
  uchar type = 0;
  
  buf = rcvFrom(sockP, CMessage::HEADER_LEN);

  if (! sockP->isStopFlagEnabled() ) {

    type = CMessage::getTypeFromBuffer(buf);
    uint32_t dataLen = CMessage::getDataLenFromBuffer(buf);

    if (type == CConst::TYPE_STORE) {
      messageP = getStoreMsg(buf, dataLen, sockP);
      
    } else if(type == CConst::TYPE_GET_RES) {
      messageP = getGetResMsg(buf, dataLen, sockP);
    
    } else {
      if (dataLen > 0) {
        UCharBuffer dataBuf = rcvFrom(sockP, dataLen);
        CMessage::addToBuffer(buf, dataBuf);
      }

      if (type == CConst::TYPE_HELLO) {
        messageP = CMessageHello::fromBuffer(buf); 
          
      } else if (type == CConst::TYPE_JOIN_REQ) {
        messageP = CMessageJnrq::fromBuffer(buf);
  
      } else if (type == CConst::TYPE_JOIN_RES) {
        messageP = CMessageJnrs::fromBuffer(buf);

      } else if (type == CConst::TYPE_KEEPALIVE) {
        messageP = CMessageKeepAlive::fromBuffer(buf);

      } else if (type == CConst::TYPE_NOTIFY) {
        messageP = CMessageNotify::fromBuffer(buf);

      } else if (type == CConst::TYPE_STATUS_REQ) {
        messageP = CMessageStatusReq::fromBuffer(buf);
    
      } else if (type == CConst::TYPE_STATUS_RES) {
        messageP = CMessageStatusRes::fromBuffer(buf);
    
      } else if (type == CConst::TYPE_CHECK_REQ) {
        messageP = CMessageCheckReq::fromBuffer(buf);
      
      } else if (type == CConst::TYPE_CHECK_RES) {
        messageP = CMessageCheckRes::fromBuffer(buf);
      
      } else if (type == CConst::TYPE_SEARCH_REQ) {
        messageP = CMessageSearchReq::fromBuffer(buf);

      } else if (type == CConst::TYPE_SEARCH_RES) {
        messageP = CMessageSearchRes::fromBuffer(buf);

      } else if (type == CConst::TYPE_GET_REQ) {
        messageP = CMessageGetReq::fromBuffer(buf);

      } else if (type == CConst::TYPE_DELETE) {
        messageP = CMessageDelete::fromBuffer(buf);

      } else {
        string msg = string("Invalid message type 0x");
        msg += CUtil::getHexStr(type);
        msg += string(" data len = ") + CUtil::getString(dataLen);
    
        C_THROW(msg);
      }
    }
  }

  return messageP;
}

/*!
  Receives specified data from TCP socket
*/
UCharBuffer CMessageFactory::rcvFrom(
  CTcpSocket *sockP, uint len)
{
  UCharBuffer buf;
  int size = sockP->receiveFrom(buf, len);
    
  if (! sockP->isStopFlagEnabled() ) {
    if ((uint)size != len ) {
      string msg = "Message size from socket is invalid. Expected = ";
      msg += CUtil::getString(len);
      msg += "actual = ";
      msg += CUtil::getString(size);
      C_THROW(msg);
    }
  }

  return buf;
}

/*!
  Returns the store msg
*/
CMessage* CMessageFactory::getStoreMsg(
  const UCharBuffer &header, uint32_t dataLen,
  CTcpSocket *sockP)
{
  uint32_t fileLen = dataLen;

  CAutoPtr<CMessageStore> mP;
  mP = CMessageStore::fromBuffer(header);

  UCharBuffer buf;
  uint32_t len = 0;

  buf = rcvFrom(sockP, sizeof(len));
  CMessage::getOrdData(buf, 0, len);
  fileLen -= sizeof(len);
  
  buf = rcvFrom(sockP, len);
  mP->setMetaData(buf);
  fileLen -= len;
  
  buf = rcvFrom(sockP, sizeof(len));
  CMessage::getOrdData(buf, 0, len);
  fileLen -= sizeof(len);
  
  buf = rcvFrom(sockP, len);
  mP->setCertData(buf);
  fileLen -= len;

  if (fileLen <= 0) {
    string msg = "Invalid file length ";
    msg += CUtil::getString(msg);
    C_THROW(msg);
  }

  string filePath = writeToTmpFile(sockP, fileLen);
  mP->setFilePath(filePath);
  mP->setFileDataSize(fileLen);
  mP->setTmpFile(true);
  
  return mP.release();
}

/*!
  Returns the store msg
*/
CMessage* CMessageFactory::getGetResMsg(
  const UCharBuffer &header, uint32_t dataLen,
  CTcpSocket *sockP)
{
  uint32_t fileLen = dataLen;

  CAutoPtr<CMessageGetRes> mP;
  mP = CMessageGetRes::fromBuffer(header);

  UCharBuffer buf;
  uint32_t len = 0;

  buf = rcvFrom(sockP, SHA_DIGEST_LENGTH);
  mP->setReqUoid(buf);
  fileLen -= SHA_DIGEST_LENGTH;

  buf = rcvFrom(sockP, sizeof(len));
  CMessage::getOrdData(buf, 0, len);
  fileLen -= sizeof(len);

  buf = rcvFrom(sockP, len);
  mP->setMetaData(buf);
  fileLen -= len;

  buf = rcvFrom(sockP, sizeof(len));
  CMessage::getOrdData(buf, 0, len);
  fileLen -= sizeof(len);


  buf = rcvFrom(sockP, len);
  mP->setCertData(buf);
  fileLen -= len;


  if (fileLen <= 0) {
    string msg = "Invalid file length ";
    msg += CUtil::getString(msg);
    C_THROW(msg);
  }

  string filePath = writeToTmpFile(sockP, fileLen);
  mP->setFilePath(filePath);
  mP->setFileDataSize(fileLen);
  mP->setTmpFile(true);

  return mP.release();
}


/*!
  Reads from the TCP socket some bytes at a time and
  stores it in the file
*/
string CMessageFactory::writeToTmpFile(
  CTcpSocket *sockP, uint32_t fileLen)
{
  uint32_t readSize = 0;
  uint32_t numLeft = fileLen;  

  string fPath;
  int fd = 0;
  CFileMgr::createTmpFile(fd, fPath);

  try {
    do {
      readSize = (numLeft > CConst::SOCK_RW_SIZE) ? CConst::SOCK_RW_SIZE
                                                  : numLeft;

      //printf("receiving %d bytes over socket\n", readSize);
      UCharBuffer b = rcvFrom(sockP, readSize);
      numLeft -= readSize;

      ssize_t size = write(fd, &(b[0]), b.size());
      if (size != static_cast<int>(b.size())) {
        string msg = string("Unable to write ") + CUtil::getString(b.size());
        msg += string(" bytes to file ") + fPath;
        msg += string(" written bytes = ") + CUtil::getString(size);
        C_THROW(msg);
        
      }
    } while (numLeft > 0);

    close(fd);

  } catch (CException &e) {
    close(fd);
    throw e;
  }

  return fPath;
}

//-----------------------------------------------------------------------------
