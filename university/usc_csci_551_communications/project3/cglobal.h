/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef CGLOBAL_H
#define CGLOBAL_H

#include <assert.h>
#include <list>
#include <vector>
#include <map>
#include <string>  
#include <arpa/inet.h>

typedef unsigned char uchar;
typedef std::string string;
typedef std::list<string> StringList;
typedef std::vector<string> StringVector;
typedef std::vector<uint> UIntVector;
typedef std::map<string, string> StringMap;
typedef std::vector<uint8_t> UCharBuffer;

#ifndef NULL
  #define NULL 0
#endif

#endif //__CGLOBAL_H__

