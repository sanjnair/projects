/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "cconst.h"

/*!
    Private constructor
*/
CConst::CConst() {}

double CConst::MIN_LAMBDA           = 0.000001;
double CConst::MAX_LAMBDA           = 2000.0;
double CConst::DEF_LAMBDA           = 0.5;

double CConst::MIN_MU               = 0.000001;
double CConst::MAX_MU               = 2000.0;;
double CConst::DEF_MU               = 0.35;

long CConst::MIN_SEEDVAL            = 0;
long CConst::MAX_SEEDVAL            = 100;
long CConst::DEF_SEEDVAL            = 0;

uint CConst::MIN_QUEUESIZE          = 1;
uint CConst::MAX_QUEUESIZE          = 100;
uint CConst::DEF_QUEUESIZE          = 5;

uint CConst::MIN_CUSTOMERS          = 1;
uint CConst::MAX_CUSTOMERS          = 100;
uint CConst::DEF_CUSTOMERS          = 20;
