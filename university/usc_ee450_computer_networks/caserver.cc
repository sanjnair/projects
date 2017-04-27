#include "caserver.h"
#include "cutil.h"
#include "cexception.h"
#include "cconstant.h"
#include "cfile.h"
#include "cifstream.h"


/*!
    Constructs the authentication  server
 */
CAServer::CAServer() : _stop(false) {}


/*!
    Deletes the authentication  server
 */
CAServer::~CAServer() {}

/*!
    Starts the authentication  server
 */
void CAServer::start()
{
    CUtil::writeOutputLn("Authentication Server");
    CUtil::writeOutputLn("---------------------");
    
    initLoginInfo();
    bind_p();
    _stop = false;
}

/*!
    Stops the authentication  server
 */
void CAServer::stop()
{
    _stop = true;        
}

/*!
    Initializes the login info
*/
void CAServer::initLoginInfo() {
    
    string msg = "Reading the login information file \"";
    msg += CConstant::LOGIN_FILE;
    msg += "\".\n";
    CUtil::writeOutputLn(msg);
    
    if (! CFile::exists(CConstant::LOGIN_FILE)) {
        msg = "Unable to open login file ";
        msg += CConstant::LOGIN_FILE;
        msg += " for reading. Make sure the file exists";
        msg += " and permissions are correct.";

        C_THROW(msg);
    }
    
    CFile file(CConstant::LOGIN_FILE);
    if (file.size() <= 0) {
        msg = "Invalid login file. ";
        msg += CConstant::LOGIN_FILE;
        msg += ". No records found.";
    }

    CIfStream s(&file);
    s.open();

    try {
        while (s.isGood() && (! s.isEof())) {
            string str = CUtil::trim(s.readLine());
            if (str.length() > 0) {
                processLoginLine(str);
            }
        }
    } catch(CException &e) {
        s.close();
        throw e;
    }
    s.close();

    if (_loginMap.size() <=0) {
        string msg("No login info found in the file");
        C_THROW(msg); 
    }

}

/*!
    Process the login line
*/
void CAServer::processLoginLine(const string &line)
{
    
    string name;
    string val;

    CUtil::getNvp(line, name, val);
    CUtil::validateUserInput(name);
    CUtil::validateUserInput(val);

    _loginMap[name] = val;
}

/*!
    Binds the socket and waits for incoming connection.
*/
void CAServer::bind_p() 
{
    CUdpSocket sock;    
    sock.openSock();
    string strIp = sock.getSystemIp();
    ulong ip = sock.ipStrToUlong(strIp);

    sock.bindTo(ip, CConstant::ASERVER_PORT);
    
    string msg("Bound to host ");
    msg.append(strIp);
    msg.append(" and port ");
    msg.append(CUtil::getString(sock.getBoundPort()));    
    CUtil::writeOutputLn(msg);

    CUtil::writeOutputLn("Waiting for requests...");
    CUtil::writeOutputLn("------------------------\n");

    do {
        struct sockaddr_in dAddr;
        
        try {
            string s = sock.readDatagram(dAddr);
            processReq(sock, dAddr, s);
        } catch (CException &e) {
            CUtil::writeError(e.toString());
        }
    } while(! _stop);
}

/*!
    Process the request
*/
void CAServer::processReq(CUdpSocket &sock, struct sockaddr_in &addr, string s)
{
    try {
        CUtil::writeRecdMsg(sock.getIpAddress(addr), sock.getPort(addr));
        
        StringMap m;
        CUtil::toProtoMap(s, m);
        
        string from = CUtil::getFromMap(m, CConstant::C_FROM);
        if (from == CConstant::V_FROM_USERVER) {
            processUServerReq(sock, addr, m);
        } else if(from == CConstant::TYPE_FROM_CLIENT) {
            processClientReq(sock, addr, m);
        } else {
            C_THROW("From field not found in the data received from receiver");
        }
    } catch (CException &e) {
        sendErrorMsg(sock, addr, e.getMessage());
        CUtil::writeError(e.toString());
    }
}

/*!
    Process the upload request
*/
void CAServer::processUServerReq(CUdpSocket &sock, struct sockaddr_in &addr, StringMap &m)
{
    CUServerData data;

    data._id    = CUtil::getFromMap(m, CConstant::C_USERVER_ID);
    data._ip    = CUtil::getFromMap(m, CConstant::C_IP);
    data._port  = CUtil::getFromMap(m, CConstant::C_PORT);
    data._lock  = CUtil::getFromMap(m, CConstant::C_LOCK);

    _uServerMap[data._id] = data;

    sendOkMsg(sock, addr);
}

