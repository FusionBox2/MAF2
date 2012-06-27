/*=========================================================================

 Program: MAF2
 Module: testPicFactoryApp
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "testPicFactoryApp.h"
#include "mafPics.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testPicFactoryApp)

//--------------------------------------------------------------------------------
bool testPicFactoryApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPics.Initialize();	

  // this defines a char** variable called ico_xpm
  #include "ico.xpm"
  
  // this add the icon to the PicFactory as "ico"
  mafADDPIC(ico);

  // these replace the File_Open and File_Save icon with ico
  mafPics.Add("FILE_OPEN",ico_xpm);
  mafPics.Add("FILE_SAVE",ico_xpm);

  m_logic = new testPicFactoryLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testPicFactoryApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}






