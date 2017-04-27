#include <cctype>
#include <iostream>
#include "cutil.h"
#include "cexception.h"
#include "cconst.h"
#include "ctcpsocket.h"

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

