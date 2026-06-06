#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"
#include "ftk/Base/String.h"
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
#include <set>
#include <string>

BEGIN_FTK_NAMESPACE

class mafStorageElement;
class mafStorageElementBuilder;

namespace model::data_
{
  class Node;

	class FTK_CORE_EXPORT NodeLink final
  {
  public:
    const mafString& GetName() const { return m_Name; }
    //NodeLink(/*mafID id=-1,*/Node* node = nullptr, mafID sub_id = -1) :m_NodeId(-1), m_Node(node), m_NodeSubId(sub_id) {}
    mafID GetId() { return m_NodeId; }
    NodeLink& SetId(mafID id) { m_NodeId = id; return (*this); }
    Node* m_Node;
    mafString m_Name;
    mafID   m_NodeId;
    mafID   m_NodeSubId;
  };

  class FTK_CORE_EXPORT Node : public std::enable_shared_from_this<Node>, public mafEventSource, public mafBaseEventHandler, public mafTimeStamped, public mafObjectWithGUI
  {
  public:
    public:
	  using Rootclass = Node;
	  mafCommonTypeMacro(Node)
	  static bool IsStaticType(const char* type) { return strcmp(type, GetStaticTypeName()) == 0; };
	  static bool IsStaticType(const mafTypeID& type) { return type == GetStaticTypeId(); }
	  virtual const mafTypeID& GetTypeId() const { return GetStaticTypeId(); }
	  virtual const char* GetTypeName() const { return GetStaticTypeName(); }
	  virtual bool IsA(const char* type) const { return IsStaticType(type); }
	  virtual bool IsA(const mafTypeID& type) const { return IsStaticType(type); }
	  virtual Pointer NewInstance() const { return Thisclass::New(); }
	  Rootclass* NewObjectInstance() const { return NewInstance(); }
	  mafNewTypeMacro;

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

    using Children = std::vector<std::shared_ptr<Node> >;
    using Links = std::set<NodeLink, NameCompare<NodeLink> >;
    using Attributes = std::set<std::shared_ptr<model::data::Attribute>, NameComparePtr<model::data::Attribute> >;


    static std::shared_ptr<Node> Create(const char* NodeType);

    Node(const Node&) = delete;
    Node(Node&&) noexcept = delete;
    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&) noexcept = delete;

  	~Node() override;

    void Store(mafStorageElementBuilder& builder) const;

    void Restore(const mafStorageElement& value);

    //virtual void Print(std::ostream& os, int tabs = 0);// const;

    int Initialize();

    void Shutdown();

    bool IsInitialized() { return m_Initialized; }

    const mafString& GetName() { return m_Name; }

    void SetName(const mafString& name);

    //virtual int DeepCopy(Node* a);

    /** TODO: to be moved to mafVME
      perform a copy by simply referencing the copied node's data array.
      Beware: This can allow to save memory when doing special tasks, but
      can be very dangerous making one of the VME inconsistent. Some nodes
      do not support such a function! */
      //virtual int ShallowCopy(Node *a);

    //virtual bool CanCopy(Node* vme);

  	static std::shared_ptr<Node> MakeCopy(Node* a);

  	std::shared_ptr<Node> MakeCopy() { return MakeCopy(this); }

    static std::shared_ptr<Node> CopyTree(Node* vme, Node* parent = NULL);

    //virtual void UpdateLinks(std::vector<std::pair<Node*, Node*> >& nodes);

    std::shared_ptr<Node> CopyTree();

    std::shared_ptr<Node> GetChild(mafID idx, bool onlyVisible = false);

    std::shared_ptr<Node> GetFirstChild(bool onlyVisible = false);

    std::shared_ptr<Node> GetLastChild(bool onlyVisible = false);

    std::shared_ptr<Node> GetByPath(const mafString& path, bool onlyVisible = true);

    int AddChild(std::shared_ptr<Node> node);

    void RemoveChild(mafID idx, bool onlyVisible = false);

    void RemoveChild(Node* node);


    int FindNodeIdx(Node* a, bool onlyVisible = false);

    int FindNodeIdx(const mafString& name, bool onlyVisible = false);


    std::shared_ptr<Node> FindInTreeByTag(const mafTagItem& tag);

    std::shared_ptr<Node> FindInTreeByName(const mafString& name, bool match_case = true, bool whole_word = true);

