// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wupdlock.h>

#include "Document.h"
#include "DocumentContext.h"
#include "DocumentManager.h"
#include "ISelectionController.h"
#include "mafInteractionManager.h"
#include "mafOp.h"
#include "ftk/Core/Operation.h"
#include "ftk/Core/OperationManager.h"
#include "ftk/Gui/wx/TreeAdapter.h"
#include "ftk/Gui/wx/PropertyView.h"
#include "PropertyViewModel.h"
#include "VisibilityStatusController.h"
#include "ftk/Core/IProperty.h"
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
//#include "wx/cmdline.h"

#if wxUSE_AUI
#include "wx/aui/tabmdi.h"
#include <wx/aui/aui.h>
#endif // wxUSE_AUI

#include <wx/artprov.h>
#include <wx/preferences.h>
#include <wx/aboutdlg.h>

#include "ftk/Gui/wx/MainFrame.h"
#include "ftk/Core/Node.h"
#include "ftk/IO/Parse.h"
#include "ftk/IO/StorageElement.h"
/*#include "Gui/mafGUI.h"
#include "Gui/mafGUIDockSettings.h"*/
#include "mafPics.h"
#include "ftk/Gui/wx/SideBar.h"

#include "ViewManager.h"
#include "TreeViewModel.h"
#include "TreeStatusViewModel.h"
#include "ftk/Gui/wx/TreeView.h"
#include "ftk/VME/VMEFactory.h"
#include "ftk/VME/PipeFactoryVME.h"
#include "ftk/Core/AttributeFactory.h"
#include "ftk/VME/ItemFactory.h"
#include "VTKViewModel.h"
#include "ftk/Gui/wx/OldGuiView.h"
#include "ftk/Core/IPropertiesBuilder.h"
#include "ftk/Core/OperationProperty.h"
#include "OperationCreateFactory.h"
#include "mafOpCreateGroup.h"
#include <memory>
#include "lhpOpImporterOBJ.h"
#include "lhpOpImporterMetashapeOBJ.h"


template<class BaseFrame>
class OldAppFrame : public BaseFrame
{
public:
	enum MENU_ID_ENUM
	{
		ID_START = wxID_LAST,
		ID_STATUSBAR = ID_START,
		ID_STANDARD_TOOLBAR,
		ID_ADDITIONAL_TOOLBAR,
		ID_NAVIGATOR,
		ID_LOGTEXTCTRL,
		ID_TIMEBAR,
		ID_END
		//ID_VIEW_START,
		//ID_VIEW_END
	};
	OldAppFrame();

	OldAppFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	~OldAppFrame() override;

	bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	//protected:

		//void OnAbout(wxCommandEvent& e);

		//void OnMenu(wxCommandEvent& e);

		//void OnUpdateUI(wxUpdateUIEvent& e);

private:

	void CreateMenuBar();

	std::unique_ptr<wxMenu> CreateViewMenu();

	wxWindow* CreateStdToolBar();

	wxWindow* CreateAddToolBar();

	wxWindow* CreateNavigator();

	wxWindow* CreateTimebar();

	wxTextCtrl* CreateLogWindow();

protected:
	wxNotebook* m_Notebook = nullptr;
	wxMenu* m_viewMenu = nullptr;
	wxMenu* m_operationsMenu = nullptr;
};

template <class BaseFrame>
OldAppFrame<BaseFrame>::OldAppFrame() = default;

template <class BaseFrame>
OldAppFrame<BaseFrame>::OldAppFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	Create(parent, id, title, pos, size, style, name);
}

template <class BaseFrame>
OldAppFrame<BaseFrame>::~OldAppFrame() = default;

template <class BaseFrame>
bool OldAppFrame<BaseFrame>::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
	{
		return false;
	}
	BaseFrame::AddPane(CreateStdToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Standard").Name("Standard Toolbar").Top().Floatable(false));
	BaseFrame::AddPane(CreateAddToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Additional").Name("Additional Toolbar").Top().Position(2).Floatable(false));
	auto m_log = CreateLogWindow();
	//auto m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_log));
	//wxLogMessage("This is the log window");
	BaseFrame::AddPane(m_log, wxAuiPaneInfo().Bottom().Name("Help").Caption(_("Help Window")));
	BaseFrame::AddPane(CreateNavigator(), wxAuiPaneInfo().Name("sidebar").Caption(wxT("ControlBar")).Right().Layer(2).MinSize(BaseFrame::FromDIP(wxSize(240, 450))).TopDockable(false).BottomDockable(false));
	BaseFrame::AddPane(CreateTimebar(), wxAuiPaneInfo().Name("timebar").Caption(wxT("TimeBar")).Bottom().Row(1).Layer(2).ToolbarPane().LeftDockable(false).RightDockable(false).MinSize(100, 22).Floatable(false).Gripper(false).Resizable(false).Movable(false)
	);

	CreateMenuBar();
	BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent& WXUNUSED(event)) {BaseFrame::Close(); }, wxID_EXIT);
	return true;
}

