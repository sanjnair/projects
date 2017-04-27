///
/// Utility functions and classes
/// @author - Sanjay Nair
///
#ifndef _GAME_UTIL_H_
#define _GAME_UTIL_H_

#include <string>

typedef long RESULT;

#define RES_OK              ((RESULT)0L)
#define RES_ERR             ((RESULT)0x80040000L)


///
/// Enumeation for color state
///
enum class StoneColor {
    NoColor,
    White,
    Black
};

///
/// Cell information
///
struct Cell
{
    int row;
    int col;

    Cell(int row, int col);
    bool operator == (const Cell& rhs) const;
};

///
/// Logging information
///
class Log {
private:
    Log();

public:
    static void e(const std::string& msg);
    static void d(const std::string& msg);
};

///
/// Utility class
///
class Util {
private:
    Util();

public:
    static RESULT toSize_t(const std::string& str, size_t& refVal);
    
    static std::string& ltrim(std::string& str);
    static std::string& rtrim(std::string& str);
    static std::string& trim(std::string& str);
    static std::string getFileNameFromPath(const std::string& filePath);
    static std::string replaceFileExtension(const std::string& filePath, const std::string& extn);
};

#endif //_GAME_UTIL_H_

