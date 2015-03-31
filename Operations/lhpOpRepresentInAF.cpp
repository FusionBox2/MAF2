/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRepresentInAF.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-05 14:51:05 $
  Version:   $Revision: 1.11 $
  Authors:   Paolo Quadrani - porting Daniele Giunchi  
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

#include "lhpOpRepresentInAF.h"

#include "mafDecl.h"
#include <wx/busyinfo.h>

#include "mafEvent.h"

#include "mafVME.h"
#include "mafSmartPointer.h"
#include "mafVMELandmark.h"
#include "mafGUI.h"
#include "mafJointAnalysis.h"

#include "vtkMAFSmartPointer.h"
#include "mafMatrixVector.h"
#include "mafAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpRepresentInAF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpRepresentInAF::lhpOpRepresentInAF(const wxString& label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType        = OPTYPE_OP;
  m_Canundo       = false;
  m_MultiTime     = true;
  m_ApplyChildren = true;
}
//----------------------------------------------------------------------------
lhpOpRepresentInAF::~lhpOpRepresentInAF( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* lhpOpRepresentInAF::Copy()
//----------------------------------------------------------------------------
{
  return new lhpOpRepresentInAF(m_Label);
}
//----------------------------------------------------------------------------
bool lhpOpRepresentInAF::Accept(mafNode* node)
//----------------------------------------------------------------------------
{
  if(node == NULL && !node->IsMAFType(mafVMELandmarkCloud) || ((mafVMELandmarkCloud*)node)->IsOpen())
    return false;
  mafVMERefSysAbstract *afsys = GetRefSys(mafVME::SafeDownCast(node));
  return (afsys != NULL);
}
//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_CHOOSE = MINID,
  ID_MULTIPLE_TIME_REGISTRATION,
  ID_APPLY_CHILDREN,
};
//----------------------------------------------------------------------------
void lhpOpRepresentInAF::OpRun()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);


  //m_Gui->Bool(ID_MULTIPLE_TIME_REGISTRATION,_("multi-time"),&m_MultiTime,1);
  //m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,((mafVMELandmarkCloud*)m_Input)->IsAnimated());
  ////m_Gui->Bool(ID_APPLY_CHILDREN,_("Apply children"),&m_ApplyChildren,1);

  //m_Gui->OkCancel();

  //m_Gui->Divider();
  //ShowGui();
  OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void lhpOpRepresentInAF::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case wxOK:
        OpStop(OP_RUN_OK);
        break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
        break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void lhpOpRepresentInAF::OpDo()
//----------------------------------------------------------------------------
{
  wxBusyInfo wait(_("Please wait, working..."));
  mafVMELandmarkCloud *inputCloud = mafVMELandmarkCloud::SafeDownCast(m_Input);
  mafVMERefSysAbstract *refAF     = GetRefSys(inputCloud);

  std::vector<mafTimeStamp> timeStamps;
  inputCloud->GetLocalTimeStamps(timeStamps);
  for(unsigned i = 0; i < timeStamps.size(); i++)
  {
    mafMatrix cldMatr;
    inputCloud->GetOutput()->GetMatrix(cldMatr, timeStamps[i]);
    cldMatr.SetTimeStamp(timeStamps[i]);
    inputCloud->SetMatrix(cldMatr);
    for(unsigned j = 0; j < inputCloud->GetNumberOfLandmarks(); j++)
    {
      double pos[4];
      inputCloud->GetLandmark(j, pos, timeStamps[i]);
      inputCloud->SetLandmark(j, pos[0], pos[1], pos[2], timeStamps[i]);
    }
    /*if(m_ApplyChildren)
    {
      for(unsigned j = 0; j < inputCloud->GetNumberOfChildren(); j++)
      {
        mafVME *child       = mafVME::SafeDownCast(inputCloud->GetChild(j));
        mafVMEAFRefSys *sys = mafVMEAFRefSys::SafeDownCast(child);
        mafMatrix chlMatr;
        if(child == NULL || sys == refAF)
          continue;
        child->GetOutput()->GetMatrix(chlMatr, timeStamps[i]);
        chlMatr.SetTimeStamp(timeStamps[i]);
        child->SetMatrix(chlMatr);
      }
    }*/
  }
  for(unsigned i = 0; i < timeStamps.size(); i++)
  {
    mafMatrix refMatr;
    mafMatrix cldMatr;
    mafMatrix difMatr;
    refAF->GetOutput()->GetAbsMatrix(refMatr, timeStamps[i]);
    inputCloud->GetOutput()->GetAbsMatrix(cldMatr, timeStamps[i]);
    inputCloud->SetAbsMatrix(refMatr, timeStamps[i]);
    refMatr.Invert();
    mafMatrix::Multiply4x4(cldMatr, refMatr, difMatr);
    for(unsigned j = 0; j < inputCloud->GetNumberOfLandmarks(); j++)
    {
      double pos[4];
      double tmp[4];
      inputCloud->GetLandmark(j, pos, timeStamps[i]);
      pos[3] = 1.0;
      cldMatr.MultiplyPoint(pos, tmp);
      refMatr.MultiplyPoint(tmp, pos);
      inputCloud->SetLandmark(j, pos[0], pos[1], pos[2], timeStamps[i]);
    }
    /*if(m_ApplyChildren)
    {
      for(unsigned j = 0; j < inputCloud->GetNumberOfChildren(); j++)
      {
        mafVME *child       = mafVME::SafeDownCast(inputCloud->GetChild(j));
        mafVMEAFRefSys *sys = mafVMEAFRefSys::SafeDownCast(child);
        mafMatrix chlMatr;
        mafMatrix newChlMatr;
        if(child == NULL || sys == refAF)
          continue;
        child->GetOutput()->GetMatrix(chlMatr, timeStamps[i]);
        mafMatrix::Multiply4x4(chlMatr, difMatr, newChlMatr);
        newChlMatr.SetTimeStamp(timeStamps[i]);
        child->SetMatrix(newChlMatr);
      }
    }*/
  }
  for(unsigned j = 0; j < inputCloud->GetNumberOfChildren(); j++)
  {
    mafVME *child = mafVME::SafeDownCast(inputCloud->GetChild(j));
    child->Modified();
    child->Update();
  }

}
//----------------------------------------------------------------------------
void lhpOpRepresentInAF::OpUndo()
//----------------------------------------------------------------------------
{
}
