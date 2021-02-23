/*=========================================================================

 Program: MAF2
 Module: mafGUIFileHistory
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


#include "mafGUIFileHistory.h"
#include "mafDecl.h"
//----------------------------------------------------------------------------
// local const
//----------------------------------------------------------------------------
#define NUMBER_OF_RECENT_FILE 9

//----------------------------------------------------------------------------
mafGUIFileHistory::mafGUIFileHistory() :wxFileHistory(NUMBER_OF_RECENT_FILE)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIFileHistory::AddFileToHistory(const wxString& file)
//----------------------------------------------------------------------------
{
  wxFileHistory::AddFileToHistory(file);
}
//----------------------------------------------------------------------------
void mafGUIFileHistory::RemoveFileFromHistory(int i)
//----------------------------------------------------------------------------
{
  wxFileHistory::RemoveFileFromHistory(i);
}
//----------------------------------------------------------------------------
void mafGUIFileHistory::AddFilesToMenu()
//----------------------------------------------------------------------------
{
  wxFileHistory::AddFilesToMenu();
}
//----------------------------------------------------------------------------
void mafGUIFileHistory::AddFilesToMenu(wxMenu* menu)
//----------------------------------------------------------------------------
{
  wxFileHistory::AddFilesToMenu(menu);
}
