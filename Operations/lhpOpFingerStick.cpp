/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpFingerStick.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-05 14:14:37 $
  Version:   $Revision: 1.0 $
  Authors:   Fedor Moiseev / Vladik Aranov
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


#include "wx/wxprec.h"
#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include <wx/wxprec.h>
#include <math.h>
#include <vector>
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafOp.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "lhpOpFingerStick.h"
#include "mafOpExplodeCollapse.H"
#include "lhpOpImporterC3DBTK.h"

#include "mafVMELandmarkCloud.h"

#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"
#include "mafVMEGroup.h"


#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include "mafVectors.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum 
{
  RIGID =0,
  SIMILARITY,
  AFFINE
};

#ifndef DIM
#define DIM(a)  (sizeof((a)) / sizeof(*(a)))
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

mafCxxTypeMacro(lhpOpFingerStick)

//----------------------------------------------------------------------------
// static persistent data:
//----------------------------------------------------------------------------
mafVMELandmarkCloud  *lhpOpFingerStick::m_StickDefinitionPersistent = NULL;
//----------------------------------------------------------------------------
// Forward Refs
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void  lhpOpFingerStick::ParseString(wxString &pFirstLine, wxString &sOne, wxString &sTwo)
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
bool lhpOpFingerStick::ReadLMDictionary(mafString *fileName)
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
lhpOpFingerStick::lhpOpFingerStick(const mafString& label) : Superclass(label), m_DictionaryFName(_R(""))
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_Output  = NULL;

  
  m_PlateCalibration = NULL;
  m_PlateCalibrationName = _R("");
  
  m_BallsCalibration = NULL;
  m_BallsCalibrationName = _R("");
  
  m_PalpatorCalibration = NULL;
  m_PalpatorCalibrationName = _R("");
  
  m_PlateCloud = NULL;
  m_PlateCloudName = _R("");
  
  m_PalpatorCloud = NULL;
  m_PalpatorCloudName = _R("");

  m_Method               = 0;

  m_Registered      = NULL;
  m_pointsSource    = NULL;
  m_pointsTarget    = NULL;
  
  m_registration_mode = RIGID;

  m_weight       = NULL;

  m_RegisterTransform = NULL;

}

//----------------------------------------------------------------------------
lhpOpFingerStick::~lhpOpFingerStick( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Output);
  vtkDEL(m_Registered);
  vtkDEL(m_pointsSource);
  vtkDEL(m_pointsTarget);

  if(m_weight)
  {
    delete[] m_weight;
    m_weight = NULL;
  }

}

