/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterAnsysCDBFile.cpp,v $
  Language:  C++
  Date:      $Date: 2008-10-28 16:32:37 $
  Version:   $Revision: 1.1.2.2 $
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
lhpOpImporterAnsysCDBFile::lhpOpImporterAnsysCDBFile(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;
  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
  m_AnsysInputFileName = "";
  m_CacheDir = (mafGetApplicationDirectory() + "\\CDBParser\\AnsysReaderCache").c_str();
  m_AnsysInputFileNameFullPath		= "";
  m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
  
  // This is for deploy: need to work on PYTHONPATH to solve issues with Python modules execution path...
  m_AnsysPythonImporterFullPathFileName = (mafGetApplicationDirectory() + "\\CDBParser\\ansysCDBReader.py").c_str();
  
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
  lhpOpImporterAnsysCDBFile *cp = new lhpOpImporterAnsysCDBFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpImporterAnsysCDBFile::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
    // ShowGui();
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

//   wxExecute (command, output, errors);
  
  // execute the Python reader
  wxString command2execute;
  // command2execute = "python.exe ansysreaderpath filename";
  // mafLogMessage(wxGetCwd());

  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  command2execute = "python.exe";
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
void lhpOpImporterAnsysCDBFile::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildcard = "cdb files (*.cdb)|*.cdb|All Files (*.*)|*.*";

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
