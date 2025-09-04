#include "Node.h"

#include "mafDecl.h"

#include "ftk/Core/NodeFactory.h"
#include "mafIndent.h"
#include "mafEventIO.h"
#include "ftk/IO/StorageElement.h"
#include "mafStorage.h"
#include "mafGUI.h"
#include <wx/tokenzr.h>
#include <assert.h>

#ifdef VTK_USE_ANSI_STDLIB
#include <sstream>
#endif
#include <strstream>

#define UpdateUpDownAvailability(node)\
do\
{\
  if(node == nullptr)\
    break;\
  if(node->m_Gui == nullptr)\
    break;\
  bool up = false;\
  bool down = false;\
  bool right = false;\
  bool left = false;\
  if(node->m_Parent != nullptr)\
  {\
    up    = (node != node->m_Parent->GetChild(0).get());\
    down  = (node != node->m_Parent->GetChild(node->m_Parent->GetNumberOfChildren() - 1).get());\
    left  = (node == node->m_Parent->GetChild(0).get() && node->m_Parent->m_Parent != nullptr);\
    right = (node != node->m_Parent->GetChild(0).get());\
  }\
  node->m_Gui->Enable(ID_MOVEUP, up);\
  node->m_Gui->Enable(ID_MOVEDN, down);\
  node->m_Gui->Enable(ID_MOVERIGHT, right);\
  node->m_Gui->Enable(ID_MOVELEFT, left);\
}\
while(0)

BEGIN_FTK_NAMESPACE

namespace model::data
{
	/** IDs for the GUI */
    enum NODE_WIDGET_ID
    {
        ID_HELP = MINID,
        ID_LAST
    };

    //-------------------------------------------------------------------------
    Node::Node()
        //-------------------------------------------------------------------------
    {
        SetChannel(MCH_NODE);
    }

    //-------------------------------------------------------------------------
    Node::~Node()
        //-------------------------------------------------------------------------
    {
        // advise observers this is being destroyed
        InvokeEvent(this, NODE_DESTROYED);
        RemoveAllChildren();
        RemoveAllLinks();
        RemoveAllAttributes();
        Shutdown();
        //SetParent(NULL);//at this point parent should be already NULL, as the pointer from their is destroyed (referencecount == 0, calling destructor)
    }

    //------------------------------------------------------------------------------
    void Node::SetId(mafID id)
        //------------------------------------------------------------------------------
    {
        m_Id = id;
        Modified();
    }

    //------------------------------------------------------------------------------
    mafID Node::GetId() const
        //------------------------------------------------------------------------------
    {
        return m_Id;
    }

    //------------------------------------------------------------------------------
    int Node::Initialize()
        //------------------------------------------------------------------------------
    {
        if (m_Initialized)
            return MAF_OK;

        if (this->InternalInitialize() == MAF_OK)
        {
            m_Initialized = true;
            return MAF_OK;
        }

        return MAF_ERROR;

    }

    //------------------------------------------------------------------------------
    void Node::Shutdown()
        //------------------------------------------------------------------------------
    {
        if (m_Initialized)
        {
            InternalShutdown();
            m_Initialized = false;
        }
    }

    //-------------------------------------------------------------------------
    int Node::InternalInitialize()
        //-------------------------------------------------------------------------
    {
        auto root = GetRoot();

        for (auto& entry : m_Links)
        {
            auto& link = entry.second;
            if (link.GetNode() == nullptr && link.GetId() >= 0)
            {
                auto node = root->FindInTreeById(link.GetId());
                assert(node);
                if (node)
                {
                    // attach linked node to this one
                    link.SetNode(node.get());
                    node->AddObserver(this);
                }
            }
        }

        for (auto& child : m_Children)
        {
            if (child->Initialize() == MAF_ERROR)
                return MAF_ERROR;
        }
        return MAF_OK;
    }
    //-------------------------------------------------------------------------
    void Node::InternalShutdown()
        //-------------------------------------------------------------------------
    {
        // shutdown children
        for (auto& child : m_Children)
            child->Shutdown();
    }

    //-------------------------------------------------------------------------
    void Node::ForwardUpEvent(mafEventBase& maf_event)
        //-------------------------------------------------------------------------
    {
        ForwardUpEvent(&maf_event);
    }

    //-------------------------------------------------------------------------
    void Node::ForwardUpEvent(mafEventBase* maf_event)
        //-------------------------------------------------------------------------
    {
        if (m_Parent)
        {
            maf_event->SetChannel(MCH_UP);
            m_Parent->OnEvent(maf_event);
        }
    }
    //-------------------------------------------------------------------------
    void Node::ForwardDownEvent(mafEventBase& maf_event)
        //-------------------------------------------------------------------------
    {
        ForwardDownEvent(&maf_event);
    }

    //-------------------------------------------------------------------------
    void Node::ForwardDownEvent(mafEventBase* maf_event)
        //-------------------------------------------------------------------------
    {
        if (GetNumberOfChildren() > 0)
        {
            for (auto& child : m_Children)
            {
                maf_event->SetChannel(MCH_DOWN);
                child->OnEvent(maf_event);
            }
        }
    }

    //-------------------------------------------------------------------------
    void Node::SetName(const mafString& name)
        //-------------------------------------------------------------------------
    {
        m_Name = name; // force string copy
        Modified();
        mafEvent ev(this, VME_MODIFIED); ev.SetVme(this);
        InvokeEvent(ev);
        ForwardUpEvent(ev);
        ForwardDownEvent(ev);
    }

    //-------------------------------------------------------------------------
    size_t Node::GetNumberOfChildren() const
        //-------------------------------------------------------------------------
    {
        return m_Children.size();
    }

    //-------------------------------------------------------------------------
    size_t Node::GetNumberOfChildren(bool onlyVisible /*=false*/)
        //-------------------------------------------------------------------------
    {
        //This function is redefined because the original is defined const and 
        //here we call non-const functions
        if (onlyVisible)
        {
            size_t visibleNodes = 0;
            //counting visible nodes
            for (auto& child : m_Children)
                if (child->IsVisible())
                    visibleNodes++;
            return visibleNodes;
        }
        else
        {
            return m_Children.size();
        }
    }

    //-------------------------------------------------------------------------
    bool Node::IsAChild(Node* a)
        //-------------------------------------------------------------------------
    {
        return a->GetParent() == this;
    }

