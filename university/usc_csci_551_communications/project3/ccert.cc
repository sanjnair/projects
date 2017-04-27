/***************************************************************************
**    Sun Aug 27 2006
**    (C) 2006 by Sanjay Nair
**    sanjaysn <at> usc <dot> edu
****************************************************************************/

#include "ccert.h"
#include "cexception.h"
#include "cutil.h"
#include "cconst.h"
#include "cfile.h"

/*!
  Construts the nsWriter with the given file path
*/
CCert::CCert() {}

/*!
  Deletes the log
*/
CCert::~CCert() {}

/*!
  Resets the cert
*/
void CCert::reset(const string &certDir)
{
  string pubPath = CUtil::getPath(certDir, FILE_PUBLIC);
  string privPath = CUtil::getPath(certDir, FILE_PRIVATE);

  if (CFile::exists(pubPath)) {
    CFile::deleteFile(pubPath);
  }

  if (CFile::exists(privPath)) {
    CFile::deleteFile(privPath);
  }
  
}

/*!
  Returns true if the certificate exist
*/
bool CCert::isCertExist(const string &certDir)
{
  string pubPath = CUtil::getPath(certDir, FILE_PUBLIC);
  string privPath = CUtil::getPath(certDir, FILE_PRIVATE);

  return (CFile::exists(pubPath) &&  CFile::exists(privPath));
}

/*!
  Generates the certificate in the given certDir
*/
void CCert::genCert(const string &certDir)
{
  string cmd;
  string pubPath = CUtil::getPath(certDir, FILE_PUBLIC);
  string privPath = CUtil::getPath(certDir, FILE_PRIVATE);

  cmd  = PATH_OPENSSL;
  cmd += " req -new -nodes -newkey rsa:1024 -x509 ";
  cmd += "-subj \"/C=US/ST=CA/L=LA/O=USC/OU=Servant-`";
  cmd += "date +%d%b%Y`-rootca/CN=`hostname`/emailAddress=`";
  cmd += PATH_WHOAMI;
  cmd += "`@usc.edu\" -keyout ";
  cmd += privPath;
  cmd += " -out ";
  cmd += pubPath;

  string output = execCmd(cmd);

  if (! isCertExist(certDir)) {
    string msg = "unable to generate certificate in ";
    msg += certDir;
    msg += ". Output = ";
    msg += output;
    C_THROW(msg);
  }
}

/*!
  Signs the inFile and writes it to outFile
*/
void CCert::sign(
  const string &certDir,
  const string &inPath,
  const string &outPath)
{
  string cmd;
  string pubPath = CUtil::getPath(certDir, FILE_PUBLIC);
  string privPath = CUtil::getPath(certDir, FILE_PRIVATE);

  cmd  = PATH_OPENSSL;
  cmd += " smime -sign -in ";
  cmd += inPath;
  cmd += " -out ";
  cmd += outPath;
  cmd += " -nocerts -signer ";
  cmd += pubPath;
  cmd += " -inkey ";
  cmd += privPath;

  string output = execCmd(cmd);

  if (! CFile::exists(outPath)) {
    string msg = "unable to sign file ";
    msg += outPath;
    msg += ". Output = ";
    msg += output;
    C_THROW(msg);
  }
}

/*!
  Returns true, if the digital certificate could be verified.
*/
bool CCert::verify(
  const string &pubPath, const string &filePath)
{
  string cmd;

  cmd  = PATH_OPENSSL;
  cmd += " smime -verify -noverify -in ";
  cmd += filePath;
  cmd += " -out /dev/null -signer ";
  cmd += pubPath;
  cmd += " -certfile ";    
  cmd += pubPath;  

  string output = CUtil::trim(execCmd(cmd));
  return (VERIFY_STR == output);
}

/*!
  Executes the command using popen and verifies the
  string in stderr
*/
string CCert::execCmd(const string &cmd)
{
  FILE *pfp = NULL;
  char cmdBuf[1024];
  char buf[256];

  snprintf(cmdBuf, sizeof(cmdBuf), "bash -c \"%s 2>&1\"", cmd.c_str());

  if ((pfp = (FILE*)popen(cmdBuf, "r")) == NULL) {
    string msg = string("Unable to execute command ") + cmd;
    C_THROW(msg);
  }

  string output;
  while (fgets(buf, sizeof(buf), pfp) != NULL) {
    output += buf;
  }
  pclose(pfp);

  return output;
}

//-----------------------------------------------------------------------------
const string CCert::FILE_PUBLIC         = "cert.pem";
const string CCert::FILE_PRIVATE        = "private.pem";
const string CCert::VERIFY_STR          = "Verification successful";

#ifdef linux
const string CCert::PATH_WHOAMI         = "/usr/bin/whoami";
const string CCert::PATH_OPENSSL        = "/usr/bin/openssl";

#else
const string CCert::PATH_WHOAMI         = "/usr/ucb/whoami";
const string CCert::PATH_OPENSSL        = "/home/scf-22/csci551b/openssl/bin/openssl";
#endif
