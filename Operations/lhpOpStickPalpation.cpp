/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpStickPalpation.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:19:42 $
  Version:   $Revision: 1.4 $
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

#include "lhpOpStickPalpation.h"

#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include "wx/busyinfo.h"
#include <math.h>

#include "mafDecl.h"
#include "mafOp.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafDictionary.h"
#include "mafOpExplodeCollapse.h"
#include "lhpOpImporterC3DBTK.h"

#include "mafSmartPointer.h"

#include "mafVME.h"
#include "mafVMEC3DData.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafMatrix.h"

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
  RIGID  = 0,
  SIMILARITY,
  AFFINE
};

#ifndef DIM
#define DIM(a)  (sizeof((a)) / sizeof(*(a)))
#endif


//----------------------------------------------------------------------------
// static persistent data:
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Forward Refs
//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpStickPalpation)

namespace {
  typedef mafVME *(MatchName)(mafVME *pVME, const char *name);
  //----------------------------------------------------------------------------
  mafVME *MatchStickDefinition(mafVME *pVME, const char *name)
  //----------------------------------------------------------------------------
  {
    bool bStickDetected = false;
    bool bWandDetected  = false;
    bool bDefDetected   = false;
    mafVME *paDetArray[2];
    wxInt32 nI;
    char const *pVMEName;

    //supposed to have correct type
    if(!pVME->IsA("mafVMELandmarkCloud"))
    {
      return NULL;
    }
    //this cloud is supposed to have words "stick, "wand and "definitions" somewhere in it's or it's parent name
    paDetArray[0] = pVME;
    paDetArray[1] = pVME->GetParent();
    for(nI = 0; nI < DIM(paDetArray); nI++)
    {
      if(paDetArray[nI] == NULL)
      {
        continue;
      }
      pVMEName = paDetArray[nI]->GetName().GetCStr();
      if(strstr(pVMEName, "stick") != NULL || strstr(pVMEName, "Stick") != NULL || strstr(pVMEName, "STICK") != NULL)
      {
        bStickDetected = true;
      }
      if(strstr(pVMEName, "wand") != NULL || strstr(pVMEName, "Wand") != NULL || strstr(pVMEName, "WAND") != NULL)
      {
        bWandDetected = true;
      }
      if(strstr(pVMEName, "definition") != NULL || strstr(pVMEName, "Definition") != NULL || strstr(pVMEName, "DEFINITION") != NULL)
      {
        bDefDetected = true;
      }
    }
    if(bDefDetected && (bStickDetected || bWandDetected))
    {
      return pVME;
    }
    return NULL;
  }

  //----------------------------------------------------------------------------
  mafVME *MatchStick(mafVME *pVME, const char *name)
  //----------------------------------------------------------------------------
  {
    bool bStickDetected = false;
    bool bWandDetected  = false;
    bool bPlpDetected   = false;
    char const *pVMEName;

    if(pVME == NULL)
      return NULL;
    //supposed to have correct type
    if(!pVME->IsA("mafVMELandmarkCloud"))
    {
      return NULL;
    }
    pVMEName = pVME->GetName().GetCStr();
    if(strstr(pVMEName, "stick") != NULL || strstr(pVMEName, "Stick") != NULL || strstr(pVMEName, "STICK") != NULL)
    {
      bStickDetected = true;
    }
    if(strstr(pVMEName, "wand") != NULL || strstr(pVMEName, "Wand") != NULL || strstr(pVMEName, "WAND") != NULL)
    {
      bWandDetected = true;
    }
    if(strstr(pVMEName, "palpator") != NULL || strstr(pVMEName, "Palpator") != NULL || strstr(pVMEName, "PALPATOR") != NULL)
    {
      bPlpDetected = true;
    }

    if(bPlpDetected || bStickDetected || bWandDetected)
    {
      return pVME;
    }
    return NULL;
  }
  //----------------------------------------------------------------------------
  mafVME *MatchWithName(mafVME *pVME, const char *name)
  //----------------------------------------------------------------------------
  {
    if(pVME == NULL)
      return NULL;
    //supposed to have correct type
    if(!pVME->IsA("mafVMELandmarkCloud"))
    {
      return NULL;
    }
    if(strstr(pVME->GetName().GetCStr(), name) != NULL)
      return pVME;
    return NULL;
  }
  //----------------------------------------------------------------------------
  mafVME *MatchCriterion(mafVME *pRoot, MatchName pCritFunc, const char *name)
  //----------------------------------------------------------------------------
  {
    wxInt32 nI;
    mafVME  *pRetVME = NULL;

    if(pRoot == NULL)
    {
      return NULL;
    }
    pRetVME = pCritFunc(pRoot, name);

    for(nI = 0; nI < pRoot->GetNumberOfChildren() && pRetVME == NULL; nI++)
    {
      pRetVME = MatchCriterion(mafVME::SafeDownCast(pRoot->GetChild(nI)), pCritFunc, name);
    }

    return pRetVME;
  }




