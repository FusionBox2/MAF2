#pragma once

#include "ftkConfigure.h"
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "ftk/Base/Object.h"
#include "mafNode.h"
#include "mafEventSender.h"
#include <vector>

/** mafNodeIterator - an m-way tree data structure iterator
  mafNodeIterator is a class to traverse a tree data structure. It allows 
  to set the traverse modality. The iterator allows to set some callback to the 
  traverse by means of the MAF Subject/Observer mechanism.
  Issued events are: "PreTraversal","PostTraversal","Deeper","Upper","FirstNode",
  "LastNode", "Done".
  These can be overidden by adding an observer
  
  @sa mafNode
  */

class MAF_EXPORT mafNodeIterator final : public mafEventSource
{
public:
  mafBaseTypeMacro(mafNodeIterator);
  mafNodeIterator(std::shared_ptr<mafNode> root = nullptr);


  /** Retrieve the current node pointer data from the iterator. */
  std::shared_ptr<mafNode> GetCurrentNode() {return m_CurrentNode;}

  /**  Shortcut to traverse the tree*/
  std::shared_ptr<mafNode> GetFirstNode() {this->GoToFirstNode(); return (m_TraversalDone)?nullptr:this->GetCurrentNode();}
  /**  Shortcut to traverse the tree*/
  std::shared_ptr<mafNode> GetLastNode() {this->GoToLastNode(); return (m_TraversalDone)?nullptr:this->GetCurrentNode();}
  /**  Shortcut to traverse the tree*/
  std::shared_ptr<mafNode> GetNextNode() {this->GoToNextNode(); return (m_TraversalDone)?nullptr:this->GetCurrentNode();}
  /**  Shortcut to traverse the tree*/
  std::shared_ptr<mafNode> GetPreviousNode() {this->GoToPreviousNode(); return (m_TraversalDone)?nullptr:this->GetCurrentNode();}

  /**
    Return true if the VME is visible. This function can be overridden to implement
    different visibility rules.*/
  //bool IsVisible(mafNode *node) { return node->IsVisible();}

  /** Allow to ignore m_VisibleToTraverse flag for the iterator. */
  void IgnoreVisibleToTraverse(bool ignore) {m_IgnoreVisibleToTraverse = ignore;};

  /**
  Set the root node of the (sub)tree to be traversed. Used to set the start 
  point. */
  void SetRootNode(std::shared_ptr<mafNode> root);
  
  /**
    Initialize the traversal of the container. 
    Set the iterator to the "beginning" of the container.*/
  void InitTraversal();
  
  /**
    Check if the iterator is at the end of the container. Returns 1
    for yes and 0 for no.*/
  int IsDoneWithTraversal() {return m_TraversalDone;}
  
  /** Increment the iterator to the next location.*/
  int GoToNextNode();
  
  /** Decrement the iterator to the next location.*/
  int GoToPreviousNode();
  
  /** Go to the first item of the list.*/
  int GoToFirstNode();
  
  /** Go to the last item of the list.*/
  int GoToLastNode();

  /**
    Set/Get the traverse modality. Default to PreOrder, i.e. process 
    first the root an then the sub trees (left to right).
    Change the traverse mode restart the traversing!*/
  void SetTraversalMode(int mode);
  int GetTraversalMode() { return m_TraversalMode;}

  /**
    Set the traverse modality to PreOrder: first the root then the subtrees
    left to right*/
  void SetTraversalModeToPreOrder() {this->SetTraversalMode(PreOrder);}
  /**
    Set the traverse modality to PostOrder: first the the subtrees
    left to right, then the root.*/
  void SetTraversalModeToPostOrder() {this->SetTraversalMode(PostOrder);}

  /**
    Set the traverse modality to InOrder: first the the left subtrees
    (left to right), then right subtrees (left to right), and finally the root.
    The partioning of the sub-trees is dicotomic. This mode is useful for
    B-Tree's traversing*/
  //void SetTraversalModeToInOrder() {this->SetTraversalMode(InOrder);}
  
  enum traversalMode {PreOrder=0,PostOrder};

  enum events_ID {ID_PreTraversal=0,ID_PostTraversal,ID_Deeper, ID_Upper, ID_FirstNode, ID_LastNode, ID_Done};
protected:

  /** Find the left most leaf of the tree*/
  std::shared_ptr<mafNode> FindLeftMostLeaf(std::shared_ptr<mafNode> node);

  /** Find the right most leaf of the tree*/
  std::shared_ptr<mafNode> FindRightMostLeaf(std::shared_ptr<mafNode> node);

  /** Callback function. By default send an event through the m_EventSource source. */
  void PreExecute(); 

  /** Callback function. By default send an event through the m_EventSource source. */
  void PostExecute();  

  /** Callback function. By default send an event through the m_EventSource source. */
  void DeeperExecute(mafNode *); 

  /** Callback function. By default send an event through the m_EventSource source. */
  void UpperExecute(mafNode *);  

  /** Callback function. By default send an event through the m_EventSource source. */
  void FirstExecute(); 

  /** Callback function. By default send an event through the m_EventSource source. */
  void LastExecute();  

  /** Callback function. By default send an event through the m_EventSource source. */
  void DoneExecute();  

  std::shared_ptr<mafNode> m_RootNode;
  std::shared_ptr<mafNode> m_CurrentNode;
  int             m_TraversalMode = 0;
  int             m_TraversalDone = 0;
  bool            m_IgnoreVisibleToTraverse = false;

  std::vector<mafID> m_CurrentIdx;
};