//----------------------------------------------------------------------------
mafOp* lhpOpFingerStick::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpFingerStick(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpFingerStick::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_DEFAULT = MINID,
  ID_PLATE_CALIBRATION,
  ID_PLATE_CALIBRATION_NAME,
  ID_BALLS_CALIBRATION,
  ID_BALLS_CALIBRATION_NAME,
  ID_PALPATOR_CALIBRATION,
  ID_PALPATOR_CALIBRATION_NAME,
  ID_PLATE_CLOUD,
  ID_PLATE_CLOUD_NAME,
  ID_PALPATOR_CLOUD,
  ID_PALPATOR_CLOUD_NAME,
  ID_LOAD_DICTIONARY,
  ID_USE_HOLES,
  ID_LOAD_LIST,
  ID_CLEAN_LIST,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

//----------------------------------------------------------------------------
void lhpOpFingerStick::OpRun()   
//----------------------------------------------------------------------------
{
  wxString sLMName("");

  CreateGui();
  assert(!m_pointsSource && !m_pointsTarget);
  m_pointsSource = vtkPoints::New();
  m_pointsTarget = vtkPoints::New();

}
  
//----------------------------------------------------------------------------
void lhpOpFingerStick::CreateGui()
//----------------------------------------------------------------------------
{
  if(m_Gui == NULL)
  {
    m_Gui = new mafGUI(this);
    m_Gui->SetListener(this);
    m_Gui->Label(_R(""));
    m_Gui->Button(ID_PALPATOR_CALIBRATION, _R("Palpator calibration cloud"), _R(""), _R("Press to select target cloud with wand calibration.") );
    m_Gui->Label(_R("Trg c. VME:"),&m_PalpatorCalibrationName);
    m_Gui->Button(ID_PLATE_CALIBRATION, _R("Plate calibration cloud"), _R(""), _R("Press to select wand cloud with wand definition.") );
    m_Gui->Label(_R("Wnd d.VME"),&m_PlateCalibrationName);
    m_Gui->Label(_R(""));
    m_Gui->FileOpen(ID_LOAD_LIST, _R("List"),  &m_ListFName, _R("*.lst"));
    m_Gui->Label(_R(""));
    m_Gui->Button(ID_CLEAN_LIST, _R("Clean"), _R(""), _R("Press to cancel list using") );
    m_Gui->Label(_R(""));
    m_Gui->FileOpen(ID_LOAD_DICTIONARY, _R("Dictionary"),  &m_DictionaryFName, _R("*.txt"));
    m_Gui->Label(_R(""));
    //m_Gui->Bool(ID_USE_HOLES, "Use holes", &m_Method, 0, "Turn on to use holes for palpator calibration..");
    //m_Gui->Label("");
    m_Gui->OkCancel();
  }
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpFingerStick::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    mafEventMacro(mafEvent(this,result));
  }
  else if (result == OP_RUN_OK)
  {
    if(m_DictionaryFName.empty() || m_PlateCalibration == NULL || m_PalpatorCalibration == NULL)
    {
      //wxMessageBox("Wand is not defined. Possibly wand data are not imported.","Alert", wxOK , NULL);
      wxMessageBox("Not all data defined for operation.","Alert", wxOK , NULL);
      return;
    }
   
    HideGui();
    mafEventMacro(mafEvent(this,result));
  }
}
//----------------------------------------------------------------------------
void lhpOpFingerStick::SetNodeName(mafVME *pVME, mafString *pName) 
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
void lhpOpFingerStick::OnEvent(mafEventBase *e) 
//----------------------------------------------------------------------------
{ 
  int nI;
  switch(e->GetId())
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
      break;
    }
    case ID_LOAD_LIST:
    {
      if(!m_ListFName.empty())
      {
        m_LMDict.clear();
        ReadLMDictionary(&m_ListFName);
      }
      break;
    }
    case ID_CLEAN_LIST:
    {
      m_ListFName = _R("");
      m_LMDict.clear();
      m_Gui->Update();
      break;
    }

    case ID_PLATE_CALIBRATION:
    {
      mafString s(_R("Choose wand calibration cloud"));
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      if(!e.GetVme()->IsA("mafVMELandmarkCloud"))
      {
        wxMessageBox("Selected VME should be of mafVMELandmarkCloud type.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }
      m_PlateCalibration = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_PlateCalibration, &m_PlateCalibrationName);
      m_Gui->Update();
      break;
    }
    case ID_BALLS_CALIBRATION:
    {
      mafString s(_R("Choose wand definition cloud"));
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      if(!e.GetVme()->IsA("mafVMELandmarkCloud"))
      {
        wxMessageBox("Selected VME should be of mafVMELandmarkCloud type.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }
      m_BallsCalibration = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_BallsCalibration, &m_BallsCalibrationName);
      m_Gui->Update();
      break;
    }
    case ID_PALPATOR_CALIBRATION:
    {
      mafString s(_R("Choose  target calibration cloud"));
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      if(!e.GetVme()->IsA("mafVMELandmarkCloud"))
      {
        wxMessageBox("Selected VME should be of mafVMELandmarkCloud type.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }
      m_PalpatorCalibration = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_PalpatorCalibration, &m_PalpatorCalibrationName);

      mafVME *parent = m_PalpatorCalibration->GetParent();
      if(parent != NULL)
      {
        for(nI = 0; nI < parent->GetNumberOfChildren(); nI++)
        {
          mafVME *child = (mafVME *)parent->GetChild(nI);
          if(child->IsA("mafVMELandmarkCloud") && strcmp(child->GetName().GetCStr(), "Plate") == 0)
          {
            m_PlateCalibration = (mafVMELandmarkCloud *)child;
            SetNodeName(m_PlateCalibration, &m_PlateCalibrationName);
            break;
          }
        }
      }
      m_Gui->Update();
      break;
    }
    case ID_PLATE_CLOUD:
    {
      mafString s(_R("Choose target cloud"));
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      if(!e.GetVme()->IsA("mafVMELandmarkCloud"))
      {
        wxMessageBox("Selected VME should be of mafVMELandmarkCloud type.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }
      m_PlateCloud = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_PlateCloud, &m_PlateCloudName);
      m_Gui->Update();
      break;
    }
    case ID_PALPATOR_CLOUD:
    {
      mafString s(_R("Choose target cloud"));
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      if(!e.GetVme()->IsA("mafVMELandmarkCloud"))
      {
        wxMessageBox("Selected VME should be of mafVMELandmarkCloud type.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }
      m_PalpatorCloud = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_PalpatorCloud, &m_PalpatorCloudName);
      m_Gui->Update();
      break;
    }
    default:
    {
      mafEventMacro(*e); 
    }
    break;
  }
}


//----------------------------------------------------------------------------
void lhpOpFingerStick::OpDo()
//----------------------------------------------------------------------------
{
  wxInt32        nI, nJ;
  double         res[3];
  double         tipPoint[3];
  const char     *PointsName = (m_Method) ? "HOLES" : "BALLS";
  mafVME         *vme;
  int            calculated = 0;
  const unsigned StrLim = 2000;
  char           strinst[StrLim];


  //modified by Stefano. 18-9-2003
  wxBusyInfo wait("Please wait, working...");


  mafDEL(m_Output);
  if(m_BallsCalibration == NULL)
  {
    mafVME *parent = m_PalpatorCalibration->GetParent();
    if(parent != NULL)
    {
      for(nI = 0; nI < parent->GetNumberOfChildren(); nI++)
      {
        mafVME *child = (mafVME *)parent->GetChild(nI);
        if(child->IsA("mafVMELandmarkCloud"))
        {
          strncpy(strinst, child->GetName().GetCStr(), StrLim);
          unsigned ln = strlen(child->GetName().GetCStr()) + 1;
          if(ln > StrLim)
            ln = StrLim;
          for(unsigned it = 0; it < ln; it++)
            strinst[it] = toupper(strinst[it]);

          if(strstr(strinst, PointsName) != NULL)
          {
            m_BallsCalibration = (mafVMELandmarkCloud *)child;
            SetNodeName(m_BallsCalibration, &m_BallsCalibrationName);
            break;
          }
        }
      }
    }
    if(m_BallsCalibration == NULL)
    {
      wxMessageBox("Predefined points not found","Alert", wxOK , NULL);
      return;
    }
  }

  mafString spath, sname, sext;
  mafSplitPath(m_DictionaryFName, &spath, &sname, &sext);


  tipPoint[0] = 0;
  tipPoint[1] = 0;
  tipPoint[2] = 0;

  mafVMEGroup *group;
  mafNEW(group); //We got a Reference on it
  group->SetName(_R("Finger palpator"));
  group->ReparentTo(m_Input);
  mafEventMacro(mafEvent(this,VME_ADD,group));

  for(nI = 0; nI < ((!m_ListFName.empty()) ? m_LMDict.size() : m_BallsCalibration->GetNumberOfLandmarks()); nI++)
  {
    int LMIndex = (!m_ListFName.empty()) ? m_BallsCalibration->FindLandmarkIndex(mafWxToString(m_LMDict[nI].first)) : nI;
    if(LMIndex == -1)
      continue;
    mafString file(spath);
    mafString name;
    file += _R("\\");
    name  = (!m_ListFName.empty()) ? mafWxToString(m_LMDict[nI].second) : m_BallsCalibration->GetLandmarkName(nI);
    file += name; 
    file += _R(".c3d");

    lhpOpImporterC3DBTK *importer=new lhpOpImporterC3DBTK(_R("importer"));
    //importer->TestModeOn();
    importer->SetC3DFileName(file.GetCStr());
    importer->SetDictionaryFileName(m_DictionaryFName.GetCStr());
    importer->SetImportTrajectories(true);
    importer->SetImportAnalog(false);
    importer->SetImportPlatform(false);
    importer->SetImportEvent(false);
    importer->Import();
    mafVMEGroup *c3dImported = importer->GetGroup();
    if(c3dImported != NULL)
      c3dImported->Register(this);
    vme = c3dImported;
    delete importer;

    mafEventMacro(mafEvent(this,VME_ADD,vme));

    if(vme != NULL)
    {
      for(nJ = 0; nJ < vme->GetNumberOfChildren(); nJ++)
      {
        mafVME *child = (mafVME *)vme->GetChild(nJ);
        if(child->IsA("mafVMELandmarkCloud"))
        {
          strncpy(strinst, child->GetName().GetCStr(), StrLim);
          unsigned ln = strlen(child->GetName().GetCStr()) + 1;
          if(ln > StrLim)
            ln = StrLim;
          for(unsigned it = 0; it < ln; it++)
            strinst[it] = tolower(strinst[it]);

          if(strstr(strinst, "palpator") != 0)
          {
            m_PalpatorCloud = (mafVMELandmarkCloud *)child;
            SetNodeName(m_PalpatorCloud, &m_PalpatorCloudName);
          }
          else if(strstr(strinst, "plate") != 0)
          {
            m_PlateCloud = (mafVMELandmarkCloud *)child;
            SetNodeName(m_PlateCloud, &m_PlateCloudName);
          }
        }
      }
      if(ProcessSingleLM(LMIndex, res))
      {
        for(nJ = 0; nJ < 3; nJ++)
         tipPoint[nJ] += res[nJ];
        calculated++;

        mafVMELandmarkCloud *locCalibr;
        mafNEW(locCalibr);
        mafString nmLC = _R("Finger palpator based on ") + name;
        locCalibr->SetName(nmLC);
        locCalibr->SetRadius(15);
        locCalibr->Close();

        for(wxInt32 nK = 0; nK < m_PalpatorCalibration->GetNumberOfLandmarks(); nK++)
        {
          double x, y, z;
          m_PalpatorCalibration->GetLandmark(nK, x, y, z, -1);
          locCalibr->AppendLandmark(x, y, z, m_PalpatorCalibration->GetLandmarkName(nK));
        }

        locCalibr->AppendLandmark(res[0], res[1], res[2], _R("TIP"));
        locCalibr->ReparentTo(group);
        mafEventMacro(mafEvent(this,VME_ADD,locCalibr));
        mafDEL(locCalibr);
      }
      mafEventMacro(mafEvent(this,VME_REMOVE,vme));
      mafDEL(vme);
    }
  }
  if(calculated == 0)
  {
    mafEventMacro(mafEvent(this,VME_REMOVE,group));
    mafDEL(group);
    return;
  }

  for(nJ = 0; nJ < 3; nJ++)
   tipPoint[nJ] /= calculated;


  mafVMELandmarkCloud *averagedCalibr;
  mafNEW(averagedCalibr);
  averagedCalibr->SetName(_R("Finger palpator wand definition"));
  averagedCalibr->SetRadius(15);
  averagedCalibr->Close();

  for(nI = 0; nI < m_PalpatorCalibration->GetNumberOfLandmarks(); nI++)
  {
    double x, y, z;
    m_PalpatorCalibration->GetLandmark(nI, x, y, z, -1);
    averagedCalibr->AppendLandmark(x, y, z, m_PalpatorCalibration->GetLandmarkName(nI));
  }

  averagedCalibr->AppendLandmark(tipPoint[0], tipPoint[1], tipPoint[2], _R("TIP"));
  averagedCalibr->ReparentTo(group);
  mafEventMacro(mafEvent(this,VME_ADD,averagedCalibr));
  mafDEL(averagedCalibr);
  m_Output = group;
}

//----------------------------------------------------------------------------
bool lhpOpFingerStick::ProcessSingleLM(int lmIndex, double result[3])
//----------------------------------------------------------------------------
{
  wxInt32      nI, nJ;
  vtkMatrix4x4 *mVTK = NULL;
  std::vector<mafTimeStamp> kframes;
  double       totalRes[3] = {0.0, 0.0, 0.0};

  //we need to 
  if(m_BallsCalibration == NULL || m_PlateCloud == NULL || m_PalpatorCloud == NULL)
  {
    wxLogMessage("Not all necessary data found!");//,"Alert", wxOK , NULL);
    return false;
  }

  bool bPlateCloudClosed = !m_PlateCloud->IsOpen();

  if(!bPlateCloudClosed)
  {
    mafOp *pCloseOp = new mafOpExplodeCollapse(_R("close cloud"));
    pCloseOp->SetInput(m_PlateCloud);
    pCloseOp->SetListener(GetListener());
    pCloseOp->OpDo();
    cppDEL(pCloseOp); 
  }

  bool bPlateCalibrationClosed = !m_PlateCalibration->IsOpen();

  if(!bPlateCalibrationClosed)
  {
    mafOp *pCloseOp = new mafOpExplodeCollapse(_R("close cloud"));
    pCloseOp->SetInput(m_PlateCalibration);
    pCloseOp->SetListener(GetListener());
    pCloseOp->OpDo();
    cppDEL(pCloseOp); 
  }

  m_PlateCloud->GetTimeStamps(kframes);

  vtkMatrix4x4 *t_matrixPlate    = vtkMatrix4x4::New();
  vtkMatrix4x4 *t_matrixPalpator = vtkMatrix4x4::New();


  std::vector<std::vector<double> >devs;
  std::vector<V3d<double> > pointers;
  devs.resize(m_PlateCloud->GetNumberOfTimeStamps());
  pointers.resize(m_PlateCloud->GetNumberOfTimeStamps());
  unsigned framesTaken = 0;
  for(nI = 0; nI < m_PlateCloud->GetNumberOfTimeStamps(); nI++)
  {
    double ballPt[4];
    double result[4];
    mafTimeStamp  currTime  = kframes[nI];

    m_RegisterTransform = vtkWeightedLandmarkTransform::New();

    devs[nI].clear();
    if(ExtractMatchingPoints(m_PlateCalibration, m_PlateCloud, -1, currTime) >= 3)
      RegisterPoints(t_matrixPlate, devs[nI]);
    else
      continue;

    vtkDEL(m_RegisterTransform);

    m_BallsCalibration->GetLandmark(lmIndex, ballPt);
    ballPt[3] = 1.0;

    t_matrixPlate->MultiplyPoint(ballPt, result);

    m_RegisterTransform = vtkWeightedLandmarkTransform::New();

    if(ExtractMatchingPoints(m_PalpatorCloud, m_PalpatorCalibration, currTime, -1) >= 3)
      RegisterPoints(t_matrixPalpator, devs[nI]);
    else
      continue;

    vtkDEL(m_RegisterTransform);

    t_matrixPalpator->MultiplyPoint(result, ballPt);

    /*double l_aver = 0.0;
    for(unsigned di = 0; di < devs.size(); di++)
      l_aver += devs[di];
    l_aver /= devs.size();
    double l_disp = 0.0;
    for(unsigned di = 0; di < devs.size(); di++)
      l_disp += (devs[di] - l_aver) * (devs[di] - l_aver);
    l_disp /= devs.size();

    l_disp = sqrt(l_disp);

    wxLogMessage("Frame %d deviation %lf", nI, l_aver);


    bool passFilter = true;
    for(unsigned di = 0; di < devs.size() && passFilter; di++)
    {
      if(fabs(devs[di] - l_aver) > 3 * l_disp)
        passFilter = false;
    }
    if(!passFilter)
    {
      wxLogMessage("Deviation filter is not passed! Frame skipped");
      continue;
    }*/



    for(nJ = 0; nJ < 3; nJ++)
      totalRes[nJ] += ballPt[nJ];
    framesTaken++;
  }
  vtkDEL(t_matrixPalpator);
  vtkDEL(t_matrixPlate);
  if(framesTaken == 0)
  {
    wxLogMessage("No matching frames found!");
    return false;
  }
  if(framesTaken != m_PlateCloud->GetNumberOfTimeStamps())
  {
    wxLogMessage("%d frames skipped", m_PlateCloud->GetNumberOfTimeStamps() - framesTaken);
  }
  for(nJ = 0; nJ < 3; nJ++)
  {
    totalRes[nJ] /= framesTaken;
    result [nJ]   = totalRes[nJ];
  }
  bool bPalpatorCalibrationClosed = !m_PalpatorCalibration->IsOpen();

  if(!bPalpatorCalibrationClosed)
  {
    mafOp *pCloseOp = new mafOpExplodeCollapse(_R("close cloud"));
    pCloseOp->SetInput(m_PalpatorCalibration);
    pCloseOp->SetListener(GetListener());
    pCloseOp->OpDo();
    cppDEL(pCloseOp); 
  }

  if(!bPalpatorCalibrationClosed)
  {
    mafOp *pOpenOp = new mafOpExplodeCollapse(_R("open cloud"));
    pOpenOp->SetInput(m_PalpatorCalibration);
    pOpenOp->SetListener(GetListener());
    pOpenOp->OpDo();
    cppDEL(pOpenOp); 
  }

  if(!bPlateCloudClosed)
  {
    mafOp *pOpenOp = new mafOpExplodeCollapse(_R("open cloud"));
    pOpenOp->SetInput(m_PlateCloud);
    pOpenOp->SetListener(GetListener());
    pOpenOp->OpDo();
    cppDEL(pOpenOp); 
  }

  if(!bPlateCalibrationClosed)
  {
    mafOp *pOpenOp = new mafOpExplodeCollapse(_R("open cloud"));
    pOpenOp->SetInput(m_PlateCalibration);
    pOpenOp->SetListener(GetListener());
    pOpenOp->OpDo();
    cppDEL(pOpenOp); 
  }

  return true;
}



//----------------------------------------------------------------------------
int lhpOpFingerStick::ExtractMatchingPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafTimeStamp srctime, mafTimeStamp trgtime)
//----------------------------------------------------------------------------
{
  m_pointsSource->Reset();
  m_pointsTarget->Reset();

  int npSource = src->GetNumberOfLandmarks();
  int npTarget = trg->GetNumberOfLandmarks();
  int i;
  int j;

  int ncp = 0;

  bool found_one = false;

  for(i = 0;i < npSource;i++)
  {
    mafString SourceLandmarkName = src->GetLandmarkName(i);

    //search for landmark with the same name
    bool found = false;
    for(j = 0;j < npTarget;j++)
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
      if(src->GetLandmarkVisibility(j,srctime) && trg->GetLandmarkVisibility(j,trgtime))
      {
        double lmsrcd[3];
        double lmtrgd[3];
        //add new points to arrays if no one is zero (zero indicates that data are invalid)
        src->GetLandmark(i, lmsrcd, srctime);
        trg->GetLandmark(j, lmtrgd, trgtime);

        if(lmsrcd[0] == 0.0 && lmsrcd[1] == 0.0 && lmsrcd[2] == 0.0)
          continue;
        if(lmtrgd[0] == 0.0 && lmtrgd[1] == 0.0 && lmtrgd[2] == 0.0)
          continue;

        m_pointsSource->InsertNextPoint(lmsrcd);
        m_pointsTarget->InsertNextPoint(lmtrgd);
        ncp++;
      }
    }
  }

  if(!found_one)
  {
    wxLogMessage("No matching landmarks found!");
  }
  else if(found_one && ncp == 0)
  {
    wxLogMessage("No visible matching landmarks found at this timestamp!");
  }

  m_numPoints = ncp;

  return ncp;
}
//----------------------------------------------------------------------------
void lhpOpFingerStick::RegisterPoints(vtkMatrix4x4 *res_matrix, std::vector<double>& devs)
//----------------------------------------------------------------------------
{
  int i;
  double deviation = 0.0;
  double dx, dy, dz;
  assert(m_pointsSource && m_pointsTarget);

  //setup transform calculator
  m_RegisterTransform->SetSourceLandmarks(m_pointsSource);  
  m_RegisterTransform->SetTargetLandmarks(m_pointsTarget);  
  
  if(m_weight)
  {
    {
      int number = m_numPoints;
      m_RegisterTransform->SetWeights(m_weight,number);
    }
  }

  m_RegisterTransform->SetModeToSimilarity();

  //calculate transform
  m_RegisterTransform->Update();
  m_RegisterTransform->GetMatrix(res_matrix);

  //calculate deviation
  for(i = 0; i < m_pointsSource->GetNumberOfPoints(); i++)
  {
    double coord[4];
    double result[4];
    double target[3];
    m_pointsSource->GetPoint(i, coord);
    coord[3] = 1.0;
    
    m_pointsTarget->GetPoint(i, target);

    //transform point
    res_matrix->MultiplyPoint(coord, result);

    dx = target[0] - result[0];
    dy = target[1] - result[1];
    dz = target[2] - result[2];

    devs.push_back(sqrt(dx * dx + dy * dy + dz * dz));
  }
}
