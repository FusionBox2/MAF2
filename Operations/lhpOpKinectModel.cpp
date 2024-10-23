/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpKinectModel.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:19:42 $
  Version:   $Revision: 1.4 $
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

#include "lhpOpKinectModel.h"

#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include <wx/wxprec.h>
#include "wx/busyinfo.h"
#include <math.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafPlotMath.h"

#include "mafOpExplodeCollapse.h"
#include "medOpImporterLandmark.h"
#include "lhpOpKinectAFs.h"

#include "ftk/Base/RegisteringPointer.h"
#include "mafMatrixVector.h"
#include "mafDataVector.h"
#include "mafVME.h"
#include "mafVMEGroup.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include <fstream>
#include <sstream>
#include <stack>
#include <string>

//----------------------------------------------------------------------------
// Required for MSVC
//----------------------------------------------------------------------------
#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_DEFAULT = MINID,
  ID_EXTAPPPATH,
  ID_SCALE,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

class medOpImporterLandmarkAcc : public medOpImporterLandmark
{
public:
  mafTypeMacro(medOpImporterLandmarkAcc, medOpImporterLandmark);
  medOpImporterLandmarkAcc(const mafString& label = _R("")) : medOpImporterLandmark(label){}
  std::vector<mafVME*>& GetResults(){return m_Results;}
};

mafCxxTypeMacro(medOpImporterLandmarkAcc)
mafCxxTypeMacro(lhpOpKinectModel)
//----------------------------------------------------------------------------
lhpOpKinectModel::lhpOpKinectModel(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType     = OPTYPE_OP;
  m_Canundo    = false;
  m_FileDir    = mafGetApplicationDirectory() + _R("/Data/External/");
  m_ExtAppPath = mafGetApplicationDirectory() + _R("/SkeletalViewerBart.exe");
  m_Scale1     = 1.0;
  m_Scale2     = 1.0;
  m_Scale3     = 1.0;
}

//----------------------------------------------------------------------------
lhpOpKinectModel::~lhpOpKinectModel()
//----------------------------------------------------------------------------
{
  Clear();
}

//----------------------------------------------------------------------------
mafOp* lhpOpKinectModel::Copy()
//----------------------------------------------------------------------------
{
  lhpOpKinectModel *op = new lhpOpKinectModel(GetLabel());
  op->m_Canundo = m_Canundo;
  op->m_OpType = m_OpType;
  op->SetListener(GetListener());
  return op;
}

//----------------------------------------------------------------------------
bool lhpOpKinectModel::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(vme);
  if(!cloud)
    return false;
  int pelv_ind[4] = {-1, -1, -1, -1};
  int rleg_ind[3] = {-1, -1, -1};
  int lleg_ind[3] = {-1, -1, -1};

  for(int i = 0; i < cloud->GetNumberOfLandmarks(); i++)
  {
    if(cloud->GetLandmarkName(i) == _R("LASI") || cloud->GetLandmarkName(i) == _R("LeftHip"))
      pelv_ind[0] = i;
    if(cloud->GetLandmarkName(i) == _R("RASI") || cloud->GetLandmarkName(i) == _R("RightHip"))
      pelv_ind[1] = i;
    if(cloud->GetLandmarkName(i) == _R("RPSI") || cloud->GetLandmarkName(i) == _R("Pelvis"))
      pelv_ind[2] = i;
    if(cloud->GetLandmarkName(i) == _R("LPSI") || cloud->GetLandmarkName(i) == _R("Pelvis"))
      pelv_ind[3] = i;

    if(cloud->GetLandmarkName(i) == _R("RKNE") || cloud->GetLandmarkName(i) == _R("RightKnee"))
      rleg_ind[0] = i;
    if(cloud->GetLandmarkName(i) == _R("RANK") || cloud->GetLandmarkName(i) == _R("RightAnkle"))
      rleg_ind[1] = i;
    if(cloud->GetLandmarkName(i) == _R("RTOE") || cloud->GetLandmarkName(i) == _R("RightFoot"))
      rleg_ind[2] = i;

    if(cloud->GetLandmarkName(i) == _R("LKNE") || cloud->GetLandmarkName(i) == _R("LeftKnee"))
      lleg_ind[0] = i;
    if(cloud->GetLandmarkName(i) == _R("LANK") || cloud->GetLandmarkName(i) == _R("LeftAnkle"))
      lleg_ind[1] = i;
    if(cloud->GetLandmarkName(i) == _R("LTOE") || cloud->GetLandmarkName(i) == _R("LeftFoot"))
      lleg_ind[2] = i;
  }
  for(int i = 0; i < 4; i++)
  {
    if(pelv_ind[i] == -1)
      return false;
    if(i == 3)
      break;
    if(lleg_ind[i] == -1 || rleg_ind[i] == - 1)
      return false;
  }

  return true;
}


