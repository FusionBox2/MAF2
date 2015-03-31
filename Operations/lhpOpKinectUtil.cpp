/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpKinectUtil.cpp,v $
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

#include "lhpOpKinectUtil.h"

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
#include "lhpOpKinectAFs.h"

#include "mafSmartPointer.h"
#include "mafMatrixVector.h"
#include "mafDataVector.h"
#include "mafVME.h"
#include "mafVMEGroup.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "medOpImporterLandmark.h"
#include <fstream>
#include <sstream>
#include <stack>
#include <vcl_fstream.h>
#include <vcl_string.h>

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
  ID_EXTAPPPATHMODEL,
  ID_LOAD_DICT,
  ID_CLEAR_DICT,
  ID_FREQ,
  ID_AFS,
  ID_TYPEOFREFS,
  ID_MODEL,
  ID_SCALE,
  ID_SCALE1,
  ID_SCALE2,
  ID_SCALE3,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

namespace 
{
  bool parsefloat(std::istream& istr, double& val)
  {
    /*std::stack<int> symbs;

    while(!istr.eof())
    {
      int c = istr.get();
      if(c == ',')
        c = '.';
      symbs.push(c);
      if(c == '.')
        break;
    }

    while(!symbs.empty())
    {
      if(!istr.putback(symbs.top()).good())
      {
        assert(false);
      }
      symbs.pop();
    }*/

    istr >> val;
    return !istr.fail();
  }
}


class medOpImporterLandmarkAccU : public medOpImporterLandmark
{
public:
  mafTypeMacro(medOpImporterLandmarkAccU, medOpImporterLandmark);
  medOpImporterLandmarkAccU(const mafString& label = "") : medOpImporterLandmark(label){}
  std::vector<mafVME*>& GetResults(){return m_Results;}
};

mafCxxTypeMacro(medOpImporterLandmarkAccU)


mafCxxTypeMacro(lhpOpKinectUtil)
//----------------------------------------------------------------------------
lhpOpKinectUtil::lhpOpKinectUtil(bool extapp, const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_IMPORTER;
  m_Canundo   = false;
  m_FileDir = mafGetApplicationDirectory() + "/Data/External/";
  m_DictionaryFileName = "";
  m_ExtAppPath         = "SkeletalViewer.exe";
  m_ExtAppPathModel    = "LLOptim.exe";
  m_Scale  = 1.0;
  m_Freq   = 30.0;
  m_AFs    = false;
  m_Model  = true;
  m_Scale1 = 1.0;
  m_Scale2 = 1.0;
  m_Scale3 = 1.0;
  m_ExtApp = extapp;
  m_TypeOfRefs = 0;
  DictionaryUpdate();
}

//----------------------------------------------------------------------------
lhpOpKinectUtil::~lhpOpKinectUtil()
//----------------------------------------------------------------------------
{
  Clear();
}

//----------------------------------------------------------------------------
mafOp* lhpOpKinectUtil::Copy()
//----------------------------------------------------------------------------
{
  lhpOpKinectUtil *op = new lhpOpKinectUtil(m_ExtApp, GetLabel());
  op->m_Canundo = m_Canundo;
  op->m_OpType = m_OpType;
  op->SetListener(GetListener());
  return op;
}

//----------------------------------------------------------------------------
bool lhpOpKinectUtil::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;
  return true;
}


