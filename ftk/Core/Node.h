#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"
#include "ftk/Base/String.h"
#include "ftk/Core/NodeIterator.h"
#include "ftk/IO/Parse.h"

#include "mafBaseEventHandler.h"
#include "mafTagItem.h"
#include "mafTimeStamped.h"
#include "mafAttribute.h"
#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafObjectWithGUI.h"
#include "mafEventSender.h"

#include <vector>
#include <map>
#include <set>
#include <string>

BEGIN_FTK_NAMESPACE

class mafStorageElement;
class mafStorageElementBuilder;

namespace model::data
{
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
    class MAF_EXPORT Node : public std::enable_shared_from_this<Node>, public mafEventSource, public mafBaseEventHandler, public mafTimeStamped, public mafObjectWithGUI
    {
    public:
        mafBaseTypeMacro(Node)

        enum
        {
            INVALID_ID = -1
        };
        template<typename T>
        struct NameComparePtr
        {
            bool operator()(const std::shared_ptr<T>& t1, const std::shared_ptr<T>& t2) const { return t1->GetName() < t2->GetName(); }
            bool operator()(const mafString& s, const std::shared_ptr<T>& t) const { return s < t->GetName(); }
            bool operator()(const std::shared_ptr<T>& t, const mafString& s) const { return t->GetName() < s; }
            using is_transparent = std::true_type;
        };
        template<typename T>
        struct NameCompare
        {
            bool operator()(const T& t1, const T& t2) const { return t1.GetName() < t2.GetName(); }
            bool operator()(const mafString& s, const T& t) const { return s < t.GetName(); }
            bool operator()(const T& t, const mafString& s) const { return t.GetName() < s; }
            using is_transparent = std::true_type;
        };

        /** data structure used to store a link VME and its Id */
        class NodeLink final
        {
        public:
            NodeLink(/*mafID id=-1,*/Node* node = nullptr, mafID sub_id = -1) :m_NodeId(-1), m_Node(node), m_NodeSubId(sub_id) {}
            Node* GetNode() const { return m_Node; }
            NodeLink& SetNode(Node* node) { m_Node = node; return (*this); }
            mafID GetId() const { return m_NodeId; }
            NodeLink& SetId(mafID id) { m_NodeId = id; return (*this); }
            mafID GetSubId() const { return m_NodeSubId; }
            NodeLink& SetSubId(mafID subId) { m_NodeSubId = subId; return (*this); }
        private:
            Node* m_Node;
            mafID   m_NodeSubId;
            mafID   m_NodeId;
        };

        using Children = std::vector<std::shared_ptr<Node> >;
        using Links = std::map<mafString, NodeLink>;
        using Attributes = std::set<std::shared_ptr<model::data::Attribute>, NameComparePtr<model::data::Attribute> >;
        using Iterator = model::data::NodeIterator<Node>;

        static std::shared_ptr<Node> Create(const char* NodeType);
        /** print a dump of this object */
        virtual void Print(std::ostream& os, const int tabs = 0);// const;

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
        bool IsInitialized() { return m_Initialized; }


        /** return the name of this node*/
        const mafString& GetName() { return m_Name; }

        /** set node name */
        void SetName(const mafString& name);

        /**
          Copy the contents of another node into this one. Notice that subtrees
          are not copied, i.e. copy is not recursive!
          Concrete nodes should reimplement this function to verify admitted
          conversion. */
        virtual int DeepCopy(Node* a);

        /** TODO: to be moved to mafVME
          perform a copy by simply referencing the copied node's data array.
          Beware: This can allow to save memory when doing special tasks, but
          can be very dangerous making one of the VME inconsistent. Some nodes
          do not support such a function! */
          //virtual int ShallowCopy(Node *a);

        Iterator begin();

        Iterator end();

        /** Test if the given node instance can be copied into this. This function should
          be reimplemented into subclasses classes*/
        virtual bool CanCopy(Node* vme);

        /** Create a copy of this node (do not copy the sub tree,just the node) */
        static std::shared_ptr<Node> MakeCopy(Node* a);
        std::shared_ptr<Node> MakeCopy() { return MakeCopy(this); }

        /** Copy the given VME tree into a new tree. In case a parent is provided, link the new
          root node to it. Return the root of the new tree.*/
        static std::shared_ptr<Node> CopyTree(Node* vme, Node* parent = NULL);

        virtual void UpdateLinks(std::vector<std::pair<Node*, Node*> >& nodes);

        /** Make a copy of the whole subtree and return its pointer */
        std::shared_ptr<Node> CopyTree();

        /** Return a the pointer to a child given its index.
            If only visible is true return the idx-th visible to traverse node */
        std::shared_ptr<Node> GetChild(mafID idx, bool onlyVisible = false);

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
        std::shared_ptr<Node> GetByPath(const mafString& path, bool onlyVisible = true);

