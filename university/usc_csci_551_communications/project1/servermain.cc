/***************************************************************************
**                          Server Main
**                          -------------------
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <signal.h> 
#include "cutil.h"
#include "cconst.h"
#include "cexception.h"
#include "cserver.h"

#include "cmutex.h"
#include "cwaitcondition.h"

/*!
  Signal handler.
*/
void handleSignal(int sig_num)
{

  //mask further signals
  sigset_t mset;
  sigset_t oset;
  sigfillset(&mset);
  sigprocmask(SIG_SETMASK, &mset, &oset);
  
  CServer::getInstance()->stop();
  CServer::releaseInstance();
  exit(0);
}

/*!
    Prints the usage
*/
void printUsage()
{
  string str = "Usage: server [-t seconds] [-d delay] [port]";
  CUtil::writeOutputLn(str);
}

/*!
    Starts the server
*/
void startServer(int argc, char *argv[])
{
  bool inputError = false;
  bool portProc = false;
  bool toutProc = false;
  bool delayProc = false;

  int tempNum;
  uint tout = CConst::DEF_SERVER_TIMEOUT;
  uint delay = CConst::DEF_SERVER_DELAY;
  uint16_t port = CConst::DEF_SERVER_PORT;

  if ((argc < 0) || (argc > 6)) {
    inputError = true;
    
  } else {
    try {
      for (int i=1; i<argc; i++) {
        string s = argv[i];
        s = CUtil::trim(s);

        if ((s == string("-t")) && (! toutProc)) {
          ((i+1) >= argc) ? inputError = true : i++;
          s = argv[i];
          s = CUtil::trim(s);

          tempNum = CUtil::getNumber<int>(s);
          CUtil::errorIfNegativeNum(tempNum);
          tout = static_cast<uint>(tempNum);
          if (0 == tout) {
            tout = CConst::DEF_SERVER_TIMEOUT;
          }
          toutProc = true;
          
        } else if ((s == string("-d")) && (! delayProc)) {
          ((i+1) >= argc) ? inputError = true : i++;
          s = argv[i];
          s = CUtil::trim(s);

          tempNum = CUtil::getNumber<int>(s);
          CUtil::errorIfNegativeNum(tempNum);
          delay = static_cast<uint>(tempNum);
          delayProc = true;
          
        } else if (! portProc) {

          tempNum = CUtil::getNumber<int>(s);
          CUtil::errorIfNegativeNum(tempNum);
          port = static_cast<uint>(tempNum);
          portProc = true;
        } else {
          inputError = true;
        }
        if (inputError) {
          break;
        }
      }
    } catch (CException &e) {
      CUtil::writeError(e.getMessage(), true);
      inputError = true;
    }
  }

  if (inputError) {
    printUsage();
    
  } else {
    //setup handler for Ctrl+c
    signal(SIGINT, handleSignal);
    CServer::getInstance()->start(tout, delay, port);
  }
  CServer::releaseInstance();
}

/*!
    Main server function
*/
int main(int argc, char *argv[])
{
  int status = 0;
  try {
    startServer(argc, argv);
  } catch (CException &e) {
    CUtil::writeError(e.getMessage(), true);
    status = -1;
  } catch (...) {
    CUtil::writeError("Main: Unexpected Exception");
    status = -1;
  }
  return status;
}
