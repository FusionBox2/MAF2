/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMSF
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

#include "mafOpExporterMSF.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"

#include "mafNode.h"
#include "mafVME.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafNodeIterator.h"
#include "mafNodeManager.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterMSF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterMSF::mafOpExporterMSF(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;

  m_MSFFile    = _R("");
	m_MSFFileDir = _R("");//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpExporterMSF::~mafOpExporterMSF()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpExporterMSF::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = _R("MAF Storage Format (*.msf)|*.msf");
	mafString f;
  if (m_MSFFile.IsEmpty())
  {
    f = mafGetSaveFile(m_MSFFileDir,wildc); 
    m_MSFFile = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_MSFFile.IsEmpty()) 
	{
    if (ExportMSF() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
bool mafOpExporterMSF::Accept(mafNode *vme)
//----------------------------------------------------------------------------
{
  return (vme != NULL) && (!vme->IsA("mafVMERoot"));
}
//----------------------------------------------------------------------------
int mafOpExporterMSF::ExportMSF()
//----------------------------------------------------------------------------
{					
  if (!m_TestMode)
  {
  	wxBusyInfo wait("Saving MSF: Please wait");
  }
  assert(!m_MSFFile.IsEmpty());

	if(!mafFileExists(m_MSFFile))
	{
		mafString path, name, ext, dir2;
		mafSplitPath(m_MSFFile,&path,&name,&ext);
		dir2 = path + _R("\\") + name;
		if(!mafDirExists(dir2))
			mafDirMake(dir2);
		m_MSFFile = dir2 + _R("\\") + name + _R(".") + ext;
	}

  mafNodeManager manager;
  mafVMEStorage storage;
  storage.SetManager(&manager);
  storage.SetURL(m_MSFFile);
  mafVMERoot *root;
  mafNEW(root);
  root->SetName(_R("root"));
  root->Initialize();
  manager.SetRoot(root);

  std::vector<idValues> values;

  mafNodeIterator *iter = m_Input->NewIterator();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    idValues value;
    value.oldID = node;//->GetId();
    values.push_back(value);
  }
  iter->Delete();
//  mafVME *parent = (mafVME *)m_Input->GetParent();
//  m_Input->ReparentTo(storage.GetRoot());
  mafNode::CopyTree(m_Input,root);

  iter = root->GetFirstChild()->NewIterator();
  int index = 0;
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    //idValues value;
    values[index].newID = node;//->GetId();

    index++;

    
  }
  iter->Delete();

  std::vector<mafString> linkToEliminate;
  iter = root->GetFirstChild()->NewIterator();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    linkToEliminate.clear();

    for (mafNode::mafLinksMap::iterator it=node->GetLinks()->begin();it!=node->GetLinks()->end();it++)
    {
      bool foundID = false;
      for (int i=0;i<values.size();i++)
      {
        //int id = it->second.m_NodeId;
        if (it->second.m_Node/*Id*/ == values[i].oldID)
        {
          it->second.m_Node/*Id*/ = values[i].newID;
          foundID = true;
        }
      }

      if (!foundID)
      {
        linkToEliminate.push_back(it->first);
      }
    }

    for (int i=0;i<linkToEliminate.size();i++)
    {
      node->RemoveLink(linkToEliminate[i]);
    }
  }
  iter->Delete();

//   mafNode *n = m_Input->CopyTree();
//   n->Register(NULL);
//   n->ReparentTo(storage.GetRoot());
	//mafNode::CopyTree(m_Input, storage.GetRoot());
  ((mafVME *)root->GetFirstChild())->SetAbsMatrix(*((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());  //Paolo 5-5-2004
  mafDEL(root);
  if (storage.Store() != MAF_OK)
  {
    if (!m_TestMode)
    {
    	wxMessageBox("Error while exporting MSF");
    }
    return MAF_ERROR;
  }
  return MAF_OK;
//  m_Input->ReparentTo(parent);
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterMSF::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExporterMSF *cp= new mafOpExporterMSF(GetLabel());
  cp->SetListener(GetListener());
  cp->m_MSFFile     = m_MSFFile;
  cp->m_MSFFileDir  = m_MSFFileDir;
  return cp;
}
