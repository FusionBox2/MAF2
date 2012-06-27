/*=========================================================================

 Program: MAF2
 Module: mafGUILab
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


#include "mafGUILab.h"

//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUILab,wxStaticText)
    EVT_LEFT_DOWN(mafGUILab::OnLeftMouseButtonDown)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(mafGUILab,wxStaticText)

//----------------------------------------------------------------------------
void mafGUILab::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  e.SetEventObject(this);
  ProcessEvent(e);
}
