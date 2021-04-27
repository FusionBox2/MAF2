/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVMEDataSetAttributes
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpImporterVMEDataSetAttributes.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMEDataSetAttributesImporter.h"
#include "mafVMEGeneric.h"

#include "mafGUI.h"

#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterVMEDataSetAttributes);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterVMEDataSetAttributes::mafOpImporterVMEDataSetAttributes(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  
  m_FileName = _R("");
  m_FilePrefix = _R("");
  m_TimeVarying = 0;
  m_TSFileName = _R("");
  m_UseTSFile = 0;
  m_AttributeType = mafVMEDataSetAttributesImporter::POINT_DATA;
  
  m_InputPreserving = false;
  m_Input = NULL;

  m_UseIdArray = false;
  m_IdArrayName = _R("ANSYS_ELEMENT_ID");
}

//----------------------------------------------------------------------------
mafOpImporterVMEDataSetAttributes::~mafOpImporterVMEDataSetAttributes( ) 
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
bool mafOpImporterVMEDataSetAttributes::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterVMEDataSetAttributes::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterVMEDataSetAttributes *cp = new mafOpImporterVMEDataSetAttributes(GetLabel());
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterVMEDataSetAttributes::OpRun()   
{
  CreateGui();
  ShowGui();
}


int mafOpImporterVMEDataSetAttributes::Read()
{
  
  if (!m_TestMode)
  {
    wxBusyInfo wait("Reading file: ...");  
  }

  mafVMEGeneric *input = mafVMEGeneric::SafeDownCast(m_Input);

  mafVMEDataSetAttributesImporter* attributesImporter = NULL;
  
  attributesImporter = new mafVMEDataSetAttributesImporter;
  attributesImporter->SetInput(input);
	attributesImporter->SetFileName(m_FileName.GetCStr());
  attributesImporter->SetTimeVaryingFlag(m_TimeVarying == 1 ? true : false);
  attributesImporter->SetUseTSFile(m_UseTSFile ? true : false);
  attributesImporter->SetTSFileName(m_TSFileName.GetCStr());
  attributesImporter->SetFilePrefix(m_FilePrefix.GetCStr());
  attributesImporter->SetAttributeType(m_AttributeType);
  attributesImporter->SetUseIdArrayFlag(m_UseIdArray);
  attributesImporter->SetIdArrayName(m_IdArrayName.GetCStr());

  int returnValue = attributesImporter->Read();

  if (returnValue == MAF_ERROR)
  {
    if (!m_TestMode)
    {
        mafErrorMessage(_M(mafString(_L("Error parsing input files! See log window for details..."))));
    }
  } 
  else
  {
    m_Output = attributesImporter->GetOutput();
  }

  cppDEL(attributesImporter);

  return returnValue;
}

void mafOpImporterVMEDataSetAttributes::OpStop(int result)
{
  if (result == OP_RUN_OK)
  {
    this->Read();
  } 
  
  HideGui();

	mafEventMacro(mafEvent(this,result));  	   
}

enum Mesh_Importer_ID
{
  ID_FIRST = MINID,
  ID_AttributeType,
  ID_FileName,
  ID_FilePrefix,
  ID_UseTSFile,
  ID_TSFileName,
  ID_TimeVarying,
  ID_UseIdArray,
  ID_IdArrayName,
  ID_OK,
  ID_CANCEL,
  ID_HELP,
};

