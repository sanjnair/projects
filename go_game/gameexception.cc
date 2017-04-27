///
/// Game Exception class
/// @author - Sanjay Nair
///
#include "gameexception.h"
using namespace std;


/// Constructor (C strings).
GameException::GameException(const char* message)
    : _msg(message)
    , _lineNum(0)
{
}


/** Constructor (C++ STL strings).
*  @param message The error message.
*/
GameException::GameException(const string& message)
    : _msg(message)
    , _lineNum(0)
{}

/// Constructor 
GameException::GameException(
    const string& message, 
    const string& fileName, 
    int lineNum) 
    : _msg(message)
    , _filename(fileName)
    , _lineNum(0)
{}

/** Destructor.
* Virtual to allow for subclassing.
*/
GameException::~GameException() throw () {}

/** Returns a pointer to the (constant) error description.
*  @return A pointer to a const char*. The underlying memory
*          is in posession of the Exception object. Callers must
*          not attempt to free the memory.
*/
const char* GameException::what() const throw () {
    return _msg.c_str();
}

///
/// Returns the message
///
const string& GameException::message() const throw () {
    return _msg;
}