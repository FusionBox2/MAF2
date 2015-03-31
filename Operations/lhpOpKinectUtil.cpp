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


#include "splines.h"


#include <list>

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
#include "mafVMERoot.h"
#include "mafVMEItemVTK.h"
#include "lhpVMEKMInfo.h"
#include "medOpImporterLandmark.h"
#include <fstream>
#include <sstream>
#include <stack>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vnl\vnl_matrix.h>

#include "mafNodeIterator.h"
#include "mafVMEInfoText.h"


#include "mafVMEStorage.h"
#include "mafNodeManager.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "medVMEAnalog.h"


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
  mafVMEGroup *ImportMSFFile(mafString& m_File)
  {
    mafString unixname = m_File;
    mafString path, name, ext;
    mafString m_TmpDir;
    mafString m_MSFFile;
    mafSplitPath(m_File,&path,&name,&ext);

    if(ext == "zmsf")
    {
      unixname = mafOpenZIP(m_File, ::wxGetCwd(), m_TmpDir);
      if(unixname.IsEmpty())
      {
        return NULL;
      }
      wxSetWorkingDirectory(m_TmpDir.GetCStr());
    }

    unixname.ParsePathName(); // convert to unix format

    m_MSFFile = unixname; 
    mafVMEStorage *storage;
    mafNodeManager manager;
    storage = mafVMEStorage::New();
    storage->SetManager(&manager);
    storage->SetURL(m_MSFFile);

    int res = storage->Restore();
    if (res != MAF_OK)
    {
      // if some problems occurred during import give feedback to the user
      //if (!m_TestMode)
      //  mafErrorMessage(_("Errors during file parsing! Look the log area for error messages."));
      //return MAF_ERROR;
    }
    mafVMERoot *root = mafVMERoot::SafeDownCast(manager.GetRoot());

    mafString group_name = "Skeletal model";

    mafNodeIterator *iter = root->NewIterator();
    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if(node == root)
        continue;
      mafVMEGenericAbstract *vmeWithDataVector = mafVMEGenericAbstract::SafeDownCast(node);
      if (vmeWithDataVector)
      {
        mafDataVector *dataVector = vmeWithDataVector->GetDataVector();
        if(dataVector)
        {
          for(mafDataVector::Iterator it = dataVector->Begin(); it != dataVector->End(); ++it)
          {
            if(mafVMEItemVTK *vitem = mafVMEItemVTK::SafeDownCast(it->second))
              vitem->GetData();
          }
        }
      }
    }
    iter->Delete();


    mafVMEGroup *m_Group;

    m_Group = mafVMEGroup::New();
    m_Group->Register(NULL);
    m_Group->Initialize();
    m_Group->SetName(group_name);
    //m_Group->ReparentTo(m_Input);

    while (mafNode *node = root->GetFirstChild())
    {
      node->ReparentTo(m_Group);

      // Losi 03/16/2010 Bug #2049 fix
      /*mafVMEGeneric *vme = mafVMEGeneric::SafeDownCast(node);
      if(vme)
      {
        // Update data vector id to avoid duplicates
        mafDataVector *dataVector = vme->GetDataVector();
        if(dataVector)
        {
          dataVector->UpdateVectorId();
        }
      }*/
    }
    m_Group->Update();
    mafRemoveDirectory(m_TmpDir);
    cppDEL(storage);
    return m_Group;
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


  bool readMatrix(const mafString &fullFileName, std::vector<std::vector<double> >& rmatrix)
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
      return false;
    rmatrix.push_back(row_vals);

    while(true)
    {
      if(!parsefloat(mstream, row_vals[0]))
        break;
      for (unsigned int k = 1; k < colz; ++k) 
      {
        if (mstream.eof())
          return false;
        if(!parsefloat(mstream, row_vals[k]))
          return false;
      }
      rmatrix.push_back(row_vals);
    }
    delete[] buffer;

    return true;
  }




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

  void writeParams(const mafString& tmpPath, const mafString& inputPath, const mafString& inputFile, const mafString& staticFile, const mafString& outputPath, bool llimb, bool ulimb)
  {
    /*mafString mtlbopt;
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

    return;

    */


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
    strm << "%%% function KV_Local_Param_Inp_Shv" << endl; 
    strm << "" << endl; 
    strm << "%%%%% ===========================================" << endl; 
    strm << "%%%  Disp_Debug_Yes = 0; %%% 0 1" << endl; 
    strm << "%%%  " << endl; 
    strm << "Disp_Debug_Yes = 1; %%% 0 1" << endl; 
    strm << "" << endl; 
    strm << "%%%%% ===========================================" << endl; 
    strm << "%%% " << endl; 
    strm << "KV_Input_Kinect = 1; %%% 1-Kinect; 0-Vicon" << endl; 
    strm << "%%% KV_Input_Kinect = 0; %%% 1-Kinect; 0-Vicon" << endl; 
    strm << "" << endl; 
    strm << "%%% " << endl; 
    strm << "Anthropom_S035_Yes = 1; %%% 1-MAIN, 0- for Vicon !!! file load 'm50_Static2_SVSJ_may2012.bnr'" << endl; 
    strm << "%%% Anthropom_S035_Yes = 0; %%% 1-MAIN, 0- for Vicon !!! file load 'm50_Static2_SVSJ_may2012.bnr'" << endl; 
    strm << "" << endl; 
    strm << "%%% OLD  Vicon_P1_In_YES = 1; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)" << endl; 
    strm << "%%% OLD  Vicon_P1_In_YES = 0; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)" << endl; 
    strm << "" << endl; 
    strm << "%%% " << endl; 
    strm << "Vicon_P1_In_YES =  [1 1 1 1 1]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P1_all: R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [1 1 1 1 1]-MAIN" << endl; 
    strm << "%%% Vicon_P1_In_YES =  [0 0 0 0 0]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P0_all: R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [0 0 0 0 0]-All OPT" << endl; 
    strm << "%%% Vicon_P1_In_YES =  [1 1 1 0 0]; %%% 1-P1_protcl; 0-P0_protcl(OPTIMIZ)  %%% P1_R_L_Legs(1 2) P1_Trx(3) P0_R_L_Hands(4 5) [1 1 1 0 0]-test" << endl; 
    strm << "%%%%% ===========================================" << endl; 
    strm << "%%% " << endl; 
    strm << "Reduced_Output_Kinect_Yes = 0; %%% 0-MAIN 1-analysis 'Opt_5_Bds'(see below)" << endl; 
    strm << "%%% Reduced_Output_Kinect_Yes = 1; %%% 0-MAIN 1-analysis 'Opt_5_Bds'(see below)" << endl; 
    strm << "%%%%% ===========================================" << endl; 


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

    strm << "';%%% Dir_Name_Results = 'Output_Kinect_Processed'; %%% 'test'  'test_06'  'test_07'" << endl; 
      strm << "" << endl; 


      strm << "%%%%% ===========================================" << endl; 
      strm << "%%% Static_Pose_Yes = 1; %%% 0 1-Scale from static pose" << endl; 
      strm << "%%% " << endl; 
      strm << "Static_Pose_Yes = 0; %%% 0 1-Scale from static pose" << endl; 
      strm << "%%% FN_Kin_Data_Inp_Static = '" << staticFile.GetCStr() << "'; %%% Serge Squat 21may2012 SVSJ" << endl;
    strm << "%%%%% ===========================================" << endl;
    strm << "FN_Kin_Data_Inp = '" << inputFile.GetCStr() << "'; %%% Serge Squat 21may2012 SVSJ" << endl;
    strm << "%%%%% ===========================================" << endl; 
      strm << "%%% " << endl; 
      strm << "Save_Model_Yes = 0; %%% 0-MAIN 1" << endl; 
      strm << "%%% Save_Model_Yes = 1; %%% 0-MAIN 1" << endl; 
      strm << "%%% " << endl; 
      strm << "Load_Model_Yes = 1; %%% 0-Requred ASCII:[TR72_3FN_MATL_LL.DAT, TR72_3FN_MATL_UL.DAT];  1-MAIN" << endl; 
      strm << "%%% Load_Model_Yes = 0; %%% 0-Requred ASCII:[TR72_3FN_MATL_LL.DAT, TR72_3FN_MATL_UL.DAT];  1-MAIN" << endl; 
      strm << "if Save_Model_Yes, Load_Model_Yes = 0; end" << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "%%% " << endl;
    if(llimb && ulimb) 
      strm << "Opt_5_Bds = [1 1 1 1 1]; %%% R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [1 1 1 1 1]-MAIN" << endl; 
    else if(llimb)
    strm << "Opt_5_Bds = [1 1 1 0 0]; %%% R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [1 1 1 1 1]-MAIN" << endl;
    else if(ulimb)
      strm << "Opt_5_Bds = [0 0 1 1 1]; %%% R_L_Legs(1 2) Trx(3) R_L_Hands(4 5) [1 1 1 1 1]-MAIN" << endl;
    strm << "%%% Opt_5_Bds = [0 0 0 0 1]; %%% L_Hand(5) [0 0 0 0 1]-Test " << endl; 
      strm << "%%% Opt_5_Bds = [1 0 0 0 1]; %%% R_Leg L_Hand(1 5) [1 0 0 0 1]-Test " << endl; 
      strm << "%%% Opt_5_Bds = [1 0 0 0 0]; %%% R_Leg (1 ) [1 0 0 0 0]-Test " << endl; 
      strm << "%%% Opt_5_Bds = [0 1 0 0 0]; %%% L_Leg (2 ) [0 1 0 0 0]-Test " << endl; 
      strm << "%%% Opt_5_Bds = [1 1 0 0 0]; %%% RL_Leg (1 2) [1 1 0 0 0]-Test " << endl; 
      strm << "%%% Opt_5_Bds = [0 0 0 1 1]; %%% R_L_Hands(4 5) [0 0 0 1 1]-Test " << endl; 
      strm << "%%% Opt_5_Bds = [0 0 1 1 1]; %%% Trx(3) R_L_Hands(4 5) [0 0 1 1 1]-Test " << endl; 
      strm << "%%% Opt_5_Bds = [1 1 0 0 0]; %%% R_L_Legs(1 2) [1 1 0 0 0]-Test " << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "%%% H_time_Refram = 0.1; %%% Normal Speed, e.g. SVSJ Squat_S02 21may2012" << endl; 
      strm << "%%% H_time_Refram = 0.02; %%% Fast Speed, e.g. SVSJ Jump_S10 21may2012" << endl; 
      strm << "%%% " << endl; 
      strm << "H_time_Refram = 0.1; %%% Fast Speed, e.g. SVSJ Jump_S10 21may2012" << endl; 
      strm << "%%% H_time_Refram = 0.01; %%% VERY Fast Speed, THEN(!!!) capturing frec(e.g.33fps) will be USED" << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "Equal_Frec_25_Yes = 1; %%% 0 1-MAIN; for Output reframing" << endl; 
      strm << "%%% if Equal_Frec_25_Yes == 1, Frec_New = 100; h_Time = 1/Frec_New; end %%% 100 FPS" << endl; 
      strm << "%%% " << endl; 
      strm << "if Equal_Frec_25_Yes == 1, Frec_New = 10; h_Time = 1/Frec_New; end %%% 25 FPS" << endl; 
      strm << "%%% if Equal_Frec_25_Yes == 1, Frec_New = 10; h_Time = 1/Frec_New; end %%% 100 FPS" << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "%%% " << endl;
    strm << "Thorax_Root_Yes = 0; %%% 1 0-MAIN" << endl; 
      strm << "%%% Thorax_Root_Yes = 1; %%% 1 0-MAIN" << endl; 
      strm << "if sum(Opt_5_Bds(1:3),2) == 0, Thorax_Root_Yes = 1; end " << endl; 
      strm << "" << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "%%% " << endl; 
      strm << "Kinect_In_YES = 1; %%% 0 1-Kinect Data" << endl; 
      strm << "%%% Kinect_In_YES = 0; %%% 0-Vicon Data 1" << endl; 
      strm << "%%% Kinect_In_YES = KV_Input_Kinect;" << endl; 
      strm << "IFLag_AL_Fout = 1; %%% 0 1-MAIN" << endl; 
      strm << "LL_OPT_YES = 1; %%% 0 1-MAIN" << endl; 
      strm << "UpL_OPT_YES = 1; %%% 0 1-MAIN" << endl; 
      strm << "DoFs_Smooth_Yes = 1; %%% 0 1-MAIN" << endl; 
      strm << "DoFs_Corr_Yes = 1; %%% 0 1-MAIN" << endl; 
      strm << "%%% " << endl; 
      strm << "Should_LR_Corr_Yes = 1; %%% 0 1-MAIN" << endl; 
      strm << "%%% Should_LR_Corr_Yes = 0; %%% 0-Test '09Free.txt'   1-MAIN" << endl; 
      strm << "Surfl_Yes = 0; %%% 1-Trx_Ellipsoid_Fig1, 0-MAIN" << endl; 
      strm << "Par_Smth_30 = 1; %%% 10-MAIN smoothing param for OVP spline" << endl; 
      strm << "%%% Par_Smth_30 = 30; %%% 10-MAIN smoothing param for OVP spline" << endl; 
      strm << "%%% " << endl; 
      strm << "%%% Par_Smth_30 = 20; %%% 10-MAIN smoothing param for OVP spline" << endl; 
      strm << "%%% Par_Smth_30 = 100; %%% 10-MAIN smoothing param for OVP spline" << endl; 
      strm << "" << endl; 
      strm << "%%% SMTH_50000_Yes = 0; %%% Pelvis smoothed" << endl; 
      strm << "%%% " << endl; 
      strm << "SMTH_50000_Yes = 1; %%% Pelvis NOT smoothed" << endl; 
      strm << "%%% Mult_tol_Kinect = 100; %%% 1-Default" << endl; 
      strm << "%%% Mult_tol_Kinect = 20; %%% 1-Default" << endl; 
      strm << "%%% " << endl; 
      strm << "Mult_tol_Kinect = 1; %%% 1-Default for Vic_2_Kin data" << endl; 
      strm << "%%% Mult_tol_Kinect = 0; %%% 1-Default for Vic_2_Kin data" << endl; 
      strm << "" << endl; 
      strm << "%%% " << endl; 
      strm << "Delt_RL_0p1 = 0.1; %%% Shoulder rhythm range EXTENSION" << endl; 
      strm << "%%% TEST 27 Sept 2012 TOO MUCH Delt_RL_0p1 = 0.5; %%% Shoulder rhythm range EXTENSION" << endl; 
      strm << "%%% TEST 27 Sept 2012 Delt_RL_0p1 = 1.0; %%% Shoulder rhythm range EXTENSION" << endl; 
      strm << "" << endl; 
      strm << "%%% TEST 28 Sept 2012 Delt_RL_0p1 = 0.3; %%% Shoulder rhythm range EXTENSION put Check_Border_Yes = 0;" << endl; 
      strm << "" << endl; 
      strm << "%%% " << endl; 
      strm << "Check_Border_Yes = 1;" << endl; 
      strm << "%%% Check_Border_Yes = 0;" << endl; 
      strm << "%%% " << endl; 
      strm << "HKA_Flx_Ext_50000_Yes = 1; %%% RL: HipKnAnk Trnsl & Flx/Ext NOT smoothed" << endl; 
      strm << "%%% " << endl; 
      strm << "PiG_C3D_Out_Yes = 1; %%% 1 - Out C3D LL PiG; 0-no C3D out" << endl; 
      strm << "" << endl; 
      strm << "%%% Lat_Dist_Inp = 30; %%% " << endl; 
      strm << "%%% " << endl; 
      strm << "Lat_Dist_Inp = 20; %%% " << endl; 
      strm << "%%% Mult_Line_Inp = 1/3; %%% " << endl; 
      strm << "%%% " << endl; 
      strm << "Mult_Line_Inp = 2/3; %%% " << endl; 
      strm << "" << endl; 
      strm << "%%% " << endl; 
      strm << "Pig_Lat_Wnds_Yes = 0; %%% 0-Kinect; 1-Vicon PiG" << endl; 
      strm << "%%% " << endl; 
      strm << "Wnd_Dat_From_Exp = 0; %%% MAIN" << endl; 
      strm << "%%% Wnd_Dat_From_Exp = 1;" << endl; 
      strm << "" << endl; 
      strm << "%%% " << endl; 
      strm << "Delt_ang = 48*pi/180; Alpha_D = 1./3; Ampl_Sin1 = 0.1;  Ampl_Sin2 = 0.17; " << endl; 
      strm << "%%% Delt_ang = 48*pi/180; Alpha_D = .4; Ampl_Sin1 = 0.1;  Ampl_Sin2 = 0.17; " << endl; 
      strm << "" << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "x0_Poly_Apprx_Yes = [1 1 1 1 1]; %%% 1 0" << endl; 
      strm << "%%% N_Polyn = [3 3 3 3 3]; %%% [2-5] polyn" << endl; 
      strm << "%%% " << endl; 
      strm << "N_Polyn = [3 3 3 4 4]; %%% [2-5] polyn" << endl; 
      strm << "%%% Mult_T5 = [0.5 0.5 0.5 0.0 0.0]; %%% [0.0-1.0]" << endl; 
      strm << "%%% " << endl; 
      strm << "Mult_T5 = [0.5 0.5 0.5 0.3 0.3]; %%% [0.0-1.0]" << endl; 
      strm << "%%% Mult_T5 = [0.5 0.5 0.5 0.5 0.5]; %%% [0.0-1.0]" << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "%%% " << endl; 
      strm << "Bord_Delt = 0.0001;" << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "%%% " << endl; 
      strm << "Start_Fr_N = 1; " << endl; 
      strm << "%%% Start_Fr_N = 263;" << endl; 
      strm << "%%% Start_Fr_N = 201;" << endl; 
      strm << "%%% Start_Fr_N = 301; %%% UP" << endl; 
      strm << "" << endl; 
      strm << "%%% for Vicon Start_Fr_N = 230; %%% UL_21jul.c3d" << endl; 
      strm << "%%% for Vicon Start_Fr_N = 229; %%% UL_21jul.c3d" << endl; 
      strm << "%%% for Vicon Start_Fr_N = 1; %%% 02Squat.c3d" << endl; 
      strm << "" << endl; 
      strm << "%%% " << endl; 
      strm << "End_Fr_N = -1; %%% all data from Vicon C3D" << endl; 
      strm << "%%% End_Fr_N = 501; %%% data from Vicon C3D" << endl; 
      strm << "%%% End_Fr_N = 121; %%% data from Kinect New_Vic_Sqt_tst_C.dat" << endl; 
      strm << "%%% End_Fr_N = 351;  %%% Kinect_MBA_dec_2012; Abd_UpL_Sqt 17dec2012" << endl; 
      strm << "" << endl; 
      strm << "%%% " << endl; 
      strm << "Decim_Frm = 4;" << endl; 
      strm << "%%% Vic_Pelv_4ALs = [0 1 1 1]; %%%  !!! LIAS LIPS  RIAS RIPS " << endl; 
      strm << "%%% " << endl; 
      strm << "Vic_Pelv_4ALs = [1 1 1 1]; %%% MAIN  !!! LIAS LIPS  RIAS RIPS " << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "%%%%%%%%%%  11 June SVSJ Squat" << endl; 
      strm << "%%% RT_Kin_2_Vic = [  -0.99962947  -0.0034224409    0.027003927      342.23228;   0.026780458    0.053893448     0.99818751     -1777.7479; -0.0048715725     0.99854083   -0.053781824      955.71808];" << endl; 
      strm << "%%%%%%%%%%  11 June SVSJ 14Trunk" << endl; 
      strm << "%%% RT_Kin_2_Vic = [ -0.99957423 -7.5018323e-005 0.029178048  334.05104;  0.029113473  0.063978283  0.99752653 -1781.5372; -0.0019415942  0.99795129 -0.063948859 985.43056 ];" << endl; 
      strm << "%%%%%%%%%%  11 June SVSJ 09Free.txt" << endl; 
      strm << "%%% RT_Kin_2_Vic = [ -0.9990442  -0.003181937    0.04359438      295.4147; 0.043351262    0.055448726     0.99751998     -1789.0452; -0.005591298     0.9984565   -0.05525779      957.5889];" << endl; 
      strm << "%%%%%%%%%%  11 June SVSJ 12Walk.txt" << endl; 
      strm << "%%% RT_Kin_2_Vic = [ -0.9998067    0.01282821    0.01490113      380.5636; 0.015643311    0.059843495     0.99808519     -1767.8663; 0.011911915     0.99812534   -0.060032602      1020.6004];" << endl; 
      strm << "%%%%%%%%%%  11 June SVSJ 11Run.txt" << endl; 
      strm << "%%% RT_Kin_2_Vic = [ -0.9999932  -0.002111409   0.003019968       403.936; 0.0028994238    0.054935149     0.99848571     -1778.9454; -0.002274114     0.9984877   -0.05492865      955.1424];" << endl; 
      strm << "%%% " << endl; 
      strm << "RT_Kin_2_Vic = [ -0.9999932  -0.002111409   0.003019968       403.936; 0.0028994238    0.054935149     0.99848571     -1778.9454; -0.002274114     0.9984877   -0.05492865      955.1424];" << endl; 
      strm << "" << endl; 
      strm << "%%% UL_21jul.c3d and Session08  for Kinect" << endl; 
      strm << "RT_Kin_2_Vic = [ -0.9973536   0.001262106    0.07269276      323.7613; 0.072604385    0.069536243     0.99493382     -2358.8713; -0.003799069     0.9975786   -0.06944386      812.5347];" << endl; 
      strm << "%%% 02Squat.c3d and Sqt_tst_C.dat for Kinect " << endl; 
      strm << "RT_Kin_2_Vic = [ -0.9996295  -0.003422441    0.02700393      342.2323; 0.026780458    0.053893448     0.99818751     -1777.7479; -0.004871572     0.9985408   -0.05378182      955.7181];" << endl; 
      strm << "" << endl; 
      //    strm << "%%% "Smth_Scl_Sqt_tst_C.dat" %%% 02Squat.c3d and Sqt_tst_C.dat for Kinect " << endl; 
      strm << "RT_Kin_2_Vic = [ -0.9997103  -0.002643808   -0.02392442      472.9902; -0.024027614    0.050614826     0.99842917     -1768.9051; -0.001428724     0.9987147   -0.05066369       946.895];" << endl; 
      strm << "" << endl; 
      //    strm << "%%% "Smth_EqLnk_Sqt_tst_C.dat" %%% 02Squat.c3d and Sqt_tst_C.dat for Kinect " << endl; 
      strm << "RT_Kin_2_Vic = [ -0.9999756  -0.002091416   -0.00667023      429.0553; -0.0067581791    0.045323936     0.99894948     -1769.3241; -0.001786898     0.9989702   -0.04533696      934.7044];" << endl; 
      strm << "%%% UL_21jul.c3d and Smth_EqLnk_Session08  for Kinect" << endl; 
      strm << "RT_Kin_2_Vic = [ -0.9942669   0.006880558     0.1067052      227.3685; 0.10692255    0.055059747     0.99274165     -2358.0297; 0.0009554544     0.9984594   -0.05547977      767.3736];" << endl; 
      strm << "" << endl; 
      strm << "%%% NO Transform to Vicon GCS " << endl; 
      strm << "RT_Kin_2_Vic = [ eye(3) [0 0 0]'];" << endl; 
      strm << "%%% New_Vic_Sqt_tst_C.dat RT_Kin_2_Vic = [ eye(3) [0 0 0]'];" << endl; 
      strm << "" << endl; 
      strm << "%%%%% ===========================================" << endl; 
      strm << "" << endl; 
      strm << "%%% end %%% %%% function KV_Local_Param_Inp_Shv" << endl; 
      strm << "" << endl; 
      strm.close();
  }
  mafVMEGroup *ModelImport(const mafString& path, mafVME *target)
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
    if(!grp)
      return NULL;


    mafString plot_files[] = {
      /*"DoFs_2_LL72_UpL72_001.dat",
      "DoFs_2rf25_LL72_UpL72_001.dat",
      "DoFs_OVP_2_LL72_UpL72_001.dat",*/
      "DoFs_OVP_Smth_2_LL72_UpL72_001.dat",
    };

    mafString chNameShV[] ={
      "Dummy",//                 3:5;
      "Pelvis",//(Data(:,6:8));
      "RightHip",//(Data(:,9:11));
      "Dummy",//                 12:14;
      "Dummy",//                 15:17;
      "Dummy",//                 18:20;
      "Dummy",//                 21:23;
      "Dummy",//                 24:26;
      "RightKnee",//(Data(:,27:29));
      "Dummy",//                 30:32;
      "RightAnkle",//(Data(:,33:35));
      "Dummy",//                 36:38;
      "Dummy",//                 39:41;
      "LeftHip",//(Data(:,42:44));
      "Dummy",//                 45:47;
      "Dummy",//                 48:50;
      "Dummy",//                 51:53;
      "Dummy",//                 54:56;
      "Dummy",//                 57:59;
      "LeftKnee",//(Data(:,60:62));
      "Dummy",//                 63:65;
      "LeftAnkle",//(Data(:,66:68));
      "Dummy",//                 69:14;
      "Dummy",//                 72:14;
      "Dummy",//                 75:14;
      "Dummy",//                 78:14;
      "Dummy",//                 81:14;
      "Spine",//(Data(:,84:86));
      "Dummy",//                 87:14;
      "Thorax",//(Data(:,90:92));
      "Dummy",//                 93:14;
      "Neck",//(Data(:,96:98));
      "Dummy",//                 99:14;
      "Head",//(Data(:,102:104));
      "RightClavicle",//(Data(:,105:107));
      "RightScapula",//(Data(:,108:110));
      "Dummy",//                 111:14;
      "RightShoulder",//(Data(:,114:116));
      "RightElbow",//(Data(:,117:119));
      "RightForearm",//(Data(:,120:122));
      "RightWrist",//(Data(:,123:125));
      "LeftClavicle",//(Data(:,126:128));
      "LeftScapula",//(Data(:,129:131));
      "Dummy",//                 132:14;
      "LeftShoulder",//(Data(:,135:137));
      "LeftElbow",//(Data(:,138:140));
      "LeftForearm",//(Data(:,141:143));
      "LeftWrist"//(Data(:,144:146));
    };


    for(int i = 0; i < DIM(plot_files); i++)
    {
      mafString fpath;
      fpath = path;
      fpath += "\\";
      fpath += plot_files[i];
      if(!mafFileExists(fpath))
        continue;

      std::vector<std::vector<double> > rmatrix;
      if(!readMatrix(fpath, rmatrix))
        continue;

      lhpVMEKMInfo *kmi = lhpVMEKMInfo::New();
      kmi->Register(NULL);
      kmi->SetName(target->GetName() + "_GeneralInfo");
      kmi->ReparentTo(target);

      medVMEAnalog *analog = medVMEAnalog::New();
      analog->Register(NULL);

      //mafString analogVmeName;
      //analogVmeName.Append(plot_files[i]);
      analog->SetName(target->GetName() + "_Graph");

      vnl_matrix<double> analogMatrix;
      analogMatrix.set_size(rmatrix[0].size() - 1 - 25 * 3, rmatrix.size()); //set dimensions

      std::vector<mafString> channelsNameList; //string array for channel name


      //For every Sample
      for(int currentSample=0; currentSample < rmatrix.size(); currentSample++)
      {
        mafTimeStamp currentTime = rmatrix[currentSample][1];

        analogMatrix.put(0, currentSample, currentTime); //fill first row with timeframe, every column is a time

        //For every channel
        int skipped = 0;
        for(int currentChannel = 0; currentChannel < rmatrix[0].size() - 2; currentChannel++)
        {

          //intData.m_ChannelName = m_Acq->GetAnalog(currentChannel)->GetLabel().c_str();
          //intData.m_AnalogValue = m_Acq->GetAnalog(currentChannel)->GetValues()(currentSample, 0);
          //intData.m_ChannelUnit = m_Acq->GetAnalog(currentChannel)->GetUnit().c_str();

          if(chNameShV[currentChannel / 3] == "Dummy")
          {
            skipped++;
            continue;
          }
          if(currentSample == 0) 
          {
            mafString chName;
            chName = chNameShV[currentChannel / 3];
            if(currentChannel % 3 == 0)
              chName += " FlexExt";
            else if(currentChannel % 3 == 1)
              chName += " RotExtInt";
            else if(currentChannel % 3 == 2)
              chName += " AbdAdd";
            channelsNameList.push_back(chName);
          }

          analogMatrix.put(currentChannel + 1 - skipped, currentSample, rmatrix[currentSample][currentChannel + 2] * (180. / 3.14159265358979323846)); //fill following rows with values, every channel is a row
        }

      }
      if(kmi->GetName().FindFirst("Hand-To-Head") != -1 || kmi->GetName().FindFirst("Hand-To-Mouth") != -1 || kmi->GetName().FindFirst("Hand-To-Back") != -1)
      {
        for(int currentSample = 0; currentSample < analogMatrix.columns(); currentSample++)
        {
          mafTimeStamp currentTime = analogMatrix.get(0, currentSample);

          double tmp;
          tmp = analogMatrix.get(40, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(0))
            kmi->SetValue(0, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(1))
            kmi->SetValue(1, tmp);

          tmp = analogMatrix.get(41, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(6))
            kmi->SetValue(6, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(7))
            kmi->SetValue(7, tmp);

          tmp = analogMatrix.get(42, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(3))
            kmi->SetValue(3, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(4))
            kmi->SetValue(4, tmp);

          tmp = analogMatrix.get(58, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(9))
            kmi->SetValue(9, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(10))
            kmi->SetValue(10, tmp);
          tmp = analogMatrix.get(59, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(15))
            kmi->SetValue(15, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(16))
            kmi->SetValue(16, tmp);
          tmp = analogMatrix.get(60, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(12))
            kmi->SetValue(12, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(13))
            kmi->SetValue(13, tmp);



          tmp = analogMatrix.get(43, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(27))
            kmi->SetValue(27, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(28))
            kmi->SetValue(28, tmp);
          tmp = analogMatrix.get(44, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(33))
            kmi->SetValue(33, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(34))
            kmi->SetValue(34, tmp);
          tmp = analogMatrix.get(45, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(30))
            kmi->SetValue(30, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(31))
            kmi->SetValue(31, tmp);


          tmp = analogMatrix.get(61, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(36))
            kmi->SetValue(36, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(37))
            kmi->SetValue(37, tmp);
          tmp = analogMatrix.get(62, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(42))
            kmi->SetValue(42, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(43))
            kmi->SetValue(43, tmp);
          tmp = analogMatrix.get(63, currentSample);
          if(currentSample == 0 || tmp > kmi->GetValue(39))
            kmi->SetValue(39, tmp);
          if(currentSample == 0 || tmp < kmi->GetValue(40))
            kmi->SetValue(40, tmp);
        }
      }


      analog->SetData(analogMatrix, 0);

      mafTagItem tag_Sig;
      tag_Sig.SetName("SIGNALS_NAME");
      tag_Sig.SetNumberOfComponents(rmatrix.size());
      analog->GetTagArray()->SetTag(tag_Sig);

      mafTagItem *tag_Signals = analog->GetTagArray()->GetTag("SIGNALS_NAME");
      for (int n = 0; n < channelsNameList.size(); n++)
      {
        tag_Signals->SetValue(channelsNameList[n], n);
      }
      analog->ReparentTo(target);
      mafDEL(analog);
      mafDEL(kmi);
    }
    grp->ReparentTo(target);
    grp->Delete();

    return grp;
  }
}

