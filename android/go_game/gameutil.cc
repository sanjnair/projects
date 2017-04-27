///
/// Utility functions and classes
/// @author - Sanjay Nair
///

#include <iostream>
#include "gameutil.h"
#include <regex>
using namespace std;

///
/// Creates cell object
///
Cell::Cell(int r, int c)
    : row(r), col(c)
{}

bool Cell::operator== (const Cell& rhs) const
{
    return ((row == rhs.row) && (col == rhs.col));
}

///
/// Log class constructor
///
Log::Log() {}

///
/// Error logging
///
void Log::e(const string& msg) 
{
    cout << "[ERROR] " << msg.c_str() << endl;
}

///
/// Debug logging
///
void Log::d(const string& msg) 
{
    cout << msg.c_str() << endl;
}

///
/// Util class constructor
///
Util::Util() {}

///
/// Converts string to int
///
RESULT Util::toSize_t(
    const string& str,
    size_t& refVal)
{
    size_t idx = 0;
    int val = 0;
    try {
        val = stoi(str, &idx);
    }
    catch (exception&) {
        return RES_ERR;
    }

    if (idx != str.length()) {
        return RES_ERR;
    }
    if (val < 0) {
        return RES_ERR;
    }

    refVal = static_cast<size_t>(val);
    return RES_OK;
}

///
/// Left Trims the string
///
std::string& Util::ltrim(std::string & str)
{
    auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
    str.erase(str.begin(), it2);
    return str;
}

///
/// Right Trims the string
///
std::string& Util::rtrim(std::string & str)
{
    auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
    str.erase(it1.base(), str.end());
    return str;
}

///
/// Trims the string
///
string& Util::trim(string& str) {
    ltrim(str);
    rtrim(str);
    return str;
}

///
/// Get filename from file path
///
std::string Util::getFileNameFromPath(const std::string& filePath)
{
    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = filePath.rfind(sep, filePath.length());
    if (i != string::npos) {
        return(filePath.substr(i + 1, filePath.length() - i));
    }

    return("");
}

///
/// Returns a new string after replacing the extension of the 
/// file identified with filePath
///
std::string Util::replaceFileExtension(
    const std::string& filePath,
    const std::string& extn)
{
    bool appendExtension = true;
    string fileName = getFileNameFromPath(filePath);
    string::size_type i;

    string s = filePath;

    // check if filename has an extension
    i = fileName.rfind('.', fileName.length());
    if (i != string::npos) {
        i = s.rfind('.', s.length());

        if (i != string::npos) {
            s.replace(i + 1, extn.length(), extn);
            appendExtension = false;
        }
    }

    if (appendExtension) {
        s += extn;
    }

    return s;
}
