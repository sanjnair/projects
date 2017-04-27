/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include "ctcpsocket.h"
#include "cutil.h"
#include "cexception.h"
#include "cmutexlocker.h"

/*!
    Constructs the TCP socket
*/
CTcpSocket::CTcpSocket()
{
  _isOpen = false;
  _isConnected = false;
  _stopFlag = false;
  _localPort = 0;
  _remotePort = 0;
}

/*!
    Deletes the TCP socket
*/
CTcpSocket::~CTcpSocket()
{
  closeSock();
}

/*!
  Sets the stop flag so that recv can lookup
*/
void CTcpSocket::stop()
{
  {
    CMutexLocker m(&_mutex);
    _stopFlag = true;
  }
  closeSock();
}

/*!
  Returns true if the stop flag is enabled
*/
bool CTcpSocket::isStopFlagEnabled()
{
  CMutexLocker m(&_mutex);
  return _stopFlag;
}

/*!
    Opens the socket
 */
void CTcpSocket::openSock()
{
  closeSock();
  int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  errorIfInalidCode("unable to get the socket", s);

  //int reuseAddr = 1;
  //setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void*)(&reuseAddr), sizeof(int));

  _socket = static_cast<in_addr_t>(s);
  _isOpen = true;
}

/*!
    Closes the socket
*/
void CTcpSocket::closeSock()
{
  if (_isOpen) {
    close(_socket);
    _socket = 0;
    _isOpen = false;
    _isConnected = false;
    _remotePort = 0;
    _remoteHost.clear();
  }
}

/*!
  Returns true if the socket is opened
*/
bool CTcpSocket::isOpened() const
{
  return _isOpen;
}

/*!
    Returns socket value
*/
in_addr_t  CTcpSocket::getSocket() const
{
    errorIfNotOpen();
    return _socket;
}

/*!
  Initializes the data structure for already
  connected socket.
*/
void CTcpSocket::connectTo(
      in_addr_t socket, const struct sockaddr_in &dAddr)
{
  _socket = socket;
  _sockAddr = dAddr;
  _isOpen = true;

  _localPort = getPort(getSockName());
  _isConnected = true;
}

/*!
    Connects to given ip and port
*/
void CTcpSocket::connectTo(const string &host, uint16_t port)
{

  string ip = getIp(host);

  initSockAddr(_sockAddr, ipStrToAddr(ip), port);
  int status = connect(
                  _socket,
                  (struct sockaddr *)&_sockAddr,
                  sizeof(struct sockaddr));

    string msg("Unable to connect to host (ip = ");
    msg += ip + string(" ) and port (");
    msg += CUtil::getString(port) + string(" ).");

    if (EINTR == errno) {
      msg += "timed out";
    }
    errorIfInalidCode(msg, status);

    _localPort = getPort(getSockName());
    _remotePort = port;
    _remoteHost = host;
    _isConnected = true;
}

/*!
  Returns true if the socket is connected
*/
bool CTcpSocket::isConnected() const
{
  return _isConnected;
}

/*!
  Returns the Local port
*/
uint16_t CTcpSocket::getLocalPort() const
{
  return _localPort;
}

/*!
  Returns the Remote port
*/
uint16_t CTcpSocket::getRemotePort() const
{
  errorIfNotConnected();
  return _remotePort;
}

/*!
  Returns the Remote Host
*/
string CTcpSocket::getRemoteHost() const
{
  errorIfNotConnected();
  return _remoteHost;
}

/*!
  Returns peer host name
*/
string CTcpSocket::getPeerHostName() const
{
  struct sockaddr_in addr;
  socklen_t t= 0;

  getPeerNameStruct(&addr, &t);
  return getHostName(addr);
}

/*!
  Returns peer port
*/
uint16_t CTcpSocket::getPeerPort() const
{
  struct sockaddr_in addr;
  socklen_t t= 0;

  getPeerNameStruct(&addr, &t);
  return getPort(addr);

}

/*!
  Returns the peer name structure
*/
void CTcpSocket::getPeerNameStruct(struct sockaddr_in *addr, socklen_t *t) const
{
  int status = getpeername(_socket, (struct sockaddr*)addr, t);
  string msg = "Unable to get the peer port";
  errorIfInalidCode(msg, status);
}
//-----------------------------------------------------------------------------

/*!
  Sends the data on to the socket
*/
int CTcpSocket::sendTo(const UCharBuffer &buf)
{
  errorIfNotOpen();

  int status = 0;
  status = send(_socket, &buf[0], buf.size(), 0);

  errorIfInalidCode("TCP: Unable to send data. Remote socket might be closed.", status);
  if ((uint)status != buf.size()) {
    string msg = string("Unable to send ") + CUtil::getString(buf.size());
    msg += string(" bytes to socket. Sent bytes = ") + CUtil::getString(status);
    C_THROW(msg);
  }
  return status;
}

