/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoBuildHierarchy.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-10 17:30:46 $
  Version:   $Revision: 1.1 $
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

#ifdef __GNUG__
    #pragma implementation "mmoBuildHierarchy.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include <wx/wxprec.h>
#include <math.h>
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafOp.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mmoBuildHierarchy.h"
#include "mmoExplodeCollapse.H"

#include "mafVMELandmarkCloud.h"
#include "mafAbsMatrixPipe.h"
#include "mafSmartPointer.h"
#include "mafTransformFrame.h"
#include "mmoBuildHierarchy.h"
#include "mmuTimeSet.h"

#include "mafVME.h"
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


//----------------------------------------------------------------------------
mmoBuildHierarchy::mmoBuildHierarchy(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = false;
  m_root    = NULL;
}

//----------------------------------------------------------------------------
mmoBuildHierarchy::~mmoBuildHierarchy( ) 
//----------------------------------------------------------------------------
{
  Destroy(&m_root);
}

//----------------------------------------------------------------------------
mafOp* mmoBuildHierarchy::Copy()   
//----------------------------------------------------------------------------
{
  return new mmoBuildHierarchy(m_Label);
}

//----------------------------------------------------------------------------
bool mmoBuildHierarchy::Accept(mafNode* vme)
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
void mmoBuildHierarchy::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
}

//----------------------------------------------------------------------------
void mmoBuildHierarchy::CreateGui()
//----------------------------------------------------------------------------
{
  if(m_Gui == NULL)
  {
    m_Gui = new mmgGui(this);
    m_Gui->SetListener(this);
    m_Gui->FileOpen(ID_LOAD_DICTIONARY, "Dictionary",  &m_DictionaryFName);
    m_Gui->Label("");
    m_Gui->FileOpen(ID_LOAD_HIERARCHY, "Hierarchy",  &m_HierarchyFName);
    m_Gui->Label("");
    m_Gui->OkCancel();
  }
  ShowGui();
}

