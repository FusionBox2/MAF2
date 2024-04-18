/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterC3DFused.cpp,v $
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

#include "lhpOpImporterC3DFused.h"
#include "lhpOpImporterC3DBTK.h"
#include "lhpOpExporterCSVGraph.h"

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

#include "vtkSmartPointer.h"
#include "mafMatrixVector.h"
#include "mafAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpImporterC3DFused);
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
lhpOpImporterC3DFused::lhpOpImporterC3DFused(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType           = OPTYPE_IMPORTER;
  m_Canundo          = true;
  
  m_Source           = NULL;
  m_SourceName       =_R("none");
  m_PointsSource     = NULL;
  m_PointsTarget     = NULL;
  m_ListFName        = _R("");
  m_DictionaryFileName = _R("");
  m_LMRenameFileName   = _R("");
  m_PscScriptFileName = _R("");
  m_CSVExported = false;
  m_RegistrationMode = RIGID;
}
//----------------------------------------------------------------------------
lhpOpImporterC3DFused::~lhpOpImporterC3DFused( ) 
//----------------------------------------------------------------------------
{
  vtkDEL(m_PointsSource);
  vtkDEL(m_PointsTarget);
  for (auto& p : m_Imported)
  {
    mafDEL(p);
  }
  for (auto& p : m_Registered)
  {
    mafDEL(p);
  }
}
//----------------------------------------------------------------------------
mafOp* lhpOpImporterC3DFused::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpImporterC3DFused(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpImporterC3DFused::Accept(mafNode* node)
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
  ID_MULTIPLE_TIME_REGISTRATION = MINID,
  ID_REGTYPE,
  ID_LOAD_SCRIPT,
  ID_LOAD_DICT,
  ID_CLEAR_DICT,
  ID_LOAD_LMREN,
  ID_CLEAR_LMREN,
  ID_LOAD_PSC_SCRIPT,
  ID_CLEAR_PSC,
};
//----------------------------------------------------------------------------
void lhpOpImporterC3DFused::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildcard = _R("c3d files (*.c3d)|*.c3d");

  m_C3DInputFileNameFullPaths.clear();
  {
    mafGetOpenMultiFiles(mafGetApplicationDirectory() + _R("/Data/External/"), wildcard, m_C3DInputFileNameFullPaths);
  }

  if (m_C3DInputFileNameFullPaths.size() == 0)
  {
    mafEventMacro(mafEvent(this, OP_RUN_CANCEL));
  }
  else if (!m_TestMode)
  {
    m_Source = (mafVME*)m_Input;
    m_SourceName = m_Input->GetName();

    m_Gui = new mafGUI(this);
    m_Gui->SetListener(this);

    m_Gui->FileOpen(ID_LOAD_LMREN, _R("Renamer"), &m_LMRenameFileName, _R("*.txt"));
    m_Gui->Button(ID_CLEAR_LMREN, _R("Clean"), _R(""), _R("Press to cancel using LM renamer"));
    m_Gui->FileOpen(ID_LOAD_DICT, _R("Segment"), &m_DictionaryFileName, _R("*.txt"));
    m_Gui->Button(ID_CLEAR_DICT, _R("Clean"), _R(""), _R("Press to cancel using dictionary"));
    m_Gui->Divider();
    m_Gui->FileOpen(ID_LOAD_PSC_SCRIPT, _R("CSVGraph"), &m_PscScriptFileName, _R("*.psc"));
    m_Gui->Button(ID_CLEAR_PSC, _R("Clean"), _R(""), _R("Press to cancel graph export"));
    m_Gui->Divider();

    m_Gui->Label(_L("source :"), true);
    m_Gui->Label(&m_SourceName);

    int num_choices = 3;
    const mafString choices_string[] = { _L("rigid"), _L("similarity"), _L("affine") };
    m_Gui->Combo(ID_REGTYPE, _L("reg. type"), &m_RegistrationMode, num_choices, choices_string);

//     m_Gui->Bool(ID_MULTIPLE_TIME_REGISTRATION, _("multi-time"), &m_MultiTime, 1);
//     m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION, false);
    m_Gui->Divider();
// 
    m_Gui->FileOpen(ID_LOAD_SCRIPT, _R("Script"), &m_ListFName, _R("*.txt"));
    m_Gui->Label(_R(""));

    m_Gui->OkCancel();

    m_Gui->Divider();
    ShowGui();
  }
  else
  {
    /*if (Import())
    {
      mafEventMacro(mafEvent(this, OP_RUN_OK));
    }
    else
    {
      mafEventMacro(mafEvent(this, OP_RUN_CANCEL));
    }*/
  }
  assert(!m_PointsSource && !m_PointsTarget);
  m_PointsSource = vtkPoints::New();
  m_PointsTarget = vtkPoints::New();

}

