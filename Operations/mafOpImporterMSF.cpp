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
#include "mafStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGroup.h"
#include "mafVMEGeneric.h"
#include "mafVMEItemVTK.h"
#include "mafDataVector.h"
#include "ftk/Core/NodeManager.h"

#include "vtkSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterMSF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterMSF::mafOpImporterMSF(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_File    = _R("");
}
//----------------------------------------------------------------------------
mafOpImporterMSF::~mafOpImporterMSF()
//----------------------------------------------------------------------------
{
  mafRemoveDirectory(m_TmpDir);
  m_TmpDir = _R("");
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterMSF::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterMSF *cp = new mafOpImporterMSF(GetLabel());
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterMSF::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    mafString fileDir = _R("");//mafGetApplicationDirectory().c_str();
    mafString wildc  = _L("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
    m_File = mafGetOpenFile(fileDir, wildc, _L("Choose MSF file"));
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.empty()) 
  {
    if (ImportMSF() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
  }
  {mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}
//----------------------------------------------------------------------------
int mafOpImporterMSF::ImportMSF()
//----------------------------------------------------------------------------
{
  mafString unixname = m_File;
  mafString path, name, ext;
  mafSplitPath(m_File,&path,&name,&ext);

  if(ext == _R("zmsf"))
  {
    unixname = mafOpenZIP(m_File, mafWxToString(::wxGetCwd()), m_TmpDir);
    if(unixname.empty())
    {
      if (!m_TestMode)
        mafMessage(_M(mafString(_L("Bad or corrupted zmsf file!"))));
      return MAF_ERROR;
    }
    wxSetWorkingDirectory(mafStringToWx(m_TmpDir));
  }

  ParsePathName(unixname); // convert to unix format

  m_MSFFile = unixname; 
  model::data::NodeManager manager;
  auto storage = std::make_unique<mafStorage>();
  storage->SetManager(&manager);
  storage->SetURL(m_File);

  int res = storage->Restore();
  if (res != MAF_OK)
  {
    // if some problems occurred during import give feedback to the user
    if (!m_TestMode)
      mafErrorMessage(_M(mafString(_L("Errors during file parsing! Look the log area for error messages."))));
    //return MAF_ERROR;
  }
  auto root = mafVMERoot::SafeDownCast(manager.GetRoot());
      
  mafString group_name = _R("imported from ") + name + _R(".") + ext;

  for (auto& node : *root)
  {
    if(&node == root.get())
      continue;
    auto vmeWithDataVector = mafVMEGenericAbstract::SafeDownCast(&node);
    if (vmeWithDataVector)
    {
      mafDataVector *dataVector = vmeWithDataVector->GetDataVector();
      if(dataVector)
      {
        for(auto& elem : *dataVector)
        {
          if(auto vitem = mafVMEItemVTK::SafeDownCast(elem.second.get()))
            vitem->GetData();
        }
      }
    }
  }
 


  m_Group = mafVMEGroup::NewSPtr();
  m_Group->SetName(group_name);
  mafNode::ReparentTo(m_Group, GetInput().get());
  
  while (auto node = root->GetChild(0))
  {
    mafNode::ReparentTo(node, m_Group.get());

    // Losi 03/16/2010 Bug #2049 fix
    auto vme = mafVMEGeneric::SafeDownCast(node);
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
  SetOutput(m_Group);

  return MAF_OK;
}
