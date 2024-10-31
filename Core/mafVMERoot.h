/*=========================================================================

 Program: MAF2
 Module: mafVMERoot
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMERoot_h
#define __mafVMERoot_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafRoot.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** mafVMERoot - a VME is the root of a tree of VME nodes. 
  This node is a specialized VME acting as root of the tree. It inherits
  root behavior from mafRoot and also is responsible for generating 
  Ids fore VME items added to the tree.
  @sa mafRoot mafVME

  @todo
  - 
  */
class MAF_EXPORT mafVMERoot : public mafVME, public mafRoot
{
public:
  mafTypeMacro(mafVMERoot,mafVME);

  /** compare two VME-Root. */
  bool Equals(mafVME *vme) override;

  enum ROOT_GUI_ID
  {
    ID_APPLICATION_STAMP = Superclass::ID_LAST,
    ID_LAST
  };

  int DeepCopy(mafNode *a) override;

    /** 
    Return highest ItemId assigned for this tree. Return -1 if no one has
    been assigned yet.*/
  mafID GetMaxItemId() {return m_MaxItemId;}

  /** Return next available ItemId and increment the internal counter.*/
  mafID GetNextItemId() { this->Modified();return ++m_MaxItemId;}

  /**
    Set the ItemMaxId. Beware when using this function to avoid non unique
    Ids. */
  void SetMaxItemId(mafID id) { m_MaxItemId=id; Modified();}
  void ResetMaxItemId() { this->SetMaxItemId(0);}

  /** Clean the tree and all the data structures of this root*/
  void CleanTree() { \
    mafNode::CleanTree(); \
    m_Attributes.clear(); \
  };

  /** set the tree reference system */
  void SetMatrix(const mafMatrix &mat) override;

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;// const;

  /** return no timestamps */
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;

  /** allow only a NULL parent */
  bool CanReparentTo(mafNode *parent) override {return parent==NULL;}

  void OnEvent(mafEventBase *maf_event) override;

  mafID GetNewNodeId() override {return GetNextNodeId();}

  /** Redefined to update the gui. */
  void Update() override;

  /** return an xpm-icon that can be used to represent this node */
  static const char ** GetIcon();   //SIL. 11-4-2005:  

  /** Return the suggested pipe-typename for the visualization of this vme */
  mafString GetVisualPipe() override {return mafString(_R("mafPipeBox"));};


protected:
  /** allowed only dynamic allocation via New() */
  mafVMERoot();
  ~mafVMERoot() override;  

  /** Create GUI for the VME */
  mafGUI  *CreateGui() override;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  mafID         m_MaxItemId; ///< id counter for VME items
  std::shared_ptr<mafTransform> m_Transform; ///< pose matrix for the root

  mafString m_ApplicationStamp;

private:
  mafVMERoot(const mafVMERoot&); // Not implemented
  void operator=(const mafVMERoot&); // Not implemented
};
#endif
