/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRegSurfWithCloud.cpp,v $
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

#include "lhpOpRegSurfWithCloud.h"

#include "mafDecl.h"
#include <wx/busyinfo.h>

#include "mafEvent.h"
#include "mafGUI.h"

#include "mafGUIDialog.h"

#include "mafVME.h"
#include "mafSmartPointer.h"
#include "mafVMELandmark.h"
#include "mafDictionary.h"

#include "vtkMAFSmartPointer.h"
#include "mafMatrixVector.h"
#include "mafAbsMatrixPipe.h"
#include "mafNodeIterator.h"


#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpRegSurfWithCloud);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpRegSurfWithCloud::lhpOpRegSurfWithCloud(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType      = OPTYPE_OP;
  m_Canundo     = true;
  m_Source      = NULL;
  m_Resultat    = NULL;
  m_SourceName  = "none";
  m_MultiTime   = true;
  m_ScriptFName = "";
}
//----------------------------------------------------------------------------
lhpOpRegSurfWithCloud::~lhpOpRegSurfWithCloud( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Resultat);
}
//----------------------------------------------------------------------------
mafOp* lhpOpRegSurfWithCloud::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpRegSurfWithCloud(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpRegSurfWithCloud::Accept(mafNode* node)
//----------------------------------------------------------------------------
{
  return (node != NULL);// && node->IsMAFType(mafVMESurface));
};
//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_CHOOSE = MINID,
  ID_MULTIPLE_TIME_REGISTRATION,
  ID_LOAD_SCRIPT
};
//----------------------------------------------------------------------------
void lhpOpRegSurfWithCloud::OpRun()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Label(_("source :"),true);
  m_Gui->Label(&m_SourceName);

  m_Gui->Button(ID_CHOOSE,_("source "));

  m_Gui->FileOpen(ID_LOAD_SCRIPT, "Load reg list",  &m_ScriptFName, "*.txt");
  //m_Gui->Bool(ID_MULTIPLE_TIME_REGISTRATION,_("multi-time"),&m_MultiTime,1);
  //m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,false);

  m_Gui->OkCancel();

  m_Gui->Enable(wxOK,false);
  m_Gui->Divider();
  ShowGui();
}
//----------------------------------------------------------------------------
void lhpOpRegSurfWithCloud::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_LOAD_SCRIPT:
      {
        if(m_ScriptFName != "")
          ReadDictionary(&m_ScriptFName, m_LMDict);
        break;
      }
      case ID_CHOOSE:
      {
        mafString s(_("Choose cloud"));
        mafEvent e(this,VME_CHOOSE, &s, (long)&lhpOpRegSurfWithCloud::ClosedCloudAccept);
        mafEventMacro(e);
        mafNode *vme = e.GetVme();
        OnChooseVme(vme);
        break;
      }
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
void lhpOpRegSurfWithCloud::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    mafEventMacro(mafEvent(this,result));
    return;
  }

  wxBusyInfo wait(_("Please wait, working..."));

  if(m_Resultat)
    mafDEL(m_Resultat);
  
  mafVMEGroup *grp;
  mafNEW(grp);
  m_Resultat = grp;
  
  /*if(m_Resultat->CanCopy(m_Input))
    m_Resultat->DeepCopy(m_Input);
  else
  {
    wxMessageBox(_("Bad follower!"), _("Alert"), wxOK, NULL);
    mafDEL(m_Resultat);
    return;
  }*/

  for(unsigned s = 0; s < m_LMDict.size(); s++)
  {
    mafVMESurface       *surf  = NULL;
    mafVMELandmarkCloud *cloud = NULL;

    for(unsigned i = 0; i < m_Input->GetNumberOfChildren(); i++)
    {
      mafVMESurface *surfChild = mafVMESurface::SafeDownCast(m_Input->GetChild(i));
      if(surfChild == NULL || strcmp(surfChild->GetName(), m_LMDict[s].first.c_str()) != 0)
        continue;
      mafNEW(surf);
      if(surf->CanCopy(surfChild))
        surf->DeepCopy(surfChild);
      else
        mafDEL(surf);
      break;
    }
    if(surf == NULL)
      continue;

    mafNodeIterator *iter = mafNodeIterator::New();
    iter->SetRootNode(m_Source);
    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      mafVMELandmarkCloud *cloudChild = mafVMELandmarkCloud::SafeDownCast(node);
      if(cloudChild != NULL && strcmp(cloudChild->GetName(), m_LMDict[s].second.c_str()) == 0)
      {
        cloud = cloudChild;
        break;
      }
    }
    iter->Delete();

    /*for(unsigned i = 0; i < m_Source->GetNumberOfChildren(); i++)
    {
      mafVMELandmarkCloud *cloudChild = mafVMELandmarkCloud::SafeDownCast(m_Source->GetChild(i));
      if(cloudChild != NULL && strcmp(cloudChild->GetName(), m_LMDict[s].second.c_str()) == 0)
      {
        cloud = cloudChild;
        break;
      }
    }*/
    if(cloud == NULL)
    {
      mafDEL(surf);
      continue;
    }
    if(m_MultiTime)
    {
      std::vector<mafTimeStamp> timeStamps;
      cloud->GetLocalTimeStamps(timeStamps);
      if(timeStamps.size() > 0)
        surf->GetMatrixVector()->RemoveAllItems();
      for(unsigned i = 0; i < timeStamps.size(); i++)
      {
        //surf->SetTimeStamp(timeStamps[i]);
        mafMatrix folMatrix;
        cloud->GetOutput()->GetAbsMatrix(folMatrix, timeStamps[i]);
        surf->SetAbsMatrix(folMatrix, timeStamps[i]);
        surf->Modified();
        surf->Update();
      }
    }
    else
    {
      mafTimeStamp ts = cloud->GetTimeStamp();
      //surf->SetTimeStamp(ts);
      surf->GetMatrixVector()->RemoveAllItems();
      mafMatrix folMatrix;
      cloud->GetOutput()->GetAbsMatrix(folMatrix, ts);
      surf->SetAbsMatrix(folMatrix, ts);
      surf->Modified();
      surf->Update();
    }
    surf->ReparentTo(m_Resultat);
    mafDEL(surf);
  }

  wxString name = wxString::Format("%s registered on %s", m_Resultat->GetName(), m_Source->GetName());
  m_Resultat->SetName(name);
  m_Resultat->ReparentTo(m_Input->GetParent());

  HideGui();
  mafEventMacro(mafEvent(this,result));

}

//----------------------------------------------------------------------------
void lhpOpRegSurfWithCloud::OpDo()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, VME_ADD, m_Resultat));
}
//----------------------------------------------------------------------------
void lhpOpRegSurfWithCloud::OpUndo()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, VME_REMOVE, m_Resultat));
}

//----------------------------------------------------------------------------
void lhpOpRegSurfWithCloud::OnChooseVme(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(!vme) // user choose cancel - keep everything as before
    return;

  //if(vme->IsA("mafVMELandmarkCloud"))
  {
    //m_Source = (mafVMELandmarkCloud *)vme;
    m_Source = mafVME::SafeDownCast(vme);
    if(!m_Source)
      return;
    m_SourceName = m_Source->GetName();
    /*if(m_Source->IsAnimated())
      m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,true);*/
    m_Gui->Enable(wxOK,true);
  }
  /*else
  {
    wxMessageBox(_("Bad vme type!"), _("Alert"), wxOK, NULL);
    return;
  }*/
  m_Gui->Update();
}