/*!
    Process the client request
*/
void CAServer::processClientReq(CUdpSocket &sock, struct sockaddr_in &addr, StringMap &m)
{
    string type = CUtil::getFromMap(m, CConstant::C_TYPE);

    if (type == CConstant::TYPE_CLIENT_PING) {
        sendOkMsg(sock, addr);

    } else if (type == CConstant::TYPE_CLIENT_LOGIN) {
        verifyLogin(sock, addr, m);

    } else if (type == CConstant::TYPE_CLIENT_U_LIST) {
        processUServerIdReq(sock, addr, m);

    } else {
        string msg("Invalid type specified: ");
        msg += type;
        sendErrorMsg(sock, addr, msg);
    }
}

/*!
    Verifies the login and password
*/
void CAServer::verifyLogin(CUdpSocket &sock, struct sockaddr_in &addr, StringMap &m)
{
    string login = CUtil::getFromMap(m, CConstant::C_LOGIN);
    string pwd = CUtil::getFromMap(m, CConstant::C_PWD);

    bool error = true;

    if (_loginMap.find(login) != _loginMap.end()) {
        if (_loginMap[login] == pwd) {
            error = false;
        }
    } 
    m.clear();

    if (error) {
        string msg("unable to authenticate user: ");
        msg += login;
        
        m[CConstant::C_STATUS] = CConstant::V_LOGIN_ERROR;
        m[CConstant::C_ERROR_MSG] = msg;
        sendMsg(sock, addr, CUtil::fromProtoMap(m));

    } else {
        if (_uServerMap.size() <= 0) {
            string msg("No upload servers registered.");
            sendErrorMsg(sock, addr, msg);

        } else {
            string s;
            std::map<string, CUServerData>::iterator iter;
            uint index = 0;

            for (iter = _uServerMap.begin(); iter != _uServerMap.end(); iter++) {
                s += iter->first;
                index++;

                if(index < _uServerMap.size()) {
                    s += CConstant::ID_SEP;
                }
            }
            StringMap m;
            m[CConstant::C_STATUS] = CConstant::V_STATUS_OK;
            m[CConstant::C_USERVER_LIST] =  s;
            sendMsg(sock, addr, CUtil::fromProtoMap(m));
        }
    }
}

/*!
    Process the userver request from client
 */
void CAServer::processUServerIdReq(CUdpSocket &sock, struct sockaddr_in &addr, StringMap &m)
{
    
    int id = CUtil::getUInt(CUtil::getFromMap(m, CConstant::C_USERVER_ID));
    string sid = CUtil::getString(id);

    if (_uServerMap.find(sid) != _uServerMap.end()) {
        const CUServerData &data = _uServerMap[sid];
        int lockKey = CUtil::getUInt(data._lock) * CConstant::ASERVER_KEY;

        StringMap m;
        m[CConstant::C_STATUS] = CConstant::V_STATUS_OK;
        m[CConstant::C_USERVER_ID] = data._id;
        m[CConstant::C_IP] = data._ip;
        m[CConstant::C_PORT] = data._port;
        m[CConstant::C_LOCK_KEY] = CUtil::getString(lockKey);
        
        sendMsg(sock, addr, CUtil::fromProtoMap(m));
   
    } else {
       string msg("Unablet to find upload server entry with id ");
       msg += sid;
       sendErrorMsg(sock, addr, msg);
    }
}

/*!
    Send the message back to the caller
*/
void CAServer::sendMsg(CUdpSocket &sock, struct sockaddr_in &addr, const string &cmd)
{
    try {
        sock.writeDatagram(cmd, addr);
    } catch (CException &e) {
        CUtil::writeError(e.toString());
    }
}

/*!
    Send the message back to the caller

*/
void CAServer::sendOkMsg(CUdpSocket &sock, struct sockaddr_in &addr)
{
    StringMap m;
    
    m[CConstant::C_STATUS] = CConstant::V_STATUS_OK;
    sendMsg(sock, addr, CUtil::fromProtoMap(m));
}


/*!
    Sends the error message
 */
void CAServer::sendErrorMsg(CUdpSocket &sock, struct sockaddr_in &addr, const string &msg)
{
    StringMap m;
    
    m[CConstant::C_STATUS] = CConstant::V_STATUS_ERROR;
    m[CConstant::C_ERROR_MSG] = msg;

    sendMsg(sock, addr, CUtil::fromProtoMap(m));
}

