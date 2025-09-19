#pragma once

#include "ftkConfigure.h"

#include <wx/notebook.h>
#include <wx/splitter.h>

#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"
#include "ftk/Gui/wx/CheckTree.h"

BEGIN_FTK_NAMESPACE

class mafGUIHolder;
class mafGUI;
class mafGUIPanelStack;
class mafView;
class mafGUISplittedPanel;

namespace gui::wx
{
	class MAF_EXPORT SideBar : public mafEventSender
	{
	public:
		SideBar(wxWindow* parent, int id, mafBaseEventHandler* Listener, long style = DOUBLE_NOTEBOOK);
		~SideBar() override;

		enum SIDEBAR_STYLE
		{
			SINGLE_NOTEBOOK,
			DOUBLE_NOTEBOOK
		};

		void NodeAdd(std::shared_ptr<model::data::Node> node);

		void NodeRemove(model::data::Node* node);

		void NodeSelected(model::data::Node* node);

		void NodeCollapse(model::data::Node* node);

		void NodeExpand(model::data::Node* node);

		void NodeCollapseSubTree(model::data::Node* node);

		void NodeExpandSubTree(model::data::Node* node);

		void NodeExpandVisible(model::data::Node* node);

		void NodeShow(model::data::Node* node, bool visibility);

		void NodeModified(model::data::Node* node);

		void VmePropertyRemove(mafGUI* gui);

		void ViewSelect(mafView* view);

		void ViewDeleted(mafView* view);

		void OpShowGui(bool push_gui, mafGUIPanel* panel);

		void OpHideGui(bool view_closed);

		void EnableSelect(bool enable);

		void Show();

		mafGUICheckTree* GetTree() { return m_Tree; }

		bool IsShown() const { return m_Notebook->IsShown(); }
		void Show(bool show) { m_Notebook->Show(show); }

		wxNotebook* m_Notebook;
		wxNotebook* m_VmeNotebook;
		//mafGUINamedPanel    *m_SideSplittedPanel;
		wxSplitterWindow* m_SideSplittedPanel;
	protected:
		void UpdateVmePanel();

		mafGUIPanelStack* m_OpPanel;
		mafGUIHolder* m_ViewPropertyPanel;
		mafGUICheckTree* m_Tree;

		mafGUIHolder* m_VmePanel;
		mafGUIHolder* m_VmeOutputPanel;
		mafGUIHolder* m_VmePipePanel;

		mafView* m_SelectedView = nullptr;

		mafGUI* m_NewAppendingGUI;
		mafGUI* m_OldAppendingGUI;

		mafGUI* m_CurrentVmeGui = nullptr;
		mafGUI* m_CurrentPipeGui = nullptr;

		int m_Style;
	};
}

using mafSideBar = gui::wx::SideBar;

END_FTK_NAMESPACE
