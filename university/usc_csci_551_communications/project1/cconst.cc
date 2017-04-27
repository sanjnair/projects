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

const uint16_t CConst::DEF_SERVER_PORT    = 13212;
const uint CConst::DEF_SERVER_TIMEOUT     = 60;
const uint CConst::DEF_SERVER_DELAY       = 0;
const uint CConst::RECV_TIMEOUT           = 10;
const uint CConst::MAX_CLIENT_REQ_SIZE    = 1024;

const uint16_t CConst::ADR_REQ            = 0xFE10;
const uint16_t CConst::FSZ_REQ            = 0xFE20;
const uint16_t CConst::GET_REQ            = 0xFE30;
const uint16_t CConst::ADR_RPLY           = 0xFE11;
const uint16_t CConst::FSZ_RPLY           = 0xFE21;
const uint16_t CConst::GET_RPLY           = 0xFE31;
const uint16_t CConst::ADR_FAIL           = 0xFE12;
const uint16_t CConst::FSZ_FAIL           = 0xFE22;
const uint16_t CConst::GET_FAIL           = 0xFE32;
const uint16_t CConst::ALL_FAIL           = 0xFCFE;