    //-------------------------------------------------------------------------
    std::shared_ptr<Node> Node::GetChild(mafID idx, bool onlyVisible)
        //-------------------------------------------------------------------------
    {
        if (onlyVisible)
        {
            size_t count = 0;
            for (auto& child : m_Children)
            {
                if (child->IsVisible())
                {
                    if (idx == count)
                    {
                        return child;
                    }
                    ++count;
                }
            }
            return nullptr;
        }
        if (idx >= 0 && idx < m_Children.size())
        {
            return m_Children[idx];
        }
        return nullptr;
    }

    //-------------------------------------------------------------------------
    int Node::FindNodeIdx(Node* a, bool onlyVisible)
        //-------------------------------------------------------------------------
    {
        int nChild = 0;
        for (auto& child : m_Children)
        {
            if (!onlyVisible || child->IsVisible())
            {
                if (child.get() == a)
                {
                    return nChild;
                }
                ++nChild;
            }
        }
        return -1;
    }

    //-------------------------------------------------------------------------
    int Node::FindNodeIdx(const mafString& name, bool onlyVisible)
        //-------------------------------------------------------------------------
    {
        int nChild = 0;
        for (auto& child : m_Children)
        {
            if (!onlyVisible || child->IsVisible())
            {
                if (child->GetName() == name)
                {
                    return nChild;
                }
                ++nChild;
            }
        }
        return -1;
    }
    //-------------------------------------------------------------------------
    std::shared_ptr<Node> Node::FindInTreeByTag(const mafTagItem& tag)
        //-------------------------------------------------------------------------
    {
        if (auto titem = GetTagArray()->GetTag(tag.GetName()))
        {
            if (tag == *titem)
                return SharedFromThis();
        }

        for (auto& child : m_Children)
        {
            if (auto node = child->FindInTreeByTag(tag))
            {
                return node;
            }
        }
        return nullptr;
    }
    //-------------------------------------------------------------------------
    std::shared_ptr<Node> Node::FindInTreeByName(const mafString& name, bool match_case, bool whole_word)
        //-------------------------------------------------------------------------
    {
        if (match_case)
        {
            if (whole_word && GetName() == name)
                return SharedFromThis();
            if (!whole_word && GetName().find(name) != mafString::npos)
                return SharedFromThis();
        }
        else
        {
            mafString word_to_search;
            mafString myName;
            if (match_case)
            {
                word_to_search = name;
                myName = GetName();
            }
            else
            {
                word_to_search = name.Lower();
                myName = GetName().Lower();
            }

            if (whole_word && myName == word_to_search)
                return SharedFromThis();
            if (!whole_word && myName.find(word_to_search) != mafString::npos)
                return SharedFromThis();

        }
        for (auto& child : m_Children)
        {
            if (auto node = child->FindInTreeByName(name, match_case, whole_word))
            {
                return node;
            }
        }
        return nullptr;
    }
    //-------------------------------------------------------------------------
    std::shared_ptr<Node> Node::FindInTreeById(const mafID id)
        //-------------------------------------------------------------------------
    {
        if (GetId() == id)
        {
            return SharedFromThis();
        }
        for (auto& child : m_Children)
        {
            if (auto node = child->FindInTreeById(id))
            {
                return node;
            }
        }
        return nullptr;
    }
    //-------------------------------------------------------------------------
    int Node::AddChild(std::shared_ptr<Node> node)
        //-------------------------------------------------------------------------
    {
        // We cannot reparent to a subnode!!!
        if (node->IsInTree(this))
            return MAF_ERROR;
        return ReparentTo(node, this);
    }

    //-------------------------------------------------------------------------
    void Node::RemoveChild(Node* node)
        //-------------------------------------------------------------------------
    {
        if (!node)
        {
            mafWarningMacro("Trying to remove NULL child node");
            return;
        }
        if (!IsAChild(node))
        {
            mafWarningMacro("Trying to remove node that is not a child of this node");
        }
        auto pntr = node->SharedFromThis();
        SetParentNew(pntr, nullptr);
    }

    //-------------------------------------------------------------------------
    void Node::RemoveChild(mafID idx, bool onlyVisible)
        //-------------------------------------------------------------------------
    {
        if (auto oldnode = GetChild(idx, onlyVisible))
        {
            RemoveChild(oldnode.get());
            return;
        }
        mafWarningMacro("Trying to remove a child node with wrong index: " << idx);
    }

    //-------------------------------------------------------------------------
    int Node::ReparentTo(std::shared_ptr<Node> sharedThis, Node* newparent)
        //-------------------------------------------------------------------------
    {
        // Add this node to the new parent children list and
        // remove it from old parent children list.
        // We first add it to the new parent, thus it is registered
        // from the new parent, the we remove it from the list of the old parent.
        // We must keep the oldparent pointer somewhere since it is overwritten
        // by AddChild.
        // self register to preserve from distruction
        auto pntr = sharedThis;
        if ((SetParentNew(sharedThis, nullptr) == MAF_OK) && (SetParentNew(sharedThis, newparent) == MAF_OK))
            return MAF_OK;
        return MAF_ERROR;
    }

    //-------------------------------------------------------------------------
    Node* Node::GetRoot()
        //-------------------------------------------------------------------------
    {
        for (auto node = this; node; node = node->GetParent())
        {
            if (node->GetParent() == nullptr)
            {
                return node;
            }
        }
        return nullptr;
    }

    //-------------------------------------------------------------------------
    bool Node::IsEmpty() const
        //-------------------------------------------------------------------------
    {
        return m_Children.empty();
    }

    //-------------------------------------------------------------------------
    bool Node::IsInTree(Node* a) const
        //-------------------------------------------------------------------------
    {
        for (auto node = a; node; node = node->GetParent())
        {
            if (this == node)
                return true;
        }
        return false;
    }

    //-------------------------------------------------------------------------
    void Node::RemoveAllChildren()
        //-------------------------------------------------------------------------
    {
        size_t num = this->GetNumberOfChildren();
        for (size_t i = 0; i < num; i++)
        {
            auto curr = this->GetChild(num - i - 1);
            if (curr.get())
                SetParentNew(curr, nullptr);
        }
        m_Children.clear();
    }

