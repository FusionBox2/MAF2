#pragma once

#include "ftkConfigure.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/treectrl.h>

#include "mafEventSender.h"
#include "mafGUINamedPanel.h"

#include <unordered_map>

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	class FTK_GUI_EXPORT Tree : public mafGUINamedPanel, public mafEventSender
	{
	public:

		using NodeID = intptr_t;

		Tree(wxWindow* parent, wxWindowID id = wxID_ANY, bool CloseButton = false, bool HideTitle = false);

		~Tree() override;

		void Reset();

		class TreeItemData;

		bool AddNode(NodeID node_id, NodeID parent_id, const mafString& label, int icon = 0, int selectedIcon = -1, TreeItemData* data = nullptr);

		bool DeleteNode(NodeID node_id);

		mafString GetNodeLabel(NodeID node_id) const;

		bool SetNodeLabel(NodeID node_id, const mafString& label);

		int  GetNodeIcon(NodeID node_id) const;

		bool SetNodeIcon(NodeID node_id, int icon);

		NodeID GetSelected() const;

		bool SelectNode(NodeID node_id);

		void CollapseNode(NodeID node_id);

		void ExpandNode(NodeID node_id);

		void CollapseNodeSubTree(NodeID node_id);

		void ExpandNodeSubTree(NodeID node_id);

		void ExpandNodeVisible(NodeID node_id);

		bool NodeHasChildren(NodeID node_id) const;

		/** Set the images to be used for the nodes.
			Must be set before adding any node.
			The default ImageList provide 4 icons :
			-1 gray dot
			-2 red dot
			-3 blue dot
			-4 yellow dot
		*/
		void SetImageList(std::unique_ptr<wxImageList> img);

		wxTreeItemId ItemFromNode(NodeID node_id) const;

		NodeID NodeFromItem(const wxTreeItemId& item) const;

	protected:

		virtual void OnSelectionChanged(wxTreeEvent& event);

		void OnSize(wxSizeEvent& event);

		bool NodeExist(NodeID node_id) const;

		int CheckIconId(int icon) const;

		bool m_PreventNotify = false;
		wxTreeCtrl* m_NodeTree = nullptr;
		std::unique_ptr<wxImageList> m_NodeImages;
		std::unordered_map<NodeID, wxTreeItemId> m_NodeTable;

		class TreeItemData : public wxTreeItemData
		{
		public:
			TreeItemData(NodeID node_id) { m_NodeId = node_id; }

			NodeID GetNode() const { return m_NodeId; }

		protected:

			NodeID m_NodeId;
		};
	};
}

END_FTK_NAMESPACE
