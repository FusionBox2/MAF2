/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterAnsysInputFile.cpp,v $
  Language:  C++
  Date:      $Date: 2008-11-17 13:07:45 $
  Version:   $Revision: 1.6.2.2 $
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
#include "lhpOpImporterAnsysInputFile.h"

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
mafCxxTypeMacro(lhpOpImporterAnsysInputFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpImporterAnsysInputFile::lhpOpImporterAnsysInputFile(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_PythonExe = "python.exe_UNDEFINED";
  m_PythonwExe = "pythonw.exe_UNDEFINED";
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;
  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
  m_AnsysInputFileName = "";
  m_CacheDir = (mafGetApplicationDirectory() + "\\Data\\AnsysReaderCache").c_str();
  m_AnsysInputFileNameFullPath		= "";
  m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
  
  // This is for deploy: need to work on PYTHONPATH to solve issues with Python modules execution path...
  m_AnsysPythonImporterFullPathFileName = (mafGetApplicationDirectory() + "\\ASCIIParser\\ansysReader.py").c_str();
  
  // This is for local testing: 
  // m_AnsysPythonImporterFullPathFileName = "D:\\vapps\\LHPBuilder_Parabuild\\ASCIIParser\\ansysReader.py";
  m_Pid = -1;
}

//----------------------------------------------------------------------------
lhpOpImporterAnsysInputFile::~lhpOpImporterAnsysInputFile()
//----------------------------------------------------------------------------
{
  mafDEL(m_ImportedVmeMesh);
}
//----------------------------------------------------------------------------
bool lhpOpImporterAnsysInputFile::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* lhpOpImporterAnsysInputFile::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpImporterAnsysInputFile *cp = new lhpOpImporterAnsysInputFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpImporterAnsysInputFile::OpRun()   
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
    m_PythonExe = eventGetPythonExe.GetString()->GetCStr();
    m_PythonExe.Append(" ");
  }

  mafEvent eventGetPythonwExe;
  eventGetPythonwExe.SetSender(this);
  eventGetPythonwExe.SetId(ID_REQUEST_PYTHONW_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonwExe);

  if(eventGetPythonwExe.GetString())
  {
    m_PythonwExe.Erase(0);
    m_PythonwExe = eventGetPythonwExe.GetString()->GetCStr();
    m_PythonwExe.Append(" ");
  }


  CreateGui();
    
}
//----------------------------------------------------------------------------
int lhpOpImporterAnsysInputFile::Read()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Loading file: ..."));
  }
	
  wxArrayString output;
  wxArrayString errors;

//   wxExecute (command, output, errors);
  
  // execute the Python reader
  wxString command2execute;
  // mafLogMessage(wxGetCwd());

  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  command2execute = m_PythonExe.GetCStr();
  command2execute.Append(" \"\"");
  command2execute.Append(m_AnsysPythonImporterFullPathFileName.c_str());
  command2execute.Append(" \"");
  command2execute.Append(m_AnsysInputFileNameFullPath.c_str());
  command2execute.Append("\" \"");
  command2execute.Append(m_CacheDir);
  command2execute.Append("\" ");
  command2execute.Append("\"nodes.lis\" \"elements.lis\" \"materials.lis\"");
  
  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  // m_Pid = wxExecute(command2execute, output, errors, wxEXEC_NODISABLE);
  m_Pid = wxExecute(command2execute, wxEXEC_SYNC);

  for (int i = 0; i < output.GetCount(); i++)
  {
    mafLogMessage(output[i].c_str());
  }
  
  if ( !command2execute )
    return MAF_ERROR;

  mafLogMessage(_T("Command process '%s' terminated with exit code %d."),
    command2execute.c_str(), m_Pid);

  m_NodesFileName = m_CacheDir + "\\nodes.lis" ;
  m_ElementsFileName = m_CacheDir + "\\elements.lis" ;
  m_MaterialsFileName = m_CacheDir + "\\materials.lis" ;

  mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(m_NodesFileName.c_str());
  reader->SetElementsFileName(m_ElementsFileName.c_str());
  reader->SetMaterialsFileName(m_MaterialsFileName.c_str());
	int returnValue = reader->Read();

  if (returnValue == MAF_ERROR)
  {
    if (!m_TestMode)
    {
      mafMessage(_("Error parsing input files! See log window for details..."),_("Error"));
    }
  } 
  else if (returnValue == MAF_OK)
  {
    mafNEW(m_ImportedVmeMesh);
    m_ImportedVmeMesh->SetName("Imported Mesh");
	  m_ImportedVmeMesh->SetDataByDetaching(reader->GetOutput()->GetUnstructuredGridOutput()->GetVTKData(),0);

    mafTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");
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
void lhpOpImporterAnsysInputFile::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildcard = "inp files (*.inp)|*.inp|All Files (*.*)|*.*";

  int result = OP_RUN_CANCEL;
  m_AnsysInputFileNameFullPath = "";
  
  wxString f;
  f = mafGetOpenFile(m_FileDir,wildcard).c_str(); 
  if(!f.IsEmpty() && wxFileExists(f))
  {
    m_AnsysInputFileNameFullPath = f;
    Read();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void lhpOpImporterAnsysInputFile::OnEvent(mafEventBase *maf_event) 
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
long lhpOpImporterAnsysInputFile::GetPid()   
//----------------------------------------------------------------------------
{
  return m_Pid;
}
