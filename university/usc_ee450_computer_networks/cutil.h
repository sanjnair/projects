/**
 * Utilities
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   07/22/2006 - Created
 */

#ifndef __CUTIL_H__
#define __CUTIL_H__

#if defined WIN32
    #pragma warning(disable:4786)
#endif

#include "cglobal.h"
#include <map>
typedef std::map<string, string> StringMap;

class CUtil {

public:
    static string getString(int n);
    static void writeOutput(const string &s);
    static void writeOutputLn(const string &s);
    static void writeError(const string &s, bool newLine=true);
    static string trim(const string &s);
    static int getUInt(const string &s);
    static string getCmd(const string &name, const string &val, bool sep);

    static string fromProtoMap(const StringMap &m);
    static void toProtoMap(const string &s, StringMap &m);
    static void getNvp(const string &s, string &name, string &val);
    static string getFromMap(const StringMap &m, const string &name);

    static void validateUserInput(const string &s);
    static void writeRecdMsg(const string &ip, int port);
    static string getFromConsole(bool t = true);

private:
    /**
     * Constructor is private
     */
    CUtil();
};

#endif //__CUTIL_H__

