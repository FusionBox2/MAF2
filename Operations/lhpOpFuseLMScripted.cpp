/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpFuseLMScripted.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.2 $
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

#include "lhpOpFuseLMScripted.h"

#include "mafDecl.h"
#include <wx/busyinfo.h>
#include "wx/wxprec.h"
#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include <wx/wxprec.h>

#include <list>
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafGUIDialog.h"

#include "mafNodeIterator.h"
#include "mafVME.h"
#include "mafSmartPointer.h"
#include "mafVMELandmark.h"

#include "vtkMAFSmartPointer.h"
#include "mafMatrixVector.h"
#include "mafAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpFuseLMScripted);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum ID_REGISTER_CLUSTERS
{
  RIGID =0,
  SIMILARITY,
  AFFINE
};
//----------------------------------------------------------------------------
lhpOpFuseLMScripted::lhpOpFuseLMScripted(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType           = OPTYPE_OP;
  m_Canundo          = true;
  
  m_Source           = NULL;
  m_Target           = NULL;
  m_Registered       = NULL;
  m_SourceName       ="none";
  m_TargetName       ="none";
  m_MultiTime        = 0;
  m_ListFName        = "";
}
//----------------------------------------------------------------------------
lhpOpFuseLMScripted::~lhpOpFuseLMScripted( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Registered);
}
//----------------------------------------------------------------------------
mafOp* lhpOpFuseLMScripted::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpFuseLMScripted(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpFuseLMScripted::Accept(mafNode* node)
//----------------------------------------------------------------------------
{
  if(!node) return false;
  return true;
};
//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_CHOOSE = MINID,
  ID_MULTIPLE_TIME_REGISTRATION,
  ID_LOAD_SCRIPT,
};
//----------------------------------------------------------------------------
void lhpOpFuseLMScripted::OpRun()   
//----------------------------------------------------------------------------
{
  m_Source = (mafVMELandmarkCloud*)m_Input;
  m_SourceName = m_Input->GetName();
  
  mafString wildcard = "Dictionary (*.txt)|*.txt|All Files (*.*)|*.*";

  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  
  m_Gui->Label(_("source :"),true);
  m_Gui->Label(&m_SourceName);
  
  m_Gui->Label(_("target :"),true);
  m_Gui->Label(&m_TargetName);
  m_Gui->Button(ID_CHOOSE,_("target "));
  
  
  m_Gui->Bool(ID_MULTIPLE_TIME_REGISTRATION,_("multi-time"),&m_MultiTime,1);
  m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,false);
  
  m_Gui->FileOpen(ID_LOAD_SCRIPT, "Script",  &m_ListFName, "*.txt");
  m_Gui->Label("");

  m_Gui->OkCancel();

  m_Gui->Enable(wxOK,false);
  m_Gui->Divider();
  ShowGui();

}

//----------------------------------------------------------------------------
void  lhpOpFuseLMScripted::ParseString(wxString &pFirstLine, wxString &sOne, wxString &sTwo)
//----------------------------------------------------------------------------
{
  wxInt32    nJ, nK; 
  //skip to first word
  for(nJ = 0; nJ < pFirstLine.Length(); nJ++)
  {
    if(pFirstLine[nJ] != ' ' && pFirstLine[nJ] != '\t')//if(pFirstLine[nJ] == '\"')
    {
      break;
    }
  }
  //skip first word
  for(nK = nJ; nK < pFirstLine.Length(); nK++)
  {
    if(pFirstLine[nK] == ' ' || pFirstLine[nK] == '\t')//if(pFirstLine[nK] == '\"')
    {
      break;
    }
  }
  sOne = pFirstLine.Mid(nJ, nK - nJ);

  //skip to second word
  for(nJ = nK; nJ < pFirstLine.Length(); nJ++)
  {
    if(pFirstLine[nJ] != ' ' && pFirstLine[nJ] != '\t')//if(pFirstLine[nJ] == '\"')
    {
      break;
    }
  }

  //skip second word
  for(nK = nJ; nK < pFirstLine.Length(); nK++)
  {
    if(pFirstLine[nK] == ' ' || pFirstLine[nK] == '\t')//if(pFirstLine[nK] == '\"')
    {
      break;
    }
  }
  sTwo =  pFirstLine.Mid(nJ, nK - nJ);
}


