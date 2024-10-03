/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpBuildHierarchy.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:19:11 $
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

#include "lhpOpBuildHierarchy.h"
#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include "wx/busyinfo.h"
#include <math.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafOpExplodeCollapse.h"
#include "mafDictionary.h"

#include "mafTransformFrame.h"
#include "mmuTimeSet.h"
#include "mafVMELandmarkCloud.h"
#include "mafAbsMatrixPipe.h"
#include "ftk/Base/RegisteringPointer.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

//----------------------------------------------------------------------------
// Required for MSVC
//----------------------------------------------------------------------------
#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_DEFAULT = MINID,
  ID_LOAD_HIERARCHY ,
  ID_LOAD_DICTIONARY,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

mafCxxTypeMacro(lhpOpBuildHierarchy)

//----------------------------------------------------------------------------
lhpOpBuildHierarchy::lhpOpBuildHierarchy(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = false;
  m_root    = NULL;
}

//----------------------------------------------------------------------------
lhpOpBuildHierarchy::~lhpOpBuildHierarchy()
//----------------------------------------------------------------------------
{
  Destroy(&m_root);
}

//----------------------------------------------------------------------------
mafOp* lhpOpBuildHierarchy::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpBuildHierarchy(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpBuildHierarchy::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;

  if(!vme->IsA("mafVME"))
  {
    return false;
  }
  
  return true;
}


//----------------------------------------------------------------------------
void lhpOpBuildHierarchy::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
}

//----------------------------------------------------------------------------
void lhpOpBuildHierarchy::CreateGui()
//----------------------------------------------------------------------------
{
  if(m_Gui == NULL)
  {
    m_Gui = new mafGUI(this);
    m_Gui->SetListener(this);
    m_Gui->FileOpen(ID_LOAD_DICTIONARY, _L("Dictionary"),  &m_DictionaryFName);
    m_Gui->Label(_R(""));
    m_Gui->FileOpen(ID_LOAD_HIERARCHY, _L("Hierarchy"),  &m_HierarchyFName);
    m_Gui->Label(_R(""));
    m_Gui->OkCancel();
  }
  ShowGui();
}

