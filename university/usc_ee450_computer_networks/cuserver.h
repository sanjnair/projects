/**
 * Utilities
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CUSERVER_H__
#define __CUSERVER_H__

#include "cglobal.h"
#include "ctcpsocket.h"
#include "cutil.h"

class CUServer : public ITcpListener {

public:
    CUServer(uint id);
    virtual ~CUServer();

    void start();
    virtual void notifyConnect(int newSocket, struct sockaddr_in &addr);

private:
    void startListening();
    void connectToAs(CTcpSocket *tSock);
    void newConnection(int newSocket);

    void verifyLock(StringMap &m);
    void setFileName(StringMap &m);
    void setFileWStatus(StringMap &m);
    void setFileContent(StringMap &m);


    void cleanup();
    void sendOkMsg(); 
    void sendErrorMsg(const string &msg);

    struct sockaddr_in _addr;
    int _id;
    CTcpSocket *_tSock;
    string _fileName;
    string _fileWStatus;
    bool _fileExist;

    
    static const int USERVER_LOCK;
};

#endif //__CUSERVER_H__