template <class BaseFrame>
wxWindow* OldAppFrame<BaseFrame>::CreateStdToolBar()
{
	auto toolBar = new wxAuiToolBar(this, ID_STANDARD_TOOLBAR);
	//auto toolBar = new wxToolBar(this, ID_STANDARD_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT);
	//hdpi m_StdToolBar->SetToolBitmapSize(wxSize(16, 15));
	toolBar->AddTool(wxID_NEW, wxGetStockLabel(wxID_NEW, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_NEW), wxGetStockLabel(wxID_NEW, wxSTOCK_WITH_ACCELERATOR));//, wxGetStockHelpString(wxID_NEW)
	toolBar->AddTool(wxID_OPEN, wxGetStockLabel(wxID_OPEN, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN), wxGetStockLabel(wxID_OPEN, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddTool(wxID_SAVE, wxGetStockLabel(wxID_SAVE, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_FILE_SAVE), wxGetStockLabel(wxID_SAVE, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_ABOUT, wxGetStockLabel(wxID_ABOUT, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_INFORMATION), wxGetStockLabel(wxID_ABOUT, wxSTOCK_WITH_ACCELERATOR));
	toolBar->Realize();
	return toolBar;
}

template <class BaseFrame>
wxWindow* OldAppFrame<BaseFrame>::CreateAddToolBar()
{
	auto toolBar = new wxAuiToolBar(this, ID_ADDITIONAL_TOOLBAR);
	//auto toolBar = new wxToolBar(this, ID_ADDITIONAL_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT);
	//hdpi m_AddToolBar->SetToolBitmapSize(wxSize(16, 15));
	toolBar->AddTool(wxID_UNDO, wxGetStockLabel(wxID_UNDO, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_UNDO), wxGetStockLabel(wxID_UNDO, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddTool(wxID_REDO, wxGetStockLabel(wxID_REDO, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_REDO), wxGetStockLabel(wxID_REDO, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_COPY, wxGetStockLabel(wxID_COPY, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_COPY), wxGetStockLabel(wxID_COPY, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddTool(wxID_CUT, wxGetStockLabel(wxID_CUT, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_CUT), wxGetStockLabel(wxID_CUT, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddTool(wxID_PASTE, wxGetStockLabel(wxID_PASTE, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_PASTE), wxGetStockLabel(wxID_PASTE, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddTool(wxID_DELETE, wxGetStockLabel(wxID_DELETE, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_DELETE), wxGetStockLabel(wxID_DELETE, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_FIND, wxGetStockLabel(wxID_FIND, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_FIND), wxGetStockLabel(wxID_FIND, wxSTOCK_WITH_ACCELERATOR));
	toolBar->Realize();
	return toolBar;
}

template <class BaseFrame>
wxWindow* OldAppFrame<BaseFrame>::CreateNavigator()
{
	//auto
	m_Notebook = new wxNotebook(this, ID_NAVIGATOR);
	m_Notebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

	//auto m_SideSplittedPanel = new wxSplitterWindow(m_Notebook, -1, wxDefaultPosition, wxDefaultSize,/*wxSP_3DSASH |*/ wxSP_NOBORDER);

	//tree
	//auto m_Tree = new gui::wx::NamedPanel(m_SideSplittedPanel, wxID_ANY, false, true);

	//auto
	//m_NodeTree = new wxTreeCtrl(m_Tree, wxID_ANY, wxDefaultPosition, BaseFrame::FromDIP(wxSize(100, 100)), wxNO_BORDER | wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS);
	//m_NodeTree->Bind(wxEVT_TREE_SEL_CHANGED, [this](wxTreeEvent& event) {OnSelectionChanged(event); });
	//m_Tree->Add(m_NodeTree, 1, wxEXPAND);
	//tree ----------------------------
	//auto m_Tree = new mafGUICheckTree(m_SideSplittedPanel, -1, false, true);
	//m_Tree->SetListener(Listener);
	//m_Tree->SetSize(-1, BaseFrame::FromDIP(300));
	//m_Tree->SetTitle(_R(" vme hierarchy: "));
	//m_Notebook->AddPage(m_SideSplittedPanel, _("data tree"), true);

	//view property panel
	auto m_ViewPropertyPanel = new gui::wx::NamedPanel/*mafGUIHolder*/(m_Notebook, wxID_ANY, false, true);
	m_ViewPropertyPanel->SetTitle(_L("no view selected:"));
	m_Notebook->AddPage(m_ViewPropertyPanel, _("view settings"));

	//op_panel ----------------------------
	auto m_OpPanel = new gui::wx::Panel/*mafGUIPanelStack*/(m_Notebook, wxID_ANY);
	//auto empty_op = new gui::wx::NamedPanel/*mafGUINamedPanel*/(m_OpPanel, wxID_ANY, false, true);
	/*empty_op->SetTitle(_L(" no operation running:"));
	m_OpPanel->Push(empty_op);*/
	m_Notebook->AddPage(m_OpPanel, _("operation"));

	bool doubleNotebook = true;
	if (doubleNotebook)//style == DOUBLE_NOTEBOOK)
	{
		//auto m_VmeNotebook = new wxNotebook(m_SideSplittedPanel, wxID_ANY);
		//m_VmeNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

		//auto m_VmeOutputPanel = new gui::wx::NamedPanel/*mafGUIHolder*/(m_VmeNotebook, wxID_ANY, false, true);
		//m_VmeNotebook->AddPage(m_VmeOutputPanel, _("vme output"));
		//auto m_VmePipePanel = new gui::wx::NamedPanel/*mafGUIHolder*/(m_VmeNotebook, wxID_ANY, false, true);
		//m_VmeNotebook->AddPage(m_VmePipePanel, _("visual props"));
		//auto m_VmePanel = new gui::wx::NamedPanel/*mafGUIHolder*/(m_VmeNotebook, wxID_ANY, false, true);
		//m_VmeNotebook->AddPage(m_VmePanel, _("vme"));

		//m_SideSplittedPanel->SetMinimumPaneSize(BaseFrame::FromDIP(50));
		//m_SideSplittedPanel->SplitHorizontally(m_Tree, m_VmeNotebook);
	}
	else
	{
		//auto m_VmePanel = new gui::wx::NamedPanel/*mafGUIHolder*/(m_Notebook, wxID_ANY, false, true);
		//m_Notebook->AddPage(m_VmePanel, _("vme"));

		//auto m_VmeOutputPanel = new gui::wx::NamedPanel/*mafGUIHolder*/(m_SideSplittedPanel, wxID_ANY, false, true);
		//m_VmeOutputPanel->Show(false);
		//m_SideSplittedPanel->SetMinimumPaneSize(BaseFrame::FromDIP(5));
		//m_SideSplittedPanel->SplitHorizontally(m_Tree, m_VmeOutputPanel);

		//     m_VmePipePanel = new mafGUIHolder(m_Notebook,-1,false,true);
		//     m_VmePipePanel->Show(false);
	}

	return m_Notebook;
}

template <class BaseFrame>
wxWindow* OldAppFrame<BaseFrame>::CreateTimebar()
{
	auto timebar = new gui::wx::Panel(this, ID_TIMEBAR, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL);
	auto m_Sizer = new wxBoxSizer(wxHORIZONTAL);
	timebar->SetAutoLayout(true);
	timebar->SetSizer(m_Sizer);
	m_Sizer->Fit(timebar);
	m_Sizer->SetSizeHints(timebar);

	auto labTimeMin = new wxStaticText(timebar, wxID_ANY, "0");// mafStringToWx(m_TimeMinString));
	//labTimeMin->SetValidator(mafGUIValidator(timebar, -1, labTimeMin, &m_TimeMinString));

	auto labTimeMax = new wxStaticText(timebar, wxID_ANY, "1000");// mafStringToWx(m_TimeMaxString));
	//labTimeMax->SetValidator(mafGUIValidator(timebar, -1, labTimeMax, &m_TimeMaxString));

	auto lab1 = new wxStaticText(timebar, wxID_ANY, " time ");
	auto m_TimeBarEntry = new wxTextCtrl(timebar, wxID_ANY/*ID_ENTRY*/, "", wxDefaultPosition, BaseFrame::FromDIP(wxSize(40, 17)), wxTE_PROCESS_ENTER);
	//m_TimeBarEntry->SetValidator(mafGUIValidator(timebar, ID_ENTRY, m_TimeBarEntry, &m_Time, m_TimeMin, m_TimeMax));

	auto m_TimeBarSlider = new wxSlider(timebar, wxID_ANY/*ID_SLIDER*/, 0, 0, 1);// , m_Time, m_TimeMin, m_TimeMax);
	//m_TimeBarSlider->SetValidator(mafGUIValidator(timebar, ID_SLIDER, m_TimeBarSlider, &m_Time, m_TimeBarEntry));
	//m_TimeBarSlider->SetNumberOfSteps(m_NumberOfIntervals);

	//TransferDataToWindow();
	m_Sizer->Add(labTimeMin, 0, wxALIGN_CENTER);
	m_Sizer->Add(m_TimeBarSlider, 1, wxEXPAND);
	m_Sizer->Add(labTimeMax, 0, wxALIGN_CENTER);
	m_Sizer->Add(lab1, 0, wxALIGN_CENTER);
	m_Sizer->Add(m_TimeBarEntry, 0, wxALIGN_CENTER);

	//m_TimeBarButtons[0] = new mafGUIPicButton(timebar, _R("TIME_BEGIN"), TIME_BEGIN, this);
	//m_TimeBarButtons[1] = new mafGUIPicButton(timebar, _R("TIME_PREV"), TIME_PREV, this);
	//m_TimeBarButtons[2] = new mafGUIPicButton(timebar, _R("TIME_PLAY"), TIME_PLAY, this);
	//m_TimeBarButtons[3] = new mafGUIPicButton(timebar, _R("TIME_NEXT"), TIME_NEXT, this);
	//m_TimeBarButtons[4] = new mafGUIPicButton(timebar, _R("TIME_END"), TIME_END, this);

	m_Sizer->Add(new wxBitmapButton(timebar, wxID_ANY, mafPictureFactory::GetPictureFactory()->GetBmp(_R("TIME_BEGIN"))));
	m_Sizer->Add(new wxBitmapButton(timebar, wxID_ANY, mafPictureFactory::GetPictureFactory()->GetBmp(_R("TIME_PREV"))));
	m_Sizer->Add(new wxBitmapButton(timebar, wxID_ANY, mafPictureFactory::GetPictureFactory()->GetBmp(_R("TIME_PLAY"))));
	m_Sizer->Add(new wxBitmapButton(timebar, wxID_ANY, mafPictureFactory::GetPictureFactory()->GetBmp(_R("TIME_NEXT"))));
	m_Sizer->Add(new wxBitmapButton(timebar, wxID_ANY, mafPictureFactory::GetPictureFactory()->GetBmp(_R("TIME_END"))));
	return timebar;
}

template<class BaseFrame>
wxTextCtrl* OldAppFrame<BaseFrame>::CreateLogWindow()
{
	return new wxTextCtrl(this, ID_LOGTEXTCTRL, wxEmptyString, wxDefaultPosition, BaseFrame::FromDIP(wxSize(250, 100)), wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
}

template <class BaseFrame>
void OldAppFrame<BaseFrame>::CreateMenuBar()
{
	auto fileMenu = std::make_unique<wxMenu>();
	fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT);

	auto editMenu = std::make_unique<wxMenu>();
	editMenu->Append(wxID_UNDO);
	editMenu->Append(wxID_REDO);
	editMenu->AppendSeparator();
	editMenu->Append(wxID_FIND);

	auto viewMenu = CreateViewMenu();

	auto  ppMenu = std::make_unique<wxMenu>();
	m_operationsMenu = ppMenu.release();

	auto toolsMenu = std::make_unique<wxMenu>();
	toolsMenu->Append(wxID_PREFERENCES);

	auto helpMenu = std::make_unique<wxMenu>();
	helpMenu->Append(wxID_ABOUT);

	//CreateMenuBar();
	//CreateStatus();
	//m_auiManager.SetManagedWindow(this);
	//ribbon = CreateRibbon();
	//AddPane(ribbon, wxAuiPaneInfo().ToolbarPane().Caption("Ribbon").Name("Ribbon").Top().Floatable(false));

	//AddPane(CreateNavigator(), wxAuiPaneInfo().Right().Layer(1)/*.PinButton().MinimizeButton().MaximizeButton()*/.Name("Navigator").Caption("Navigator").MinSize(BaseFrame::FromDIP(wxSize(240, 450))));
	//AddPane(CreateLogWindow(), wxAuiPaneInfo().Bottom().Name("Help").Caption(_("Help Window")));

	/*auto menuFile = std::make_unique<wxMenu>();
	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);
	/*if (m_mode == FTK::View::Mode::Mode_Single)
		AppendDocumentFileCommands(menuFile, true);* /
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);*/

	auto menubar = std::make_unique<wxMenuBar>();
	menubar->Append(fileMenu.release(), wxGetStockLabel(wxID_FILE));
	menubar->Append(editMenu.release(), wxGetStockLabel(wxID_EDIT));
	if (viewMenu)
	{
		m_viewMenu = viewMenu.get();
		menubar->Append(viewMenu.release(), "View");
	}
	if (m_operationsMenu)
	{
		menubar->Append(m_operationsMenu, "Operations");
	}
	menubar->Append(toolsMenu.release(), _("Tools"));
	menubar->Append(helpMenu.release(), wxGetStockLabel(wxID_HELP));
	BaseFrame::SetMenuBar(menubar.release());
}

template<class BaseFrame>
std::unique_ptr<wxMenu> OldAppFrame<BaseFrame>::CreateViewMenu()
{
	std::unique_ptr<wxMenu> viewMenu = std::make_unique<wxMenu>();
	std::unique_ptr<wxMenu> toolbarsMenu;
	for (auto& pi : BaseFrame::GetPanes())
	{
		if (pi.caption.empty())
		{
			continue;
		}
		if (pi.IsToolbar())
		{
			if (!toolbarsMenu)
			{
				toolbarsMenu = std::make_unique<wxMenu>();
			}
			toolbarsMenu->Append(pi.window->GetId(), pi.caption, "", wxITEM_CHECK);
		}
		else// if (pi.window->GetId() != wxID_ANY)
		{
			viewMenu->Append(pi.window->GetId(), pi.caption, "", wxITEM_CHECK);
		}

		BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent& event)
			{
				auto& pi = BaseFrame::GetPane(event.GetId());
				BaseFrame::ShowPane(pi, !BaseFrame::IsPaneShown(pi));
			}, pi.window->GetId());
		BaseFrame::Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event)
			{
				event.Check(BaseFrame::IsPaneShown(event.GetId()));
			}, pi.window->GetId());
	}
	if (auto statusbar = BaseFrame::GetStatusBar())
	{
		viewMenu->Append(ID_STATUSBAR, "Status bar", "", wxITEM_CHECK);
		BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent& event)
			{
				BaseFrame::ShowStatus(!BaseFrame::IsStatusShown());
			}, ID_STATUSBAR);
		BaseFrame::Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event)
			{
				event.Check(BaseFrame::IsStatusShown());
			}, ID_STATUSBAR);
	}
	if (toolbarsMenu)
	{
		viewMenu->Insert(0, wxID_ANY, "Toolbars", toolbarsMenu.release());
	}
	return viewMenu;
}



#ifdef jkj
namespace FTK
{
	class Document
	{
	public:
		Document() = default;
	};

	class DocumentManager
	{
	public:
		DocumentManager() = default;
		bool New() { mDocument = std::make_unique<Document>(); return true; }
		bool Open(const std::string & url)
		{
			io::Reader reader(_R("MSF"), _R("2.2"));
			reader.Load(mafStdToString(url));
			auto node = reader.GetRoot();
			auto root = node[_R("Root")].As<model::data::Node>();
			mDocument = std::make_unique<Document>();
			return true;
		}
		bool Save() { return true; }
		bool SaveAs() { return true; }
		bool Close() { mDocument = nullptr; return true; }
	private:
		std::unique_ptr<Document> mDocument;
	};
	class Logic
	{
	public:
		Logic()// = default;
		{
			manager.Open("D:/Downloads/wetransfer_lhpsergiorc_2026-05-18_1355/lhpSergiorc/lhpSergiorc.msf");
		}
		DocumentManager manager;
	};

	class MainFrameAnyBase
	{
	public:
		MainFrameAnyBase(wxWindow* frame)
			: m_frame(frame)
		{
			m_logic = std::make_unique<Logic>();
		}

		MainFrameAnyBase(const MainFrameAnyBase&) = delete;
		MainFrameAnyBase& operator=(const MainFrameAnyBase&) = delete;
		MainFrameAnyBase(MainFrameAnyBase&&) = delete;
		MainFrameAnyBase& operator=(MainFrameAnyBase&&) = delete;
		~MainFrameAnyBase() = default;

		Logic* GetLogic() const { return m_logic.get(); }

	protected:
		// This is similar to wxDocChildFrameAnyBase method with the same name:
		// while we're not an event handler ourselves and so can't override
		// TryBefore(), we provide a helper that the derived template class can use
		// from its TryBefore() implementation.
		bool TryProcessEvent(wxEvent& event);

		wxWindow* const m_frame = nullptr;
		std::unique_ptr<Logic> m_logic;
	};

	bool MainFrameAnybase::TryProcessEvent(wxEvent& event)
	{
		if (!m_logic)
			return false;

		// If we have an active view, its associated child frame may have
		// already forwarded the event to wxDocManager, check for this:
		//if (auto view = m_docManager->GetAnyUsableView())
		//{
			//if (auto childFrame = view->GetDocChildFrame(); childFrame && childFrame->HasAlreadyProcessed(event))
				//return false;
		//}

		// But forward the event to wxDocManager ourselves if there are no views at
		// all or if this event hadn't been sent to the child frame previously.
		return false;//m_docManager->ProcessEventLocally(event);
	}



	template <class BaseFrame>
	class MainFrameAny : public BaseFrame, public MainFrameAnyBase
	{
	public:
		MainFrameAny() : MainFrameAnyBase(this) {}

		MainFrameAny(
			wxFrame* frame,
			wxWindowID id,
			const wxString& title,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_FRAME_STYLE,
			const wxString& name = wxASCII_STR(wxFrameNameStr))
			: MainFrameAnyBase(this)
		{
			Create(frame, id, title, pos, size, style, name);
		}

		MainFrameAny(const MainFrameAny&) = delete;
		MainFrameAny& operator=(const MainFrameAny&) = delete;

		bool Create(
			wxFrame* frame,
			wxWindowID id,
			const wxString& title,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_FRAME_STYLE,
			const wxString& name = wxASCII_STR(wxFrameNameStr))
		{
			if (!BaseFrame::Create(frame, id, title, pos, size, style, name))
				return false;

			this->Bind(wxEVT_MENU, &MainFrameAny::OnExit, this, wxID_EXIT);
			this->Bind(wxEVT_CLOSE_WINDOW, &MainFrameAny::OnCloseWindow, this);

			return true;
		}

	protected:
		// hook the document manager into event handling chain here
		virtual bool TryBefore(wxEvent& event) override
		{
			// It is important to send the event to the base class first as
			// wxMDIParentFrame overrides its TryBefore() to send the menu events
			// to the currently active child frame and the child must get them
			// before our own TryProcessEvent() is executed, not afterwards.
			return BaseFrame::TryBefore(event) || TryProcessEvent(event);
		}

	private:
		void OnExit(wxCommandEvent& WXUNUSED(event))
		{
			BaseFrame::Close();
		}

		void OnCloseWindow(wxCloseEvent& event)
		{
			/*if (m_docManager && !m_docManager->Clear(!event.CanVeto()))
			{
				// The user decided not to close finally, abort.
				event.Veto();
			}
			else*/
			{
				// Just skip the event, base class handler will destroy the window.
				event.Skip();
			}
		}
	};

	template <class ChildFrame, class ParentFrame>
	class ChildFrameAny : public ChildFrame//, public ftkDocChildFrameAnyBase
	{
	public:
		typedef ChildFrame BaseClass;

		// default ctor, use Create after it
		ChildFrameAny() = default;

		ChildFrameAny(const ChildFrameAny&) = delete;
		ChildFrameAny& operator=(const ChildFrameAny&) = delete;

		// ctor for a frame showing the given view of the specified document
		ChildFrameAny(//FTK::View* view,
			ParentFrame* parent,
			wxWindowID id,
			const wxString& title,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_FRAME_STYLE,
			const wxString& name = wxASCII_STR(wxFrameNameStr))
		{
			Create(//view,
				parent, id, title, pos, size, style, name);
		}

		bool Create(//FTK::View* view,
			ParentFrame* parent,
			wxWindowID id,
			const wxString& title,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDEFAULT_FRAME_STYLE,
			const wxString& name = wxASCII_STR(wxFrameNameStr))
		{
			this->Bind(wxEVT_ACTIVATE, &ChildFrameAny::OnActivate, this);
			this->Bind(wxEVT_CLOSE_WINDOW, &ChildFrameAny::OnCloseWindow, this);

			//if (!ftkDocChildFrameAnybase::Create(view, this))
				//return false;

			if (!BaseClass::Create(parent, id, title, pos, size, style, name))
				return false;

			return true;
		}

	protected:
		// hook the child view into event handlers chain here
		//virtual bool TryBefore(wxEvent& event) override
		//{
			//return TryProcessEvent(event) || BaseClass::TryBefore(event);
		//}

	private:
		void OnActivate(wxActivateEvent& event)
		{
			BaseClass::OnActivate(event);

			//if (m_childView)
				//m_childView->Activate(event.GetActive());
		}

		void OnCloseWindow(wxCloseEvent& event)
		{
			//if (CloseView(event))
				//this->Destroy();
			//else: vetoed
		}
	};
}



bool App::OnInit()
{
	if (!wxApp::OnInit())
	{
		return false;
	}

	auto frame = CreateMainFrame();
	if (!frame)
	{
		return false;
	}

	auto menuFile = new wxMenu;

	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);

	/*if (m_mode == FTK::View::Mode::Mode_Single)
		AppendDocumentFileCommands(menuFile, true);*/

	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	// A nice touch: a history of files visited. Use this menu.
	//docManager->FileHistoryUseMenu(menuFile);
#if wxUSE_CONFIG
	//docManager->FileHistoryLoad(*wxConfig::Get());
#endif // wxUSE_CONFIG

	CreateMenuBarForFrame(frame, menuFile);
	//frame->Bind(wxEVT_MENU, [frame](const wxCommandEvent& event){frame->Close();}, wxID_EXIT);

	frame->SetIcon(wxICON(doc));
	frame->Centre();
	frame->Show();
	return true;
}


//void App::OnForceCloseAll(wxCommandEvent& WXUNUSED(event))
//{
	// Pass "true" here to force closing just for testing this functionality,
	// there is no real reason to force the issue here.
	//FTK::DocManager::GetDocumentManager()->CloseDocuments(true);
//}



struct MySettings
{
	DockSettings m_dockSettings;
	bool m_general = true;
	bool m_advanced = true;
};

class App : public wxApp
{
public:
	App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	~App() override;

	bool OnInit() override;
	int OnExit() override;
	void OnInitCmdLine(wxCmdLineParser& parser) override;
	bool OnCmdLineParsed(wxCmdLineParser& parser) override;

	MySettings& GetSettings() { return m_settings; }
	void UpdateSettings(const MySettings& settings);
#ifdef __WXMAC__
	//void MacNewFile() override;
#endif // __WXMAC__

private:
	MyFrame<PaneFrame<wxMDIParentFrame>>* m_frame;
	wxFrame* CreateMainFrame();

	// force close all windows
	//void OnForceCloseAll(wxCommandEvent& event);

#if wxUSE_MDI_ARCHITECTURE
	const char* const MDI = "mdi";
#endif
#if wxUSE_AUI
	const char* const AUI = "aui";
#endif
	const char* const SDI = "sdi";
	const char* const SINGLE = "single";
	ftkString m_mode;
	MySettings m_settings;
};
wxDECLARE_APP(App);

class PrefsPageGeneralPanel : public wxPanel
{
public:
	PrefsPageGeneralPanel(wxWindow* parent) : wxPanel(parent)
	{
		m_useMarkdown = new wxCheckBox(this, wxID_ANY, "Use Markdown syntax");
		m_spellcheck = new wxCheckBox(this, wxID_ANY, "Check spelling");

		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(m_useMarkdown, wxSizerFlags().Border());
		sizer->Add(m_spellcheck, wxSizerFlags().Border());

		SetSizerAndFit(sizer);

		m_useMarkdown->Bind(wxEVT_CHECKBOX,
			&PrefsPageGeneralPanel::ChangedUseMarkdown,
			this);
		m_spellcheck->Bind(wxEVT_CHECKBOX,
			&PrefsPageGeneralPanel::ChangedSpellcheck,
			this);
	}

	bool TransferDataToWindow() override
	{
		//m_settingsCurrent = wxGetApp().GetSettings();
		//m_useMarkdown->SetValue(m_settingsCurrent.m_general);
		//m_spellcheck->SetValue(m_settingsCurrent.m_advanced);
		return true;
	}

	bool TransferDataFromWindow() override
	{
		// Called on platforms with modal preferences dialog to save and apply
		// the changes.
		//wxGetApp().UpdateSettings(m_settingsCurrent);
		return true;
	}

private:
	void UpdateSettingsIfNecessary()
	{
		// On some platforms (OS X, GNOME), changes to preferences are applied
		// immediately rather than after the OK or Apply button is pressed, so
		// we need to take them into account as soon as they happen. On others
		// (MSW), we need to wait until the changes are accepted by the user by
		// pressing the "OK" button. To reuse the same code for both cases, we
		// always update m_settingsCurrent object under all platforms, but only
		// update the real application settings if necessary here.
		if (wxPreferencesEditor::ShouldApplyChangesImmediately())
		{
			wxGetApp().UpdateSettings(m_settingsCurrent);
		}
	}

	void ChangedUseMarkdown(wxCommandEvent& e)
	{
		m_settingsCurrent.m_general = e.IsChecked();
		UpdateSettingsIfNecessary();
	}

	void ChangedSpellcheck(wxCommandEvent& e)
	{
		m_settingsCurrent.m_advanced = e.IsChecked();
		UpdateSettingsIfNecessary();
	}

	wxCheckBox* m_useMarkdown;
	wxCheckBox* m_spellcheck;

	// Settings corresponding to the current values in this dialog.
	MySettings m_settingsCurrent;
};

class PrefsPageGeneral : public wxStockPreferencesPage
{
public:
	PrefsPageGeneral() : wxStockPreferencesPage(Kind_General) {}
	virtual wxWindow* CreateWindow(wxWindow* parent) override
	{
		return new PrefsPageGeneralPanel(parent);
	}
};


class PrefsPageTopicsPanel : public wxPanel
{
public:
	PrefsPageTopicsPanel(wxWindow* parent) : wxPanel(parent)
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new wxStaticText(this, wxID_ANY, "Search in these topics:"), wxSizerFlags().Border());
		wxListBox* box = new wxListBox(this, wxID_ANY);
		box->SetMinSize(wxSize(400, 300));
		sizer->Add(box, wxSizerFlags(1).Border().Expand());
		m_fulltext = new wxCheckBox(this, wxID_ANY, "Automatically build fulltext index");
		sizer->Add(m_fulltext, wxSizerFlags().Border());

		SetSizerAndFit(sizer);

		if (wxPreferencesEditor::ShouldApplyChangesImmediately())
		{
			m_fulltext->Bind(wxEVT_CHECKBOX,
				&PrefsPageTopicsPanel::ChangedFulltext,
				this);
		}
	}

	virtual bool TransferDataToWindow() override
	{
		// This is the place where you can initialize values, e.g. from wxConfig.
		// For demonstration purposes, we just set hardcoded values.
		//m_fulltext->SetValue(true);
		// TODO: handle the listbox
		return true;
	}

	virtual bool TransferDataFromWindow() override
	{
		// Called on platforms with modal preferences dialog to save and apply
		// the changes.
		//wxCommandEvent dummy;
		//ChangedFulltext(dummy);
		// TODO: handle the listbox
		return true;
	}

private:
	void ChangedFulltext(wxCommandEvent& WXUNUSED(e))
	{
		// save new m_fulltext value and apply the change to the app
	}

	wxCheckBox* m_fulltext;
};

class PrefsPageTopics : public wxPreferencesPage
{
public:
	virtual wxString GetName() const override { return "Topics"; }
	virtual wxBitmapBundle GetIcon() const override
	{
		return wxArtProvider::GetBitmapBundle(wxART_HELP, wxART_TOOLBAR);
	}
	virtual wxWindow* CreateWindow(wxWindow* parent) override
	{
		return new PrefsPageTopicsPanel(parent);
	}
};

App::App()
{
#if wxUSE_MDI_ARCHITECTURE
	m_mode = MDI;
#else
	m_mode = SDI;
#endif
}

App::~App() = default;

bool App::OnInit()
{
	if (!wxApp::OnInit())
	{
		return false;
	}
	auto frame = new MyFrame<PaneFrame<wxMDIParentFrame>>//new FTK::ParentFrameAny<wxMDIParentFrame>
		(
			nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr)
		);
	m_settings.m_dockSettings = GetDockSettings(frame->GetDockManager());
	//if (frame == nullptr)
	//{
		//return false;
	//}

	m_frame = frame;
	return true;
}

int App::OnExit()
{
	return wxApp::OnExit();
}

void App::OnInitCmdLine(wxCmdLineParser& parser)
{
	wxApp::OnInitCmdLine(parser);
#if wxUSE_MDI_ARCHITECTURE
	parser.AddSwitch("", "mdi", "run in MDI mode: multiple documents, single window");
#endif
#if wxUSE_AUI
	parser.AddSwitch("", "aui", "run in MDI mode using AUI: multiple documents, single window");
#endif
	parser.AddSwitch("", "sdi", "run in SDI mode: multiple documents, multiple windows");
	parser.AddSwitch("", "single", "run in single document mode");

	parser.AddParam("filename", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL);
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser)
{
	int numModeOptions = 0;

#if wxUSE_MDI_ARCHITECTURE
	if (parser.Found(MDI))
	{
		m_mode = MDI;
		++numModeOptions;
	}
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	if (parser.Found(AUI))
	{
		m_mode = AUI;
		++numModeOptions;
	}
#endif // wxUSE_AUI

	if (parser.Found(SDI))
	{
		m_mode = SDI;
		++numModeOptions;
	}

	if (parser.Found(SINGLE))
	{
		m_mode = SINGLE;
		++numModeOptions;
	}

	if (numModeOptions > 1)
	{
		wxLogError("Only a single option choosing the mode can be given.");
		return false;
	}

	// save any files given on the command line: we'll open them in OnInit()
	// later, after creating the frame
	//for (size_t i = 0; i != parser.GetParamCount(); ++i)
	//	m_filesFromCmdLine.push_back(parser.GetParam(i));

	return wxApp::OnCmdLineParsed(parser);
}

void App::UpdateSettings(const MySettings& settings)
{
	m_settings = settings;
	ApplyDockSettings(m_frame->GetDockManager(), settings.m_dockSettings);
}

#ifdef __WXMAC__
void App::MacNewFile()
{
	//FTK::DocManager::GetDocumentManager()->CreateNewDocument();
}
#endif

wxFrame* App::CreateMainFrame()
{
#if wxUSE_MDI_ARCHITECTURE
	if (m_mode == MDI)
	{
		auto result = new MyFrame<PaneFrame<wxMDIParentFrame>>//new FTK::ParentFrameAny<wxMDIParentFrame>
			(
				nullptr, wxID_ANY,
				GetAppDisplayName(),
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(1280, 720), nullptr)
			);
		return result;
	}
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	if (m_mode == AUI)
	{
		auto result = new MyFrame<PaneFrame<wxAuiMDIParentFrame>>
			(
				nullptr, wxID_ANY,
				GetAppDisplayName(),
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(1280, 720), nullptr)
			);
		return result;
	}