    //------------------------------------------------------------------------------
    int Node::OnSetParent(Node* parent) { return MAF_OK; }
    //-------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    int Node::SetParentNew(std::shared_ptr<Node> sharedThis, Node* parent)
        //-------------------------------------------------------------------------
    {
        auto _Parent = sharedThis->GetParent();
        // reparenting to NULL is admitted in any case
        if ((parent != nullptr && !sharedThis->CanReparentTo(parent)) || sharedThis->IsInTree(parent))
        {
            // modified by Stefano 27-10-2004: Changed the error macro to give feedback about node names 
            mafErrorMacro("Cannot reparent the VME: " << sharedThis->GetName().GetCStr() << " under the " << parent->GetTypeName() << " named " << parent->GetName().GetCStr());
            return MAF_ERROR;
        }
        if (_Parent == parent)
        {
            return MAF_OK;
        }
        auto pntr = sharedThis;//self protection from destruction
        Node* old_root = (_Parent ? _Parent->GetRoot() : nullptr);
        Node* new_root = (parent ? parent->GetRoot() : nullptr);

        // if the Node was attached to another tree, first send detaching event
        if (old_root != nullptr)
        {
            if (new_root != old_root)
            {
                { mafEventBase evUnq(sharedThis.get(), NODE_DETACHED_FROM_TREE); sharedThis->InvokeEvent(&evUnq); }
                { mafEventBase evUnq(sharedThis.get(), NODE_DETACHED_FROM_TREE); sharedThis->ForwardUpEvent(&evUnq); }
                { mafEventBase evUnq(sharedThis.get(), NODE_DETACHED_FROM_TREE, NULL, MCH_DOWN); sharedThis->OnEvent(&evUnq); }
            }
            int idx = _Parent->FindNodeIdx(sharedThis.get());
            if (idx == -1)
            {
                mafWarningMacro("Child index could not be found");
            }
            else
            {
                Node* prev = (idx > 0) ? _Parent->m_Children[idx - 1].get() : nullptr;
                Node* next = (idx < _Parent->m_Children.size() - 1) ? _Parent->m_Children[idx + 1].get() : nullptr;
                _Parent->m_Children.erase(_Parent->m_Children.begin() + idx);
                UpdateUpDownAvailability(prev);
                UpdateUpDownAvailability(next);
                _Parent->Modified();
            }
        }

        sharedThis->m_Parent = parent;
        _Parent = parent;

        for (auto& n : *sharedThis)
        {
            n.UpdateId();
        }

        if (new_root != nullptr)
        {
            if (_Parent->IsInitialized() && (sharedThis->Initialize() == MAF_ERROR))
                return MAF_ERROR;
            Node* prev = (_Parent->m_Children.size() > 0) ? _Parent->m_Children[_Parent->m_Children.size() - 1].get() : nullptr;
            _Parent->m_Children.push_back(sharedThis);
            UpdateUpDownAvailability(prev);
            _Parent->Modified();
            if (new_root != old_root)
            {
                { mafEventBase evUnq(sharedThis.get(), NODE_ATTACHED_TO_TREE); sharedThis->InvokeEvent(&evUnq); }
                { mafEventBase evUnq(sharedThis.get(), NODE_ATTACHED_TO_TREE); sharedThis->ForwardUpEvent(&evUnq); }
                { mafEventBase evUnq(sharedThis.get(), NODE_ATTACHED_TO_TREE, nullptr, MCH_DOWN); sharedThis->OnEvent(&evUnq); }
            }
        }
        UpdateUpDownAvailability(sharedThis.get());
        sharedThis->Modified();
        return sharedThis->OnSetParent(parent);
    }
    //-------------------------------------------------------------------------
    std::shared_ptr<Node> Node::MakeCopy(Node* a)
        //-------------------------------------------------------------------------
    {
        std::shared_ptr<Node> newnode(a->NewInstance());
        if (newnode)
            newnode->DeepCopy(a);
        return newnode;
    }

    //-------------------------------------------------------------------------
    int Node::DeepCopy(Node* a)
        //-------------------------------------------------------------------------
    {
        assert(a);
        if (!a)
        {
            mafErrorMacro("Cannot copy NULL into Node");
            return MAF_ERROR;
        }
        if (!CanCopy(a))
        {
            mafErrorMacro("Cannot copy Node of type " << a->GetTypeName() << " into a Node of type " << GetTypeName());
            return MAF_ERROR;
        }
        // Copy attributes
        RemoveAllAttributes();
        for (auto& elem : a->m_Attributes)
        {
            auto attr = elem.get();
            assert(attr);
            m_Attributes.insert(attr->MakeCopy());
        }
        SetName(a->GetName());

        RemoveAllLinks();
        for (auto& lnk : a->GetLinks())
        {
            SetLink(lnk.first, lnk.second.GetNode(), lnk.second.GetSubId());
        }

        return MAF_OK;
    }

    //-------------------------------------------------------------------------
    bool Node::CanCopy(Node* node)
        //-------------------------------------------------------------------------
    {
        if (node == nullptr)
        {
            return false;
        }

        if (node->IsA(GetStaticTypeId()))
        {
            return true;
        }

        return false;
    }

    //-------------------------------------------------------------------------
    bool Node::Equals(Node* node)
        //-------------------------------------------------------------------------
    {
        if (!node || !node->IsA(GetTypeName()))
            return false;

        // do not check the ID!!!

        if (m_Name != node->m_Name)
            return false;

        // check attributes
        if (m_Attributes.size() != node->m_Attributes.size())
            return false;

        for (auto att_it = m_Attributes.begin(), att_it2 = node->m_Attributes.begin(); att_it != m_Attributes.end(); att_it++, att_it2++)
        {
            if (!(*att_it)->Equals(att_it2->get()))
                return false;

            if (att_it2 == node->m_Attributes.end())
                return false;
        }

        // check links (poor links checking, only linked VME name)
        if (m_Links.size() != node->GetLinks().size())
            return false;

        for (auto lnk_it = m_Links.begin(), lnk_it2 = node->GetLinks().begin(); lnk_it != m_Links.end(); ++lnk_it, ++lnk_it2)
        {
            if (lnk_it2 == node->GetLinks().end())
                return false;
            if (lnk_it->first != lnk_it2->first)
                return false;
            if (lnk_it->second.GetNode() != lnk_it2->second.GetNode())
                return false;
            if (lnk_it->second.GetSubId() != lnk_it2->second.GetSubId())
                return false;
        }

        return true;
    }
    //-------------------------------------------------------------------------
    bool Node::CompareTree(Node* vme)
        //-------------------------------------------------------------------------
    {
        if (!this->Equals(vme))
            return false;

        if (vme->GetNumberOfChildren() != this->GetNumberOfChildren())
        {
            return false;
        }

        for (unsigned long i = 0; i < this->GetNumberOfChildren(); i++)
        {
            if (!this->GetChild(i)->CompareTree(vme->GetChild(i).get()))
            {
                return false;
            }
        }
        return true;
    }
    //----------------------------------------------------------------------------
    void Node::UpdateLinks(std::vector<std::pair<Node*, Node*> >& nodes)
        //----------------------------------------------------------------------------
    {
        // Copy links
        for (auto& link : GetLinks())
        {
            for (unsigned i = 0; i < nodes.size(); i++)
            {
                if (link.second.GetNode() == nodes[i].first)
                {
                    //n->SetLink(lnk_it->first, mp_it->second, lnk_it->second.m_NodeSubId);
                    link.second.GetNode()->RemoveObserver(this);
                    link.second.SetNode(nodes[i].second);
                    link.second.GetNode()->AddObserver(this);
                }
            }
        }

    }
    //----------------------------------------------------------------------------
    std::shared_ptr<Node> Node::CopyTree()
        //-------------------------------------------------------------------------
    {
        std::vector<std::pair<Node*, Node*> > nodes;
        {
            for (auto& n : *this)
            {
                nodes.push_back(std::make_pair(&n, nullptr));
            }
        }
        auto res = CopyTree(this);
        if (res == nullptr)
            return nullptr;
        {
            unsigned i = 0;
            for (auto& n : *res)
            {
                nodes[i].second = &n;
            }
        }
        {
            for (auto& n : *res)
            {
                n.UpdateLinks(nodes);
            }
        }
        return res;
    }