//----------------------------------------------------------------------------
void  lhpOpImporterC3DFused::ParseString(wxString &pFirstLine, wxString &sOne, wxString &sTwo)
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
bool lhpOpImporterC3DFused::ReadLMDictionary(mafString *fileName)
//----------------------------------------------------------------------------
{
  wxTextFile   *pFile;
  wxInt32      nI; 
  wxString     sFirstName("");
  wxString     sSecondName("");
  //mafGraphDictionary *pEntry;

  pFile = new wxTextFile(fileName->toWx());

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
      mafWarningMessage(_M(_R("Syntax error in file ") + *fileName + mafString::Format(_R(", line %d. Ignoring."), nI + 1)));

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
void lhpOpImporterC3DFused::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_CLEAR_DICT:
      {
        m_DictionaryFileName = _R("");
        m_Gui->Update();
        break;
      }
      case ID_LOAD_DICT:
      {
        break;
      }
      case ID_REGTYPE:
      {
        break;
      }
      case ID_CLEAR_LMREN:
      {
        m_LMRenameFileName = _R("");
        m_Gui->Update();
        break;
      }
      case ID_LOAD_LMREN:
      {
        break;
      }
      case ID_LOAD_PSC_SCRIPT:
      {
        break;
      }
      case ID_CLEAR_PSC:
      {
        m_PscScriptFileName = _R("");
        m_Gui->Update();
        break;
      }
      case ID_LOAD_SCRIPT:
        if(!m_ListFName.IsEmpty())
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

  mafNode *CopyTreeTimeStamp(mafNode *src)
  {
    if(src == NULL)
      return NULL;

    mafNode *result = src->CopyTree();

    std::list<mafNode*> tmp;
    tmp.push_back(result);
    while(!tmp.empty())
    {
      mafNode *x = *(tmp.begin());
      tmp.pop_front();
      /*if(mafVMEGenericAbstract *gvme = mafVMEGenericAbstract::SafeDownCast(x))
      {
        mafTimeStamp ts = gvme->GetTimeStamp();
        if(mafMatrixVector *mv = gvme->GetMatrixVector())
        {
          mafMatrix *matrix = gvme->GetOutput().GetMatrix()->GetNearestMatrix(ts);
          if(matrix)
          {
            mafMatrix mat = *matrix;
            mat.SetTimeStamp(ts);
            mv->SetMatrix(mat);
          }
        }
      }
      if(mafDataVector *dv = oldVme->GetDataVector())
      {
        vmeItem = dv->GetNearestItem(timeSt);
        if (vmeItem)
        {
          if(mafVMEItem *vmeItemCopy = vmeItem->NewInstance())
          {
          vmeItemCopy->DeepCopy(vmeItem);
          vmeGeneric->GetDataVector()->AppendItem(vmeItemCopy);
          oldTime = vmeItem->GetTimeStamp();
          }
        }
      }*/

      for(int i = 0; i < x->GetNumberOfChildren(); i++)
        tmp.push_back(x->GetChild(i));
    }
    return result;
  }
}

