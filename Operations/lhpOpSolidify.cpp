/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpSolidify.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpOpSolidify.h"
#include <wx/busyinfo.h>

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"


#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"

#include "vtkPolyData.h"
#include "vtkMEDPolyDataMirror.h"
#include "mafVectors.h"
#include <vector>


#include "mafVMESurface.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpSolidify);
//----------------------------------------------------------------------------


void (*lhpOpSolidify::m_SolidificationAlgorithm)(const std::vector<V3d<double> >& gold, std::vector<std::vector<V3d<double> > >& motion, std::vector<std::vector<bool> >& visibility);

//----------------------------------------------------------------------------
lhpOpSolidify::lhpOpSolidify(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType             = OPTYPE_OP;
  m_Canundo            = true;
  m_Source             = NULL;
  m_SourceName         = "none";
}
//----------------------------------------------------------------------------
lhpOpSolidify::~lhpOpSolidify()
//----------------------------------------------------------------------------
{
  mafDEL(m_Output);
}
//----------------------------------------------------------------------------
mafOp* lhpOpSolidify::Copy()
//----------------------------------------------------------------------------
{
  lhpOpSolidify *cp = new lhpOpSolidify(GetLabel());
  cp->m_Canundo   = m_Canundo;
  cp->m_OpType    = m_OpType;
  cp->SetListener(GetListener());
  cp->m_Next      = NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool lhpOpSolidify::Accept(mafNode* node)   
//----------------------------------------------------------------------------
{
  return  (node && (node->IsMAFType(mafVMELandmarkCloud)));
}
//----------------------------------------------------------------------------
enum SURFACE_PROJ_ID
//----------------------------------------------------------------------------
{
  ID_CHOOSE = MINID,
};
//----------------------------------------------------------------------------
void lhpOpSolidify::OpRun()
//----------------------------------------------------------------------------
{  
  if(!m_TestMode)
  {
    // interface:
    m_Gui = new mafGUI(this);
    m_Gui->SetListener(this);
    m_Gui->Label(_("source :"),true);
    m_Gui->Label(&m_SourceName);
    m_Gui->Button(ID_CHOOSE,_("source "));
    m_Gui->Label("");
    m_Gui->OkCancel();
    m_Gui->Enable(wxOK, false);

    ShowGui();
  }

  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void lhpOpSolidify::OpDo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    m_Output->ReparentTo(m_Input->GetParent());
    //mafEventMacro(mafEvent(this, VME_ADD, m_Output));
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void lhpOpSolidify::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_Output));
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}

//----------------------------------------------------------------------------
void lhpOpSolidify::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  if (result == OP_RUN_CANCEL)
  {
    mafEventMacro(mafEvent(this,result));
    return;
  }

  std::vector<mafTimeStamp> stmps;
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);
  mafVMELandmarkCloud *newcloud;// = mafVMELandmarkCloud::New();
  mafString           ncname;
  ncname.Append(cloud->GetName());
  ncname.Append("_solidified_with_");
  ncname.Append(m_SourceName);
  mafNEW(newcloud);
  newcloud->SetName(ncname);
  //newcloud->DeepCopy(cloud);
  cloud->GetTimeStamps(stmps);
  std::vector<V3d<double> > gold;
  std::vector<std::vector<V3d<double> > > motion;
  std::vector<std::vector<bool> >         visibl;
  gold.resize(m_Source->GetNumberOfLandmarks());
  for(int i = 0; i < m_Source->GetNumberOfLandmarks(); i++)
  {
    m_Source->GetLandmark(i, gold[i].components);
  }
  motion.resize(stmps.size());
  visibl.resize(stmps.size());
  for(int i = 0; i < stmps.size(); i++)
  {
    mafMatrix m;
    cloud->GetOutput()->GetMatrix(m, stmps[i]);
    m.SetTimeStamp(stmps[i]);
    //newcloud->SetMatrix(m);
    motion[i].resize(cloud->GetNumberOfLandmarks());
    visibl[i].resize(cloud->GetNumberOfLandmarks());
    for(int j = 0; j < cloud->GetNumberOfLandmarks(); j++)
    {
      V3d<double> lm;
      cloud->GetLandmark(j, lm.components, stmps[i]);
      V4d<double> lm4, lm41;
      lm4 = V4d<double>(lm[0], lm[1], lm[2], 1.0);
      m.MultiplyPoint(lm4.components, lm41.components);
      motion[i][j] = V3d<double>(lm41.components);
      visibl[i][j] = cloud->GetLandmarkVisibility(j, stmps[i]);
    }
  }
  m_SolidificationAlgorithm(gold, motion, visibl);
  for(int i = 0; i < stmps.size(); i++)
  {
    for(int j = 0; j < cloud->GetNumberOfLandmarks(); j++)
    {
      V3d<double> lm = motion[i][j];
      if(newcloud->FindLandmarkIndex(cloud->GetLandmarkName(j)) == -1)
        newcloud->AppendLandmark(cloud->GetLandmarkName(j));
      newcloud->SetLandmark(cloud->GetLandmarkName(j), lm[0], lm[1], lm[2], stmps[i]);
      newcloud->SetLandmarkVisibility(cloud->GetLandmarkName(j), visibl[i][j], stmps[i]);
    }
  }
  newcloud->Modified();
  newcloud->Update();
  //newcloud->Register(this);
  m_Output = newcloud;

  mafEventMacro(mafEvent(this,result));
}


//----------------------------------------------------------------------------
void lhpOpSolidify::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_CHOOSE:
        {
          mafString s(_("Choose solid landmark"));
          mafEvent e(this,VME_CHOOSE, &s, (long)&lhpOpSolidify::RigidBodyAccept);
          mafEventMacro(e);
          if(e.GetVme() == NULL)
          {
            return;
          }
          mafNode *sel = e.GetVme();
          if(mafVMELandmarkCloud::SafeDownCast(sel)== NULL)
          {
            wxMessageBox("Selected VME should be mafVMELandmarkCloud.","Warning", wxOK|wxICON_WARNING , NULL);
            return;
          }

          m_Source = mafVMELandmarkCloud::SafeDownCast(sel);
          SetNodeName(m_Source, &m_SourceName);
          m_Gui->Enable(wxOK, true);
          m_Gui->Update();
        }
        break;
      case wxOK:
        if(m_Source == NULL)
          break;
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
void lhpOpSolidify::SetNodeName(mafVME *pVME, mafString *pName) 
//----------------------------------------------------------------------------
{
  *pName = pVME->GetName();
  if(pVME->GetParent() != NULL)
  {
    *pName = *pName + " parent:";
    *pName = *pName + pVME->GetParent()->GetName().GetCStr();
  }
}
