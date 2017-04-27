/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <math.h>
#include "cutil.h"
#include "cexception.h"
#include "cmm2.h"


#define round(X) (((X)>= 0)?(int)((X)+0.5):(int)((X)-0.5))
                                                   
/*!
  Gets the singleton instance
*/
CMm2* CMm2::getInstance()
{
  if (NULL == _instance) {
    _instance = new CMm2();
    assert(_instance);
  }
  return _instance;
}

/*!
  Releases the singleton instance
*/
void CMm2::releaseInstance()
{
  if (NULL != _instance) {
    delete _instance;
    _instance = NULL;
  }
}

/*!
  Constructs the MM2
*/
CMm2::CMm2() {
  _stopFlag = false;
  _lambda = 0.0;
  _mu = 0.0;
  _seed = 0;
  _qsize = 0;
  _ncustomer = 0;
  _dtype = DistExp;
  _singleserver = false;
  _simStartTime = 0;
  _simEndTime = 0;
  _arrival = NULL;
  _s1 = NULL;
  _s2 = NULL;
}

/*!
  Deletes the MM2
*/
CMm2::~CMm2()
{
  cleanup();
}

CMm2::CMm2(const CMm2 &other) {}

CMm2& CMm2::operator=(const CMm2 &other)
{
  return *this;
}

/*!
  Cleanup
*/
void CMm2::cleanup()
{
  if (NULL != _arrival) {
    delete _arrival;
    _arrival = NULL;
  }
  if (NULL != _s1) {
    delete _s1;
    _s1 = NULL;
  }
  if (NULL != _s2) {
    delete _s2;
    _s2 = NULL;
  }
}

/*!
  Returns Lambda
*/
double CMm2::getLambda() const
{
  return _lambda;
}

/*!
  Returns Mu
*/
double CMm2::getMu() const
{
  return _mu;
}

/*!
  Returns seed
*/
long CMm2::getSeed() const
{
  return _seed;
}

/*!
  Returns Queue size
*/
uint CMm2::getQSize() const
{
  return _qsize;
}

/*!
  Returns number of customers
*/
uint CMm2::getNumCustomer() const
{
  return _ncustomer;
}

/*!
  Returns Distribution type
*/
DistType CMm2::getDistType() const
{
  return _dtype;
}

/*!
  Returns true, if single server is enabled
*/
bool CMm2::isSingleServer() const
{
  return _singleserver;
}

/*!
  Set simulation start time
*/
void CMm2::setSimStartTime(int64_t t)
{
  _simStartTime = t;
}

/*!
  Returns simulation start time
*/
int64_t CMm2::getSimStartTime()
{
  return _simStartTime;
}

/*!
  Set simulation end time
*/
void CMm2::setSimEndTime(int64_t t)
{
  _simEndTime = t;
}

/*!
  Returns simulation end time
*/
int64_t CMm2::getSimEndTime()
{
  return _simEndTime;
}

/*!
  Starts Mm2
*/
void CMm2::start(
    double lambda, double mu, long seed, uint qsize,
    uint ncustomer, DistType dtype, bool singleserver)
{
    _lambda= lambda;
    _mu = mu;
    _seed = seed;
    _qsize = qsize;
    _ncustomer = ncustomer;
    _dtype = dtype;
    _singleserver = singleserver;

    startSimulation();
}

/*!
  Prints the timeline. Method is mutex protected
*/
void CMm2::printTimeline(int64_t sTime, int64_t eTime, const string &msg)
{
  int64_t dTime = eTime - sTime;
  int64_t sec   = dTime/1000L;
  int64_t milli = dTime - (1000L * sec);

  CMutexLocker m(&_printMutex);
  fprintf(stdout, "%08d.%03dms: %s\n",
    (uint)sec, (uint)milli, msg.c_str());
}
 
/*!
  Prints statistics
*/
void CMm2::printStatistics()
{
  _arrival->calcStatistics();
  CUtil::writeOutputLn("\nStatistics:");
  printStat("average inter-arrival time", _arrival->getAvgInterArrivalTime());
  printStat("average service time", _arrival->getAvgServiceTime());
  printStat("average number of customers in Q1", _arrival->getAvgQ1(), false);
  printStat("average number of customers at S1", _arrival->getAvgS1(), false);

  if (!_singleserver) {
    printStat("average number of customers at S2", _arrival->getAvgS2(), false);
  }
  printStat("average time spent in system", _arrival->getAvgSystemTime());
  printStat("standard deviation for time spent in system", _arrival->getSystemDeviation());
  printStat("customer drop proability", _arrival->getDropProbability(), false);
  
}

/*!
  Prints individual statistic
*/
void CMm2::printStat(const string &msg, double val, bool isTime)
{
  if (isTime) {
    val /= 1000;
  }
  fprintf(stdout, "\t%s = %8.3f", msg.c_str(), val);
  if (isTime) {
    fprintf(stdout, "s");
  }
  fprintf(stdout, "\n");  
}


/*!
  Prints the msg. Method is mutex protected
*/
void CMm2::printMsg(const string &msg)
{
  CMutexLocker m(&_printMutex);
  CUtil::writeOutputLn(msg);
}

