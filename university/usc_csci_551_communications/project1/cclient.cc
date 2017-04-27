/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <assert.h>
#include "cclient.h"
#include "cutil.h"
#include "cconst.h"
#include "cmd5data.h"

/*!
    Construts the client
*/

CClient::CClient(
  ReqType type, const string &serverHost, uint16_t port, const string &option)
  : _type(type), _serverHost(serverHost), _port(port), _option(option)
{
}

/*!
  Deletes the client
*/                             
CClient::~CClient() {}

/*!
  Outputs the banner message
*/
void CClient::outputBanner() const {

  string msg  = "\n-----------------------------------------\n";
  msg        += "Client (recv timeout = ";
  msg        += CUtil::getString(CConst::RECV_TIMEOUT);
  msg        += "s).";
  msg        += "\n-----------------------------------------\n";
  CUtil::writeOutputLn(msg);
}

/*!
  Connects the remote server and performs the operation
*/
void CClient::connect()
{
  outputBanner();
  
  string serverIp = CAbstractSocket::getIp(_serverHost);
  CTcpSocket sock;
  sock.openSock();
  sock.connectTo(serverIp, _port);

  switch(_type) {
    case ReqAddr:
      handleAddrReq(sock);    
      break;
    case ReqFileSize:
      handleFileSizeReq(sock);    
      break;
    case ReqGet:
      handleGetReq(sock);
      break;
  }
}

/*!
  Sends the request
*/
void CClient::sendRequest(CTcpSocket &sock, uint16_t type)
{
  CharBuffer buf;
  uint32_t dataSize = _option.size();
  CAbstractSocket::addToBuffer(buf, type);
  CAbstractSocket::addToBuffer(buf, dataSize);
  CAbstractSocket::addToBuffer(buf, _option);

  sock.sendTo(buf);
}

/*!
  Handles the address request
*/
void CClient::handleAddrReq(CTcpSocket &sock)
{
  sendRequest(sock, CConst::ADR_REQ);

  uint16_t reply = 0;
  uint32_t dataSize = 0;
  getHeaderData(sock, reply, dataSize);

  if ((reply != CConst::ADR_RPLY) && (reply != CConst::ADR_FAIL) &&
      (reply != CConst::ALL_FAIL))
  {
    string msg = string("Invalid reply code received ");
    msg += CUtil::getString(reply);
    C_THROW(msg);
  }

  string output;

  if (CConst::ADR_RPLY == reply) {
    if (0 == dataSize) {
      C_THROW("Invalid data size (0) received for Address request");
    }

    string sAddr = sock.receiveStrFrom(dataSize, CConst::RECV_TIMEOUT);
    output = string("ADDR = ") + sAddr;
  } else {
    output = string("ADDR request for '") + _option;
    output += string("' failed");
  }
  writeOutput(output);
}

/*!
  Handles the File Size request
*/
void CClient::handleFileSizeReq(CTcpSocket &sock)
{
  sendRequest(sock, CConst::FSZ_REQ);
  uint16_t reply = 0;
  uint32_t dataSize = 0;
  getHeaderData(sock, reply, dataSize);

  if ((reply != CConst::FSZ_RPLY) && (reply != CConst::FSZ_FAIL) &&
      (reply != CConst::ALL_FAIL))
  {
    string msg = string("Invalid reply code received ");
    msg += CUtil::getString(reply);
    C_THROW(msg);
  }

  string output;
  if (CConst::FSZ_RPLY == reply) {
    if (0 == dataSize) {
      C_THROW("Invalid data size (0) received for FileSize request");
    }

    string sAddr = sock.receiveStrFrom(dataSize, CConst::RECV_TIMEOUT);
    output = string("FILESIZE = ") + sAddr;
  } else {
    output = string("FILESIZE request for '") + _option;
    output += string("' failed");
  }
  writeOutput(output);
}

/*!
  Handles the getrequest
*/
void CClient::handleGetReq(CTcpSocket &sock)
{
  sendRequest(sock, CConst::GET_REQ);

  uint16_t reply = 0;
  uint32_t dataSize = 0;
  getHeaderData(sock, reply, dataSize);

  if ((reply != CConst::GET_RPLY) && (reply != CConst::GET_FAIL) &&
      (reply != CConst::ALL_FAIL))
  {
    string msg = string("Invalid reply code received ");
    msg += CUtil::getString(reply);
    C_THROW(msg);
  }

  string output;
  if (CConst::GET_RPLY == reply) {
    CMd5Data md5data;
    uint fileSize = 0;

    if (dataSize > 0) {
      for (uint i=0; i<dataSize; i++) {
        CharBuffer buf = sock.receiveFrom(1, CConst::RECV_TIMEOUT);
        unsigned char  uBuf[1];
        uBuf[0] = static_cast<unsigned char>(buf[0]);
        md5data.update(uBuf, 1);
        fileSize++;
      }
    } else {
      unsigned char  uBuf[0];
      md5data.update(uBuf, 0);
    }
    md5data.final();

    if(dataSize != fileSize) {
      output = string("Error: received size (") + CUtil::getString(fileSize);
      output = string(") does not match data size") + CUtil::getString(dataSize);
      output = string(") returned by server.");
    } else {
      output = string("FILESIZE = ") + CUtil::getString(fileSize);
      output += string(", MD5 = ") + md5data.getHexString();
    }
  } else {
    output = string("GET request for '") + _option;
    output += string("' failed");
  }
  writeOutput(output);
}

/*!
  Returns the header data
*/
void CClient::getHeaderData(
  CTcpSocket &sock, uint16_t &reply, uint32_t &size) const
{
  CharBuffer buffer;

  buffer = sock.receiveFrom(2,CConst::RECV_TIMEOUT);
  CAbstractSocket::getFromBuffer(buffer, reply);
  errorIfInvalidReply(reply);

  buffer = sock.receiveFrom(4,CConst::RECV_TIMEOUT);
  CAbstractSocket::getFromBuffer(buffer, size);
}

/*!
  Throws exception if the reply is invalid
*/
void CClient::errorIfInvalidReply(uint16_t reply) const
{
  if ((reply != CConst::ADR_RPLY) && (reply != CConst::FSZ_RPLY) &&
      (reply != CConst::GET_RPLY) && (reply != CConst::ADR_FAIL) &&
      (reply != CConst::FSZ_FAIL) && (reply != CConst::GET_FAIL) &&
      (reply != CConst::ALL_FAIL)) {

      string msg = string("Invalid reply code ");
      msg += CUtil::getString(reply);
      C_THROW(msg);
  }
}

/*!
  Write to output
*/
void CClient::writeOutput(const string &s) const
{
  string str = string("\t") + s;
  CUtil::writeOutputLn(str);
}