    //----------------------------------------------------------------------------
    std::shared_ptr<Node> Node::CopyTree(Node* vme, Node* parent)
        //-------------------------------------------------------------------------
    {
        auto v = vme->MakeCopy();
        ReparentTo(v, parent);

        for (unsigned long i = 0; i < vme->GetNumberOfChildren(); i++)
        {
            if (auto child = vme->GetChild(i))
                if (child->IsVisible())
                    Node::CopyTree(child.get(), v.get());
        }
        return v;
    }

    //-------------------------------------------------------------------------
    void Node::SetAttribute(std::shared_ptr<mafAttribute> a)
        //-------------------------------------------------------------------------
    {
        if (auto it = m_Attributes.find(a->GetName()); it != m_Attributes.end())
            m_Attributes.erase(it);
        m_Attributes.insert(a);
    }

    //-------------------------------------------------------------------------
    std::shared_ptr<mafAttribute> Node::GetAttribute(const mafString& name)
        //-------------------------------------------------------------------------
    {
        if (auto it = m_Attributes.find(name); it != m_Attributes.end())
            return *it;
        return nullptr;
    }

    //-------------------------------------------------------------------------
    std::shared_ptr<const mafAttribute> Node::GetAttribute(const mafString& name) const
        //-------------------------------------------------------------------------
    {
        if (auto it = m_Attributes.find(name); it != m_Attributes.end())
            return *it;
        return nullptr;
    }

    //-------------------------------------------------------------------------
    void Node::RemoveAttribute(const mafString& name)
        //-------------------------------------------------------------------------
    {
        m_Attributes.erase(m_Attributes.find(name));
    }

    //-------------------------------------------------------------------------
    void Node::RemoveAllAttributes()
        //-------------------------------------------------------------------------
    {
        m_Attributes.clear();
    }

    //-------------------------------------------------------------------------
    std::shared_ptr<mafTagArray> Node::GetTagArray()
        //-------------------------------------------------------------------------
    {
        auto tarray = mafTagArray::SafeDownCast(GetAttribute(_R("TagArray")));
        if (!tarray)
        {
            tarray = mafTagArray::NewSPtr();
            tarray->SetName(_R("TagArray"));
            SetAttribute(tarray);
        }
        return tarray;
    }

    //-------------------------------------------------------------------------
    Node* Node::GetLink(const mafString& name)
        //-------------------------------------------------------------------------
    {
        if (auto it = m_Links.find(name); it != m_Links.end())
        {
            // if the link is still valid return its pointer
            // Check node validity instead of checking 'm_NodeId'
            // then if m_NodeId is different from m_Id, the link will
            // be updated.
            //if (it->second.m_Node != NULL && it->second.m_Node->IsInitialized() && it->second.m_Node->GetIdManager() == GetIdManager())
            if (it->second.GetNode() && it->second.GetNode()->IsValid() && it->second.GetNode()->GetRoot() == GetRoot())
            {
                return it->second.GetNode();
            }
        }

        return NULL;
    }

    Node::Iterator Node::begin()
    {
        Iterator iter(this);
        iter.GoToFirstNode();
        return iter;
    }

    Node::Iterator Node::end()
    {
        return Iterator(this);
    }



    //-------------------------------------------------------------------------
    mafID Node::GetLinkSubId(const mafString& name)
        //-------------------------------------------------------------------------
    {
        if (auto it = m_Links.find(name); it != m_Links.end())
        {
            return it->second.GetSubId();
        }
        return -1;
    }
    //-------------------------------------------------------------------------
    void Node::SetLink(const mafString& name, Node* node, mafID sub_id)
        //-------------------------------------------------------------------------
    {
        assert(node);

        if (node == NULL)
        {
            mafLogMessage(_M(mafString(_L("Warning!! NULL node can not be set as link."))));
            return;
        }

        if (auto it = m_Links.find(name); it != m_Links.end())
        {
            // if already linked simply return
            if (it->second.GetNode() == node && it->second.GetSubId() == sub_id)
                return;

            // detach old linked node, if present
            if (it->second.GetNode())
                it->second.GetNode()->RemoveObserver(this);
        }

        // set the link to the new node
        m_Links[name] = NodeLink(node, sub_id);

        // attach as observer of the linked node to catch events
        // of de/attachment to the tree and destroy event.
        node->AddObserver(this);
        Modified();
    }
    //-------------------------------------------------------------------------
    void Node::RemoveLink(const mafString& name)
        //-------------------------------------------------------------------------
    {
        if (auto it = m_Links.find(name); it != m_Links.end())
        {
            assert(it->second.GetNode());
            // detach as observer from the linked node
            if (it->second.GetNode())
            {
                it->second.GetNode()->RemoveObserver(this);
            }

            m_Links.erase(it); // remove linked node from links container
            Modified();
        }
    }

    //-------------------------------------------------------------------------
    void Node::RemoveAllLinks()
        //-------------------------------------------------------------------------
    {
        for (auto it = m_Links.begin(); it != m_Links.end(); it++)
        {
            // detach as observer from the linked node
            if (it->second.GetNode())
                it->second.GetNode()->RemoveObserver(this);
        }
        m_Links.clear();
        Modified();
    }
    //-------------------------------------------------------------------------
    MTimeType Node::GetMTime()
        //-------------------------------------------------------------------------
    {
        auto mtime = this->mafTimeStamped::GetMTime();
        if (m_DependsOnLinkedNode)
        {
            for (auto& link : m_Links)
            {
                // check linked node timestamp
                if (link.second.GetNode())
                {
                    mtime = (std::max)(mtime, link.second.GetNode()->GetMTime());
                }
            }
        }
        return mtime;
    }

