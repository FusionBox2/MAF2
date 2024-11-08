#pragma once

#include "ftkConfigure.h"

#include "mafEventBase.h"
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafStorage;
class mafNode;
class mafRoot;
//------------------------------------------------------------------------------
// mafEventIO
//------------------------------------------------------------------------------
/** Implementation of event used to exchange info about tree I/O.
  This event is thought to allow exchanging information regarding
  I/O of the tree.

  @sa mafEventBase mafVME mafVMEItem mafStorage mafSubject mafObserver
*/
class MAF_EXPORT mafEventIO: public mafEventBase
{
	mafTypeMacroN(mafEventIO);
public:
	mafEventIO() = default;
	mafEventIO(void* sender, mafID id);
  ~mafEventIO() override;

 
  /** copy constructor, this makes a copy of the event */
  virtual void DeepCopy(const mafEventIO *c);

  mafStorage* GetStorage() const;

  void SetStorage(mafStorage *storage);

  mafNode* GetRoot() const;

  void SetRoot(mafNode *root);
  
protected:
  mafStorage*     m_Storage = nullptr;
  mafNode*        m_Root = nullptr;
};
