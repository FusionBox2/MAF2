/*=========================================================================

 Program: MAF2
 Module: mafOpCreateGenericVme
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


#include "mafOpCreateGenericVme.h"
#include "mafNodeGeneric.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateGenericVme);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateGenericVme::mafOpCreateGenericVme(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true;
  m_vme = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateGenericVme::~mafOpCreateGenericVme()
//----------------------------------------------------------------------------
{
  mafDEL(m_vme);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateGenericVme::Copy()
//----------------------------------------------------------------------------
{
  return new mafOpCreateGenericVme(m_Label);
}
//----------------------------------------------------------------------------
void mafOpCreateGenericVme::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  InvokeEvent(*event);
}
//----------------------------------------------------------------------------
void mafOpCreateGenericVme::OpRun()
//----------------------------------------------------------------------------
{
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
//----------------------------------------------------------------------------
void mafOpCreateGenericVme::OpDo()
//----------------------------------------------------------------------------
{
  assert(!m_vme);

  mafNEW(m_vme);
  static int counter = 0;
  wxString name = wxString::Format("vme generic %d",counter++);
  m_vme->SetName(name);
  m_vme->ReparentTo(m_Input);
  {mafEvent evUnq(this,VME_ADD,m_vme); InvokeEvent(evUnq);}
}
//----------------------------------------------------------------------------
void mafOpCreateGenericVme::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_vme);
  {mafEvent evUnq(this,VME_REMOVE,m_vme); InvokeEvent(evUnq);}
  mafDEL(m_vme);
}