//----------------------------------------------------------------------------
void mmoBuildHierarchy::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    mafEventMacro(mafEvent(this,result));
  }
  else if (result == OP_RUN_OK)
  {
    HideGui();
    mafEventMacro(mafEvent(this,result));
  }
}
//----------------------------------------------------------------------------
void mmoBuildHierarchy::OnEvent(mafEventBase *maf_event) 
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
      if(m_DictionaryFName != "")
      {
        ReadLandmarksDictionary(m_DictionaryFName.GetCStr());
      }
      break;
    }
    case ID_LOAD_HIERARCHY:
    {
      if(m_HierarchyFName != "")
      {
        ReadFromFile(m_HierarchyFName.GetCStr());
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
void makeReparent(mafVME *child, mafVME *newParent)
{
  int num, t;
  mmuTimeVector input_time;
  mmuTimeVector target_time;
  mmuTimeVector time;
  mafTimeStamp  cTime, startTime;
  mafVME        *oldParent;

  oldParent = mafVME::SafeDownCast(child->GetParent());

  child->GetAbsTimeStamps(input_time);
  newParent->GetAbsTimeStamps(target_time);
  mmuTimeSet::Merge(input_time,target_time,time);
  num = time.size();

  startTime = newParent->GetTimeStamp();

  std::vector< mafAutoPointer<mafMatrix> > new_input_pose;
  new_input_pose.resize(num);

  for (t = 0; t < num; t++)
  {
    new_input_pose[t] = mafMatrix::New();
  }

  //change reference system
  mafSmartPointer<mafTransformFrame> transform;
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

    new_input_pose[t]->DeepCopy(transform->GetMatrixPointer());
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
    //mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
  else
  {
    mafLogMessage("Something went wrong while reparenting (bad pointer or memory errors)"); 
  }
#ifndef OLDVERSION
  for (t = 0; t < num; t++)
  {
    child->SetAbsMatrix(*new_input_pose[t], time[t]);
  }
#endif
}

void reparentAll(mmoBuildHierarchy::mafFrame *pRoot, mafVME *root)
{
  mmoBuildHierarchy::mafFrame *pNext;
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

void restoreRootPlaces(mmoBuildHierarchy::mafFrame *pRoot)
{
  mmoBuildHierarchy::mafFrame *pNext;
  if(pRoot == NULL)
    return;
  if(pRoot->GetVME() == NULL)
    restoreRootPlaces(pRoot->GetChild());
  else
    makeReparent(pRoot->GetVME(), pRoot->GetParentVME());

  for(pNext = pRoot->GetNext(); pNext != NULL; pNext = pNext->GetNext())
  {
    restoreRootPlaces(pNext);
  }
}

void hierarchyReparent(mmoBuildHierarchy::mafFrame *pRoot, mafVME *parent)
{
  mmoBuildHierarchy::mafFrame *pNext;
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
void mmoBuildHierarchy::OpDo()
//----------------------------------------------------------------------------
{
  BindToVME((mafVME*)m_Input, m_root);

  mafVME *pVMERoot = (mafVME *)m_Input->GetRoot();

  reparentAll(m_root, pVMERoot);
  hierarchyReparent(m_root, NULL);
  restoreRootPlaces(m_root);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  return;
}



//----------------------------------------------------------------------------
void mmoBuildHierarchy::OpUndo()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool mmoBuildHierarchy::ReadFromFile(const wxString& fileName)
//----------------------------------------------------------------------------
{
  wxTextFile *pFile;
  wxInt32    nI; 
  wxString   sChild("");
  wxString   sParent("");
  mafFrame   *pParentFrame;
  mafFrame   *pChildFrame;

  if(m_dictionary.size() == 0)
  {
    if(wxNO == wxMessageBox(wxString::Format("Dictionary is not loaded yet. Are you sure you want to continue?"), "Warning.", wxYES_NO | wxCENTRE | wxICON_WARNING))
    {
      return false;
    }
  }

  //cleanup old if any
  Destroy(&m_root);

  pFile = new wxTextFile(fileName);

  if(pFile == NULL)
  {
    return false;
  }
  pFile->Open();
  if(!pFile->IsOpened())
  {
    cppDEL(pFile);
    return false;
  }

  for(nI = 0; nI < pFile->GetLineCount(); )
  {
    wxString &pFirstLine = pFile->GetLine(nI);
    //match it as beginning of block
    if(pFirstLine == "")
    {
      nI++;
      continue;
    }

    ExtractTwoWordsFromString(pFirstLine, sParent, sChild);

    if(sChild == "" || sParent == "")
    {
      //consider string in valid
      nI++;
      continue;
    }
    //create parent if needed
    bool addRoot = false;
    pParentFrame = FindFrame(sParent, FALSE);
    if((pParentFrame == NULL) ^ (m_root == NULL))
    {
      addRoot = true;
      //wxMessageBox(wxString::Format("Error in line %d: Bone %s sets a new root", nI + 1, sParent.GetData()), "Error.", wxOK | wxCENTRE | wxICON_ERROR);
      //cppDEL(pParentFrame);
      //Destroy(&m_root);
      //goto Done;
    }
    pParentFrame = FindFrame(sParent, TRUE);
    pChildFrame  = FindFrame(sChild, TRUE);
    if(pChildFrame->GetNext() != NULL  || pChildFrame->GetChild() != NULL)
    {
      wxMessageBox(wxString::Format("Error in line %d: Bone %s already in hierarchy", nI + 1, sChild.GetData()), "Error.", wxOK | wxCENTRE | wxICON_ERROR);
      cppDEL(pChildFrame);
      cppDEL(pParentFrame);
      Destroy(&m_root);
      goto Done;
    }
    if(pChildFrame->GetParent() != NULL)
    {
      wxMessageBox(wxString::Format("Error in line %d: Bone %s already have a parent", nI + 1, sChild.GetData()), "Error.", wxOK | wxCENTRE | wxICON_ERROR);
      cppDEL(pChildFrame);
      cppDEL(pParentFrame);
      Destroy(&m_root);
      goto Done;
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
Done:
  pFile->Close();
  cppDEL(pFile);
  return true;
}

//----------------------------------------------------------------------------
bool mmoBuildHierarchy::ReadLandmarksDictionary(const wxString &fileName)
//----------------------------------------------------------------------------
{
  wxTextFile   *pFile;
  wxInt32      nI; 
  wxString     sFirstName("");
  wxString     sSecondName("");
  bool         bShowWarnings = TRUE;

  pFile = new wxTextFile(fileName);

  if(pFile == NULL)
  {
    return false;
  }
  pFile->Open();
  if(!pFile->IsOpened())
  {
    cppDEL(pFile);
    return false;
  }

  //clean up old data if any
  m_dictionary.clear();

  for(nI = 0; nI < pFile->GetLineCount(); )
  {
    wxString &pFirstLine = pFile->GetLine(nI);
    //match it as beginning of block
    if(pFirstLine == "" || pFirstLine[0] == '#')
    {
      nI++;
      continue;
    }    

    ExtractTwoWordsFromString(pFirstLine, sFirstName, sSecondName);

    if(sFirstName == "" || sSecondName == "")
    {
      if(bShowWarnings)
        //consider string in invalid
        if(wxCANCEL == wxMessageBox(wxString::Format("Syntax error in file %s, line %d. Ignoring. Press Cancel to ignore all other warnings for this file.", fileName.GetData(), nI + 1), "Warning.", wxOK | wxCENTRE | wxICON_WARNING | wxCANCEL))
        {
          bShowWarnings = FALSE;
        }

        nI++;
        continue;
    }
    //just add to dictionary
    m_dictionary.push_back(std::make_pair(sFirstName, sSecondName));
    // to next
    nI++;
  }
  pFile->Close();
  cppDEL(pFile);
  return true;
}

//----------------------------------------------------------------------------
void  mmoBuildHierarchy::Destroy(mafFrame **root)
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
void mmoBuildHierarchy::BindToVME(mafVME *pvme, mmoBuildHierarchy::mafFrame *pStart)
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
  wxString const * pVMENameStr = LookupUserName(pStart->GetName());
  if(pVMENameStr != NULL)
  {
    pFoundVME = (mafVME*)pVMERoot->FindInTreeByName(pVMENameStr->GetData());
  }
  //and again ^_^
  if(pFoundVME == NULL)
  {
    pVMENameStr = LookupStdName(pStart->GetName());
    if(pVMENameStr != NULL)
      pFoundVME = (mafVME*)pVMERoot->FindInTreeByName(pVMENameStr->GetData());
  }
  //try again in case of failure
  if(pFoundVME == NULL)
  {
    pFoundVME = (mafVME*)pVMERoot->FindInTreeByName(pStart->GetName()->GetData());
  }

  pStart->SetVME(pFoundVME);
  if(pFoundVME != NULL)
    pStart->SetParentVME(pFoundVME->GetParent());
  if(LookupStdName(pStart->GetName()) != NULL)
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
void  mmoBuildHierarchy::ExtractTwoWordsFromString(wxString &pFirstLine, wxString &sOne, wxString &sTwo)
//----------------------------------------------------------------------------
{
  wxInt32    nJ, nK; 
  //skip to first word
  for(nJ = 0; nJ < pFirstLine.Length(); nJ++)
  {
    if(pFirstLine[nJ] == '\"')
    {
      break;
    }
  }
  //skip first word
  for(nK = nJ + 1; nK < pFirstLine.Length(); nK++)
  {
    if(pFirstLine[nK] == '\"')
    {
      break;
    }
  }
  sOne = pFirstLine.Mid(nJ + 1, nK - nJ - 1);

  //skip to second word
  for(nJ = nK + 1; nJ < pFirstLine.Length(); nJ++)
  {
    if(pFirstLine[nJ] == '\"')
    {
      break;
    }
  }

  //skip second word
  for(nK = nJ + 1; nK < pFirstLine.Length(); nK++)
  {
    if(pFirstLine[nK] == '\"')
    {
      break;
    }
  }
  sTwo =  pFirstLine.Mid(nJ + 1, nK - nJ - 1);
}

//----------------------------------------------------------------------------
mmoBuildHierarchy::mafFrame::mafFrame()
//----------------------------------------------------------------------------
{
  //m_landmarks = new LandmarkArray();

  m_next   = NULL;
  m_vme    = NULL;
  m_name   = NULL; 
  m_parent = NULL;
  m_child  = NULL;
}

//----------------------------------------------------------------------------
mmoBuildHierarchy::mafFrame::~mafFrame()
//----------------------------------------------------------------------------
{
  cppDEL(m_next);
  cppDEL(m_name); 
  cppDEL(m_child);
  m_parent = NULL;
  m_vme    = NULL;  
}

//----------------------------------------------------------------------------
wxString const * mmoBuildHierarchy::LookupStdName(wxString const *name)
//----------------------------------------------------------------------------
{
  wxInt32      nI; 

  for(nI = 0; nI < m_dictionary.size(); nI++)
  {
    if(m_dictionary[nI].second == (*name))
    {
      return &m_dictionary[nI].first;
    }
    //already a ref one
    if(m_dictionary[nI].first == (*name))
    {
      return &m_dictionary[nI].first;
    }
  }
  //failed lookup
  return NULL;
}

//----------------------------------------------------------------------------
wxString const * mmoBuildHierarchy::LookupUserName(wxString const *name)
//----------------------------------------------------------------------------
{
  wxInt32      nI; 

  for(nI = 0; nI < m_dictionary.size(); nI++)
  {
    if(m_dictionary[nI].first == (*name))
    {
      return &m_dictionary[nI].second;
    }
    //already a ref one
    if(m_dictionary[nI].second == (*name))
    {
      return &m_dictionary[nI].second;
    }
  }
  //failed lookup
  return NULL;
}

//----------------------------------------------------------------------------
mmoBuildHierarchy::mafFrame *mmoBuildHierarchy::FindFrameUsingDictionary(wxString const &str, bool bCreateIfNotFound)    
//----------------------------------------------------------------------------
{
  mafFrame *pRet = NULL;
  wxString const *pStr;

  pRet = FindFrame(str, bCreateIfNotFound);

  if(pRet == NULL)
  {
    pStr = LookupUserName(&str);
    if(pStr != NULL)
      pRet = FindFrame(*pStr, bCreateIfNotFound);
    if(pRet == NULL)
    {
      pStr = LookupStdName(&str);
      if(pStr != NULL)
        pRet = FindFrame(*pStr, bCreateIfNotFound);
    }
  }

  return (pRet);
}
//----------------------------------------------------------------------------
mmoBuildHierarchy::mafFrame *mmoBuildHierarchy::FindFrame(wxString const &str, bool bCreateIfNotFound)
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
    pRet->SetName(&str);
  }

  return (pRet);
}
//----------------------------------------------------------------------------
mmoBuildHierarchy::mafFrame *mmoBuildHierarchy::FindFrame(mmoBuildHierarchy::mafFrame *pRoot, wxString const &str)    
//----------------------------------------------------------------------------
{
  mafFrame *pRet;
  mafFrame *pNext;

  if(pRoot == NULL)
  {
    return (NULL);
  }
  //check this
  if(*pRoot->GetName() == str)
  {
    return (pRoot);
  }

  //look into childrens
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


