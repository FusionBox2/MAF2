/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpMergeClouds.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-19 11:42:56 $
  Version:   $Revision: 1.3 $
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

#include "lhpOpMergeClouds.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafGUI.h"
#include "mafVMESurface.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"
#include "mafNodeIterator.h"
#include "mafVMELandmarkCloud.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

#include "mafVectors.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpMergeClouds);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
 lhpOpMergeClouds:: lhpOpMergeClouds(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType       = OPTYPE_OP;
  m_Canundo      = false;
  m_MergeClouds.clear();
}

//----------------------------------------------------------------------------
 lhpOpMergeClouds::~lhpOpMergeClouds()
//----------------------------------------------------------------------------
{
}  

//----------------------------------------------------------------------------
bool lhpOpMergeClouds::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(vme == NULL)
    return false;
  if(mafVME::SafeDownCast(vme)== NULL)
    return false;
  return true;
}

//----------------------------------------------------------------------------
mafOp * lhpOpMergeClouds::Copy()
//----------------------------------------------------------------------------
{
  lhpOpMergeClouds *cp = new  lhpOpMergeClouds(GetLabel());
  return cp; 
}


//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void lhpOpMergeClouds::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  m_Gui->Label(_L("Surfaces"), true);
  m_ListBox = m_Gui->ListBox(ID_MERGE_CLOUD/*LISTBOX*/);

  for(int j = 0; j < m_MergeClouds.size();j++)
  {
    m_ListBox->Append(m_MergeClouds[j]->GetName().toWx());
  }


  m_Gui->Button(ID_ADD_CLOUD, _L("Add"), _R(""), _R(""));
  m_Gui->Button(ID_REMOVE_CLOUD, _L("Remove"), _R(""), _R(""));

  m_Gui->OkCancel();
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpMergeClouds::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
  case wxOK:          
    { 
      OpStop(OP_RUN_OK);
    }
    break;
  case wxCANCEL:
    {    
      OpStop(OP_RUN_CANCEL);
    }
    break;
  case ID_ADD_CLOUD:
    {
      /*if(m_ListBox->GetCount()!=0)
      {
      wxMessageBox("Current max point number is one!");
      return;
      }*/
      mafString s(_R("Choose surface to join"));
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      mafNode *sel = e.GetVme();
      if(mafVME::SafeDownCast(sel)== NULL)
      {
        mafWarningMessage(_M("Selected VME is not acceptable."));
        return;
      }
      if(sel == m_Input)
      {
        mafWarningMessage(_M("Selected VME should not be the same as Input."));
        return;
      }

      mafString t;
      t = mafVME::SafeDownCast(sel)->GetName();
      m_ListBox->Append(_(t.toWx()));
      m_ListBox->SetStringSelection(_(t.toWx()));
      m_MergeClouds.push_back(mafVME::SafeDownCast(sel));


      m_ListBox->Update();
      m_Gui->Update();
    }
    break;
  case ID_REMOVE_CLOUD:
    { 
      if(m_ListBox->GetCount()!=0)
      {
        int number = m_ListBox->GetSelection();
        if(number != -1)
        {
          m_ListBox->Delete(number);
          for(int i = number + 1; i < m_MergeClouds.size();i++)
          {
            m_MergeClouds[i - 1] = m_MergeClouds[i];
          }
          m_MergeClouds.resize(m_MergeClouds.size());
          m_Gui->Update();
        }

      }
    }
    break;

  /*case ID_JOINSURF:
    {
      mafString s("Choose surface to join");
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      mafNode *sel = e.GetVme();
      if(mafVME::SafeDownCast(sel)== NULL || !mafVME::SafeDownCast(sel)->GetOutput()->IsMAFType(mafVMEOutputSurface))
      {
        wxMessageBox("Selected VME should have mafVMEOutputSurface as Output.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }

      m_JoinSurf = mafVME::SafeDownCast(sel);
      SetNodeName(m_JoinSurf, &m_JoinSurfName);
      m_Gui->Update();
    }
    break;*/
  default:
    {
      mafEventMacro(*maf_event); 
    }
    break;
  }
}


