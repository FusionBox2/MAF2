// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/aui/aui.h>
#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/preferences.h>
#include <wx/splitter.h>
#include "wx/treectrl.h"

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"

#include "Model.h"

#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <map>
#include <sstream>



#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"

namespace ftk
{
	class StringWriter : public DataWriter
	{
	public:
		StringWriter(std::map<std::string, std::string>* storage) : m_storage(storage) {}
		void StartWriteObject(const std::string& name, const Object& obj) override {}
		void WriteProperty(const std::string& name, const std::string& value) override
		{
			(*m_storage)[name] = value;
		}
		void StopWriteObject() override {}


		void visit(const std::string& name, const std::string& value)
		{
			(*m_storage)[name] = value;
		}
		void visit(const std::string& name, int value)
		{
			(*m_storage)[name] = std::to_string(value);
		}

	private:
		std::map<std::string, std::string>* m_storage;
	};

	class PropGridWriter : public DataWriter
	{
	public:
		PropGridWriter(wxPropertyGrid* propertyGrid) : m_propertyGrid(propertyGrid) {}
		void StartWriteObject(const std::string& name, const Object& obj) override {}
		void WriteProperty(const std::string& name, const std::string& value) override
		{
			if (auto prop = m_propertyGrid->GetProperty(name))
			{
				prop->SetValueFromString(value);
			}
			else
			{
				m_propertyGrid->Append(new wxStringProperty(name, wxPG_LABEL, value));
			}
		}
		void StopWriteObject() override {}

	private:
		wxPropertyGrid* m_propertyGrid;
	};

	class PropGridReader : public DataReader
	{
	public:
		PropGridReader(wxPropertyGrid* propertyGrid) : m_propertyGrid(propertyGrid)
		{
		}
		void        StartReadObject(const std::string& name, Object& obj) override {}
		std::string ReadProperty(const std::string& name) override
		{
			if (auto prop = m_propertyGrid->GetProperty(name))
				return prop->GetValueAsString().ToStdString();
			return {};
		}
		void        StopReadObject() override {}

	private:
		wxPropertyGrid* m_propertyGrid;
	};
	class NewNavigator : public wxNotebook
	{
		class TreeItemData : public wxTreeItemData
		{
		public:
			TreeItemData(Node* node) : m_node(node) {}
			~TreeItemData() override
			{
			}
			Node* getNode() const { return m_node; };
		private:
			Node* m_node;
		};
		enum IDs
		{
			PGID = 1,
			ID_INFO_TREE = 5152
		};
	public:
		void OnSize(wxSizeEvent& event)
		{
			wxNotebook::OnSize(event);
			Layout();
		}

		void OnItemStateClick(wxTreeEvent& event)
		{
			// toggle item state
			wxTreeItemId itemId = event.GetItem();
			m_tree->SetItemState(itemId, m_tree->GetItemState(itemId) ^ 0x1);// wxTREE_ITEMSTATE_NEXT);
		}

		void OnSelectionChanged(wxTreeEvent& /*event*/)
		{
			PopulateProperties();
		}

		void PopulateProperties()
		{
			m_properties->Clear();
			auto id = static_cast<TreeItemData*>(m_tree->GetItemData(m_tree->GetSelection()));
			if (auto p = m_properties->GetProperty("name"))
			{
				p->SetValue(id->getNode()->getName());
			}
			else
			{
				//auto& props = id->getNode()->GetProperties();
				//p = new wxStringProperty("name", wxPG_LABEL, id->getNode()->getName());
				//m_properties->Append(p);
				//m_properties->Append(new wxIntProperty("size", wxPG_LABEL));
			}
			PropGridWriter writer(m_properties);
			//id->getNode()->Save(writer);
		}

		void OnPropertyGridChange(wxPropertyGridEvent& event)
		{
			PropGridReader reader(m_properties);
			auto id = static_cast<TreeItemData*>(m_tree->GetItemData(m_tree->GetSelection()));
			//id->getNode()->Load(reader);
			wxPGProperty* p = event.GetProperty();
			if (p)
			{
				auto nm = p->GetName();
				auto vl = p->GetValueAsString();
			}
			else
			{
			}
		}

