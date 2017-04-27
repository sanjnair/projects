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

class CUtil {
public:

  template <typename T> static string getString(T t);
  template <typename T> static string getHexStr(T t);
  template <typename T> static string getHexStr(T *buf, int len);
  template <typename T> static T getInteger(const string &s, bool cp = false, int radix = 0);
  template <typename T> static T getUInteger(const string &s, int radix = 0);
  template <typename T> static T getFloat(const string &s, bool cp = false);
  template <typename T> static void validateRange(T min, T max, T val, const string &name);  

  static string getHexStr(const UCharBuffer &b);
  static double getDouble(const string &s, bool cp=false);
  static string trim(const string &s);
  static bool startsWith(const string &s, const string &pattern, bool cs=false);
  static bool endsWith(const string &s, const string &pattern, bool cs=false);  
  static string toLower(const string &s);
  static string toUpper(const string &s);
  static bool isEqual(const string &s1, const string &s2, bool cs = false);
  static StringVector split(const string &str, const string &sep);
  static void getNvp(
                const string &s, const string &sep,
                string &name, string &value);
  
  static void writeOutput(const void *data, uint size,uint dataSize, FILE *fp);
  static void writeOutput(const void *data, uint size, uint dataSize);  
  static void writeOutput(const string &s);
  static void writeOutputLn(const string &s);
  static void writeError(const string &s, bool newLine=true);
  
  static string getFromConsole(bool t = true);
  static void errorIfNotDigit(char c);
  static void errorIfNegativeNum(int n);
  static void CUtil::writeRecdMsg(const string &ip, uint16_t port);

  static struct timeval getCurrentTime();
  static int64_t getDiffMillis(struct timeval &end, struct timeval &start);
  
  static int64_t getCurrentMillis();
  static void sleepFor(int64_t milli);
  static bool isElapsed(uint past, uint cur, uint duration);
  static string getPath(const string &dir, const string &file);
  static string getFileName(const string &path);
  
private:
    CUtil();

    template <typename T>
    static void validateNumber(T num, const string &s, char *endp, bool cp);
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
T CUtil::getInteger(const string &s, bool cp, int radix)
{
  char *endp = NULL;
  long l = strtol(s.c_str(), &endp, radix);
  validateNumber(l, s, endp, cp);
  return static_cast<T>(l);  
}

template <typename T>
T CUtil::getUInteger(const string &s, int radix)
{
  char *endp = NULL;
  ulong l = strtoul(s.c_str(), &endp, radix);
  return static_cast<T>(l);
}

/*!
  Returns the Unsigned integral type for the given string
*/
template <typename T>
T CUtil::getFloat(const string &s, bool cp)
{
  char *endp = NULL;
  double d = strtod(s.c_str(), &endp);
  validateNumber(d, s, endp, cp);
  return static_cast<T>(d);
}

/*!
  Validates the number and throws exception if number is not valid.
  if cp is true, then this method checks if number is positive or not.
*/
template <typename T>
void CUtil::validateNumber(T num, const string &s, char *endp, bool cp)
{
  if ((NULL == endp) || (s.c_str() ==  endp) || (*endp != '\0')) {
    string msg = string("Unable to convert ") + s + string(" to number");
    C_THROW(msg);
  }

  if (cp) {
    T zero = 0;
    if (num < zero) {
      string msg = "Number ";
      msg += s;
      msg += " is not positive.";
      C_THROW(msg);
    }
  }
}

/*!
  Returns the hex string for the given integer type
*/
template <typename T>
string CUtil::getHexStr(T t)
{
  int size = sizeof(t);
  string format = string("%0") + getString(size * 2) + string("x");
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

  for (int i = 0; i<len; i++) {
    s +=  getHexStr(buf[i]);
  }
  return s;
}

/*!
  Validates the range. If val does not fit within the min and max,
  this method throws exception
*/
template <typename T>
void CUtil::validateRange(T min, T max, T val, const string &name)
{
  if ((val < min) || (val > max)) {
    string msg = string("Invalid value (") + getString(val) + string(") ");
    msg += string("for ") + name + string (". Must be between ");
    msg += getString(min) + string(" and ") + getString(max);
    C_THROW(msg);
  }
}

#endif //__CUTIL_H__

