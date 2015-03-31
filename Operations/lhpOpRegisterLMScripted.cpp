/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRegisterLMScripted.cpp,v $
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

#include "lhpOpRegisterLMScripted.h"

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
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpRegisterLMScripted);
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
lhpOpRegisterLMScripted::lhpOpRegisterLMScripted(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType           = OPTYPE_OP;
  m_Canundo          = true;
  
  m_Source           = NULL;
  m_Target           = NULL;
  m_Registered       = NULL;
  m_PointsSource     = NULL;
  m_PointsTarget     = NULL;
  m_SourceName       ="none";
  m_TargetName       ="none";
  m_MultiTime        = 0;
  m_RegistrationMode = RIGID;
  m_ListFName        = "";
}
//----------------------------------------------------------------------------
lhpOpRegisterLMScripted::~lhpOpRegisterLMScripted( ) 
//----------------------------------------------------------------------------
{
  vtkDEL(m_PointsSource);
  vtkDEL(m_PointsTarget);
  mafDEL(m_Registered);
}
//----------------------------------------------------------------------------
mafOp* lhpOpRegisterLMScripted::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpRegisterLMScripted(m_Label);
}
//----------------------------------------------------------------------------
bool lhpOpRegisterLMScripted::Accept(mafNode* node)
//----------------------------------------------------------------------------
{
  if(!node) return false;
  return true;
  //if( node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen() )
  if(ClosedCloudAccept(node))
  {
    if(!mafVMELandmarkCloud::SafeDownCast(node)->IsAnimated())
      return true;
  }
  return false;
};
//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_CHOOSE = MINID,
  ID_MULTIPLE_TIME_REGISTRATION,
  ID_REGTYPE,
  ID_LOAD_SCRIPT,
};
//----------------------------------------------------------------------------
void lhpOpRegisterLMScripted::OpRun()   
//----------------------------------------------------------------------------
{
  m_Source = (mafVMELandmarkCloud*)m_Input;
  m_SourceName = m_Input->GetName();
  
  int num_choices = 3;
  const wxString choices_string[] = {_("rigid"), _("similarity"), _("affine")}; 
  mafString wildcard = "Dictionary (*.txt)|*.txt|All Files (*.*)|*.*";

  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  
  m_Gui->Label(_("source :"),true);
  m_Gui->Label(&m_SourceName);
  
  m_Gui->Label(_("target :"),true);
  m_Gui->Label(&m_TargetName);
  m_Gui->Button(ID_CHOOSE,_("target "));
  
  m_Gui->Combo(ID_REGTYPE, _("reg. type"), &m_RegistrationMode, num_choices, choices_string); 
  
  m_Gui->Bool(ID_MULTIPLE_TIME_REGISTRATION,_("multi-time"),&m_MultiTime,1);
  m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,false);
  
  m_Gui->FileOpen(ID_LOAD_SCRIPT, "Script",  &m_ListFName, "*.txt");
  m_Gui->Label("");

  m_Gui->OkCancel();

  m_Gui->Enable(wxOK,false);
  m_Gui->Divider();
  ShowGui();

  assert(!m_PointsSource && !m_PointsTarget);
  m_PointsSource = vtkPoints::New();
  m_PointsTarget = vtkPoints::New();

}

//----------------------------------------------------------------------------
void  lhpOpRegisterLMScripted::ParseString(wxString &pFirstLine, wxString &sOne, wxString &sTwo)
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
bool lhpOpRegisterLMScripted::ReadLMDictionary(mafString *fileName)
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
void lhpOpRegisterLMScripted::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_CHOOSE:
      {
        mafString s(_("Choose cloud"));
        mafEvent e(this,VME_CHOOSE, &s, NULL/*, (long)&lhpOpRegisterLMScripted::ClosedCloudAccept*/);
        mafEventMacro(e);
        mafNode *vme = e.GetVme();
        OnChooseTargetVme(vme);
      }
      break;
      case ID_REGTYPE:
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

