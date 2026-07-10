// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/cmdline.h"

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
#include "Gui/mafGUI.h"
#include "Gui/mafGUIDockSettings.h"
#include <memory>

using ftkString = std::string;
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

	bool MainFrameAnyBase::TryProcessEvent(wxEvent& event)
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

			//if (!ftkDocChildFrameAnyBase::Create(view, this))
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

#ifdef __WXMAC__
	void MacNewFile() override;
#endif // __WXMAC__

private:
	wxFrame* CreateMainFrame();
	void CreateMenuBarForFrame(wxFrame* frame, wxMenu* file);

	// force close all windows
	//void OnForceCloseAll(wxCommandEvent& event);

	// show the about box: as we can have different frames it's more
	// convenient, even if somewhat less usual, to handle this in the
	// application object itself
	void OnAbout(wxCommandEvent& event);

#if wxUSE_MDI_ARCHITECTURE
	const char* const MDI = "mdi";
#endif
#if wxUSE_AUI
	const char* const AUI = "aui";
#endif
	const char* const SDI = "sdi";
	const char* const SINGLE = "single";
	enum class Mode
	{
#if wxUSE_MDI_ARCHITECTURE
		Mode_MDI,   // MDI mode: multiple documents, single top level window
#endif
#if wxUSE_AUI
		Mode_AUI,   // MDI AUI mode
#endif
		Mode_SDI,   // SDI mode: multiple documents, multiple top level windows
		Mode_Single // single document mode (and hence single top level window)
	};
	Mode m_mode;
};


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

#endif

template<class BaseFrame>
class MyFrame : public BaseFrame
{
public:
	enum MENU_ID_ENUM
	{
		ID_START = wxID_LAST,
		ID_STATUSBAR = ID_START,
		ID_STANDARD_TOOLBAR,
		ID_ADDITIONAL_TOOLBAR,
		//ID_NOTEBOOK,
		//ID_NAVIGATOR,
		//ID_LOGTEXTCTRL,
		//ID_END,
		//ID_VIEW_START,
		//ID_VIEW_END
	};
	MyFrame();

	MyFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	~MyFrame() override;

	bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

//protected:

	//void OnAbout(wxCommandEvent& e);

	//void OnMenu(wxCommandEvent& e);
	
	//void OnUpdateUI(wxUpdateUIEvent& e);

//private:

	void CreateMenuBar();

	std::unique_ptr<wxMenu> CreateViewMenu();

	wxWindow* CreateStdToolBar();
	
	wxWindow* CreateAddToolBar();
	
	wxWindow* CreateLogWindow();
};

template <class BaseFrame>
MyFrame<BaseFrame>::MyFrame() = default;

template <class BaseFrame>
MyFrame<BaseFrame>::MyFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	Create(parent, id, title, pos, size, style, name);
}

template <class BaseFrame>
MyFrame<BaseFrame>::~MyFrame() = default;

template <class BaseFrame>
bool MyFrame<BaseFrame>::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
	{
		return false;
	}
	BaseFrame::AddPane(CreateStdToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Standard").Name("Standard Toolbar").Top().Floatable(false));
	BaseFrame::AddPane(CreateAddToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Additional").Name("Additional Toolbar").Top().Position(2).Floatable(false));
	BaseFrame::AddPane(CreateLogWindow(), wxAuiPaneInfo().Bottom().Name("Help").Caption(_("Help Window")));
	CreateMenuBar();
	BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent& WXUNUSED(event)) {BaseFrame::Close(); }, wxID_EXIT);
	return true;
}

template <class BaseFrame>
wxWindow* MyFrame<BaseFrame>::CreateStdToolBar()
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
wxWindow* MyFrame<BaseFrame>::CreateAddToolBar()
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

template<class BaseFrame>
wxWindow* MyFrame<BaseFrame>::CreateLogWindow()
{
	return new wxTextCtrl(this, 5153, wxEmptyString, wxDefaultPosition, BaseFrame::FromDIP(wxSize(250, 100)), wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
}

template<class BaseFrame>
std::unique_ptr<wxMenu> MyFrame<BaseFrame>::CreateViewMenu()
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

template <class BaseFrame>
void MyFrame<BaseFrame>::CreateMenuBar()
{
	auto fileMenu = std::make_unique<wxMenu>();
	fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT);

	auto viewMenu = CreateViewMenu();

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
	if (viewMenu)
	{
		menubar->Append(viewMenu.release(), "View");
	}
	menubar->Append(toolsMenu.release(), _("Tools"));
	menubar->Append(helpMenu.release(), wxGetStockLabel(wxID_HELP));
	BaseFrame::SetMenuBar(menubar.release());
}

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

	void ShowPreferencesEditor(wxWindow* parent);
	void DismissPreferencesEditor();

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

	// show the about box: as we can have different frames it's more
	// convenient, even if somewhat less usual, to handle this in the
	// application object itself
	void OnAbout(wxCommandEvent& event);
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
	std::unique_ptr<wxPreferencesEditor> m_prefEditor;
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

class PreferencesPageDockPanel : public mafGUI
{
public:
	PreferencesPageDockPanel() : mafGUI(nullptr)
	{
		m_settings = wxGetApp().GetSettings().m_dockSettings;
		mafGUIDockSettings::FillGui(this, m_settings);
	}

	virtual bool TransferDataToWindow() override
	{
		return mafGUI::TransferDataToWindow();
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
	void ChangedFulltext(wxCommandEvent& WXUNUSED(e))
	{
		// save new m_fulltext value and apply the change to the app
	}
	DockSettings m_settings;
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
		mafSetFrame(parent);
		return new PreferencesPageDockPanel;
	}
};

App::App()
{
	SetVendorName("ftkApp");
	SetAppName("ftkApp");
	SetAppDisplayName("ftkApp");
#if wxUSE_MDI_ARCHITECTURE
	m_mode = MDI;
#else
	m_mode = SDI;
#endif
	Bind(wxEVT_MENU, &App::OnAbout, this, wxID_ABOUT);
	//Bind(wxEVT_MENU, &App::OnForceCloseAll, this, wxID_CLEAR);
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
	frame->DragAcceptFiles(true);

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

	Bind(wxEVT_DROP_FILES,
		[this](const wxDropFilesEvent& event)
		{
			for (int i = 0; i < event.GetNumberOfFiles(); i++)
			{
				//event.GetFiles()[i]
			}
		}
	);
	Bind(wxEVT_MENU, [this, frame](wxCommandEvent& event) {ShowPreferencesEditor(frame); }, wxID_PREFERENCES);

	frame->SetIcon(wxICON(APP));
	frame->Show();
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

void App::ShowPreferencesEditor(wxWindow* parent)
{
	if (!m_prefEditor)
	{
		m_prefEditor = std::make_unique<wxPreferencesEditor>();
		m_prefEditor->AddPage(new PrefsPageGeneral());
		m_prefEditor->AddPage(new PrefsPageTopics());
		m_prefEditor->AddPage(new PreferencesPageDock());
	}
	m_prefEditor->Show(parent);
}

void App::DismissPreferencesEditor()
{
	if (m_prefEditor)
		m_prefEditor->Dismiss();
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

void App::OnAbout(wxCommandEvent& WXUNUSED(event))
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

wxIMPLEMENT_APP(App);
