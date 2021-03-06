/*=========================================================================

 Program: MAF2
 Module: mafNode
 Authors: Marco Petrone, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafNode_h
#define __mafNode_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafReferenceCounted.h"
#include "mafStorable.h"
#include "mafSmartPointer.h"
#include "mafObserver.h"
#include "mafTagItem.h"
#include "mafString.h"
#include "mafTimeStamped.h"
#include "mafAttribute.h"
#include "mafDecl.h"
#include <vector>
#include <map>
#include <string>

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafAutoPointer<mafNode>);
EXPORT_STL_MAP(MAF_EXPORT,mafString,mafAutoPointer<mafAttribute>);
#endif

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafEventSource;
class mafNodeIterator;
class mafTagArray;
class mafGUI;

/** data structure used to store a link VME and its Id */
class MAF_EXPORT mmuNodeLink :public mafUtility
{
public:
  mmuNodeLink(mafID id=-1,mafNode *node=NULL, mafID sub_id=-1):m_NodeId(id),m_Node(node),m_NodeSubId(sub_id) {}
  mafNode *m_Node;
  mafID   m_NodeId;
  mafID   m_NodeSubId;
};
#ifdef MAF_EXPORTS
EXPORT_STL_MAP(MAF_EXPORT,mafString,mmuNodeLink);
#endif

//----------------------------------------------------------------------------
// mafNode
//----------------------------------------------------------------------------
/** mafNode - the base class for the MAF tree.
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

  @sa mafNodeRoot
*/
class MAF_EXPORT mafNode : public mafReferenceCounted, public mafStorable, public mafObserver, public mafTimeStamped
{
public:
  mafAbstractTypeMacro(mafNode,mafReferenceCounted);

  /** defined to allow MakeCopy implementation. For the base class return a NULL pointer. */
  virtual mafObject *NewObjectInstance() const {return NULL;}

  /** Interface to allow creation of a copy of the node (works only for concrete subclasses) */
  mafNode *NewInstance() {return SafeDownCast(NewObjectInstance());}
  
  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /**
    Initialize this node. Subclasses can redefine InternalInitialize() to customize
    the initialization. A node is typically initialized when added to the tree, or
    just after tree loading in case of load from storage. */
  int Initialize();
  
  /**
    Shutdown this node. Subclasses can redefine InternalShutdown() to customize 
    actions for shutting down. A node is typically shutdown when detached from
    the tree */
  void Shutdown();
  
  /** Return true if this agent has been initialized */
  int IsInitialized() {return m_Initialized;}

  /** serialize the object on a store. @todo syntax to be changed */
  int Store();

  /** unserialized the object from a storage. @todo syntax to be changed */
  int Restore();
  
  /** return the name of this node*/
  const char *GetName() {return m_Name;}

  /** set node name */
  void SetName(const char *name);
  
  /**
    Copy the contents of another node into this one. Notice that subtrees
    are not copied, i.e. copy is not recursive!
    Concrete nodes should reimplement this function to verify admitted
    conversion. */
  virtual int DeepCopy(mafNode *a);
  
  /** TODO: to be moved to mafVME
    perform a copy by simply referencing the copied node's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some nodes
    do not support such a function! */  
  //virtual int ShallowCopy(mafNode *a);
  
  /** Test if the given node instance can be copied into this. This function should
    be reimplemented into subclasses classes*/
  virtual bool CanCopy(mafNode *vme);
  
  /** Create a copy of this node (do not copy the sub tree,just the node) */
  static mafNode *MakeCopy(mafNode *a);
  mafNode *MakeCopy() {return MakeCopy(this);}
  
  /** Copy the given VME tree into a new tree. In case a parent is provided, link the new
    root node to it. Return the root of the new tree.*/
  static mafNode *CopyTree(mafNode *vme, mafNode *parent=NULL);
  
  /** Make a copy of the whole subtree and return its pointer */
  mafNode *CopyTree() {return CopyTree(this);}
  
  /** Return a the pointer to a child given its index. 
      If only visible is true return the idx-th visible to traverse node */
  mafNode *GetChild(mafID idx, bool onlyVisible=false);
  /** Get the First child in the list.
      If only visible is true return the first visible to traverse node */
  mafNode *GetFirstChild(bool onlyVisible=false);
  /** Get the Lase child in the list.
      If only visible is true return the last visible to traverse node */
  mafNode *GetLastChild(bool onlyVisible=false);

  /** Get A child by path.
  The pats are generated from a series of keyword divided by '\'
  
  The possible keywords are:
  next: return the next node at same level
  prev: return the previous node at same level
  firstPair: return the first node at same level
  lastPair: return the last node at same level
  firstChild: return the first node between children 
  lastChild: return the last node between children
  pair[<number>]: return the <number>-th node between pairs
  pair{<node name>}: return the node named <node name> between pairs
  child[<number>]: return the <number>-th node between children
  child{<node name>}: return the node named <node name> between children
  ..:

  An example path is:
  "../../child{sideB}/child[2]"

  By default this function search only on visible to traverse nodes
  */
  mafNode *GetByPath(const char *path, bool onlyVisible=true);
  
