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

#include "mafOpExplodeCollapse.h"
#include "lhpOpKinectAFs.h"

#include "mafSmartPointer.h"
#include "mafMatrixVector.h"
#include "mafDataVector.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
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
  ID_LOAD_DICT,
  ID_CLEAR_DICT,
  ID_FREQ,
  ID_AFS,
  ID_TYPEOFREFS,
  ID_SCALE,
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

mafCxxTypeMacro(lhpOpKinectUtil)
//----------------------------------------------------------------------------
lhpOpKinectUtil::lhpOpKinectUtil(bool extapp, const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_IMPORTER;
  m_Canundo   = false;
  m_FileDir = mafGetApplicationDirectory() + "/Data/External/";
  m_DictionaryFileName = "";
  m_ExtAppPath         = "SkeletalViewerBart.exe";
  m_Scale  = 1000.0;
  m_Freq   = 30.0;
  m_AFs    = true;
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
    m_Gui->FileOpen(ID_EXTAPPPATH, "Ext app", &m_ExtAppPath, "*.exe");
  m_Gui->Double(ID_SCALE, _("Scale"), &m_Scale, 0.0);
  m_Gui->Double(ID_FREQ, _("Freq"), &m_Freq, 0.000001);
  m_Gui->Bool(ID_AFS, _("AFs"), &m_AFs);
  m_Gui->Combo(ID_TYPEOFREFS, "Type",&m_TypeOfRefs, 2, refs_names);
  m_Gui->Label("");
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

  if(m_ExtApp)
  {
    if(m_ExtAppPath.IsEmpty())
      return false;
    mafString filetoprd = mafCreateTempFileName("");
    filetoprd += ".txt";
    mafString commandline = m_ExtAppPath;
    commandline += " " + filetoprd;
    if(wxExecute(commandline.GetCStr(), wxEXEC_SYNC) != 0)
      return false;
    if(!mafFileExists(filetoprd))
      return false;
    m_C3DInputFileNameFullPaths.clear();
    m_C3DInputFileNameFullPaths.push_back(filetoprd);
  }

  for(unsigned fileIndex = 0; fileIndex < m_C3DInputFileNameFullPaths.size(); fileIndex++)
  {
    mafVME *imported = ImportSingleFile(m_C3DInputFileNameFullPaths[fileIndex]);
    if(imported != NULL)
    {
      if(m_ExtApp)
        imported->SetName("KinectCaptured");
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


  mafVMELandmarkCloud *dlc;
  mafNEW(dlc);

  dlc->SetName(mafFileNameFromPath(fullFileName));
  dlc->SetRadius(15);

  int current_lm = 0;
  int initValue = (rmatrix[0].size() % 3 == 1) ? 1 : 0;

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

    dlc->AppendLandmark(lm_name);
    current_lm++;

    for (int i = 0; i < rmatrix.size(); i++)
    { 
      double ts = (initValue == 1) ? rmatrix[i][0] : i / m_Freq;
      dlc->SetLandmark(lm_name,
        m_Scale * rmatrix[i][j],
        m_Scale * rmatrix[i][j + 1],
        m_Scale * rmatrix[i][j + 2], 
        ts);
    }	

  }						
  return dlc;
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
