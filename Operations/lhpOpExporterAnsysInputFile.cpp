/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpExporterAnsysInputFile.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-08 10:33:23 $
  Version:   $Revision: 1.6 $
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

#include "lhpOpExporterAnsysInputFile.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"

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
lhpOpExporterAnsysInputFile::lhpOpExporterAnsysInputFile(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;
  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
  m_AnsysInputFileName = "";
  m_CacheDir = (mafGetApplicationDirectory() + "\\Data\\AnsysWriterCache").c_str();
  m_AnsysOutputFileNameFullPath		= "";
  m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
  
  // This is for deploy: need to work on PYTHONPATH to solve issues with Python modules execution path...
  m_AnsysPythonExporterFullPathFileName = (mafGetApplicationDirectory() + "\\ASCIIParser\\ansysWriter.py").c_str();
  
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
  lhpOpExporterAnsysInputFile *cp = new lhpOpExporterAnsysInputFile(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpExporterAnsysInputFile::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
}
//----------------------------------------------------------------------------
int lhpOpExporterAnsysInputFile::Read()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Writing file: ..."));
  }

  m_NodesFileName = m_CacheDir + "\\AnsysExporterNodes.lis" ;
  m_ElementsFileName = m_CacheDir + "\\AnsysExporterElements.lis" ;
  m_MaterialsFileName = m_CacheDir + "\\AnsysExporterMaterials.lis" ;

  mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
  mafVMEMesh *input = mafVMEMesh::SafeDownCast(m_Input);
  assert(input);

  exporter->SetInput(input->GetUnstructuredGridOutput()->GetUnstructuredGridData());
  exporter->SetOutputNodesFileName(m_NodesFileName.c_str());
  exporter->SetOutputElementsFileName(m_ElementsFileName.c_str());
  exporter->SetOutputMaterialsFileName(m_MaterialsFileName.c_str());

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
  wxString command2execute;
  
  mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  // read cache files and create ansys file
  command2execute = "python.exe ";
  command2execute.Append(" \"\"");
  command2execute.Append(m_AnsysPythonExporterFullPathFileName.c_str());
  command2execute.Append(" \"");
  command2execute.Append(m_NodesFileName.c_str());
  command2execute.Append("\" \"");
  command2execute.Append(m_ElementsFileName.c_str());
  command2execute.Append("\" \"");
  command2execute.Append(m_MaterialsFileName.c_str());
  command2execute.Append("\" \"");
  command2execute.Append(m_AnsysOutputFileNameFullPath.c_str());
  command2execute.Append("\"");
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
  mafString wildc = "Stereo Litography (*.stl)|*.stl";

  m_Gui = new mmgGui(this);
  m_Gui->Label("absolute matrix",true);
  m_Gui->Bool(ID_ABS_MATRIX_TO_STL,"apply",&m_ABSMatrixFlag,0);
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
  mafString wildcard = "inp files (*.inp)|*.inp|All Files (*.*)|*.*";

  m_AnsysOutputFileNameFullPath = "";

  bool cacheDirExist = wxDirExists(m_CacheDir.GetCStr());

  if (cacheDirExist == false)
  {
    std::ostringstream stringStream;
    stringStream << "creating cache dir: " << m_CacheDir.GetCStr() << std::endl;
    mafLogMessage(stringStream.str().c_str());          
    wxMkdir(m_CacheDir.GetCStr());
  }
  else
  {
    std::ostringstream stringStream;
    stringStream << "found cache dir: " << m_CacheDir.GetCStr() << std::endl;
    mafLogMessage(stringStream.str().c_str());

  }

  wxString f;
  f = mafGetSaveFile(m_FileDir,wildcard).c_str(); 
  if(!f.IsEmpty())
  {
    m_AnsysOutputFileNameFullPath = f;
    Read();
  }
 }

//----------------------------------------------------------------------------
void lhpOpExporterAnsysInputFile::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));        
}