    //-------------------------------------------------------------------------
    void Node::OnNodeDetachedFromTree(Node* node)
        //-------------------------------------------------------------------------
    {
        for (auto it = m_Links.begin(); it != m_Links.end(); it++)
        {
            if (it->second.GetNode() == node)
            {
            }
        }
    }

    //-------------------------------------------------------------------------
    void Node::OnNodeAttachedToTree(Node* node)
        //-------------------------------------------------------------------------
    {
        for (auto it = m_Links.begin(); it != m_Links.end(); it++)
        {
            if (it->second.GetNode() == node)
            {
            }
        }
    }
    //-------------------------------------------------------------------------
    void Node::OnNodeDestroyed(Node* node)
        //-------------------------------------------------------------------------
    {
        for (auto it = m_Links.begin(); it != m_Links.end(); it++)
        {
            if (it->second.GetNode() == node)
            {
                //it->second.m_Node->GetEventSource()->RemoveObserver(this);
                m_Links.erase(it); // remove linked node from links container
                Modified();
                it = m_Links.begin();
                if (it == m_Links.end())
                    break;
            }
        }
    }
    //-------------------------------------------------------------------------
    void Node::OnEvent(mafEventBase* e)
        //-------------------------------------------------------------------------
    {
        if (e->GetChannel() == MCH_DOWN)
        {
            switch (e->GetId())
            {
            case NODE_DETACHED_FROM_TREE:
                ReleaseNodeId(GetId());
                { mafEventBase evUnq(this, NODE_DETACHED_FROM_TREE); InvokeEvent(&evUnq); }
                break;
            case NODE_ATTACHED_TO_TREE:
            { mafEventBase evUnq(this, NODE_ATTACHED_TO_TREE); InvokeEvent(&evUnq); }
            break;
            }
            ForwardDownEvent(e);
            return;
        }
        // events arriving directly from another node
        if (e->GetChannel() == MCH_NODE)
        {
            switch (e->GetId())
            {
            case NODE_DETACHED_FROM_TREE:
                OnNodeDetachedFromTree((Node*)e->GetSender());
                break;
            case NODE_ATTACHED_TO_TREE:
                OnNodeAttachedToTree((Node*)e->GetSender());
                break;
            case NODE_DESTROYED:
                OnNodeDestroyed((Node*)e->GetSender());
                break;
            }
            return;
        }

        // events to be sent up or down in the tree are simply forwarded
        if (e->GetChannel() == MCH_UP)
        {
            mafEvent* gui_event = mafEvent::SafeDownCast(e);
            if (gui_event && (gui_event->GetSender() == m_Gui))
            {
                switch (gui_event->GetId())
                {
                case ID_NAME:
                    SetName(m_Name);
                    break;
                case ID_PRINT_INFO:
                    OnPrint();
                    break;
                case ID_HELP:
                {

                    mafEvent helpEvent;
                    helpEvent.SetSender(this);
                    mafString vmeTypeName = _R(this->GetTypeName());
                    helpEvent.SetString(&vmeTypeName);
                    helpEvent.SetId(OPEN_HELP_PAGE);
                    ForwardUpEvent(helpEvent);
                }
                break;
                case ID_MOVEUP:
                {
                    if (auto parent = GetParent())
                        parent->MoveChildUp(this);
                }
                break;
                case ID_MOVEDN:
                {
                    if (auto parent = GetParent())
                        parent->MoveChildDown(this);
                }
                break;
                case ID_MOVERIGHT:
                {
                    if (auto parent = GetParent())
                    {
                        if (this != parent->GetChild(0).get())
                        {
                            ReparentTo(parent->GetChild(parent->FindNodeIdx(this)), parent->GetChild(parent->FindNodeIdx(this) - 1).get());
                            { mafEvent evUnq(this, VME_SELECT); evUnq.SetVme(this); ForwardUpEvent(evUnq); }
                        }
                    }
                }
                break;
                case ID_MOVELEFT:
                {
                    if (auto parent = GetParent())
                    {
                        if (auto grandparent = parent->GetParent())
                        {
                            int numChildren = grandparent->GetNumberOfChildren();
                            int parentidx = grandparent->FindNodeIdx(parent);
                            ReparentTo(parent->GetChild(parent->FindNodeIdx(this)), grandparent);
                            { mafEvent evUnq(this, VME_SELECT); evUnq.SetVme(this); ForwardUpEvent(evUnq); }
                            for (int i = 0; i < (numChildren - parentidx - 1); i++)
                                grandparent->MoveChildUp(this);
                        }
                    }
                }
                break;
                }
                return;
            }
            else
            {
                switch (e->GetId())
                {
                case NODE_GET_ROOT:
                {
                    mafEventIO* maf_event = mafEventIO::SafeDownCast(e);
                    maf_event->SetRoot(GetRoot());
                }
                break;
                default:
                    ForwardUpEvent(e);
                }
            }
            return;
        }
    }
    //-------------------------------------------------------------------------
    void Node::InternalStore(mafStorageElementBuilder& parent)
        //-------------------------------------------------------------------------
    {
        parent(_R("Name")).SetValue(m_Name);
        parent(_R("Id")).SetValue(mafToString(m_Id));

        auto attributes = parent[_R("Attributes")];
        attributes(_R("NumberOfItems")).SetValue(m_Attributes.size());
        if (!m_Attributes.empty())
        {
            auto entry = attributes[_R("Item")];
            size_t idx = 0;
            for (auto it = m_Attributes.begin(); it != m_Attributes.end(); ++it)
            {
                entry[idx++].SetValue(it->get());
            }
        }

        // store Links
        unsigned numberOfLinks = 0;
        for (auto links_it = m_Links.begin(); links_it != m_Links.end(); ++links_it)
        {
            auto& link = links_it->second;
            if (links_it->second.GetNode() && links_it->second.GetNode()->IsValid() && links_it->second.GetNode()->GetRoot() == GetRoot())
                numberOfLinks++;
        }

        auto links_element = parent[_R("Links")];
        links_element(_R("NumberOfLinks")).SetValue(mafToString((long)numberOfLinks));
        for (auto links_it = m_Links.begin(); links_it != m_Links.end(); ++links_it)
        {
            auto& link = links_it->second;
            if (links_it->second.GetNode() && links_it->second.GetNode()->IsValid() && links_it->second.GetNode()->GetRoot() == GetRoot())
            {
                auto link_item_element = links_element[_R("Link")];
                link_item_element(_R("Name")).SetValue(links_it->first);
                link_item_element(_R("NodeId")).SetValue(link.GetNode()->GetId());
                link_item_element(_R("NodeSubId")).SetValue(link.GetSubId());
            }
        }

        // store the visible children into a tmp array
        auto children = parent[_R("Children")];
        children(_R("NumberOfItems")).SetValue((uint64_t)GetNumberOfChildren());
        if (GetNumberOfChildren() != 0)
        {
            auto entry = children[_R("Node")];
            for (unsigned int i = 0; i < GetNumberOfChildren(); i++)
            {
                auto node = GetChild(i);
                if (node->IsVisible())
                {
                    entry[i].SetValue(node.get());
                }
            }
        }
    }