  //----------------------------------------------------------------------------
  /*inline bool LMCSetState(mafVMELandmarkCloud *cloud, mafObserver *listener, bool openState)
  //----------------------------------------------------------------------------
  {
    bool LMCOpened = cloud->IsOpen();
    //if(LMCOpened == openState)
      return LMCOpened;
    if(openState)
    {
      mafOp *OpenOp = new mafOpExplodeCollapse("open cloud");
      OpenOp->SetInput(cloud);
      OpenOp->SetListener(listener);
      OpenOp->OpDo();
      cppDEL(OpenOp); 
      return LMCOpened;
    }
    {
      mafOp *CloseOp = new mafOpExplodeCollapse("close cloud");
      CloseOp->SetInput(cloud);
      CloseOp->SetListener(listener);
      CloseOp->OpDo();
      cppDEL(CloseOp); 
      return LMCOpened;
    }
  }*/
  //----------------------------------------------------------------------------
  void FindMatchingLM(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, std::vector<std::pair<int, int> >& lmpairs)
    //----------------------------------------------------------------------------
  {
    int    npSource    = src->GetNumberOfLandmarks();
    int    npTarget    = trg->GetNumberOfLandmarks();

    lmpairs.clear();

    for(int i = 0; i < npSource; i++)
    {
      mafString SourceLandmarkName = src->GetLandmarkName(i);
      //search for landmark with the same name
      for(int j = 0; j < npTarget; j++)
      {
        mafString TargetLandmarkName = trg->GetLandmarkName(j);
        if(SourceLandmarkName == TargetLandmarkName)
        {
          lmpairs.push_back(std::make_pair(i, j));
          break;
        }
      }
    }
  }

}
//----------------------------------------------------------------------------
lhpOpStickPalpation::lhpOpStickPalpation(const mafString& label) : Superclass(label), m_DictionaryFName(_R(""))
//----------------------------------------------------------------------------
{
  m_OpType               = OPTYPE_OP;
  m_Canundo              = true;
  
  m_StickCalibration     = NULL;
  m_StickCalibrationName = _R("");
  
  m_StickDefinition      = NULL;
  m_StickDefinitionName  = _R("");
  m_LimbCalibration      = NULL;
  m_LimbCalibrationName  = _R("");
  m_LimbCloud            = NULL;
  m_LimbCloudName        = _R("");
  m_TrgMotion            = NULL;
  m_TrgMotionName        = _R("");
  //m_NewLandmarkName      = "";

  m_WarnNamesNotMatched  = false;
  //m_NewIndex             = 0;
  m_Registered           = NULL;
  m_pointsSource         = NULL;
  m_pointsTarget         = NULL;
  m_RegistrationMode     = SIMILARITY;
  m_weight               = NULL;
}

