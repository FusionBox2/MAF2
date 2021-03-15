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
#include "mafFilesDirs.h"
#include "mafPlotMath.h"

#include "mafOpExplodeCollapse.h"
#include "medOpImporterLandmark.h"
#include "lhpOpKinectAFs.h"

#include "mafSmartPointer.h"
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
  medOpImporterLandmarkAcc(const mafString& label = "") : medOpImporterLandmark(label){}
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
  m_FileDir    = mafGetApplicationDirectory() + "/Data/External/";
  m_ExtAppPath = mafGetApplicationDirectory() + "/SkeletalViewerBart.exe";
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
    if(cloud->GetLandmarkName(i) == "LASI" || cloud->GetLandmarkName(i) == "LeftHip")
      pelv_ind[0] = i;
    if(cloud->GetLandmarkName(i) == "RASI" || cloud->GetLandmarkName(i) == "RightHip")
      pelv_ind[1] = i;
    if(cloud->GetLandmarkName(i) == "RPSI" || cloud->GetLandmarkName(i) == "Pelvis")
      pelv_ind[2] = i;
    if(cloud->GetLandmarkName(i) == "LPSI" || cloud->GetLandmarkName(i) == "Pelvis")
      pelv_ind[3] = i;

    if(cloud->GetLandmarkName(i) == "RKNE" || cloud->GetLandmarkName(i) == "RightKnee")
      rleg_ind[0] = i;
    if(cloud->GetLandmarkName(i) == "RANK" || cloud->GetLandmarkName(i) == "RightAnkle")
      rleg_ind[1] = i;
    if(cloud->GetLandmarkName(i) == "RTOE" || cloud->GetLandmarkName(i) == "RightFoot")
      rleg_ind[2] = i;

    if(cloud->GetLandmarkName(i) == "LKNE" || cloud->GetLandmarkName(i) == "LeftKnee")
      lleg_ind[0] = i;
    if(cloud->GetLandmarkName(i) == "LANK" || cloud->GetLandmarkName(i) == "LeftAnkle")
      lleg_ind[1] = i;
    if(cloud->GetLandmarkName(i) == "LTOE" || cloud->GetLandmarkName(i) == "LeftFoot")
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
      mafEventMacro(mafEvent(this,OP_RUN_OK));
    }
    else
    {
      mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
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
      mafEventMacro(*maf_event); 
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
  m_Gui->FileOpen(ID_EXTAPPPATH, "Ext app", &m_ExtAppPath, "*.exe");
  m_Gui->Double(ID_SCALE, _("Scale1"), &m_Scale1, 0.0);
  m_Gui->Double(ID_SCALE, _("Scale2"), &m_Scale2, 0.0);
  m_Gui->Double(ID_SCALE, _("Scale3"), &m_Scale3, 0.0);

  m_Gui->OkCancel();
}