        /** Add a child to this node. Return MAF_OK if success.*/
        int AddChild(std::shared_ptr<Node> node);

        /** Remove a child node*/
        void RemoveChild(mafID idx, bool onlyVisible = false);
        /** Remove a child node*/
        void RemoveChild(Node* node);

        /** Find a child given its pointer and return its index. Return -1 in case of not found or failure.
            If only visible is true return the idx of visible to traverse nodes subset */
        int FindNodeIdx(Node* a, bool onlyVisible = false);

        /** Find a child index given its name. Search is performed only on first level children not
          in the sub-tree. Return -1 in case of not found or failure.
          If only visible is true return the idx of visible to traverse nodes subset */
        int FindNodeIdx(const mafString& name, bool onlyVisible = false);

        /** Find a node in all the subtrees matching the given TagName/TagValue pair.*/
        std::shared_ptr<Node> FindInTreeByTag(const mafTagItem& tag);

        /** Find a node in all the subtrees matching the given VME Name.*/
        std::shared_ptr<Node> FindInTreeByName(const mafString& name, bool match_case = true, bool whole_word = true);

        /** Find a node in all the subtrees matching the given VME Name.*/
        std::shared_ptr<Node> FindInTreeById(const mafID id);

        /**
          Reparent this Node into a different place of the same tree.
          BEWARE: Reparent into a different tree is allowed, but could
          generate bad problems. Inherited classes should reimplement this
          function to avoid these problems when reparenting to different trees.
          To move a node into a different tree you better use DeepCopy to copy
          it into a Node of that tree.*/
        static int ReparentTo(std::shared_ptr<Node> sharedThis, Node* parent);
        static int ReparentTo(std::shared_ptr<Node> sharedThis, std::shared_ptr<Node> parent);

        /** Import all children of another tree into this tree */
        void Import(Node* tree);

        /** Return true if the given one is a child of this node.*/
        bool IsAChild(Node* a);

        /** Moves child with given index up.*/
        void MoveChildUp(int idx);

        /** Moves child with given index down.*/
        void MoveChildDown(Node* child);

        /** Moves child with given index up.*/
        void MoveChildUp(Node* child);

        /** Moves child with given index down.*/
        void MoveChildDown(int idx);

        /**
          Find a node in all the subtrees, searching recursively into sub nodes.
          Return true if found. */
        bool IsInTree(Node* a) const;

        /** Return the root of the tree this node owns to. */
        Node* GetRoot();
        std::shared_ptr<Node> GetRootSPtr();

        bool IsEmpty() const;

        /** Valid VMEs have m_ID >= 0. The root has m_Id = 0, other VMEs have m_Id > 0.*/
        bool IsValid() { return m_Id >= 0; };

        /** Return the number of children of this node
              If only visible is true return the number visible to traverse nodes */
        size_t GetNumberOfChildren() const;

        /** Return the number of children of this node
              If only visible is true return the number visible to traverse nodes */
        size_t GetNumberOfChildren(bool onlyVisible);

        /**
        Return the pointer to the parent node (if present)*/
        Node* GetParent() const;
        std::shared_ptr<Node> GetParentSPtr() const;

        /**
          Remove all children nodes. If the children are not referenced by other objects
          they will be automatically deallocated by UnRegister() mechanism, and
          the removal will recurse.*/
        void RemoveAllChildren();

        /**
          Set/Get the flag to make this VME visible to tree traversal. mafVMEIterator,
          GetSpaceBounds and Get4DBounds will skip this VME if the flag is OFF.*/
        void SetVisibleToTraverse(bool flag) { m_VisibleToTraverse = flag; }
        bool GetVisibleToTraverse() { return m_VisibleToTraverse; }

        /**  Return true if visible to tree traversal*/
        bool IsVisible() { return m_VisibleToTraverse; }

        /** Compare two nodes. sublcasses should redefine this function. */
        virtual bool Equals(Node* vme);

        /**
          Compare the two subtrees starting from this node with the given one. Two trees
          are considered equivalent if they have equivalent nodes (@sa Equals() ), and are
          disposed in the same hierarchy.
          Order of children node is significative for comparison! */
        bool CompareTree(Node* vme);

        /**  return true if node can be reparented under the specified node*/
        virtual bool CanReparentTo(Node* parent) { return parent == NULL || !IsInTree(parent); }

        /** Precess events coming from other objects */
        void OnEvent(mafEventBase* e) override;

        void SetAttribute(std::shared_ptr<mafAttribute> a);
        std::shared_ptr<mafAttribute> GetAttribute(const mafString& name);
        std::shared_ptr<const mafAttribute> GetAttribute(const mafString& name) const;
        void RemoveAttribute(const mafString& name);
        void RemoveAllAttributes();

