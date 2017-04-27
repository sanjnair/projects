#include "cutil.h"
#include "cexception.h"
#include "cabstractsocket.h"

/*!
    Creates abstract socket
*/
CAbstractSocket::CAbstractSocket(int type) : _socket(-1), _type(type), _boundPort(0) {}

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

    #ifdef WIN32
        WSADATA wsaData;
        WORD version;
        version = MAKEWORD( 2, 0 );
        int status = WSAStartup(version, &wsaData);
        
        if (0 != status) {
            errorIfInalidCode("WSAStartup", -1);
        }

    #endif
    _socket = socket(AF_INET, _type, 0);
    errorIfInalidCode("unable to get the socket", _socket);


}

/*!
    Closes the socket
*/
void CAbstractSocket::closeSock()
{
    if (_socket >= 0) {
        #ifdef WIN32
            closesocket(_socket);
            WSACleanup();
        #else
            close(_socket);
        #endif
        
        _socket = -1;        
    }
}

/*!
    Returns socket value
*/
int CAbstractSocket::getSocket() const
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
    Binds the socket to given ip and port
*/
void CAbstractSocket::bindTo(ulong addr, int port)
{
    errorIfNotOpen();

    struct sockaddr_in _sAddr;
    initSockAddr(_sAddr, addr, port);

    int status = bind(_socket,(struct sockaddr *) &_sAddr, sizeof(_sAddr));
    errorIfInalidCode("unable to bind", status);

    struct sockaddr_in tAddr;


    #if defined WIN32
      int tAddrLen = sizeof(tAddr);
    #else
      socklen_t tAddrLen = sizeof(tAddr);
    #endif
    
    status = getsockname(_socket, (struct sockaddr *) &tAddr, &tAddrLen);
    errorIfInalidCode("unable to get socket info", status);

    _boundPort = getPort(tAddr);
    _boundHost = getIpAddress(tAddr);
}

/*!
    Returns the IP address given the socket structure
*/
string CAbstractSocket::getIpAddress(const struct sockaddr_in &a)
{
    return inet_ntoa(a.sin_addr);
}

/*!
    Returns the port given the socket structure
*/
int CAbstractSocket::getPort(const struct sockaddr_in &a)
{
    return ntohs(a.sin_port);
}

/*!
    Returns bounded port number
*/
int CAbstractSocket::getBoundPort() const
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
void CAbstractSocket::errorIfInalidCode(const string &s, int c) const
{
    if (c <0) {
        string msg(s);
        msg += ". Code: ";
        msg += CUtil::getString(c);
        msg += ". Error No: ";
        msg += CUtil::getString(errno);
        C_THROW(msg); 
    }
}

/*!
    Returns the system ip
*/
string CAbstractSocket::getSystemIp()
{
    errorIfNotOpen();

    char hostname[100];
    int status = gethostname(hostname, sizeof(hostname));
    errorIfInalidCode("unable to get host name", status); 
                                                                        
    struct hostent *ht = gethostbyname(hostname);
    if (! ht) {
        errorIfInalidCode("unable to get ip address for the host", -1); 
    }
    
    string s = inet_ntoa(*((struct in_addr *)ht->h_addr_list[0]));
    
    if (s.length() <=0) {
        string msg("Unable to get the IP address of the local machine");
        C_THROW(msg);
    }

    return s;
}

/*!
    Converts IP to Long
*/
ulong CAbstractSocket::ipStrToUlong(const string &s)
{
    ulong ip = inet_addr(s.c_str());
    return htonl(ip);
}

/*!
    Throws exception if the socket is not open
*/
void CAbstractSocket::errorIfNotOpen() const
{
    if (_socket < 0) {
        string msg("socket is not opened");
        C_THROW(msg); 
    }
}

/*!
    Initializes socket address structure
*/
void CAbstractSocket::initSockAddr(struct sockaddr_in &sockAddr, ulong addr, int port)
{
    memset(&sockAddr, 0, sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_addr.s_addr = htonl(addr);
    memset(&(sockAddr.sin_zero), '\0', 8);
}