//----------------------------------------------------------------------------
/*void lhpOpBuildHierarchy::OpStop(int result)
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
void lhpOpBuildHierarchy::OnEvent(mafEventBase *maf_event) 
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
    case ID_LOAD_DICTIONARY:
    {
      if(!m_DictionaryFName.empty())
      {
        ReadDictionary(&m_DictionaryFName, m_dictionary);
      }
      break;
    }
    case ID_LOAD_HIERARCHY:
    {
      if(!m_HierarchyFName.empty())
      {
        ReadFromFile(m_HierarchyFName);
      }
      break;
    }
    default:
    {
      mafEventMacro(*maf_event); 
    }
    break;
  }
}

#define OLDVERSION
//----------------------------------------------------------------------------
static void makeReparent(mafVME *child, mafVME *newParent)
//----------------------------------------------------------------------------
{
  int num, t;
  mmuTimeVector input_time;
  mmuTimeVector target_time;
  mafTimeStamp  cTime, startTime;
  mafVME        *oldParent;

  oldParent = mafVME::SafeDownCast(child->GetParent());

  child->GetAbsTimeStamps(input_time);
  newParent->GetAbsTimeStamps(target_time);
  mmuTimeVector time = mmuTimeSet::Merge(input_time,target_time);
  num = time.size();

  startTime = newParent->GetTimeStamp();

  std::vector< std::shared_ptr<mafMatrix> > new_input_pose;
  new_input_pose.resize(num);

  for (t = 0; t < num; t++)
  {
    new_input_pose[t] = mafMatrix::NewSPtr();
  }

  //change reference system
  auto transform = mafTransformFrame::NewSPtr();
  for (t = 0; t < num; t++)
  {
    cTime = time[t];

    child->SetTimeStamp(cTime);
    newParent->SetTimeStamp(cTime);
    oldParent->SetTimeStamp(cTime);
#ifdef OLDVERSION
    transform->SetTimeStamp(cTime);
    transform->SetInput(child->GetMatrixPipe());
    transform->SetInputFrame(oldParent->GetAbsMatrixPipe());
    transform->SetTargetFrame(newParent->GetAbsMatrixPipe());
    transform->Update();

    new_input_pose[t]->DeepCopy(*transform->GetMatrixPointer());
#else
    mafMatrix mtr;
    child->GetOutput()->GetAbsMatrix(mtr, cTime);
    new_input_pose[t]->DeepCopy(&mtr);
#endif

  }

  child->SetTimeStamp(startTime);
  newParent->SetTimeStamp(startTime);
  oldParent->SetTimeStamp(startTime);
#ifdef OLDVERSION
  for (t = 0; t < num; t++)
  {
    child->SetMatrix(*new_input_pose[t]);
  }
#endif
  if (child->ReparentTo(newParent) == MAF_OK)
  {
    //{mafEvent evUnq(this,CAMERA_UPDATE); mafEventMacro(evUnq);}
  }
  else
  {
    mafLogMessage(_M("Something went wrong while reparenting (bad pointer or memory errors)")); 
  }
#ifndef OLDVERSION
  for (t = 0; t < num; t++)
  {
    child->SetAbsMatrix(*new_input_pose[t], time[t]);
  }
#endif
}

//----------------------------------------------------------------------------
static void reparentAll(lhpOpBuildHierarchy::mafFrame *pRoot, mafVME *root)
//----------------------------------------------------------------------------
{
  lhpOpBuildHierarchy::mafFrame *pNext;
  if(pRoot == NULL)
    return;
  if(pRoot->GetVME() != NULL)
    makeReparent(pRoot->GetVME(), root);

  reparentAll(pRoot->GetChild(), root);

  for(pNext = pRoot->GetNext(); pNext != NULL; pNext = pNext->GetNext())
  {
    reparentAll(pNext, root);
  }
}


//----------------------------------------------------------------------------
static bool searchVMEInTree(lhpOpBuildHierarchy::mafFrame *pRoot, mafVME *search)
//----------------------------------------------------------------------------
{
  lhpOpBuildHierarchy::mafFrame *pNext;
  if(pRoot == NULL)
    return false;
  if(pRoot->GetVME() == search)
    return true;
  if(searchVMEInTree(pRoot->GetChild(), search))
    return true;

  for(pNext = pRoot->GetNext(); pNext != NULL; pNext = pNext->GetNext())
  {
    if(searchVMEInTree(pNext, search))
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------
static void restoreRootPlaces(lhpOpBuildHierarchy::mafFrame *pRoot, mafVME *input)
//----------------------------------------------------------------------------
{
  lhpOpBuildHierarchy::mafFrame *pNext;
  if(pRoot == NULL)
    return;
  if(pRoot->GetVME() == NULL)
    restoreRootPlaces(pRoot->GetChild(), input);
  else
  {
    if(!searchVMEInTree(pRoot->GetChild(), pRoot->GetParentVME()))
      makeReparent(pRoot->GetVME(), pRoot->GetParentVME());
    if(!searchVMEInTree(pRoot->GetChild(), input))
      makeReparent(pRoot->GetVME(), input);
  }

  for(pNext = pRoot->GetNext(); pNext != NULL; pNext = pNext->GetNext())
  {
    restoreRootPlaces(pNext, input);
  }
}
//----------------------------------------------------------------------------
static bool checkPossibility(lhpOpBuildHierarchy::mafFrame *pRoot)
//----------------------------------------------------------------------------
{
  lhpOpBuildHierarchy::mafFrame *pNext;
  if(pRoot == NULL)
    return true;
  if(pRoot->GetVME() == NULL)
  {
    if(pRoot->GetParent() != NULL && pRoot->GetParent()->GetVME() != NULL && !pRoot->GetVME()->CanReparentTo(pRoot->GetParent()->GetVME()))
      return false;
  }
  if(!checkPossibility(pRoot->GetChild()))
    return false;

  for(pNext = pRoot->GetNext(); pNext != NULL; pNext = pNext->GetNext())
  {
    if(!checkPossibility(pNext))
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------
static void hierarchyReparent(lhpOpBuildHierarchy::mafFrame *pRoot, mafVME *parent)
//----------------------------------------------------------------------------
{
  lhpOpBuildHierarchy::mafFrame *pNext;
  if(pRoot == NULL)
    return;

  if(parent != NULL)
  if(pRoot->GetVME() != NULL)
    makeReparent(pRoot->GetVME(), parent);

  hierarchyReparent(pRoot->GetChild(), pRoot->GetVME());

  for(pNext = pRoot->GetNext(); pNext != NULL; pNext = pNext->GetNext())
  {
    hierarchyReparent(pNext, parent);
  }
}


//----------------------------------------------------------------------------
void lhpOpBuildHierarchy::OpDo()
//----------------------------------------------------------------------------
{
  BindToVME((mafVME*)m_Input, m_root);

  mafVME *pVMERoot = (mafVME *)m_Input->GetRoot();

  if(!checkPossibility(m_root))
  {
    wxMessageBox(wxString::Format("Hierarchy contains impossible relation."), "Warning.", wxOK | wxCENTRE | wxICON_WARNING);
    return;
  }

  reparentAll(m_root, pVMERoot);
  hierarchyReparent(m_root, NULL);
  restoreRootPlaces(m_root, (mafVME*)m_Input);
  {mafEvent evUnq(this,CAMERA_UPDATE); mafEventMacro(evUnq);}
  return;
}

//----------------------------------------------------------------------------
void lhpOpBuildHierarchy::OpUndo()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool lhpOpBuildHierarchy::ReadFromFile(const mafString& fileName)
//----------------------------------------------------------------------------
{
  wxInt32                                      nI; 
  mafFrame                                     *pParentFrame;
  mafFrame                                     *pChildFrame;
  mafString                                    fname(fileName);
  std::vector<std::pair<mafString, mafString> >  hierContent;

  if(m_dictionary.size() == 0)
  {
    if(wxNO == wxMessageBox(wxString::Format("Dictionary is not loaded yet. Are you sure you want to continue?"), "Warning.", wxYES_NO | wxCENTRE | wxICON_WARNING))
    {
      return false;
    }
  }

  ReadDictionary(&fname, hierContent);
  if(hierContent.size() == 0)
  {
    wxMessageBox(wxString::Format("Hierarchy is empty, nothing to do."), "Warning.", wxOK | wxCENTRE | wxICON_WARNING);
    return false;
  }

  //cleanup old if any
  Destroy(&m_root);

  for(nI = 0; nI < hierContent.size(); )
  {
    if(hierContent[nI].second.empty() || hierContent[nI].first.empty())
    {
      //consider string in valid
      nI++;
      continue;
    }
    //create parent if needed
    bool addRoot = false;
    pParentFrame = FindFrame(hierContent[nI].first, FALSE);
    if((pParentFrame == NULL) ^ (m_root == NULL))
    {
      addRoot = true;
    }
    pParentFrame = FindFrame(hierContent[nI].first, TRUE);
    pChildFrame  = FindFrame(hierContent[nI].second, TRUE);
    if(pChildFrame->GetNext() != NULL  || pChildFrame->GetChild() != NULL)
    {
      mafErrorMessage(_M(mafString::Format(_R("Error in line %d: Bone "), nI + 1) + hierContent[nI].second + _R(" is already in hierarchy")));
      cppDEL(pChildFrame);
      cppDEL(pParentFrame);
      Destroy(&m_root);
      return true;
    }
    if(pChildFrame->GetParent() != NULL)
    {
      mafErrorMessage(_M(mafString::Format(_R("Error in line %d: Bone "), nI + 1) + hierContent[nI].second + _R(" already has a parent")));
      cppDEL(pChildFrame);
      cppDEL(pParentFrame);
      Destroy(&m_root);
      return true;
    }
    if(m_root == NULL)
    {
      m_root = pParentFrame;
    }
    else if(addRoot)
    {
      pParentFrame->SetNext(m_root);
      m_root = pParentFrame;
    }
    if(pParentFrame->GetChild() == NULL)
    {
      //we are first child
      pParentFrame->SetChild(pChildFrame);
    }
    else
    {
      pChildFrame->SetNext(pParentFrame->GetChild());
      pParentFrame->SetChild(pChildFrame);
    }
    pChildFrame->SetParent(pParentFrame);
    // to next
    nI++;
  }
  return true;
}

//----------------------------------------------------------------------------
void  lhpOpBuildHierarchy::Destroy(mafFrame **root)
//----------------------------------------------------------------------------
{
  mafFrame *pCurrent;

  if(*root == NULL)
  {
    cppDEL(m_root);
  }
  else
  {
    if((*root)->GetParent() != NULL)
    {
      if((*root)->GetParent()->GetChild() == (*root))
      {
        (*root)->GetParent()->SetChild(NULL);
      }
      else
      {
        for(pCurrent = (*root)->GetParent()->GetChild(); pCurrent != NULL; pCurrent = pCurrent->GetNext())
        {
          if(pCurrent->GetNext() == (*root))
          {
            //fount it
            pCurrent->SetNext(NULL);
            break;
          }
        }
      }
    }
    cppDEL(*root);
  }
}
//----------------------------------------------------------------------------
void lhpOpBuildHierarchy::BindToVME(mafVME *pvme, lhpOpBuildHierarchy::mafFrame *pStart)
//----------------------------------------------------------------------------
{
  //ensure we have root
  mafVME *pVMERoot = pvme;//(mafVME *)pvme->GetRoot();
  mafVME *pFoundVME;
  mafFrame *pCur;

  if (pStart == NULL)
  {
    pStart = m_root;
  }

  if(pStart == NULL)
  {
    //nothing to do
    return;
  }

  pFoundVME = NULL;
  auto pVMENameStr = LookupUserName(pStart->GetName(), m_dictionary);
  if(pVMENameStr != NULL)
  {
    pFoundVME = (mafVME*)pVMERoot->FindInTreeByName(*pVMENameStr);
  }
  //and again ^_^
  if(pFoundVME == NULL)
  {
    pVMENameStr = LookupStdName(pStart->GetName(), m_dictionary);
    if(pVMENameStr != NULL)
      pFoundVME = (mafVME*)pVMERoot->FindInTreeByName(*pVMENameStr);
  }
  //try again in case of failure
  if(pFoundVME == NULL)
  {
    pFoundVME = (mafVME*)pVMERoot->FindInTreeByName(pStart->GetName());
  }

  pStart->SetVME(pFoundVME);
  if(pFoundVME != NULL)
    pStart->SetParentVME(pFoundVME->GetParent());
  if(LookupStdName(pStart->GetName(), m_dictionary) != NULL)
  {
    //pStart->SetID(GetBoneIDByName(*LookupStdName(pStart->GetName())));
  }
  else
  {
    //pStart->SetID(GetBoneIDByName(wxString(pStart->GetName()->GetData())));
  }
  //to all children
  if(pStart->GetChild() != NULL)
    BindToVME(pvme, pStart->GetChild());
  for(pCur = pStart->GetNext(); pCur != NULL; pCur = pCur->GetNext())
  {
    BindToVME(pvme, pCur);
  }
}


//----------------------------------------------------------------------------
lhpOpBuildHierarchy::mafFrame::mafFrame()
//----------------------------------------------------------------------------
{
  //m_landmarks = new LandmarkArray();

  m_next   = NULL;
  m_vme    = NULL;
  m_name   = _R("");
  m_parent = NULL;
  m_child  = NULL;
}

//----------------------------------------------------------------------------
lhpOpBuildHierarchy::mafFrame::~mafFrame()
//----------------------------------------------------------------------------
{
  cppDEL(m_next);
  cppDEL(m_child);
  m_parent = NULL;
  m_vme    = NULL;  
}


//----------------------------------------------------------------------------
lhpOpBuildHierarchy::mafFrame *lhpOpBuildHierarchy::FindFrameUsingDictionary(const mafString& str, bool bCreateIfNotFound)    
//----------------------------------------------------------------------------
{
  mafFrame *pRet = NULL;
  mafString const *pStr;

  pRet = FindFrame(str, bCreateIfNotFound);

  if(pRet == NULL)
  {
    pStr = LookupUserName(str, m_dictionary);
    if(pStr != NULL)
      pRet = FindFrame(*pStr, bCreateIfNotFound);
    if(pRet == NULL)
    {
      pStr = LookupStdName(str, m_dictionary);
      if(pStr != NULL)
        pRet = FindFrame(*pStr, bCreateIfNotFound);
    }
  }

  return (pRet);
}
//----------------------------------------------------------------------------
lhpOpBuildHierarchy::mafFrame *lhpOpBuildHierarchy::FindFrame(const mafString& str, bool bCreateIfNotFound)
//----------------------------------------------------------------------------
{
  mafFrame *pRet = NULL;

  if(m_root != NULL)
  {
    pRet = FindFrame(m_root, str);
  }

  if(bCreateIfNotFound && pRet == NULL)
  {
    pRet = new mafFrame();
    pRet->SetName(str);
  }

  return (pRet);
}
//----------------------------------------------------------------------------
lhpOpBuildHierarchy::mafFrame *lhpOpBuildHierarchy::FindFrame(lhpOpBuildHierarchy::mafFrame *pRoot, const mafString& str)    
//----------------------------------------------------------------------------
{
  mafFrame *pRet;
  mafFrame *pNext;

  if(pRoot == NULL)
  {
    return (NULL);
  }
  //check this
  if(pRoot->GetName() == str)
  {
    return (pRoot);
  }

  //look into children
  pRet = FindFrame(pRoot->GetChild(), str);
  if(pRet != NULL)
  {
    return (pRet);
  }

  //look into siblings
  for(pNext = pRoot->GetNext(); pNext != NULL && pRet == NULL; pNext = pNext->GetNext())
  {
    pRet = FindFrame(pNext, str);
  }

  return (pRet);
}