bool lhpOpImporterC3DFused::RegistrationProcedure()
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, working...");
  }

  for (auto fn : m_C3DInputFileNameFullPaths)
  {
    lhpOpImporterC3DBTK *importer = new lhpOpImporterC3DBTK(_R("importer"));
    //importer->TestModeOn();
    importer->SetC3DFileName(fn.GetCStr());
    importer->SetDictionaryFileName(m_DictionaryFileName.GetCStr());
    importer->SetLMRenameFileName(m_LMRenameFileName.GetCStr());
    importer->SetImportTrajectories(true);
    importer->SetImportAnalog(false);
    importer->SetImportPlatform(false);
    importer->SetImportEvent(false);
    importer->Import();
    mafVMEGroup *c3dImported = importer->GetGroup();
    c3dImported->Register(this);
    m_Imported.push_back(c3dImported);
    //vme = c3dImported;
    delete importer;
  }

  bool processed = false;
  for (auto Target : m_Imported)
  {
    bool multiTime = CheckMultiTime(Target);
    mafVME *Registered = NULL;
    if (Registered == NULL)
    {
      mafString name = m_Source->GetName() + _R(" registered on ") + Target->GetName();
      Registered = mafVME::SafeDownCast(m_Source->CopyTree());
      Registered->Register(this);
      Registered->SetName(name);
    }


    std::vector<bool> usedEntries;
    usedEntries.resize(m_LMDict.size());
    for (int i = 0; i < m_LMDict.size(); i++)
      usedEntries[0] = false;
    std::list<mafNode*> srcTrav;
    std::list<mafNode*> regTrav;
    FillTraverseList(m_Source, srcTrav);
    FillTraverseList(Registered, regTrav);

    std::list<mafNode*>::iterator itsrc, itreg;
    for (itsrc = srcTrav.begin(), itreg = regTrav.begin(); itsrc != srcTrav.end() && itreg != regTrav.end(); ++itsrc, ++itreg)
    {
      mafNode *nsrc = *itsrc;
      mafNode *nreg = *itreg;
      //while(mafVMEInfoText *vit = mafVMEInfoText::SafeDownCast(nreg))
      //  nreg = iterreg->GetNextNode();
      mafVMELandmarkCloud *lmcs = mafVMELandmarkCloud::SafeDownCast(nsrc);
      mafVMELandmarkCloud *lmcr = mafVMELandmarkCloud::SafeDownCast(nreg);
      mafVMELandmarkCloud *lmct = NULL;
      if (lmcs == NULL)//lmcr is of the same type as lmcs
        continue;
      const char *search_name = nsrc->GetName().GetCStr();
      for (int i = 0; i < m_LMDict.size(); i++)
      {
        search_name = NULL;
        if (usedEntries[i])
          continue;
        if (nsrc->GetName() == mafWxToString(m_LMDict[i].first))
        {
          usedEntries[i] = true;
          search_name = m_LMDict[i].second.c_str();
          break;
        }
      }
      if (search_name)
      {
        mafNodeIterator *lmitert = Target->NewIterator();
        for (mafNode *lmt = lmitert->GetFirstNode(); lmt; lmt = lmitert->GetNextNode())
        {
          mafVMELandmarkCloud *lmtmp = mafVMELandmarkCloud::SafeDownCast(lmt);
          if (lmtmp == NULL)
            continue;
          if (strstr(lmtmp->GetName().GetCStr(), search_name) != NULL)
          {
            lmct = lmtmp;
            break;
          }
        }
        mafDEL(lmitert);
      }
      if (lmct == NULL)
        continue;
      bool res = ProcessNode(lmcs, lmct, lmcr, multiTime);
      processed = processed || res;
    }
    if (Registered != NULL)
      m_Registered.push_back(Registered);
  }
  if(!m_TestMode)
  {
    delete wait;
  }
  return processed;
}
#ifdef IUYUIYIUYIUY
bool lhpOpImporterC3DFused::ProcessNode(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *registered, bool multiTime)
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

  if(multiTime)
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
#endif
bool lhpOpImporterC3DFused::ProcessNode(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *registered, bool multiTime)
{
   mafVMEInfoText *info;
   mafNEW(info);
   mafString name = _R("Info for registration ") + src->GetName() + _R(" into ") + trg->GetName();
   info->SetName(name);
   info->SetPosLabel(_R("Registration residual: "), 0);
   info->SetPosShow(true, 0);
   bool infoAdded = false;

  bool lmcsOpened = src->IsOpen();
  bool lmctOpened = trg->IsOpen();
  bool lmcrOpened = registered->IsOpen();
  if(lmcsOpened)
    src->Close();
  if(lmctOpened)
    trg->Close();
  if(lmcrOpened)
    registered->Close();


  if(multiTime)
  {
    std::vector<mafTimeStamp> timeStamps;
    trg->GetLocalTimeStamps(timeStamps);
    int numTimeStamps = timeStamps.size();

    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

    for (int t = 0; t < numTimeStamps; t++)
    {
      double currTime = timeStamps[t];
      long p = t * 100 / numTimeStamps;
      //  mafProgressBarSetValueMacro(p);
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(intptr_t)p));
      //Set the new time for the vme used to register the one frame source 
      trg->SetTimeStamp(currTime); //set current time
      trg->Update(); //>UpdateAllData();
      if(ExtractMatchingPoints(src, trg, currTime))
      {
         if(!infoAdded)
           info->ReparentTo(registered);
         infoAdded = true;
         double tr = RegisterPoints(src, trg, registered, currTime);
         info->SetAbsPose(tr, 0.0, 0.0, 0.0, 0.0, 0.0, currTime);
      }
    }
    timeStamps.clear();

    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }
  else
  {
    //RegisterPoints(m_Source->GetCurrentTime());
    if(ExtractMatchingPoints(src, trg))
    {
       if(!infoAdded)
         info->ReparentTo(registered);
       infoAdded = true;
       double tr = RegisterPoints(src, trg, registered);
       info->SetAbsPose(tr, 0.0, 0.0, 0.0, 0.0, 0.0);
    }
  }
  mafDEL(info);
  if(lmcsOpened)
    src->Open();
  if(lmctOpened)
    trg->Open();
  if(lmcrOpened)
    registered->Open();
  return true;
}