		const bool DOUBLE_NOTEBOOK = true;
		NewNavigator(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxNotebookNameStr)
			: wxNotebook(parent, id, pos, size, style, name)
		{
			//SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));


			wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER/* | wxSP_3DSASH | wxSP_FULLSASH*//*wxSP_THIN_SASH*/);
			auto treePanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
			auto sizer = new wxBoxSizer(wxVERTICAL);
			treePanel->SetSizerAndFit(sizer);
			m_tree = createTree(treePanel);
			sizer->Add(m_tree, 1, wxEXPAND);
			AddPage(splitter, _("data tree"), true);

			Bind(wxEVT_TREE_STATE_IMAGE_CLICK, &NewNavigator::OnItemStateClick, this, m_tree->GetId(), wxID_ANY);


			m_root = std::make_unique<Node>("root");
			m_root->addChild(std::make_unique<Node>("child1"));
			m_root->getChildren()[0]->addChild(std::make_unique<NodeDerived>("child11", 5));
			m_root->addChild(std::make_unique<Node>("child2"));
			auto& p1 = m_root->GetProperties();
			auto& p2 = m_root->getChildren()[0]->getChildren()[0]->GetProperties();
			std::map<std::string, std::string> strg;
			StringWriter strgWriter(&strg);
			m_root->getChildren()[0]->getChildren()[0]->visitProperties(strgWriter);
			//m_root->Save(strgWriter);

			std::stack<wxTreeItemId> treeIds;
			std::stack<Node*> nodes;
			nodes.push(m_root.get());
			treeIds.push(m_tree->AddRoot(nodes.top()->getName(), 0, 1, new TreeItemData(m_root.get())));
			for (; !treeIds.empty();)
			{
				auto parentNode = nodes.top();
				auto parentId = treeIds.top();
				m_mapping[parentNode] = parentId;
				nodes.pop();
				treeIds.pop();
				for (auto& node : parentNode->getChildren())
				{
					nodes.push(node.get());
					treeIds.push(m_tree->AppendItem(parentId, node->getName(), 0, 1, new TreeItemData(node.get())));
					m_tree->SetItemState(treeIds.top(), 3);
				}
			}
			m_tree->ExpandAll();
			Bind(wxEVT_TREE_SEL_CHANGED, &NewNavigator::OnSelectionChanged, this, m_tree->GetId(), wxID_ANY);

			if (DOUBLE_NOTEBOOK)
			{
				auto nodeNotebook = new wxNotebook(splitter, wxID_ANY);
				//nodeNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
				const char* panelNames[] = { "vme output" , "visual props", "vme" };
				std::vector<wxPanel*> panels;
				for (auto name : panelNames)
				{
					panels.push_back(new wxPanel(nodeNotebook, wxID_ANY));// , false, true));
					nodeNotebook->AddPage(panels.back(), name);
				}
				nodeNotebook->SetSelection(2);

				panels.back()->SetSizerAndFit(new wxBoxSizer(wxVERTICAL));

 				m_properties = new wxPropertyGrid(panels.back(), PGID, wxDefaultPosition, wxDefaultSize, wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED);
				Bind(wxEVT_PG_CHANGED, &NewNavigator::OnPropertyGridChange, this, m_properties->GetId(), wxID_ANY);
				
				if (auto idd = m_tree->GetSelection())
				{
					auto id = static_cast<TreeItemData*>(m_tree->GetItemData(idd));
					auto p = new wxStringProperty("String Property", wxPG_LABEL, id->getNode()->getName());
					//p->O
					m_properties->Append(p);
					m_properties->Append(new wxIntProperty("Int Property", wxPG_LABEL));
					//pg->Append(new wxBoolProperty("Bool Property", wxPG_LABEL));
				}
				panels.back()->GetSizer()->Add(m_properties, 1, wxEXPAND);
				splitter->SetMinimumPaneSize(50);
				splitter->SplitHorizontally(treePanel, nodeNotebook);
			}
			else
			{
				auto panel = new wxPanel(this, wxID_ANY);// , false, true);
				AddPage(panel, _("vme"));

				auto panel2 = new wxPanel(splitter, wxID_ANY);// , false, true);
				panel2->Show(true);
				splitter->SetMinimumPaneSize(50);
				splitter->SplitHorizontally(treePanel, panel2);
			}

		}