#endif // wxUSE_AUI

	if (m_mode == SDI || m_mode == SINGLE)
	{
		auto result = new MyFrame<PaneFrame<wxFrame>>//new FTK::ParentFrameAny<wxFrame>
			(
				nullptr, wxID_ANY,
				GetAppDisplayName(),
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(1280, 720), nullptr)
			);
		return result;
	}
	return nullptr;
}

#endif

/*class TreeViewModel
{
public:
	TreeViewModel(std::shared_ptr<model::data::Node> root)
		: m_root(root)
	{}

	NodeVM root() const { return m_root; }

private:
	std::shared_ptr<model::data::Node> m_root;
};*/
#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/window.h>

#ifdef jkhkhkhk
class NativeCheckboxBundleImpl : public wxBitmapBundleImpl
{
public:
	NativeCheckboxBundleImpl(wxWindow* win, int stateFlags)
		: m_win(win), m_stateFlags(stateFlags)
	{
	}

	// Defines the base (100% scale) logical size of your checkbox icon
	wxSize GetDefaultSize() const override
	{
		return wxSize(16, 16);
	}

	wxSize GetPreferredBitmapSizeAtScale(double scale) const override
	{
		return GetDefaultSize() * scale;
	}

	// Called automatically by wxWidgets when the system requests a specific resolution
	wxBitmap GetBitmap(const wxSize& size) override
	{
		// 1. Create a bitmap matched perfectly to the requested physical DPI size
		wxBitmap bmp(size);
		wxMemoryDC dc(bmp);

		// 2. Clear background to match the parent control
		dc.SetBackground(wxBrush(m_win->GetBackgroundColour()));
		dc.Clear();

		// 3. Ask the native OS theme renderer to draw the checkbox at this exact size
		wxRect rect(0, 0, size.x, size.y);
		wxRendererNative::Get().DrawCheckBox(m_win, dc, rect, m_stateFlags);

		dc.SelectObject(wxNullBitmap);
		return bmp;
	}

private:
	wxWindow* m_win;
	int m_stateFlags; // e.g., wxCONTROL_CHECKED or 0 for unchecked
};

