#include "cexception.h"
#include "cutil.h"

/*!
  Constructs Exception
 */
CException::CException() : _line(0) {}

/*!
  Constructs exception with parameters
*/
CException::CException(const std::string &m, const std::string f, int l)
    : _msg(m), _file(f), _line(l) {} 

/*!
  Deletes exception
*/
CException::~CException() {}

/*!
  Returns the message
*/
std::string CException::getMessage() const
{
    return _msg;
}

/*!
  Returns the file
*/
std::string CException::getFile() const
{
    return _file;
}

/*!
  Returns line
*/
int CException::getLine() const
{
    return _line;
}

/*!
  Returns string represetnation of the exception
*/
std::string CException::toString() const
{
  std::string s(_msg);
  s += " [ file: ";
  s += _file;
  s += ", line: ";
  s += CUtil::getString(_line);
  s += " ]";

  return s;
}

