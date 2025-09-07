#include "ftk/Core/NodeManager.h"
#include "mafDecl.h"
#include "mafNode.h"
#include "ftk/Core/Root.h"
#include "ftk/IO/StorageElement.h"
#include "mafEvent.h"

namespace model::data
{
	NodeManager::NodeManager() = default;

	NodeManager::~NodeManager()
	{
		SetRoot(nullptr);
		SetListener(nullptr);
	}

	void NodeManager::OnEvent(mafEventBase* maf_event)
	{
		if (maf_event->GetChannel() != MCH_UP)
			return;
		// events coming from the tree...
		if (maf_event->GetId() == NODE_ATTACHED_TO_TREE)
		{
			NotifyAdd((mafNode*)maf_event->GetSender());
			Modified(true);
			return;
		}
		if (maf_event->GetId() == NODE_DETACHED_FROM_TREE)
		{
			NotifyRemove((mafNode*)maf_event->GetSender());
			Modified(true);
			return;
		}
		InvokeEvent(*maf_event);
	}

	std::shared_ptr<mafNode> NodeManager::GetRoot()
	{
		return m_Root;
	}

	bool NodeManager::SetRoot(std::shared_ptr<mafNode> root)
	{
		NotifyRemove(m_Root.get());
		if (auto rt = Root::SafeDownCast(GetRoot().get()))
			rt->SetListener(nullptr);
		m_Root = std::move(root);
		if (auto rt = Root::SafeDownCast(GetRoot().get()))
			rt->SetListener(this);
		NotifyAdd(m_Root.get());
		return true;
	}

	void NodeManager::VmeRemove(mafNode* n)
	{
		if (n == nullptr)
			return;
		if (!m_Root || !m_Root->IsInTree(n))
		{
			assert(false);
			return;
		}
		if (m_Root.get() != n)
		{
			auto parent = n->GetParent();
			mafNode::ReparentTo(parent->GetChild(parent->FindNodeIdx(n)), nullptr);
		}
		m_Modified = true;
	}

	void NodeManager::NotifyRemove(mafNode* n)
	{
		auto iter = n->begin();
		iter.IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
		iter.SetTraversalModeToPostOrder(); // traverse is: first the subtree left to right, then the root
		for (; iter != n->end(); ++iter)
		{
			mafEvent evUnq(this, VME_REMOVING); evUnq.SetVme(iter.GetCurrentNode()); InvokeEvent(evUnq);
		} // raise notification event (to logic)
	}

	void NodeManager::NotifyAdd(mafNode* n)
	{
		auto iter = n->begin();
		iter.IgnoreVisibleToTraverse(true); // ignore visible to traverse flag and visits all nodes
		for (; iter != n->end(); ++iter)
		{
			{ mafEvent evUnq(this, VME_ADDED); evUnq.SetVme(iter.GetCurrentNode()); InvokeEvent(evUnq); } // raise notification event (to logic)
		}
		m_Modified = true;
	}

	void NodeManager::InternalStore(mafStorageElementBuilder& node)
	{
		if (!m_Root)
			return;
		m_Root->BuildIds();
		node[_R("Root")].SetValue(m_Root.get());// != MAF_OK ? MAF_ERROR : MAF_OK;
	}

	void NodeManager::InternalRestore(const mafStorageElement& node)
	{
		SetRoot(nullptr);
		if (auto root = node[_R("Root")].As<mafNode>())
		{
			root->RestoreLinks();
			if (root->Initialize() == MAF_ERROR)
				return;
			SetRoot(root);
		}
	}
}