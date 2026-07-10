#pragma once

#include "TreeAdapter.h"

#include <wx/wupdlock.h>

#include "ftk/Gui/ITreeViewNode.h"

namespace
{
	struct TreeItemData : wxTreeItemData
	{
		TreeItemData(gui::ITreeViewModel::NodeId n)
		: node(n)
		{
		}
		gui::ITreeViewModel::NodeId node;
	};
}

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	TreeAdapter::TreeAdapter(wxTreeCtrl* tree)
		: m_tree(tree)
	{
		m_tree->Bind(wxEVT_TREE_BEGIN_DRAG,
			&TreeAdapter::onBeginDrag, this);

		m_tree->Bind(wxEVT_TREE_END_DRAG,
			&TreeAdapter::onEndDrag, this);

		m_tree->Bind(wxEVT_TREE_END_LABEL_EDIT,
			&TreeAdapter::onEndLabelEdit, this);

		m_tree->Bind(wxEVT_TREE_ITEM_ACTIVATED,
			&TreeAdapter::onTreeItemActivated, this);

		m_tree->Bind(wxEVT_TREE_SEL_CHANGED,
			&TreeAdapter::OnSelectionChanged, this);
	}

	void TreeAdapter::setModel(ITreeViewModel* model)
	{
		m_connections.clear();
		m_model = model;
		m_tree->DeleteAllItems();
		m_nodeToItem.clear();
		if (m_model)
		{
			m_connections.push_back(m_model->connectNodeAdded([this](ITreeViewModel::NodeId n) {onNodeAdded(n); }));
			m_connections.push_back(m_model->connectNodeRemoved([this](ITreeViewModel::NodeId n) {onNodeRemoved(n); }));
			m_connections.push_back(m_model->connectNodeMoved([this](ITreeViewModel::NodeId n) {onNodeMoved(n); }));
			m_connections.push_back(m_model->connectNodeChanged([this](ITreeViewModel::NodeId n) {onNodeChanged(n); }));
			rebuild();
		}
	}

	void TreeAdapter::onNodeAdded(ITreeViewModel::NodeId n)
	{
		auto parentId = m_nodeToItem[n->parent()];
		if (!parentId.IsOk())
		{
			return;
		}

		auto id = createItem(parentId, n);

		m_tree->Expand(parentId);
	}

	void TreeAdapter::onNodeRemoved(ITreeViewModel::NodeId n)
	{
		if (auto it = m_nodeToItem.find(n); it != m_nodeToItem.end())
		{
			m_tree->Delete(it->second);
			m_nodeToItem.erase(it);
		}
	}

	//void TreeAdapter::onNodeMoved(const NodeMoved& e)
	//{
		//rebuild();
	//}

	void TreeAdapter::onNodeMoved(ITreeViewModel::NodeId n)
	{
		auto it = m_nodeToItem.find(n);
		if (it == m_nodeToItem.end())
			return;

		auto oldId = it->second;

		// Save subtree structure (node still exists)

		// Remove old UI node
		m_tree->Delete(oldId);
		m_nodeToItem.erase(n);

		// Insert under new parent
		auto newParentId = m_nodeToItem[n->parent()];
		auto newId = buildRecursive(newParentId, n);
		m_tree->Expand(newParentId);
	}

	void TreeAdapter::onNodeChanged(ITreeViewModel::NodeId n)
	{
		if (auto it = m_nodeToItem.find(n); it != m_nodeToItem.end())
		{
			m_tree->SetItemText(it->second, m_model->getNodeName(n));
			int status = m_model->getNodeStatus(n);
			m_tree->SetItemImage(it->second, status, wxTreeItemIcon_Normal);
			m_tree->SetItemImage(it->second, status, wxTreeItemIcon_Selected);
		}
	}

	void TreeAdapter::rebuild()
	{
		wxWindowUpdateLocker freezer(m_tree);
		m_nodeToItem.clear();
		m_tree->DeleteAllItems();
		buildRecursive(wxTreeItemId(), m_model->root());
		m_tree->ExpandAll();
	}

	wxTreeItemId TreeAdapter::item(ITreeViewModel::NodeId node) const
	{
		if (auto it = m_nodeToItem.find(node); it != m_nodeToItem.end())
		{
			return it->second;
		}
		return wxTreeItemId();
	}

	wxTreeItemId TreeAdapter::createItem(wxTreeItemId parent, ITreeViewModel::NodeId node)
	{
		wxTreeItemId id;

		if (!parent.IsOk())
		{
			id = m_tree->AddRoot(m_model->getNodeName(node));
		}
		else
		{
			id = m_tree->AppendItem(parent, m_model->getNodeName(node));
		}
		m_nodeToItem[node] = id;
		m_tree->SetItemData(id, new TreeItemData{ node });
		m_tree->SetItemImage(id, m_model->getNodeStatus(node), wxTreeItemIcon_Normal);
		m_tree->SetItemImage(id, m_model->getNodeStatus(node), wxTreeItemIcon_Selected);
		return id;
	}

	wxTreeItemId TreeAdapter::buildRecursive(wxTreeItemId parent, ITreeViewModel::NodeId node)
	{
		wxTreeItemId id = createItem(parent, node);
		for (auto& child : node->children())
		{
			buildRecursive(id, child);
		}
		return id;
	}

	ITreeViewModel::NodeId TreeAdapter::getNode(wxTreeItemId id) const
	{
		auto data = static_cast<TreeItemData*>(m_tree->GetItemData(id));
		return data ? data->node : nullptr;
	}

	// event handlers
	void TreeAdapter::onBeginDrag(wxTreeEvent& e)
	{
		m_dragged = e.GetItem();
		e.Allow();
	}

	void TreeAdapter::onEndDrag(wxTreeEvent& e)
	{
		auto target = e.GetItem();

		auto src = getNode(m_dragged);
		auto dst = getNode(target);

		if (!src || !dst)
		{
			return;
		}
		m_model->reparentNode(src, dst);
		rebuild();
	}

	void TreeAdapter::onEndLabelEdit(wxTreeEvent& e)
	{
		auto node = getNode(e.GetItem());
		if (!node)
		{
			return;
		}
		m_model->renameNode(node, e.GetLabel().ToStdString());
	}

	void TreeAdapter::onTreeItemActivated(wxTreeEvent& e)
	{
		auto node = getNode(e.GetItem());
		if (!node)
		{
			return;
		}
		m_model->activateNode(node);
	}

	void TreeAdapter::OnSelectionChanged(wxTreeEvent& e)
	{
		wxArrayTreeItemIds selections;
		size_t count = m_tree->GetSelections(selections);

		std::vector<ITreeViewModel::NodeId> selectedNodes;
		selectedNodes.reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			selectedNodes.push_back(getNode(selections[i]));
		}
		m_model->select(selectedNodes);
	}
}

END_FTK_NAMESPACE
