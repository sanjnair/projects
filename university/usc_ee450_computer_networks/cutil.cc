#include <iostream>
#include <stdio.h>
#include <sstream>
#include "cutil.h"
#include "cexception.h"
#include "cconstant.h"

/*!
    Returns the string form of the int parameter
*/
string CUtil::getString(int n)
{
    std::stringstream converter;
    converter << n;
    return converter.str();
}

/*!
    Writes the string to output console
*/
void CUtil::writeOutput(const string &s)
{
    printf("%s", s.c_str());
}

/*!
    Writes the string to output console
    along with a newline char
*/
void CUtil::writeOutputLn(const string &s)
{
    printf("%s\n", s.c_str());
}

/*!
    Writes error string to output console
*/
void CUtil::writeError(const string &s, bool newLine)
{
    printf("Error: %s", s.c_str());
        
    if (newLine) {
        printf("\n");
    }
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
    Returns uint value for the given string.
*/
int CUtil::getUInt(const string &s)
{
	int num = -1;
    std::istringstream sin(s);
	sin >> num;

    if (num <0) {
        string msg("unable to convert ");
        msg += s;
        msg += " to number";
        C_THROW(msg);
    }

    return static_cast<uint>(num);
}

/*!
    Returns the command string for the given name and value
*/
string CUtil::getCmd(const string &name, const string &val, bool sep)
{
    string s = name + CConstant::NV_SEP + val;
    if (sep) {
        s += CConstant::SEP;
    }   

    return s;
}

/*!
    Gets the protocol commands from the map and
    assembles it as a string
*/
string CUtil::fromProtoMap(const StringMap &m)
{
    if (m.size() <= 0) {
        string msg("Invalid map passed.");
        C_THROW(msg);
    }

    uint index =0;
    string s;
    for(StringMap::const_iterator it = m.begin(); it != m.end(); ++it) {
        s += it->first + CConstant::NV_SEP + it->second;
        index++;

        if (m.size() > index) {
            s += CConstant::SEP;
        }
    }

    return s;
}

/*!
    Gets the string and converts it to the protocol map
*/
void CUtil::toProtoMap(const string &s, StringMap &m)
{
    if (s.length() <= 0) {
        string msg("Invalid string passed for protocol value separation.");
        C_THROW(msg);
    }

    string nvp;
    string name;
    string val;
    
    uint lpos = s.find_first_not_of(CConstant::SEP, 0);
    uint pos = s.find_first_of(CConstant::SEP, lpos);
    
    while (string::npos != pos || string::npos != lpos) {
        nvp = s.substr(lpos, pos - lpos);
        getNvp(nvp, name, val);
        m[name] = val;

        lpos = s.find_first_not_of(CConstant::SEP, pos);
        pos = s.find_first_of(CConstant::SEP, lpos);
    }

    if (m.size() <= 0) {
        string msg("Unable to get data from  the protocol map");
        C_THROW(msg);
    }
}

/*!
    Returns the name value pair for the given string.
*/
void CUtil::getNvp(const string &s, string &name, string &val)
{
    uint index = s.find(CConstant::NV_SEP);
    if ((index <= 0) || (index >= s.length())) {
        string msg("Name value string is invalid. ");
        msg += s;
        C_THROW(msg);
    }
    name = CUtil::trim(s.substr(0, index));
    val = CUtil::trim(s.substr(index+1, s.length()));

    if ((name.length() <=0) || (val.length() <=0)) {
        string msg("Invalid name value pair . Name = ");
        msg += name;
        msg += " Value = ";
        msg += val;
        C_THROW(msg);
    }
}

/*!
    Gets the value from the map
*/
string CUtil::getFromMap(const StringMap &m, const string &name)
{
    StringMap::const_iterator it = m.find(name);
    
    if (it == m.end()) {
        string msg("Name ");
        msg += name;
        msg += " not found in the map";
        C_THROW(msg);
    }

    return it->second;
}

/*!
    Validates user input to see if special character
    (protocol separator is used)
*/
void CUtil::validateUserInput(const string &s)
{
    
    if ((s.find(CConstant::NV_SEP) != string::npos) ||
        (s.find(CConstant::SEP) != string::npos)) {

        string msg("Input should not contain reserved characters \"");
        msg += CConstant::NV_SEP;
        msg += "\" and \"";
        msg += CConstant::SEP;
        msg += "\". Input: ";
        msg += s;

        C_THROW(msg);
    }   
}

/*!
    Writes the received message to the output
*/
void CUtil::writeRecdMsg(const string &ip, int port)
{
    string msg("Received request from ");
    msg += ip;
    msg += " and port ";
    msg += CUtil::getString(port);
    msg += "\n--------------------------------------\n";
    writeOutputLn(msg);
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