/*!
    Receives data
*/
int CTcpSocket::receiveFrom(uchar *data, int maxSize)
{
  if (maxSize <= 0) {
    C_THROW("TcpSocket: Max Size must be greater than 0");
  }

  errorIfNotOpen();
  int status = 0;
  
  bool bContinue = true;
  while(bContinue)  {

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_socket, &readfds);
    int n = _socket + 1;
    struct timeval tv;
    tv.tv_usec = 100 * 1000;
    tv.tv_sec = 0;

    status = select(n, &readfds, NULL, NULL, &tv);
    
    if (isStopFlagEnabled()) {
      bContinue = false;
    }

    if (bContinue) {
      errorIfInalidCode("unable to recieve data from socket (select)", status);

      if (0 != status) {
        //if (FD_ISSET(_socket, &readfds)) //for mulitple sockets

        status = recv(_socket, data, maxSize, 0);
        
        errorIfInalidCode("unable to recieve data from socket (recv)", status);

        if (status == maxSize) {
          bContinue = false; //exit the while loop
          
        } else {
          string msg = string("Unable to extract ") + CUtil::getString(maxSize);
          msg += string(" from socket. Bytes received is ") + CUtil::getString(status);
          C_THROW(msg);
        } 
      }
   }
  }

  return status;
}

/*!
  Receives the number of requested bytes from the socket
*/
int CTcpSocket::receiveFrom(
    UCharBuffer &buf, int maxSize)
{
  buf.clear();
  buf.resize(maxSize);
  return receiveFrom(reinterpret_cast<uchar*>(&(buf[0])), maxSize);
}
 
//-----------------------------------------------------------------------------

/*!
  get the socket name for the locally bound socket
*/
sockaddr_in CTcpSocket::getSockName() const
{
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);;
  
  int status = getsockname(_socket, (sockaddr*)&addr, &len);
  errorIfInalidCode("unable to get socket information", status);

  return addr;
}

/*!
    Throws exception if the socket is not open
*/
void CTcpSocket::errorIfNotOpen() const
{
  if (! _isOpen) {
    string msg = "socket is not opened";
    C_THROW(msg);
  }
}

/*!
    Throws exception if the socket is not
    connected to a remote socket
*/
void CTcpSocket::errorIfNotConnected()const
{
    if (! _isConnected) {
        string msg("socket is not Connected");
        C_THROW(msg);
    }
}

//-----------------------------------------------------------------------------

/*!
    Initializes socket address structure
*/
void CTcpSocket::initSockAddr(
      struct sockaddr_in &sockAddr, in_addr_t addr, uint16_t port)
{
    memset(&sockAddr, 0, sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_addr.s_addr = htonl(addr);
    memset(&(sockAddr.sin_zero), '\0', 8);
}

/*!
  Returns the host name for the given address structure
*/
string CTcpSocket::getHostName(const struct sockaddr_in &a)
{
  struct hostent *hp;
  hp = gethostbyaddr(
          (char*)(&a.sin_addr.s_addr),
          sizeof(a.sin_addr.s_addr),
          AF_INET);

  if (NULL == hp) {
    C_THROW("Unable to get the hostname from socket structure");
  }
  string host = hp->h_name;

  if (host.length() <=0 ){
    C_THROW("Invalid hostname got from socket structure (0 length)");
  }

  return host;
}

/*!
    Returns the IP address given the socket structure
*/
string CTcpSocket::getIp(const struct sockaddr_in &a)
{
    return inet_ntoa(a.sin_addr);
}

/*!
    Returns the port given the socket structure
*/
uint16_t CTcpSocket::getPort(const struct sockaddr_in &a)
{
    return ntohs(a.sin_port);
}

/*!
    Returns the host name
*/
string CTcpSocket::getHostName()
{
    char hostname[100];
    int status = gethostname(hostname, sizeof(hostname));
    errorIfInalidCode("unable to get host name", status);
    return string(hostname);
}

/*!
  Get IP address for the given host name
*/
string CTcpSocket::getIp(const string &hostName)
{
    bool error = false;
    string s;

    struct hostent *ht = gethostbyname(hostName.c_str());
    if ((ht == NULL) || (ht->h_addr_list == NULL)) {
      error = true;
    } else {
      s = inet_ntoa(*((struct in_addr *)ht->h_addr_list[0]));

      if (s.length() <=0) {
        error = true;
      }
    }

    if (error) {
      string msg("Unable to get the IP address for host name ");
      msg += hostName;
      C_THROW(msg);
    }

    return s;
}

/*!
    Returns the system ip
*/
string CTcpSocket::getSystemIp()
{
    return getIp(getHostName());
}

/*!
    Converts IP to integer form
*/
in_addr_t CTcpSocket::ipStrToAddr(const string &s)
{
  in_addr_t ip = 0;
  inet_pton(AF_INET, s.c_str(), &ip);
  return htonl(ip);
}

/*!
    Throws exception if the code is invalid
*/
void CTcpSocket::errorIfInalidCode(const string &s, int c)
{
    if (c < 0) {
        string msg(s);
        msg += " [code: ";
        msg += CUtil::getString(c);
        msg += ", error no: ";
        msg += CUtil::getString(errno);
        msg += "].";
        C_THROW(msg);
    }
}