		~NewNavigator() override
		{
		}
	protected:
		wxTreeCtrl* createTree(wxWindow *parent)
		{
			if(auto tree = new wxTreeCtrl(parent, ID_INFO_TREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_SINGLE))
			{
#include <pic/NODE_YELLOW.xpm>
#include <pic/NODE_RED.xpm>
#include <pic/NODE_BLUE.xpm>
#include <pic/NODE_GRAY.xpm>

#include <pic/DISABLED.xpm>
#include <pic/RADIO_ON.xpm>
#include <pic/RADIO_OFF.xpm>
#include <pic/CHECK_ON.xpm>
#include <pic/CHECK_OFF.xpm>
#define mafADDPIC(X) images.push_back(wxBitmap(X##_xpm));
				{
					//				wxBitmap state_ico[num_of_status];
					// 				state_ico[NODE_NON_VISIBLE] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("DISABLED"));
					// 				state_ico[NODE_VISIBLE_OFF] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF"));
					// 				state_ico[NODE_VISIBLE_ON] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_ON"));
					// 				state_ico[NODE_MUTEX_OFF] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_OFF"));
					// 				state_ico[NODE_MUTEX_ON] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_ON"));
					wxBitmap state_ico = wxBitmap(DISABLED_xpm);
					int sw = state_ico.GetWidth();
					int sh = state_ico.GetHeight();
					wxBitmap bmp = wxBitmap(NODE_YELLOW_xpm);
					int w = bmp.GetWidth();
					int h = bmp.GetHeight();
					int mw = sw;// +w;
					int mh = (sh > h) ? sh : h;

					{
						std::vector<wxBitmapBundle> images;
						mafADDPIC(NODE_YELLOW);
						mafADDPIC(NODE_RED);
						mafADDPIC(NODE_BLUE);
						mafADDPIC(NODE_GRAY);
						tree->SetImages(images);
					}

					{
						std::vector<wxBitmapBundle> images;
						//mafADDPIC(DISABLED);
						mafADDPIC(RADIO_ON);
						mafADDPIC(RADIO_OFF);
						mafADDPIC(CHECK_ON);
						mafADDPIC(CHECK_OFF);
						tree->SetStateImages(images);
					}
				}
				return tree;
			}
			return nullptr;
		}
	private:
		std::unordered_map<Node*, wxTreeItemId> m_mapping;
		wxPropertyGrid* m_properties;
		wxTreeCtrl* m_tree;
		std::unique_ptr<Node> m_root;
		DECLARE_EVENT_TABLE()
	};
	BEGIN_EVENT_TABLE(NewNavigator, wxNotebook)
		EVT_SIZE(NewNavigator::OnSize)
	END_EVENT_TABLE()

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

	class ChildFrame : public BaseChildFrame
	{
		static const int PH = 13; //panel height
	public:
		ChildFrame(BaseFrame* parent,
					wxWindowID id,
					const wxString& title,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxDEFAULT_FRAME_STYLE,
					const wxString& name = wxASCII_STR(wxFrameNameStr))
			: BaseChildFrame(parent, id, title, pos, size, style, name)
		{
			m_AllowCloseFlag = true;
			m_EnableResize = true;
			//m_Win = m_View->GetWindow();
			//m_Win->Reparent(this);
			//m_Win->Show(true);
			//SetIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("MDICHILD_ICON")));
			//SetTitle(wxStripMenuCodes(m_View->GetLabel().toWx()));
		}

		~ChildFrame() override
		{
		}

		void SetAllowCloseWindow(bool allow_close)
		{
			m_AllowCloseFlag = allow_close;
		}

		void EnableResize(bool enable = true)
		{
			m_EnableResize = enable;
		}
	protected:
		void OnSize(wxSizeEvent& event)
		{
			if (!m_EnableResize)
			{
				return;
			}

			int w, h;
			//don't initialize w & h using the event - use GetClientSize instead
			this->GetClientSize(&w, &h);

			//			if (h < PH || w < PH) return;

				//		m_Win->Move(0, 0);
					//	m_Win->SetSize(w, h);
						//m_Win->Layout();
#ifndef WIN32
			//if (m_View)
			{
				//m_View->SetWindowSize(w, h);
			}
#endif
			//m_View->CameraUpdate();
		}

		void OnSelect(wxCommandEvent& event)
		{
			Activate(); // allow activation with the RMouse too
		}

		void OnCloseWindow(wxCloseEvent& event)
		{
			if (!m_AllowCloseFlag) { return; }

			// VIEW_DELETE must be sent from here and not from the destructor
			  // otherwise VIEW_DELETE is sent also on the closing of the application
			  // when the listener (the ViewManager) has been already destroyed

			//mafEventMacro(mafEvent(this, VIEW_DELETE, m_View));
			Destroy();
			//m_View = NULL;
		}


		void OnActivate(wxActivateEvent& event)
		{
			//			if (event.GetActive() && m_View)
				//		{
					//		mafEventMacro(mafEvent(this, VIEW_SELECT, m_View, (wxWindow*)NULL));
						//	Layout();
						//}
		}

		void OnMaximize(wxMaximizeEvent& event)
		{
			//			if (m_View)
				//		{
					//		mafString msg = _R("MaximizeSelectedView");
						//	mafEventMacro(mafEvent(this, VIEW_MAXIMIZE, &msg));
						//}
		}

		bool         m_AllowCloseFlag;
		bool         m_EnableResize;

		DECLARE_EVENT_TABLE()
	};
	BEGIN_EVENT_TABLE(ChildFrame, BaseChildFrame)
		EVT_CLOSE(ChildFrame::OnCloseWindow)
		EVT_SIZE(ChildFrame::OnSize)
		//EVT_BUTTON(VIEW_CLICKED, NewMDIChildFrame::OnSelect)
		EVT_ACTIVATE(ChildFrame::OnActivate)
		EVT_MAXIMIZE(ChildFrame::OnMaximize)
	END_EVENT_TABLE()

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

		MainFrame() {}

		MainFrame(wxWindow* parent,
					wxWindowID id,
					const wxString& title,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = DefaultFrameStyle,
					const wxString& name = wxASCII_STR(wxFrameNameStr))
			: BaseFrame()
		{
			Create(parent, id, title, pos, size, style, name);
		}
		virtual bool Layout() override
		{
			auto res = BaseFrame::Layout();
			return res;
		}


		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& title,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = DefaultFrameStyle,
					const wxString& name = wxASCII_STR(wxFrameNameStr))
		{
			if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
				return false;
			CreateControls();
			//Bind(wxEVT_MENU, &NewFrame::OnPreferences, this, wxID_PREFERENCES);

			return true;
		}
	protected:
		void OnAbout(wxCommandEvent& e)
		{
			wxAboutDialogInfo info;
			info.SetName("FTK Demo App");
			info.SetCopyright("(C) 2024 FRAMOSOFT");
			info.SetVersion("v1.0");
			//info.SetWebSite("framosoft.com");
			info.AddDeveloper("Fedor Moiseev");
			wxAboutBox(info);
		}

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

		void OnMenu(wxCommandEvent& e)
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
				auto c = new ChildFrame(this, wxID_ANY, "");
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

		void OnUpdateUI(wxUpdateUIEvent& e)
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
		
	private:
		void CreateControls()
		{
			CreateMenuBar();
			CreateStatus();
			m_auiManager.SetManagedWindow(this);
			//ribbon = CreateRibbon();
			//AddPane(ribbon, wxAuiPaneInfo().ToolbarPane().Caption("Ribbon").Name("Ribbon").Top().Floatable(false));

			AddPane(CreateStdToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Standard").Name("Standard Toolbar").Top().Floatable(false));
			AddPane(CreateAddToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Additional").Name("Additional Toolbar").Top().Position(2).Floatable(false));
			AddPane(CreateNavigator(), wxAuiPaneInfo().Right().Layer(1)/*.PinButton().MinimizeButton().MaximizeButton()*/.Name("Navigator").Caption("Navigator").MinSize(BaseFrame::FromDIP(wxSize(240, 450))));
			AddPane(CreateLogWindow(), wxAuiPaneInfo().Bottom().Name("Help").Caption(_("Help Window")));

			m_auiManager.Update();
		}

		void CreateMenuBar()
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

		void CreateStatus()
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
				m_Gauge = new wxGauge(m_frameStatusBar, -1, 100, pr.GetPosition(), pr.GetSize(), wxGA_SMOOTH | wxGA_PROGRESS);
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
				);

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

		wxWindow* CreateStdToolBar()
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

		wxWindow* CreateAddToolBar()
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

		wxWindow* CreateLogWindow()
		{
			return new wxTextCtrl(this, 5153, wxEmptyString, wxDefaultPosition, BaseFrame::FromDIP(wxSize(250, 100)), wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
		}

		bool AddPane(wxWindow* window, wxAuiPaneInfo& pane_info)
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

		wxWindow* RemovePane(const wxString& pane_name)
		{
			wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
			if (pi.IsOk())
			{
				return RemovePane(pi);
			}
			return nullptr;
		}

		wxWindow* RemovePane(wxWindow* window)
		{
			wxAuiPaneInfo& pi = m_auiManager.GetPane(window);
			if (pi.IsOk())
			{
				return RemovePane(pi);
			}
			return nullptr;
		}

		wxWindow* RemovePane(wxAuiPaneInfo& pane_info)
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

		wxAuiManager m_auiManager;
		wxRibbonBar* ribbon = nullptr;
		wxGauge *m_Gauge = nullptr;
		std::unique_ptr<wxPreferencesEditor> m_preferencesEditor;
		wxDECLARE_EVENT_TABLE();
	};

	BEGIN_EVENT_TABLE(MainFrame, BaseFrame)
		EVT_SIZE(MainFrame::OnSize)
		EVT_CLOSE(MainFrame::OnCloseWindow)
		EVT_DROP_FILES(MainFrame::OnDropFile)
		EVT_IDLE(MainFrame::OnIdle)
 		EVT_MENU(wxID_NEW, MainFrame::OnMenu)
 		EVT_MENU(wxID_OPEN, MainFrame::OnMenu)
		EVT_MENU(wxID_SAVE, MainFrame::OnMenu)
		EVT_MENU(wxID_EXIT, MainFrame::OnMenu)
		EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
		EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MainFrame::OnMenu)
 		EVT_MENU_RANGE(ID_START, ID_END, MainFrame::OnMenu)
		EVT_UPDATE_UI_RANGE(ID_START, ID_END, MainFrame::OnUpdateUI)
		EVT_MENU_RANGE(ID_VIEW_START, ID_VIEW_END, MainFrame::OnMenu)
		EVT_MENU(wxID_PREFERENCES, MainFrame::OnPreferences)
// 		EVT_MENU(ID_LOAD_LAYOUT, MainFrame::OnLoadLayout)
// 		EVT_MENU(ID_SAVE_LAYOUT, MainFrame::OnSaveLayout)
	END_EVENT_TABLE()
}
namespace ftk
{
    wxFrame* CreateFrameNW(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name)
    {
        wxSize sz = (size == wxDefaultSize) ? wxWindow::FromDIP(wxSize(800, 600), nullptr) : size;
        auto frame = new MainFrame(parent, id, "FTK Demo App", pos, sz, style, name);
        return frame;
    }
}
