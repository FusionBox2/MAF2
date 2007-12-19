/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpUser.h,v $
Language:  C++
Date:      $Date: 2007-12-19 13:56:21 $
Version:   $Revision: 1.2 $
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

protected:
  bool ExecuteAuthenticationScript();

  mafString m_PythonUploadFullPath; //>directory where the scripts are
  mafString m_FileName; //>script file name
  mafString m_PythonExe; //>python  executable
};
#endif