//----------------------------------------------------------------------------
void lhpOpKinectModel::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    CreateGui();
    ShowGui();
  }
  else
  {
    if(Import())
    {
      {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
    }
    else
    {
      {mafEvent evUnq(this,OP_RUN_CANCEL); InvokeEvent(evUnq);}
    }
  }
}

//----------------------------------------------------------------------------
void lhpOpKinectModel::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
    case wxOK:
    {
      if(Import())
      {
        this->OpStop(OP_RUN_OK);
      }
      else
      {
        this->OpStop(OP_RUN_CANCEL);
      }
    }
    break;
    case wxCANCEL:
    {
      this->OpStop(OP_RUN_CANCEL);
    }
    break;
    case ID_EXTAPPPATH:
      break;
    case ID_SCALE:
      break;
    default:
    {
      InvokeEvent(*maf_event); 
    }
    break;
  }
}

//----------------------------------------------------------------------------
void lhpOpKinectModel::OpDo()
//----------------------------------------------------------------------------
{
  wxBusyInfo wait("Please wait, create all VMEs in tree");

  for(unsigned i = 0; i < m_Imported.size(); i++)
  {
    m_Imported[i]->ReparentTo(m_Input->GetParent());
  }
}

//----------------------------------------------------------------------------
void lhpOpKinectModel::OpUndo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Imported.size(); i++)
  {
    m_Imported[i]->ReparentTo(NULL);
  }
}
//----------------------------------------------------------------------------
void lhpOpKinectModel::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->FileOpen(ID_EXTAPPPATH, _R("Ext app"), &m_ExtAppPath, _R("*.exe"));
  m_Gui->Double(ID_SCALE, _L("Scale1"), &m_Scale1, 0.0);
  m_Gui->Double(ID_SCALE, _L("Scale2"), &m_Scale2, 0.0);
  m_Gui->Double(ID_SCALE, _L("Scale3"), &m_Scale3, 0.0);

  m_Gui->OkCancel();
}

