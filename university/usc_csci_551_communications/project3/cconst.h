/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cglobal.h"

class CConst {

public:
  static const string INIT_NBORFILE;

  static const uint8_t TYPE_JOIN_REQ;
  static const uint8_t TYPE_JOIN_RES;
  static const uint8_t TYPE_HELLO;
  static const uint8_t TYPE_KEEPALIVE;
  static const uint8_t TYPE_NOTIFY;
  static const uint8_t TYPE_CHECK_REQ;
  static const uint8_t TYPE_CHECK_RES;
  static const uint8_t TYPE_SEARCH_REQ;
  static const uint8_t TYPE_SEARCH_RES;
  static const uint8_t TYPE_GET_REQ;
  static const uint8_t TYPE_GET_RES;
  static const uint8_t TYPE_STORE;
  static const uint8_t TYPE_DELETE;
  static const uint8_t TYPE_STATUS_REQ;
  static const uint8_t TYPE_STATUS_RES;

  static const string TYPE_JOIN_REQ_STR;
  static const string TYPE_JOIN_RES_STR;
  static const string TYPE_HELLO_STR;
  static const string TYPE_KEEPALIVE_STR;
  static const string TYPE_NOTIFY_STR;
  static const string TYPE_CHECK_REQ_STR;
  static const string TYPE_CHECK_RES_STR;
  static const string TYPE_SEARCH_REQ_STR;
  static const string TYPE_SEARCH_RES_STR;
  static const string TYPE_GET_REQ_STR;
  static const string TYPE_GET_RES_STR;
  static const string TYPE_STORE_STR;
  static const string TYPE_DELETE_STR;
  static const string TYPE_STATUS_REQ_STR;
  static const string TYPE_STATUS_RES_STR;

  static const uint8_t NOTIFY_UNKNOWN;
  static const uint8_t NOTIFY_SHUTDOWN;
  static const uint8_t NOTIFY_UNEXPECTED;

  static const uint8_t STATUS_NEIGHBORS;
  static const uint8_t STATUS_FILES;  

  static const uint8_t SEARCH_FILENAME;
  static const uint8_t SEARCH_SHA1;
  static const uint8_t SEARCH_KWRDS;

  static const string OBJ_TYPE_MSG;
  static const string OBJ_TYPE_FILE;
  static const string SEP_NODE;

  static const string CMD_SHUTDOWN;
  static const string CMD_STATUS;
  static const string CMD_STORE;
  static const string CMD_SEARCH;
  static const string CMD_GET;
  static const string CMD_DELETE;
  static const string VAL_NEIGHBORS;
  static const string VAL_FILES;

  static const string OPTION_SEARCH_FILENAME;
  static const string OPTION_SEARCH_SHA1;
  static const string OPTION_SEARCH_KWRDS;

  static const string NEWLINE;
  static const string CRLF;
  static const string DOUBLE_QUOTE;
  static const string NVP_SEP;
  static const string COMMA_SEP;
  static const string SPACE_SEP;

  static const uint SOCK_RW_SIZE;

private:
  CConst();
};

