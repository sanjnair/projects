/***************************************************************************
**                          Client Main
**                          -------------------
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cclient.h"
#include "cutil.h"
#include "cconst.h"
#include "cexception.h"

/*!
    Prints the usage
*/
void printUsage()
{
  string str = "Usage: client {-a|-f|-g} hostname[:port] string";
  CUtil::writeOutputLn(str);
}

bool getType(const string &s, ReqType &type)
{
  bool error = false;

  if (string("-a") == s) {
    type = ReqAddr;
  } else if (string("-f") == s) {
    type = ReqFileSize;
  } else if (string("-g") == s) {
    type = ReqGet;
  } else {
    error = true;
  }

  return error;
}

/*!
  Returns true if the string is not valid host.
  Else returns false
*/
bool getServerHost(const string &s, string &host, uint16_t &port)
{
  bool error = false;
  unsigned int index = s.find(":");

  if (string::npos == index) {
    host = s;
  } else {
    host = s.substr(0,index);

    if (host.length() <=0) {
      error = true;
    } else {
      if ((index+1) >= s.length()) {
        error = true;
      } else {
        string sPort = s.substr(index+1,s.length()-1);
        int tempNum = CUtil::getNumber<int>(sPort);
          CUtil::errorIfNegativeNum(tempNum);
          port = static_cast<uint>(tempNum);
      }
    }
  }
  return error;
}

/*!
    Starts the server
*/
void startClient(int argc, char *argv[])
{
  bool inputError = false;

  string serverHost;
  string remoteHost;
  uint16_t port = CConst::DEF_SERVER_PORT;
  ReqType type = ReqAddr;
  
  if (argc != 4) {
    inputError = true;
  } else {

    try {
      for (int i=1; i<argc; i++) {
          string s = argv[i];
          s = CUtil::trim(s);

          if (s.length() <=0) {
            inputError = true;
          
          } else {
            switch(i) {
              case 1:
                inputError = getType(s, type);
                break;
            case 2:
              inputError = getServerHost(s, serverHost, port);
              break;
            case 3:
              remoteHost = s;
              break;
          }
        }
        if (inputError) {
          break;
        }
      }
    } catch (CException &e) {
      CUtil::writeError(e.getMessage());
      inputError = true;
    }
  }

  if (inputError) {
    printUsage();
  } else {
    CClient client(type, serverHost, port, remoteHost);
    client.connect();
  }    
}

/*!
    Main server function
*/
int main(int argc, char *argv[])
{
  int status = 0;
  try {
    startClient(argc, argv);
  } catch (CException &e) {
    CUtil::writeError(e.getMessage(), true);
    status = -1;
  }
  return status;
}
