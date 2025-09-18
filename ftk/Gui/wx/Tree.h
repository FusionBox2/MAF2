#pragma once

#include "ftkConfigure.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/laywin.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <wx/hash.h>
#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"
#include "mafGUINamedPanel.h"

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	class FTK_GUI_EXPORT Tree : public mafGUINamedPanel, public mafEventSender
	{
	public:
		using NodeID = intptr_t;
		Tree(wxWindow* parent, wxWindowID id = wxID_ANY, bool CloseButton = false, bool HideTitle = false);
		~Tree() override;

		/** Clears all items in the tree. */
		void Reset();

		class TreeItemData;
		/** Create a new tree item with the specified parent,label and icon.
			Set parent = 0 to create the root. 0 is not a valid node_id.
		*/
		bool AddNode(NodeID node_id, NodeID parent_id, const mafString& label, int icon = 0, TreeItemData* data = nullptr);

		/** Delete the specified node, and its subtree. */
		bool DeleteNode(NodeID node_id);

		/** Set the label for the node. */
		bool SetNodeLabel(NodeID node_id, const mafString& label);

		/** Get the label for the node. Add by Mucci 19/09/2007*/
		mafString GetNodeLabel(NodeID node_id);

		/** Check if the node has children. Add by Mucci 19/09/2007*/
		bool NodeHasChildren(NodeID node_id);

		/** Return the parent id of the node. Add by Mucci 19/09/2007*/
		intptr_t GetNodeParent(NodeID node_id);

		/** Move a node, and its subtree. */
		bool SetNodeParent(NodeID node_id, NodeID parent_id);

		/** Set the icon for the node. */
		bool SetNodeIcon(NodeID node_id, int icon);

		/** Return the icon index for the node 'node_id'. */
		int  GetNodeIcon(NodeID node_id);

		/** Select the node. */
		bool SelectNode(NodeID node_id);

		/** Set the images to be used for the nodes.
			Must be set before adding any node.
			The default ImageList provide 4 icons :
			-1 gray dot
			-2 red dot
			-3 blue dot
			-4 yellow dot
		*/
		void SetImageList(wxImageList* img);

		/** Sort the children of node_id. (not the subtree)
			give node_id = 0 to specify the root. */
		void SortChildren(NodeID node_id = 0);

		/** Sort the subtree of node_id.
		give node_id = 0 to specify the root. */
		void SortSubTree(intptr_t NodeID = 0);

		/** collapse the children of node_id */
		void CollapseNode(NodeID node_id);

		/** expand the children of node_id */
		void ExpandNode(NodeID node_id);

		/** collapse the children of node_id */
		void CollapseNodeSubTree(NodeID node_id);

		/** expand the children of node_id */
		void ExpandNodeSubTree(NodeID node_id);

		void ExpandNodeVisible(NodeID node_id);

		/** Return the node item from node id. */
		wxTreeItemId ItemFromNode(NodeID node_id);

		/** Return node id from node item. */
		NodeID NodeFromItem(wxTreeItemId& item);


		void SetTreeStyle(long style) { m_NodeTree->SetWindowStyle(style); }
		long GetTreeStyle() { return m_NodeTree->GetWindowStyle(); }

	protected:
		/** Private function that notify the Listener of node selection and deselection. */
		virtual void OnSelectionChanged(wxTreeEvent& event);

		/** When tree is used on a wxNotebook m_NodeTree must be called on Sizing . */
		void OnSize(wxSizeEvent& event);

		/** Return true if node exist. */
		bool NodeExist(NodeID node_id);

		/** Delete recursively a node and its subtree. */
		void DeleteNode2(NodeID node_id);

		/** Move a node, and its subtree. */
		void SetNodeParent2(NodeID node_id, NodeID parent_id);

		/** Check that id is a valid index in the imagelist - return the (eventually clamped) value */
		int CheckIconId(int icon);

		bool IsRootHidden() { return (GetTreeStyle() & wxTR_HIDE_ROOT) != 0; }

		bool m_PreventNotify;
		NodeID m_NodeRoot;
		wxImageList* m_NodeImages = nullptr;
		wxHashTable* m_NodeTable = nullptr;

		//----------------------------------------------------------------------------
		// mafGUITreeItemData :
		/// Data to be attached to an item of mafGUITree, holds the reference to a node_id 
		//----------------------------------------------------------------------------
		class TreeItemData : public wxTreeItemData {
		public:
			TreeItemData(NodeID node_id) { m_NodeId = node_id; }
			NodeID GetNode() const { return m_NodeId; }
		protected:
			NodeID m_NodeId;
		};
		wxTreeCtrl* m_NodeTree;

		//----------------------------------------------------------------------------
		// mafGUITreeTableElement:
		/// specialized HashTable element used in mafGUITree to store a reference to a wxTreeItemId 
		//----------------------------------------------------------------------------
		class TreeTableElement : public wxObject
		{
		public:
			TreeTableElement(wxTreeItemId item) { m_TreeItemId = item; }
			wxTreeItemId GetItem() const { return m_TreeItemId; }
			void SetItem(wxTreeItemId item) { m_TreeItemId = item; }
		protected:
			wxTreeItemId m_TreeItemId;
		};
	};
}

using mafGUITree = gui::wx::Tree;

END_FTK_NAMESPACE
