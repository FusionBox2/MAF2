/*=========================================================================

 Program: MAF2
 Module: mafNodeManager
 Authors: Silvano Imboden
 
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


#include "mafNodeManager.h"
#include "mafDecl.h"
#include "mafNode.h"
#include "mafRoot.h"
#include "mafNodeIterator.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
mafNodeManager::mafNodeManager()
//----------------------------------------------------------------------------
{
  m_Modified = false;
  m_Root     = NULL;
}
//----------------------------------------------------------------------------
mafNodeManager::~mafNodeManager()
//----------------------------------------------------------------------------
{
  SetRoot(NULL);
  SetListener(NULL);
}
//----------------------------------------------------------------------------
void mafNodeManager::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (maf_event->GetChannel()!=MCH_UP)
    return;
  // events coming from the tree...
  if(maf_event->GetId() == NODE_ATTACHED_TO_TREE)
  {
    NotifyAdd((mafNode *)maf_event->GetSender());
    MSFModified(true);
    return;
  }
  if(maf_event->GetId() == NODE_DETACHED_FROM_TREE)
  {
    NotifyRemove((mafNode *)maf_event->GetSender());
    MSFModified(true);
    return;
  }
  mafEventMacro(*maf_event);
}

//----------------------------------------------------------------------------
mafNode *mafNodeManager::GetRoot()
//----------------------------------------------------------------------------
{
  return m_Root;
}

//----------------------------------------------------------------------------
bool mafNodeManager::SetRoot(mafNode *root)
//----------------------------------------------------------------------------
{
  NotifyRemove(m_Root);
  if(mafRoot *rt = mafRoot::SafeDownCast(GetRoot()))
    rt->SetListener(NULL);
  m_Root = root;
  if(mafRoot *rt = mafRoot::SafeDownCast(GetRoot()))
    rt->SetListener(this);
  NotifyAdd(m_Root);
  return true;
}
//----------------------------------------------------------------------------
void mafNodeManager::VmeAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  if(n == NULL)
    return ;
  // check the node's parent
  mafNode *vp = n->GetParent();  
  if(vp != NULL && !m_Root->IsInTree(vp))
  {
    assert(false);
    return;
  }
  if(vp == NULL) 
    n->ReparentTo(m_Root); // reparent the node to the root
  m_Modified = true;
}
//----------------------------------------------------------------------------
void mafNodeManager::VmeRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  if(n == NULL)
    return ;
  if(!m_Root || !m_Root->IsInTree(n))
  {
    assert(false);
    return;
  }
  n->ReparentTo(NULL);
  m_Modified = true;
}
//----------------------------------------------------------------------------
void mafNodeManager::NotifyRemove(mafNode *n)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
  iter->SetTraversalModeToPostOrder(); // traverse is: first the subtree left to right, then the root
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		mafEventMacro(mafEvent(this,VME_REMOVING,node)); // raise notification event (to logic)
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafNodeManager::NotifyAdd(mafNode *n)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = n->NewIterator();
  iter->IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    mafEventMacro(mafEvent(this,VME_ADDED,node)); // raise notification event (to logic)
  }
  iter->Delete();
  m_Modified = true;
}
