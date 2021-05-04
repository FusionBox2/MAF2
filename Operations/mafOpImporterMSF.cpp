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
#include "wx/busyinfo.h"
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/fs_zip.h>

#include "mafEvent.h"
#include "mafNodeIterator.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGroup.h"
#include "mafVMEGeneric.h"
#include "mafVMEItemVTK.h"
#include "mafDataVector.h"
#include "mafNodeManager.h"

#include "vtkMAFSmartPointer.h"

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
  m_Group   = NULL;
}
//----------------------------------------------------------------------------
mafOpImporterMSF::~mafOpImporterMSF()
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
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

	try{

		//wxBusyInfo wait40("importMSF");
		//Sleep(1500);
		mafString unixname = m_File;
		mafString path, name, ext;
		mafSplitPath(m_File, &path, &name, &ext);

		if (ext == _R("zmsf"))
		{
			unixname = mafOpenZIP(m_File, ::wxGetCwd(), m_TmpDir);
			if (unixname.IsEmpty())
			{
				if (!m_TestMode)
					mafMessage(_("Bad or corrupted zmsf file!"));
				return MAF_ERROR;
			}
			wxSetWorkingDirectory(m_TmpDir.toWx());
		}

		unixname.ParsePathName(); // convert to unix format

		m_MSFFile = unixname;
		mafVMEStorage *storage;
		mafNodeManager manager;
		storage = mafVMEStorage::New();
		storage->SetManager(&manager);
		storage->SetURL(m_File);

		int res = storage->Restore();
		if (res != MAF_OK)
		{
			// if some problems occurred during import give feedback to the user
			if (!m_TestMode)
				mafErrorMessage(_M(mafString(_L("Errors during file parsing! Look the log area for error messages."))));

			//wxBusyInfo wait4010("Errors during file parsing! Look the log area for error messages.");
			//Sleep(1500);
			//return MAF_ERROR;
		}
		mafVMERoot *root = mafVMERoot::SafeDownCast(manager.GetRoot());

		mafString group_name = _R("imported from ") + name + _R(".") + ext;

		mafNodeIterator *iter = root->NewIterator();
		for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		{

			//wxBusyInfo wait4011(node->GetName().GetCStr());
			//Sleep(1500);
			if (node == root)
				continue;
			mafVMEGenericAbstract *vmeWithDataVector = mafVMEGenericAbstract::SafeDownCast(node);
			mafString t = node->GetName() + _R(" cast ok");
			//wxBusyInfo wait411(t.toWx());
			//Sleep(1500);
			if (vmeWithDataVector)
			{
				mafDataVector *dataVector = vmeWithDataVector->GetDataVector();
				if (dataVector)
				{
					for (mafDataVector::Iterator it = dataVector->Begin(); it != dataVector->End(); ++it)
					{
						if (mafVMEItemVTK *vitem = mafVMEItemVTK::SafeDownCast(it->second))
							vitem->GetData();
					}
				}
			}
		}


		//wxBusyInfo wait4061("for loop ok");
		//Sleep(1500);
		iter->Delete();


		mafNEW(m_Group);
		m_Group->SetName(group_name);
		m_Group->ReparentTo(m_Input);

		while (mafNode *node = root->GetFirstChild())
		{

			//wxBusyInfo wait4011(node->GetName().toWx());
			//Sleep(1500);
			node->ReparentTo(m_Group);

			// Losi 03/16/2010 Bug #2049 fix
			mafVMEGeneric *vme = mafVMEGeneric::SafeDownCast(node);
			if (vme)
			{
				// Update data vector id to avoid duplicates
				mafDataVector *dataVector = vme->GetDataVector();
				if (dataVector)
				{
					dataVector->UpdateVectorId();
				}
			}
		}
		m_Group->Update();
		m_Output = m_Group;
		///wxBusyInfo wait4021("while loop ok");
		//Sleep(1500);
		mafDEL(storage);

		//wxBusyInfo wait410("importMSF done");
		//Sleep(1500);
		return MAF_OK;
	}
	catch (const std::exception& e)
	{

		wxBusyInfo wait40(e.what());
		Sleep(1500);
	}
}
