/**
 * Constant file
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CCONSTANT_H__
#define __CCONSTANT_H__

#include "cglobal.h"

class CConstant {

public:
    static const string LOGIN_FILE;
    static const int ASERVER_PORT;
    static const int ASERVER_KEY;
    static const int UDP_TIMEOUT;

    static const string C_FROM;
    static const string C_USERVER_ID;
    static const string C_TYPE;
    static const string C_PORT;
    static const string C_IP;
    static const string C_LOCK;
    static const string C_LOCK_KEY;
    static const string C_STATUS;
    static const string C_ERROR_MSG;
    static const string C_LOGIN;
    static const string C_PWD;
    static const string C_USERVER_LIST;
    static const string C_FILE_NAME;
    static const string C_FILE_W_STATUS;
    static const string C_FILE_CONTENT;

    static const string TYPE_CLIENT_PING;
    static const string TYPE_CLIENT_LOGIN;
    static const string TYPE_CLIENT_U_LIST;
    static const string TYPE_FROM_CLIENT;
    static const string TYPE_CLIENT_INIT;
    static const string TYPE_FILE_NAME;
    static const string TYPE_FILE_CONTENT;
    static const string TYPE_EXIT;
    
    static const string V_FROM_USERVER;
    static const string V_STATUS_OK;
    static const string V_STATUS_ERROR;
    static const string V_LOGIN_ERROR;
    
    static const string V_FILE_NOT_EXIST;
    static const string V_FILE_EXIST;
    static const string V_FILE_OVERWRITE;
    static const string V_FILE_APPEND;
    
    
    static const string SEP;
    static const string NV_SEP;
    static const string ID_SEP;

private:
    /**
     * Constructor is private
     */
    CConstant();
};

#endif //__CCONSTANT_H__