// Convenience factory function to instantiate the bundle
wxBitmapBundle CreateNativeCheckboxBundle(wxWindow* win, int stateFlags)
{
	return wxBitmapBundle::FromImpl(new NativeCheckboxBundleImpl(win, stateFlags));
}

/*wxBitmapBundle create_native_checkbox_bundle(win, state_flags) :
	# state_flags example : wx.CONTROL_CHECKED or 0 (for unchecked)
	size = wx.Size(16, 16) # Base logical size

	# Create a bundle from an implementation that scales natively
	bmp = wx.Bitmap(size)
	dc = wx.MemoryDC(bmp)
	dc.SetBackground(wx.Brush(win.GetBackgroundColour()))
	dc.Clear()

	# Ask the native OS renderer to draw the checkbox
	wx.RendererNative.Get().DrawCheckBox(win, dc, wx.Rect(0, 0, 16, 16), state_flags)
	dc.SelectObject(wx.NullBitmap)

	# Convert to a bundle so wxTreeCtrl scales it automatically under high - DPI
	return wx.BitmapBundle(bmp)*/
const std::vector<wxBitmapBundle>& initializeImageList(wxWindow *win)
{
	static std::optional<std::vector<wxBitmapBundle>> images;
	if (images)
		return *images;
	std::vector<wxBitmapBundle> newImages;
	newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("DISABLED")));
	newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF")));
	//newImages.push_back(CreateNativeCheckboxBundle(win, wxCONTROL_CHECKED));// mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF")));
	//newImages.push_back(CreateNativeCheckboxBundle(win , 0));// mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF")));
	//newImages.push_back(CreateNativeCheckboxBundle(win, wxCONTROL_CHECKED));// mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF")));
	newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_ON")));
	newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_OFF")));
	newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_ON")));
	/*const size_t num_of_status = 5;
	wxBitmap state_ico[num_of_status];
	state_ico[0] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("DISABLED"));
	state_ico[1] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF"));
	state_ico[2] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_ON"));
	state_ico[3] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_OFF"));
	state_ico[4] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_ON"));
	int sw = state_ico[0].GetWidth();
	int sh = state_ico[0].GetHeight();

	auto images = std::make_unique<wxImageList>(sw, sh, false, num_of_status);
	for (auto& icon : state_ico)
	{
		images->Add(icon);
	}
	return images;*/
	images = std::move(newImages);
	return *images;
}

