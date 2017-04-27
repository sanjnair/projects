/**
 * Utilities
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CASERVER_H__
#define __CASERVER_H__

#if defined WIN32
    #pragma warning(disable:4786)
#endif

#include "cglobal.h"
#include "cutil.h"
#include "cudpsocket.h"

#if defined WIN32
    #pragma warning(disable:4786)
#endif

#include <map>
typedef std::map<string, string> StringMap;

class CUServerData
{
public:
    string _id;
    string _ip;
    string _port;
    string _lock;
};

class CAServer {

public:
    CAServer();
    ~CAServer();

    void start();
    void stop();

private:
    void initLoginInfo();
    void processLoginLine(const string &line);
    void bind_p();
    
    void sendMsg(CUdpSocket &sock, struct sockaddr_in &addr, const string &cmd); 
    void sendOkMsg(CUdpSocket &sock, struct sockaddr_in &addr); 
    void sendErrorMsg(CUdpSocket &sock, struct sockaddr_in &addr, const string &msg); 

    void processReq(CUdpSocket &sock, struct sockaddr_in &addr, string s);
    void processUServerReq(CUdpSocket &sock, struct sockaddr_in &addr, StringMap &m);
    void processClientReq(CUdpSocket &sock, struct sockaddr_in &addr, StringMap &m);
    void verifyLogin(CUdpSocket &sock, struct sockaddr_in &addr, StringMap &m);
    void processUServerIdReq(CUdpSocket &sock, struct sockaddr_in &addr, StringMap &m);

    
    bool _stop;
    StringMap _loginMap;
    std::map<string, CUServerData> _uServerMap;
};



#endif //__CASERVER_H__

