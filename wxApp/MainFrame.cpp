#include "ftkConfigure.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/aboutdlg.h>
#include <wx/aui/aui.h>
#include <wx/artprov.h>

#include <ftk/Gui/ViewFrame.h>

BEGIN_FTK_NAMESPACE

//using BaseFrame = wxFrame;
using BaseChildFrame = wxMDIChildFrame;
//using BaseChildFrame = wxAuiMDIChildFrame;
//using NewFrame = NewFrameT<wxFrame, wxDEFAULT_FRAME_STYLE>;
//using NewFrame = NewFrameT<wxMDIParentFrame, wxDEFAULT_FRAME_STYLE>;
//using NewFrame = NewFrameT<wxAuiMDIParentFrame, wxDEFAULT_FRAME_STYLE>;
//template<class BaseFrame, long DefaultStyle>
const long DefaultFrameStyle = wxDEFAULT_FRAME_STYLE;
//using BaseFrame = wxFrame;
using BaseFrame = wxMDIParentFrame;
//using BaseFrame = wxAuiMDIParentFrame;
//using NewFrame = NewFrameT<wxFrame, wxDEFAULT_FRAME_STYLE>;
//using NewFrame = NewFrameT<wxMDIParentFrame, wxDEFAULT_FRAME_STYLE>;
//using NewFrame = NewFrameT<wxAuiMDIParentFrame, wxDEFAULT_FRAME_STYLE>;
//template<class BaseFrame, long DefaultStyle>

class MainFrame : public BaseFrame
{
public:
	enum MENU_ID_ENUM
	{
		ID_START = wxID_LAST,
		ID_STANDARD_TOOLBAR = ID_START,
		ID_ADDITIONAL_TOOLBAR,
		ID_NOTEBOOK,
		ID_NAVIGATOR,
		ID_STATUSBAR,
		ID_LOGTEXTCTRL,
		ID_END,
		ID_VIEW_START,
		ID_VIEW_END
	};

	MainFrame();
	MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,	long style = DefaultFrameStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));
	bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultFrameStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

protected:

	void OnAbout(wxCommandEvent& e);
	void OnMenu(wxCommandEvent& e);
	void OnUpdateUI(wxUpdateUIEvent& e);

private:

	void CreateControls();
	void CreateMenuBar();
	void CreateStatus();
	wxWindow* CreateStdToolBar();
	wxWindow* CreateAddToolBar();
	wxWindow* CreateLogWindow();

	bool AddPane(wxWindow* window, wxAuiPaneInfo& pane_info);
	wxWindow* RemovePane(const wxString& pane_name);
	wxWindow* RemovePane(wxWindow* window);
	wxWindow* RemovePane(wxAuiPaneInfo& pane_info);

	wxAuiManager m_auiManager;
};

MainFrame::MainFrame() = default;

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: BaseFrame()
{
	Create(parent, id, title, pos, size, style, name);
}

bool MainFrame::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
		return false;
	CreateControls();

	Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
	
	Bind(wxEVT_MENU, &MainFrame::OnMenu, this, wxID_NEW);
	Bind(wxEVT_MENU, &MainFrame::OnMenu, this, wxID_OPEN);
	Bind(wxEVT_MENU, &MainFrame::OnMenu, this, wxID_SAVE);
	Bind(wxEVT_MENU, &MainFrame::OnMenu, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MainFrame::OnMenu, this, wxID_FILE1, wxID_FILE9);

	Bind(wxEVT_MENU, &MainFrame::OnMenu, this, ID_START, ID_END);
	Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateUI, this, ID_START, ID_END);

	Bind(wxEVT_MENU, &MainFrame::OnMenu, this, ID_VIEW_START, ID_VIEW_END);

	return true;
}

