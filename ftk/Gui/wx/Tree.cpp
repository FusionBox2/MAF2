#include "ftk/Gui/wx/Tree.h"
#include "mafDecl.h"
#include "mafPics.h"
#include "mafNode.h"
#include "mafEvent.h"

#include <wx/wupdlock.h>

#include <list>

namespace gui::wx
{
	Tree::Tree(wxWindow* parent, wxWindowID id, bool CloseButton, bool HideTitle)
		:mafGUINamedPanel(parent, id, CloseButton, HideTitle)
	{
		m_NodeTree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(100, 100)), wxNO_BORDER | wxTR_HAS_BUTTONS);
		m_NodeTree->Bind(wxEVT_TREE_SEL_CHANGED, [this](wxTreeEvent& event) {OnSelectionChanged(event); });
		Add(m_NodeTree, 1, wxEXPAND);
		Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {OnSize(event); });

		wxBitmap bmp = mafPictureFactory::GetPictureFactory()->GetBmp(_R("NODE_GRAY"));
		int w = bmp.GetWidth();
		int h = bmp.GetHeight();

		m_NodeImages = std::make_unique<wxImageList>(w, h, false, 4);
		m_NodeImages->Add(bmp);
		m_NodeImages->Add(mafPictureFactory::GetPictureFactory()->GetBmp(_R("NODE_RED")));
		m_NodeImages->Add(mafPictureFactory::GetPictureFactory()->GetBmp(_R("NODE_BLUE")));
		m_NodeImages->Add(mafPictureFactory::GetPictureFactory()->GetBmp(_R("NODE_YELLOW")));
		m_NodeTree->SetImageList(m_NodeImages.get());
	}

	Tree::~Tree() = default;

	void Tree::Reset()
	{
		m_NodeTree->DeleteAllItems();
		m_NodeTable.clear();
	}

	bool Tree::AddNode(NodeID node_id, NodeID parent_id, const mafString& label, int icon, int selectedIcon, TreeItemData* data)
	{
		/*
		- se parent_id = 0 to create the root
		- icon must be a valid index in the m_NodeImages - otherwise it is clamped
		- fails if node_id already exist
		- parent_id must exist
		- return true on success
		*/

		icon = CheckIconId(icon);
		if (selectedIcon != -1)
		{
			selectedIcon = CheckIconId(selectedIcon);
		}

		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			return false;
		}

		if (data == nullptr)
		{
			data = new TreeItemData(node_id);
		}

		if (m_NodeTree->IsEmpty())
		{
			if (auto item = m_NodeTree->AddRoot(mafStringToWx(label), icon, selectedIcon, data); item.IsOk())
			{
				m_NodeTable[node_id] = item;
				return true;
			}
			return false;
		}

		if (auto parent_item = ItemFromNode(parent_id); parent_item.IsOk())
		{
			if (auto item = m_NodeTree->AppendItem(parent_item, mafStringToWx(label), icon, icon, data); item.IsOk())
			{
				m_NodeTable[node_id] = item;
				m_NodeTree->Expand(parent_item);
				return true;
			}
		}
		return false;
	}

	bool Tree::DeleteNode(NodeID node_id)
	{
		/*
		- node_id must exist
		- return true on success
		- check that node_id exist
		- calls DeleteNode2 to delete all the item in the subtree keeping m_NodeTable consistent
		- keep parent->HasChildren consistent
		- if the item was selected, select the parent node (). [ to prevent the shown property-gui to become inconsistent ]
		*/

		auto item = ItemFromNode(node_id);
		if (!item.IsOk())
		{
			return false;
		}

		auto parent_item = m_NodeTree->GetItemParent(item);
		if (parent_item.IsOk() && m_NodeTree->IsSelected(item))
		{
			m_NodeTree->SelectItem(parent_item);
		}

		while (m_NodeTree->HasChildren(item))
		{
			auto leaf = item;
			while (m_NodeTree->HasChildren(leaf))
				leaf = m_NodeTree->GetLastChild(leaf);
			auto id = NodeFromItem(item);
			m_NodeTree->Delete(leaf);
			m_NodeTable.erase(id);
		}
		m_NodeTree->Delete(item);
		m_NodeTable.erase(node_id);
		return true;
	}

	mafString Tree::GetNodeLabel(NodeID node_id) const
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			return mafWxToString(m_NodeTree->GetItemText(item));
		}
		return _R("");
	}

	bool Tree::SetNodeLabel(NodeID node_id, const mafString& label)
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			m_NodeTree->SetItemText(item, mafStringToWx(label));
			return true;
		}
		return false;
	}

	int Tree::GetNodeIcon(NodeID node_id) const
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			return m_NodeTree->GetItemImage(item);
		}
		return 0;
	}

	bool Tree::SetNodeIcon(NodeID node_id, int icon)
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			icon = CheckIconId(icon);
			m_NodeTree->SetItemImage(item, icon);
			m_NodeTree->SetItemImage(item, icon, wxTreeItemIcon_Selected);
			return true;
		}
		return false;
	}

	Tree::NodeID Tree::GetSelected() const
	{
		if (auto item = m_NodeTree->GetSelection(); item.IsOk())
		{
			return NodeFromItem(item);
		}
		return 0;
	}

	bool Tree::SelectNode(NodeID node_id)
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			m_PreventNotify = true;
			m_NodeTree->SelectItem(item);
			m_PreventNotify = false;
			return true;
		}
		return false;
	}

	void Tree::CollapseNode(NodeID node_id)
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			m_NodeTree->Collapse(item);
		}
	}

	void Tree::ExpandNode(NodeID node_id)
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			m_NodeTree->Expand(item);
		}
	}

	namespace
	{
		void CollapseAllChildren(wxTreeCtrl* tree, wxTreeItemId& item)
		{
			wxTreeItemIdValue cookie;
			for (wxTreeItemId idCurr = tree->GetFirstChild(item, cookie); idCurr.IsOk(); idCurr = tree->GetNextChild(item, cookie))
			{
				CollapseAllChildren(tree, idCurr);
			}
			if (item != tree->GetRootItem() || !tree->HasFlag(wxTR_HIDE_ROOT))
				tree->Collapse(item);
		}
		void ExpandAllChildren(wxTreeCtrl* tree, wxTreeItemId& item)
		{
			if (item != tree->GetRootItem() || !tree->HasFlag(wxTR_HIDE_ROOT))
				tree->Expand(item);
			wxTreeItemIdValue cookie;
			for (wxTreeItemId idCurr = tree->GetFirstChild(item, cookie); idCurr.IsOk(); idCurr = tree->GetNextChild(item, cookie))
			{
				ExpandAllChildren(tree, idCurr);
			}
		}
	}

	void Tree::CollapseNodeSubTree(NodeID node_id)
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			wxWindowUpdateLocker noUpdates(m_NodeTree);
			CollapseAllChildren(m_NodeTree, item);
		}
	}

	void Tree::ExpandNodeSubTree(NodeID node_id)
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			wxWindowUpdateLocker noUpdates(m_NodeTree);
			ExpandAllChildren(m_NodeTree, item);
		}
	}

	void Tree::ExpandNodeVisible(NodeID node_id)
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			m_NodeTree->EnsureVisible(item);
		}
	}

	bool Tree::NodeHasChildren(NodeID node_id) const
	{
		if (auto item = ItemFromNode(node_id); item.IsOk())
		{
			return m_NodeTree->ItemHasChildren(item);
		}
		return false;
	}

	void Tree::SetImageList(std::unique_ptr<wxImageList> img)
	{
		if (!m_NodeTree->IsEmpty())
		{
			mafLogMessage(_M("warning: Tree::SetImageList must be called before adding any node"));
			// if you replace the image-list with a shorter one 
			// the icon-index actually in use by the existing nodes 
			// can become inconsistent 

			//return; //SIL. 7-4-2005: - commented 4 testing -- to be reinserted
		}

		if (img == m_NodeImages)
			return;
		m_NodeTree->SetImageList(img.get());
		m_NodeImages = std::move(img);
	}

	wxTreeItemId Tree::ItemFromNode(NodeID node_id) const
	{
		if (auto it = m_NodeTable.find(node_id); it != end(m_NodeTable))
		{
			return it->second;
		}
		return {};
	}

	Tree::NodeID Tree::NodeFromItem(const wxTreeItemId& item) const
	{
		if (auto nd = static_cast<TreeItemData*>(m_NodeTree->GetItemData(item)))
			return nd->GetNode();
		return 0;
	}

	void Tree::OnSelectionChanged(wxTreeEvent& event)
	{
		if (m_PreventNotify)
		{
			return;
		}

		if (wxTreeItemId item = event.GetItem();item.IsOk())
		{
			mafEvent evUnq(this, VME_SELECT); evUnq.SetArg(NodeFromItem(item)); InvokeEvent(evUnq);
		}
		event.Skip();
	}

	void Tree::OnSize(wxSizeEvent& event)
	{
		event.Skip();
		m_NodeTree->Refresh();
	}

	/*bool Tree::NodeExist(NodeID node_id) const
	{
		return m_NodeTable.find(node_id) != end(m_NodeTable);
	}*/

	int Tree::CheckIconId(int icon) const
	{
		if (!m_NodeImages) return 0;
		if (icon < 0)
		{
			mafLogMessage(_M(_R("Tree: icon id = ") + mafToString(icon) + _R(" out of range ")));
			return 0;
		}
		if (icon >= m_NodeImages->GetImageCount())
		{
			mafLogMessage(_M(_R("Tree: icon id = ") + mafToString(icon) + _R(" out of range ")));
			return m_NodeImages->GetImageCount() - 1;
		}
		return icon;
	}
}