//----------------------------------------------------------------------------
void lhpOpImporterC3DFused::OpDo()
//----------------------------------------------------------------------------
{
  for (auto p : m_Imported)
    p->ReparentTo(m_Input->GetRoot());
  for (auto p : m_Registered)
    p->ReparentTo(m_Input->GetRoot());
  if (!m_CSVExported && !m_PscScriptFileName.IsEmpty())
  {
    m_CSVExported = true;
    mafString path = m_C3DInputFileNameFullPaths[0];
    path.ExtractPathName();
    for (auto iIt = m_Imported.begin(), rIt = m_Registered.begin(), iItE = m_Imported.end(); iIt != iItE; ++iIt, ++ rIt)
    {
      lhpOpExporterCSVGraph *csvexporter = new lhpOpExporterCSVGraph();
      csvexporter->SetPSCFileName(m_PscScriptFileName.GetCStr());
      csvexporter->SetFileName(/*path + "/" + */(*iIt)->GetName() + _R(".csv"));
      csvexporter->SetInput(*rIt);
      csvexporter->ExportGraphs();
    }
  }
}
//----------------------------------------------------------------------------
void lhpOpImporterC3DFused::OpUndo()
//----------------------------------------------------------------------------
{
  for (auto p : m_Imported)
    p->ReparentTo(NULL);
  for (auto p : m_Registered)
    p->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
int lhpOpImporterC3DFused::ExtractMatchingPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, double time)
//----------------------------------------------------------------------------
{
  m_PointsSource->Reset();
  m_PointsTarget->Reset();

  src->Update();
  vtkDataSet *polySource =src->GetOutput()->GetVTKData();
  vtkDataSet *polyTarget =trg->GetOutput()->GetVTKData();

  src->GetOutput()->Update();
  trg->GetOutput()->Update();

  int npSource = polySource->GetNumberOfPoints();
  int npTarget = polyTarget->GetNumberOfPoints();

  if(npSource == 0 || npTarget == 0) return 0;

  int i, j;
  //number of common points between m_Source and m_Target
  int ncp = 0;

  bool found_one = false;

  for(i=0;i<npSource;i++)
  {
    if(!src->GetLandmarkVisibility(i))
      continue;
    mafString SourceLandmarkName = src->GetLandmarkName(i);

    bool found = false;
    for(j=0;j<npTarget;j++)
    {
      mafString TargetLandmarkName = trg->GetLandmarkName(j);
      if(SourceLandmarkName == TargetLandmarkName)
      {
        found = true;
        found_one = true;
        break;
      }
    }

    if (found)
    {
      if(trg->GetLandmarkVisibility(j,time))
      {
        
        m_PointsSource->InsertNextPoint(polySource->GetPoint(i));
        m_PointsTarget->InsertNextPoint(polyTarget->GetPoint(j));
        ncp++;
      }
    }
  }

  if(!found_one)
  {
    wxMessageBox("No matching landmarks found!","Alert", wxOK , NULL);
  }
  else if(found_one && (ncp == 0) && (!trg->IsAnimated()))
  {
    wxMessageBox("No visible matching landmarks found at this timestamp!","Alert", wxOK , NULL);
  }

  return ncp;
}
//----------------------------------------------------------------------------
double lhpOpImporterC3DFused::RegisterPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *reg, double currTime)
//----------------------------------------------------------------------------
{
  double deviation = 0.0;
  assert(m_PointsSource && m_PointsTarget);

  vtkWeightedLandmarkTransform *RegisterTransform = vtkWeightedLandmarkTransform::New();

  RegisterTransform->SetSourceLandmarks(m_PointsSource);
  RegisterTransform->SetTargetLandmarks(m_PointsTarget);
  
  if(currTime < 0)
    currTime = trg->GetTimeStamp();
  
  switch (m_RegistrationMode)
  {
    case RIGID:
      RegisterTransform->SetModeToRigidBody();
    break;
    case SIMILARITY:
      RegisterTransform->SetModeToSimilarity();
    break;
    case AFFINE:
      RegisterTransform->SetModeToAffine();
    break;
  }
  RegisterTransform->Update();

  vtkMatrix4x4 *t_matrix = vtkMatrix4x4::New();
  t_matrix->Identity();

  //calculate deviation
  for(int i = 0; i < m_PointsSource->GetNumberOfPoints(); i++)
  {
    double coord[4];
    double result[4];
    double target[3];
    double dx, dy, dz;
    m_PointsSource->GetPoint(i, coord);
    coord[3] = 1.0;

    m_PointsTarget->GetPoint(i, target);

    //transform point
    RegisterTransform->GetMatrix()->MultiplyPoint(coord, result);

    dx = target[0] - result[0];
    dy = target[1] - result[1];
    dz = target[2] - result[2];

    deviation += dx * dx + dy * dy + dz * dz;
  }
  if(m_PointsSource->GetNumberOfPoints() != 0)
    deviation /= m_PointsSource->GetNumberOfPoints();
  deviation = sqrt(deviation);

  
  //post-multiply the registration matrix by the abs matrix of the target to position the
  //registered  at the correct position in the space

  vtkMatrix4x4 *regt_matrix = vtkMatrix4x4::New();
  regt_matrix->Identity();
  RegisterTransform->GetMatrix(regt_matrix);
  vtkDEL(RegisterTransform);

  mafMatrix *mat;
  mafNEW(mat);
  mat->Identity();
  trg->GetOutput()->GetAbsMatrix(*mat,currTime);  //modified by Marco. 2-2-2004

  vtkMatrix4x4::Multiply4x4(mat->GetVTKMatrix(),regt_matrix,t_matrix);
  mafDEL(mat);
  vtkDEL(regt_matrix);

  int numLandmarks = trg->GetNumberOfVisibleLandmarks(currTime);

  if((numLandmarks < 2) || ((numLandmarks < 4) && (m_RegistrationMode == AFFINE)))
  {
    vtkDEL(t_matrix);
    return deviation;
  }

  mafVMEGenericAbstract *registering = reg;
  if(mafVMESurface *srf = mafVMESurface::SafeDownCast(reg->GetParent()))
  {
    mafMatrix t2, t2inv;
    reg->GetOutput()->GetMatrix(t2, currTime);
    mafMatrix::Invert(t2, t2inv);
    vtkMatrix4x4 *r_matrix = vtkMatrix4x4::New();
    r_matrix->Identity();
    vtkMatrix4x4::Multiply4x4(t_matrix, t2inv.GetVTKMatrix(), r_matrix);
    vtkMatrix4x4 *tmpm = r_matrix;
    r_matrix = t_matrix;
    t_matrix = tmpm;
    vtkDEL(r_matrix);
    registering = srf;
  }

  registering->SetTimeStamp(currTime);
  mafMatrix temp;
  temp.SetVTKMatrix(t_matrix);
  temp.SetTimeStamp(currTime);
  temp.Modified();
  registering->GetOutput()->Update();
  registering->SetAbsMatrix(temp);
  registering->Modified();
  registering->Update();
  vtkDEL(t_matrix);
  return deviation;
}
//----------------------------------------------------------------------------
bool lhpOpImporterC3DFused::CheckMultiTime(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(!vme) // user choose cancel - keep everything as before
    return false;
  std::vector<mafTimeStamp> kframes;
  mafVME *Target = (mafVME*)vme;
  Target->GetTimeStamps(kframes);
  return (kframes.size() > 1);
}