//----------------------------------------------------------------------------
bool lhpOpKinectModel::Import()
//----------------------------------------------------------------------------
{
  bool result = false;
  Clear();

  if(m_ExtAppPath.IsEmpty())
    return false;

  std::vector<mafTimeStamp> timeStamps;
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);
  cloud->GetTimeStamps(timeStamps);
  int    numframes = timeStamps.size();

  mafString path, nameext;
  mafSplitPath(m_ExtAppPath, &path, &nameext);
  size_t length = path.Length();
  if(length != 0 && path[length - 1] != '/' && path[length - 1] != '\\')
    path += "/";

  mafString params;
  params = path;
  params += "Inp_FuBx.dat";
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

  mafString pelv_names[] = {"LASI", "RASI", "RPSI", "LPSI"};
  mafString rleg_names[] = {"RTHI", "RKNE", "RTIB", "RANK", "RHEE", "RTOE"};
  mafString lleg_names[] = {"LTHI", "LKNE", "LTIB", "LANK", "LHEE", "LTOE"};
  int pelv_ind[4] = {-1, -1, -1, -1};
  int rleg_ind[3] = {-1, -1, -1};
  int lleg_ind[3] = {-1, -1, -1};

  for(int i = 0; i < cloud->GetNumberOfLandmarks(); i++)
  {
    if(cloud->GetLandmarkName(i) == "LASI" || cloud->GetLandmarkName(i) == "LeftHip")
      pelv_ind[0] = i;
    if(cloud->GetLandmarkName(i) == "RASI" || cloud->GetLandmarkName(i) == "RightHip")
      pelv_ind[1] = i;
    if(cloud->GetLandmarkName(i) == "RPSI" || cloud->GetLandmarkName(i) == "Pelvis")
      pelv_ind[2] = i;
    if(cloud->GetLandmarkName(i) == "LPSI" || cloud->GetLandmarkName(i) == "Pelvis")
      pelv_ind[3] = i;

    if(cloud->GetLandmarkName(i) == "RKNE" || cloud->GetLandmarkName(i) == "RightKnee")
      rleg_ind[0] = i;
    if(cloud->GetLandmarkName(i) == "RANK" || cloud->GetLandmarkName(i) == "RightAnkle")
      rleg_ind[1] = i;
    if(cloud->GetLandmarkName(i) == "RTOE" || cloud->GetLandmarkName(i) == "RightFoot")
      rleg_ind[2] = i;

    if(cloud->GetLandmarkName(i) == "LKNE" || cloud->GetLandmarkName(i) == "LeftKnee")
      lleg_ind[0] = i;
    if(cloud->GetLandmarkName(i) == "LANK" || cloud->GetLandmarkName(i) == "LeftAnkle")
      lleg_ind[1] = i;
    if(cloud->GetLandmarkName(i) == "LTOE" || cloud->GetLandmarkName(i) == "LeftFoot")
      lleg_ind[2] = i;
  }

  mafString fn1;
  mafString fn2;
  mafString fn3;

  fn1 = path;
  fn1 += "Pelvis_mot.txt";
  fn2 = path;
  fn2 += "R_Foot_mot.txt";
  fn3 = path;
  fn3 += "L_Foot_mot.txt";

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
      outF1 << pelv_names[j];
      sprintf(numbs, " %16lf %16lf %16lf \n", invec[0], invec[1], invec[2]);
      outF1 << numbs;

      if(j == 3)
        break;

      cloud->GetLandmark(rleg_ind[j], invec, t);
      invec[3] = 1.0;
      outF2 << rleg_names[2 * j + 0];
      sprintf(numbs, " %16lf %16lf %16lf \n", 0.0, 0.0, 0.0);
      outF2 << numbs;
      outF2 << rleg_names[2 * j + 1];
      sprintf(numbs, " %16lf %16lf %16lf \n", invec[0], invec[1], invec[2]);
      outF2 << numbs;

      cloud->GetLandmark(lleg_ind[j], invec, t);
      invec[3] = 1.0;
      outF3 << lleg_names[2 * j + 0];
      sprintf(numbs, " %16lf %16lf %16lf \n", 0.0, 0.0, 0.0);
      outF3 << numbs;
      outF3 << lleg_names[2 * j + 1];
      sprintf(numbs, " %16lf %16lf %16lf \n", invec[0], invec[1], invec[2]);
      outF3 << numbs;
    }
  }

  outF1.close();
  outF2.close();
  outF3.close();


  mafString commandline = m_ExtAppPath;
  wxSetWorkingDirectory(path.GetCStr());
  commandline += " TR72_3FN.DAT rtk__out.dat";
  if(wxExecute(commandline.GetCStr(), wxEXEC_SYNC) != 0)
    return false;

  mafString files[] = {"L_Foot.txt",
                       "L_Pate.txt",
                       "L_Shan.txt",
                       "L_Thg1.txt",
                       "L_Thg2.txt",
                       "L_Thg3.txt",
                       "R_Pate.txt",
                       "R_Foot.txt",
                       "R_Shan.txt",
                       "R_Thg1.txt",
                       "R_Thg2.txt",
                       "R_Thg3.txt",
                       "Pelvis.txt"};
  mafVMEGroup *grp = NULL;
  for(int i = 0; i < DIM(files); i++)
  {
    mafString fpath;
    fpath = path;
    fpath += files[i];
    if(!mafFileExists(fpath))
      continue;
    medOpImporterLandmarkAcc *imp = new medOpImporterLandmarkAcc();
    imp->SetFileName(fpath);
    imp->Read();
    std::vector<mafVME*>& res = imp->GetResults();
    if(grp == NULL && !res.empty())
    {
      mafNEW(grp);
      grp->SetName("KinectModel");
    }
    for(std::vector<mafVME*>::iterator it = res.begin(); it != res.end(); ++it)
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
