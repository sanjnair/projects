/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cglobal.h"

class CConst {

public:
  static const uint16_t DEF_SERVER_PORT;
  static const uint DEF_SERVER_TIMEOUT;
  static const uint DEF_SERVER_DELAY;
  static const uint RECV_TIMEOUT;
  static const uint MAX_CLIENT_REQ_SIZE;  

  static const uint16_t ADR_REQ;
  static const uint16_t FSZ_REQ;
  static const uint16_t GET_REQ;
  static const uint16_t ADR_RPLY;
  static const uint16_t FSZ_RPLY;
  static const uint16_t GET_RPLY;
  static const uint16_t ADR_FAIL;
  static const uint16_t FSZ_FAIL;
  static const uint16_t GET_FAIL;
  static const uint16_t ALL_FAIL;
  
private:
  CConst();
};

