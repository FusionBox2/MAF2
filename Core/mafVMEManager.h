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
#ifndef __mafVMEManager_H__
#define __mafVMEManager_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include "mafNode.h"
#include "mafVMERoot.h"

//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------

/** mafVMEManager : Class managing VMEs inside a MAF application.

This class is responsible for managing the VME in a MAF application:
It can add or remove VME to the current tree, add a new tree.

*/
//----------------------------------------------------------------------------
class MAF_EXPORT mafVMEManager: public mafBaseEventHandler, public mafEventSender
//----------------------------------------------------------------------------
{
public:
  /** constructor */
  mafVMEManager();

  /** destructor */
  ~mafVMEManager(); 

  /** Process events coming from other components */
  void OnEvent(mafEventBase *e);
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
  mafVMERoot *GetRoot();
   /** Set the tree's root. */
  bool SetRoot(mafVMERoot *root);
protected:
  bool                m_Modified;         ///< Used to known when the tree has been modified...
  mafAutoPointer<mafVMERoot> m_Root;

  /** friend test class */
  friend class mafVMEManagerTest; // Losi 02/16/2010 for test class
};
#endif