void MainFrame::CreateControls()
{
	CreateMenuBar();
	CreateStatus();
	m_auiManager.SetManagedWindow(this);
	//ribbon = CreateRibbon();
	//AddPane(ribbon, wxAuiPaneInfo().ToolbarPane().Caption("Ribbon").Name("Ribbon").Top().Floatable(false));

	AddPane(CreateStdToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Standard").Name("Standard Toolbar").Top().Floatable(false));
	AddPane(CreateAddToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Additional").Name("Additional Toolbar").Top().Position(2).Floatable(false));
	//AddPane(CreateNavigator(), wxAuiPaneInfo().Right().Layer(1)/*.PinButton().MinimizeButton().MaximizeButton()*/.Name("Navigator").Caption("Navigator").MinSize(BaseFrame::FromDIP(wxSize(240, 450))));
	AddPane(CreateLogWindow(), wxAuiPaneInfo().Bottom().Name("Help").Caption(_("Help Window")));

	m_auiManager.Update();
}

void MainFrame::CreateMenuBar()
{
	auto menuBar = new wxMenuBar;

	auto fileMenu = new wxMenu;
	fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT);

	auto layoutMenu = new wxMenu;
	//layoutMenu->Append(ID_LOAD_LAYOUT, _("Load"));
	//layoutMenu->Append(ID_SAVE_LAYOUT, _("Save"));

	auto viewMenu = new wxMenu;
	viewMenu->Append(ID_VIEW_START, _("Add view"));
	viewMenu->Append(ID_VIEW_END, _("Add external view"));
	wxMenuItem* layoutItem = viewMenu->Append(wxID_ANY, _("Layout"), layoutMenu);

	auto opMenu = new wxMenu;

	auto toolsMenu = new wxMenu;
	toolsMenu->Append(wxID_PREFERENCES);

	auto helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT);

	menuBar->Append(fileMenu, wxGetStockLabel(wxID_FILE));
	menuBar->Append(viewMenu, _("View"));
	menuBar->Append(opMenu, _("Operation"));
	menuBar->Append(toolsMenu, _("Tools"));
	menuBar->Append(helpMenu, wxGetStockLabel(wxID_HELP));
	BaseFrame::SetMenuBar(menuBar);
}

void MainFrame::CreateStatus()
{
	if (auto statusBar = BaseFrame::CreateStatusBar(1, wxSTB_DEFAULT_STYLE, ID_STATUSBAR))
	{
		int widths[] = { -1,60,60,60,210,120 };
		std::transform(std::begin(widths), std::end(widths), std::begin(widths), [this](int w) {return BaseFrame::FromDIP(w); });
		statusBar->SetFieldsCount(6);
		statusBar->SetStatusWidths(6, widths);
		statusBar->SetStatusText(_("welcome"), 0);
		statusBar->SetStatusText(" ", 1);
		statusBar->SetStatusText(" ", 2);
		statusBar->SetStatusText(" ", 3);

		wxRect pr;
		statusBar->GetFieldRect(4, pr);
		/*m_Gauge = new wxGauge(m_frameStatusBar, -1, 100, pr.GetPosition(), pr.GetSize(), wxGA_SMOOTH | wxGA_PROGRESS);
		m_Gauge->SetForegroundColour(*wxRED);
		m_Gauge->SetValue(50);
		m_Gauge->Show(false);
		statusBar->Bind(wxEVT_SIZE,
			[this](const wxSizeEvent& event)
			{
				wxRect r;
				GetStatusBar()->GetFieldRect(4, r);
				m_Gauge->SetPosition(r.GetPosition());
			}
		);*/

		wxMenuBar* menubar = GetMenuBar();
		if (!menubar)
			return;
		int idx = menubar->FindMenu("View");
		if (idx == wxNOT_FOUND)
			return;
		wxMenu* viewMenu = menubar->GetMenu(idx);
		int ids = viewMenu->FindItem("Layout");
		if (ids != wxNOT_FOUND)
		{
			size_t pos = 0;
			wxMenuItem* sm = viewMenu->FindChildItem(ids, &pos);
			viewMenu->Insert(pos, statusBar->GetId(), "Status bar", "", wxITEM_CHECK);
		}
		else
		{
			viewMenu->Append(statusBar->GetId(), "Status bar", "", wxITEM_CHECK);
		}
	}
}

wxWindow* MainFrame::CreateStdToolBar()
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

wxWindow* MainFrame::CreateAddToolBar()
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