  /** Add a child to this node. Return MAF_OK if success.*/
  virtual int AddChild(mafNode *node);

  /** Remove a child node*/
  virtual void RemoveChild(const mafID idx, bool onlyVisible=false);
  /** Remove a child node*/
  virtual void RemoveChild(mafNode *node);

  /** Find a child given its pointer and return its index. Return -1 in case of not found or failure.
      If only visible is true return the idx of visible to traverse nodes subset */
  int FindNodeIdx(mafNode *a, bool onlyVisible=false);

  /** Find a child index given its name. Search is performed only on first level children not
    in the sub-tree. Return -1 in case of not found or failure.
    If only visible is true return the idx of visible to traverse nodes subset */
  int FindNodeIdx(const char *name, bool onlyVisible=false);

  /** Find a node in all the subtrees matching the given TagName/TagValue pair.*/
  mafNode *FindInTreeByTag(const char *name,const char *value="",int type=MAF_STRING_TAG); 
  
  /** Find a node in all the subtrees matching the given VME Name.*/
  mafNode *FindInTreeByName(const char *name, bool match_case = true, bool whole_word = true);

  /** Find a node in all the subtrees matching the given VME Name.*/
  mafNode *FindInTreeById(const mafID id);

  /**
    Reparent this Node into a different place of the same tree. 
    BEWARE: Reparent into a different tree is allowed, but could
    generate bad problems. Inherited classes should reimplement this
    function to avoid these problems when reparenting to different trees.
    To move a node into a different tree you better use DeepCopy to copy 
    it into a Node of that tree.*/
  virtual int ReparentTo(mafNode *parent);

  /** Import all children of another tree into this tree */
  void Import(mafNode *tree);

  /** Return true if the given one is a child of this node.*/
  bool IsAChild(mafNode *a);

  /**
    Find a node in all the subtrees, searching recursively into sub nodes.
    Return true if found. */
  bool IsInTree(mafNode *a);

  /** Return the root of the tree this node owns to. */
  mafNode *GetRoot();

  bool IsEmpty() const {return GetNumberOfChildren()==0;}

  /** Valid VMEs have m_ID >= 0. The root has m_Id = 0, other VMEs have m_Id > 0.*/
  bool IsValid() {return m_Id >= 0;};

  /** Return the number of children of this node 
        If only visible is true return the number visible to traverse nodes */
  unsigned long GetNumberOfChildren() const ;
  
  /** Return the number of children of this node 
        If only visible is true return the number visible to traverse nodes */
  unsigned long GetNumberOfChildren(bool onlyVisible);
  
  /**
  Return the pointer to the parent node (if present)*/
  mafNode *GetParent() const {return m_Parent;};

  /**
    Remove recursively all nodes from this tree, forcing all subnodes
    to detach their children. You better use RemoveAllChildren instead!!! */
  void CleanTree();

  /**
    Remove all children nodes. If the children are not referenced by other objects
    they will be automatically deallocated by UnRegister() mechanism, and
    the removal will recurse.*/
  void RemoveAllChildren();
  
  /**
    Return a new Tree iterator already set to traverse 
    the sub tree starting a this node. Remember to delete the iterator after use it.*/
  mafNodeIterator *NewIterator();

  /**
    Set/Get the flag to make this VME visible to tree traversal. mafVMEIterator,
    GetSpaceBounds and Get4DBounds will skip this VME if the flag is OFF.*/
  void SetVisibleToTraverse(bool flag) {m_VisibleToTraverse=flag;}
  bool GetVisibleToTraverse() {return m_VisibleToTraverse;}

  /**  Return true if visible to tree traversal*/
  bool IsVisible() {return m_VisibleToTraverse;}

  /** Compare two nodes. sublcasses should redefine this function. */
  virtual bool Equals(mafNode *vme);

  /**
    Compare the two subtrees starting from this node with the given one. Two trees
    are considered equivalent if they have equivalent nodes (@sa Equals() ), and are 
    disposed in the same hierarchy.
    Order of children node is significative for comparison! */
  bool CompareTree(mafNode *vme);

  /**  return true if node can be reparented under the specified node*/
  virtual bool CanReparentTo(mafNode *parent) {return parent==NULL||!IsInTree(parent);}

  /** redefined to cope with tree registering */
  virtual void UnRegister(void *o);

