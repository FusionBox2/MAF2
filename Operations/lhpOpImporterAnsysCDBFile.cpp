/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterAnsysCDBFile.cpp,v $
  Language:  C++
  Date:      $Date: 2009-04-10 13:50:21 $
  Version:   $Revision: 1.1.2.8 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpBuilderDecl.h"
#include "lhpUtils.h"

#include "lhpOpImporterAnsysCDBFile.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"

#include "mafVMEMeshAnsysTextImporter.h"

#include "vtkMAFSmartPointer.h"

#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpImporterAnsysCDBFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpImporterAnsysCDBFile::lhpOpImporterAnsysCDBFile(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_PythonExe = _R("python.exe_UNDEFINED");
  m_PythonwExe = _R("pythonw.exe_UNDEFINED");

  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;
  m_NodesFileName = _R("");
  m_ElementsFileName = _R("");
  m_MaterialsFileName = _R("");
  m_AnsysInputFileName = _R("");
  m_CacheDir = lhpUtils::lhpGetApplicationDirectory() + _R("\\Data\\AnsysReaderCache");
  m_AnsysInputFileNameFullPath		= _R("");
  m_FileDir = lhpUtils::lhpGetApplicationDirectory() + _R("/Data/External/");
  
  // This is for deploy: need to work on PYTHONPATH to solve issues with Python modules execution path...
  m_AnsysPythonImporterFullPathFileName = lhpUtils::lhpGetApplicationDirectory() + _R("\\CDBParser\\ansysCDBReader.py");
  
  // This is for local testing: 
  // m_AnsysPythonImporterFullPathFileName = "D:\\vapps\\LHPBuilder_Parabuild\\CDBParser\\ansysCDBReader.py";
  m_Pid = -1;
}

//----------------------------------------------------------------------------
lhpOpImporterAnsysCDBFile::~lhpOpImporterAnsysCDBFile()
//----------------------------------------------------------------------------
{
  mafDEL(m_ImportedVmeMesh);
}
//----------------------------------------------------------------------------
bool lhpOpImporterAnsysCDBFile::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* lhpOpImporterAnsysCDBFile::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpImporterAnsysCDBFile *cp = new lhpOpImporterAnsysCDBFile(GetLabel());
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpImporterAnsysCDBFile::OpRun()   
//----------------------------------------------------------------------------
{
  // get python interpreters
  mafEvent eventGetPythonExe;
  eventGetPythonExe.SetSender(this);
  eventGetPythonExe.SetId(ID_REQUEST_PYTHON_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonExe);

  if(eventGetPythonExe.GetString())
  {
    m_PythonExe.Erase(0);
    m_PythonExe = *eventGetPythonExe.GetString();
    m_PythonExe.Append(_R(" "));
  }

  mafEvent eventGetPythonwExe;
  eventGetPythonwExe.SetSender(this);
  eventGetPythonwExe.SetId(ID_REQUEST_PYTHONW_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonwExe);

  if(eventGetPythonwExe.GetString())
  {
    m_PythonwExe.Erase(0);
    m_PythonwExe = *eventGetPythonwExe.GetString();
    m_PythonwExe.Append(_R(" "));
  }


  CreateGui();
    
}
//----------------------------------------------------------------------------
int lhpOpImporterAnsysCDBFile::Read()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Loading file: ..."));
  }
	
  wxArrayString output;
  wxArrayString errors;
  
  // execute the Python reader
  mafString command2execute;
  
  mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  command2execute = m_PythonExe;
  command2execute.Append(_R(" \""));
  command2execute.Append(m_AnsysPythonImporterFullPathFileName);
  command2execute.Append(_R("\""));
  command2execute.Append(_R(" \""));
  command2execute.Append(m_AnsysInputFileNameFullPath);
  command2execute.Append(_R("\""));
  command2execute.Append(_R(" \""));
  command2execute.Append(m_CacheDir);
  command2execute.Append(_R("\" "));
  command2execute.Append(_R("\"nodes.lis\" \"elements.lis\" \"materials.lis\""));
  
  mafLogMessage(_M(_R("Executing command: '") + command2execute + _R("'")));

  // m_Pid = wxExecute(command2execute, output, errors, wxEXEC_NODISABLE);
  m_Pid = wxExecute(command2execute.toWx(), wxEXEC_SYNC);

  for (int i = 0; i < output.GetCount(); i++)
  {
    mafLogMessage(_M(mafWxToString(output[i])));
  }
  
  if ( !command2execute.GetCStr())
    return MAF_ERROR;

  mafLogMessage(_M(_R("Command process '") + command2execute + _R("' terminated with exit code ") + mafToString(m_Pid) + _R(".")));

  m_NodesFileName = m_CacheDir + _R("\\nodes.lis");
  m_ElementsFileName = m_CacheDir + _R("\\elements.lis");
  m_MaterialsFileName = m_CacheDir + _R("\\materials.lis");

  mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(m_NodesFileName.GetCStr());
  reader->SetElementsFileName(m_ElementsFileName.GetCStr());
  reader->SetMaterialsFileName(m_MaterialsFileName.GetCStr());
	int returnValue = reader->Read();

  if (returnValue == MAF_ERROR)
  {
    if (!m_TestMode)
    {
      mafErrorMessage(_M(mafString(_L("Error parsing input files! See log window for details..."))));
    }
  } 
  else if (returnValue == MAF_OK)
  {
    mafNEW(m_ImportedVmeMesh);
    m_ImportedVmeMesh->SetName(_R("Imported Mesh"));
	  m_ImportedVmeMesh->SetDataByDetaching(reader->GetOutput()->GetUnstructuredGridOutput()->GetVTKData(),0);

    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));
    m_ImportedVmeMesh->GetTagArray()->SetTag(tag_Nature);

    m_Output = m_ImportedVmeMesh;
  }

  delete reader;
  return returnValue;
}
//----------------------------------------------------------------------------
// Operation constants
//----------------------------------------------------------------------------
enum Mesh_Importer_ID
{
  ID_FIRST = MINID,
  ID_Importer_Type,
  ID_NodesFileName,
  ID_ElementsFileName,
  ID_MaterialsFileName,
  ID_OK,
  ID_CANCEL,
};
//----------------------------------------------------------------------------
void lhpOpImporterAnsysCDBFile::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildcard = _R("cdb files (*.cdb)|*.cdb|All Files (*.*)|*.*");

  int result = OP_RUN_CANCEL;
  m_AnsysInputFileNameFullPath = _R("");
  
  mafString f;
  f = mafGetOpenFile(m_FileDir,wildcard); 
  if(!f.IsEmpty() && mafFileExists(f))
  {
    m_AnsysInputFileNameFullPath = f;
    Read();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void lhpOpImporterAnsysCDBFile::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case wxOK:
      {
        this->Read();
        this->OpStop(OP_RUN_OK);
      }
      break;
      case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
      }
      break;
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
long lhpOpImporterAnsysCDBFile::GetPid()   
//----------------------------------------------------------------------------
{
  return m_Pid;
}
