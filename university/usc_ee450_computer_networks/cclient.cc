#include <iostream>
#include "cclient.h"
#include "cexception.h"
#include "cconstant.h"

/*!
    Constructs the client
*/
CClient::CClient()  : _port(0), _lockKey(0) {}

/*!
    Deletes the client
*/
CClient::~CClient() {}

/*!
    Starts the client
*/
void CClient::start()
{
    CUtil::writeOutputLn("Client");
    CUtil::writeOutputLn("------");

    connectToAs();
}

/*!
    Authenticates with the authentication server
*/
void CClient::connectToAs()
{
    CUtil::writeOutputLn("Attempting to connect to authentication server");
    
    //first try to ping the AS
    CUdpSocket uSock;
    uSock.openSock();

    StringMap m;
    m[CConstant::C_FROM] = CConstant::TYPE_FROM_CLIENT;
    m[CConstant::C_TYPE] = CConstant::TYPE_CLIENT_PING;

    execUdp(uSock, m);
    CUtil::writeOutputLn("Successfully connected to authentication server");


    m.clear();

    string uids = authenticate(uSock);
    //------------------------------------------------------------
    CUtil::writeOutputLn("Authentication is sucesssful!");
    
    if (! isUserExit()) {
        int id = getUserverId(uids);

        m[CConstant::C_FROM]  = CConstant::TYPE_FROM_CLIENT;
        m[CConstant::C_TYPE]  = CConstant::TYPE_CLIENT_U_LIST;
        m[CConstant::C_USERVER_ID] = CUtil::getString(id);

        execUdp(uSock, m);

        _ip = CUtil::getFromMap(m, CConstant::C_IP);
        _port = CUtil::getUInt(CUtil::getFromMap(m, CConstant::C_PORT));
        _lockKey = CUtil::getUInt(CUtil::getFromMap(m, CConstant::C_LOCK_KEY));

        connectToUs();
    }
}

/*!
    Gets the user credentials
*/
string CClient::authenticate(CUdpSocket &uSock)
{
    const int MAX_ATTEMPTS = 3;

    int nAttempts = 1;
    string login;
    string pwd;
    string msg;
    StringMap m;
    bool cont = true;
    
    do {
        msg  = "Authentication: Attempt ";
        msg += CUtil::getString(nAttempts);
        msg += " of ";
        msg += CUtil::getString(MAX_ATTEMPTS);
        msg += "\n--------------------------------";
        CUtil::writeOutputLn(msg);
        
        CUtil::writeOutput("Please enter login id: ");
        login = CUtil::getFromConsole();
        CUtil::writeOutput("Please enter password: ");
        pwd = CUtil::getFromConsole();

        if ((login.length() <= 0) || (pwd.length() <=0)) {
            CUtil::writeOutputLn("Login and password is not valid");

        } else {
            bool bValid = false;
            try {
                CUtil::validateUserInput(login);
                CUtil::validateUserInput(pwd);
                bValid = true;
            } catch (CException &exp) {
                CUtil::writeError(exp.getMessage());
            }

            if (bValid) {
                m[CConstant::C_FROM]  = CConstant::TYPE_FROM_CLIENT;
                m[CConstant::C_TYPE]  = CConstant::TYPE_CLIENT_LOGIN;
                m[CConstant::C_LOGIN] = login;
                m[CConstant::C_PWD]   = pwd;
                execUdp(uSock, m);

                if (CConstant::V_STATUS_OK == CUtil::getFromMap(m, CConstant::C_STATUS)) {
                    msg = CUtil::getFromMap(m, CConstant::C_USERVER_LIST);
                    break;
                } else {
                    CUtil::writeError(CUtil::getFromMap(m, CConstant::C_ERROR_MSG));
                }
            }
        }

        nAttempts++;
        if (nAttempts > MAX_ATTEMPTS) {
            string msg("maximum attempts expired. Restart the application");
            C_THROW(msg);
        } 

        CUtil::writeOutputLn("\n");

    } while(cont);

    return msg;
}

