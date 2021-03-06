/*=========================================================================

 Program: MAF2
 Module: mmoCreateVmeSurface
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


#include "mmoCreateVmeSurface.h"
#include "mafVMESurface.h"
#include "vtkConeSource.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoCreateVmeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateVmeSurface::mmoCreateVmeSurface(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true;
  m_vme = NULL;
}
//----------------------------------------------------------------------------
mmoCreateVmeSurface::~mmoCreateVmeSurface()
//----------------------------------------------------------------------------
{
  mafDEL(m_vme);
}
//----------------------------------------------------------------------------
mafOp* mmoCreateVmeSurface::Copy()
//----------------------------------------------------------------------------
{
  return new mmoCreateVmeSurface(m_Label);
}
//----------------------------------------------------------------------------
void mmoCreateVmeSurface::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*event);
}
//----------------------------------------------------------------------------
void mmoCreateVmeSurface::OpRun()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateVmeSurface::OpDo()
//----------------------------------------------------------------------------
{
  assert(!m_vme);

  mafNEW(m_vme);
  static int counter = 0;
  wxString name = wxString::Format("vme surface %d",counter++);
  m_vme->SetName(name);
  m_vme->ReparentTo( (mafVME*)m_Input);

  vtkConeSource *cs = vtkConeSource::New();
  cs->SetResolution(20);
  cs->CappingOn();
  cs->Update();
  m_vme->SetData(cs->GetOutput(),0);
  cs->Delete();

  mafEventMacro(mafEvent(this,VME_ADD,m_vme));
}
//----------------------------------------------------------------------------
void mmoCreateVmeSurface::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_vme);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_vme));
  mafDEL(m_vme);
}