wxWindow* MainFrame::CreateLogWindow()
{
	return new wxTextCtrl(this, 5153, wxEmptyString, wxDefaultPosition, BaseFrame::FromDIP(wxSize(250, 100)), wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
}


bool MainFrame::AddPane(wxWindow* window, wxAuiPaneInfo& pane_info)
{
	if (!m_auiManager.AddPane(window, pane_info))
	{
		return false;
	}

	if (pane_info.caption.IsEmpty())
	{
		return true;
	}

	wxMenuBar* menubar = GetMenuBar();
	if (!menubar)
		return true;
	int idx = menubar->FindMenu("View");
	if (idx == wxNOT_FOUND)
		return true;
	wxMenu* viewMenu = menubar->GetMenu(idx);
	if (pane_info.IsToolbar())
	{
		wxMenu* toolbarsMenu = nullptr;
		int ids = viewMenu->FindItem("Toolbars");
		if (ids == wxNOT_FOUND)
		{
			toolbarsMenu = new wxMenu;
			wxMenuItem* toolbarsItem = viewMenu->Insert(0, wxID_ANY, "Toolbars", toolbarsMenu);
		}
		else
		{
			wxMenuItem* sm = viewMenu->FindItem(ids);
			toolbarsMenu = sm->GetSubMenu();
		}
		if (toolbarsMenu)
		{
			toolbarsMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
		}
	}
	else
	{
		int ids = viewMenu->FindItem("Layout");
		if (ids != wxNOT_FOUND)
		{
			size_t pos = 0;
			wxMenuItem* sm = viewMenu->FindChildItem(ids, &pos);
			viewMenu->Insert(pos, window->GetId(), pane_info.caption, "", wxITEM_CHECK);
		}
		else
		{
			viewMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
		}
	}
	return true;
}

wxWindow* MainFrame::RemovePane(const wxString& pane_name)
{
	wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
	if (pi.IsOk())
	{
		return RemovePane(pi);
	}
	return nullptr;
}

wxWindow* MainFrame::RemovePane(wxWindow* window)
{
	wxAuiPaneInfo& pi = m_auiManager.GetPane(window);
	if (pi.IsOk())
	{
		return RemovePane(pi);
	}
	return nullptr;
}

wxWindow* MainFrame::RemovePane(wxAuiPaneInfo& pane_info)
{
	wxMenuBar* menubar = GetMenuBar();
	int idx = menubar->FindMenu("View");
	if (idx != wxNOT_FOUND)
	{
		wxMenu* viewMenu = menubar->GetMenu(idx);
		if (pane_info.IsToolbar())
		{
			int ids = viewMenu->FindItem("Toolbars");
			wxMenuItem* sm = viewMenu->FindItem(ids);
			wxMenu* toolbarsMenu = sm->GetSubMenu();
			delete toolbarsMenu->Remove(pane_info.window->GetId());
			if (toolbarsMenu->GetMenuItemCount() == 0)
			{
				delete viewMenu->Remove(sm);
			}
		}
		else
		{
			delete viewMenu->Remove(pane_info.window->GetId());
		}
	}
	wxWindow* w = pane_info.window;
	if (!m_auiManager.DetachPane(pane_info.window))
		return nullptr;
	return w;
}

void MainFrame::OnAbout(wxCommandEvent& e)
{
	wxAboutDialogInfo info;
	info.SetName("FTK Demo App");
	info.SetCopyright("(C) 2025 FRAMOSOFT");
	info.SetVersion("v1.0");
	//info.SetWebSite("framosoft.com");
	info.AddDeveloper("Fedor Moiseev");
	wxAboutBox(info);
}

void MainFrame::OnMenu(wxCommandEvent& e)
{
	if (e.GetId() == wxID_EXIT)
	{
		Close();
		return;
	}
	else if (e.GetId() == ID_STATUSBAR)
	{
		if (wxStatusBar* statusBar = GetStatusBar())
		{
			statusBar->Show(!statusBar->IsShown());
			//SetStatusBar(nullptr);
			//statusBar->Destroy();
		}
		else
		{
			CreateStatus();
		}
		m_auiManager.Update();
		return;
	}
	else if (ID_VIEW_START <= e.GetId() && e.GetId() < ID_VIEW_END)
	{
		auto c = new wxMDIChildFrame(this, wxID_ANY, "");
		//c->SetWindowStyleFlag(m_ChildFrameStyle);
		//c->SetListener(m_ViewManager);
		c->Show();
		return;
	}
	else if (e.GetId() == ID_VIEW_END)
	{
		auto c = new wxFrame(this, wxID_ANY, "");
		//c->SetWindowStyleFlag(m_ChildFrameStyle);
		//c->SetListener(m_ViewManager);
		c->Show();
		return;
	}
	else
	{
		wxAuiPaneInfoArray& all_panes = m_auiManager.GetAllPanes();
		size_t count = all_panes.GetCount();
		for (size_t i = 0; i < count; i++)
		{
			if (e.GetId() == all_panes[i].window->GetId())
			{
				all_panes[i].Show(!all_panes[i].IsShown());
				m_auiManager.Update();
				return;
			}
		}
	}
}

void MainFrame::OnUpdateUI(wxUpdateUIEvent& e)
{
	wxAuiPaneInfoArray& all_panes = m_auiManager.GetAllPanes();
	size_t count = all_panes.GetCount();
	for (size_t i = 0; i < count; i++)
	{
		if (e.GetId() == all_panes[i].window->GetId())
		{
			e.Check(all_panes[i].IsShown());
		}
	}
	if (e.GetId() == ID_STATUSBAR)
	{
		e.Check(GetStatusBar() != nullptr && GetStatusBar()->IsShown());
	}
}
#if 0
class MainFrame : public BaseFrame
{
public:

protected:

	void OnPreferences(wxCommandEvent& e)
	{
		if (!m_preferencesEditor)
		{
			m_preferencesEditor = std::make_unique<wxPreferencesEditor>();
		}
		m_preferencesEditor->Show(this);
	}

	void OnCloseWindow(wxCloseEvent& e)
	{
		if (m_preferencesEditor)
		{
			m_preferencesEditor->Dismiss();
		}
		e.Skip();
	}

	void OnDropFile(wxDropFilesEvent& e)
	{
	}

	void OnIdle(wxIdleEvent& event)
	{
	}

	void OnLayout(wxCommandEvent& e)
	{
	}

	void OnSize(wxSizeEvent& e)
	{
	}

private:


	wxRibbonBar* CreateRibbon()
	{
		auto m_ribbon = new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRIBBON_BAR_FLOW_HORIZONTAL
			| wxRIBBON_BAR_SHOW_PAGE_LABELS
			| wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS
			| wxRIBBON_BAR_SHOW_TOGGLE_BUTTON
			//| wxRIBBON_BAR_SHOW_HELP_BUTTON
		);
		wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, "Examples");// , ribbon_xpm);
		wxRibbonPanel* toolbar_panel = new wxRibbonPanel(home, wxID_ANY, "Toolbar",
			wxNullBitmap, wxDefaultPosition, wxDefaultSize,
			wxRIBBON_PANEL_NO_AUTO_MINIMISE |
			wxRIBBON_PANEL_EXT_BUTTON);
		wxRibbonToolBar* toolbar = new wxRibbonToolBar(toolbar_panel, -1);// ID_MAIN_TOOLBAR);
		m_ribbon->Realize();
		return m_ribbon;
	}

	//In this code, the ribbon is a member of MainFrame called ribbon.
