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

#ifndef __mafNodeManager_H__
#define __mafNodeManager_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include "mafNode.h"
#include "mafNodeRoot.h"

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,wxString);
#endif

//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------

/** mafNodeManager : Class managing nodes inside a MAF application.

This class is responsible for managing the nodes in a MAF application:
It can add or remove VME to the current tree, add a new tree.

*/
//----------------------------------------------------------------------------
class MAF_EXPORT mafNodeManager: public mafBaseEventHandler, public mafEventSender
//----------------------------------------------------------------------------
{
public:
  /** constructor */
  mafNodeManager();

  /** destructor */
  ~mafNodeManager(); 

  /** Process events coming from other components */
  void OnEvent(mafEventBase *maf_event);
	/** Return true if the tree has been modifyed. */
  bool MSFIsModified() {return m_Modified;};
  
	/** Set the modify flag. */
  void MSFModified(bool modified) {m_Modified = modified;};

	/** Add the vme to the tree. */
  void VmeAdd(mafNode *n);

	/** Remove the vme from the tree. */
  void VmeRemove(mafNode *n);

	/** Send the event VME_REMOVING to inform logic that the vme and its subtree are removed from the tree. */
  void NotifyRemove(mafNode *n);

	/** Send the event VME_ADDED to inform logic that the vme and its subtree are added to the tree. */
  void NotifyAdd(mafNode *n);
	/** Return the tree's root. */
  mafNodeRoot *GetRoot();
   /** Set the tree's root. */
  bool SetRoot(mafNodeRoot *root);
protected:
  bool                m_Modified;         ///< Used to known when the tree has been modified...
  mafAutoPointer<mafNodeRoot> m_Root;

  /** friend test class */
  friend class mafNodeManagerTest; // Losi 02/16/2010 for test class
};
#endif