    std::shared_ptr<Node> FindInTreeById(const mafID id);

    static int ReparentTo(std::shared_ptr<Node> sharedThis, Node* parent);

  	static int ReparentTo(std::shared_ptr<Node> sharedThis, std::shared_ptr<Node> parent);

    void Import(Node* tree);

    bool IsAChild(Node* a);

    void MoveChildUp(int idx);

    void MoveChildDown(Node* child);

    void MoveChildUp(Node* child);

    void MoveChildDown(int idx);

    bool IsInTree(Node* a) const;

    Node* GetRoot();

  	std::shared_ptr<Node> GetRootSPtr();

    bool IsEmpty() const;

    bool IsValid() { return m_Id >= 0; }

    size_t GetNumberOfChildren() const;

    size_t GetNumberOfChildren(bool onlyVisible);

    Node* GetParent() const;

  	std::shared_ptr<Node> GetParentSPtr() const;

    void RemoveAllChildren();

    void SetVisibleToTraverse(bool flag) { m_VisibleToTraverse = flag; }

  	bool GetVisibleToTraverse() { return m_VisibleToTraverse; }

    bool IsVisible() { return m_VisibleToTraverse; }

    //virtual bool Equals(Node* vme);

    bool CompareTree(Node* vme);

    //virtual bool CanReparentTo(Node* parent) { return parent == NULL || !IsInTree(parent); }

    void OnEvent(mafEventBase* e) override{};

    const Children* GetChildren() { return &m_Children; }

    void SetAttribute(const mafString& name, std::shared_ptr<model::data::Attribute> a);

    std::shared_ptr<model::data::Attribute> GetAttribute(const mafString& name);

    std::shared_ptr<const model::data::Attribute> GetAttribute(const mafString& name) const;

    void RemoveAttribute(const mafString& name);

    void RemoveAllAttributes();

    //virtual std::shared_ptr<mafTagArray> GetTagArray();

    Node* GetLink(const mafString& name);

    mafID GetLinkSubId(const mafString& name);

    void SetLink(const mafString&, Node* node, mafID sub_id = -1);

    void RemoveLink(const mafString& name);

    size_t GetNumberOfLinks() { return m_Links.size(); }

    void RemoveAllLinks();

    Links* GetLinks() { return &m_Links; }

    void ForwardUpEvent(mafEventBase* maf_event);

  	void ForwardUpEvent(mafEventBase& maf_event);

  	void ForwardDownEvent(mafEventBase* maf_event);

  	void ForwardDownEvent(mafEventBase& maf_event);

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

    mafID GetId() const;

    static const char** GetIcon();

    void UpdateId();

    MTimeType GetMTime() override;

    //virtual mafID GetNewNodeId();

    //virtual void ReleaseNodeId(mafID id);

    void DependsOnLinkedNodeOn() { m_DependsOnLinkedNode = true; };

    void DependsOnLinkedNodeOff() { m_DependsOnLinkedNode = false; };

  	std::shared_ptr<Node> SharedFromThis();

  protected:

    Node();

    virtual void InternalStore(mafStorageElementBuilder& builder) const;

  	virtual void InternalRestore(const mafStorageElement& value);

    void SetId(mafID id);

    //virtual int InternalInitialize();

    //virtual void InternalShutdown();

    //virtual int OnSetParent(Node* parent);

    void SwapChildren(int idx1, int idx2);

    mafGUI* CreateGui() override { return nullptr; };

    void OnNodeDetachedFromTree(Node* node);

  	void OnNodeAttachedToTree(Node* node);

  	void OnNodeDestroyed(Node* node);

  	void OnPrint();

  private:
    static int SetParentNew(std::shared_ptr<Node> sharedThis, Node* parent);

  	Children m_Children;

  	Node* m_Parent = nullptr;

    Attributes  m_Attributes;

    Links       m_Links;

    mafString         m_Name;

    mafID             m_Id = INVALID_ID;

    bool m_VisibleToTraverse = true;

    bool m_Initialized = false;

    bool m_DependsOnLinkedNode = false;
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
  std::shared_ptr<Node> Parse(const Value& value, io::parse::To<std::shared_ptr<Node>>)
  {
    return Parse(value, io::parse::To<Node>{});
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