        /**
          return a pointer to the tag array attribute. If this attribute doesn't
          exist yet, create a new one. TagArray is a map storing pairs of
          Name<->components, where components are an array of mafStrings. It's a
          simple way to attach persistent attributes. For more complex attributes
          customized classes should be created, inheriting from mafAttribute
          (e.g. @sa mmaMaterial). */
        virtual std::shared_ptr<mafTagArray> GetTagArray();

        auto& GetLinks() { return m_Links; }
        auto& GetLinks() const { return m_Links; }
        size_t GetNumberOfLinks() const { return m_Links.size(); }
        Node* GetLink(const mafString& name);
        mafID GetLinkSubId(const mafString& name);
        void SetLink(const mafString&, Node* node, mafID sub_id = -1);
        void RemoveLink(const mafString& name);
        void RemoveAllLinks();

        /** used to send an event up in the tree */
        void ForwardUpEvent(mafEventBase* maf_event);
        void ForwardUpEvent(mafEventBase& maf_event);

        /** used to send an event down in the tree */
        void ForwardDownEvent(mafEventBase* maf_event);
        void ForwardDownEvent(mafEventBase& maf_event);

        /** IDs for the GUI */
        enum BASENODE_WIDGET_ID
        {
            ID_NAME = MINID,
            ID_PRINT_INFO,
            ID_MOVEUP,
            ID_MOVEDN,
            ID_MOVERIGHT,
            ID_MOVELEFT,
            ID_HELP,
            ID_LAST
        };

        /** return the Id of this node in the tree */
        mafID GetId() const;

        /** return an xpm-icon that can be used to represent this node */
        static const char** GetIcon();

        /** Check if m_Id and regenerate it if is invalid (-1) */
        void UpdateId();

        /**
        Return the modification time.*/
        MTimeType GetMTime() override;

        /** Find new unique id */
        virtual mafID GetNewNodeId();

        /** Find new unique id */
        virtual void ReleaseNodeId(mafID id);

        /**
        Turn on the flag to calculate the timestamp considering also the linked nodes*/
        void DependsOnLinkedNodeOn() { m_DependsOnLinkedNode = true; };

        /**
        Turn off the flag to calculate the timestamp considering also the linked nodes*/
        void DependsOnLinkedNodeOff() { m_DependsOnLinkedNode = false; };

        void Store(mafStorageElementBuilder& element) { InternalStore(element); }
        void Restore(const mafStorageElement& element) { InternalRestore(element); }

        ~Node() override;
        std::shared_ptr<Node> SharedFromThis();

    protected:

        Node();

        /** internally used to set the node ID */
        void SetId(mafID id);

        virtual void InternalStore(mafStorageElementBuilder& parent);
        virtual void InternalRestore(const mafStorageElement& node);

        //This function is overridden by subclasses to perform custom initialization */
        virtual int InternalInitialize();

        /** to be redefined by subclasses to define the shutdown actions */
        virtual void InternalShutdown();

        /**
          This function set the parent for this Node. It returns a value
          to allow subclasses to implement selective reparenting.*/
        virtual int OnSetParent(Node* parent);

        /** Swaps children in given positions.*/
        void SwapChildren(int idx1, int idx2);

        /**
          Internally used to create a new instance of the GUI. This function should be
          overridden by subclasses to create specialized GUIs. Each subclass should append
          its own widgets and define the enum of IDs for the widgets as an extension of
          the superclass enum. The last id value must be defined as "LAST_ID" to allow the
          subclass to continue the ID enumeration from it. For appending the widgets in the
          same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
        mafGUI* CreateGui() override;

        void OnNodeDetachedFromTree(Node* node);
        void OnNodeAttachedToTree(Node* node);
        void OnNodeDestroyed(Node* node);
        void OnPrint();

    private:
        static int SetParentNew(std::shared_ptr<Node> sharedThis, Node* parent);
        Children m_Children;     ///< list of children
        Node* m_Parent = nullptr;      ///< parent node

        Attributes  m_Attributes;   ///< attributes attached to this node

        Links       m_Links;        ///< links to other nodes in the tree

        mafString         m_Name;         ///< name of this node
        mafID             m_Id = INVALID_ID;           ///< ID of this node

        bool m_VisibleToTraverse = true;         ///< enable/disable traversing visit of this node
        bool m_Initialized = false;               ///< set true by Initialize()
        bool m_DependsOnLinkedNode = false;       ///< enable/disable calculation of MTime considering links
    };

    template<class Value>
    std::shared_ptr<Node> Parse(const Value& value, io::parse::To<Node>)
    {
        mafString type_name = value(_R("Type")).template As<mafString>();
        if (auto node = Node::Create(type_name.GetCStr()))
        {
            node->Restore(value);
            return node;
        }
        return nullptr;
    }

    template<class Value>
    void Serialize(Value& value, Node* const& node)
    {
        assert(node);
        mafString type_name = _R(node->GetTypeName());
        value(_R("Type")).SetValue(type_name);
        node->Store(value);
    }
}

END_FTK_NAMESPACE
