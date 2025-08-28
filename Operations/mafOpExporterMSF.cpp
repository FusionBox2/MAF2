#include "mafOpExporterMSF.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"

#include "mafNode.h"
#include "mafVME.h"
#include "mafStorage.h"
#include "mafVMERoot.h"
#include "mafNodeIterator.h"
#include "ftk/Core/NodeManager.h"

#include <vector>

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
  if (m_MSFFile.empty())
  {
    f = mafGetSaveFile(m_MSFFileDir,wildc); 
    m_MSFFile = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_MSFFile.empty()) 
	{
    if (ExportMSF() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
	}
	{mafEvent evUnq(this,result); InvokeEvent(evUnq);}
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
  assert(!m_MSFFile.empty());

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
  mafStorage storage;
  storage.SetManager(&manager);
  storage.SetURL(m_MSFFile);
  auto root = mafVMERoot::NewSPtr();
  root->SetName(_R("root"));
  root->Initialize();
  manager.SetRoot(root);

  std::vector<idValues> values;

  auto iter = std::make_unique<mafNodeIterator>(GetInput().get());
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    idValues value;
    value.oldID = node;//->GetId();
    values.push_back(value);
  }
  iter.reset();
//  mafVME *parent = (mafVME *)GetInput()->GetParent();
//  GetInput()->ReparentTo(storage.GetRoot());
  mafNode::CopyTree(GetInput().get(),root.get());

  iter = std::make_unique<mafNodeIterator>(root->GetFirstChild().get());
  int index = 0;
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    //idValues value;
    values[index].newID = node;//->GetId();

    index++;

    
  }
  iter.reset();

  std::vector<mafString> linkToEliminate;
  iter = std::make_unique<mafNodeIterator>(root->GetFirstChild().get());
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    linkToEliminate.clear();

    for (auto it=node->GetLinks()->begin();it!=node->GetLinks()->end();++it)
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
  iter.reset();

//   mafNode *n = GetInput()->CopyTree();
//   n->Register(NULL);
//   n->ReparentTo(storage.GetRoot());
	//mafNode::CopyTree(GetInput(), storage.GetRoot());
  mafVME::StaticDownCast(root->GetFirstChild())->SetAbsMatrix(*mafVME::StaticDownCast(GetInput())->GetOutput()->GetAbsMatrix());  //Paolo 5-5-2004
  root.reset();
  if (storage.Store() != MAF_OK)
  {
    if (!m_TestMode)
    {
    	wxMessageBox("Error while exporting MSF");
    }
    return MAF_ERROR;
  }
  return MAF_OK;
//  GetInput()->ReparentTo(parent);
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