    //-------------------------------------------------------------------------
    void Node::InternalRestore(const mafStorageElement& node)
        //-------------------------------------------------------------------------
    {
        m_Name = node(_R("Name")).As<mafString>();
        //   if (node.GetAttribute(_R("Name"), m_Name) != MAF_OK)
        //   {
        //     mafErrorMacro("I/O error restoring node of type "<<GetTypeName()<<" : cannot found Name attribute.");
        //     return MAF_ERROR;
        //   }
          // restore Id
        mafID id = node(_R("Id")).As<mafID>();
        //   if (node.GetAttribute(_R("Id"), id) != MAF_OK)
        //   {
        //     mafErrorMacro("I/O error restoring node "<<GetName().GetCStr() <<" of type "<<GetTypeName()<<" : cannot found Id attribute.");
        //     return MAF_ERROR;
        //   }
        SetId(id);

        // restore attributes
        RemoveAllAttributes();
        auto attr_items = node[_R("Attributes")][_R("Item")];
        mafID numItemsAttr = node[_R("Attributes")](_R("NumberOfItems")).As<mafID>();
        if (numItemsAttr != attr_items.size())
        {
            mafErrorMacro("Number of attributes differs from number of entries");// << GetName());
            return;
        }

        for (size_t i = 0; i < attr_items.size(); i++)
        {
            auto item = attr_items[i].As<mafAttribute>();
            assert(item);
            if (item)
            {
                m_Attributes.insert(item);
            }
        }

        // restore Links
        RemoveAllLinks();
        mafStorageElement links_element = node[_R("Links")];
        int n = links_element(_R("NumberOfLinks")).As<int>();
        //links_element->GetAttribute(_R("NumberOfLinks"), num_links);
        //int n=(int)atof(num_links.GetCStr());
        auto links_vector = links_element[_R("Link")];
        assert(links_vector.size() == n);
        for (size_t i = 0; i < links_vector.size(); i++)
        {
            mafString link_name = links_vector[i](_R("Name")).As<mafString>();
            //links_vector[i].GetAttribute(_R("Name"),link_name);
            mafID link_node_id = links_vector[i](_R("NodeId")).As<mafID>();
            mafID link_node_subid = links_vector[i](_R("NodeSubId")).As<mafID>();
            //links_vector[i].GetAttributeAsInteger(_R("NodeId"),link_node_id);
            //links_vector[i].GetAttributeAsInteger(_R("NodeSubId"),link_node_subid);
            if (!(link_node_id == -1 && link_node_subid == -1))
            {
                m_Links[link_name] = NodeLink(NULL, link_node_subid).SetId(link_node_id);
            }
        }

        // restore children
        RemoveAllChildren();
        auto child_items = node[_R("Children")][_R("Node")];
        mafID numItemsChild = node[_R("Children")](_R("NumberOfItems")).As<mafID>();
        if (numItemsChild != child_items.size())
        {
            mafErrorMacro("Number of children differs from number of entries");// << GetName());
            return;
        }

        for (size_t i = 0; i < child_items.size(); i++)
        {
            auto node = child_items[i].As<Node>();
            assert(node);
            if (node)
            {
                node->m_Parent = this;
                m_Children.push_back(node);
            }
        }
    }

    //-------------------------------------------------------------------------
    void Node::Print(std::ostream& os, const int tabs)
        //-------------------------------------------------------------------------
    {
        mafIndent indent(tabs);
        mafIndent next_indent(indent.GetNextIndent());

        os << indent << "Name: \"" << m_Name.GetCStr() << "\"" << std::endl;
        os << indent << "Initialized: " << m_Initialized << std::endl;
        os << indent << "VisibleToTraverse: " << m_VisibleToTraverse << std::endl;
        os << indent << "Parent: \"" << (m_Parent ? m_Parent->m_Name.GetCStr() : _R("NULL")) << "\"" << std::endl;
        os << indent << "Number of Children: " << GetNumberOfChildren() << std::endl;
        os << indent << "Id: " << GetId() << std::endl;
        os << indent << "Attributes:\n";
        for (auto& attr : m_Attributes)
        {
            attr->Print(os, next_indent);
        }

        os << indent << "Links:" << std::endl;
        os << indent << "Number of links:" << m_Links.size() << std::endl;
        for (auto& link : m_Links)
        {
            os << next_indent << "Name: " << link.first.GetCStr() << "\tNodeId: " << (link.second.GetNode() ? link.second.GetNode()->GetId() : -1);
            os << "\tNodeSubId: " << link.second.GetSubId() << std::endl;
        }
    }

    bool Node::IsValid() const
    {
	    return m_Id >= 0;
    }

	void Node::SetVisibleToTraverse(bool flag)
    {
	    m_VisibleToTraverse = flag;
    }

    bool Node::GetVisibleToTraverse() const
    {
	    return m_VisibleToTraverse;
    }

    bool Node::IsVisible() const
    {
	    return m_VisibleToTraverse;
    }


