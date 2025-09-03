#pragma once

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafOp.h"
#include "ftk/Base/Object.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class vtkMatrix4x4;

/**
    class name: mafOpSelect
    Operation for the selection of a vme.
*/
class MAF_EXPORT mafOpSelect: public mafOp
{
public:
  mafTypeMacro(mafOpSelect, mafOp)
    /** Constructor. */
    mafOpSelect(const mafString& label=_L("Select"));
    /** Destructor. */
   ~mafOpSelect() override; 
    /** check if node can be input of the operation. */
    bool Accept(mafNode* vme) override;
    /** selection of another node. */
    void SetNewSel(std::shared_ptr<mafNode> vme);
    /** retrieve new selected node. */
    std::shared_ptr<mafNode> GetNewSel(){return m_NewNodeSelected;}
    /** execute the operation.  */
    void OpDo() override;
    /** undo the operation. */
    void OpUndo() override;
    /** return a instance of current object. */
    mafOp* Copy() override;
protected:
  std::shared_ptr<mafNode> m_OldNodeSelected;
  std::shared_ptr<mafNode> m_NewNodeSelected;
};
/**
    class name: mafOpEdit
    Interface operation for cut, copy, and paste operation.
*/
class MAF_EXPORT mafOpEdit: public mafOp
{
public:
  mafTypeMacro(mafOpEdit, mafOp)
    /** Constructor. */
    mafOpEdit(const mafString& label=_R(""));
    /** Destructor. */
    ~mafOpEdit() override; 
    /** Builds operation's interface. */
    void OpRun() override;
    /** check if the clipboard is empty.*/
    bool     ClipboardIsEmpty();
    /** clear the clipboard. */
		void     ClipboardClear();
    /** store clipboard for backup  */
		void     ClipboardBackup();
    /** restore clipboard from backup */
		void     ClipboardRestore();
    /** return the mafNode that is in the clipboard */
    std::shared_ptr<mafNode> GetClipboard();
    /** set the clipboard */
    void SetClipboard(std::shared_ptr<mafNode> node);
protected:
  // static   std::shared_ptr<mafNode> m_Clipboard;
  static   int m_NumOperations;
  std::shared_ptr<mafNode> m_Backup;
  std::shared_ptr<mafNode> m_Selection;
};
/**
    class name: mafOpCut
    Operation which perform cut on a node input. (copying it into a clipboard)
*/
class MAF_EXPORT mafOpCut: public mafOpEdit
{
public:
  mafTypeMacro(mafOpCut, mafOpEdit)
    /** Constructor. */
    mafOpCut(const mafString& label=_L("Cut"));
    /** Destructor. */
    ~mafOpCut() override;
    /** check if node can be input of the operation. */
    bool Accept(mafNode* vme) override;
    /** execute the operation.  */
    void OpDo() override;
    /** undo the operation. */
    void OpUndo() override;
    /** return a instance of current object. */
    mafOp* Copy() override; 
    /** Load VTK data for the specified VME (Added by Losi on 03.06.2010) */
    void LoadVTKData(mafNode *vme);

protected:
  mafNode *m_SelectionParent = nullptr;
    /** Load all children in the tree (Added by Di Cosmo on 24.05.2012) */
    void LoadChild(mafNode *vme);
};
/**
class name: mafOpDelete
Operation which perform delete on a node input.
*/
class MAF_EXPORT mafOpDelete: public mafOpEdit
{
public:
  mafTypeMacro(mafOpDelete, mafOpEdit)
  /** Constructor. */
  mafOpDelete(const mafString& label=_L("Delete"));
  /** Destructor. */
  ~mafOpDelete() override;
  /** check if node can be input of the operation. */
  bool Accept(mafNode* vme) override;
  /** execute the operation.  */
  void OpDo() override;
  /** undo the operation. */
  void OpUndo() override;
  /** return a instance of current object. */
  mafOp* Copy() override; 
protected:
  mafNode *m_SelectionParent = nullptr;
};
/**
    class name: mafOpCopy
    Operation which perform copy of a node.
*/
class MAF_EXPORT mafOpCopy: public mafOpEdit
{
public:
  mafTypeMacro(mafOpCopy, mafOpEdit)
    /** Constructor. */
    mafOpCopy(const mafString& label=_L("Copy"));
    /** Destructor. */
    ~mafOpCopy() override;
    /** check if node can be input of the operation. */
    bool Accept(mafNode* vme) override;
    /** execute the operation.  */
    void OpDo() override;
    /** undo the operation. */
    void OpUndo() override;
    /** return a instance of current object. */
    mafOp* Copy() override;
};
/**
    class name: mafOpPaste
    Operation which perform paste of a node previously copied or cut.
*/
class MAF_EXPORT mafOpPaste: public mafOpEdit
{
public:
  mafTypeMacro(mafOpPaste, mafOpEdit)
    /** Constructor. */
    mafOpPaste(const mafString& label=_L("Paste"));
    /** check if node can be input of the operation. */
    bool Accept(mafNode* vme) override;
    /** execute the operation.  */    
    void OpDo() override;
    /** undo the operation. */
    void OpUndo() override;
    /** return a instance of current object. */
    mafOp* Copy() override; 
protected:
  std::shared_ptr<mafNode> m_PastedVme;
};
