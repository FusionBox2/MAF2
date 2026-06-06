#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"
#include "ftk/Base/String.h"
#include "ftk/Core/NodeIterator.h"
#include "ftk/IO/Parse.h"

#include "mafBaseEventHandler.h"
#include "mafTagItem.h"
#include "mafTimeStamped.h"
#include "mafDecl.h"
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
    class Attribute;
}
class mafTagArray;
using mafAttribute = model::data::Attribute;

namespace model::data0
{
    class MAF_EXPORT Node : public std::enable_shared_from_this<Node>, public mafEventSource, public mafBaseEventHandler, public mafTimeStamped, public mafObjectWithGUI
    {
    public:
        mafBaseTypeMacro(Node)

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

        class NodeLink final
        {
        public:

        	NodeLink(std::weak_ptr<Node> node = {}, mafID sub_id = -1) : m_Node(std::move(node)), m_NodeSubId(sub_id){}

        	std::shared_ptr<Node> GetNode() const { return m_Node.lock(); }

        	NodeLink& SetNode(const std::shared_ptr<Node>& node) { m_Node = node; return (*this); }

        	mafID GetId() const { return m_NodeId; }

        	NodeLink& SetId(mafID id) { m_NodeId = id; return (*this); }

        	mafID GetSubId() const { return m_NodeSubId; }

        	NodeLink& SetSubId(mafID subId) { m_NodeSubId = subId; return (*this); }

        private:

        	std::weak_ptr<Node> m_Node;

        	mafID   m_NodeId = -1;

        	mafID   m_NodeSubId;
        };

        using Children = std::vector<std::shared_ptr<Node> >;
        using Links = std::map<mafString, NodeLink>;
        using Attributes = std::set<std::shared_ptr<model::data::Attribute>, NameComparePtr<model::data::Attribute> >;
        using Iterator = model::data::NodeIterator<Node>;

        static std::shared_ptr<Node> Create(const char* NodeType);

    protected:

        Node();

    public:

        Node(const Node&) = delete;

        Node(Node&&) = delete;

        Node& operator=(const Node&) = delete;

        Node& operator=(Node&&) = delete;

        ~Node() override;

        virtual bool Equals(Node* node);

        virtual bool CanCopy(Node* node);

        virtual int DeepCopy(Node* node);

        std::shared_ptr<Node> MakeCopy();

        bool IsInitialized() const;

        int Initialize();

        void Shutdown();

    protected:

        virtual int InternalInitialize();

        virtual void InternalShutdown();

    public:

        const mafString& GetName() const;

        void SetName(const mafString& name);

        mafID GetId() const;

        void SetId(mafID id);

        mafID BuildIds(mafID id = 0);

        virtual void RenewIds(Node *node);

        void RestoreLinks();

        void Store(mafStorageElementBuilder& builder);

        void Restore(const mafStorageElement& value);

    protected:

        virtual void InternalStore(mafStorageElementBuilder& builder);

        virtual void InternalRestore(const mafStorageElement& value);

    public:

        std::shared_ptr<mafAttribute> GetAttribute(const mafString& name);

        std::shared_ptr<const mafAttribute> GetAttribute(const mafString& name) const;

        virtual std::shared_ptr<mafTagArray> GetTagArray();

        void SetAttribute(std::shared_ptr<mafAttribute> a);

        void RemoveAttribute(const mafString& name);

        void RemoveAllAttributes();

        /** TODO: to be moved to mafVME
          perform a copy by simply referencing the copied node's data array.
          Beware: This can allow to save memory when doing special tasks, but
          can be very dangerous making one of the VME inconsistent. Some nodes
          do not support such a function! */
          //virtual int ShallowCopy(Node *a);

        bool IsEmpty() const;

        size_t GetNumberOfChildren(bool onlyVisible = false) const;

        bool IsAChild(Node* a) const;

        std::shared_ptr<Node> GetChild(mafID idx, bool onlyVisible = false);

        int AddChild(std::shared_ptr<Node> node);

        void RemoveChild(mafID idx, bool onlyVisible = false);

        void RemoveChild(Node* node);

        void RemoveAllChildren();

        bool IsInTree(Node* a) const;

        Node* GetParent() const;

        Node* GetRoot();

        bool IsVisible() const;

        bool GetVisibleToTraverse() const;

        void SetVisibleToTraverse(bool flag);

        Iterator begin();

        Iterator end();

        static std::shared_ptr<Node> CopyTree(Node* vme, Node* parent = nullptr);

        std::shared_ptr<Node> CopyTree();

        std::shared_ptr<Node> GetByPath(const mafString& path, bool onlyVisible = true);

        int FindNodeIdx(Node* a, bool onlyVisible = false);

        int FindNodeIdx(const mafString& name, bool onlyVisible = false);

        static std::shared_ptr<Node> FindInTreeByTag(std::shared_ptr<Node> sharedThis, const mafTagItem& tag);

        static std::shared_ptr<Node> FindInTreeByName(std::shared_ptr<Node> sharedThis, const mafString& name, bool match_case = true, bool whole_word = true);

        static std::shared_ptr<Node> FindInTreeById(std::shared_ptr<Node> sharedThis, mafID id);

        static int ReparentTo(std::shared_ptr<Node> sharedThis, Node* parent);

        bool CompareTree(Node* node);

        virtual bool CanReparentTo(Node* parent);

    protected:

        virtual int OnSetParent(Node* parent);

        void OnNodeDetachedFromTree(Node* node);

        void OnNodeAttachedToTree(Node* node);

        void OnNodeDestroyed(Node* node);

    private:

        static int SetParentNew(std::shared_ptr<Node> sharedThis, Node* parent);

    public:

        size_t GetNumberOfLinks() const { return m_Links.size(); }

        auto& GetLinks() { return m_Links; }

    	auto& GetLinks() const { return m_Links; }

    	Node* GetLink(const mafString& name);

    	mafID GetLinkSubId(const mafString& name);

    	void SetLink(const mafString&, Node* node, mafID sub_id = -1);

    	void RemoveLink(const mafString& name);

    	void RemoveAllLinks();

        void OnEvent(mafEventBase* e) override;

        void ForwardUpEvent(mafEventBase* maf_event);

    	void ForwardUpEvent(mafEventBase& maf_event);

        void ForwardDownEvent(mafEventBase* maf_event);

    	void ForwardDownEvent(mafEventBase& maf_event);

        enum BASENODE_WIDGET_ID
        {
            ID_NAME = MINID,
            ID_PRINT_INFO,
            ID_HELP,
            ID_LAST
        };

        virtual void UpdateLinks(std::vector<std::pair<Node*, Node*> >& nodes);

        static const char** GetIcon();

        MTimeType GetMTime() override;

        void DependsOnLinkedNodeOn() { m_DependsOnLinkedNode = true; }

        void DependsOnLinkedNodeOff() { m_DependsOnLinkedNode = false; }

        std::shared_ptr<Node> SharedFromThis();

        virtual void Print(std::ostream& os, const int tabs = 0);// const;

    protected:

        mafGUI* CreateGui() override;

    	void OnPrint();

    private:

    	bool m_Initialized = false;

    	Children m_Children;

    	Node* m_Parent = nullptr;

        Attributes m_Attributes;

        Links m_Links;

        mafString m_Name;

    	mafID m_Id = -1;

        bool m_VisibleToTraverse = true;

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
    auto Parse(const Value& value, io::parse::To<std::shared_ptr<Node>>)
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
