/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cconst.h"

/*!
    Private constructor
*/
CConst::CConst() {}

const string CConst::INIT_NBORFILE            = "init_neighbor_list";

const uint8_t CConst::TYPE_JOIN_REQ           = 0xFC;
const uint8_t CConst::TYPE_JOIN_RES           = 0XFB;
const uint8_t CConst::TYPE_HELLO              = 0xFA;
const uint8_t CConst::TYPE_KEEPALIVE          = 0xF8;
const uint8_t CConst::TYPE_NOTIFY             = 0xF7;
const uint8_t CConst::TYPE_CHECK_REQ          = 0xF6;
const uint8_t CConst::TYPE_CHECK_RES          = 0xF5;
const uint8_t CConst::TYPE_SEARCH_REQ         = 0xEC;
const uint8_t CConst::TYPE_SEARCH_RES         = 0xEB;
const uint8_t CConst::TYPE_GET_REQ            = 0xDC;
const uint8_t CConst::TYPE_GET_RES            = 0xDB;
const uint8_t CConst::TYPE_STORE              = 0xCC;
const uint8_t CConst::TYPE_DELETE             = 0xBC;
const uint8_t CConst::TYPE_STATUS_REQ         = 0xAC;
const uint8_t CConst::TYPE_STATUS_RES         = 0xAB;

const string CConst::TYPE_JOIN_REQ_STR        = "JNRQ";
const string CConst::TYPE_JOIN_RES_STR        = "JNRS";
const string CConst::TYPE_HELLO_STR           = "HLLO";
const string CConst::TYPE_KEEPALIVE_STR       = "KPAV";
const string CConst::TYPE_NOTIFY_STR          = "NTFY";
const string CConst::TYPE_CHECK_REQ_STR       = "CKRQ";
const string CConst::TYPE_CHECK_RES_STR       = "CKRS";
const string CConst::TYPE_SEARCH_REQ_STR      = "SHRQ";
const string CConst::TYPE_SEARCH_RES_STR      = "SHRS";
const string CConst::TYPE_GET_REQ_STR         = "GTRQ";
const string CConst::TYPE_GET_RES_STR         = "GTRS";
const string CConst::TYPE_STORE_STR           = "STOR";
const string CConst::TYPE_DELETE_STR          = "DELT";
const string CConst::TYPE_STATUS_REQ_STR      = "STRQ";
const string CConst::TYPE_STATUS_RES_STR      = "STRS";

const string CConst::OBJ_TYPE_MSG             = "msg";
const string CConst::OBJ_TYPE_FILE            = "file";
const string CConst::SEP_NODE                 = ":";

const uint8_t CConst::NOTIFY_UNKNOWN          = 0x0;
const uint8_t CConst::NOTIFY_SHUTDOWN         = 0x1;
const uint8_t CConst::NOTIFY_UNEXPECTED       = 0x2;

const uint8_t CConst::STATUS_NEIGHBORS        = 0x01;
const uint8_t CConst::STATUS_FILES            = 0x02;

const uint8_t CConst::SEARCH_FILENAME         = 0x01;
const uint8_t CConst::SEARCH_SHA1             = 0x02;
const uint8_t CConst::SEARCH_KWRDS            = 0x03;

const string CConst::CMD_SHUTDOWN             = "shutdown";
const string CConst::CMD_STATUS               = "status";
const string CConst::CMD_STORE                = "store";
const string CConst::CMD_SEARCH               = "search";
const string CConst::CMD_GET                  = "get";
const string CConst::CMD_DELETE               = "delete";
const string CConst::VAL_NEIGHBORS            = "neighbors";
const string CConst::VAL_FILES                = "files";

const string CConst::OPTION_SEARCH_FILENAME   = "filename";
const string CConst::OPTION_SEARCH_SHA1       = "sha1hash";
const string CConst::OPTION_SEARCH_KWRDS      = "keywords";

const string CConst::NEWLINE                  = "\n";
const string CConst::CRLF                     = "\r\n";
const string CConst::DOUBLE_QUOTE             = "\"";
const string CConst::NVP_SEP                  = "=";
const string CConst::COMMA_SEP                = ",";
const string CConst::SPACE_SEP                = " ";

const uint CConst::SOCK_RW_SIZE               = 1024;