//----------------------------------------------------------------------------
bool lhpOpKinectModel::Import()
//----------------------------------------------------------------------------
{
  bool result = false;
  Clear();

  if(m_ExtAppPath.empty())
    return false;

  std::vector<mafTimeStamp> timeStamps;
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);
  cloud->GetTimeStamps(timeStamps);
  int    numframes = timeStamps.size();

  mafString path, nameext;
  mafSplitPath(m_ExtAppPath, &path, &nameext);
  size_t length = path.length();
  if(length != 0 && path[length - 1] != '/' && path[length - 1] != '\\')
    path += _R("/");

  mafString params;
  params = path;
  params += _R("Inp_FuBx.dat");
  double lasttimestamp = 0.0;
  if(numframes > 0)
    lasttimestamp = timeStamps[numframes - 1];
  if(FILE *fp = fopen(params.GetCStr(), "wt"))
  {
    fputs("Scale_Kin/Anthr, Scale_Kin/S035 \n", fp);
    fprintf(fp, "%f, %f, %f\n", m_Scale1, m_Scale2, m_Scale3);
    fputs("tk,NSolu \n", fp);
    fprintf(fp, "%f , %d\n", lasttimestamp, numframes);
    fclose(fp);
  }

  mafString pelv_names[] = {_R("LASI"), _R("RASI"), _R("RPSI"), _R("LPSI")};
  mafString rleg_names[] = {_R("RTHI"), _R("RKNE"), _R("RTIB"), _R("RANK"), _R("RHEE"), _R("RTOE")};
  mafString lleg_names[] = {_R("LTHI"), _R("LKNE"), _R("LTIB"), _R("LANK"), _R("LHEE"), _R("LTOE")};
  int pelv_ind[4] = {-1, -1, -1, -1};
  int rleg_ind[3] = {-1, -1, -1};
  int lleg_ind[3] = {-1, -1, -1};

  for(int i = 0; i < cloud->GetNumberOfLandmarks(); i++)
  {
    if(cloud->GetLandmarkName(i) == _R("LASI") || cloud->GetLandmarkName(i) == _R("LeftHip"))
      pelv_ind[0] = i;
    if(cloud->GetLandmarkName(i) == _R("RASI") || cloud->GetLandmarkName(i) == _R("RightHip"))
      pelv_ind[1] = i;
    if(cloud->GetLandmarkName(i) == _R("RPSI") || cloud->GetLandmarkName(i) == _R("Pelvis"))
      pelv_ind[2] = i;
    if(cloud->GetLandmarkName(i) == _R("LPSI") || cloud->GetLandmarkName(i) == _R("Pelvis"))
      pelv_ind[3] = i;

    if(cloud->GetLandmarkName(i) == _R("RKNE") || cloud->GetLandmarkName(i) == _R("RightKnee"))
      rleg_ind[0] = i;
    if(cloud->GetLandmarkName(i) == _R("RANK") || cloud->GetLandmarkName(i) == _R("RightAnkle"))
      rleg_ind[1] = i;
    if(cloud->GetLandmarkName(i) == _R("RTOE") || cloud->GetLandmarkName(i) == _R("RightFoot"))
      rleg_ind[2] = i;

    if(cloud->GetLandmarkName(i) == _R("LKNE") || cloud->GetLandmarkName(i) == _R("LeftKnee"))
      lleg_ind[0] = i;
    if(cloud->GetLandmarkName(i) == _R("LANK") || cloud->GetLandmarkName(i) == _R("LeftAnkle"))
      lleg_ind[1] = i;
    if(cloud->GetLandmarkName(i) == _R("LTOE") || cloud->GetLandmarkName(i) == _R("LeftFoot"))
      lleg_ind[2] = i;
  }

  mafString fn1;
  mafString fn2;
  mafString fn3;

  fn1 = path;
  fn1 += _R("Pelvis_mot.txt");
  fn2 = path;
  fn2 += _R("R_Foot_mot.txt");
  fn3 = path;
  fn3 += _R("L_Foot_mot.txt");

  std::ofstream outF1, outF2, outF3;
  outF1.open(fn1.GetCStr());
  outF2.open(fn2.GetCStr());
  outF3.open(fn3.GetCStr());


  for (int index = 0; index < numframes; index++)
  {
    char numbs[1000];
    double t = timeStamps[index];
    outF1 << "Time";
    outF2 << "Time";
    outF3 << "Time";
    sprintf(numbs, " %16lf\n", t);
    outF1 << numbs;
    outF2 << numbs;
    outF3 << numbs;

    for(int j = 0; j < 4; j++)
    {
      double invec[4];
      cloud->GetLandmark(pelv_ind[j], invec, t);
      invec[3] = 1.0;
      outF1 << pelv_names[j].GetCStr();
      sprintf(numbs, " %16lf %16lf %16lf \n", invec[0], invec[1], invec[2]);
      outF1 << numbs;

      if(j == 3)
        break;

      cloud->GetLandmark(rleg_ind[j], invec, t);
      invec[3] = 1.0;
      outF2 << rleg_names[2 * j + 0].GetCStr();
      sprintf(numbs, " %16lf %16lf %16lf \n", 0.0, 0.0, 0.0);
      outF2 << numbs;
      outF2 << rleg_names[2 * j + 1].GetCStr();
      sprintf(numbs, " %16lf %16lf %16lf \n", invec[0], invec[1], invec[2]);
      outF2 << numbs;

      cloud->GetLandmark(lleg_ind[j], invec, t);
      invec[3] = 1.0;
      outF3 << lleg_names[2 * j + 0].GetCStr();
      sprintf(numbs, " %16lf %16lf %16lf \n", 0.0, 0.0, 0.0);
      outF3 << numbs;
      outF3 << lleg_names[2 * j + 1].GetCStr();
      sprintf(numbs, " %16lf %16lf %16lf \n", invec[0], invec[1], invec[2]);
      outF3 << numbs;
    }
  }

  outF1.close();
  outF2.close();
  outF3.close();


  mafString commandline = m_ExtAppPath;
  wxSetWorkingDirectory(path.toWx());
  commandline += _R(" TR72_3FN.DAT rtk__out.dat");
  if(wxExecute(commandline.toWx(), wxEXEC_SYNC) != 0)
    return false;

  mafString files[] = {_R("L_Foot.txt"),
                       _R("L_Pate.txt"),
                       _R("L_Shan.txt"),
                       _R("L_Thg1.txt"),
                       _R("L_Thg2.txt"),
                       _R("L_Thg3.txt"),
                       _R("R_Pate.txt"),
                       _R("R_Foot.txt"),
                       _R("R_Shan.txt"),
                       _R("R_Thg1.txt"),
                       _R("R_Thg2.txt"),
                       _R("R_Thg3.txt"),
                       _R("Pelvis.txt")};
  mafVMEGroup *grp = NULL;
  for(int i = 0; i < DIM(files); i++)
  {
    mafString fpath;
    fpath = path;
    fpath += files[i];
    if(!mafFileExists(fpath))
      continue;
    medOpImporterLandmarkAcc *imp = new medOpImporterLandmarkAcc();
    imp->SetFileName(fpath.GetCStr());
    imp->Read();
    std::vector<mafVME*>& res = imp->GetResults();
    if(grp == NULL && !res.empty())
    {
      mafNEW(grp);
      grp->SetName(_R("KinectModel"));
    }
    for(auto it = res.begin(); it != res.end(); ++it)
    {
      (*it)->ReparentTo(grp);
    }
    delete imp;
  }
  if(grp)
    m_Imported.push_back(grp);
  return true;
}

//----------------------------------------------------------------------------
void lhpOpKinectModel::Clear()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Imported.size(); i++)
  {
    mafDEL(m_Imported[i]);
  }
  m_Imported.clear();
}
