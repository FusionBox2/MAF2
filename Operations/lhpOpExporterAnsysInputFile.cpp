/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpExporterAnsysInputFile.cpp,v $
  Language:  C++
  Date:      $Date: 2009-04-10 13:50:21 $
  Version:   $Revision: 1.9.2.4 $
  Authors:   Stefano Perticoni
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
#include "lhpOpExporterAnsysInputFile.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMEMesh.h"
#include "mafVMEMeshAnsysTextExporter.h"
#include "mafAbsMatrixPipe.h"

#include "vtkMAFSmartPointer.h"

#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpExporterAnsysInputFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpExporterAnsysInputFile::lhpOpExporterAnsysInputFile(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_PythonExe = _R("python.exe_UNDEFINED");
  m_PythonwExe = _R("pythonw.exe_UNDEFINED");

  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;
  m_NodesFileName = _R("");
  m_ElementsFileName = _R("");
  m_MaterialsFileName = _R("");
  m_AnsysInputFileName = _R("");
  m_CacheDir = lhpUtils::lhpGetApplicationDirectory() + _R("\\Data\\AnsysWriterCache");
  m_AnsysOutputFileNameFullPath		= _R("");
  m_FileDir = lhpUtils::lhpGetApplicationDirectory() + _R("/Data/External/");
  
  // This is for deploy: need to work on PYTHONPATH to solve issues with Python modules execution path...
  m_AnsysPythonExporterFullPathFileName = lhpUtils::lhpGetApplicationDirectory() + _R("\\ASCIIParser\\ansysWriter.py");
  
  m_Pid = -1;
  m_ABSMatrixFlag = 1;
}

enum ANSYS_EXPORTER_ID
{
  ID_ABS_MATRIX_TO_STL = MINID,  
};

//----------------------------------------------------------------------------
lhpOpExporterAnsysInputFile::~lhpOpExporterAnsysInputFile()
//----------------------------------------------------------------------------
{
  
  mafDEL(m_ImportedVmeMesh);
}
//----------------------------------------------------------------------------
bool lhpOpExporterAnsysInputFile::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node->IsA("mafVMEMesh"));
}
//----------------------------------------------------------------------------
mafOp* lhpOpExporterAnsysInputFile::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpExporterAnsysInputFile *cp = new lhpOpExporterAnsysInputFile(GetLabel());
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpExporterAnsysInputFile::OpRun()   
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
int lhpOpExporterAnsysInputFile::Write()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Writing file: ..."));
  }

  m_NodesFileName = m_CacheDir + _R("\\AnsysExporterNodes.lis");
  m_ElementsFileName = m_CacheDir + _R("\\AnsysExporterElements.lis");
  m_MaterialsFileName = m_CacheDir + _R("\\AnsysExporterMaterials.lis");

  mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
  mafVMEMesh *input = mafVMEMesh::SafeDownCast(m_Input);
  assert(input);

  exporter->SetInput(input->GetUnstructuredGridOutput()->GetUnstructuredGridData());
  exporter->SetOutputNodesFileName(m_NodesFileName.GetCStr());
  exporter->SetOutputElementsFileName(m_ElementsFileName.GetCStr());
  exporter->SetOutputMaterialsFileName(m_MaterialsFileName.GetCStr());

  mafVMEMesh *inMesh = mafVMEMesh::SafeDownCast(m_Input);
  assert(inMesh);

  exporter->SetMatrix(inMesh->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());
  exporter->SetApplyMatrix(m_ABSMatrixFlag);
  int result = exporter->Write();
  delete exporter;
  
  if (result == MAF_OK)
  {
    // continue
  } 
  else
  {
    wxMessageBox("Problems generating intermediate output files! \n\
See the log area for more details. ");
    return MAF_ERROR;
  }

  wxArrayString output;
  wxArrayString errors;
  
  // execute the Python reader
  mafString command2execute;
  
  mafLogMessage(_M(_R("Current working directory is: '") + mafWxToString(wxGetCwd()) + _R("' ")));

  // read cache files and create ansys file
  command2execute = m_PythonExe;
  command2execute.Append(_R(" \"\""));
  command2execute.Append(m_AnsysPythonExporterFullPathFileName);
  command2execute.Append(_R(" \""));
  command2execute.Append(m_NodesFileName);
  command2execute.Append(_R("\" \""));
  command2execute.Append(m_ElementsFileName);
  command2execute.Append(_R("\" \""));
  command2execute.Append(m_MaterialsFileName);
  command2execute.Append(_R("\" \""));
  command2execute.Append(m_AnsysOutputFileNameFullPath);
  command2execute.Append(_R("\""));
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

  return MAF_OK;

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
void lhpOpExporterAnsysInputFile::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildc = _R("Stereo Litography (*.stl)|*.stl");

  m_Gui = new mafGUI(this);
  m_Gui->Label(_R("absolute matrix"),true);
  m_Gui->Bool(ID_ABS_MATRIX_TO_STL,_R("apply"),&m_ABSMatrixFlag,0);
  m_Gui->OkCancel();  
  m_Gui->Divider();

  ShowGui();

}
//----------------------------------------------------------------------------
void lhpOpExporterAnsysInputFile::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case wxOK:
      {
        OnOK();
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
long lhpOpExporterAnsysInputFile::GetPid()   
//----------------------------------------------------------------------------
{
  return m_Pid;
}

void lhpOpExporterAnsysInputFile::OnOK()
{
  mafString wildcard = _R("inp files (*.inp)|*.inp|All Files (*.*)|*.*");

  m_AnsysOutputFileNameFullPath = _R("");

  bool cacheDirExist = mafDirExists(m_CacheDir);

  if (cacheDirExist == false)
  {
    std::ostringstream stringStream;
    stringStream << "creating cache dir: " << m_CacheDir.GetCStr() << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
    mafDirMake(m_CacheDir);
  }
  else
  {
    std::ostringstream stringStream;
    stringStream << "found cache dir: " << m_CacheDir.GetCStr() << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }

  mafString f;
  f = mafGetSaveFile(m_FileDir,wildcard); 
  if(!f.IsEmpty())
  {
    m_AnsysOutputFileNameFullPath = f;
    Write();
  }
 }

//----------------------------------------------------------------------------
void lhpOpExporterAnsysInputFile::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));        
}