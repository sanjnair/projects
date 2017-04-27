#include <stdlib.h>
#include <sys/time.h>
#include <cctype>
#include <iostream>
#include "cutil.h"
#include "cexception.h"
#include "cconst.h"

/*!
  Writes unformatted data to the output
*/
void CUtil::writeOutput(
  const void *data, uint size, uint dataSize, FILE *fp)
{
  string msg;
  if(NULL == fp) {
    msg = "File handle is Null";
    C_THROW(msg);
  }
  errno = 0;
  fwrite(data, dataSize, size, fp);

  if( 0 != errno  ) {
    msg = "Failed to write to file.";
    C_THROW(msg);
  }
}

/*!
  Writes unformatted data to the output
*/
void CUtil::writeOutput(
  const void *data, uint size, uint dataSize)
{
  writeOutput(data, size, dataSize, stdout);
}

/*!
  Writes unformatted data to the output
*/
void CUtil::writeOutput(const string &s)
{
  writeOutput(s.c_str(), s.size(), sizeof(char));
}

/*!
  Writes unformatted data to the output
*/
void CUtil::writeOutputLn(const string &s)
{
  string str = s + string("\n");
  writeOutput(str);
}

/*!
    Writes error string to output console
*/
void CUtil::writeError(const string &s, bool newLine)
{
  string str = string("Error: ") + s;
  if (newLine) {
    str += string("\n");
  }
  writeOutput(str.c_str(), str.size(), sizeof(char), stderr);
}   

/*!
    Trims the whitespace from the string
    
    \attention - CODE REUSE 
    code from Thinking in C++ book.
*/
string CUtil::trim(const std::string& s) {
    if(s.length() == 0) {
        return s;
    }
    int b = s.find_first_not_of(" \t");
    int e = s.find_last_not_of(" \t");
    if(b == -1) {// No non-spaces
        return "";
    }
    return std::string(s, b, e - b + 1);
}

/*!
    Gets user input from console
*/
string CUtil::getFromConsole(bool t)
{
    char c[1001];
    std::cin.clear();
    std::cin.getline(c, 1001);
    
    string s(c);
    if (t) {
        s = trim(s);
    }

    return s;
}

/*!
  Throws error, if the character is not a digit
*/
void CUtil::errorIfNotDigit(char c) {
  if( ! isdigit(c)) {
    char buf[1];
    buf[0] = c;
    string msg("Character is not a digit. ");
    msg += buf;
  }
}

/*!
  Throws exception if number is negative
*/
void CUtil::errorIfNegativeNum(int n)
{
  if (n <0) {
    string msg = "Invalid Number ";
    msg += getString(n);
    msg += ". Positive value expected";
    C_THROW(msg);
  }
}

/*!
    Writes the received message to the output
*/
void CUtil::writeRecdMsg(const string &ip, uint16_t port)
{
    string msg("Received request from ");
    msg += ip;
    msg += " and port ";
    msg += CUtil::getString(port);
    msg += "\n--------------------------------------\n";
    writeOutputLn(msg);
}

/*!
  Returns the current time
*/

struct timeval CUtil::getCurrentTime()
{
  struct timeval tv;
  if (gettimeofday(&tv, NULL) < 0) {
    C_THROW("Unable to get time of the day");
  }
  return tv;
}

/*!
  Returns the difference between two timevals in milliseconds.
  If start is greater than end, this method will throw exception.

  \Attention: CODE REUSE
  The GNU C Library
  http://www.delorie.com/gnu/docs/glibc/libc_428.html
*/
int64_t CUtil::getDiffMillis(struct timeval &end, struct timeval &start)
{
   struct timeval result;

  /* Perform the carry for the later subtraction by updating y. */
  if (end.tv_usec < start.tv_usec) {
    int nsec = (start.tv_usec - end.tv_usec) / 1000000 + 1;
    start.tv_usec -= 1000000 * nsec;
    start.tv_sec += nsec;
  }
  if ((end.tv_usec - start.tv_usec) > 1000000) {
    int nsec = (end.tv_usec - start.tv_usec) / 1000000;
    start.tv_usec += 1000000 * nsec;
    start.tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result.tv_sec = end.tv_sec - start.tv_sec;
  result.tv_usec = end.tv_usec - start.tv_usec;

  if (end.tv_sec < start.tv_sec) {
    C_THROW("Start time is greater than end time. Time diff goes to negative");
  }
  int64_t t = int64_t(result.tv_sec * (int64_t)1000) +  int64_t(result.tv_usec / int64_t(1000));
  return t;
}

/*!
  Returns the current time in milli second resolution
*/
int64_t CUtil::getCurrentMillis()
{
  struct timeval tv;
  if (gettimeofday(&tv, NULL) < 0) {
    C_THROW("Unable to get time of the day");
  }
  int64_t t = (tv.tv_sec * 1000L) +  (tv.tv_usec / 1000L);
  return t;
}

/*!
  Sleep for the given milli seconds
*/
void CUtil::sleepFor(int64_t milli)
{
  struct timeval tv;
  tv.tv_sec = milli / 1000L;
  tv.tv_usec = (milli % 1000L) * 1000L;
  int ret = select (0, NULL, NULL, NULL, &tv);

  if (ret < 0) {
    writeError(string("select returned ") + getString(ret));
  }
}

