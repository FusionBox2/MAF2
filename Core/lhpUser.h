/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpUser.h,v $
Language:  C++
Date:      $Date: 2008-10-15 15:32:55 $
Version:   $Revision: 1.5 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __lhpUser_H__
#define __lhpUser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafString.h"
#include "mafUser.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** lhpUser - Used to manage username and password; store them and give basic function to check user's credentials.
This is intended as starting point for user managing. Method for Check User Credentials can be redefined to be
customized at application level according to custom requests.
*/
class lhpUser : public mafUser
{
public:
  lhpUser();
  virtual ~lhpUser();

  /** Function to be customized at application level.
  By default open Login Dialog if the user did not inserted any information.*/
  virtual bool CheckUserCredentials();
  
  /** Used with proxy authentication*/
  void SetProxyURL(mafString proxyURL) {m_ProxyURL = proxyURL;};
  mafString GetProxyURL() {return m_ProxyURL;};
  
  /** Used with proxy authentication*/
  void SetProxyPort(mafString proxyPort) {m_ProxyPort = proxyPort;};
  mafString GetProxyPort() {return m_ProxyPort;};

  /** return true if user is authenticated on BiomedTown*/
  static bool IsAuthenticated() {return m_IsAuthenticated;};

protected:
  bool ExecuteAuthenticationScript();

  static bool m_IsAuthenticated;

  mafString m_PythonUploadFullPath; //>directory where the scripts are
  mafString m_FileName; //>script file name
  mafString m_PythonExe; //>python  executable
  mafString m_PythonwExe; //>python  executable

  mafString m_ProxyURL;
  mafString m_ProxyPort;
};
#endif
