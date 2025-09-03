#include "mafOpSelect.h"
#include "mafNode.h"
#include "mafVMERoot.h"
#include "ftk/Base/String.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"
#ifdef MAF_USE_VTK
  #include "mafVMELandmarkCloud.h"
#endif
//#include "vtkMatrix4x4.h"
//#include "vtkMath.h"
#include "vtkDataSet.h"

//initialize the Clipboard
// mafAutoPointer<mafNode>  mafOpEdit::m_Clipboard(NULL);

static std::shared_ptr<mafNode> m_Clipboard;
int  mafOpEdit::m_NumOperations(0); 

//////////////////
// mafOpSelect ://
//////////////////
//----------------------------------------------------------------------------
mafOpSelect::mafOpSelect(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true; 
  m_OpType  = OPTYPE_EDIT; 
}
//----------------------------------------------------------------------------
mafOpSelect::~mafOpSelect()
//----------------------------------------------------------------------------
{
} 
//----------------------------------------------------------------------------
mafOp* mafOpSelect::Copy() 
//----------------------------------------------------------------------------
{
  mafOpSelect *cp  = new mafOpSelect();
  cp->m_OldNodeSelected  = m_OldNodeSelected;
  cp->m_NewNodeSelected  = m_NewNodeSelected;
  return cp;
}
//----------------------------------------------------------------------------
bool mafOpSelect::Accept(mafNode* vme)     
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
void mafOpSelect::SetNewSel(std::shared_ptr<mafNode> vme)
//----------------------------------------------------------------------------
{
  m_NewNodeSelected = vme;
}
//----------------------------------------------------------------------------
void mafOpSelect::OpDo()
//----------------------------------------------------------------------------
{
  if (m_OldNodeSelected == nullptr)
    m_OldNodeSelected = GetInput();
  {mafEvent evUnq(this,VME_SELECTED); evUnq.SetVme(m_NewNodeSelected.get()); InvokeEvent(evUnq);}
};
//----------------------------------------------------------------------------
void mafOpSelect::OpUndo()
//----------------------------------------------------------------------------
{
  {mafEvent evUnq(this,VME_SELECTED); evUnq.SetVme(m_OldNodeSelected.get()); InvokeEvent(evUnq);}
};