/*!
  Starts the simulation
*/
void CMm2::startSimulation()
{

  printParams();
  initRandom(_seed);
  initSignalMask();

  _s1 = new CMserver(1, this);
  assert(_s1);

  if (! _singleserver) {
    _s2 = new CMserver(2, this);
    assert(_s2);
  }
  _arrival = new CArrival(this);
  assert(_arrival);

  setSignalMask(true);
  _s1->start();
  setSignalMask(false);

  if (_s2) {
    setSignalMask(true);
    _s2->start();
    setSignalMask(false);
  }

  setSignalMask(true);
  _arrival->start();
  setSignalMask(false);

  //wait for the arrival thread to join
  _arrival->join();
  {
    CMutexLocker m(&_mutex);
    _stopFlag = true;
  }
  {
    CMutexLocker m(&_mutex);
    _cond.notifyAll();
  }
  _s1->join();
  if (_s2) {
    _s2->join();
  }
  setSimEndTime(CUtil::getCurrentMillis());
  printStatistics();
}

/*!
  Stops the MM2 Server
*/
void CMm2::stop()
{
  printMsg("Stop request rcvd. Please wait for current simulation to finish...");
  if (! isStopFlagSet()) {
    //instruct the arrival thread to stop
    _arrival->stop();
  }
}

/*!
  Returns true if stop flag is set.
  This method locks the mutex before accessing
  the shared variable
*/
bool CMm2::isStopFlagSet()
{
  CMutexLocker m(&_mutex);
  return _stopFlag;
}

/*!
  Inserts the customer to the queue and wakes up the
  waiting threads
*/
void CMm2::insertCustomer(CCustomer *c)
{
  CMutexLocker m(&_mutex);

  int64_t t = CUtil::getCurrentMillis();
  string msg;

  if (_customerList.size() >= _qsize) {
    c->dropped();
    c->setDepartureTime(CUtil::getCurrentMillis());
    
    msg = c->getName() + string(" dropped");
    printTimeline(c->getSimStartTime(), t, msg);

  } else {
    c->setQEnterTime(t);
    msg = c->getName() + string( " enters Q1");
    printTimeline(c->getSimStartTime(), t, msg);

    _customerList.push_back(c);
    _cond.notifyAll();
  }
}

/*!
  Returns the Customer from the queue. If no customer
  is in the queue, then this method waits for a customer
  to be added into the queue
*/
CCustomer* CMm2::getCustomer()
{
  CMutexLocker m(&_mutex);
  while ((! _stopFlag) && (_customerList.empty())) {
    _cond.wait(&_mutex);
  }

  CCustomer *c = NULL;
  if (! _customerList.empty()) {
    int64_t t = CUtil::getCurrentMillis();
    c = _customerList.front();
    _customerList.pop_front();
    c->setQDepartureTime(t);

    string msg = c->getName() + string(" leaves Q1, time in Q1 = ");
    msg += CUtil::getString(c->getQtime()) + string("ms");
    printTimeline(c->getSimStartTime(), t, msg);
  }

  return c;
}

/*!
  Print the parameters
*/
void CMm2::printParams() const
{
  string system = (_singleserver) ? "M/M/1" : "M/M/2";
  string distr = (_dtype == DistExp) ? "exp" : "det";

  string s("\nParameters:\n");
  s += string("\tlambda = ") + CUtil::getString(_lambda) + string("\n");
  s += string("\tmu = ") + CUtil::getString(_mu) + string("\n");
  s += string("\tsystem = ") + system + string("\n");
  s += string("\tseed = ") + CUtil::getString(_seed) + string("\n");
  s += string("\tsize = ") + CUtil::getString(_qsize) + string("\n");  
  s += string("\tnumber = ") + CUtil::getString(_ncustomer) + string("\n");
  s += string("\tdistribution = ") + distr + string("\n");

  CUtil::writeOutputLn(s);
}

/*!
  Initializes the random generator
*/    
void CMm2::initRandom(long l_seed)
{
  if (l_seed == 0L) {
    time_t localtime=(time_t)0;
    time(&localtime);
    srand48((long)localtime);
  } else {
    srand48(l_seed);
  }
}

/*!
  Returns the interval in milli seconds
*/
int CMm2::getInterval(bool exponential, double rate)
{
  int ival = 0;
  if (exponential) {
    double dval=(double)drand48();
    ival = exponentialInterval(dval, rate);
  } else {
    double millisecond=((double)1000)/rate;
    ival =  (int)(round(millisecond));
  }
  if (ival < 1) {
    ival = 1;
  } else if (ival > 10000) {
    ival = 10000;
  }
  
  return ival;
}

/*!
  Returns the exponential interval in milli seconds.
*/
int CMm2::exponentialInterval(double dval, double rate)
{
  double w = (-1/rate) * log(1 - dval);
  return round(w * 1000);
}

/*!
  Initialize the signal mask
*/
void CMm2::initSignalMask()
{
  sigemptyset (&_signal_mask);
  sigaddset (&_signal_mask, SIGINT);
  sigaddset (&_signal_mask, SIGTERM);
}

/*!
  Sets the signal mask.
  Main thread calls this method with true argument to
  block all the signals of interest so that threads will
  inherit them prior to creating all threads. Once the threads
  are created, they are unblocked by calling this methid with
  false parameter.
*/
void CMm2::setSignalMask(bool block) const
{
  int rc = 0;
  if (block) {
    rc = pthread_sigmask(SIG_BLOCK, &_signal_mask, NULL);
  } else {
    rc = pthread_sigmask(SIG_UNBLOCK, &_signal_mask, NULL);
  }
  if (0 != rc) {
    string msg = "Call to pthread_sigmask returned error ";
    msg += CUtil::getString(msg);
    C_THROW(msg);
  }
}

//-----------------------------------------------------------------------------
CMm2* CMm2::_instance = NULL;