//----------------------------------------------------------------------------
bool lhpOpFuseLMScripted::ReadLMDictionary(mafString *fileName)
//----------------------------------------------------------------------------
{
  wxTextFile   *pFile;
  wxInt32      nI; 
  wxString     sFirstName("");
  wxString     sSecondName("");
  //mafGraphDictionary *pEntry;

  pFile = new wxTextFile(fileName->GetCStr());

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
  //m_LMDict.clear();

  for(nI = 0; nI < pFile->GetLineCount(); )
  {
    wxString &pFirstLine = pFile->GetLine(nI);
    //match it as beginning of block
    if(pFirstLine == "" || pFirstLine[0] == '#')
    {
      nI++;
      continue;
    }    

    ParseString(pFirstLine, sFirstName, sSecondName);

    if(sFirstName == "" || sSecondName == "")
    {
      //consider string in invalid
      wxMessageBox(wxString::Format("Syntax error in file %s, line %d. Ignoring.", fileName->GetCStr(), nI + 1), "Warning.", wxOK | wxCENTRE | wxICON_WARNING);

      nI++;
      continue;
    }
    //pEntry = new mafGraphDictionary(sFirstName, sSecondName);
    //just add to dictionary
    m_LMDict.push_back(std::make_pair(sFirstName, sSecondName));// Add(pEntry);
    // to next
    nI++;
  }
  pFile->Close();
  cppDEL(pFile); 
  return true;
}

//----------------------------------------------------------------------------
void lhpOpFuseLMScripted::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_CHOOSE:
      {
        mafString s(_("Choose cloud"));
        mafEvent e(this,VME_CHOOSE, &s, NULL/*, (long)&lhpOpFuseLMScripted::ClosedCloudAccept*/);
        mafEventMacro(e);
        mafNode *vme = e.GetVme();
        OnChooseTargetVme(vme);
      }
      break;
      case ID_LOAD_SCRIPT:
        if(m_ListFName != "")
        {
          m_LMDict.clear();
          ReadLMDictionary(&m_ListFName);
        }
        break;
      case wxOK:
        if(RegistrationProcedure())
          OpStop(OP_RUN_OK);
        else
          OpStop(OP_RUN_CANCEL);
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

namespace
{
  void FillTraverseList(mafNode *node, std::list<mafNode*>& traverse)
  {
    std::list<mafNode*> tmp;
    tmp.push_back(node);
    while(!tmp.empty())
    {
      mafNode *x = *(tmp.begin());
      tmp.pop_front();
      if(!x->IsVisible())
        continue;
      traverse.push_back(x);
      for(int i = 0; i < x->GetNumberOfChildren(); i++)
        tmp.push_back(x->GetChild(i));
    }
  }
}

bool lhpOpFuseLMScripted::RegistrationProcedure()
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, working...");
  }

  if(m_Registered == NULL)
  {
    wxString name = wxString::Format("%s registered on %s",m_Source->GetName().GetCStr(), m_Target->GetName().GetCStr());
    m_Registered= mafVME::SafeDownCast(m_Source->CopyTree());
    m_Registered->Register(this);
    m_Registered->SetName(name);
  }

  bool processed = false;

  std::vector<bool> usedEntries;
  usedEntries.resize(m_LMDict.size());
  for(int i = 0; i < m_LMDict.size(); i++)
    usedEntries[0] = false;
  std::list<mafNode*> srcTrav;
  std::list<mafNode*> regTrav;
  FillTraverseList(m_Source, srcTrav);
  FillTraverseList(m_Registered, regTrav);

  std::list<mafNode*>::iterator itsrc, itreg;
  for(itsrc = srcTrav.begin(), itreg = regTrav.begin(); itsrc != srcTrav.end() && itreg != regTrav.end(); ++itsrc, ++itreg)
  {
    mafNode *nsrc = *itsrc;
    mafNode *nreg = *itreg;
    //while(mafVMEInfoText *vit = mafVMEInfoText::SafeDownCast(nreg))
    //  nreg = iterreg->GetNextNode();
    mafVMELandmarkCloud *lmcs = mafVMELandmarkCloud::SafeDownCast(nsrc);
    mafVMELandmarkCloud *lmcr = mafVMELandmarkCloud::SafeDownCast(nreg);
    mafVMELandmarkCloud *lmct = NULL;
    if(lmcs == NULL)//lmcr is of the same type as lmcs
      continue;
    const char *search_name = nsrc->GetName();
    for(int i = 0; i < m_LMDict.size(); i++)
    {
      search_name = NULL;
      if(usedEntries[i])
        continue;
      if(mafString(nsrc->GetName()) == mafString(m_LMDict[i].first))
      {
        usedEntries[i] = true;
        search_name = m_LMDict[i].second.c_str();
        break;
      }
    }
    if(search_name)
    {
      mafNodeIterator *lmitert = m_Target->NewIterator();
      for(mafNode *lmt = lmitert->GetFirstNode(); lmt; lmt = lmitert->GetNextNode())
      {
        mafVMELandmarkCloud *lmtmp = mafVMELandmarkCloud::SafeDownCast(lmt);
        if(lmtmp == NULL)
          continue;
        if(strstr(lmtmp->GetName(), search_name) != NULL)
        {
          lmct = lmtmp;
          break;
        }
      }
      mafDEL(lmitert);
    }
    if(lmct == NULL)
      continue;
    bool res = ProcessNode(lmcs, lmct, lmcr);
    processed = processed || res;
  }

  if(!m_TestMode)
  {
    delete wait;
  }
  return processed;
}