bool lhpOpRegisterLMScripted::RegistrationProcedure()
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, working...");
  }

  if(m_Registered == NULL)
  {
    wxString name = wxString::Format("%s registered on %s",m_Source->GetName(), m_Target->GetName());
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

bool lhpOpRegisterLMScripted::ProcessNode(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *registered)
{
  mafVMEInfoText *info;
  mafNEW(info);
  wxString name = wxString::Format("Info for registration %s into %s",m_Source->GetName(), m_Target->GetName());
  info->SetName(name);
  info->SetPosLabel("Registration residual: ", 0);
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


  mafVMEGenericAbstract *reg = registered;
  if(mafVMESurface *srf = mafVMESurface::SafeDownCast(registered->GetParent()))
    reg = srf;
  if(m_MultiTime)
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
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,p));
      //Set the new time for the vme used to register the one frame source 
      trg->SetTimeStamp(currTime); //set current time
      trg->Update(); //>UpdateAllData();
      if(ExtractMatchingPoints(src, trg, currTime))
      {
        if(!infoAdded)
          info->ReparentTo(reg);
        infoAdded = true;
        double tr = RegisterPoints(src, trg, reg, currTime);
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
        info->ReparentTo(reg);
      infoAdded = true;
      double tr = RegisterPoints(src, trg, reg);
      info->SetAbsPose(tr, 0.0, 0.0, 0.0, 0.0, 0.0);
    }
  }
  mafDEL(info);
  if(lmcsOpened)
    src->Close();
  if(lmctOpened)
    trg->Close();
  if(lmcrOpened)
    registered->Close();
  return true;
}

//----------------------------------------------------------------------------
void lhpOpRegisterLMScripted::OpDo()
//----------------------------------------------------------------------------
{
  m_Registered->ReparentTo(m_Input->GetRoot());
}
//----------------------------------------------------------------------------
void lhpOpRegisterLMScripted::OpUndo()
//----------------------------------------------------------------------------
{
  m_Registered->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
int lhpOpRegisterLMScripted::ExtractMatchingPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, double time)
//----------------------------------------------------------------------------
{
  m_PointsSource->Reset();
  m_PointsTarget->Reset();

  src->Update();
  vtkDataSet *polySource =src->GetOutput()->GetVTKData();
  vtkDataSet *polyTarget =trg->GetOutput()->GetVTKData();

  polySource->Update();
  polyTarget->Update();

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
    wxString SourceLandmarkName = src->GetLandmarkName(i);

    bool found = false;
    for(j=0;j<npTarget;j++)
    {
      wxString TargetLandmarkName = trg->GetLandmarkName(j);
      if(mafString(SourceLandmarkName) == mafString(TargetLandmarkName))
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
double lhpOpRegisterLMScripted::RegisterPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMEGenericAbstract *reg, double currTime)
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
  mafMatrix *mat;
  mafNEW(mat);
  mat->Identity();
  trg->GetOutput()->GetAbsMatrix(*mat,currTime);  //modified by Marco. 2-2-2004
  vtkMatrix4x4::Multiply4x4(mat->GetVTKMatrix(),RegisterTransform->GetMatrix(),t_matrix);
  mafDEL(mat);
  vtkDEL(RegisterTransform);

  int numLandmarks = trg->GetNumberOfVisibleLandmarks(currTime);

  if((numLandmarks < 2) || ((numLandmarks < 4) && (m_RegistrationMode == AFFINE)))
  {
    vtkDEL(t_matrix);
    return deviation;
  }

  reg->SetTimeStamp(currTime);
  mafMatrix temp;
  temp.SetVTKMatrix(t_matrix);
  temp.SetTimeStamp(currTime);
  temp.Modified();
  reg->GetOutput()->Update();
  reg->SetAbsMatrix(temp);
  reg->Modified();
  reg->Update();
  vtkDEL(t_matrix);
  return deviation;
}
//----------------------------------------------------------------------------
void lhpOpRegisterLMScripted::OnChooseTargetVme(mafNode *vme)
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




//----------------------------------------------------------------------------
/*void lhpOpRegisterLMScripted::OpDo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_OutputVMEs.size(); i++)
  {
    if (m_OutputVMEs[i])
    {
      m_OutputVMEs[i]->ReparentTo(m_Input);
      mafEventMacro(mafEvent(this, VME_ADD, m_OutputVMEs[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void lhpOpRegisterLMScripted::OpUndo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_OutputVMEs.size(); i++)
  {
    if (m_OutputVMEs[i])
    {
      mafEventMacro(mafEvent(this, VME_REMOVE, m_OutputVMEs[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}*/