class NodePropertiesBuilder : public IPropertiesBuilder
{
public:
	NodePropertiesBuilder(DocumentContext& context, model::data::Node* n): m_context(context), m_node(n){}

	~NodePropertiesBuilder() override = default;

	std::vector<std::unique_ptr<IProperty>> getProperties() const override
	{
		std::vector<std::unique_ptr<IProperty>> result = m_node->getProperties();
		auto pureProperty = 
			makeProperty
			(_R("name"),
				[nn = m_node]() {return mafStringToStd(nn->GetName()); },
				[nn = m_node, context = &m_context](const std::string& name) {nn->SetName(mafStdToString(name));  context->getDocument()->m_nodeChanged.emit({nn}); }
			);
		for (auto& prop : result)
		{
			prop = core::makeOperationProperty(*m_context.getOperationManager(), std::move(prop));
		}
		//result.push_back(std::make_unique<OperationProperty>(*m_context.getOperationManager(), std::move(pureProperty)));
		//m_context.getOperationManager()->Submit(std::make_unique<OpChangeProperty>(std::move(p), name));

		return result;
	}

private:
	DocumentContext& m_context;
	model::data::Node* m_node;
};

template<class BaseFrame>
class FrameWithManagers : public BaseFrame
{
public:
	FrameWithManagers();