bool lhpOpFuseLMScripted::ProcessNode(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *registered)
{
  bool lmcsOpened = src->IsOpen();
  bool lmctOpened = trg->IsOpen();
  bool lmcrOpened = registered->IsOpen();
  if(!lmcsOpened)
    src->Open();
  if(!lmctOpened)
    trg->Open();
  if(!lmcrOpened)
    registered->Open();
  std::vector<unsigned> mapping;
  mapping.resize(src->GetNumberOfLandmarks());
  for(std::vector<unsigned>::iterator it = mapping.begin(); it != mapping.end(); ++it)
    *it = trg->GetNumberOfLandmarks();

  for(unsigned i = 0; i < src->GetNumberOfLandmarks(); i++)
  {
    for(unsigned j = 0; j < trg->GetNumberOfLandmarks(); j++)
    {
      if(src->GetLandmarkName(i) == trg->GetLandmarkName(j))
        mapping[i] = j;
    }
  }

  if(m_MultiTime)
  {
    for(unsigned i = 0; i < src->GetNumberOfLandmarks(); i++)
    {
      if(mapping[i] == trg->GetNumberOfLandmarks())
        continue;
      mafVMELandmark *lmtrg = trg->GetLandmark(mapping[i]);
      mafVMELandmark *lmreg = registered->GetLandmark(i);
      std::vector<mafTimeStamp> timeStamps;
      lmtrg->GetAbsTimeStamps(timeStamps);
      int numTimeStamps = timeStamps.size();
      for (int t = 0; t < numTimeStamps; t++)
      {
        double currTime = timeStamps[t];
        double pos[3], rot[3];
        lmtrg->GetOutput()->GetAbsPose(pos, rot, currTime);
        lmreg->SetAbsPose(pos, rot, currTime);
      }
      timeStamps.clear();
    }
  }
  else
  {
    for(unsigned i = 0; i < src->GetNumberOfLandmarks(); i++)
    {
      if(mapping[i] == trg->GetNumberOfLandmarks())
        continue;
      mafVMELandmark *lmtrg = trg->GetLandmark(mapping[i]);
      mafVMELandmark *lmreg = registered->GetLandmark(i);
      double pos[3], rot[3];
      lmtrg->GetOutput()->GetAbsPose(pos, rot);
      lmreg->SetAbsPose(pos, rot);
    }
  }
  if(!lmcsOpened)
    src->Close();
  if(!lmctOpened)
    trg->Close();
  if(!lmcrOpened)
    registered->Close();
  return true;
}
//----------------------------------------------------------------------------
void lhpOpFuseLMScripted::OpDo()
//----------------------------------------------------------------------------
{
  m_Registered->ReparentTo(m_Input->GetRoot());
}
//----------------------------------------------------------------------------
void lhpOpFuseLMScripted::OpUndo()
//----------------------------------------------------------------------------
{
  m_Registered->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void lhpOpFuseLMScripted::OnChooseTargetVme(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(!vme) // user choose cancel - keep everything as before
    return;
  std::vector<mafTimeStamp> kframes;
  m_Target = (mafVME*)vme;
  m_TargetName = vme->GetName();
  m_Target->GetTimeStamps(kframes);
  if(kframes.size() > 1)
    m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,true);
  m_Gui->Enable(wxOK,true);
  m_Gui->Update();
}
