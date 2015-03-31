/*=========================================================================

 Program: MAF2
 Module: mafVMEManager
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


#include "mafVMEManager.h"
#include "mafDecl.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
mafVMEManager::mafVMEManager()
//----------------------------------------------------------------------------
{
  m_Modified = false;
  m_Root     = NULL;
}
//----------------------------------------------------------------------------
mafVMEManager::~mafVMEManager()
//----------------------------------------------------------------------------
{
  SetRoot(NULL);
  SetListener(NULL);
}
//----------------------------------------------------------------------------
void mafVMEManager::OnEvent(mafEventBase *maf_event)
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
mafVMERoot *mafVMEManager::GetRoot()
//----------------------------------------------------------------------------
{
  return m_Root;
}

//----------------------------------------------------------------------------
bool mafVMEManager::SetRoot(mafVMERoot *root)
//----------------------------------------------------------------------------
{
  NotifyRemove(m_Root);
  if(m_Root)
    m_Root->SetListener(NULL);
  m_Root = root;
  if(m_Root)
    m_Root->SetListener(this);
  NotifyAdd(m_Root);
  return true;
}
//----------------------------------------------------------------------------
void mafVMEManager::VmeAdd(mafNode *n)
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
void mafVMEManager::VmeRemove(mafNode *n)
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
void mafVMEManager::NotifyRemove(mafNode *n)
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
void mafVMEManager::NotifyAdd(mafNode *n)
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