	FrameWithManagers(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	~FrameWithManagers() override;

	bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	void OnFileOpen(wxCommandEvent& e);

	std::unique_ptr<DocumentManager> m_documentManager;
	std::unique_ptr<mafInteractionManager> m_interactionManager;
	std::unique_ptr<ViewManager> m_viewManager;

	std::unique_ptr<gui::wx::IView> m_navigator;
	std::shared_ptr<TreeStatusViewModel> m_navigatorModel;
	
	std::unique_ptr<gui::wx::IView> m_nodeProperties;
	std::shared_ptr<gui::IViewModel> m_nodeModel;

	std::unique_ptr<gui::wx::IView> m_pipeProperties;
	std::shared_ptr<gui::IViewModel> m_pipeModel;

	std::unique_ptr<gui::wx::IView> m_outProperties;
	std::shared_ptr<gui::IViewModel> m_outModel;

	std::vector<base::Connection> m_connections;
	base::Connection m_selectedNodeChanged;
	base::Connection m_selectedNodePropertiesChanged;

	std::vector<std::function<std::unique_ptr<core::Operation>(DocumentContext&, std::shared_ptr<model::data::Node>)>> m_operations;
};

template <class BaseFrame>
FrameWithManagers<BaseFrame>::FrameWithManagers() = default;

template <class BaseFrame>
FrameWithManagers<BaseFrame>::FrameWithManagers(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	Create(parent, id, title, pos, size, style, name);
}

template <class BaseFrame>
FrameWithManagers<BaseFrame>::~FrameWithManagers() = default;

template <class BaseFrame>
void FrameWithManagers<BaseFrame>::OnFileOpen(wxCommandEvent& e)
{
	//if (!m_documentManager->open())
		//return;

	{
		auto& context = m_documentManager->get(0);
		int menuId = 10000;
		auto addop = [&menuId, &context, this](const base::String& typeName, const base::String& name, const base::String& nodeName)
			{
				//m_operations.push_back([tn = typeName](DocumentContext& ctx) {return std::make_unique<OperationCreateFactory>(ctx, tn, _R("")); });
				BaseFrame::m_operationsMenu->Append(menuId, wxString("Create " + base::StringToStd(name)));
				BaseFrame::Bind(wxEVT_MENU, [this, &context, &nodeName](wxCommandEvent& event)
					{
						if (auto sel = context.getSelectionController().selected(); sel.size() == 1)
						{
							//context.getOperationManager()->Submit(m_operations[event.GetId() - 10000](context, nodeName));
						}
					}, menuId++);
			};
		//addop(_R("mafVMEGroup"), _R("Group"));
		//addop(_R("mafVMESurfaceParametric"), _R("Surface Parametric"));

		auto addCompatibleOp = [&menuId, &context, this](std::function<std::unique_ptr<mafOp>()> createFN, const base::String& name)
			{
				BaseFrame::m_operationsMenu->Append(menuId, wxString("Create " + base::StringToStd(name)));
				BaseFrame::Bind(wxEVT_MENU, [this, &context, fn = std::move(createFN)](wxCommandEvent& event)
					{
						if (auto sel = context.getSelectionController().selected(); sel.size() == 1)
						{
							auto op = fn();
							op->SetInput(sel[0]->SharedFromThis());
							context.getOperationManager()->Submit(std::move(op));
							m_navigator->setModel(nullptr);
							m_navigatorModel = std::make_shared<TreeStatusViewModel>(m_documentManager->get(0));// , * new StatusController());
							m_navigator->setModel(m_navigatorModel);
						}
					}, menuId++);
			};
		addCompatibleOp([]() {return std::make_unique<mafOpCreateGroup>(_R("Group")); }, _R("Create New Group"));
		addCompatibleOp([]() {return std::make_unique<lhpOpImporterOBJ>(_R("OBJ")); }, _R("Import OBJ"));
		addCompatibleOp([]() {return std::make_unique<lhpOpImporterMetashapeOBJ>(_R("OBJ")); }, _R("Import Metashape OBJ"));
	}

	//auto childFrame = new wxMDIChildFrame(this, wxID_ANY, "");
	//auto renderModel = std::make_unique<VTKViewModel>(m_documentManager->get(0));
	//auto mm = renderModel.get();
	//auto renderView = m_viewManager->createRenderView(std::move(renderModel), childFrame);
	//childFrame->Show();

	/*auto model2 = std::make_unique<PropertyViewModel>();// m_documentManager->get(0), * mm);
	auto tree2 = std::make_unique<gui::wx::PropertyView>(BaseFrame::m_Notebook);
	tree2->attach(std::move(model2));*/


	auto sideSplittedPanel = new wxSplitterWindow(BaseFrame::m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,/*wxSP_3DSASH |*/ wxSP_NOBORDER);
	
	{
		m_navigatorModel = std::make_shared<TreeStatusViewModel>(m_documentManager->get(0));// , * new StatusController());
		auto navigatorView = std::make_unique<gui::wx::TreeView>(sideSplittedPanel);
		navigatorView->setStateImages(initializeImageList(navigatorView->widget()));
		navigatorView->setModel(m_navigatorModel);
		navigatorView->widget()->SetSizer(new wxBoxSizer(wxVERTICAL));
		m_navigator = std::move(navigatorView);
	}
	BaseFrame::m_Notebook->InsertPage(0, sideSplittedPanel, _("data tree"), true);

	auto inspectorNotebook = new wxNotebook(sideSplittedPanel, wxID_ANY);
	inspectorNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
	inspectorNotebook->SetSizer(new wxBoxSizer(wxVERTICAL));
	sideSplittedPanel->SetMinimumPaneSize(BaseFrame::FromDIP(50));
	sideSplittedPanel->SplitHorizontally(m_navigator->widget(), inspectorNotebook);

	PropertyViewModel* pvm = nullptr;
	bool doubleNotebook = true;
	if (doubleNotebook)//style == DOUBLE_NOTEBOOK)
	{
		{
			auto propModel = std::make_shared<PropertyViewModel>();// m_documentManager->get(0), * mm);
			auto propView = std::make_unique<gui::wx::PropertyView>(inspectorNotebook);
			propView->setModel(propModel);
			inspectorNotebook->AddPage(propView->widget(), _("vme output"));
			m_outProperties = std::move(propView);
			m_outModel = std::move(propModel);
		}

		{
			auto propModel = std::make_shared<PropertyViewModel>();// m_documentManager->get(0), * mm);
			auto propView = std::make_unique<gui::wx::PropertyView>(inspectorNotebook);
			propView->setModel(propModel);
			inspectorNotebook->AddPage(propView->widget(), _("visual props"));
			m_pipeProperties = std::move(propView);
			m_pipeModel = std::move(propModel);
		}

		{
			auto propModel = std::make_shared<PropertyViewModel>();// m_documentManager->get(0), * mm);
			pvm = propModel.get();
			auto propView = std::make_unique<gui::wx::OldGuiView>(inspectorNotebook);
			propView->setModel(propModel);
			inspectorNotebook->AddPage(propView->widget(), _("vme"));
			m_nodeProperties = std::move(propView);
			m_nodeModel = std::move(propModel);
		}
	}
	else
	{
		{
			auto propModel = std::make_shared<PropertyViewModel>();// m_documentManager->get(0), * mm);
			auto propView = std::make_unique<gui::wx::PropertyView>(BaseFrame::m_Notebook);
			propView->setModel(propModel);
			BaseFrame::m_Notebook->AddPage(propView->widget(), _("vme"));
			m_nodeProperties = std::move(propView);
			m_nodeModel = std::move(propModel);
		}

		auto m_VmeOutputPanel = new gui::wx::NamedPanel/*mafGUIHolder*/(sideSplittedPanel, wxID_ANY, false, true);
		m_VmeOutputPanel->Show(false);
		sideSplittedPanel->SetMinimumPaneSize(BaseFrame::FromDIP(5));
		sideSplittedPanel->SplitHorizontally(m_navigator->widget(), m_VmeOutputPanel);

		//m_VmePipePanel = new mafGUIHolder(m_Notebook,-1,false,true);
		//m_VmePipePanel->Show(false);
	}
	auto context = &m_documentManager->get(0);
	if (auto sel = context->getSelectionController().selected(); sel.size() == 1)
	{
		auto curNode = sel[0];
		m_selectedNodeChanged = context->getDocument()->connectNodeChanged([pvm](const NodeChanged& e)
			{
				pvm->changed();
			});
		m_selectedNodePropertiesChanged = curNode->connectPropertiesChanged([pvm, context, curNode]()
			{
				pvm->setProperties(NodePropertiesBuilder(*context, curNode).getProperties());
			});
		pvm->setProperties(NodePropertiesBuilder(*context, sel[0]).getProperties());
	}

	m_connections.push_back(context->getSelectionController().connectSelectionChanged([this, pvm, context](model::data::Node*n)
	{
		if (context->getSelectionController().isSelected(n))
		{
			if (auto sel = context->getSelectionController().selected(); sel.size() == 1)
			{
				auto curNode = sel[0];
				m_selectedNodeChanged = context->getDocument()->connectNodeChanged([pvm, context](const NodeChanged& e)
				{
					pvm->changed();
				});
				//m_selectedNodePropertiesChanged = curNode->connectPropertiesChanged([this, pvm, context, curNode]()
					//{
						//BaseFrame::CallAfter([pvm, context, curNode]() {pvm->setProperties(NodePropertiesBuilder(*context, curNode).getProperties());});
					//});
				pvm->setProperties(NodePropertiesBuilder(*context, sel[0]).getProperties());
			}
			else
			{
				pvm->setProperties({});
				m_selectedNodeChanged = base::Connection();
			}
		}
	}));
}

template <class BaseFrame>
bool FrameWithManagers<BaseFrame>::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
	{
		return false;
	}

#ifdef MAF_USE_VTK
	{
		mafSetFrame(this);
		m_interactionManager = std::make_unique<mafInteractionManager>();
		//m_interactionManager->SetListener(this);
		SetGlobalMouse(m_interactionManager->GetMouseDevice());
	}
#endif

