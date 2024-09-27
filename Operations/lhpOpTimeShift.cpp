/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpTimeShift.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:19:42 $
  Version:   $Revision: 1.4 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpOpTimeShift.h"

#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include <wx/wxprec.h>
#include "wx/busyinfo.h"
#include <math.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafOpExplodeCollapse.h"

#include "ftk/Base/RegisteringPointer.h"
#include "mafMatrixVector.h"
#include "mafDataVector.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"

//----------------------------------------------------------------------------
// Required for MSVC
//----------------------------------------------------------------------------
#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_DEFAULT = MINID,
  ID_SHIFTVAL,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

mafCxxTypeMacro(lhpOpTimeShift)
//----------------------------------------------------------------------------
lhpOpTimeShift::lhpOpTimeShift(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_OP;
  m_Canundo   = false;
  m_Shift     = 0.0;
}

//----------------------------------------------------------------------------
lhpOpTimeShift::~lhpOpTimeShift()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafOp* lhpOpTimeShift::Copy()
//----------------------------------------------------------------------------
{
  lhpOpTimeShift *op = new lhpOpTimeShift(GetLabel());
  op->m_Shift = m_Shift;
  return op;
}

//----------------------------------------------------------------------------
bool lhpOpTimeShift::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;

  if(mafVMEGenericAbstract::SafeDownCast(vme) == NULL)
  {
    return false;
  }
  if(mafVMELandmarkCloud::SafeDownCast(vme) != NULL)
  {
    if(mafVMELandmarkCloud::SafeDownCast(vme)->IsOpen())
      return false;
  }
  return true;
}


//----------------------------------------------------------------------------
void lhpOpTimeShift::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
}

//----------------------------------------------------------------------------
void lhpOpTimeShift::CreateGui()
//----------------------------------------------------------------------------
{
  if(m_Gui == NULL)
  {
    mafString strng;
    int nFrames = mafVMEGenericAbstract::SafeDownCast(m_Input)->GetNumberOfLocalTimeStamps();
    strng = mafString::Format(_R("Node has %d timestamps"), nFrames);
    m_Gui = new mafGUI(this);
    m_Gui->SetListener(this);
    m_Gui->Label(strng);
    m_Gui->Label(_R("Shift for, s"));
    m_Gui->Double(ID_SHIFTVAL, _R(""), &m_Shift);
    m_Gui->OkCancel();
  }
  ShowGui();
}

//----------------------------------------------------------------------------
/*void lhpOpTimeShift::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    {mafEvent evUnq(this,result); mafEventMacro(evUnq);}
  }
  else if (result == OP_RUN_OK)
  {
    HideGui();
    {mafEvent evUnq(this,result); mafEventMacro(evUnq);}
  }
}*/
//----------------------------------------------------------------------------
void lhpOpTimeShift::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
    case wxOK:          
    { 
      OpStop(OP_RUN_OK);
      break;
    }
    case wxCANCEL:
    {    
      OpStop(OP_RUN_CANCEL);
      break;
    }
    case ID_SHIFTVAL:
      {
        break;
      }
    default:
    {
      mafEventMacro(*maf_event); 
    }
    break;
  }
}

//----------------------------------------------------------------------------
void lhpOpTimeShift::OpDo()
//----------------------------------------------------------------------------
{
  //modified by Stefano. 18-9-2003
  wxBusyInfo wait("Please wait, working...");
  std::vector<mafTimeStamp> kframes;
  mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(m_Input);

  mafMatrixVector *mv = vme->GetMatrixVector();
  mafDataVector   *dv = vme->GetDataVector();

  for(auto& elem : *mv)
  {
    //elem.first += m_Shift;
    //elem.second->SetTimeStamp(elem.second->GetTimeStamp() + m_Shift);
  }
  for(auto& elem : *dv)
  {
    //elem.first += m_Shift;
    //elem.second->SetTimeStamp(elem.second->GetTimeStamp() + m_Shift);
  }

  return;
}

//----------------------------------------------------------------------------
void lhpOpTimeShift::OpUndo()
//----------------------------------------------------------------------------
{
}