/*!
    Checks the communication error
*/ 
void CClient::checkCommError(const StringMap &m)
{
    string s = CUtil::getFromMap(m, CConstant::C_STATUS);
    if (CConstant::V_STATUS_ERROR == s) {
        s = CUtil::getFromMap(m, CConstant::C_ERROR_MSG);
        C_THROW(s);
    }

}

/*!
    Returns true, if the user want to exit        
*/
bool CClient::isUserExit() const
{
    string msg;
    CUtil::writeOutput("Do you want to exit (y to exit, any other key to continue)? ");
    
    msg = CUtil::getFromConsole();

    if ((string("y") == msg) ||
        (string("Y") == msg)) {
        
        return true;
    }
    CUtil::writeOutputLn("");
    return false;
}


/*!
    Returns the Upload server id, which the user chose
*/
int CClient::getUserverId(const string &s)
{
    std::map<int,int> idMap;

    string sId;
    int id;

    uint lpos = s.find_first_not_of(CConstant::ID_SEP, 0);
    uint pos = s.find_first_of(CConstant::ID_SEP, lpos);

    while (string::npos != pos || string::npos != lpos) {
        sId = s.substr(lpos, pos - lpos);
        id = CUtil::getUInt(sId);
        idMap[id] = id;

        lpos = s.find_first_not_of(CConstant::ID_SEP, pos);
        pos = s.find_first_of(CConstant::ID_SEP, lpos);
    }

    bool cont = true;
    do {
        CUtil::writeOutputLn("Please enter one of the available upload servers: ");
        CUtil::writeOutputLn("----------------------------------------------------\n");
        
        std::map<int,int>::const_iterator it;
        
        for(it = idMap.begin(); it != idMap.end(); it++) {
            string msg = "Enter ";
            msg += CUtil::getString(it->first);
            msg += " To select Upload server ";
            msg += CUtil::getString(it->first);
            CUtil::writeOutputLn(msg);
        }
        
        CUtil::writeOutputLn("");
        sId = CUtil::getFromConsole();
        try {
            id = CUtil::getUInt(sId);
            
            if (idMap.find(id) != idMap.end()) {
                cont = false;
            } else {
                CUtil::writeError("Invalid id selected.");
            }
        } catch(CException &e) {
            CUtil::writeError(e.getMessage());
        }
    } while (cont);

    return id;
}

/*!
    Executes the UDP 
*/
void CClient::execUdp(CUdpSocket &uSock, StringMap &m)
{
    string sIp = uSock.getSystemIp();
    struct sockaddr_in dAddr;

    string c = CUtil::fromProtoMap(m);
    uSock.writeDatagram(c, uSock.ipStrToUlong(sIp), CConstant::ASERVER_PORT);
    c = uSock.readDatagram(dAddr, CConstant::UDP_TIMEOUT);

    m.clear();
    CUtil::toProtoMap(c, m);
    checkCommError(m);
}

/*!
    Connect to the upload server
*/  
void CClient::connectToUs()
{
    CTcpSocket tSock;
    tSock.openSock();
    tSock.connectTo(_ip, _port);

    try {
        initUServConn(tSock);
        sendFile(tSock);
        sendExitMsg(tSock);

    } catch(CException &ex) {
        sendExitMsg(tSock);
        throw ex;
    }
}

/*!
    Initiates the connection to Upload seerver
*/
void CClient::initUServConn(CTcpSocket &tSock)
{

    int lock = _lockKey / CConstant::ASERVER_KEY;

    StringMap m;
    m[CConstant::C_TYPE] = CConstant::TYPE_CLIENT_INIT;
    m[CConstant::C_LOCK] = CUtil::getString(lock);

    execTcp(tSock, m);

    CUtil::writeOutputLn("\nConnection accepted by the Upload server");
    CUtil::writeOutputLn("-----------------------------------------");
}