//Rendered bitmap are also members of MainFrame, and are called ribbonFileNormalBitmap and ribbonFileHoveredBitmap
//Finally, the wxStaticBitmap is called ribbonFileBt.
	void RealizeRibbonCustomButtons()
	{
		wxRibbonArtProvider* artProvider = ribbon->GetArtProvider();
		if (artProvider == NULL) return;

		wxColor buttonColor;
		//if (!wxConfigBase::Get()->Read(_T("/Skin/FileButtonColor"), &buttonColor))
		buttonColor = wxColour(200, 200, 255);

		//Create a temporary fake ribbon used to render the button with a custom color
		wxRibbonBar* fakeRibbon = new wxRibbonBar(this);
		fakeRibbon->SetArtProvider(artProvider->Clone());
		fakeRibbon->GetArtProvider()->SetColourScheme(buttonColor, buttonColor, buttonColor);

		//The device context used to render the button in memory
		wxMemoryDC dc;

		//Compute width of the bitmap button
		int width; artProvider->GetBarTabWidth(dc, fakeRibbon, _("File"), wxNullBitmap, &width, NULL, NULL, NULL);

		//Create a fake ribbon page...
		wxRibbonPage* page = new wxRibbonPage(fakeRibbon, wxID_ANY, _("File"));
		//...and the associated wxRibbonPageTabInfo
		wxRibbonPageTabInfo tabInfo;
		tabInfo.rect = wxRect(0, 0, width, 16 /*Will be changed later*/);
		tabInfo.ideal_width = width;
		tabInfo.small_begin_need_separator_width = width;
		tabInfo.small_must_have_separator_width = width;
		tabInfo.minimum_width = width;
		tabInfo.page = page;
		tabInfo.active = true;
		tabInfo.hovered = false;
		wxRibbonPageTabInfoArray pages;
		pages.Add(tabInfo);
		pages.Add(tabInfo); //Add page twice to ensure that tab have a correct height

		//Compute height of the bitmap button and create bitmap
		int height = artProvider->GetTabCtrlHeight(dc, ribbon, pages);
		wxBitmap bitmapLabel(width + 2, height);
		dc.SelectObject(bitmapLabel);

		tabInfo.rect = wxRect(0, 0, width, height + 2); //We've got the correct height now.

		//Render the file button. Use the background of the real ribbon.
		artProvider->DrawTabCtrlBackground(dc, fakeRibbon, bitmapLabel.GetSize());
		fakeRibbon->GetArtProvider()->DrawTab(dc, fakeRibbon, tabInfo);
		/*ribbonFileNormalBitmap = wxBitmap(bitmapLabel);

		//Render the hovered file button
		wxBitmap bitmapHoveredLabel(ribbonFileNormalBitmap.ConvertToImage());
		dc.SelectObject(bitmapHoveredLabel);

		tabInfo.active = false;
		tabInfo.hovered = true;
		artProvider->DrawTabCtrlBackground(dc, fakeRibbon, bitmapHoveredLabel.GetSize());
		wxColour backgroundColour = wxColor(bitmapHoveredLabel.ConvertToImage().GetRed(0, 0), bitmapHoveredLabel.ConvertToImage().GetGreen(0, 0), bitmapHoveredLabel.ConvertToImage().GetBlue(0, 0)); //For later use...
		fakeRibbon->GetArtProvider()->DrawTab(dc, fakeRibbon, tabInfo);
		ribbonFileHoveredBitmap = bitmapHoveredLabel;

		//Cut a bit the bottom of the bitmaps
		if (ribbonFileNormalBitmap.GetSize().GetHeight() > 3)
			ribbonFileNormalBitmap.SetHeight(ribbonFileNormalBitmap.GetSize().GetHeight() - 2);

		if (ribbonFileHoveredBitmap.GetSize().GetHeight() > 3)
			ribbonFileHoveredBitmap.SetHeight(ribbonFileHoveredBitmap.GetSize().GetHeight() - 2);

		fakeRibbon->Destroy();

		//Finally create our bitmaps and make sure the ribbon is ready.
		ribbonFileBt->SetPosition(wxPoint(3, 1));
		ribbonFileBt->SetBitmap(ribbonFileNormalBitmap);
		ribbon->SetTabCtrlMargins(bitmapLabel.GetSize().GetWidth() + 3 + 3, 0);*/
	}
	wxWindow* CreateNavigator()
	{
		return new NewNavigator(this, ID_NAVIGATOR, wxDefaultPosition, BaseFrame::FromDIP(wxSize(200, 500)));
	}


	wxAuiManager m_auiManager;
	wxRibbonBar* ribbon = nullptr;
	wxGauge* m_Gauge = nullptr;
	std::unique_ptr<wxPreferencesEditor> m_preferencesEditor;
	wxDECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(MainFrame, BaseFrame)
EVT_SIZE(MainFrame::OnSize)
EVT_CLOSE(MainFrame::OnCloseWindow)
EVT_DROP_FILES(MainFrame::OnDropFile)
EVT_IDLE(MainFrame::OnIdle)
EVT_MENU(wxID_PREFERENCES, MainFrame::OnPreferences)
// 		EVT_MENU(ID_LOAD_LAYOUT, MainFrame::OnLoadLayout)
// 		EVT_MENU(ID_SAVE_LAYOUT, MainFrame::OnSaveLayout)
END_EVENT_TABLE()
#endif

END_FTK_NAMESPACE

namespace ftk
{
	wxFrame* CreateFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	{
		wxSize sz = (size == wxDefaultSize) ? wxWindow::FromDIP(wxSize(800, 600), nullptr) : size;
		auto frame = new MainFrame(parent, id, "FTK Demo App", pos, sz, style, name);
		return frame;
	}
}