//struct _edge
//{
//  vtkIdType v1, v2;
//  bool      inverse;
//};
//----------------------------------------------------------------------------
void lhpOpMergeClouds::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  if (result == OP_RUN_CANCEL)
  {
    mafEventMacro(mafEvent(this,result));
    return;
  }


  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void lhpOpMergeClouds::SetNodeName(mafVME *pVME, mafString *pName) 
//----------------------------------------------------------------------------
{
  *pName = pVME->GetName();
  if(pVME->GetParent() != NULL)
  {
    *pName += _R(" parent:");
    *pName += pVME->GetParent()->GetName();
  }
}




//----------------------------------------------------------------------------
void  lhpOpMergeClouds::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
}

void lhpOpMergeClouds::MergeNodes(mafVME *vme1, mafVME *vme2, const mafMatrix &transf)
{
  mafVMELandmarkCloud *cloud1 = mafVMELandmarkCloud::SafeDownCast(vme1);
  mafVMELandmarkCloud *cloud2 = mafVMELandmarkCloud::SafeDownCast(vme2);
  if(cloud1 == NULL || cloud2 == NULL)
    return;
  if(cloud1->GetNumberOfLandmarks() != cloud2->GetNumberOfLandmarks())
    return;
  bool opened1 = cloud1->IsOpen();
  bool opened2 = cloud1->IsOpen();
  if(opened1)
    cloud1->Close();
  if(opened2)
    cloud2->Close();
  std::vector<mafTimeStamp> stamps1;
  cloud1->GetLocalTimeStamps(stamps1);
  mafTimeStamp shift = 1.0;
  if(stamps1.size() > 1)
    shift = 2 * stamps1[stamps1.size() - 1] - stamps1[stamps1.size() - 2];
  else if(stamps1.size() > 0)
    shift = stamps1[stamps1.size() - 1] + 1.0;

  std::vector<mafTimeStamp> mstamps2;
  std::vector<mafTimeStamp> dstamps2;
  cloud2->GetMatrixTimeStamps(mstamps2);
  cloud2->GetDataTimeStamps(dstamps2);


  for(int i = 0; i < mstamps2.size(); i++)
  {
    mafMatrix m;
    cloud2->GetOutput()->GetMatrix(m, mstamps2[i]);
    m.SetTimeStamp(mstamps2[i] + shift);
    cloud1->SetMatrix(m);
  }
  for(int i = 0; i < dstamps2.size(); i++)
  {
    for(unsigned j = 0; j < cloud2->GetNumberOfLandmarks(); j++)
    {
      double pos[3];
      bool vis;
      cloud2->GetLandmarkPosition(j, pos, dstamps2[i]);
      vis = cloud2->GetLandmarkVisibility(j, dstamps2[i]);
      cloud1->SetLandmark(j, pos[0], pos[1], pos[2], dstamps2[i] + shift);
      cloud1->SetLandmarkVisibility(j, vis, dstamps2[i] + shift);
    }
  }
  if(opened2)
    cloud2->Open();
  if(opened1)
    cloud1->Open();
}


//----------------------------------------------------------------------------
void lhpOpMergeClouds::OpDo()   
//----------------------------------------------------------------------------
{
  for(int i = 0; i < m_MergeClouds.size(); i++)
  {
    mafNodeIterator *iter1 = m_Input->NewIterator();
    mafNodeIterator *iter2 = m_MergeClouds[i]->NewIterator();
    mafMatrix m;
    mafNode *node1, *node2;
    for (node1 = iter1->GetFirstNode(), node2 = iter2->GetFirstNode(); node1 && node2; node1 = iter1->GetNextNode(), node2 = iter2->GetNextNode())
      MergeNodes(mafVME::SafeDownCast(node1), mafVME::SafeDownCast(node2), m);
    iter2->Delete();
    iter1->Delete();
  }
  //if(m_OutSurface)
  {
    //m_OutSurface->ReparentTo(m_Input->GetRoot());
    //mafEventMacro(mafEvent(this, VME_ADD, m_OutSurface));
  }
  mafEventMacro(mafEvent(this, VME_MODIFIED, m_Input));
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void lhpOpMergeClouds::OpUndo()   
//----------------------------------------------------------------------------
{
}