void mafOpImporterVMEDataSetAttributes::CreateGui()
{
  mafString wildcard = _R("txt files (*.txt)|*.txt|All Files (*.*)|*.*");

  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
 
  mafEvent buildHelpGui;
  buildHelpGui.SetSender(this);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  mafEventMacro(buildHelpGui);

  if (buildHelpGui.GetArg() == true)
  {
	  m_Gui->Button(ID_HELP, _R("Help"), _R(""));
  }

  m_Gui->Label(_R(" "));
  m_Gui->Label(_R("attributes file:"),true);
  m_Gui->FileOpen (ID_FileName, _R(""),	&m_FileName, wildcard);
  
  m_Gui->Label(_R("time varying"));
  m_Gui->Bool(ID_TimeVarying, _R(""),&m_TimeVarying);
  m_Gui->Divider();

  m_Gui->Label(_R("file prefix:"),true);
  m_Gui->String(ID_FilePrefix, _R(""),	&m_FilePrefix, wildcard);
  m_Gui->Divider();

  m_Gui->Bool(ID_UseTSFile,_R("use TS file:"),&m_UseTSFile);
  m_Gui->Divider();

  m_Gui->Label(_R("time stamps file:"),true);
  m_Gui->FileOpen (ID_TSFileName, _R(""),	&m_TSFileName, wildcard);
  m_Gui->Divider();

  mafString attributeType[2]={_R("point data"),_R("cell data")};
  m_Gui->Label(_R("Importer type"),true);
  m_Gui->Combo(ID_AttributeType, _R(""),&m_AttributeType,2,attributeType);
  m_Gui->Divider(2);

  m_Gui->Label(_R("use id map array"));
  m_Gui->Bool(ID_UseIdArray, _R(""),&m_UseIdArray);
  m_Gui->Divider();
  m_Gui->Label(_R("id map array name:"),true);
  m_Gui->String(ID_IdArrayName, _R(""),	&m_IdArrayName);
  m_Gui->Enable(ID_IdArrayName, m_UseIdArray ? true : false);
  m_Gui->Divider();

  m_Gui->Divider();
  m_Gui->OkCancel();

  m_Gui->Update();

  m_TimeVarying ? EnableTimeVaryingGui(true) : EnableTimeVaryingGui(false);
}
  

void mafOpImporterVMEDataSetAttributes::EnableTimeVaryingGui( bool enable )
{
  m_Gui->Enable(ID_FilePrefix, enable);
  m_Gui->Enable(ID_UseTSFile, enable);
  m_Gui->Enable(ID_TSFileName, enable && m_UseTSFile);
}
void mafOpImporterVMEDataSetAttributes::OnEvent(mafEventBase *maf_event) 
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {

	case ID_HELP:
	{
		mafEvent helpEvent;
		helpEvent.SetSender(this);
		mafString operationLabel = GetLabel();
		helpEvent.SetString(&operationLabel);
		helpEvent.SetId(OPEN_HELP_PAGE);
		mafEventMacro(helpEvent);
	}
	break;

    case ID_FileName:
    break;

    case ID_FilePrefix:
    break;

    case ID_TSFileName:
    break;

    case ID_TimeVarying:
      m_TimeVarying ? EnableTimeVaryingGui(true) : EnableTimeVaryingGui(false);
    break;

    case ID_UseTSFile:
      m_Gui->Enable(ID_TSFileName,m_UseTSFile ? true : false);
    break;
    
    case ID_UseIdArray:
      m_Gui->Enable(ID_IdArrayName, m_UseIdArray ? true : false);
    break;

    case wxOK:
    {
      this->OpStop(OP_RUN_OK);

      //WORKAROUND CODE 
      //referred to bug 933
      if (m_Output != NULL)
      {
        mafEventMacro(mafEvent(this,VME_SHOW, m_Output, false));
        mafEventMacro(mafEvent(this,VME_SHOW, m_Output, true));
      }
      //END WORKAROUND CODE 

      return;
    }
    break;

    case wxCANCEL:
    {
      this->OpStop(OP_RUN_CANCEL);
      return;
    }
    break;
    
    default:
      mafEventMacro(*e);
    break;
    }	
  }
}

void mafOpImporterVMEDataSetAttributes::SetFileName( const char *filename )
{
	m_FileName = _R(filename);
}

void mafOpImporterVMEDataSetAttributes::SetFilePrefix( const char *filePrefix )
{
	m_FilePrefix = _R(filePrefix);
}

void mafOpImporterVMEDataSetAttributes::SetTSFileName( const char *tsFileName )
{
	m_TSFileName = _R(tsFileName);
}

void mafOpImporterVMEDataSetAttributes::SetUseTSFile( bool useTSFile )
{
  m_UseTSFile = useTSFile;
}

void mafOpImporterVMEDataSetAttributes::SetDataTypeToPointData()
{
  m_AttributeType = mafVMEDataSetAttributesImporter::POINT_DATA;
}

void mafOpImporterVMEDataSetAttributes::SetDataTypeToCellData()
{
  m_AttributeType = mafVMEDataSetAttributesImporter::CELL_DATA;	
}

void mafOpImporterVMEDataSetAttributes::TimeVaryingOn()
{
  m_TimeVarying = true	;
}

void mafOpImporterVMEDataSetAttributes::TimeVaryingOff()
{
	m_TimeVarying = false;
}
