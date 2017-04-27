#include "ctcpsocket.h"
#include "cutil.h"
#include "cexception.h"

/*!
    Constructs the TCP socket
*/
CTcpSocket::CTcpSocket() 
    : CAbstractSocket(SOCK_STREAM), _listener(0), _stopAccept(false) {}

/*!
    Constructs a TCP connection with already opened socket
*/
CTcpSocket::CTcpSocket(int socket) 
    : CAbstractSocket(SOCK_STREAM), _listener(0), _stopAccept(false) 
{
    _socket = socket;
}

/*!
    Deletes the TCP socket
*/
CTcpSocket::~CTcpSocket() {}

/*!
    Opens the TCP server
*/
void CTcpSocket::openServer()
{
    openSock();
    
    #if defined WIN32
    char yes = '\0001';
    #else
        int yes = 1;
    #endif        

    int status = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    errorIfInalidCode("unable set socket options", status);
}

/*!
    Server starts listening
*/
void CTcpSocket::startListen()
{
    errorIfNotOpen();

    int status = listen(_socket, BACKLOG);
    errorIfInalidCode("unable to listen on a TCP port", status);
}
   
/*!
    Server accepts connection from clients
*/
void CTcpSocket::acceptFrom(ITcpListener *listener)
{
    errorIfNotOpen();
    _listener = listener;
    
    struct sockaddr_in dAddr;
    memset(&dAddr, 0, sizeof(dAddr));
    int dSocket = 0;
    
    #if defined WIN32
      int addrSize = sizeof(sockaddr_in);
    #else
      socklen_t addrSize = sizeof(sockaddr_in);
    #endif

    if( ! _stopAccept) {
        do {
            CUtil::writeOutputLn("Waiting for clients to connect...");
            CUtil::writeOutputLn("-----------------------------");

            dSocket = accept(_socket, (struct sockaddr *)&dAddr, &addrSize);
            errorIfInalidCode("Accept returned invalid socket", dSocket);
            
            _listener->notifyConnect(dSocket, dAddr);
            
            #if ! defined WIN32
              CTcpSocket t(dSocket);
              t.closeSock();
            #endif

        } while(! _stopAccept);
    }
}

/*!
    Sends the data
*/
int CTcpSocket::sendTo(const char *data, int maxSize)
{
    errorIfNotOpen();

    int status = send(_socket, data, maxSize, 0);
    errorIfInalidCode("unable to send data using TCP", status);
    return status;

}

/*!
    Sends data
*/
int CTcpSocket::sendTo(const string &str)
{
    errorIfNotOpen();
    return sendTo(str.c_str(), str.length());
    
}

/*!
    Receives data
*/
int CTcpSocket::receiveFrom(char *data, int maxSize)
{
    errorIfNotOpen();
    int status = recv(_socket, data, maxSize, 0);
    errorIfInalidCode("unable to recieve data from TCP port", status);
    return status;
}

/*!
    Receives data
*/
string CTcpSocket::receiveFrom()
{   
    errorIfNotOpen();

    char buf[SOCK_BUF_SIZE];
    int size = receiveFrom(buf, SOCK_BUF_SIZE-1);
    string s(buf, size);
    return s;
}

/*!
    Connects to given ip and port
*/
void CTcpSocket::connectTo(const string &ip, int port)
{
    struct sockaddr_in addr;
    initSockAddr(addr, ipStrToUlong(ip), port);

    int status = connect(_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr));

    string msg("Unable to connect to host (ip = ");
    msg += ip;
    msg += " ) and port (";
    msg += CUtil::getString(port);
    msg += " ).";
    errorIfInalidCode(msg, status);
}

