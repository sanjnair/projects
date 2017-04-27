/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cglobal.h"

class CConst {

public:
  static double MIN_LAMBDA;
  static double MAX_LAMBDA;  
  static double DEF_LAMBDA;

  static double MIN_MU;
  static double MAX_MU;
  static double DEF_MU;

  static long MIN_SEEDVAL;
  static long MAX_SEEDVAL;
  static long DEF_SEEDVAL;

  static uint MIN_QUEUESIZE;
  static uint MAX_QUEUESIZE;
  static uint DEF_QUEUESIZE;

  static uint MIN_CUSTOMERS;
  static uint MAX_CUSTOMERS;
  static uint DEF_CUSTOMERS;
  
private:
  CConst();
};

