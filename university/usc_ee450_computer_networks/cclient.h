/**
 * Utilities
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CCLIENT_H__
#define __CCLIENT_H__

#include "cglobal.h"
#include "cutil.h"
#include "cudpsocket.h"
#include "ctcpsocket.h"

class CClient {

public:
    CClient();
    ~CClient();

    void start();

private:

    enum FileChoice {
        FileOverWrite = 1,
        FileAppend,
        FileAbort
    };

    void connectToAs();
    string authenticate(CUdpSocket &uSock);
    void checkCommError(const StringMap &m);
    bool isUserExit() const;
    int getUserverId(const string &s);
    void execUdp(CUdpSocket &uSock, StringMap &m);
    void sendExitMsg(CTcpSocket &tSock);

    void connectToUs();
    void initUServConn(CTcpSocket &tSock);
    void sendFile(CTcpSocket &tSock);
    void execTcp(CTcpSocket &tSock, StringMap &m, bool waitForResp=true);
    string getFileFromUser();
    string getFileContentFromUser();
    FileChoice getFileChoiseFromUser();
    

    string _ip;
    int _port;
    int _lockKey;

};

#endif //__CCLIENT_H__

