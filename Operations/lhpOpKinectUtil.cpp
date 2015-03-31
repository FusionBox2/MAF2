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

#include "resource.h"

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
  ID_LOAD_DICT,
  ID_CLEAR_DICT,
  ID_FREQ,
  ID_AFS,
  ID_TYPEOFREFS,
  ID_FSUFFIX,
  ID_MODEL,
  ID_SCALE,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

class medOpImporterLandmarkAccU : public medOpImporterLandmark
{
public:
  mafTypeMacro(medOpImporterLandmarkAccU, medOpImporterLandmark);
  medOpImporterLandmarkAccU(const mafString& label = "") : medOpImporterLandmark(label){}
  std::vector<mafVME*>& GetResults(){return m_Results;}
};

mafCxxTypeMacro(medOpImporterLandmarkAccU)



namespace 
{
  void extractTool(const mafString& tmpPath)
  {
    const char *resNames[]=    {"IDR_EXE2", "LL_Model_S035_K",                    
      "LL_Model_S035_V",                     
      "TR72_3FN_MATL_LL_K",                  
      "TR72_3FN_MATL_LL_V",                  
      "TR72_3FN_MATL_UL_K",                  
      "TR72_3FN_MATL_UL_V",                  
      "UpL_Model_S035_K",                    
      "UpL_Model_S035_V",                    

      "KV_Local_Param_Inp_Shv_K",            
      "KV_Local_Param_Inp_Shv_V",            
      "All_Regr_p_coeffs_2_ALL_Kin_R_Abd_N", 
      "All_Regr_p_coeffs_2_ALL_Kin_R_Flx_N"};

    const char *names[]=
    {"KinVic_Opt_ShV_2012.exe","LL_Model_S035_K.dat",
    "LL_Model_S035_V.dat",
    "TR72_3FN_MATL_LL_K.DAT",
    "TR72_3FN_MATL_LL_V.DAT",
    "TR72_3FN_MATL_UL_K.DAT",
    "TR72_3FN_MATL_UL_V.DAT",
    "UpL_Model_S035_K.dat",
    "UpL_Model_S035_V.dat",

    "KV_Local_Param_Inp_Shv_K.m",
    "KV_Local_Param_Inp_Shv_V.m",
    "All_Regr_p_coeffs_2_ALL_Kin_R_Abd_N.txt",
    "All_Regr_p_coeffs_2_ALL_Kin_R_Flx_N.txt"};



    HINSTANCE hInstance = wxGetInstance();

    for(int i = 0; i < 13; i++)
    {
      mafString apppath;
      apppath = tmpPath;
      apppath += "\\";
      apppath += names[i];
      HRSRC hrSrc = FindResource(hInstance, resNames[i], i==0?"EXE":"DAT");
      HGLOBAL exef = LoadResource(hInstance, hrSrc);
      size_t sz = SizeofResource(hInstance, hrSrc);
      void *buf = LockResource(exef);
      FILE *exeF = fopen(apppath.GetCStr(), "wb");
      fwrite(buf, 1, sz, exeF);
      fclose(exeF);
    }
  }