	m_viewManager = std::make_unique<ViewManager>();
	int menuId = 11000;
	BaseFrame::m_viewMenu->Append(menuId, wxString("Create " + std::string("VTK View")));
	BaseFrame::Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event)
		{
			event.Enable(m_documentManager->count() >= 1);

		}, menuId);
	BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent& event)
		{
			auto childFrame = new wxMDIChildFrame(this, wxID_ANY, "VTK");
			auto renderModel = std::make_shared<VTKViewModel>(m_documentManager->get(0));
			auto renderView = m_viewManager->create(_R(""), childFrame);
			renderView->setModel(renderModel);
			//renderView->attach(std::move(renderModel));
			childFrame->Bind(wxEVT_ACTIVATE, [this, renderView, renderModel](wxActivateEvent& event)
			{
				if (event.GetActive())
				{
					m_viewManager->setActive(renderView);
					m_navigatorModel->setStatusController(std::make_shared<VisibilityStatusController>(renderModel));
					m_navigatorModel->onActivated = [this, renderModel](model::data::Node* node)
						{
							renderModel->toggleVisibility(node);
						};
				}
			});
			childFrame->Bind(wxEVT_CLOSE_WINDOW, [this, renderView, childFrame](wxCloseEvent& event) {m_viewManager->remove(renderView); childFrame->Destroy(); });

			childFrame->Show();

		}, menuId++);
	m_documentManager = std::make_unique<DocumentManager>();
	/*m_documentManager->setConfirmHandler([this](DocumentContext&)
	{
			int res = wxMessageBox("Save changes?", "Confirm", wxYES_NO | wxCANCEL);
			if (res == wxYES)
			{
				return DocumentManager::CloseDecision::Save;
			}
			if (res == wxNO)
			{
				return DocumentManager::CloseDecision::Discard;
			}
			return DocumentManager::CloseDecision::Cancel;
	});
	m_documentManager->setOpenHandler([this]()
		{
			std::string file(wxLoadFileSelector("", ".msf").c_str());
			if (!file.empty())
				return std::vector<std::string>{file};
			return std::vector<std::string>{};
		});
	m_documentManager->setSaveAsHandler([this]()
		{
			return std::string(wxSaveFileSelector("", ".msf").c_str());
		});*/
	BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent&) {m_documentManager->create(); }, wxID_NEW);
	BaseFrame::Bind(wxEVT_MENU, &FrameWithManagers<BaseFrame>::OnFileOpen, this, wxID_OPEN);
	//BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent&) {m_documentManager->save(0); }, wxID_SAVE);
	//BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent&) {m_documentManager->saveAs(0); }, wxID_SAVEAS);
	BaseFrame::DragAcceptFiles(true);
	BaseFrame::Bind(wxEVT_DROP_FILES, [this](const wxDropFilesEvent& event)
		{
			std::vector<std::string> urls;
			for (int i = 0; i < event.GetNumberOfFiles(); i++)
			{
				urls.emplace_back(event.GetFiles()[i].c_str());
			}
			//m_documentManager->open(urls);
		}
	);

	BaseFrame::Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& e)
	{
		//e.Veto();
		if (m_navigator)
		{
			m_navigator->setModel(nullptr);
		}
		m_viewManager->detach();
		m_navigator.reset();
		m_viewManager.reset();
		while (m_documentManager->count() >0)
		{
			if (!m_documentManager->close(m_documentManager->count() - 1))
			{
				return;
			}
		}
		e.Skip();
	});


	BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent& event)
		{
			if (m_documentManager->count() > 0)
			{
				auto om = m_documentManager->get(0).getOperationManager();
				if (om->CanUndo())
				{
					om->Undo();
					return;
				}
			}
			event.Skip();
		}, wxID_UNDO);
	BaseFrame::Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event)
		{
			if (m_documentManager->count() > 0)
			{
				auto om = m_documentManager->get(0).getOperationManager();
				if (om->CanUndo())
				{
					event.Enable(true);
					return;
				}
			}
			event.Enable(false);
		}, wxID_UNDO);
	BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent& event)
		{
			if (m_documentManager->count() > 0)
			{
				auto om = m_documentManager->get(0).getOperationManager();
				if (om->CanRedo())
				{
					om->Redo();
					return;
				}
			}
			event.Skip();
		}, wxID_REDO);
	BaseFrame::Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event)
		{
			if (m_documentManager->count() > 0)
			{
				auto om = m_documentManager->get(0).getOperationManager();
				if (om->CanRedo())
				{
					event.Enable(true);
					return;
				}
			}
			event.Enable(false);
		}, wxID_REDO);

	return true;
}

