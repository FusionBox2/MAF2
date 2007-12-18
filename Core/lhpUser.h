/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpUser.h,v $
Language:  C++
Date:      $Date: 2007-12-18 12:08:38 $
Version:   $Revision: 1.1 $
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

};
#endif
