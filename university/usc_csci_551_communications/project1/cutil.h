/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#ifndef __CUTIL_H__
#define __CUTIL_H__

#include <sstream>
#include "cglobal.h"
#include "cexception.h"

class CTcpSocket;

class CUtil {
public:

  template <typename T> static string getString(T t);
  template <typename T> static T getNumber(const string &t);
  template <typename T> static string getHexStr(T t);
  template <typename T> static string getHexStr(T *buf, int len);
  
  static void writeOutput(const void *data, uint size,uint dataSize, FILE *fp);
  static void CUtil::writeOutput(const void *data, uint size, uint dataSize);  
  static void writeOutput(const string &s);
  static void writeOutputLn(const string &s);
  static void writeError(const string &s, bool newLine=true);
  
  static string trim(const string &s);
  static string getFromConsole(bool t = true);
  static void errorIfNotDigit(char c);
  static void errorIfNegativeNum(int n);
  static void CUtil::writeRecdMsg(const string &ip, uint16_t port);

private:
    CUtil();
};

/*!
  Returns the string for the given primitive type
*/
template <typename T>
string CUtil::getString(T t)
{
  std::stringstream c;
  c << t;
  return c.str();
}

/*!
  Returns the integral type for the given string
*/
template <typename T>
T CUtil::getNumber(const string &s)
{
  T t;
  std::istringstream iss(s);
  bool status =!(iss >> std::dec >> t).fail();

  if (!status) {
    string msg("unable to convert ");
    msg += s;
    msg += " to number";
    C_THROW(msg);
  }
  return t;
}

/*!
  Returns the hex string for the given integer type
*/
template <typename T>
string CUtil::getHexStr(T t)
{
  int size = sizeof(t);
  string format = string("%0") + getString(size) + string("x");
  char buf[100];

  if (size == sizeof(uint8_t)) {
    sprintf(buf, format.c_str(), static_cast<uint8_t>(t));
    
  } else if (size == sizeof(uint16_t)) {
    sprintf(buf, format.c_str(), static_cast<uint16_t>(t));
    
  } else if (size == sizeof(uint32_t)) {
    sprintf(buf, format.c_str(), static_cast<uint32_t>(t));
  } else {
    sprintf(buf, format.c_str(), t);
  }

  return string(buf);
}

/*!
  Returns the hex string for the given buffer
*/
template <typename T>
string CUtil::getHexStr(T *buf, int len)
{
  string s;

  for (int i=0; i<len; i++) {
    s +=  getHexStr(buf[i]);
  }
  return s;
}

#endif //__CUTIL_H__