struct AppSettings
{};

class OldApp : public wxApp
{
public:
	OldApp();

	OldApp(const OldApp&) = delete;
	OldApp& operator=(const OldApp&) = delete;

	~OldApp() override;

	bool OnInit() override;
	int OnExit() override;

	// show the about box: as we can have different frames it's more
	// convenient, even if somewhat less usual, to handle this in the
	// application object itself
	void OnAbout(wxCommandEvent& event);

#ifdef __WXMAC__
	void MacNewFile() override;
#endif // __WXMAC__

	AppSettings& GetSettings() { return m_settings; }
	void UpdateSettings(const AppSettings& settings);

	void ShowPreferencesEditor(wxWindow* parent);
	void DismissPreferencesEditor();
private:
	std::unique_ptr<wxPreferencesEditor> m_preferencesEditor;
	AppSettings m_settings;
};

OldApp::OldApp()
{
	SetVendorName("ftkApp");
	SetAppName("ftkApp");
	SetAppDisplayName("ftkApp");
	Bind(wxEVT_MENU, &OldApp::OnAbout, this, wxID_ABOUT);
	//Bind(wxEVT_MENU, &App::OnForceCloseAll, this, wxID_CLEAR);
}

OldApp::~OldApp() = default;

bool OldApp::OnInit()
{
	if (!wxApp::OnInit())
	{
		return false;
	}

	int result = model::data::AttributeFactory::Initialize();
	assert(result == MAF_OK);

	result = ItemFactory::Initialize();
	assert(result == MAF_OK);

	result = VMEFactory::Initialize();
	assert(result == MAF_OK);

	result = mafPipeFactoryVME::Initialize();
	assert(result == MAF_OK);




	auto frame = new FrameWithManagers<OldAppFrame<PaneFrame<wxMDIParentFrame>>>//new FTK::ParentFrameAny<wxMDIParentFrame>
		(
			nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr)
		);
	Bind(wxEVT_MENU, [this, frame](wxCommandEvent& event) {ShowPreferencesEditor(frame); }, wxID_PREFERENCES);
	frame->SetIcon(wxICON(APP));
	frame->Show();
#ifdef __WIN32__
	Bind(wxEVT_IDLE,
		[frame](const wxIdleEvent& event)
		{
			MEMORYSTATUS ms;
			GlobalMemoryStatus(&ms);
			wxString s;
			s << _("free memory") << " " << (ms.dwAvailPhys >> 20) << " MB";
			if (frame->GetStatusBar())
			{
				frame->SetStatusText(s, 5);
			}
		}
	);
#endif

	return true;
}

int OldApp::OnExit()
{
	return wxApp::OnExit();
}

void OldApp::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetVersion(FTK_VERSION);
	wxAboutBox(info);
	//wxString modeName = "Mixed mode";

	//const int docsCount = 0;// ftkDocManager::GetDocumentManager()->GetDocuments().size();

	/*wxLogMessage
	(
		"This is the wxWidgets Document/View Sample\n"
		"running in %s mode.\n"
		"%d open documents.\n"
		"\n"
		"Authors: Julian Smart, Vadim Zeitlin\n"
		"\n"
		"Usage: docview [--{mdi,aui,sdi,single}]",
		modeName,
		docsCount
	);*/
}

class PreferencesPageDockPanel : public wxPanel
{
public:
	PreferencesPageDockPanel(wxWindow* parent) : wxPanel(parent)
	{
		//m_settings = wxGetApp().GetSettings().m_dockSettings;
		//mafGUIDockSettings::FillGui(this, m_settings);
	}

	virtual bool TransferDataToWindow() override
	{
		return wxPanel::TransferDataToWindow();
		// This is the place where you can initialize values, e.g. from wxConfig.
		// For demonstration purposes, we just set hardcoded values.
		//m_fulltext->SetValue(true);
		// TODO: handle the listbox
		//Update();
		//return true;
	}

	virtual bool TransferDataFromWindow() override
	{
		// Called on platforms with modal preferences dialog to save and apply
		// the changes.
		//wxCommandEvent dummy;
		//ChangedFulltext(dummy);
		// TODO: handle the listbox
		//wxGetApp().UpdateSettings(m_settingsCurrent);
		return true;
	}

private:
	//void ChangedFulltext(wxCommandEvent& WXUNUSED(e))
	//{
	//	// save new m_fulltext value and apply the change to the app
	//}
	//DockSettings m_settings;
	//wxCheckBox* m_fulltext;
};
class PreferencesPageDock : public wxPreferencesPage
{
public:
	virtual wxString GetName() const override { return _("User Interface Preferences"); }
	wxBitmapBundle GetIcon() const override
	{
		return wxArtProvider::GetBitmapBundle(wxART_HELP, wxART_TOOLBAR);
	}
	wxWindow* CreateWindow(wxWindow* parent) override
	{
		//mafSetFrame(parent);
		return new PreferencesPageDockPanel(parent);
	}
};


void OldApp::ShowPreferencesEditor(wxWindow* parent)
{
	if (!m_preferencesEditor)
	{
		m_preferencesEditor = std::make_unique<wxPreferencesEditor>();
		//m_preferencesEditor->AddPage(new PrefsPageGeneral());
		//m_preferencesEditor->AddPage(new PrefsPageTopics());
		m_preferencesEditor->AddPage(new PreferencesPageDock());
	}
	m_preferencesEditor->Show(parent);
}

void OldApp::DismissPreferencesEditor()
{
	if (m_preferencesEditor)
	{
		m_preferencesEditor->Dismiss();
	}
}
#endif
