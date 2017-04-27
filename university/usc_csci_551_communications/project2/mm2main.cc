/***************************************************************************
**                          Main
**                          -------------------
**    Sep 09 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <signal.h> 
#include "cutil.h"
#include "cconst.h"
#include "cexception.h"
#include "cmm2.h"

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
  CMm2::getInstance()->stop();
  signal(SIGINT, handleSignal);
}

/*!
    Prints the usage
*/
void printUsage()
{
  string str;
  str  = "Usage: mm2 [-lambda lambda] [-mu mu] [-s]\n";
  str += "           [-seed seedval] [-size sz]\n";
  str += "           [-n num] [-d {exp|det}]";

  CUtil::writeOutputLn(str);
}

/*!
  Gets the value for the given option
*/
string getOptVal(int argc, char *argv[], int i)
{
  if ((i+1) >= argc) {
    C_THROW("Invalid option");
  }
  return CUtil::trim(string(argv[i+1]));

}

/*!
  Returns the Lambda
*/
double getLambda(const string &val) {
  double t = CUtil::getFloat<double>(val, true);
  CUtil::validateRange(
    CConst::MIN_LAMBDA, CConst::MAX_LAMBDA,
    t, "Lambda");

  return t;
}

/*!
  Returns the mu 
*/
double getMu(const string &val) {
  double t = CUtil::getFloat<double>(val, true);
  CUtil::validateRange(
    CConst::MIN_MU, CConst::MAX_MU,
    t, "Mu");

  return t;
}

/*!
  Returns the seed value
*/
long getSeedVal(const string &val) {
  long t = CUtil::getInteger<long>(val, 0, true);
  CUtil::validateRange(
    CConst::MIN_SEEDVAL, CConst::MAX_SEEDVAL,
    t, "Seed Value");

  return t;
}

/*!
  Returns the Queue size
*/
uint getQSize(const string &val) {
  uint t = CUtil::getInteger<uint>(val, 0, true);
  CUtil::validateRange(
    CConst::MIN_QUEUESIZE, CConst::MAX_QUEUESIZE,
    t, "Queue Size");

  return t;
}

/*!
  Returns the customer value
*/
uint getNumCustomer(const string &val) {
  uint t = CUtil::getInteger<uint>(val, 0, true);
  CUtil::validateRange(
    CConst::MIN_CUSTOMERS, CConst::MAX_CUSTOMERS,
    t, "Customer Number");
    
  return t;
}

/*!
    Starts the server
*/
void startMm2(int argc, char *argv[])
{
  double lambda     = CConst::DEF_LAMBDA;
  double mu         = CConst::DEF_MU;
  long seedval      = CConst::DEF_SEEDVAL;
  uint qSize        = CConst::DEF_QUEUESIZE;
  uint nCustomer    = CConst::DEF_CUSTOMERS;
  DistType dType    = DistExp;
  bool singleServer = false;

  bool bError = false;

  try {
    for (int i=1; i<argc; i++) {
      string option = CUtil::trim(string(argv[i]));

      if(string("-s") == option) {
        singleServer = true;

      } else {
        string value = getOptVal(argc, argv, i);
        i++;

        if (string("-lambda") == option) {
          lambda = getLambda(value);

        } else if(string("-mu") == option) {
          mu = getMu(value);

        } else if(string("-seed") == option) {
          seedval = getSeedVal(value);

        } else if(string("-size") == option) {
          qSize = getQSize(value);

        } else if(string("-n") == option) {
          nCustomer = getNumCustomer(value);
          
        } else if(string("-d") == option) {
          if (string("exp") == value) {
            dType = DistExp;
          } else if (string("det") == value) {
            dType = DistDet;
          } else {
            bError = true;
          }
        } else {
          bError = true;
        }
      }
      if (bError) {
        break;
      }
    }
  } catch (CException &e) {
    CUtil::writeError(e.getMessage());
    bError = true;
  }

  if (bError) {
    printUsage();
  } else {
    //setup handler for Ctrl+c
    signal(SIGINT, handleSignal);
    CMm2::getInstance()->start(
      lambda, mu, seedval, qSize, nCustomer, dType, singleServer );
  }
  CMm2::releaseInstance();
}

/*!
    Main server function
*/
int main(int argc, char *argv[])
{
  int status = 0;
  try {
    startMm2(argc, argv);
  } catch (CException &e) {
    CUtil::writeError(e.getMessage(), true);
    status = -1;
  } catch (...) {
    CUtil::writeError("Main: Unexpected Exception");
    status = -1;
  }
  return status;
}
