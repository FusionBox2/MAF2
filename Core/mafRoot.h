#pragma once

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafEventSender.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
namespace model::data
{
    class Node;
}
using mafNode = model::data::Node;
class mafStorageElement;
class mafStorageElementBuilder;

/** mafRoot - this class represent an interface for the root nodes of a MAF tree
  mafRoot is an interface class to specialize node to become a root node. It forces
  a NULL parent node and support a Listener object to forward up events coming from
  the tree. The listener is typically the object responsible for creating the tree.
  The root node is also responible for generating Ids for new nodes attached to the
  tree.
  @sa  mafNode
*/
class MAF_EXPORT mafRoot: public mafEventSender
{
public:

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** 
    Return highest NodeId assigned for this tree. Return -1 if no one has
    been assigned yet.*/
  mafID GetMaxNodeId() {return m_MaxNodeId;}

  /** Return next available NodeId and increment the internal counter.*/
  mafID GetNextNodeId() { return ++m_MaxNodeId;}

  /**
    Set the NodeMaxId. Beware when using this function to avoid non unique
    Ids. */
  void SetMaxNodeId(mafID id) { m_MaxNodeId=id;}
  void ResetMaxNodeId() { this->SetMaxNodeId(0);}

  static mafRoot* SafeDownCast(mafNode *o);

protected:
  mafRoot();
  ~mafRoot() override;

  virtual void StoreRoot(mafStorageElementBuilder& parent);
  virtual void RestoreRoot(const mafStorageElement& node);

  mafID       m_MaxNodeId = 0; ///< Counter for node Ids

private:
  mafRoot(const mafRoot&); // Not implemented
  void operator=(const mafRoot&); // Not implemented
  
};
