/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CMESSAGEFACTORY_H
#define CMESSAGEFACTORY_H

#include "cglobal.h"

class CTcpSocket;
class CMessage;

class CMessageFactory {

public:
    ~CMessageFactory();

    static CMessage *createMsg(CTcpSocket *sockP);
    
private:
  CMessageFactory();

  static UCharBuffer rcvFrom(CTcpSocket *sockP, uint len);
  static CMessage * getStoreMsg(
    const UCharBuffer &header, uint32_t dataLen, CTcpSocket *sockP);

  static CMessage * getGetResMsg(
    const UCharBuffer &header, uint32_t dataLen, CTcpSocket *sockP);

  static string writeToTmpFile(CTcpSocket *sockP, uint32_t dataLen);
  
};


#endif //CMESSAGEFACTORY_H

