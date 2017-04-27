/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include <unistd.h>
#include "ckbhandler.h"
#include "cexception.h"
#include "cglobal.h"
#include "cconst.h"
#include "cutil.h"
#include "ciniparser.h"
#include "clog.h"

/*!
  Creates the keyboard handler
*/
CKbHandler::CKbHandler() : CThread()
{
  _stopFlag = false;
  _opInProgress = false;
  _servant = NULL;
}

/*!
  Deltes the keyboard handler
*/
CKbHandler::~CKbHandler() {}

/*!
  Sets the servant interface
*/
void CKbHandler::setServantHandle(IServant *s)
{
  _servant = s;
}

/*!
  Stops keyboard handler
*/
void CKbHandler::stop()
{
  {
    CMutexLocker m(&_mutex);
    _stopFlag = true;
  }

  if (kill(getpid(), SIGUSR2) < 0) {
    C_THROW("Unable to issue the kill call on SIGUSR2");
  }
}

/*!
  Returns true if the stop flag is enabled
*/
bool CKbHandler::isStopFlagEnabled()
{
  CMutexLocker m(&_mutex);
  return _stopFlag;
}

/*!
  Sets the operation in progress flag
*/
void CKbHandler::setOpInProgress(bool flag)
{
  CMutexLocker m(&_mutex);
  _opInProgress = flag;
}

/*!
  Returns the operation in progress flag
*/
bool CKbHandler::isOpInProgress()
{
  CMutexLocker m(&_mutex);
  return _opInProgress;
}

/*!
  Inidcates that the operation interrupted
*/
void CKbHandler::opInterrupted()
{
  bool opInProgress = isOpInProgress();

  if (opInProgress) {
    setOpInProgress(false);
  } else {
    outputStr(string(), true);
  }
  outputPrompt();
}

/*!
  Outputs the string to stdout
*/
void CKbHandler::outputStr(const string &str, bool newLine)
{
  string s = (newLine) ? (str + "\r\n") : str;
  write(STDIN_FILENO, s.c_str(), s.length());
}

/*!
  Outputs the prompt
*/
void CKbHandler::outputPrompt()
{
  if (_prompt.length() <= 0) {
    _prompt = string ("servant:");
    _prompt += CUtil::getString(_servant->getIniParser()->getPort());
    _prompt += string("> ");
  }
  outputStr(_prompt);
}

/*!
  Thread run
*/
void CKbHandler::run()
{
  assert(_servant);

  try {
    initSigHandler();
    handleUserInput();

  } catch (CException &e) {
    _servant->getLog()->logError(e.toString());
    _servant->setLastError(e.getMessage());
    _servant->stop();
  }
}

/*!
  Initialize the signal handler
*/
void CKbHandler::initSigHandler()
{
  int ret = sigemptyset(&_sigmask);
  errorIfInvalidSigCode(ret, "CKbHandler::initSigHandler::sigemptyset");

  ret = sigaddset(&_sigmask, SIGUSR2);
  errorIfInvalidSigCode(ret, "CKbHandler::initSigHandler::sigaddset");  
  _servant->setSigMask(&_sigmask, false);
}

/*!
  Throws exception if the signal error code is invalid
*/
void CKbHandler::errorIfInvalidSigCode(
  int code, const string &method) const
{
  if (code < 0) {
    string msg = string("Call to ") + method;
    msg += string(" returned error. code = ") + CUtil::getString(code);
    msg += string(", errno = ") + CUtil::getString(errno);
  }
}


/*!
  Process the user input.
  If this method returns false, then prompt
  should not be displayed. Otherwise prompt should be displayed.
*/
bool CKbHandler::processInput(const string &input)
{
  bool bDisplayPrompt = true;
  string str = CUtil::trim(input);
  
  if (str.length() > 0) {
    if (! isOpInProgress()) {

      if (CUtil::isEqual(CConst::CMD_SHUTDOWN, str, true)) {
        _servant->stop();
        {
          CMutexLocker m(&_mutex);
          _stopFlag = true;
        }
        bDisplayPrompt = false;
        
      } else {
        try {
          bDisplayPrompt = _servant->handleUserInput(str);
          setOpInProgress(! bDisplayPrompt);
          
        } catch (CException &e) {
          outputStr(e.getMessage(), true);
        }
      }
    } else {
      string msg("Previous operation is in progress. Press Ctrl-C to cancel.");
      outputStr(msg, true);
      bDisplayPrompt = false;
    }
  } else {
    bDisplayPrompt = (! isOpInProgress());
  }
  return bDisplayPrompt;
}

/*!
  Handles the user input
*/
void CKbHandler::handleUserInput()
{
  bool bDisplayPrompt = true;
  outputPrompt();

  string input;
  ssize_t bytes_read = 0;
  char buf[MAX_USER_INPUT];
  memset(buf, '0', MAX_USER_INPUT);

  while (! isStopFlagEnabled()) {

    bytes_read = read(STDIN_FILENO, buf, sizeof(buf));

    if (! isStopFlagEnabled()) {
      if (bytes_read > 0) {
        input = string(buf, bytes_read);
        bDisplayPrompt = processInput(input);

        if (! isStopFlagEnabled() && bDisplayPrompt) {
          outputPrompt();
        }
      } else {
        string msg = string("Received read error from keyboard handler");
        msg += CUtil::getString(bytes_read);
        C_THROW(msg);
      }
    }
  }
}

/*!
  Returns user's input
*/
string CKbHandler::getUserInput() const
{
  char buf[MAX_USER_INPUT];
  ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf));
  return string(buf, bytes_read);
}