    //-------------------------------------------------------------------------
    const char** Node::GetIcon()
        //-------------------------------------------------------------------------
    {
#include "Node.xpm"
        return Node_xpm;
    }
    //-------------------------------------------------------------------------
    /*mafGUI *Node::GetGui()
    //-------------------------------------------------------------------------
    {
      if (m_Gui == NULL)
        CreateGui();
      assert(m_Gui);
      return m_Gui;
    }
    //-------------------------------------------------------------------------
    void Node::DeleteGui()
    //-------------------------------------------------------------------------
    {
      cppDEL(m_Gui);
    }*/
    //-------------------------------------------------------------------------
    mafGUI* Node::CreateGui()
        //-------------------------------------------------------------------------
    {
        assert(m_Gui == NULL);
        m_Gui = new mafGUI(this);

        mafString type_name = _R(GetTypeName());
        if ((*GetMAFExpertMode()))
            m_Gui->Button(ID_PRINT_INFO, type_name, _R(""), _R("Print node debug information"));

        m_Gui->String(ID_NAME, _R("name :"), &m_Name);

        mafEvent buildHelpGui;
        buildHelpGui.SetSender(this);
        buildHelpGui.SetId(GET_BUILD_HELP_GUI);
        ForwardUpEvent(buildHelpGui);

        if (buildHelpGui.GetArg())
        {
            m_Gui->Button(ID_HELP, _R("Help"), _R(""));
        }

        m_Gui->Divider();

        m_Gui->Button(ID_MOVEUP, _L("UP"), _L("Up"), _L("Move up in parent list"));
        m_Gui->TwoButtons(ID_MOVELEFT, ID_MOVERIGHT, _L("Move left in tree"), _L("Move right in tree"));
        m_Gui->Button(ID_MOVEDN, _L("DN"), _L("Down"), _L("Move down in parent list"));
        m_Gui->Divider();

        UpdateUpDownAvailability(this);
        return m_Gui;
    }
    //-------------------------------------------------------------------------
    void Node::UpdateId()
        //-------------------------------------------------------------------------
    {
        SetId(GetNewNodeId());
    }

    //-------------------------------------------------------------------------
    void Node::OnPrint()
        //-------------------------------------------------------------------------
    {
#ifdef VTK_USE_ANSI_STDLIB
        std::stringstream ss1;

        Print(ss1);
        wxString message = ss1.str().c_str();

        mafLogMessage("[VME PRINTOUT:]\n");

        for (int pos = message.Find('\n'); pos >= 0; pos = message.Find('\n'))
        {
            wxString tmp = message.Mid(0, pos);
            mafLogMessage(tmp.c_str());
            message = message.Mid(pos + 1);
        }
#else
        std::stringstream stringStream;
        stringStream << "[VME PRINTOUT:]\n";
        Print(stringStream);
        stringStream << std::endl;   // Paolo 13/06/2005: needed to close correctly the strstream
        //mafLogMessage("[VME PRINTOUT:]\n%s\n", ss1.str()); 
        mafLogMessage(_M(stringStream.str().c_str()));
#endif
    }


    //-------------------------------------------------------------------------
    mafID Node::GetNewNodeId()
        //-------------------------------------------------------------------------
    {
        return (m_Parent) ? m_Parent->GetNewNodeId() : -1;
    }

    //-------------------------------------------------------------------------
    void Node::ReleaseNodeId(mafID id)
        //-------------------------------------------------------------------------
    {
        if (id == -1)
            return;
        if (m_Parent)
            m_Parent->ReleaseNodeId(id);
    }


    //-------------------------------------------------------------------------
    void Node::MoveChildUp(Node* child)
        //-------------------------------------------------------------------------
    {
        if (child == NULL)
            return;
        int idx = FindNodeIdx(child);
        if (idx < 0)
            return;
        MoveChildUp(idx);
    }
    //-------------------------------------------------------------------------
    void Node::MoveChildDown(Node* child)
        //-------------------------------------------------------------------------
    {
        if (child == NULL)
            return;
        int idx = FindNodeIdx(child);
        if (idx < 0)
            return;
        MoveChildDown(idx);
    }

    //-------------------------------------------------------------------------
    void Node::MoveChildUp(int idx)
        //-------------------------------------------------------------------------
    {
        if (idx <= 0 || idx >= m_Children.size())
            return;
        SwapChildren(idx - 1, idx);
    }
    //-------------------------------------------------------------------------
    void Node::MoveChildDown(int idx)
        //-------------------------------------------------------------------------
    {
        if (idx < 0 || m_Children.size() <= 1)
            return;
        SwapChildren(idx, idx + 1);
    }

