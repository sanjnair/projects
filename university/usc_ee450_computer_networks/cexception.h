/**
 * Exception class 
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CEXCEPTION_H__
#define __CEXCEPTION_H__

#include "cglobal.h"

#define C_THROW(msg) throw CException(msg, __FILE__, __LINE__);

class CException 
{
public:
    CException();
    CException(const std::string &m, const std::string f, int l);
    ~CException();

    std::string getMessage() const;
    std::string getFile() const;
    int getLine() const;

    std::string toString() const;

private:
    std::string _msg;
    std::string _file;
    int _line;

};

#endif //__CEXCEPTION_H__

