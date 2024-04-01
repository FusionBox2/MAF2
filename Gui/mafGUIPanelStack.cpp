/*=========================================================================

 Program: MAF2
 Module: mafGUIPanelStack
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


#include "mafDecl.h"
#include "mafGUIPanelStack.h"
//----------------------------------------------------------------------------
// mafGUIPanelStack
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIPanelStack,mafGUIPanel)
END_EVENT_TABLE()
//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafGUIPanelStack::mafGUIPanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
               const wxSize& size, long style, const mafString& name)
:mafGUIPanel(parent,id,pos,size,style,name.toWx())         
//----------------------------------------------------------------------------
{
  m_Sizer =  new wxBoxSizer( wxVERTICAL );
  this->SetAutoLayout( TRUE );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mafGUIPanelStack::~mafGUIPanelStack( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafGUIPanelStack::Put(mafGUIPanel* p)
//----------------------------------------------------------------------------
{
   Push(p);
   return true;
}
//----------------------------------------------------------------------------
bool mafGUIPanelStack::Remove(mafGUIPanel* p)
//----------------------------------------------------------------------------
{
  Pop();
  return true;
}
//----------------------------------------------------------------------------
void mafGUIPanelStack::Push(mafGUIPanel* p)
//----------------------------------------------------------------------------
{
  if(!m_Panels.empty() && m_Panels.top() == p) return;

  if(!m_Panels.empty())
  {
	  m_Panels.top()->Reparent(mafGetFrame());
	  m_Panels.top()->Show(false);
	  m_Sizer->Detach(m_Panels.top());
  }

  m_Panels.push(p);

  m_Panels.top()->Show(true);
  m_Panels.top()->Reparent(this);
  m_Sizer->Add(m_Panels.top(),1,wxEXPAND);
   
  Layout();
}
//----------------------------------------------------------------------------
void mafGUIPanelStack::Pop()
//----------------------------------------------------------------------------
{
	if (m_Panels.empty()) return;

	m_Panels.top()->Show(false);
	m_Panels.top()->Reparent(mafGetFrame());
  m_Sizer->Detach(m_Panels.top());

  m_Panels.pop();

  if(!m_Panels.empty())
  {
	  m_Panels.top()->Show(true);
	  m_Panels.top()->Reparent(this);
	  m_Sizer->Add(m_Panels.top(), 1, wxEXPAND);
  }

  Layout();
}
