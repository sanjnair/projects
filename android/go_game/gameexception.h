///
/// Game Exception class
/// @author - Sanjay Nair
///

#ifndef _GAME_EXCEPTION_H_
#define _GAME_EXCEPTION_H_

#include <string>

class GameException : public std::exception
{
public:
    explicit GameException(const char* message);
    explicit GameException(const std::string& message);
    explicit GameException(const std::string& message, const std::string& fileName, int lineNum);
    virtual ~GameException() throw ();
    virtual const char* what() const throw ();
    const std::string& message() const throw ();

protected:
    std::string _msg;
    std::string _filename;
    int _lineNum;
};

#endif //_GAME_EXCEPTION_H_
