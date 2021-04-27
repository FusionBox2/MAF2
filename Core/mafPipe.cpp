/*=========================================================================

 Program: MAF2
 Module: mafPipe
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
//----------------------------------------------------------------------------


#include "mafGUI.h"

#include "mafPipe.h"
#include "mafNode.h"
#include "mafView.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipe);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipe::mafPipe()
//----------------------------------------------------------------------------
{
  m_Node     = NULL;
  m_View     = NULL;
  m_Selected = false;
}
//----------------------------------------------------------------------------
void mafPipe::Create(mafNode *node, mafView *view)
//----------------------------------------------------------------------------
{
  m_Node     = node;
  m_View     = view;
  m_Selected = false;
}
//----------------------------------------------------------------------------
mafPipe::~mafPipe()
//----------------------------------------------------------------------------
{
  SetListener(NULL);
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
mafGUI* mafPipe::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);

  mafString type = _R(GetTypeName());
  m_Gui->Label(_R("type :"), type);
	m_Gui->Divider();
  return m_Gui;
}