//----------------------------------------------------------------------------
lhpOpStickPalpation::~lhpOpStickPalpation() 
//----------------------------------------------------------------------------
{
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
mafOp* lhpOpStickPalpation::Copy()
//----------------------------------------------------------------------------
{
  return new lhpOpStickPalpation(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpStickPalpation::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;

  if(!vme->IsA("mafVMELandmarkCloud"))
  {
    return false;
  }

  return true;
}


//----------------------------------------------------------------------------
void lhpOpStickPalpation::OpRun()
//----------------------------------------------------------------------------
{
  m_LimbCloud     = mafVMELandmarkCloud::SafeDownCast(m_Input);
  SetNodeName(m_LimbCloud, &m_LimbCloudName);

  m_TrgMotion     = mafVME::SafeDownCast(m_Input->GetParent());
  SetNodeName(m_TrgMotion, &m_TrgMotionName);

  if(m_StickDefinition == NULL)
  {
    m_StickDefinition = (mafVMELandmarkCloud *)MatchCriterion((mafVME *)m_Input->GetRoot(), MatchStickDefinition, NULL);
    if(m_StickDefinition)
      SetNodeName(m_StickDefinition, &m_StickDefinitionName);
  }

  CreateGui();
  assert(!m_pointsSource && !m_pointsTarget);
  m_pointsSource = vtkPoints::New();
  m_pointsTarget = vtkPoints::New();
}

//----------------------------------------------------------------------------
void lhpOpStickPalpation::CreateGui()
//----------------------------------------------------------------------------
{
  const mafString choices_string[] = {_L("rigid"), _L("similarity"), _L("affine")}; 
  if(m_Gui == NULL)
  {
    m_Gui = new mafGUI(this);
    m_Gui->SetListener(this);
    m_Gui->Label(_R(""));
    m_Gui->Button(ID_STICK_DEFINITION, _R("Wand definition cloud"), _R(""), _R("Press to select wand cloud with wand definition.") );
    m_Gui->Label(_R("Wnd d.VME"),&m_StickDefinitionName);
    m_Gui->Button(ID_STICK_CALIBRATION, _R("Wand calibration cloud"), _R(""), _R("Press to select cloud for wand calibration.") );
    m_Gui->Label(_R("Wnd c.VME"),&m_StickCalibrationName);
    m_Gui->Button(ID_LIMB_CALIBRATION, _R("Target calibration cloud"), _R(""), _R("Press to select target cloud with wand calibration.") );
    m_Gui->Label(_R("Trg c. VME:"),&m_LimbCalibrationName);
    m_Gui->Button(ID_LIMB_CLOUD, _R("Target cloud"), _R(""), _R("Press to select target cloud.") );
    m_Gui->Label(_R("Target VME: "),&m_LimbCloudName);

    m_Gui->Button(ID_TRG_MOT, _R("Target motion"), _R(""), _R("Press to select target motion") );
    m_Gui->Label(_R("Target VME: "),&m_TrgMotionName);


    m_Gui->Label(_R(""));
    m_Gui->FileOpen(ID_LOAD_SCRIPT, _R("Script"),  &m_ScriptFName, _R("Script file|*.srp"));
    m_Gui->Label(_R(""));
    m_Gui->Button(ID_CLEAN_SCRIPT, _R("Clean"), _R(""), _R("Press to cancel script using") );
    m_Gui->Label(_R(""));
    m_Gui->FileOpen(ID_LOAD_DICTIONARY, _R("Dictionary"),  &m_DictionaryFName, _R("Text files|*.txt|All files|*.*"));
    m_Gui->Label(_R(""));
    m_Gui->Combo(ID_REG_TYPE, _L("reg. type"), &m_RegistrationMode, 3, choices_string); 
    m_Gui->OkCancel();
  }
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpStickPalpation::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    {mafEvent evUnq(this,result); mafEventMacro(evUnq);}
  }
  else if (result == OP_RUN_OK)
  {
    if(m_ScriptFName.empty())
    {
      if(m_StickCalibration == NULL || m_StickDefinition == NULL || m_LimbCalibration == NULL || m_LimbCloud == NULL)
      {
        wxMessageBox("Not all data defined for operation.","Alert", wxOK , NULL);
        return;
      }
    }
    else
    {
      if(m_StickDefinition == NULL || (m_TrgMotion == NULL && m_LimbCloud == NULL))
      {
        wxMessageBox("Not all data defined for operation.","Alert", wxOK , NULL);
        return;
      }
    }
    HideGui();
    {mafEvent evUnq(this,result); mafEventMacro(evUnq);}
  }
}
//----------------------------------------------------------------------------
void lhpOpStickPalpation::SetNodeName(mafVME *pVME, mafString *pName) 
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
void lhpOpStickPalpation::OnEvent(mafEventBase *e) 
//----------------------------------------------------------------------------
{ 
  int  i;
  bool b;
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
    case ID_REG_TYPE:
      {
        break;
      }
    case ID_LOAD_DICTIONARY:
    {
      break;
    }
    case ID_LOAD_SCRIPT:
    {
      if(!m_ScriptFName.empty())
      {
        ReadDictionary(&m_ScriptFName, m_LMDict);
      }
      break;
    }
    case ID_CLEAN_SCRIPT:
    {
      m_ScriptFName = _R("");
      m_Gui->Update();
      break;
    }
    case ID_STICK_CALIBRATION:
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
      m_StickCalibration = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_StickCalibration, &m_StickCalibrationName);
      m_Gui->Update();
      break;
    }
    case ID_STICK_DEFINITION:
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
      m_StickDefinition = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_StickDefinition, &m_StickDefinitionName);
      m_Gui->Update();
      break;
    }
    case ID_LIMB_CALIBRATION:
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
      m_LimbCalibration = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_LimbCalibration, &m_LimbCalibrationName);
      m_Gui->Update();
      break;
    }
    case ID_LIMB_CLOUD:
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
      m_LimbCloud = (mafVMELandmarkCloud *)e.GetVme();
      SetNodeName(m_LimbCloud, &m_LimbCloudName);
      m_Gui->Update();
      break;
    }
    case ID_TRG_MOT:
    {
      mafString s(_R("Choose node with landmark clouds as children"));
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      b = false;
      for(i = 0; i < e.GetVme()->GetNumberOfChildren(); i++)
      {
        if(e.GetVme()->GetChild(i)->IsA("mafVMELandmarkCloud"))
        {
          b = true;
          break;
        }
      }
      if(!b)
      {
        mafWarningMessage(_M("Selected VME should contain mafVMELandmarkCloud as a child."));
        return;
      }
      m_TrgMotion = mafVME::SafeDownCast(e.GetVme());
      SetNodeName(m_TrgMotion, &m_TrgMotionName);
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
void lhpOpStickPalpation::GetLandmark(mafVMELandmarkCloud  *pStickCloud, int nIDx, double vpPoint[4], mafTimeStamp t) const
//----------------------------------------------------------------------------
{
  pStickCloud->GetLandmark(nIDx, vpPoint, t);
  vpPoint[3] = 1.;
}

//----------------------------------------------------------------------------
void lhpOpStickPalpation::OpDo()
//----------------------------------------------------------------------------
{
  wxInt32 nL;
  mafVME  *vme;
  
  //modified by Stefano. 18-9-2003
  wxBusyInfo wait("Please wait, working...");

  if(m_ScriptFName.empty())
  {
    ProcessSingleLM();
    return;
  }

  if(m_DictionaryFName.empty())
    wxMessageBox("Dictionary for c3d import is not specified. Trying to use C3D_dictionary.txt","Alert", wxOK , NULL);


  mafString spath, sname, sext;
  mafSplitPath(m_ScriptFName, &spath, &sname, &sext);

  for(nL = 0; nL < m_LMDict.size(); nL++)
  {
    mafString file(spath);
    mafString dict(m_DictionaryFName);

    file += _R("\\");
    file += m_LMDict[nL].first; 
    file += _R(".c3d");

    if(dict.empty())
    {
      dict  = spath;
      dict += _R("\\C3D_dictionnary.txt");
    }

    m_LimbCloud        = NULL;
    m_LimbCalibration  = NULL;
    m_StickCalibration = NULL;

    mafLogMessage(_M(_R("Importing landmark ") + m_LMDict[nL].first + _R(" to segment ") + m_LMDict[nL].second));
#ifndef OLD_IMPORTER
    lhpOpImporterC3DBTK *importer=new lhpOpImporterC3DBTK(_R("importer"));
    //importer->TestModeOn();
    importer->SetC3DFileName(file.GetCStr());
    importer->SetDictionaryFileName(dict.GetCStr());
    importer->SetImportTrajectories(true);
    importer->SetImportAnalog(false);
    importer->SetImportPlatform(false);
    importer->SetImportEvent(false);
    importer->Import();
    mafVMEGroup *c3dImported = importer->GetGroup();
    c3dImported->Register(this);
    vme = c3dImported;
    delete importer;
#else

    mafVMEC3DData *reader;
    mafNEW(reader);
    reader->SetFileName(file.GetCStr());
    reader->SetDictionaryFileName(dict.GetCStr());

    reader->DictionaryOn();

    reader->Read();

    vme = reader;
#endif
    mafString path, name, ext;
    mafSplitPath(file,&path,&name,&ext);
    vme->SetName(name);
    {mafEvent evUnq(this,VME_ADD,vme); mafEventMacro(evUnq);}

    m_LimbCloud        = mafVMELandmarkCloud::SafeDownCast(MatchCriterion(m_TrgMotion, MatchWithName, m_LMDict[nL].second.GetCStr()));
    m_LimbCalibration  = mafVMELandmarkCloud::SafeDownCast(MatchCriterion(vme, MatchWithName, m_LMDict[nL].second.GetCStr()));
    m_StickCalibration = mafVMELandmarkCloud::SafeDownCast(MatchCriterion(vme, MatchStick, NULL));

    if(m_LimbCloud == NULL)
      wxLogMessage("Target cloud is not found");
    if(m_LimbCalibration == NULL)
      wxLogMessage("Target calibration cloud is not found");
    if(m_StickCalibration == NULL)
      wxLogMessage("Wand calibration cloud is not found");
    if(m_LimbCloud != NULL && m_LimbCalibration != NULL && m_StickCalibration != NULL)
      ProcessSingleLM();
    {mafEvent evUnq(this,VME_REMOVE,vme); mafEventMacro(evUnq);}
    mafDEL(vme);
  }
}

//----------------------------------------------------------------------------
void lhpOpStickPalpation::ProcessSingleLM()
//----------------------------------------------------------------------------
{
  V4d<double>               localTip;
  std::vector<V4d<double> > tips;
  std::vector<int>          tipsIdx;
  std::vector<mafTimeStamp> kframes1;
  std::vector<mafTimeStamp> kframes2;

  std::vector<std::pair<int, int> > stickDefCalib;
  std::vector<std::pair<int, int> > limbCalibData;

  FindMatchingLM(m_StickDefinition, m_StickCalibration, stickDefCalib);
  if(stickDefCalib.size() < 3)
  {
    wxLogMessage("Stick definition and stick calibration clouds have not enough common points.");
    return;
  }

  FindMatchingLM(m_LimbCalibration, m_LimbCloud, limbCalibData);
  if(limbCalibData.size() < 3)
  {
    wxLogMessage("Limb calibration and target clouds have not enough common points.");
    return;
  }

  mafVME *vmlc = NULL;
  if(m_LimbCalibration->IsOpen())
    vmlc = m_LimbCalibration->GetLandmark(0);
  else
    vmlc = m_LimbCalibration;
  mafVME *vmsc = NULL;
  if(m_StickCalibration->IsOpen())
    vmsc = m_StickCalibration->GetLandmark(0);
  else
    vmsc = m_StickCalibration;

  if(vmlc != NULL)
    vmlc->GetAbsTimeStamps(kframes1);
  if(vmsc != NULL)
    vmsc->GetAbsTimeStamps(kframes2);

  if(kframes1.size() == 0)
  {
    wxLogMessage("Target calibration has no frames.");
    return;
  }
  if(kframes1.size() != kframes2.size())
  {
    wxLogMessage("Wand calibration and target calibration have different number of frames.");
    return;
  }
  for(unsigned i = 0; i < kframes1.size(); i++)
  {
    if(kframes1[i] != kframes2[i])
    {
      wxLogMessage("Wand calibration and target calibration have different timestamps.");
      return;
    }
  }

  //2. find Tip coods in this system
  int tipIndex = m_StickDefinition->FindLandmarkIndex(_R("TIP"));
  if(tipIndex == -1)
  {
    wxLogMessage("TIP mark not found on wand. Either revise your dictionary or rename tip to TIP.");
    return;
  }
  m_StickDefinition->GetLandmark(tipIndex, localTip.components);
  localTip[3] = 1.0;

  for(int i = 0; i < kframes1.size(); i++)
  {
    double deviation = 0.0;
    V4d<double> result;
    mafTimeStamp currTime  = kframes1[i];
    mafMatrix    regMatrix;
    mafMatrix    absMatrStick;

    m_StickCalibration->GetOutput()->GetAbsMatrix(absMatrStick, currTime);
    if(ExtractMatchingPoints(m_StickDefinition, m_StickCalibration, stickDefCalib, -1, currTime) >= 3)
      deviation = RegisterPoints(regMatrix.GetVTKMatrix());
    else
    {
      wxLogMessage("Stick matching. Not enough visible matching landmarks found at timestamp %lf", currTime);
      continue;
    }

    V4d<double> tipPnt = localTip;

    regMatrix.MultiplyPoint(tipPnt.components, result.components);
    result[3] = 1.0;
    absMatrStick.MultiplyPoint(result.components, result.components);
    result[3] = 1.0;
    tips.push_back(result);
    tipsIdx.push_back(i);
  }

  if(tips.size() == 0)
  {
    wxLogMessage("Stick definition to stick calibration are not matchable.");
    return;
  }



  std::vector<V3d<double> > lmPositions;
  std::vector<int>          lmPositionNums;
  lmPositions.resize(m_LimbCalibration->GetNumberOfLandmarks());
  lmPositionNums.resize(m_LimbCalibration->GetNumberOfLandmarks());
  for(int i = 0; i < m_LimbCalibration->GetNumberOfLandmarks(); i++)
  {
    lmPositions[i] = V3d<double>(0.0, 0.0, 0.0);
    lmPositionNums[i] = 0;
  }

  int          referenceIndex = 0;
  mafTimeStamp referenceTS    = kframes1[tipsIdx[referenceIndex]];
  mafMatrix absMatrLimbRef;
  m_LimbCalibration->GetOutput()->GetAbsMatrix(absMatrLimbRef, referenceTS);
  for(int i = 0; i < m_LimbCalibration->GetNumberOfLandmarks(); i++)
  {
    if(m_LimbCalibration->GetLandmarkVisibility(i, referenceTS))
    {
      V4d<double> lmPos;
      m_LimbCalibration->GetLandmark(i, lmPos[0], lmPos[1], lmPos[2], referenceTS);
      lmPos[3] = 1.0;
      absMatrLimbRef.MultiplyPoint(lmPos.components, lmPos.components);
      lmPositions[i] = V3d<double>(lmPos.components);
      lmPositionNums[i] = 1;
    }
  }

  localTip[0] = tips[referenceIndex][0];
  localTip[1] = tips[referenceIndex][1];
  localTip[2] = tips[referenceIndex][2];
  localTip[3] = 1.0;//tips[0][3]

  int numberRegistered = 1;
#ifndef SKIP_AVERAGING
  std::vector<std::pair<int, int> > limbCalibCalib;
  for(int i = 0; i < m_LimbCalibration->GetNumberOfLandmarks(); i++)
    limbCalibCalib.push_back(std::make_pair(i, i));
  for(int i = 0; i < tipsIdx.size(); i++)
  {
    if(i == referenceIndex)
      continue;;

    double deviation = 0.0;
    V4d<double> tippos;
    V4d<double> tipsI;
    mafTimeStamp  currTime  = kframes1[tipsIdx[i]];
    mafMatrix regMatrix;
    mafMatrix absMatrLimb;

    m_LimbCalibration->GetOutput()->GetAbsMatrix(absMatrLimb, currTime);
    absMatrLimb.Invert();
    
    if(ExtractMatchingPoints(m_LimbCalibration, m_LimbCalibration, limbCalibCalib, currTime, referenceTS) >= 3)
      deviation = RegisterPoints(regMatrix.GetVTKMatrix());
    else
    {
      wxLogMessage("Target calibration averaging. Not enough visible matching landmarks found to register timestamp %lf to timestamp %ls", currTime, kframes1[tipsIdx[0]]);
      continue;
    }
    numberRegistered++;

    tipsI = tips[i];
    absMatrLimb.MultiplyPoint(tipsI.components, tipsI.components);
    tipsI[3] = 1.0;
    regMatrix.MultiplyPoint(tipsI.components, tippos.components);
    absMatrLimbRef.MultiplyPoint(tippos.components, tippos.components);
    localTip += tippos;
    localTip[3] = 1.0;

    for(int j = 0; j < m_LimbCalibration->GetNumberOfLandmarks(); j++)
    {
      if(m_LimbCalibration->GetLandmarkVisibility(j, currTime))
      {
        V4d<double> lmpos;
        V4d<double> newpos;
        m_LimbCalibration->GetLandmark(j, lmpos.components, currTime);
        lmpos[3] = 1.0;

        absMatrLimb.MultiplyPoint(lmpos.components, lmpos.components);
        lmpos[3] = 1.0;

        regMatrix.MultiplyPoint(lmpos.components, newpos.components);
        newpos[3] = 1.0;

        absMatrLimbRef.MultiplyPoint(newpos.components, newpos.components);

        lmPositions[j] += V3d<double>(newpos.components);
        lmPositionNums[j]++;
      }
    }
  }
#endif
  if(numberRegistered == 0)
  {
    wxLogMessage("Problem processing target calibration.");
    return;
  }
  for(int i = 0; i < m_LimbCalibration->GetNumberOfLandmarks(); i++)
  {
    if(lmPositionNums[i])
      lmPositions[i] /= lmPositionNums[i];
  }
  localTip /= numberRegistered;

  //create cloud for averaged positions
  mafVMELandmarkCloud *averagedCalibr;
  mafNEW(averagedCalibr);
  averagedCalibr->Open();
  averagedCalibr->SetName(_R("averaged landmark cloud"));
  averagedCalibr->SetRadius(15);
  averagedCalibr->SetNumberOfLandmarks(m_LimbCalibration->GetNumberOfLandmarks() + 1);

  for(int i = 0; i < m_LimbCalibration->GetNumberOfLandmarks(); i++)
  {
    averagedCalibr->SetLandmarkName(i, m_LimbCalibration->GetLandmarkName(i));
    averagedCalibr->SetLandmark(i, lmPositions[i][0], lmPositions[i][1], lmPositions[i][2]);
    averagedCalibr->SetLandmarkVisibility(i, lmPositionNums[i] != 0);
  }
  averagedCalibr->SetLandmarkName(m_LimbCalibration->GetNumberOfLandmarks(), _R("TIPPED"));
  averagedCalibr->SetLandmark(m_LimbCalibration->GetNumberOfLandmarks(), localTip[0], localTip[1], localTip[2]);
  averagedCalibr->SetLandmarkVisibility(m_LimbCalibration->GetNumberOfLandmarks(), true);



  if(m_LimbCloud->IsOpen())
  {
    mafVMELandmark *lm = m_LimbCloud->GetLandmark(0);
    if(lm != NULL)
      lm->GetMatrixTimeStamps(kframes1);
  }
  else
  {
    m_LimbCloud->GetDataTimeStamps(kframes1);
  }

  bool added = false;
  int  newIndex = -1;
  for (int t = 0; t < kframes1.size(); t++)
  {
    mafMatrix    regMatrix;
    mafTimeStamp currTime  = kframes1[t];
    double       deviation = 0.0;
    V4d<double>  palpatedLM;

    //calculate registration transform
    if(ExtractMatchingPoints(averagedCalibr, m_LimbCloud, limbCalibData, -1, currTime) >= 3)
      deviation = RegisterPoints(regMatrix.GetVTKMatrix());
    else
    {
      wxLogMessage("Adding palpated point. Not enough visible matching landmarks found for timestamp %lf", currTime);
      continue;
    }

    //add landmark if not existed
    if(!added)
    {
      mafString lmNameNew;
      int index = 0;
      lmNameNew = m_LimbCalibration->GetParent()->GetName();
      do 
      {
        newIndex = m_LimbCloud->FindLandmarkIndex(lmNameNew);
        index++;
        lmNameNew = m_LimbCalibration->GetParent()->GetName() + mafToString(index);
      }
      while(newIndex != -1);

      newIndex = m_LimbCloud->AppendLandmark(m_LimbCalibration->GetParent()->GetName());
      if(newIndex == -1)
        continue;
      m_LimbCloud->Modified();
      m_LimbCloud->Update();
      added = true;
    }

    regMatrix.MultiplyPoint(localTip.components, palpatedLM.components);
    m_LimbCloud->SetLandmark(newIndex, palpatedLM[0], palpatedLM[1], palpatedLM[2], currTime);
  }
  m_UndoList.push_back(std::make_pair(m_LimbCloud, newIndex));

  mafDEL(averagedCalibr);
}


//----------------------------------------------------------------------------
void lhpOpStickPalpation::OpUndo()
//----------------------------------------------------------------------------
{
  for(int i = m_UndoList.size(); i > 0; i--)
  {
    mafVMELandmarkCloud *lmUpd   = m_UndoList[i - 1].first;
    wxInt32             lmUpdIdx = m_UndoList[i - 1].second;
    //bool bCloudWasOpen = LMCSetState(lmUpd, m_Listener, false);

    lmUpd->RemoveLandmark(lmUpdIdx);
    lmUpd->Modified();

    //LMCSetState(lmUpd, m_Listener, bCloudWasOpen);
  }
  m_UndoList.clear();
}

//----------------------------------------------------------------------------
int lhpOpStickPalpation::ExtractMatchingPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, const std::vector<std::pair<int, int> >& lmpairs, mafTimeStamp srctime, mafTimeStamp trgtime)
//----------------------------------------------------------------------------
{
  V3d<double> lmsrc;
  V3d<double> lmtrg;
  int         ncp = 0;

  m_pointsSource->Reset();
  m_pointsTarget->Reset();

  //number of common points in src and trg
  for(int i = 0; i < lmpairs.size(); i++)
  {
    //if is separated into 2 for debug purposes
    if(!src->GetLandmarkVisibility(lmpairs[i].first,srctime))
      continue;
    if(!trg->GetLandmarkVisibility(lmpairs[i].second,trgtime))
      continue;

    //add new points to arrays if no one is zero (zero indicates that data are invalid)
    src->GetLandmark(lmpairs[i].first,  lmsrc.components, srctime);
    trg->GetLandmark(lmpairs[i].second, lmtrg.components, trgtime);
    m_pointsSource->InsertNextPoint(lmsrc.components);
    m_pointsTarget->InsertNextPoint(lmtrg.components);
    ncp++;
  }
  m_numPoints = ncp;
  return ncp;
}
//----------------------------------------------------------------------------
double lhpOpStickPalpation::RegisterPoints(vtkMatrix4x4 *res_matrix)
//----------------------------------------------------------------------------
{
  int i;
  double deviation = 0.0;
  double dx, dy, dz;
  assert(m_pointsSource && m_pointsTarget);

  vtkWeightedLandmarkTransform *RegisterTransform;
  vtkNEW(RegisterTransform);

  //setup transform calculator
  RegisterTransform->SetSourceLandmarks(m_pointsSource);
  RegisterTransform->SetTargetLandmarks(m_pointsTarget);
  
  if(m_weight)
  {
    RegisterTransform->SetWeights(m_weight, m_numPoints);
  }

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

  //calculate transform
  RegisterTransform->Update();
  RegisterTransform->GetMatrix(res_matrix);

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

    deviation += dx * dx + dy * dy + dz * dz;
  }
  if(m_pointsSource->GetNumberOfPoints() != 0)
    deviation /= m_pointsSource->GetNumberOfPoints();
  deviation = sqrt(deviation);

  vtkDEL(RegisterTransform);
  return deviation;
}
