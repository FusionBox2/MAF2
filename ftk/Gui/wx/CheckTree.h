#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/wx/Tree.h"

#include <map>

BEGIN_FTK_NAMESPACE

namespace model::data
{
	class Node;
}
using mafNode = model::data::Node;
class mafView;

namespace gui::wx
{
	class FTK_GUI_EXPORT CheckTree : public Tree
	{
	public:

		CheckTree(wxWindow* parent, wxWindowID id = wxID_ANY, bool CloseButton = false, bool HideTitle = false);

		~CheckTree() override;

		void NodeAdd(std::shared_ptr<mafNode> node);

		void NodeRemove(mafNode* node);

		mafNode* GetSelectedNode() const;

		void NodeSelected(mafNode* n);

		void NodeCollapse(mafNode* node);

		void NodeExpand(mafNode* node);

		void NodeCollapseSubTree(mafNode* node);

		void NodeExpandSubTree(mafNode* node);

		void NodeExpandVisible(mafNode* node);

		bool IsIconChecked(wxTreeItemId item);

		void NodeShow(mafNode* node, bool show);

		void NodeModified(mafNode* node);

		void ViewSelected(mafView* view);

		void ViewDeleted(mafView* view);

		// todo: 
		// setting m_enableSelect to false
		// doesn't prevent to change the selection using the keyboard !!
		//SIL. 7-4-2005: 

		  /** Enable the selection of a node tree.*/
		void EnableSelect(bool enable);

		wxTreeCtrl* GetTree() { return m_NodeTree; }

	protected:

		void NodeUpdateIcon(mafNode* n);

		void TreeUpdateIcon();

		virtual int GetVmeStatus(mafNode* vme);

		int ClassNameToIcon(const mafString& classname);

		virtual void InitializeImageList();

		wxBitmap MergeIcons(wxBitmap state, wxBitmap vme);

	public:

		void OnMouseDown(wxMouseEvent& event);

		void OnMouseUp(wxMouseEvent& event);

		virtual void OnIconClick(wxTreeItemId item);

		virtual void ShowContextualMenu(wxMouseEvent& event);

		void OnSelectionChanged(wxTreeEvent& event) override;

	protected:
		mafView* m_View = nullptr;
		bool     m_CanSelect = true;

		class CheckTreeItemData : public TreeItemData
		{
		public:
			CheckTreeItemData(intptr_t node_id, std::shared_ptr<mafNode> n = nullptr) : TreeItemData(node_id), sharednode(n) {}
			~CheckTreeItemData() override = default;
			void SetSharedNode(std::shared_ptr<mafNode> n) { sharednode = n; }
			std::shared_ptr<mafNode> GetSharedNode() const { return sharednode; }
		private:
			std::shared_ptr<mafNode> sharednode;
		};

		using MapClassNameToIcon = std::map<mafString, int> ;
		MapClassNameToIcon m_MapClassNameToIcon;
	};
}

using mafGUICheckTree = gui::wx::CheckTree;

END_FTK_NAMESPACE
