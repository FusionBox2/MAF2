#pragma once

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafEventBase.h"
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
namespace model::data
{
	class Node;
}
using mafNode = model::data::Node;
class mafStorage;
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
public:
  mafEventIO();
  mafEventIO(void* sender, mafID id);
  ~mafEventIO() override;

  mafTypeMacroN(mafEventIO);
 
  /** copy constructor, this makes a copy of the event */
  virtual void DeepCopy(const mafEventIO *c);

  /** set the storage reference */
  void SetStorage(mafStorage *storage);

  mafStorage *GetStorage() const;

  void SetRoot(mafNode *root);
  
  /** return the pointer to the tree root node */
  mafNode *GetRoot() const;

protected:
  mafStorage*     m_Storage = nullptr;
  mafNode*        m_Root = nullptr;
};
