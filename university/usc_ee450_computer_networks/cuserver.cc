#include "cuserver.h"
#include "cexception.h"
#include "cconstant.h"
#include "cfile.h"
#include "cifstream.h"
#include "cudpsocket.h"
#include "cofstream.h"


/*!
    Constructs the Upload server
*/
CUServer::CUServer(uint id) : _id(id), _tSock(0), _fileExist(0) {}

/*!
    Deletes the Upload server
*/
CUServer::~CUServer() 
{
    cleanup();
}

/*!
    Starts the server
*/
void CUServer::start()
{
    
    string msg("Upload Server (#");
    msg += CUtil::getString(_id);
    msg += ")";
    CUtil::writeOutputLn(msg);
    CUtil::writeOutputLn("-----------------------");
    
    startListening();
}

/*!
    Cleans up server
 */
void CUServer::cleanup()
{
    if (_tSock) {
        delete _tSock;
        _tSock = NULL;
    }
}

/*!
    Starts listening 
*/
void CUServer::startListening()
{
    _tSock = new CTcpSocket();
    _tSock->openSock();
    
    string strIp = _tSock->getSystemIp();
    ulong ip = _tSock->ipStrToUlong(strIp);
    _tSock->bindTo(ip, 0);
    
    string msg("Bound to address ");
    msg.append(_tSock->getBoundHost());
    msg.append(" and port ");
    msg.append(CUtil::getString(_tSock->getBoundPort()));
    CUtil::writeOutputLn(msg);
    CUtil::writeOutputLn("----------------------------\n");
    
    CUtil::writeOutputLn("Contacting the authentication server");
    connectToAs(_tSock);
    CUtil::writeOutputLn("Successfully contacted the authentication server");
    
    _tSock->startListen();
    _tSock->acceptFrom(this);
}

/*!
    Connects to the authentication server.
*/
void CUServer::connectToAs(CTcpSocket *_tSock)
{
    CUdpSocket uSock;
    uSock.openSock();
    string sIp = _tSock->getSystemIp();

    StringMap m;
    m[CConstant::C_FROM] = CConstant::V_FROM_USERVER;
    m[CConstant::C_USERVER_ID] = CUtil::getString(_id);
    m[CConstant::C_PORT] = CUtil::getString(_tSock->getBoundPort());
    m[CConstant::C_IP] = sIp;
    m[CConstant::C_LOCK] = CUtil::getString(USERVER_LOCK);
    
    string c = CUtil::fromProtoMap(m);
    uSock.writeDatagram(c, _tSock->ipStrToUlong(sIp), CConstant::ASERVER_PORT);
    
    struct sockaddr_in dAddr;
    string msg = uSock.readDatagram(dAddr, CConstant::UDP_TIMEOUT);

    m.clear();
    CUtil::toProtoMap(msg, m);

    string s = CUtil::getFromMap(m, CConstant::C_STATUS);
    if (CConstant::V_STATUS_ERROR == s) {
        msg = CUtil::getFromMap(m, CConstant::C_ERROR_MSG);
        C_THROW(msg);
    }
}   

/*!
    Notifies the connection information
*/ 
void CUServer::notifyConnect(int newSocket, struct sockaddr_in &addr)
{
    CUtil::writeRecdMsg(_tSock->getIpAddress(addr), _tSock->getPort(addr));
    
    #if ! defined WIN32
    if (! fork()) {
        cleanup();
    #endif 
    
        _addr = addr;
        newConnection(newSocket);

    #if ! defined WIN32
    }
    #endif 
}

