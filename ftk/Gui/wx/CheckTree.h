#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/wx/Tree.h"

#include <map>

BEGIN_FTK_NAMESPACE

namespace model::data
{
	class Node;
}
class mafView;

namespace gui::wx
{
	class FTK_GUI_EXPORT CheckTree : public Tree
	{
	public:

		CheckTree(wxWindow* parent, wxWindowID id = wxID_ANY, bool CloseButton = false, bool HideTitle = false);

		~CheckTree() override;

		void NodeAdd(std::shared_ptr<model::data::Node> node);

		void NodeRemove(model::data::Node* node);

		model::data::Node* GetSelectedNode() const;

		void NodeSelected(model::data::Node* n);

		void NodeCollapse(model::data::Node* node);

		void NodeExpand(model::data::Node* node);

		void NodeCollapseSubTree(model::data::Node* node);

		void NodeExpandSubTree(model::data::Node* node);

		void NodeExpandVisible(model::data::Node* node);

		bool IsIconChecked(wxTreeItemId item);

		void NodeShow(model::data::Node* node, bool show);

		void NodeModified(model::data::Node* node);

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

		void NodeUpdateIcon(model::data::Node* n);

		void TreeUpdateIcon();

		virtual int GetVmeStatus(model::data::Node* vme);

		int ClassNameToIcon(const mafString& classname);

		virtual void InitializeImageList();

		wxBitmap MergeIcons(wxBitmap state, wxBitmap vme);

	public:

		void OnMouseDown(wxMouseEvent& event);

		void OnMouseUp(wxMouseEvent& event);

		virtual void OnIconClick(wxTreeItemId item);

		virtual void ShowContextualMenu(wxMouseEvent& event);

	protected:
		void OnSelectionChanged(wxTreeEvent& event) override;

		mafView* m_View = nullptr;
		bool     m_CanSelect = true;

		class CheckTreeItemData : public TreeItemData
		{
		public:
			CheckTreeItemData(intptr_t node_id, std::shared_ptr<model::data::Node> n = nullptr) : TreeItemData(node_id), sharednode(n) {}
			~CheckTreeItemData() override = default;
			void SetSharedNode(std::shared_ptr<model::data::Node> n) { sharednode = n; }
			std::shared_ptr<model::data::Node> GetSharedNode() const { return sharednode; }
		private:
			std::shared_ptr<model::data::Node> sharednode;
		};

		using MapClassNameToIcon = std::map<mafString, int> ;
		MapClassNameToIcon m_MapClassNameToIcon;
	};
}

using mafGUICheckTree = gui::wx::CheckTree;

END_FTK_NAMESPACE
