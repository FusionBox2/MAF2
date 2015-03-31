/*=========================================================================

 Program: MAF2
 Module: mafGUIComboBox
 Authors: Daniele Giunchi
 
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


#include "mafGUIComboBox.h"
//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
#define ID_CLB 100
//----------------------------------------------------------------------------
// mafGUIComboBox
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIComboBox,wxComboBox)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIComboBox::mafGUIComboBox(wxWindow* parent, wxWindowID id, wxArrayString array,const wxPoint& pos, const wxSize& size, long style)
:wxComboBox(parent,id,"",pos,size,array,style) 
//----------------------------------------------------------------------------
{
  m_OldValue = -1;
}
//----------------------------------------------------------------------------
mafGUIComboBox::~mafGUIComboBox( ) 
//----------------------------------------------------------------------------
{
}
#ifdef WIN32
//----------------------------------------------------------------------------
bool mafGUIComboBox::MSWCommand(WXUINT param, WXWORD id)
//----------------------------------------------------------------------------
{
  bool result;
  result = wxComboBox::MSWCommand(param,id);
  switch ( param )
  {
    case CBN_SELCHANGE:
      {
        if(GetSelection() != m_OldValue)
          mafEventMacro(mafEvent(this, GetId(), (long)GetSelection()));
        m_OldValue = GetSelection();
      }
      
      break;
  }
  return result;
}
#endif
//----------------------------------------------------------------------------
void mafGUIComboBox:: SetSelection(int n)
//----------------------------------------------------------------------------
{
  wxComboBox::SetSelection(n);
  m_OldValue = GetSelection();
}