//----------------------------------------------------------------------------
void lhpOpKinectUtil::OpRun()   
//----------------------------------------------------------------------------
{
  if(!m_ExtApp)
  {
    mafString wildcard = "Kinect data files (*.txt)|*.txt";
    m_C3DInputFileNameFullPaths.clear();
    mafGetOpenMultiFiles(m_FileDir,wildcard, m_C3DInputFileNameFullPaths);
    if(m_C3DInputFileNameFullPaths.size() == 0) 
    {
      mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
      return;
    }
  }
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
void lhpOpKinectUtil::OnEvent(mafEventBase *maf_event) 
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
    case ID_EXTAPPPATHMODEL:
      break;
    case ID_FREQ:
      break;
    case ID_AFS:
      m_Gui->Enable(ID_TYPEOFREFS, (m_AFs != 0));
      break;
    case ID_MODEL:
      m_Gui->Enable(ID_EXTAPPPATHMODEL, (m_Model != 0));
      m_Gui->Enable(ID_SCALE1, (m_Model != 0));
      m_Gui->Enable(ID_SCALE2, (m_Model != 0));
      m_Gui->Enable(ID_SCALE3, (m_Model != 0));
      break;
    case ID_TYPEOFREFS:
      break;
    case ID_SCALE:
    case ID_SCALE1:
    case ID_SCALE2:
    case ID_SCALE3:
      break;
    case ID_CLEAR_DICT:
      {
        m_DictionaryFileName = "";
      }//WARNING! NO break operator here, execution will continue in ID_LOAD_DICT
    case ID_LOAD_DICT:
      {
        DictionaryUpdate();
        break;
      }
    default:
    {
      mafEventMacro(*maf_event); 
    }
    break;
  }
}

//----------------------------------------------------------------------------
void lhpOpKinectUtil::OpDo()
//----------------------------------------------------------------------------
{
  wxBusyInfo wait("Please wait, create all VMEs in tree");

  for(unsigned i = 0; i < m_Imported.size(); i++)
  {
    m_Imported[i]->ReparentTo(m_Input);
  }
}