  /** return a reference to the event source issuing events for this object */
  mafEventSource *GetEventSource() {return m_EventSource;}

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *e);

  typedef std::vector<mafAutoPointer<mafNode> > mafChildrenVector;

  /**
    return list of children. The returned list is a const, since it can be
    modified by means of nodes APIs */
  const mafChildrenVector *GetChildren() {return &m_Children;}

  typedef std::map<mafString,mafAutoPointer<mafAttribute> > mafAttributesMap;

  /** 
    return the list of attributes. Attributes vector can be manipulated
    directly by means of the map container APIs. */
  mafAttributesMap *GetAttributes() {return &m_Attributes;}

  /** Set a new attribute. The given attribute is */
  void SetAttribute(const char *name,mafAttribute *a);

  /** return an attribute given the name */
  mafAttribute *GetAttribute(const char *name);

  /** remove an attibute */
  void RemoveAttribute(const char *name);

  /** remove all the attributes of this node */
  void RemoveAllAttributes();

  /** 
    return a pointer to the tag array attribute. If this attribute doesn't
    exist yet, create a new one. TagArray is a map storing pairs of
    Name<->components, where components are an array of mafStrings. It's a
    simple way to attach persistent attributes. For more complex attributes
    customized classes should be created, inheriting from mafAttribute
    (e.g. @sa mmaMaterial). */
  mafTagArray  *GetTagArray();

  typedef std::map<mafString,mmuNodeLink> mafLinksMap;

  /** 
    return the value of a link to another node in the tree. If no link with
    such a name exists return NULL. */
  mafNode *GetLink(const char *name);

  /** 
  Return the subId associated with the link (used for mafVMELandmark)*/
  mafID GetLinkSubId(const char *name);

  /** set a link to another node in the tree */
  void SetLink(const char *name, mafNode *node, mafID sub_id = -1);

  /** remove a link */
  void RemoveLink(const char *name);

  /** return the number of links stored in this Node */
  mafID GetNumberOfLinks() {return m_Links.size();}

  /** remove all links */
  void RemoveAllLinks();
  
  /** return links array: links from this node to other arrays */
  mafLinksMap *GetLinks() {return &m_Links;}

  /** used to send an event up in the tree */
  void ForwardUpEvent(mafEventBase *maf_event);
  void ForwardUpEvent(mafEventBase &maf_event);

  /** used to send an event down in the tree */
  void ForwardDownEvent(mafEventBase *maf_event);
  void ForwardDownEvent(mafEventBase &maf_event);

  /** IDs for the GUI */
  enum NODE_WIDGET_ID
  {
    ID_NAME = MINID,
    ID_PRINT_INFO,
	ID_HELP,
    ID_LAST
  };

  /** create and return the GUI for changing the node parameters */
  mafGUI *GetGui();

  /** destroy the Gui */
  void DeleteGui();

  /** return the Id of this node in the tree */
  mafID GetId() const;

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Check if m_Id and regenerate it if is invalid (-1) */
  void UpdateId();

  /**
  Return the modification time.*/
  virtual unsigned long GetMTime();

  /** 
  Turn on the flag to calculate the timestamp considering also the linked nodes*/
  void DependsOnLinkedNodeOn() {m_DependsOnLinkedNode = true;};

  /** 
  Turn off the flag to calculate the timestamp considering also the linked nodes*/
  void DependsOnLinkedNodeOff() {m_DependsOnLinkedNode = false;};

protected:

  mafNode();
  virtual ~mafNode();

  /** internally used to set the node ID */
  void SetId(mafID id);

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  //This function is overridden by subclasses to perform custom initialization */
  virtual int InternalInitialize();

  /** to be redefined by subclasses to define the shutdown actions */
  virtual void InternalShutdown() {};

  /**
    This function set the parent for this Node. It returns a value
    to allow subclasses to implement selective reparenting.*/
  virtual int SetParent(mafNode *parent);
  
  /**
    Internally used to create a new instance of the GUI. This function should be
    overridden by subclasses to create specialized GUIs. Each subclass should append
    its own widgets and define the enum of IDs for the widgets as an extension of
    the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
    subclass to continue the ID enumeration from it. For appending the widgets in the
    same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mafGUI  *CreateGui();

  void OnNodeDetachedFromTree(mafEventBase *e);
  void OnNodeAttachedToTree(mafEventBase *e);
  void OnNodeDestroyed(mafEventBase *e);

  mafGUI            *m_Gui;         ///< pointer to the node GUI

  mafChildrenVector m_Children;     ///< list of children
  mafNode           *m_Parent;      ///< parent node

  mafAttributesMap  m_Attributes;   ///< attributes attached to this node

  mafLinksMap       m_Links;        ///< links to other nodes in the tree

  mafString         m_Name;         ///< name of this node
  mafID             m_Id;           ///< ID of this node

  bool m_VisibleToTraverse;         ///< enable/disable traversing visit of this node
  bool m_Initialized;               ///< set true by Initialize()
  bool m_DependsOnLinkedNode;       ///< enable/disable calculation of MTime considering links

  mafEventSource    *m_EventSource; ///< source of events issued by the node
};

#endif
