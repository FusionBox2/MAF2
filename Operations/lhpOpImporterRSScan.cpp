/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterRSScan.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.2 $
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

#include "lhpOpImporterRSScan.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"

#include "lhpVMEMeshRSScanImporter.h"

#include "vtkSmartPointer.h"

#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpImporterRSScan);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpImporterRSScan::lhpOpImporterRSScan(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImportedVmeMesh = NULL;
  m_PlateParamsFileName = _R("");
  m_DataFileName = _R("");
  m_Scale        = 1.0;
  m_TimeShift    = 0.0;
}

//----------------------------------------------------------------------------
lhpOpImporterRSScan::~lhpOpImporterRSScan()
//----------------------------------------------------------------------------
{
  mafDEL(m_ImportedVmeMesh);
}
//----------------------------------------------------------------------------
bool lhpOpImporterRSScan::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(node);
  if(cloud == NULL)
    return false;
  if(cloud->FindLandmarkIndex(_R("SNS1")) == -1 || cloud->FindLandmarkIndex(_R("SNS2")) == -1 || cloud->FindLandmarkIndex(_R("SNS3")) == -1)
    return false;
  return true;
}
//----------------------------------------------------------------------------
mafOp* lhpOpImporterRSScan::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpImporterRSScan *cp = new lhpOpImporterRSScan(GetLabel());
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpImporterRSScan::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
  ShowGui();
}
//----------------------------------------------------------------------------
int lhpOpImporterRSScan::Read()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Loading file: ..."));
  }

  lhpVMEMeshRSScanImporter *reader = new lhpVMEMeshRSScanImporter;
  reader->SetPlateParamsFileName(m_PlateParamsFileName.GetCStr());
  reader->SetDataFileName(m_DataFileName.GetCStr());
  reader->SetScale(m_Scale);
  reader->SetTimeShift(m_TimeShift);
  reader->SetSensors(mafVMELandmarkCloud::SafeDownCast(m_Input));
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
    m_ImportedVmeMesh = reader->GetOutput();
    m_ImportedVmeMesh->Register(this);
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
  ID_IMPORTER_TYPE,
  ID_PLATE_PARAMS_FILE_NAME,
  ID_DATA_FILE_NAME,
  ID_SCALE,
  ID_TIMESHIFT,
  ID_OK,
  ID_CANCEL,
};
//----------------------------------------------------------------------------
void lhpOpImporterRSScan::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildcardlis = _R("lis files (*.lis)|*.lis|All Files (*.*)|*.*");
  mafString wildcardxls = _R("xls files (*.xls)|*.xls|All Files (*.*)|*.*");

  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Label(_L("Plate params file:"), true);
  m_Gui->FileOpen (ID_PLATE_PARAMS_FILE_NAME, _R(""), &m_PlateParamsFileName, wildcardlis);
  m_Gui->Divider();
 
  m_Gui->Label(_L("Data file:"), true);
  m_Gui->FileOpen (ID_DATA_FILE_NAME, _R(""), &m_DataFileName, wildcardxls);
  m_Gui->Divider();
  m_Gui->Double(ID_SCALE,_L("scale"),&m_Scale, 0.0, 1000.0);
  m_Gui->Double(ID_TIMESHIFT,_L("time shift"),&m_TimeShift);

  m_Gui->Divider(2);
  m_Gui->Divider();
  m_Gui->OkCancel();

  m_Gui->Update();
}
//----------------------------------------------------------------------------
void lhpOpImporterRSScan::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_IMPORTER_TYPE:
      break;
      case ID_SCALE:
      case ID_TIMESHIFT:
        break;
      case ID_PLATE_PARAMS_FILE_NAME:
        // this->SetNodesFileName(m_NodesFileName.GetCStr());       
      break;
      case ID_DATA_FILE_NAME:
        // this->SetElementsFileName(m_ElementsFileName.GetCStr());
      break;
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
