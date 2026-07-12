#pragma once

#include "TreeAdapter.h"

#include "ftk/Gui/ITreeViewNode.h"

#include <wx/wupdlock.h>

#include <deque>

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
		addItem(n);
		auto parentId = m_nodeToItem[n->parent()];
		if (parentId.IsOk())
		{
			m_tree->Expand(parentId);
		}
	}

	void TreeAdapter::onNodeRemoved(ITreeViewModel::NodeId n)
	{
		if (auto it = m_nodeToItem.find(n); it != m_nodeToItem.end())
		{
			m_tree->Delete(it->second);
			m_nodeToItem.erase(it);
		}
	}

	void TreeAdapter::onNodeMoved(ITreeViewModel::NodeId n)
	{
		auto oldId = getItem(n);
		if (!oldId.IsOk())
		{
			return;
		}

		m_tree->Delete(oldId);
		for (auto queue = std::deque<ITreeViewModel::NodeId>(1, n); !queue.empty(); )
		{
			auto node = queue.front();
			queue.pop_front();

			m_nodeToItem.erase(node);

			for (auto& child : node->children())
			{
				queue.push_back(child);
			}
		}

		addTree(n);
		auto newParentId = getItem(n->parent());
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

	ITreeViewModel::NodeId TreeAdapter::getNode(wxTreeItemId id) const
	{
		auto data = static_cast<TreeItemData*>(m_tree->GetItemData(id));
		return data ? data->node : nullptr;
	}

	wxTreeItemId TreeAdapter::getItem(ITreeViewModel::NodeId node) const
	{
		if (auto it = m_nodeToItem.find(node); it != m_nodeToItem.end())
		{
			return it->second;
		}
		return {};
	}

	void TreeAdapter::rebuild()
	{
		wxWindowUpdateLocker freezer(m_tree);
		m_nodeToItem.clear();
		m_tree->DeleteAllItems();
		addTree(m_model->root());
		m_tree->ExpandAll();
	}

	void TreeAdapter::addItem(ITreeViewModel::NodeId node)
	{
		wxTreeItemId id;
		auto parent = getItem(node->parent());

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
	}

	void TreeAdapter::addTree(ITreeViewModel::NodeId node)
	{
		if (!node)
		{
			return;
		}

		for (auto queue = std::deque<ITreeViewModel::NodeId>(1, node); !queue.empty(); )
		{
			auto nextNode = queue.front();
			queue.pop_front();

			addItem(nextNode);

			for (auto& child : nextNode->children())
			{
				queue.push_back(child);
			}
		}
	}

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