mafCxxTypeMacro(lhpOpKinectUtil)
  //----------------------------------------------------------------------------
  lhpOpKinectUtil::lhpOpKinectUtil(bool extapp, const mafString& label, bool simple, bool llimb, bool ulimb) : Superclass(label)
  //----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_IMPORTER;
  m_Simple    = simple;
  m_LLimb     = llimb;
  m_ULimb     = ulimb;
  m_Canundo   = false;
  m_FileDir = mafGetApplicationDirectory() + "/Data/External/";
  m_DictionaryFileName = "";
  m_Scale  = 1.0;
  m_Freq   = 30.0;
  m_AFs    = false;
  m_Model  = simple;//false;
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
  lhpOpKinectUtil *op = new lhpOpKinectUtil(m_ExtApp, GetLabel(), m_Simple, m_LLimb, m_ULimb);
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
  if (!m_TestMode && !m_Simple)
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
    mafEventMacro(mafEvent(this, VME_COLLAPSESUBTREE, m_Imported[i]));
    mafEventMacro(mafEvent(this, VME_EXPAND, m_Imported[i]));
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

  //----------------------------------------------------------------------------
  int ExtractMatchingPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, double time, vtkPoints *m_PointsSource, vtkPoints *m_PointsTarget)
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
  double RegisterPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *reg, double currTime, vtkPoints *m_PointsSource, vtkPoints *m_PointsTarget)
    //----------------------------------------------------------------------------
  {
    double deviation = 0.0;
    assert(m_PointsSource && m_PointsTarget);

    vtkWeightedLandmarkTransform *RegisterTransform = vtkWeightedLandmarkTransform::New();

    RegisterTransform->SetSourceLandmarks(m_PointsSource);
    RegisterTransform->SetTargetLandmarks(m_PointsTarget);

    if(currTime < 0)
      currTime = trg->GetTimeStamp();

    RegisterTransform->SetModeToSimilarity();
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

    mafMatrix *mat = mafMatrix::New();
    mat->Register(NULL);
    mat->Identity();
    trg->GetOutput()->GetAbsMatrix(*mat,currTime);  //modified by Marco. 2-2-2004

    vtkMatrix4x4::Multiply4x4(mat->GetVTKMatrix(),regt_matrix,t_matrix);
    mafDEL(mat);
    vtkDEL(regt_matrix);

    int numLandmarks = trg->GetNumberOfVisibleLandmarks(currTime);

    if(numLandmarks < 2)
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


  bool ProcessNode(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *registered, vtkPoints *m_PointsSource, vtkPoints *m_PointsTarget)
  {
    mafVMEInfoText *info = mafVMEInfoText::New();
    info->Register(NULL);
    wxString name = wxString::Format("Info for registration");// %s into %s",m_Source->GetName().GetCStr(), m_Target->GetName().GetCStr());
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

    {
      std::vector<mafTimeStamp> timeStamps;
      trg->GetLocalTimeStamps(timeStamps);
      int numTimeStamps = timeStamps.size();


      for (int t = 0; t < numTimeStamps; t++)
      {
        double currTime = timeStamps[t];
        long p = t * 100 / numTimeStamps;
        //  mafProgressBarSetValueMacro(p);
        //Set the new time for the vme used to register the one frame source 
        trg->SetTimeStamp(currTime); //set current time
        trg->Update(); //>UpdateAllData();
        if(ExtractMatchingPoints(src, trg, currTime, m_PointsSource, m_PointsTarget))
        {
          if(!infoAdded)
            info->ReparentTo(registered);
          infoAdded = true;
          double tr = RegisterPoints(src, trg, registered, currTime, m_PointsSource, m_PointsTarget);
          info->SetAbsPose(tr, 0.0, 0.0, 0.0, 0.0, 0.0, currTime);
        }
      }
      timeStamps.clear();

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

  mafVME *RegScripted(mafVME* m_Source, mafVME* m_Target)
  {
    if(m_Source == NULL || m_Target == NULL)
      return NULL;
    vtkPoints *m_PointsSource = vtkPoints::New();
    vtkPoints *m_PointsTarget = vtkPoints::New();

    mafVME* m_Registered = NULL;
    std::vector<std::pair<wxString, wxString> >  m_LMDict;
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpPelvisALs","UpPelv"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpLumbarALs","UpLumb"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpThoraxALs","UpThor"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpCervicALs","UpCerv"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpSkullJwALs","UpSkul"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpLClavicleALs","UpLCla"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpLScapulaALs_N","UpLSca"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpLHumerusALs","UpLHum"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpLUlnaALs","UpLUln"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpLRadiusALs","UpLRad"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpLHandALs","UpLHan"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpRClavicleALs","UpRCla"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpRScapulaALs_N","UpRSca"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpRHumerusALs","UpRHum"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpRUlnaALs","UpRUln"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpRRadiusALs","UpRRad"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("UpRHandALs","UpRHan"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("PelvisALs","Pelvis"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("LeftFemurALs_3Bd_1","L_Thg1"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("LeftFemurALs_3Bd_2","L_Thg2"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("LeftFemurALs_3Bd_3","L_Thg3"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("LeftShankALs","L_Shan"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("LeftFootALs","L_Foot"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("LeftPatellaALs","L_Pate"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("RightFemurALs_3Bd_1","R_Thg1"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("RightFemurALs_3Bd_2","R_Thg2"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("RightFemurALs_3Bd_3","R_Thg3"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("RightShankALs","R_Shan"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("RightFootALs","R_Foot"));
    m_LMDict.push_back(std::make_pair<wxString, wxString>("RightPatellaALs","R_Pate"));


    if(m_Registered == NULL)
    {
      wxString name = wxString::Format("Registered model");
      m_Registered= mafVME::SafeDownCast(CopyTreeTimeStamp(m_Source));
      m_Registered->Register(NULL);
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
      bool res = ProcessNode(lmcs, lmct, lmcr, m_PointsSource, m_PointsTarget);
      processed = processed || res;
    }

    vtkDEL(m_PointsSource);
    vtkDEL(m_PointsTarget);

    if(!processed)
      mafDEL(m_Registered);

    return m_Registered;
  }
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
  mafVMEGroup *skeletalGroup;
  {
    mafString zmsfPlacement;
    mafString zmsfName;
    zmsfPlacement = mafCreateTempFileName("");
    zmsfName = zmsfPlacement + "\\SkelModel.zmsf";
    mafDirMake(zmsfPlacement);
    HINSTANCE hInstance = wxGetInstance();
    HRSRC hrSrc = FindResource(hInstance, "SkelModel", _T("ZMSF"));
    HGLOBAL exef = LoadResource(hInstance, hrSrc);
    size_t sz = SizeofResource(hInstance, hrSrc);
    void *buf = LockResource(exef);
    FILE *exeF = fopen(zmsfName.GetCStr(), "wb");
    fwrite(buf, 1, sz, exeF);
    fclose(exeF);
    skeletalGroup = ImportMSFFile(zmsfName);
    mafRemoveDirectory(zmsfPlacement);
    //skeletalGroup->ReparentTo(m_Input);
  }
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
      imported->SetName(name);
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
        //imported->SetName(name);
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
        if(/*mafFileExists(fullSName) && */mafFileExists(anthroFile))
        {
          mafString newAnthro;
          newAnthro = mtlbTmp;
          newAnthro += "/";
          newAnthro += "AnthropometryKinect.m";
          mafFileCopy(anthroFile, newAnthro);
          mafDirMake(modelPath);
          writeParams(mtlbTmp, path, nmext, staticName, modelPath, m_LLimb, m_ULimb);


          mafString commandline;
          commandline = mtlbTmp;
          commandline += "/KinVic_Opt_ShV_2012.exe KV_Local_Param_Inp_Shv_K.m LL_Model_S035_K.dat UpL_Model_S035_K.dat >a.log";
          wxSetWorkingDirectory(mtlbTmp.GetCStr());
          if(wxExecute(commandline.GetCStr(), wxEXEC_SYNC) == 0)
          {
            if(mafVMEGroup *grp = ModelImport(modelPath, imported))
            {
              if(m_Simple)
              {
                mafNode *finReg = RegScripted(skeletalGroup, grp);
                if(finReg)
                {
                  finReg->ReparentTo(imported);
                  mafDEL(finReg);
                }
              }
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
  mafDEL(skeletalGroup);
  //mafEventMacro(mafEvent(this,VIEW_CREATE, (long)12399));
  //mafEventMacro(mafEvent(this,VIEW_CREATE, (long)12400));
  return result;
}

//----------------------------------------------------------------------------
mafVME *lhpOpKinectUtil::ImportSingleFile(const mafString &fullFileName)
//----------------------------------------------------------------------------
{
  std::vector<std::vector<double> > rmatrix;
  if(!readMatrix(fullFileName, rmatrix))
    return NULL;


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