////////////////
// mafOpEdit: //
////////////////
//----------------------------------------------------------------------------
mafOpEdit::mafOpEdit(const mafString& label): Superclass(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true; 
  m_OpType = OPTYPE_EDIT; 
  m_NumOperations++;
}
//----------------------------------------------------------------------------
mafOpEdit::~mafOpEdit()
//----------------------------------------------------------------------------
{
  m_NumOperations--;
  if(m_NumOperations == 0)
    ClipboardClear();
} 
//----------------------------------------------------------------------------
bool mafOpEdit::ClipboardIsEmpty()
//----------------------------------------------------------------------------
{
  return GetClipboard() == nullptr;
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardClear()
//----------------------------------------------------------------------------
{
  SetClipboard(nullptr);
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardBackup()
//----------------------------------------------------------------------------
{
  assert(!m_Backup);
  m_Backup = GetClipboard();
  SetClipboard(nullptr);
}
//----------------------------------------------------------------------------
void mafOpEdit::ClipboardRestore()
//----------------------------------------------------------------------------
{
  //assert(m_Backup.GetPointer() ); - //SIL. 6-11-2003: assert removed, I may make a backup of an empy clipboard
  SetClipboard(m_Backup);
  m_Backup = nullptr;
}
//----------------------------------------------------------------------------
std::shared_ptr<mafNode> mafOpEdit::GetClipboard()
//----------------------------------------------------------------------------
{
  return m_Clipboard;
}
//----------------------------------------------------------------------------
void mafOpEdit::SetClipboard(std::shared_ptr<mafNode> node)
//----------------------------------------------------------------------------
{
  m_Clipboard = node;
}
//----------------------------------------------------------------------------
void mafOpEdit::OpRun()
//----------------------------------------------------------------------------
{
  if (m_Selection == nullptr)
    m_Selection = GetInput();
  OpStop(OP_RUN_OK);
}

///////////////
// mafOpCut ://
///////////////
//----------------------------------------------------------------------------
mafOpCut::mafOpCut(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_SelectionParent = nullptr; 
}
//----------------------------------------------------------------------------
mafOpCut::~mafOpCut() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCut::Copy() 
//----------------------------------------------------------------------------
{
  return new mafOpCut(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCut::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return (vme && !vme->IsMAFType(mafVMERoot));
}
//----------------------------------------------------------------------------
void mafOpCut::OpDo()
//----------------------------------------------------------------------------
/**
backup the clipboard
Send a VME_REMOVE for the selected vme
Move (doesn't make a copy) the selected vme (and it's subtree) in the Clipboard
Select the vme parent
*/
{
  ClipboardBackup();
  m_SelectionParent = m_Selection->GetParent();
  SetClipboard(m_Selection);

  //////////////////////////////////////////////////////////////////////////
  // It is necessary load all vtk data of the vme time varying otherwise paste or undo cause an application crash
  //////////////////////////////////////////////////////////////////////////
  LoadVTKData(m_Selection.get());

  //////////////////////////////////////////////////////////////////////////
  // Added by Losi on 03.06.2010, modify by Di Cosmo
  // It is necessary to load all vtk data of children vme otherwise paste or undo cause an application crash
  //////////////////////////////////////////////////////////////////////////
  mafVME *m_SelectionVme = mafVME::SafeDownCast(m_Selection.get());
  if(m_SelectionVme)
    LoadChild(m_SelectionVme);
  //////////////////////////////////////////////////////////////////////////

  {mafEvent evUnq(this,VME_REMOVE); evUnq.SetVme(m_Selection.get()); InvokeEvent(evUnq);}
  {mafEvent evUnq(this,VME_SELECTED); evUnq.SetVme(m_SelectionParent); InvokeEvent(evUnq);}
  if (mafVME::SafeDownCast(m_SelectionParent))
  {
    mafVME::StaticDownCast(m_SelectionParent)->GetOutput()->Update();
  }
}
//----------------------------------------------------------------------------
void mafOpCut::LoadVTKData(mafNode *vme)
//----------------------------------------------------------------------------
{
  // Added by Losi on 03.06.2010
  if (mafVME::SafeDownCast(vme))
  {
    mafTimeStamp oldTime = mafVME::SafeDownCast(vme)->GetTimeStamp();

    std::vector<mafTimeStamp> subKFrames;
    mafVME::SafeDownCast(vme)->GetTimeStamps(subKFrames);
    for (int i=0;i<subKFrames.size();i++)
    {
      mafVME::SafeDownCast(vme)->SetTimeStamp(subKFrames[i]);
      mafVME::SafeDownCast(vme)->GetOutput()->GetVTKData();
    }

    mafVME::SafeDownCast(vme)->SetTimeStamp(oldTime);
  }
}
//----------------------------------------------------------------------------
void mafOpCut::LoadChild(mafNode *vme)
//----------------------------------------------------------------------------
{
  for(int c = 0; c < vme->GetNumberOfChildren(); c++)
  {
    auto child = vme->GetChild(c).get();
    LoadVTKData(child);
    LoadChild(child);
  }
}
//----------------------------------------------------------------------------
void mafOpCut::OpUndo()
//----------------------------------------------------------------------------
/**
Move the vme in the Clipboard under it's old parent
send a VME_ADD
Restore the Clipboard
Restore the Selection
*/
{
  m_Selection = GetClipboard();

#ifdef MAF_USE_VTK
  if (m_SelectionParent->IsMAFType(mafVMELandmarkCloud) && !((mafVMELandmarkCloud *)m_SelectionParent)->IsOpen())
  {
    mafVMELandmarkCloud::StaticDownCast(m_SelectionParent)->Open();
    mafNode::ReparentTo(m_Selection, m_SelectionParent);
    mafVMELandmarkCloud::StaticDownCast(m_SelectionParent)->Close();
  }
  else
  {
    mafNode::ReparentTo(m_Selection, m_SelectionParent);
  }
#else
    m_Selection->ReparentTo(m_SelectionParent);
#endif

  if (mafVME::SafeDownCast(m_SelectionParent))
  {
    mafVME::StaticDownCast(m_SelectionParent)->GetOutput()->Update();
  }
  {mafEvent evUnq(this,VME_SELECTED); evUnq.SetVme(m_Selection.get()); InvokeEvent(evUnq);}
  ClipboardRestore();
}



//////////////////
// mafOpDelete ://
//////////////////
//----------------------------------------------------------------------------
mafOpDelete::mafOpDelete(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_Canundo         = false;
}
//----------------------------------------------------------------------------
mafOpDelete::~mafOpDelete() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpDelete::Copy() 
//----------------------------------------------------------------------------
{
  return new mafOpDelete(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpDelete::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return (vme && !vme->IsMAFType(mafVMERoot));
}
//----------------------------------------------------------------------------
void mafOpDelete::OpDo()
//----------------------------------------------------------------------------
/**
backup the clipboard
Send a VME_REMOVE for the selected vme
Move (doesn't make a copy) the selected vme (and it's subtree) in the Clipboard
Select the vme parent
*/
{
  // do not remove binary files but fill a list with files to be deleted on save by the storage.
  mafEventIO e(this,NODE_GET_STORAGE);
  m_Selection->ForwardUpEvent(e);
  mafStorage *storage = e.GetStorage();
  mafString data_filename;
  for (auto& node : *m_Selection)
  {
    if(mafVMEGenericAbstract::SafeDownCast(&node))
    {
      auto vme = mafVMEGenericAbstract::SafeDownCast(&node);
      if (mafDataVector *dv = vme->GetDataVector())
      {
        if (dv->GetSingleFileMode())
        {
          mafString archive_filename = dv->GetArchiveName();
          if (!archive_filename.empty())
          {
            storage->ReleaseURL(archive_filename);
          }
        }
        else
        {
          mafVMEItem *item;
          for (auto& entry : *dv)
          {
            item = entry.second.get();
            data_filename = _R(item->GetURL());
            storage->ReleaseURL(data_filename);
          }
        }
      }
    }
  }
  m_SelectionParent = m_Selection->GetParent(); 
  {mafEvent evUnq(this,VME_REMOVE); evUnq.SetVme(m_Selection.get()); InvokeEvent(evUnq);}
  {mafEvent evUnq(this,VME_SELECTED); evUnq.SetVme(m_SelectionParent); InvokeEvent(evUnq);}
}
//----------------------------------------------------------------------------
void mafOpDelete::OpUndo()
//----------------------------------------------------------------------------
{
}



////////////////
// mafOpCopy ://
////////////////
//----------------------------------------------------------------------------
mafOpCopy::mafOpCopy(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOpCopy::~mafOpCopy()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCopy::Copy()
//----------------------------------------------------------------------------
{
  return new mafOpCopy(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCopy::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  bool res = vme && !vme->IsMAFType(mafVMERoot);
  if(GetClipboard())
    res = res && GetClipboard()->CanCopy(vme);
  return res;
}
//----------------------------------------------------------------------------
void mafOpCopy::OpDo()
//----------------------------------------------------------------------------
/**
make the clipboard backup
copy the selected VME and its subtree into the clipboard
*/
{
  ClipboardBackup();
  SetClipboard(m_Selection->CopyTree());
  mafString copy_name;
  copy_name = _R("copy of ");
  copy_name += GetClipboard()->GetName();
  GetClipboard()->SetName(copy_name);
}
//----------------------------------------------------------------------------
void mafOpCopy::OpUndo()
//----------------------------------------------------------------------------
/**
destroy current clipboard
restore previous clipboard
*/
{
  ClipboardRestore();
}



/////////////////
// mafOpPaste ://
/////////////////
//----------------------------------------------------------------------------
mafOpPaste::mafOpPaste(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpPaste::Copy() 
//----------------------------------------------------------------------------
{                    
  return new mafOpPaste(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpPaste::Accept(mafNode* vme)       
//----------------------------------------------------------------------------
{
  //paste may be executed if
  // - vme is not NULL
  // - the clipboard is not empty
  // - THE vme on the clipboard can be re-parented under vme
  // - (this cover also restrictions imposed on Landmarks)

  if(ClipboardIsEmpty()) return false;
  if(vme == nullptr) return false;
  auto cv = GetClipboard();
  return cv->CanReparentTo(vme);
};
//----------------------------------------------------------------------------
void mafOpPaste::OpDo()                    
//----------------------------------------------------------------------------
/**
We want that Cut+Paste doesn't change the identity of a vme,so the implementation
of Paste is a little different than usual.

Paste make a copy of the object on the clipboard,
but place in the scene the original and keep the copy in the clipboard.

Them a VME_ADD is sent, selection is not changed
*/
{
  m_PastedVme = GetClipboard(); 
  mafNode::ReparentTo(m_PastedVme, m_Selection.get());
  SetClipboard(m_PastedVme->CopyTree());
}
//----------------------------------------------------------------------------
void mafOpPaste::OpUndo()                  
//----------------------------------------------------------------------------
/**
Remove the pasted vme from the scene and place it in the clipboard.
The copy in the clipboard will be automatically deleted
*/
{
  SetClipboard(m_PastedVme);
  {mafEvent evUnq(this,VME_REMOVE); evUnq.SetVme(m_PastedVme.get()); InvokeEvent(evUnq);}
}
