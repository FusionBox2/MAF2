/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMSF
 Authors: Roberto Mucci
 
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

#include "mafOpImporterMSF.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/fs_zip.h>

#include "mafEvent.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGroup.h"
#include "mafVMEGeneric.h"
#include "mafDataVector.h"

#include "vtkMAFSmartPointer.h"
#include "mafFilesDirs.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterMSF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterMSF::mafOpImporterMSF(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Group   = NULL;
}
//----------------------------------------------------------------------------
mafOpImporterMSF::~mafOpImporterMSF()
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
  mafRemoveDirectory(m_TmpDir);
  m_TmpDir = "";
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterMSF::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterMSF *cp = new mafOpImporterMSF(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterMSF::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    mafString fileDir = "";//mafGetApplicationDirectory().c_str();
    mafString wildc  = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
    m_File = mafGetOpenFile(fileDir, wildc, _("Choose MSF file"));
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty()) 
  {
    if (ImportMSF() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
  }
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
int mafOpImporterMSF::ImportMSF()
//----------------------------------------------------------------------------
{
  mafString unixname = m_File;
  wxString path, name, ext;
  wxSplitPath(m_File.GetCStr(),&path,&name,&ext);

  if(ext == "zmsf")
  {
    unixname = mafOpenZIP(m_File, ::wxGetCwd(), m_TmpDir);
    if(unixname.IsEmpty())
    {
      if (!m_TestMode)
        mafMessage(_("Bad or corrupted zmsf file!"));
      return MAF_ERROR;
    }
    wxSetWorkingDirectory(m_TmpDir.GetCStr());
  }

  mafString sub_unixname;
  if (unixname.StartsWith("\\\\"))
  {
    sub_unixname = unixname;
    sub_unixname.Erase(0,1);
    sub_unixname.ParsePathName();
    unixname = "\\\\";
    unixname += sub_unixname;
  }
  else
    unixname.ParsePathName(); // convert to unix format

  m_MSFFile = unixname; 
  mafVMEStorage *storage;
  storage = mafVMEStorage::New();
  storage->SetURL(m_MSFFile.GetCStr());
  mafVMERoot *root;
  root = storage->GetRoot();
  root->Initialize();
  root->SetName("RootB");

  int res = storage->Restore();
  if (res != MAF_OK)
  {
    // if some problems occurred during import give feedback to the user
    if (!m_TestMode)
      mafErrorMessage(_("Errors during file parsing! Look the log area for error messages."));
    //return MAF_ERROR;
  }
      
  wxString group_name = wxString::Format("imported from %s.%s",name,ext);
 
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
  return MAF_OK;
}
