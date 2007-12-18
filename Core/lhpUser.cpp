/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpUser.cpp,v $
Language:  C++
Date:      $Date: 2007-12-18 14:33:58 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpUser.h"
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

#include "mafDecl.h"
#include "mafGUIDialogLogin.h"
#include "mafCrypt.h"

#include <fstream>

//----------------------------------------------------------------------------
lhpUser::lhpUser()
//----------------------------------------------------------------------------
{
  
}
//----------------------------------------------------------------------------
lhpUser::~lhpUser()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool lhpUser::CheckUserCredentials()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  bool res;// = ShowLoginDialog();
  // Check only username and not password.
  // This can be valid for anonymous (or guest) user without password
  //res = !m_Username.IsEmpty() && res;

  

  return res;
}