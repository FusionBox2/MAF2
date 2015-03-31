/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObjectWithGUI.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:56:04 $
  Version:   $Revision: 1.35 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafObjectWithGUI_h
#define __mafObjectWithGUI_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDecl.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafGUI;

//----------------------------------------------------------------------------
// mafObjectWithGUI
//----------------------------------------------------------------------------
/** mafObjectWithGUI - the base class for the MAF tree.
  This class implements a m-way tree. You can add/remove nodes by means of AddChild
  and RemoveChild. To access the tree you can use GetChild(). You can also obtain an iterator
  to iterate through the tree with a simple for (;;) loop. This node implementation take
  advantage of the MAF reference counting mechanism. To avoid confusion constructor
  and destructor have been protected. To allocate a node use New() and to deallocate use Delete()
  or UnRegister().
  To create a copy of the node you can use MakeCopy(). To copy node content use DeepCopy(). Any 
  node has a CanCopy() to test if copying from a different node type i possible.
  A number of functions allow to query the tree, like IsEmpty(), IsInTree(), GetRoot(), GetNumberOfChildren(),
  FindNodeIdx(), FindInTreeByName(), FindInTreeById(), IsAChild(), GetParent().
  A special features allow to make a node to be skipped by iterators: SetVisibleToTraverse()
  and IsVisible().
  Comparison between nodes and trees can be accomplished through Equals() and CompareTree().
  Nore reparenting can be performed through ReparentTo(). This function returns MAF_ERROR in case
  reparenting is not allowed. Each node type can decide nodes to which it can be reparented by
  redefining the CanReparentTo() virtual function. Also each node type can decide nodes it is 
  accepting as a child by redefining the AddChild() which also can return MAF_ERROR in case of 
  denied reparenting.
  A node can detach all children RemoveAllChildren() and an entire tree can be cleaned, by detaching each sub node, 
  through CleanTree().
  Nodes inherits from mafTimeStamped a modification time updated each time Modified() is called, that can be retrieved with GetMTime().
  A tree can be initialized by calling Initialize() of its root, and deinitialized by means of Shutdown(). When
  attaching a node to an initialised tree the node is automatically initialized.
  @todo
  - events invoking
  - add storing of Id and Links
  - test Links and Id
  - test FindInTree functions
  - test node events (attach/detach from tree, destroy)
  - test DeepCopy()

  @sa mafObjectWithGUIRoot
*/
class MAF_EXPORT mafObjectWithGUI
{
public:
  mafObjectWithGUI();
  virtual ~mafObjectWithGUI();

  /** create and return the GUI for changing the node parameters */
  mafGUI *GetGui();
  /** destroy the Gui */
  void DeleteGui();

protected:

  /**
    Internally used to create a new instance of the GUI. This function should be
    overridden by subclasses to create specialized GUIs. Each subclass should append
    its own widgets and define the enum of IDs for the widgets as an extension of
    the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
    subclass to continue the ID enumeration from it. For appending the widgets in the
    same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mafGUI  *CreateGui() = 0;
  mafGUI          *m_Gui;         ///< pointer to the node GUI
};

#endif