/*!
    Manages the new connection.
*/
void CUServer::newConnection(int newSocket)
{
    
    _tSock = new CTcpSocket(newSocket);
    bool bExit = false;
    
    try {
        do {
            string s = _tSock->receiveFrom();            
            
            StringMap m;
            CUtil::toProtoMap(s, m);
            
            string type = CUtil::getFromMap(m, CConstant::C_TYPE);

            if (CConstant::TYPE_EXIT == type) {
                bExit = true;

            } else if (CConstant::TYPE_CLIENT_INIT == type) {
                verifyLock(m);

            } else if(CConstant::TYPE_FILE_NAME == type) {
                setFileName(m);

            } else if(CConstant::C_FILE_W_STATUS == type) {
                setFileWStatus(m);
            
            } else if(CConstant::C_FILE_CONTENT == type) {
                setFileContent(m);

            } else {
                string msg("Invalid type specified: ");
                msg += type;
                sendErrorMsg(msg);
            }

        } while(! bExit);
    } catch (CException &e) {
        bExit = true;
        CUtil::writeError(e.getMessage());
    }

    if(bExit) {
        string msg("\nClosing connection. (IP = ");
        msg += _tSock->getIpAddress(_addr);
        msg += " Port = ";
        msg += CUtil::getString(_tSock->getPort(_addr));
        msg += " )\n";
        
        CUtil::writeOutputLn(msg);

        #if ! defined WIN32
        _tSock->closeSock();
        exit(0);
        #endif
    }
}

/*!
    Verify the lock 
*/  
void CUServer::verifyLock(StringMap &m)
{
    int lock = CUtil::getUInt(CUtil::getFromMap(m, CConstant::C_LOCK));
    if (USERVER_LOCK == lock) {
        sendOkMsg();
    } else {
        sendErrorMsg("Connection Denied by the Upload server");
    }
}

/*!
    Sets the file name. If file exists, this method
    will ask the user to approve append / overwrite.
*/
void CUServer::setFileName(StringMap &m)
{
    _fileName = CUtil::getFromMap(m, CConstant::C_FILE_NAME);
    _fileWStatus = "";
    
    _fileExist = CFile::exists(_fileName);
     m.clear();
     m[CConstant::C_FILE_W_STATUS] = _fileExist ? CConstant::V_FILE_EXIST 
                                                : CConstant::V_FILE_NOT_EXIST;
     m[CConstant::C_STATUS] = CConstant::V_STATUS_OK;

    _tSock->sendTo(CUtil::fromProtoMap(m));
}

/*!
    Sets the file write status
*/
void CUServer::setFileWStatus(StringMap &m)
{
    _fileWStatus = CUtil::getFromMap(m, CConstant::C_FILE_W_STATUS);

    if (_fileName.length() > 0) {
        sendOkMsg();
    } else {
        sendErrorMsg("Filename is not set");
    }
}

/*!
    Sets the file content
*/
void CUServer::setFileContent(StringMap &m)
{
    string content = CUtil::getFromMap(m, CConstant::C_FILE_CONTENT);
    string errorMsg;

    if (_fileName.length() <= 0) {
        errorMsg = "Filename is not set";

    } else if( (_fileWStatus.length() > 0) == _fileExist) {
        std::ios_base::openmode mode = std::ifstream::out;
        
        if (_fileWStatus == CConstant::V_FILE_APPEND) {
            mode |= std::ifstream::app;
        } else {
            mode |= std::ifstream::trunc;
        }

        CFile f(_fileName);
        COfStream os(&f);
        os.open(mode);
        
        try {
            os.write(content);
            os.close();
            sendOkMsg();
        
        } catch (CException &e) {
            errorMsg = e.getMessage();
            os.close();
        }   
    } else {
        errorMsg = "File write status is not set";
    }
    
    if (errorMsg.length() > 0) {
        sendErrorMsg(errorMsg);
    }
}

/*!
    Send the OK message
*/
void CUServer::sendOkMsg()
{
    StringMap m;
    m[CConstant::C_STATUS] = CConstant::V_STATUS_OK;
    
    _tSock->sendTo(CUtil::fromProtoMap(m));
}

/*!
    Send the error message
*/
void CUServer::sendErrorMsg(const string &msg)
{
    StringMap m;
    m[CConstant::C_STATUS] = CConstant::V_STATUS_ERROR;
    m[CConstant::C_ERROR_MSG] = msg;
    
    _tSock->sendTo(CUtil::fromProtoMap(m));
}

//---------------------------------------------
const int CUServer::USERVER_LOCK = 20;

