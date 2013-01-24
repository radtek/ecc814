#ifndef _SV_ECC_TELNET_MONITOR_H_
#define _SV_ECC_TELNET_MONITOR_H_

#pragma once

#include <base/svdefines.h>
#include <base/SVTelnet.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TELMONITOR(map<string, string, less<string> >InputParams, char *pszReturn, int &nSize);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TELNET_VALUE(const char * pszServer, const int & nPort, const char * pszUid, const char * pszPwd, 
                  const char * pszUserPrompt, const char * pszPwdPrompt, const char * pszLoginPrompt, 
                  const char * pszOsType, const char * pszLogFile, const char * pszMatchCmd, 
                  const char * pszRuleFile, const int & nAppend, const char * pszCustomerPath, 
                  char * pszReturn, int & nSize);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TELDIRFILEMONITOR(map<string, string, less<string> >InputParams, char *pszReturn, int &nSize);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TEL_DIREXIST_FILENUM(const char * pszServer, const int & nPort, const char * pszUid, const char * pszPwd, 
                  const char * pszUserPrompt, const char * pszPwdPrompt, const char * pszLoginPrompt, 
                  const char * pszOsType, const char * pszLogFile, const int &nDayNun, const char * pszMatchCmd, 
                  const char * pszMatchCmd2, const char *pszExtParam,
                  const char * pszRuleFile, const int & nAppend, const char * pszCustomerPath, 
                  char * pszReturn, int & nSize);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TELDATE_MONITOR(map<string, string, less<string> >InputParams, char *pszReturn, int &nSize);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TEL_DATE_VALUE(const char * pszServer, const int & nPort, const char * pszUid, const char * pszPwd, 
                  const char * pszUserPrompt, const char * pszPwdPrompt, const char * pszLoginPrompt, 
                  const char * pszOsType, const char * pszLogFile, const char * pszMatchCmd, 
                  const char * pszRuleFile, const int & nAppend, const char * pszCustomerPath, 
                  char * pszReturn, int & nSize);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TelDirectory(map<string, string, less<string> >InputParams, char *pszReturn, int nSize);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TelnetConnect(const string &szMachine, int nPort, const string &szUser,
                   const string szPwd, const string &szLoginPrompt, const string &szPwdPrompt, 
                   const string &szOSPrompt, SVUnixOpMonitor &svUnixOpMonitor, string &szReturn);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TelnetExeCmd(SVUnixOpMonitor &svUnixOpMonitor, const string &szCmd, string &szReturn);

#endif