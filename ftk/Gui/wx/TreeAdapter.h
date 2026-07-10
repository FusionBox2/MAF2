#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/ITreeViewModel.h"

#include "wx/treectrl.h"

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	class TreeAdapter
	{
	public:
		TreeAdapter(wxTreeCtrl* tree);

		void setModel(ITreeViewModel* model);

		void onNodeAdded(ITreeViewModel::NodeId n);

		void onNodeRemoved(ITreeViewModel::NodeId n);

		void onNodeMoved(ITreeViewModel::NodeId n);

		void onNodeChanged(ITreeViewModel::NodeId n);

		std::function<void(ITreeViewModel::NodeId)> onItemActivated;

	private:
		wxTreeItemId item(ITreeViewModel::NodeId node) const;

		void rebuild();

		wxTreeItemId createItem(wxTreeItemId parent, ITreeViewModel::NodeId node);

		wxTreeItemId buildRecursive(wxTreeItemId parent, ITreeViewModel::NodeId node);

		ITreeViewModel::NodeId getNode(wxTreeItemId id) const;

		void onBeginDrag(wxTreeEvent& e);

		void onEndDrag(wxTreeEvent& e);

		void onEndLabelEdit(wxTreeEvent& e);

		void onTreeItemActivated(wxTreeEvent& e);

		void OnSelectionChanged(wxTreeEvent& evt);

		wxTreeCtrl* m_tree;
		ITreeViewModel* m_model = nullptr;
		std::vector<base::Connection> m_connections;
		wxTreeItemId m_dragged;
		std::unordered_map<ITreeViewModel::NodeId, wxTreeItemId> m_nodeToItem;
	};
}

END_FTK_NAMESPACE
