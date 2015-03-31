/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMSF1x
 Authors: Paolo Quadrani
 
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

#include "mafOpImporterMSF1x.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVMEGeneric.h"
#include "mafDataVector.h"
#include "mafVMERoot.h"
#include "mafTagArray.h"
#include "mafVMEStorage.h"
#include "mafMSFImporter.h"
#include "mafVMEGroup.h"
#include "mafFilesDirs.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterMSF1x);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterMSF1x::mafOpImporterMSF1x(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";
  m_Group   = NULL;

  m_FileDir = "";//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterMSF1x::~mafOpImporterMSF1x( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterMSF1x::Copy()   
//----------------------------------------------------------------------------
{
  //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
  mafOpImporterMSF1x *cp = new mafOpImporterMSF1x(GetLabel());
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterMSF1x::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = "MSF file (*.msf)|*.msf";
	mafString f;
  if(m_File.IsEmpty())
	{
		f = mafGetOpenFile(m_FileDir, wildc.GetCStr(), "Choose MSF file");
		m_File = f;
	}

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
	{
    wxSetWorkingDirectory(wxString(m_File.GetCStr()));
    ImportMSF();
	  result = OP_RUN_OK;
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpImporterMSF1x::ImportMSF()
//----------------------------------------------------------------------------
{
  bool success = false;
//	wxBusyInfo wait("Loading file: ...");
  
  mafVMEStorage *storage = mafVMEStorage::New();
  mafMSFImporter manager;
  storage->SetManager(&manager);
  storage->SetURL(m_File);
  
  //mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,preader));

  success = (storage->Restore() == MAF_OK);

  if(!success)
    mafErrorMessage("I/O Error importing MSF file.");

  mafVMERoot *root = mafVMERoot::SafeDownCast(manager.GetRoot());

  mafString path, name, ext;
  mafSplitPath(m_File.GetCStr(),&path,&name,&ext);
  mafString group_name = wxString::Format("imported from %s.%s",name.GetCStr(),ext.GetCStr()).c_str();

  mafNEW(m_Group);
  m_Group->SetName(group_name);
  m_Group->ReparentTo(m_Input);

  while (mafNode *node = root->GetFirstChild())
  {
    node->ReparentTo(m_Group);

    // Losi 03/16/2010 Bug #2049 fix
    mafVMEGeneric *vme = mafVMEGeneric::SafeDownCast(node);
    if(vme)
    {
      // Update data vector id to avoid duplicates
      mafDataVector *dataVector = vme->GetDataVector();
      if(dataVector)
      {
        dataVector->UpdateVectorId();
      }
    }
  }
  m_Group->Update();
  m_Output = m_Group;

  mafDEL(storage);
}
