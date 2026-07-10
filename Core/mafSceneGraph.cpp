/*=========================================================================

 Program: MAF2
 Module: mafSceneGraph
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


#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafView.h"
//#include "mafPipeGizmo.h"
//#include "mafPipePointSet.h"
#include "mafGUI.h"
#include "mafIndent.h"
#include "mafNode.h"
#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMEGizmo.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
mafSceneGraph::mafSceneGraph(mafView *view, vtkRenderer *ren1, vtkRenderer *ren2, vtkRenderer *ren3)
//----------------------------------------------------------------------------
{
	m_RenFront  = ren1;
	m_RenBack   = ren2;
  m_AlwaysVisibleRenderer = ren3;
	m_View      = view;

  m_Gui          = NULL;
  m_List         = NULL;
	m_SelectedVme  = NULL;

/*
  int i;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_creatable[i] = m_Mutex[i] = false;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_autoshow[i] = 0;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_shown_mutex_vme[i] = NULL;
*/
}
//----------------------------------------------------------------------------
mafSceneGraph::~mafSceneGraph( ) 
//----------------------------------------------------------------------------
{
  DeleteNodeList(m_List);
  m_List = NULL;
}
//----------------------------------------------------------------------------
void mafSceneGraph::DeleteNodeList(mafSceneNode *n) 
//----------------------------------------------------------------------------
{
  if(n)
	{
		DeleteNodeList(n->m_Next);
		delete n;
  }
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeAdd(std::shared_ptr<mafNode> vme)
//----------------------------------------------------------------------------
{
	mafSceneNode *node = NodeAdd(vme);
	if(!node) return;
	
  // must be after NodeAdd
  int nodestatus = m_View->GetNodeStatus(vme.get());
  node->m_PipeCreatable = ( nodestatus != NODE_NON_VISIBLE );
  node->m_Mutex         = ( nodestatus == NODE_MUTEX_ON  || 
                            nodestatus == NODE_MUTEX_OFF );
}
//----------------------------------------------------------------------------
mafSceneNode *mafSceneGraph::NodeAdd(std::shared_ptr<mafNode> vme)
//----------------------------------------------------------------------------
{
  assert(vme);

  // check if node already exist
	if( Vme2Node(vme.get()) ) return nullptr;

  // find parent node
  mafSceneNode *parent = NULL;
  if( vme->IsA("mafNodeRoot") || vme->IsA("mafVMERoot"))   //SIL. 20-4-2005:  - IsA("mafRoot") si puo fare ?
  {
  }
  else
  {
    parent = Vme2Node(vme->GetParent()); 
    assert(parent);
  }
  // create node
  mafSceneNode *node = new mafSceneNode(this,parent,vme,m_RenFront,m_RenBack,m_AlwaysVisibleRenderer) ;

	// insert node in the list
	if(!m_List)
		m_List = node;
  else
  {
    mafSceneNode* n;
    for(n = m_List; n->m_Next; n=n->m_Next); // go to the end of the list
      n->m_Next = node;
  }
  return node;
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeRemove(mafNode *vme)   
//----------------------------------------------------------------------------
{
  VmeShow(vme,false);

  mafSceneNode *node=nullptr;

  if(!m_List) return;

  if(m_List->m_Vme.get() == vme)
  {
     node = m_List;
     m_List = m_List->m_Next;
  }
  else
  {
    for(mafSceneNode *n = m_List; n->m_Next; n=n->m_Next) // find prec of node(vme)
    {
      if(n->m_Next->m_Vme.get() == vme)
      {
        node = n->m_Next;
        n->m_Next = n->m_Next->m_Next;
				break; 
      }
    }
  }
	if(node)
		delete node;
}
//----------------------------------------------------------------------------
mafSceneNode *mafSceneGraph::Vme2Node(mafNode *vme)   
//----------------------------------------------------------------------------
{
  for(mafSceneNode *n = m_List; n; n=n->m_Next)
    if(n->m_Vme.get() == vme)
      return n;
  return nullptr;
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeSelect(mafNode *vme, bool select)   
//----------------------------------------------------------------------------
{
  if(select) m_SelectedVme = vme;

	mafSceneNode *node = Vme2Node(vme);
	if(node == NULL) return;
  node->Select(select);
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeUpdateProperty(mafNode *vme, bool fromTag)
//----------------------------------------------------------------------------
{
  mafSceneNode *node = Vme2Node(vme);
	if(node == NULL) return;
	node->UpdateProperty(fromTag);
}
/* @@@
//----------------------------------------------------------------------------
void mafSceneGraph::SetCreatableFlag(mafNodeBaseTypes type,  bool flag)
//----------------------------------------------------------------------------
{
  m_creatable[type] = flag;
}
//----------------------------------------------------------------------------
void mafSceneGraph::SetMutexFlag(mafNodeBaseTypes type,  bool flag)
//----------------------------------------------------------------------------
{
  m_Mutex[type] = flag;
	if(flag) m_creatable[type] = true;
}
//----------------------------------------------------------------------------
void mafSceneGraph::SetAutoShowFlag(mafNodeBaseTypes type,  bool flag)
//----------------------------------------------------------------------------
{
	if(m_creatable[type])	m_autoshow[type] = flag;
}
@@@ */
//----------------------------------------------------------------------------
void mafSceneGraph::VmeShow(mafNode *vme, bool show)
//----------------------------------------------------------------------------
{
	mafSceneNode *node = Vme2Node(vme);
	if( node == NULL) return;
  
  if(!node->m_PipeCreatable) return;
  if( node->IsVisible() == show) return;

	if(show)
	{	
    if(node->m_Mutex)
		{
      {
        // Changed code below to allow all MUTEX VMEs to behave at the same manner also if 
        // they are of different type: only one MUTEX VME per time is visible into the view.
        for(mafSceneNode *n = m_List; n; n=n->m_Next)
        {
          //if(n->m_Pipe != NULL && n->m_Vme != vme && n->m_Mutex)
		      auto in_vme = mafVME::SafeDownCast(vme);
		      auto current_vme = mafVME::SafeDownCast(n->m_Vme);
          bool vme_type_check = in_vme != NULL && current_vme != NULL;
          if(n->m_Pipe != NULL && n->m_Vme.get() != vme && vme_type_check && !current_vme->IsA("mafVMEGizmo") && in_vme->GetOutput()->GetTypeId() == current_vme->GetOutput()->GetTypeId())
          {
            {mafEvent evUnq(this,VME_SHOW); evUnq.SetVme(n->m_Vme.get()); evUnq.SetBool(false); InvokeEvent(evUnq);}
          }
        }
      }

		}
    
		if(!node->m_Pipe) 
		{
			m_View->VmeCreatePipe(vme);
      if (!vme->IsMAFType(mafVMEGizmo))
      {
        if(m_View->GetNumberOfVisibleVME() == 1)
        {
          m_View->CameraReset();
        }
        else
        {
          m_View->CameraUpdate();
        }
      }
			VmeSelect(vme,vme == m_SelectedVme);
		}
	}
	else
	{
		if(node->m_Pipe) m_View->VmeDeletePipe(vme);
	}

  //node->Show(show);  //SIL. 21-4-2005: 
  
  /* @@@
  // update m_shown_mutex_vme
	if(node->m_Mutex)
	{
		mafNodeBaseTypes type = mafGetBaseType(vme);
		m_shown_mutex_vme[type] = (show) ? vme : NULL;
	}
  @@@ */
}
/* @@@
//----------------------------------------------------------------------------
void mafSceneGraph::VmeShowByType(mafNode *vme,  bool show)
//----------------------------------------------------------------------------
{
	VmeShowByType( mafGetBaseType(vme), show);
}
//----------------------------------------------------------------------------
// Scan the tree and send a VME_SHOW event for each vme with the same type.
// Type should be creatable and non mutex.
// Changed behavoir.
// Mutex vme may be shown only is no other vme of the same type is currently shown.
// Mutex vme may always be hidden.
void mafSceneGraph::VmeShowByType(mafNodeBaseTypes type,	bool show)
//----------------------------------------------------------------------------
{
	if(!m_creatable[type] ) return;
	
	for( mafSceneNode *n = m_List; n; n=n->m_Next)
	  if(type == mafGetBaseType(n->m_Vme) && n->IsVisible() != show )
			//- mutex vme may be shown only is no other vme of the same type is currently shown.
			//- mutex vme may always be hidden.
			if(!show || m_shown_mutex_vme[type]==NULL) 
				{mafEvent evUnq(this,VME_SHOW,n->m_Vme,show); InvokeEvent(evUnq);}
}
@@@ */

//----------------------------------------------------------------------------
void mafSceneGraph::VmeShowByType(mafNode *vme,  bool show)
//----------------------------------------------------------------------------
{
  for(mafSceneNode *n = m_List; n; n=n->m_Next)
    if(vme->GetTypeId() == n->m_Vme->GetTypeId() && n->IsVisible() != show)
    {
      //- mutex vme may be shown only is no other vme of the same type is currently shown.
      //- mutex vme may always be hidden.
      {mafEvent evUnq(this,VME_SHOW); evUnq.SetVme(n->m_Vme.get()); evUnq.SetBool(show); InvokeEvent(evUnq);}
      if (n->m_Mutex)
      {
        break;
      }
    }
}
//----------------------------------------------------------------------------
// Scan vme subtree and send a VME_SHOW event for each Creatable/non_mutex vme found.
// Changed behavior.
// Mutex vme may be shown only is no other vme of the same type is currently shown.
// Mutex vme may always be hidden.
void mafSceneGraph::VmeShowSubTree(mafNode *vme,  bool show)
//----------------------------------------------------------------------------
{
  auto vv = Vme2Node(vme)->m_Vme.get();
	for(auto& v : *vv)
	{
    mafSceneNode *n = Vme2Node(&v);
		if(n && n->m_PipeCreatable && n->IsVisible() != show )
		{
			// Mutex vme may be shown only is no other vme of the same type is currently shown.
			// Mutex vme may always be hidden.
			if(!show || !n->m_Mutex)
        {mafEvent evUnq(this, VME_SHOW); evUnq.SetVme(&v); evUnq.SetBool(show); InvokeEvent(evUnq);}
		} 
	}
}/*
int  mafViewVTK::GetNodeStatus(mafNode* vme)
//----------------------------------------------------------------------------
{
	int status = m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
	if (!m_PipeMap.empty())
	{
		mafString vme_type = _R(vme->GetTypeName());
		auto it = m_PipeMap.find(vme_type);
		if (it == m_PipeMap.end())
			return status;
		if (it->second.m_Visibility == NON_VISIBLE)
		{
			status = NODE_NON_VISIBLE;
		}
		else if (it->second.m_Visibility == MUTEX)
		{
			mafSceneNode* n = m_Sg->Vme2Node(vme);
			if (n != NULL)
			{
				n->m_Mutex = true;
			}
			status = m_Sg->GetNodeStatus(vme);
		}
	}
	return status;
}

//----------------------------------------------------------------------------
void mafSceneGraph::VmeAdd(std::shared_ptr<mafNode> vme)
//----------------------------------------------------------------------------
{
	mafSceneNode* node = NodeAdd(vme);
	if (!node) return;

	// must be after NodeAdd
	int nodestatus = m_View->GetNodeStatus(vme.get());
	node->m_PipeCreatable = (nodestatus != NODE_NON_VISIBLE);
	node->m_Mutex = (nodestatus == NODE_MUTEX_ON ||
		nodestatus == NODE_MUTEX_OFF);
}
*/
//----------------------------------------------------------------------------
int mafSceneGraph::GetNodeStatus(mafNode *node)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = Vme2Node(node);
	if(!n)
    return NODE_NON_VISIBLE;

  if (!node->IsMAFType(mafVME))
  {
    return NODE_NON_VISIBLE;
  }
  mafVME *vme = (mafVME *)node;
  bool creatable = n->m_PipeCreatable && vme && !vme->GetVisualPipe().empty();
	//landmark are not creatable
	//if(vme->IsA("mafNodeLandmark")) creatable = false;
  
	if(!creatable)											return NODE_NON_VISIBLE;
  if( n->m_Mutex &&  n->IsVisible())  return NODE_MUTEX_ON;
  if( n->m_Mutex && !n->IsVisible())  return NODE_MUTEX_OFF;
  if( n->IsVisible())									return NODE_VISIBLE_ON;
  if(!n->IsVisible())									return NODE_VISIBLE_OFF;
	return NODE_NON_VISIBLE;
}

//-------------------------------------------------------------------------
void mafSceneGraph::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  // Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "mafSceneGraph:" << '\t' << this << "\n";

  // print the scene graph 
  mafSceneNode *currentNode = NULL ;

  // if there is at least one node...  
  if(m_List)
  {
    currentNode = m_List;

    while (currentNode)
    {
      //print the current node
      currentNode->Print(os, 1);

      //go to the next node
      currentNode = currentNode->m_Next;
    }
  }
  else
  {
    os << indent << "The scene graph is empty" << std::endl;
  }
}