    //-------------------------------------------------------------------------
    void Node::SwapChildren(int idx1, int idx2)
        //-------------------------------------------------------------------------
    {
        if (idx1 == idx2)
            return;
        if (idx1 < 0 || idx1 >= m_Children.size())
            return;
        if (idx2 < 0 || idx2 >= m_Children.size())
            return;
        //if(m_Children.size() <= 1)
        //  return;

        auto tmp = m_Children[idx1];
        m_Children[idx1] = m_Children[idx2];
        m_Children[idx2] = tmp;
        UpdateUpDownAvailability(m_Children[idx1].get());
        UpdateUpDownAvailability(m_Children[idx2].get());
        Modified();
        mafEvent e(this, VME_MODIFIED); e.SetVme(this);
        e.SetChannel(MCH_UP);
        OnEvent(&e);
    }
    //-------------------------------------------------------------------------
    std::shared_ptr<Node> Node::GetByPath(const mafString& path, bool onlyVisible /*=true*/)
        //-------------------------------------------------------------------------
    {
        wxStringTokenizer tkz(path.toWx(), wxT("/"));

        Node* currentNode = this;
        Node* tmpParent = nullptr;
        wxString tmpString;
        long tmpIndex;

        while (tkz.HasMoreTokens() && currentNode != nullptr)
        {
            wxString token = tkz.GetNextToken();

            if (token == "next")
            {
                tmpParent = currentNode->GetParent();
                //Root case: root does not ave next
                if (tmpParent == nullptr)
                {
                    mafLogMessage(_M("Node path error: root does not have next"));
                    currentNode = nullptr;
                    break;
                }
                //getting node index
                tmpIndex = tmpParent->FindNodeIdx(currentNode, onlyVisible);
                //Size check
                if (tmpIndex == tmpParent->GetNumberOfChildren(onlyVisible) - 1)
                {
                    mafLogMessage(_M("Node path error: asked 'next' on last node"));
                    currentNode = nullptr;
                    break;
                }
                //updating current node
                currentNode = tmpParent->GetChild(tmpIndex + 1, onlyVisible).get();
            }

            else if (token == "prev")
            {
                tmpParent = currentNode->GetParent();
                //Root case: root does not ave next
                if (tmpParent == nullptr)
                {
                    mafLogMessage(_M("Node path error: root does not have next"));
                    currentNode = nullptr;
                    break;
                }
                //getting node index
                tmpIndex = tmpParent->FindNodeIdx(currentNode, onlyVisible);
                //Size check
                if (tmpIndex == 0)
                {
                    mafLogMessage(_M("Node path error: asked 'prec' on first node"));
                    currentNode = nullptr;
                    break;
                }
                //updating current node
                currentNode = tmpParent->GetChild(tmpIndex - 1, onlyVisible).get();
            }

            else if (token == "firstPair")
            {
                tmpParent = currentNode->GetParent();
                //Root case: root does not ave next
                if (tmpParent == nullptr)
                {
                    mafLogMessage(_M("Node path error: root does not have next"));
                    currentNode = nullptr;
                    break;
                }

                //updating current node
                currentNode = tmpParent->GetChild(0, onlyVisible).get();
            }

            else if (token == "lastPair")
            {
                tmpParent = currentNode->GetParent();
                //Root case: root does not ave next
                if (tmpParent == nullptr)
                {
                    mafLogMessage(_M("Node path error: root does not have next"));
                    currentNode = nullptr;
                    break;
                }
                //updating current node
                currentNode = tmpParent->GetChild(currentNode->GetNumberOfChildren(onlyVisible) - 1, onlyVisible).get();
            }

            else if (token == "firstChild")
            {
                //Root case: root does not ave next
                if (currentNode->GetNumberOfChildren(onlyVisible) == 0)
                {
                    mafLogMessage(_M("Node path error: asked 'firstChild' on no child node"));
                    currentNode = nullptr;
                    break;
                }
                //updating current node
                currentNode = currentNode->GetChild(0, onlyVisible).get();
            }

            else if (token == "lastChild")
            {
                //Root case: root does not ave next
                auto num = currentNode->GetNumberOfChildren(onlyVisible);
                if (num == 0)
                {
                    mafLogMessage(_M("Node path error: asked 'lastChild' on no child node"));
                    currentNode = nullptr;
                    break;
                }
                //updating current node
                currentNode = currentNode->GetChild(num - 1, onlyVisible).get();
            }

            else if (token.StartsWith("pair["))
            {
                //checking match bracket 
                if (token[token.size() - 1] != ']')
                {
                    mafLogMessage(_M("Node path error: pair[] wrong format"));
                    currentNode = nullptr;
                    break;
                }

                //getting the number substring
                tmpString = token.SubString(5, token.size() - 2);

                tmpParent = currentNode->GetParent();
                //Root case: root does not ave next
                if (tmpParent == nullptr)
                {
                    mafLogMessage(_M("Node path error: root does not have pairs"));
                    currentNode = nullptr;
                    break;
                }

                //Number checking
                if (!tmpString.IsNumber())
                {
                    mafLogMessage(_M("Node path error: wrong pair[] argument"));
                    currentNode = nullptr;
                    break;
                }

                tmpString.ToLong(&tmpIndex);

                //Checking bounds
                if (tmpIndex < 0 || tmpIndex > tmpParent->GetNumberOfChildren(onlyVisible) - 1)
                {
                    mafLogMessage(_M("Node path error: pair[] value outside bounds"));
                    currentNode = nullptr;
                    break;
                }

                currentNode = tmpParent->GetChild(tmpIndex, onlyVisible).get();
            }

            else if (token.StartsWith("pair{"))
            {
                //checking match bracket 
                if (token[token.size() - 1] != '}')
                {
                    mafLogMessage(_M("Node path error: pair{} wrong format"));
                    currentNode = nullptr;
                    break;
                }

                //getting the number substring
                tmpString = token.SubString(5, token.size() - 2);

                tmpParent = currentNode->GetParent();
                //Root case: root does not ave next
                if (tmpParent == nullptr)
                {
                    mafLogMessage(_M("Node path error: root does not have pairs"));
                    currentNode = nullptr;
                    break;
                }

                //getting node index
                tmpIndex = tmpParent->FindNodeIdx(mafWxToString(tmpString), onlyVisible);
                if (tmpIndex == -1)
                {
                    mafLogMessage(_M(_R("Node path error: pair{") + mafWxToString(tmpString) + _R("}, not found")));
                    currentNode = nullptr;
                    break;
                }

                currentNode = tmpParent->GetChild(tmpIndex, onlyVisible).get();
            }

            else if (token.StartsWith("child["))
            {
                //checking match bracket 
                if (token[token.size() - 1] != ']')
                {
                    mafLogMessage(_M("Node path error: child[] wrong format"));
                    currentNode = nullptr;
                    break;
                }

                //getting the number substring
                tmpString = token.SubString(6, token.size() - 2);

                //Number checking
                if (!tmpString.IsNumber())
                {
                    mafLogMessage(_M("Node path error: wrong child[] argument"));
                    currentNode = nullptr;
                    break;
                }

                tmpString.ToLong(&tmpIndex);

                //Checking bounds
                if (tmpIndex < 0 || tmpIndex > currentNode->GetNumberOfChildren(onlyVisible) - 1)
                {
                    mafLogMessage(_M("Node path error: child[] value outside bounds"));
                    currentNode = nullptr;
                    break;
                }

                currentNode = currentNode->GetChild(tmpIndex, onlyVisible).get();

            }

            else if (token.StartsWith("child{"))
            {
                //checking match bracket 
                if (token[token.size() - 1] != '}')
                {
                    mafLogMessage(_M("Node path error: child{} wrong format"));
                    currentNode = nullptr;
                    break;
                }

                //getting the number substring
                tmpString = token.SubString(6, token.size() - 2);

                //getting node index
                tmpIndex = currentNode->FindNodeIdx(mafWxToString(tmpString), onlyVisible);
                if (tmpIndex == -1)
                {
                    mafLogMessage(_M(_R("Node path error: pair{") + mafWxToString(tmpString) + _R("}, not found")));
                    currentNode = nullptr;
                    break;
                }

                currentNode = currentNode->GetChild(tmpIndex, onlyVisible).get();
            }

            else if (token == "root")
            {
                currentNode = currentNode->GetRoot();
            }

            else if (token == "..")
            {
                currentNode = currentNode->GetParent();
                if (currentNode == nullptr)
                {
                    mafLogMessage(_M("Node path error: root does not have parent"));
                    break;
                }
            }

            else if (token == ".")
            {
                //used to point to current node no update required
            }

            else
            {
                currentNode = nullptr;
                mafLogMessage(_M(_R("Node path error: unknown token:") + mafWxToString(token)));
                break;
            }
            tmpString = currentNode->GetName().toWx();

        }
        //While end
        if (currentNode == nullptr)
            return nullptr;
        return currentNode->SharedFromThis();
    }
    std::shared_ptr<Node> Node::SharedFromThis()
    {
        return shared_from_this();
    }

    Node* Node::GetParent() const
    {
        return m_Parent;
    }


    std::shared_ptr<Node>  Node::Create(const char* NodeType)
    {
        return NodeFactory::CreateNode(NodeType);
    }
}

END_FTK_NAMESPACE