  void writeParams(const mafString& tmpPath, const mafString& inputPath, const mafString& inputFile, const mafString& staticFile, const mafString& outputPath)
  {
    mafString mtlbopt;
    mtlbopt = tmpPath;

    mafString fullName;
    fullName = mtlbopt;
    fullName += "\\KV_Local_Param_Inp_Shv_K.m";

    mafString kinectDirName;
    kinectDirName = inputPath;

    mafString outKinectDirName;
    outKinectDirName = outputPath;

    std::ofstream strm;
    strm.open(fullName, std::ios::out);
    strm << "%%% function KV_Local_Param_Inp_Shv" << endl;
    strm << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%%  Disp_Debug_Yes = 0; %%% 0 1" << endl;
    strm << "%%%  " << endl;
    strm << "Disp_Debug_Yes = 1; %%% 0 1" << endl;
    strm << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "KV_Input_Kinect = 1; %%% 1-Kinect; 0-Vicon" << endl;
    strm << "%%% KV_Input_Kinect = 0; %%% 1-Kinect; 0-Vicon" << endl;
    strm << endl;
    strm << "%%% OLD  Vicon_P1_In_YES = 1; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)" << endl;
    strm << "%%% OLD  Vicon_P1_In_YES = 0; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)" << endl;
    strm << endl;
    strm << "%%% " << endl;
    strm << "Vicon_P1_In_YES =  [1 1 1 1 1]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P1_all: R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [1 1 1 1 1]-MAIN" << endl;
    strm << "%%% Vicon_P1_In_YES =  [0 0 0 0 0]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P0_all: R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [0 0 0 0 0]-All OPT" << endl;
    strm << "%%% Vicon_P1_In_YES =  [1 1 1 0 0]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P1_R_L_Legs(1 2) P1_Trx(3) P0_R_L_Hands(4 5) [1 1 1 0 0]-test" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "Reduced_Output_Kinect_Yes = 0; %%% 0-MAIN 1-analysis 'Opt_5_Bds'(see below)" << endl;
    strm << "%%% Reduced_Output_Kinect_Yes = 1; %%% 0-MAIN 1-analysis 'Opt_5_Bds'(see below)" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "Dir_Name_Data = '"; 
    
    for(int i = 0; i < kinectDirName.Length(); i++)
    {
      if(kinectDirName[i] == '/')
        strm << '\\';
      else
       strm << kinectDirName[i];
    }
    
    strm << "\\';" << endl;
    strm << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "Dir_Name_Results = '";
    
    for(int i = 0; i < outKinectDirName.Length(); i++)
    {
      if(outKinectDirName[i] == '/')
        strm << '\\';
      else 
        strm << outKinectDirName[i]; 
    }
    
    strm << "'; %%% 'test'  'test_06'  'test_07'" << endl;
    strm << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "Static_Pose_Yes = 1; %%% 0 1-Scale from static pose" << endl;
    strm << "FN_Kin_Data_Inp_Static = '" << staticFile.GetCStr() << "'; %%% Serge Squat 21may2012 SVSJ" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "FN_Kin_Data_Inp = '" << inputFile.GetCStr() << "'; %%% Serge Squat 21may2012 SVSJ" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "Save_Model_Yes = 0; %%% 0-MAIN 1" << endl;
    strm << "%%% Save_Model_Yes = 1; %%% 0-MAIN 1" << endl;
    strm << "%%% " << endl;
    strm << "Load_Model_Yes = 1; %%% 0-Requred ASCII:[TR72_3FN_MATL_LL.DAT, TR72_3FN_MATL_UL.DAT];  1-MAIN" << endl;
    strm << "%%% Load_Model_Yes = 0; %%% 0-Requred ASCII:[TR72_3FN_MATL_LL.DAT, TR72_3FN_MATL_UL.DAT];  1-MAIN" << endl;
    strm << "if Save_Model_Yes, Load_Model_Yes = 0; end" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "Opt_5_Bds = [1 1 1 1 1]; %%% R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [1 1 1 1 1]-MAIN" << endl;
    strm << "%%% Opt_5_Bds = [0 0 0 0 1]; %%% L_Hand(5) [0 0 0 0 1]-Test " << endl;
    strm << "%%% Opt_5_Bds = [1 0 0 0 1]; %%% R_Leg L_Hand(1 5) [1 0 0 0 1]-Test " << endl;
    strm << "%%% Opt_5_Bds = [1 0 0 0 0]; %%% R_Leg (1 ) [1 0 0 0 0]-Test " << endl;
    strm << "%%% Opt_5_Bds = [0 1 0 0 0]; %%% L_Leg (2 ) [0 1 0 0 0]-Test " << endl;
    strm << "%%% Opt_5_Bds = [1 1 0 0 0]; %%% RL_Leg (1 2) [1 1 0 0 0]-Test " << endl;
    strm << "%%% Opt_5_Bds = [0 0 0 1 1]; %%% R_L_Hands(4 5) [0 0 0 1 1]-Test " << endl;
    strm << "%%% Opt_5_Bds = [0 0 1 1 1]; %%% Trx(3) R_L_Hands(4 5) [0 0 1 1 1]-Test " << endl;
    strm << "%%% Opt_5_Bds = [1 1 0 0 0]; %%% R_L_Legs(1 2) [1 1 0 0 0]-Test " << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "H_time_Refram = 0.1; %%% Normal Speed, e.g. SVSJ Squat_S02 21may2012" << endl;
    strm << "%%% H_time_Refram = 0.04; %%% Fast Speed, e.g. SVSJ Jump_S10 21may2012" << endl;
    strm << "%%% H_time_Refram = 0.01; %%% VERY Fast Speed, THEN(!!!) capturing frec(e.g.33fps) will be USED" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "Equal_Frec_25_Yes = 1; %%% 0 1-MAIN; for Output reframing" << endl;
    strm << "%%% if Equal_Frec_25_Yes == 1, Frec_New = 100; h_Time = 1/Frec_New; end %%% 100 FPS" << endl;
    strm << "%%% if Equal_Frec_25_Yes == 1, Frec_New = 25; h_Time = 1/Frec_New; end %%% 25 FPS" << endl;
    strm << "%%% " << endl;
    strm << "if Equal_Frec_25_Yes == 1, Frec_New = 10; h_Time = 1/Frec_New; end %%% 100 FPS" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "Thorax_Root_Yes = 0; %%% 1 0-MAIN" << endl;
    strm << "%%% Thorax_Root_Yes = 1; %%% 1 0-MAIN" << endl;
    strm << "if sum(Opt_5_Bds(1:3),2) == 0, Thorax_Root_Yes = 1; end " << endl;
    strm << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "Kinect_In_YES = 1; %%% 0 1-Kinect Data" << endl;
    strm << "%%% Kinect_In_YES = 0; %%% 0-Vicon Data 1" << endl;
    strm << "%%% Kinect_In_YES = KV_Input_Kinect;" << endl;
    strm << "IFLag_AL_Fout = 1; %%% 0 1-MAIN" << endl;
    strm << "LL_OPT_YES = 1; %%% 0 1-MAIN" << endl;
    strm << "UpL_OPT_YES = 1; %%% 0 1-MAIN" << endl;
    strm << "DoFs_Smooth_Yes = 1; %%% 0 1-MAIN" << endl;
    strm << "DoFs_Corr_Yes = 1; %%% 0 1-MAIN" << endl;
    strm << "Should_LR_Corr_Yes = 1; %%% 0 1-MAIN" << endl;
    strm << "Surfl_Yes = 0; %%% 1-Trx_Ellipsoid_Fig1, 0-MAIN" << endl;
    strm << "%%% Par_Smth_30 = 30; %%% 10-MAIN smoothing param for OVP spline" << endl;
    strm << "%%% " << endl;
    strm << "Par_Smth_30 = 100; %%% 10-MAIN smoothing param for OVP spline" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "x0_Poly_Apprx_Yes = [1 1 1 1 1]; %%% 1 0" << endl;
    strm << "%%% N_Polyn = [3 3 3 3 3]; %%% [2-5] polyn" << endl;
    strm << "%%% " << endl;
    strm << "N_Polyn = [3 3 3 4 4]; %%% [2-5] polyn" << endl;
    strm << "%%% Mult_T5 = [0.5 0.5 0.5 0.0 0.0]; %%% [0.0-1.0]" << endl;
    strm << "%%% " << endl;
    strm << "Mult_T5 = [0.5 0.5 0.5 0.3 0.3]; %%% [0.0-1.0]" << endl;
    strm << "%%% Mult_T5 = [0.5 0.5 0.5 0.5 0.5]; %%% [0.0-1.0]" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "Bord_Delt = 0.0001;" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "%%% " << endl;
    strm << "Start_Fr_N = 1; " << endl;
    strm << "%%% Start_Fr_N = 263;" << endl;
    strm << "%%% Start_Fr_N = 201;" << endl;
    strm << "%%% Start_Fr_N = 301; %%% UP" << endl;
    strm << "%%% " << endl;
    strm << "Decim_Frm = 4;" << endl;
    strm << "%%% Vic_Pelv_4ALs = [0 1 1 1]; %%%  !!! LIAS LIPS  RIAS RIPS " << endl;
    strm << "%%% " << endl;
    strm << "Vic_Pelv_4ALs = [1 1 1 1]; %%% MAIN  !!! LIAS LIPS  RIAS RIPS " << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << endl;
    strm << "%%% end %%% %%% function KV_Local_Param_Inp_Shv" << endl;
    strm.close();
  }
  mafVMEGroup *ModelImport(const mafString& path)
  {
    mafString files[] = {
      "L_Foot.txt",
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
      "Pelvis.txt",
      "UpCerv.txt",
      "UpLCla.txt",
      "UpLHan.txt",
      "UpLHum.txt",
      "UpLRad.txt",
      "UpLSca.txt",
      "UpLUln.txt",
      "UpLumb.txt",
      "UpPelv.txt",
      "UpRCla.txt",
      "UpRHan.txt",
      "UpRHum.txt",
      "UpRRad.txt",
      "UpRSca.txt",
      "UpRUln.txt",
      "UpSkul.txt",
      "UpThor.txt",
    };

    mafVMEGroup *grp = NULL;
    for(int i = 0; i < DIM(files); i++)
    {
      mafString fpath;
      fpath = path;
      fpath += "\\";
      fpath += files[i];
      if(!mafFileExists(fpath))
        continue;
      medOpImporterLandmarkAccU *imp = new medOpImporterLandmarkAccU();
      imp->SetFileName(fpath);
      imp->Read();
      std::vector<mafVME*>& res = imp->GetResults();
      if(grp == NULL && !res.empty())
      {
        grp = mafVMEGroup::New();
        grp->Register(NULL);
        grp->SetName("KinectModel");
      }
      for(std::vector<mafVME*>::iterator it = res.begin(); it != res.end(); ++it)
      {
        (*it)->ReparentTo(grp);
      }
      delete imp;
    }
    return grp;
  }
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

mafCxxTypeMacro(lhpOpKinectUtil)
//----------------------------------------------------------------------------
lhpOpKinectUtil::lhpOpKinectUtil(bool extapp, const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_IMPORTER;
  m_Canundo   = false;
  m_FileDir = mafGetApplicationDirectory() + "/Data/External/";
  m_DictionaryFileName = "";
  m_Scale  = 1.0;
  m_Freq   = 30.0;
  m_AFs    = false;
  m_Model  = false;
  m_ExtApp = extapp;
  m_TypeOfRefs = 0;
  m_TakeScaled = 1;
  if(m_TakeScaled)
    m_FileSuffix = "_Scaled";
  else
    m_FileSuffix = "";
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
    case ID_FSUFFIX:
      if(m_TakeScaled)
        m_FileSuffix = "_Scaled";
      else
        m_FileSuffix = "";
      break;
    case ID_FREQ:
      break;
    case ID_AFS:
      m_Gui->Enable(ID_TYPEOFREFS, (m_AFs != 0));
      break;
    case ID_TYPEOFREFS:
      break;
    case ID_SCALE:
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
  {
    m_Gui->Bool(ID_FSUFFIX, "Scaled", &m_TakeScaled);
  }
  m_Gui->Double(ID_SCALE, _("Scale"), &m_Scale, 0.0);
  m_Gui->Double(ID_FREQ, _("Freq"), &m_Freq, 0.000001);
  m_Gui->Bool(ID_AFS, _("AFs"), &m_AFs);
  m_Gui->Combo(ID_TYPEOFREFS, "Type",&m_TypeOfRefs, 2, refs_names);
  m_Gui->Label("");
  m_Gui->Bool(ID_MODEL, _("Model"), &m_Model);
  m_Gui->FileOpen(ID_LOAD_DICT, "LM list",  &m_DictionaryFileName, "*.txt");
  m_Gui->Button(ID_CLEAR_DICT, "Clean", "", "Press to cancel using list" );  
  m_Gui->Enable(ID_CLEAR_DICT, (m_DictionaryFileName != ""));
  m_Gui->Enable(ID_TYPEOFREFS, (m_AFs != 0));

  m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
bool lhpOpKinectUtil::Import()
//----------------------------------------------------------------------------
{
  bool result = false;
  Clear();

  mafString filestxt, sessionstxt, filetoprd, apppath;
  std::vector<std::pair<mafString, mafVME*> > sessionsList;
  bool storageExists = false;
  if(m_ExtApp)
  {
    /*if(m_ExtAppPath.IsEmpty())
      return false;*/
    if(mafDirExists("C:\\KinectStorage"))
    {
      storageExists = true;
      filetoprd = "C:\\KinectStorage";
    }
    else
    {
      filetoprd = mafCreateTempFileName("");
    }

    apppath = filetoprd;
    if(!storageExists)
      mafDirMake(filetoprd);
    apppath += "\\mrkless.exe";

    HINSTANCE hInstance = wxGetInstance();
    HRSRC hrSrc = FindResource(hInstance, "IDR_EXE1", _T("EXE"));
    HGLOBAL exef = LoadResource(hInstance, hrSrc);
    size_t sz = SizeofResource(hInstance, hrSrc);
    void *buf = LockResource(exef);
    FILE *exeF = fopen(apppath.GetCStr(), "wb");
    fwrite(buf, 1, sz, exeF);
    fclose(exeF);

    mafString commandline = apppath;
    commandline += " -logging" + filetoprd;
    if(wxExecute(commandline.GetCStr(), wxEXEC_SYNC) != 0)
      return false;
    filestxt = filetoprd;
    filestxt += "\\Files.txt";
    sessionstxt = filetoprd;
    sessionstxt += "\\Sessions.txt";
    if(!mafFileExists(filestxt))
      return false;
    m_C3DInputFileNameFullPaths.clear();

    if(FILE* fp = fopen(filestxt, "rt"))
    {
      char string[4096];
      while(!feof(fp))
      {
        if(!fgets(string, 4096, fp))
          break;
        int len = strlen(string);
        for(int i =0; i < len; i++)
        {
          if(string[i] == '\n' || string[i] == '\r')
            string[i] = '\0';
          if(string[i] == '\\')
            string[i] = '/';
        }
        mafString fName;
        fName = mafString(string);
        m_C3DInputFileNameFullPaths.push_back(fName);
      }
      fclose(fp);
    }
    if(FILE* fp = fopen(sessionstxt, "rt"))
    {
      char string[4096];
      while(!feof(fp))
      {
        if(!fgets(string, 4096, fp))
          break;
        int len = strlen(string);
        for(int i =0; i < len; i++)
        {
          if(string[i] == '\n' || string[i] == '\r')
            string[i] = '\0';
          if(string[i] == '\\')
            string[i] = '/';
        }
        mafString fName;
        fName = mafString(string);
        sessionsList.push_back(std::make_pair(fName, (mafVME*)nullptr));
      }
      fclose(fp);
    }
  }

  mafString mtlbTmp, modelPath;
  if(m_Model)
  {
    mtlbTmp = mafCreateTempFileName("");
    mafDirMake(mtlbTmp);
    extractTool(mtlbTmp);
    modelPath = mtlbTmp;
    modelPath += "\\Model";
  }


  for(unsigned fileIndex = 0; fileIndex < m_C3DInputFileNameFullPaths.size(); fileIndex++)
  {
    mafString path, name, ext;
    mafString importName;
    mafSplitPath(m_C3DInputFileNameFullPaths[fileIndex], &path, &name, &ext);
    importName = m_C3DInputFileNameFullPaths[fileIndex];
    if(m_ExtApp)
    {
      importName = path;
      importName += "/";
      importName += name;
      importName += m_FileSuffix;
      importName += ".";
      importName += ext;
    }
    if(!mafFileExists(importName))
      continue;
    mafVME *imported = ImportSingleFile(importName);
    if(imported != NULL)
    {
      if(m_ExtApp)
      {
        //mafString pref, resname;
        //pref = mafString("");
        for(std::vector<std::pair<mafString, mafVME*> >::iterator it = sessionsList.begin(); it != sessionsList.end(); ++it)
        {
          if(it->first == path)
          {
            if(!it->second)
            {
              mafString spath, sname, sext;
              mafSplitPath(it->first, &spath, &sname, &sext);
              //pref = sname + "_";
              mafVMEGroup *grp;
              mafNEW(grp);
              it->second = grp;
              grp->SetName(sname);
              m_Imported.push_back(grp);
            }
            imported->ReparentTo(it->second);
            mafVME *tmp = imported;
            mafDEL(tmp);
            break;
          }
        }
        //resname  = pref;
        //resname += name;
        imported->SetName(name);
      }
      else
      {
        m_Imported.push_back(imported);
      }
      if(m_Model)
      {
        mafString nmext, staticName, fullSName, anthroFile;
        nmext = name;
        nmext += ".";
        nmext += ext;
        if(importName.FindFirst("_Scaled") != -1)
          staticName = "Static_Scaled.txt";
        else
          staticName = "Static.txt";
        fullSName = path;
        fullSName += "/";
        fullSName += staticName;

        anthroFile = path;
        anthroFile += "/";
        anthroFile += "AnthropometryKinect.m";
        if(mafFileExists(fullSName) && mafFileExists(anthroFile))
        {
          mafString newAnthro;
          newAnthro = mtlbTmp;
          newAnthro += "/";
          newAnthro += "AnthropometryKinect.m";
          mafFileCopy(anthroFile, newAnthro);
          mafDirMake(modelPath);
          writeParams(mtlbTmp, path, nmext, staticName, modelPath);


          mafString commandline;
          commandline = mtlbTmp;
          commandline += "/KinVic_Opt_ShV_2012.exe KV_Local_Param_Inp_Shv_K.m LL_Model_S035_K.dat UpL_Model_S035_K.dat";
          wxSetWorkingDirectory(mtlbTmp.GetCStr());
          if(wxExecute(commandline.GetCStr(), wxEXEC_SYNC) == 0)
          {
            mafVMEGroup *grp = ModelImport(modelPath);
            if(grp)
            {
              grp->ReparentTo(imported);
              mafDEL(grp);
            }
          }
          mafRemoveDirectory(modelPath);
        }
      }
      result = true;
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
  if(m_ExtApp)
  {
    if(!storageExists)
    {
      mafRemoveDirectory(filetoprd);
    }
    else
    {
      mafFileRemove(apppath);
      mafFileRemove(filestxt);
      mafFileRemove(sessionstxt);
    }
  }
  if(m_Model)
  {
    mafRemoveDirectory(mtlbTmp);
  }
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
  return cloud;
}
#ifdef TMP_EXPORT

void TmpExport()
{
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
    //fprintf(fp, "%f, %f, %f\n", m_Scale1, m_Scale2, m_Scale3);
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
}
#endif


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
