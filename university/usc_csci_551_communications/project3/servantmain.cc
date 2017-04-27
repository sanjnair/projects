/***************************************************************************
**                          Main
**                          -------------------
**    Sep 24 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cutil.h"
#include "cconst.h"
#include "cexception.h"
#include "cservant.h"

/*!
    Prints the usage
*/
void printUsage()
{
  string str;
  str  = "Usage: sv_node [-reset] startup.ini";
  CUtil::writeOutputLn(str);
}

/*!
    Starts the server
*/
void startServant(int argc, char *argv[])
{
  string iniFile;
  bool reset = false;
  bool bError = false;
  
  if ((argc >= 2) && (argc <= 3)) {
    int index =  1;

    if (argc > 2) {
      if (string("-reset") == string(argv[index])) {
        reset = true;
        index++;
      } else {
        bError = true;
      }
    }

    if (! bError) {
      iniFile = argv[index];
      if (! CUtil::endsWith(iniFile, ".ini")) {
        bError = true;
      }
    }
  } else {
    bError = true;
  }

  if (bError) {
    printUsage();
    
  } else {
    bool restart = false;
    do {
      CServant::getInstance()->start(reset, iniFile, restart);
      restart = CServant::getInstance()->isRestartEnabled();
      reset = false;

      CServant::releaseInstance();

      if (restart) {
        CUtil::writeOutputLn("Restarting the servant node");
      }
      
    } while (restart);
  }
}

/*!
    Main server function
*/
int main(int argc, char *argv[])
{
  int status = 0;
  
  try {
    startServant(argc, argv);
    
  } catch (CException &e) {
    CUtil::writeError(e.getMessage(), true);
    status = -1;
  } 
  return status;
}
