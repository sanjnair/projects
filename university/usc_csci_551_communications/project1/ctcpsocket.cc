/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <assert.h>
#include <signal.h>
#include "ctcpsocket.h"
#include "cutil.h"
#include "cexception.h"

/*!
    Constructs the TCP socket
*/
CTcpSocket::CTcpSocket() : CAbstractSocket(SOCK_STREAM) {}

/*!
    Constructs a TCP connection with already opened socket
*/
CTcpSocket::CTcpSocket(in_addr_t socket) : CAbstractSocket(SOCK_STREAM)
{
    _socket = socket;
    _isOpen = true;
}

/*!
    Deletes the TCP socket
*/
CTcpSocket::~CTcpSocket() {}
  
/*!
  Opens the socket
*/
void CTcpSocket::openSock()
{
  CAbstractSocket::openSock();
}


/*!
    Sends the data
*/
int CTcpSocket::sendTo(const char *data, int maxSize)
{
    errorIfNotOpen();
    signal(SIGPIPE, SIG_IGN);

    int status = send(_socket, data, maxSize, 0);
    errorIfInalidCode("TCP: Unable to send data. Remote socket might be closed.", status);
    return status;
}

/*!
    Sends data
*/
int CTcpSocket::sendTo(const string &str)
{
    return sendTo(str.c_str(), str.length());
}

/*!
  Sends the data one byte at a time
*/
int CTcpSocket::sendTo(const CharBuffer &buf)
{
  int status = 0;
  for (uint i=0; i<buf.size(); i++) {
    char c[1];
    c[0] = buf[i];
    sendTo(c, 1);
  }
  return status;
}

/*!
    Receives data
*/
int CTcpSocket::receiveFrom(char *data, int maxSize, int secDelay)
{
    errorIfNotOpen();
    int status = 0;

    signal(SIGPIPE, SIG_IGN);

    if (0 != secDelay) {
      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(_socket, &readfds);
      int n = _socket + 1;

      struct timeval tv;
      tv.tv_sec = secDelay;
      tv.tv_usec = 0;

      status = select(n, &readfds, NULL, NULL, &tv);

      if (0 == status) {
        string msg("Socket connection timed out after ");
        msg += CUtil::getString(secDelay);
        msg += " seconds.";
        C_THROW(msg);
      }
      errorIfInalidCode("unable to recieve data from TCP port", status);
      status = recv(_socket, data, maxSize, 0);
      errorIfInalidCode("unable to recieve data from TCP port", status);
      
    } else {
      status = recv(_socket, data, maxSize, 0);
      errorIfInalidCode("unable to recieve data from TCP port", status);
    }
    return status;
}

/*!
    Receives data
*/
string CTcpSocket::receiveFrom(int secDelay)
{   
    char buf[SOCK_BUF_SIZE];
    int size = receiveFrom(buf, SOCK_BUF_SIZE-1, secDelay);
    string s(buf, size);
    return s;
}

/*!
  Reads the specified character from socket stream
  one byte at a time.
*/
CharBuffer CTcpSocket::receiveFrom(uint numChars, int secDelay)
{
  CharBuffer buf;
  for (uint i=0; i<numChars; i++) {
    char tmp[1];
    int size = receiveFrom(tmp, 1, secDelay);

    if (1 != size) {
      string msg = string("Unexpected length of data rcvd from socket (");
      msg += CUtil::getString(size);
      msg += " expected was 1). May be the socket is closed by the server.";
      C_THROW(msg);
    }
    buf.push_back(tmp[0]);
  }
  return buf;
}

/*!
  Returns the specified number of characters from the
  socket stream one byte at a time and returns the string.
*/
string CTcpSocket::receiveStrFrom(uint numChars, int secDelay)
{
  string s;
  CharBuffer buf;
  for (uint i=0; i<numChars; i++) {
    buf = receiveFrom(1, secDelay);
    assert(buf.size() > 0);
    s += buf[0];
  }
  return s;
}

/*!
    Connects to given ip and port
*/
void CTcpSocket::connectTo(const string &ip, uint16_t port)
{
    struct sockaddr_in addr;
    initSockAddr(addr, ipStrToAddr(ip), port);
    
    int status = connect(_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr));

    string msg("Unable to connect to host (ip = ");
    msg += ip;
    msg += " ) and port (";
    msg += CUtil::getString(port);
    msg += " ).";

    if (EINTR == errno) {
      msg += "timed out";      
    }
    errorIfInalidCode(msg, status);
}

