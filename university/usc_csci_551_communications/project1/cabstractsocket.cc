/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/
#include "cexception.h"
#include "cabstractsocket.h"

/*!
    Creates abstract socket
*/
CAbstractSocket::CAbstractSocket(int type)
  : _socket(0), _isOpen(false), _type(type), _boundPort(0) {}

/*!
    Deletes abstract socket and closes the connection
*/
CAbstractSocket::~CAbstractSocket()
{
  closeSock();
}

/*!
    Opens the socket
 */
void CAbstractSocket::openSock()
{
    closeSock();
    int s = socket(AF_INET, _type, 0);
    errorIfInalidCode("unable to get the socket", s);
    _socket = static_cast<in_addr_t>(s);
    _isOpen = true;
}

/*!
    Closes the socket
*/
void CAbstractSocket::closeSock()
{
  if (_isOpen) {
    close(_socket);
    _socket = 0;
    _isOpen = false;
  }
}

/*!
    Returns socket value
*/
in_addr_t  CAbstractSocket::getSocket() const
{
    errorIfNotOpen();
    return _socket;
}

/*!
    Gets the socket type
*/
int CAbstractSocket::getType() const
{
    errorIfNotOpen();
    return _type;
}

/*!
  Private method that binds to the give address (ip) and port
*/
void CAbstractSocket::bindTo_p(
  sockaddr_in &sAddr, uint16_t port)
{
  int status = bind(_socket,(struct sockaddr *) &sAddr, sizeof(sAddr));
  errorIfInalidCode("unable to bind", status);

  struct sockaddr_in tAddr;
  socklen_t tAddrLen = sizeof(tAddr);

  status = getsockname(_socket, (struct sockaddr *) &tAddr, &tAddrLen);
  errorIfInalidCode("unable to get socket info", status);

  _boundPort = getPort(tAddr);
  _boundHost = getIp(tAddr);
} 


/*!
  Bind to all interfaces
*/
void CAbstractSocket::bindTo(uint16_t port)
{
  errorIfNotOpen();
  struct sockaddr_in sAddr;
  initSockAddr(sAddr, INADDR_ANY, port);
  bindTo_p(sAddr, port);
}

/*!
    Binds the socket to given ip and port
*/
void CAbstractSocket::bindTo(const string &ip, uint16_t port)
{
  errorIfNotOpen();
  struct sockaddr_in sAddr;
  initSockAddr(sAddr, ipStrToAddr(ip), port);
  bindTo_p(sAddr, port);
}

/*!
    Returns the IP address given the socket structure
*/
string CAbstractSocket::getIp(const struct sockaddr_in &a)
{
    return inet_ntoa(a.sin_addr);
}

/*!
    Returns the port given the socket structure
*/
uint16_t CAbstractSocket::getPort(const struct sockaddr_in &a)
{
    return ntohs(a.sin_port);
}

/*!
    Returns bounded port number
*/
uint16_t CAbstractSocket::getBoundPort() const
{
    errorIfNotOpen();
    return _boundPort;
}

/*!
    Returns bounded host (IP)
*/
string CAbstractSocket::getBoundHost() const
{
    return _boundHost;
}

/*!
    Throws exception if the code is invalid
*/
void CAbstractSocket::errorIfInalidCode(const string &s, int c)
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

/*!
    Returns the host name
*/
string CAbstractSocket::getHostName()
{
    char hostname[100];
    int status = gethostname(hostname, sizeof(hostname));
    errorIfInalidCode("unable to get host name", status);
    return string(hostname);
}

/*!
  Get IP address for the given host name
*/
string CAbstractSocket::getIp(const string &hostName)
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
string CAbstractSocket::getSystemIp()
{
    return getIp(getHostName());
}

/*!
    Converts IP to integer form
*/
in_addr_t CAbstractSocket::ipStrToAddr(const string &s)
{
  in_addr_t ip = 0;
  inet_pton(AF_INET, s.c_str(), &ip);
  return htonl(ip);
}

/*!
    Throws exception if the socket is not open
*/
void CAbstractSocket::errorIfNotOpen() const
{
    if (! _isOpen) {
        string msg("socket is not opened");
        C_THROW(msg); 
    }
}

/*!
    Initializes socket address structure
*/
void CAbstractSocket::initSockAddr(struct sockaddr_in &sockAddr, in_addr_t addr, uint16_t port)
{
    memset(&sockAddr, 0, sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_addr.s_addr = htonl(addr);
    memset(&(sockAddr.sin_zero), '\0', 8);
}

/*
  Adds the string to buffer
*/
void CAbstractSocket::addToBuffer(CharBuffer &buf, const string &s)
{
  for (uint i=0; i<s.length(); i++) {
    buf.push_back(s[i]);
  }
}

/*!
  Adds the character to buffer
*/
void CAbstractSocket::addToBuffer(CharBuffer &buf, const char c)
{
  buf.push_back(c);
}

/*!
  Adds the number to Buffer.
  If nw is true, then host to network byte order will be
  performed first
*/
void CAbstractSocket::addToBuffer(CharBuffer &buf, uint16_t num, bool nw)
{
  uint16_t t = (nw) ? htons(num) : num;
  unsigned char arr[2];
  memcpy(arr, &t, sizeof(t));

  for (int i=0; i<2; i++) {
    buf.push_back(arr[i]);
  }

  /*
  uint16_t t = (nw) ? htons(num) : num;

  for (uint i=0; i<sizeof(num); i++) {
    uint16_t mask = 0xff << (i*8);
    uint8_t c = static_cast<uint8_t>((t & mask) >> (i*8));
    buf.push_back(c);
  }*/

}

/*!
  Adds the number to Buffer.
  If nw is true, then host to network byte order will be
  performed first
*/
void CAbstractSocket::addToBuffer(CharBuffer &buf, uint32_t num, bool nw)
{
  uint32_t t = (nw) ? htonl(num) : num;
  unsigned char arr[4];
  memcpy(arr, &t, sizeof(t));
  for (int i=0; i<4; i++) {
    buf.push_back(arr[i]);
  }

  /*uint32_t t = (nw) ? htonl(num) : num;
  for (uint i=0; i<sizeof(num); i++) {
    uint32_t mask = 0xff << (i*8);
    uint8_t c = static_cast<uint8_t>((t & mask) >> (i*8));
    buf.push_back(c);
  }*/
}

/*!
  Print the buffer in Both Hex and Ascii format
*/
void CAbstractSocket::printBuffer(const CharBuffer &buf)
{
  if (buf.size() > 0) {
    for (uint i=0; i<buf.size(); i++) {
      printf("%d) 0x%x \t%c\n", i, (uint8_t)buf[i], buf[i]);
    }
  } else {
    CUtil::writeOutputLn("Buffer is empty");
  }
}