//----------------------------------------------------------------------------
void lhpOpKinectUtil::OpUndo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Imported.size(); i++)
  {
    m_Imported[i]->ReparentTo(NULL);
  }
}
//----------------------------------------------------------------------------
void lhpOpKinectUtil::CreateGui()
//----------------------------------------------------------------------------
{
  mafString refs_names[] = {"Flexion", "Abduction"};
  m_Gui = new mafGUI(this);
  if(m_ExtApp)
    m_Gui->FileOpen(ID_EXTAPPPATH, "Ext app", &m_ExtAppPath, "*.exe");
  m_Gui->Double(ID_SCALE, _("Scale"), &m_Scale, 0.0);
  m_Gui->Double(ID_FREQ, _("Freq"), &m_Freq, 0.000001);
  m_Gui->Bool(ID_AFS, _("AFs"), &m_AFs);
  m_Gui->Combo(ID_TYPEOFREFS, "Type",&m_TypeOfRefs, 2, refs_names);
  m_Gui->Label("");
  m_Gui->Bool(ID_MODEL, _("Model"), &m_Model);
  m_Gui->FileOpen(ID_EXTAPPPATHMODEL, "Mod app", &m_ExtAppPathModel, "*.exe");
  m_Gui->Double(ID_SCALE1, _("Scale1"), &m_Scale1, 0.0);
  m_Gui->Double(ID_SCALE2, _("Scale2"), &m_Scale2, 0.0);
  m_Gui->Double(ID_SCALE3, _("Scale3"), &m_Scale3, 0.0);
  m_Gui->FileOpen(ID_LOAD_DICT, "LM list",  &m_DictionaryFileName, "*.txt");
  m_Gui->Button(ID_CLEAR_DICT, "Clean", "", "Press to cancel using list" );  
  m_Gui->Enable(ID_CLEAR_DICT, (m_DictionaryFileName != ""));
  m_Gui->Enable(ID_TYPEOFREFS, (m_AFs != 0));
  m_Gui->Enable(ID_EXTAPPPATHMODEL, (m_Model != 0));
  m_Gui->Enable(ID_SCALE1, (m_Model != 0));
  m_Gui->Enable(ID_SCALE2, (m_Model != 0));
  m_Gui->Enable(ID_SCALE3, (m_Model != 0));

  m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
bool lhpOpKinectUtil::Import()
//----------------------------------------------------------------------------
{
  bool result = false;
  Clear();

  if(m_ExtApp)
  {
    if(m_ExtAppPath.IsEmpty())
      return false;
    mafString filetoprd = mafCreateTempFileName("");
    mafString commandline = m_ExtAppPath;
    commandline += " -logging" + filetoprd;
    if(wxExecute(commandline.GetCStr(), wxEXEC_SYNC) != 0)
      return false;
    filetoprd += "\\Files.txt";
    if(!mafFileExists(filetoprd))
      return false;
    m_C3DInputFileNameFullPaths.clear();

    if(FILE* fp = fopen(filetoprd, "rt"))
    {
      char string[4096];
      while(!feof(fp))
      {
        if(!fgets(string, 4096, fp))
          break;
        mafString fName;
        fName = mafString(string);
        m_C3DInputFileNameFullPaths.push_back(fName);
      }
      fclose(fp);
    }
  }

  for(unsigned fileIndex = 0; fileIndex < m_C3DInputFileNameFullPaths.size(); fileIndex++)
  {
    mafVME *imported = ImportSingleFile(m_C3DInputFileNameFullPaths[fileIndex]);
    if(imported != NULL)
    {
      if(m_ExtApp)
      {
        mafString path, name, ext;
        mafSplitPath(m_C3DInputFileNameFullPaths[fileIndex], &path, &name, &ext);
        imported->SetName(name);
      }
      result = true;
      m_Imported.push_back(imported);
      if(m_AFs)
      {
        lhpOpKinectAFs opafs;
        opafs.TestModeOn();
        opafs.SetInput(imported);
        opafs.SetTypeOfRefs(m_TypeOfRefs);
        opafs.OpRun();
        opafs.OpDo();
      }
    }
  }
  if(m_ExtApp && !m_C3DInputFileNameFullPaths.empty())
    mafFileRemove(m_C3DInputFileNameFullPaths[0]);
  return result;
}

//----------------------------------------------------------------------------
mafVME *lhpOpKinectUtil::ImportSingleFile(const mafString &fullFileName)
//----------------------------------------------------------------------------
{
  std::vector<std::vector<double> > rmatrix;
  {
    std::filebuf *pbuf;
    std::ifstream filestr;
    long size;
    char * buffer;
    filestr.open (fullFileName.GetCStr());

    pbuf=filestr.rdbuf();
    size=pbuf->pubseekoff (0,std::ios::end,std::ios::in);
    pbuf->pubseekpos (0,std::ios::in);
    buffer=new char[size];
    pbuf->sgetn (buffer,size);
    filestr.close();


    for(long i = 0; i < size; i++)
    {
      if(buffer[i] == ',')
        buffer[i] = '.';
    }

    //std::istringstream mstream(fullFileName.GetCStr(), std::ios::in);
    std::istringstream mstream(buffer);
    /*if(!mstream.is_open())
      return NULL;*/
    std::vector<double> row_vals;
    for(;;)
    {
      while (true)
      {
        int c = mstream.get();
        if (c == EOF)
          goto loademup;
        if (!isspace(c))
        {
          mstream.putback(c);
          goto readfloat;
        }
        // First newline after first number tells us the column dimension
        if (c == '\n' && row_vals.size() > 0) {
          goto loademup;
        }
      }
    readfloat:
      double val;
      if(parsefloat(mstream, val))
        row_vals.push_back(val);
      if(mstream.eof())
        goto loademup;
    }
  loademup:
    unsigned int colz = row_vals.size();
    if(colz == 0)
      return NULL;
    rmatrix.push_back(row_vals);

    while(true)
    {
      if(!parsefloat(mstream, row_vals[0]))
        break;
      for (unsigned int k = 1; k < colz; ++k) 
      {
        if (mstream.eof())
          return NULL;
        if(!parsefloat(mstream, row_vals[k]))
          return NULL;
      }
      rmatrix.push_back(row_vals);
    }
    delete[] buffer;

  }


  mafVMELandmarkCloud *cloud;
  mafNEW(cloud);
  std::vector<mafTimeStamp> timeStamps;

  cloud->SetName(mafFileNameFromPath(fullFileName));
  cloud->SetRadius(15);

  int current_lm = 0;
  int initValue = (rmatrix[0].size() % 3);
  timeStamps.resize(rmatrix.size());
  for (int i = 0; i < rmatrix.size(); i++)
  {
    if(initValue == 0)
      timeStamps[i] = i / m_Freq;
    else if(initValue == 1)
      timeStamps[i] = rmatrix[i][0];
    else //if(initValue == 2)
      timeStamps[i] = rmatrix[i][1];
  }

  for (int j = initValue; j < rmatrix[0].size(); j += 3)
  {
    mafString lm_name;
    if(current_lm < m_dictionaryStruct.size())
      lm_name = m_dictionaryStruct[current_lm];
    else
    {
      lm_name ="lm_";
      lm_name << current_lm;
    }

    cloud->AppendLandmark(lm_name);
    current_lm++;

    for (int i = 0; i < rmatrix.size(); i++)
    { 
      cloud->SetLandmark(lm_name,
        m_Scale * rmatrix[i][j],
        m_Scale * rmatrix[i][j + 1],
        m_Scale * rmatrix[i][j + 2], 
        timeStamps[i]);
    }	

  }						
  if(!cloud || !m_Model || m_ExtAppPathModel.IsEmpty())
    return cloud;

  int    numframes = timeStamps.size();

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

  for(int i = 0; i < 4; i++)
  {
    if(pelv_ind[i] == -1)
      return cloud;
    if(i == 3)
      break;
    if(lleg_ind[i] == -1 || rleg_ind[i] == - 1)
      return cloud;
  }

  mafString path, nameext;
  mafSplitPath(m_ExtAppPathModel, &path, &nameext);
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

  mafString commandline = m_ExtAppPathModel;
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
    medOpImporterLandmarkAccU *imp = new medOpImporterLandmarkAccU();
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
  {
    grp->ReparentTo(cloud);
    mafDEL(grp);
  }
  return cloud;
}

//----------------------------------------------------------------------------
void lhpOpKinectUtil::Clear()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Imported.size(); i++)
  {
    mafDEL(m_Imported[i]);
  }
  m_Imported.clear();
}
void lhpOpKinectUtil::DictionaryUpdate()
{
  bool emptyName = (m_DictionaryFileName == "");
  DestroyDictionary();
  if(!emptyName)
  {
    if(!LoadDictionary())
    {
      wxLogMessage("Error reading dictionary.");
      m_DictionaryFileName = "";
    }
  }
  else
  {
    m_dictionaryStruct.push_back("Pelvis");
    m_dictionaryStruct.push_back("Spine");
    m_dictionaryStruct.push_back("Thorax");
    m_dictionaryStruct.push_back("Head");
    m_dictionaryStruct.push_back("LeftShoulder");
    m_dictionaryStruct.push_back("LeftElbow");
    m_dictionaryStruct.push_back("LeftWrist");
    m_dictionaryStruct.push_back("LeftHand");
    m_dictionaryStruct.push_back("RightShoulder");
    m_dictionaryStruct.push_back("RightElbow");
    m_dictionaryStruct.push_back("RightWrist");
    m_dictionaryStruct.push_back("RightHand");
    m_dictionaryStruct.push_back("LeftHip");
    m_dictionaryStruct.push_back("LeftKnee");
    m_dictionaryStruct.push_back("LeftAnkle");
    m_dictionaryStruct.push_back("LeftFoot");
    m_dictionaryStruct.push_back("RightHip");
    m_dictionaryStruct.push_back("RightKnee");
    m_dictionaryStruct.push_back("RightAnkle");
    m_dictionaryStruct.push_back("RightFoot");
  }
  if(m_Gui)
  {
    m_Gui->Enable(ID_CLEAR_DICT, !emptyName);
    m_Gui->Update();
  }
}
bool lhpOpKinectUtil::LoadDictionary()
{
  vcl_string landmarkName;
  vcl_ifstream dictionaryInputStream(m_DictionaryFileName, std::ios::in);

  if(dictionaryInputStream.is_open() == 0)
    return false;
  while(dictionaryInputStream >> landmarkName)	
  {
    m_dictionaryStruct.push_back(landmarkName.c_str());
  }
  return true;
}
void lhpOpKinectUtil::DestroyDictionary()
{
  m_dictionaryStruct.clear();
}