/*!
    Obtatin the filename from the user and send it to the server
*/
void CClient::sendFile(CTcpSocket &tSock)
{
    string s = getFileFromUser();
    
    StringMap m;
    m[CConstant::C_TYPE] = CConstant::TYPE_FILE_NAME;
    m[CConstant::C_FILE_NAME] = s;

    execTcp(tSock, m);
    
    bool cont = true;
    string status = CUtil::getFromMap(m, CConstant::C_FILE_W_STATUS);

    if (status == CConstant::V_FILE_EXIST) {
        FileChoice choice = getFileChoiseFromUser();

        if(FileAbort == choice) {
            cont = false;                   
        } else {
            m.clear();
            m[CConstant::C_TYPE] = CConstant::C_FILE_W_STATUS;
            if (FileAppend == choice) {
                m[CConstant::C_FILE_W_STATUS] = CConstant::V_FILE_APPEND;
            } else {
                m[CConstant::C_FILE_W_STATUS] = CConstant::V_FILE_OVERWRITE;
            }
            execTcp(tSock, m);
        }
    } 
    
    if (cont) {
        m.clear();
        m[CConstant::C_TYPE] = CConstant::C_FILE_CONTENT;
        m[CConstant::C_FILE_CONTENT] = getFileContentFromUser();
        execTcp(tSock, m);
        CUtil::writeOutputLn("File succesfully transferred. Exitng...");
    }
}

/*!
    Executes the TCP
*/
void CClient::execTcp(CTcpSocket &tSock, StringMap &m, bool waitForResp)
{
    string c = CUtil::fromProtoMap(m);
    tSock.sendTo(c);
    
    if (waitForResp) {
        c = tSock.receiveFrom();
        m.clear();
        CUtil::toProtoMap(c, m);
        checkCommError(m);
    }
}

/*!
    Send exit message to server
*/
void CClient::sendExitMsg(CTcpSocket &tSock)
{
    StringMap m;
    m[CConstant::C_TYPE] = CConstant::TYPE_EXIT;
    execTcp(tSock, m, false);
    tSock.closeSock();
}

/*!
    Get the file content from user
*/
string CClient::getFileFromUser()
{
    string input;
    bool cont = true;
    do {
        CUtil::writeOutput("\nEnter the file name to be transferred: ");
        input = CUtil::getFromConsole();
        CUtil::writeOutputLn("");
        try {
            input = CUtil::trim(input);
            CUtil::validateUserInput(input);
            
            if (input.length() <= 0) {
                CUtil::writeError("Input invalid. Please enter again.");
            } else {
                cont = false;
            }
        } catch(CException &e) {
            CUtil::writeError(e.getMessage());
        }
    } while (cont);

    return input;
}

/*!
    send the file content from the user
*/
string CClient::getFileContentFromUser()
{
    string input;
    bool cont = true;
    do {
        CUtil::writeOutput("Enter the file content: ");
        input = CUtil::getFromConsole(false);
        CUtil::writeOutputLn("");
        try {
            input = CUtil::trim(input);
            CUtil::validateUserInput(input);
            
            if (input.length() <= 0) {
                CUtil::writeError("Input invalid. Please enter again");
            } else {
                cont = false;
            }
        } catch(CException &e) {
            CUtil::writeError(e.getMessage());
        }
    } while (cont);

    return input;    
}

/*!
    Get the user choise on file modification
*/
CClient::FileChoice CClient::getFileChoiseFromUser()
{
    FileChoice choice = FileAbort;
    bool cont = true;
    do {
        string msg("\nSpecified file name exists. Please enter one of the following options\n");
        msg += "--------------------------------------------------------------------------\n";
        msg += "Enter ";
        msg += CUtil::getString(FileOverWrite);
        msg += " to overwrite file\n";
        msg += "Enter ";
        msg += CUtil::getString(FileAppend);
        msg += " to append file\n";
        msg += "Enter ";
        msg += CUtil::getString(FileAbort);
        msg += " to abort file transfer and exit\n";
        
        CUtil::writeOutputLn(msg);

        string input = CUtil::getFromConsole();;
        try {
            CUtil::validateUserInput(input);
            CUtil::trim(input);
            if (input.length() <= 0) {
                CUtil::writeError("Input invalid. Please enter again");
            } else {
                int i = CUtil::getUInt(input);
                
                switch(i) {
                    case FileOverWrite:
                    case FileAppend:
                    case FileAbort:
                        choice = static_cast<FileChoice>(i);
                        cont = false;
                        break;
                }
            }
        } catch(CException &e) {
            CUtil::writeError(e.getMessage());
        }
    } while (cont);

    return choice;
}
