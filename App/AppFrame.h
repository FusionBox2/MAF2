#pragma once

#include "ftkConfigure.h"

#include "Gui/mafGUINamedPanel.h"

#include "ftk/Core/IProperty.h"
#include "ftk/Core/Node.h"
#include "ftk/Core/OperationProperty.h"
#include "ftk/Core/OperationManager.h"
#include "ftk/Gui/IViewModel.h"
#include "ftk/Gui/wx/IView.h"
#include "ftk/Gui/wx/TreeView.h"
#include "ftk/Gui/wx/OldGuiView.h"
#include "ftk/Gui/wx/PaneFrame.h"

#include "DocumentManager.h"
#include "DocumentContext.h"
#include "ISelectionController.h"
#include "ViewManager.h"
#include "OperationsRegistry.h"
#include "PropertyViewModel.h"
#include "TreeStatusViewModel.h"
#include "VisibilityStatusController.h"
#include "VTKViewModel.h"
#include "VTKView.h"

#include <wx/splitter.h>
#if wxUSE_AUI
#include "wx/aui/tabmdi.h"
#endif // wxUSE_AUI

#include <wx/aboutdlg.h>
#include <wx/artprov.h>

#include "wx/tokenzr.h"

#include <memory>

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	using PropertyView = OldGuiView;
}

const std::vector<wxBitmapBundle>& initializeImageList(wxWindow* win);

class NodePropertiesBuilder
{
public:
	NodePropertiesBuilder(DocumentContext& context, model::data::Node* n) : m_context(context), m_node(n) {}

	std::vector<std::unique_ptr<IProperty>> getProperties() const
	{
		std::vector<std::unique_ptr<IProperty>> result = m_node->getProperties();
		auto pureProperty =
			makeProperty
			(_R("name"),
				[nn = m_node]() {return mafStringToStd(nn->GetName()); },
				[nn = m_node, context = &m_context](const std::string& name) {nn->SetName(mafStdToString(name));  context->getDocument()->m_nodeChanged.emit({ nn }); }
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
class AppFrame : public BaseFrame
{
public:
	AppFrame();

	AppFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	~AppFrame() override;

	bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

private:
	void AddToMenu(wxMenu* menu, const wxString& menuPath, int id);

	size_t GetCurrentDocumentContext() const;

	void CreateLayout();

	virtual void ConfigureOperations() = 0;

	wxWindow* CreateStdToolBar();

	wxWindow* CreateAddToolBar();

	wxTextCtrl* CreateLogWindow();

	wxWindow* CreateControlBar();

	void RebuildMenu();

	std::unique_ptr<wxMenu> CreateViewMenu();

	void OnMenu(wxCommandEvent& event);

	void OnUpdateUI(wxUpdateUIEvent& event);

	void OnFileNew(wxCommandEvent& event);

	void OnFileOpen(wxCommandEvent& event);

	void OnFileSave(wxCommandEvent& event);

	void OnFileSaveAs(wxCommandEvent& event);

	bool FileNew();

	bool FileOpen();

	bool FileSave();

	bool FileSaveAs();

	bool FileClose();

	void OnFileClose(wxCommandEvent& event);

	void OnFilesDrop(const wxDropFilesEvent& event);

	void OnSelectionChanged(model::data::Node* node);

	void BindActiveView();

	void BindSelectedNodeActiveView();

	void OnOperationApply(wxCommandEvent& event);

	void OnOperationCancel(wxCommandEvent& event);
	
	int GetNewId();

	const int m_statusBarId = 10000;
	int m_nextId = m_statusBarId + 1;

	wxNotebook* m_sideNotebook = nullptr;

	std::unique_ptr<gui::wx::TreeView> m_navigator;
	std::shared_ptr<TreeStatusViewModel> m_navigatorModel;

	std::unique_ptr<gui::wx::IView> m_nodeProperties;
	std::shared_ptr<PropertyViewModel> m_nodeModel;
	base::Connection m_selectedNodeChanged;

	std::unique_ptr<gui::wx::IView> m_pipeProperties;
	std::shared_ptr<PropertyViewModel> m_pipeModel;
	base::Connection m_currentVisualChanged;

	std::unique_ptr<gui::wx::IView> m_outProperties;
	std::shared_ptr<PropertyViewModel> m_outModel;

	std::unique_ptr<gui::wx::IView> m_viewProperties;
	std::shared_ptr<PropertyViewModel> m_viewModel;
	base::Connection m_activeViewChanged;
	base::Connection m_activeViewModelChanged;

	std::unique_ptr<gui::wx::IView> m_operationProperties;
	std::shared_ptr<PropertyViewModel> m_operationModel;
	std::unique_ptr<core::Operation> m_currentOperation;
	base::Connection m_currentOperationChanged;
	wxPanel* m_operationEditorPanel = nullptr;
	wxButton* m_applyButton = nullptr;

	std::unique_ptr<DocumentManager> m_documentManager;
	std::unique_ptr<ViewManager> m_viewManager;
protected:
	std::unique_ptr<OperationsRegistry> m_operationsRegistry;

private:
	std::vector<base::Connection> m_connections;
	std::unordered_map<int, base::String> m_viewMenu;
	std::unordered_map<int, base::String> m_operationsMenu;

	std::unique_ptr<mafInteractionManager> m_interactionManager;
};

template <class BaseFrame>
AppFrame<BaseFrame>::AppFrame() = default;

template <class BaseFrame>
AppFrame<BaseFrame>::AppFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	Create(parent, id, title, pos, size, style, name);
}

template <class BaseFrame>
AppFrame<BaseFrame>::~AppFrame() = default;

template <class BaseFrame>
bool AppFrame<BaseFrame>::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
	{
		return false;
	}

	ConfigureOperations();
	m_viewManager = std::make_unique<ViewManager>();
	m_documentManager = std::make_unique<DocumentManager>();
#ifdef MAF_USE_VTK
	{
		mafSetFrame(this);
		m_interactionManager = std::make_unique<mafInteractionManager>();
		//m_interactionManager->SetListener(this);
		SetGlobalMouse(m_interactionManager->GetMouseDevice());
	}
#endif

	m_viewManager->registerFactory(_R("Surface"),
		[](wxWindow* parent) {return std::make_unique<VTKView>(parent); },
		[](DocumentContext& context)
		{
			auto model = std::make_shared<VTKViewModel>(context);
			model->plugVisualPipe(_R("mafVMESurface"), _R("mafPipeSurface"));
			model->plugVisualPipe(_R("mafVMELandmark"), _R("medPipeTrajectories"));
			return model;
		});

	m_viewManager->registerFactory(_R("Textured Surface"),
		[](wxWindow* parent) {return std::make_unique<VTKView>(parent); },
		[](DocumentContext& context)
		{
			auto model = std::make_shared<VTKViewModel>(context);
			model->plugVisualPipe(_R("mafVMELandmark"), _R("medPipeTrajectories"));
			model->plugVisualPipe(_R("mafVMESurface"), _R("mafPipeSurfaceTextured"));
			return model;
		});
	m_connections.push_back(m_viewManager->connectActiveViewChanged([this]() {BindActiveView(); BindSelectedNodeActiveView(); }));

	CreateLayout();
	RebuildMenu();

	BaseFrame::Bind(wxEVT_MENU, &AppFrame<BaseFrame>::OnMenu, this);
	BaseFrame::Bind(wxEVT_UPDATE_UI, &AppFrame<BaseFrame>::OnUpdateUI, this);
	BaseFrame::Bind(wxEVT_MENU, [this](wxCommandEvent& WXUNUSED(event)) {BaseFrame::Close(); }, wxID_EXIT);
	BaseFrame::Bind(wxEVT_MENU, &AppFrame<BaseFrame>::OnFileNew, this, wxID_NEW);
	BaseFrame::Bind(wxEVT_MENU, &AppFrame<BaseFrame>::OnFileOpen, this, wxID_OPEN);
	BaseFrame::Bind(wxEVT_MENU, &AppFrame<BaseFrame>::OnFileSave, this, wxID_SAVE);
	BaseFrame::Bind(wxEVT_MENU, &AppFrame<BaseFrame>::OnFileSaveAs, this, wxID_SAVEAS);
	BaseFrame::Bind(wxEVT_MENU, &AppFrame<BaseFrame>::OnFileClose, this, wxID_CLOSE);
	BaseFrame::Bind(wxEVT_DROP_FILES, &AppFrame<BaseFrame>::OnFilesDrop, this);
	return true;
}

template <class BaseFrame>
void AppFrame<BaseFrame>::AddToMenu(wxMenu* menu, const wxString& menuPath, int id)
{
	if (menuPath.empty())
	{
		return;
	}

	for (wxStringTokenizer tokenizer(menuPath, "/");;)
	{
		wxString name = tokenizer.GetNextToken();
		if (!tokenizer.HasMoreTokens())
		{
			if (int item = menu->FindItem(name); item != wxNOT_FOUND)
			{
				if (wxMenuItem* menuItem = menu->FindItem(item))
				{
					menu = menuItem->GetSubMenu();
				}
			}
			menu->Append(id, name, name);
			return;
		}
		if (int item = menu->FindItem(name); item != wxNOT_FOUND)
		{
			if (wxMenuItem* menuItem = menu->FindItem(item))
			{
				menu = menuItem->GetSubMenu();
			}
		}
		else
		{
			auto subMenu = new wxMenu;
			auto b = menu->Append(wxID_ANY, name, subMenu);
			menu = subMenu;
		}
	}
}

template <class BaseFrame>
size_t AppFrame<BaseFrame>::GetCurrentDocumentContext() const
{
	if (m_documentManager->count() > 0)
	{
		return 0;
	}
	return DocumentManager::npos;
}

template <class BaseFrame>
void AppFrame<BaseFrame>::CreateLayout()
{
	BaseFrame::CreateStatusBar();
	BaseFrame::AddPane(CreateStdToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Standard").Name("Standard Toolbar").Top().Floatable(false));
	BaseFrame::AddPane(CreateAddToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Additional").Name("Additional Toolbar").Top().Position(2).Floatable(false));
	auto log = CreateLogWindow();
	//auto m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_log));
	//wxLogMessage("This is the log window");
	BaseFrame::AddPane(log, wxAuiPaneInfo().Bottom().Name("Help").Caption(_("Help Window")));
	BaseFrame::AddPane(CreateControlBar(), wxAuiPaneInfo().Name("ControlBar").Caption(_("ControlBar")).Right().Layer(2).MinSize(BaseFrame::FromDIP(wxSize(240, 450))).TopDockable(false).BottomDockable(false));
}

template <class BaseFrame>
wxWindow* AppFrame<BaseFrame>::CreateStdToolBar()
{
	auto toolBar = new wxAuiToolBar(this, GetNewId());
	toolBar->AddTool(wxID_NEW, wxGetStockLabel(wxID_NEW, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_NEW), wxGetStockLabel(wxID_NEW, wxSTOCK_WITH_ACCELERATOR));//, wxGetStockHelpString(wxID_NEW)
	toolBar->AddTool(wxID_OPEN, wxGetStockLabel(wxID_OPEN, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN), wxGetStockLabel(wxID_OPEN, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddTool(wxID_SAVE, wxGetStockLabel(wxID_SAVE, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_FILE_SAVE), wxGetStockLabel(wxID_SAVE, wxSTOCK_WITH_ACCELERATOR));
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_ABOUT, wxGetStockLabel(wxID_ABOUT, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_INFORMATION), wxGetStockLabel(wxID_ABOUT, wxSTOCK_WITH_ACCELERATOR));
	toolBar->Realize();
	return toolBar;
}

template <class BaseFrame>
wxWindow* AppFrame<BaseFrame>::CreateAddToolBar()
{
	auto toolBar = new wxAuiToolBar(this, GetNewId());
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
wxTextCtrl* AppFrame<BaseFrame>::CreateLogWindow()
{
	return new wxTextCtrl(this, GetNewId(), wxEmptyString, wxDefaultPosition, BaseFrame::FromDIP(wxSize(250, 100)), wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
}

template <class BaseFrame>
wxWindow* AppFrame<BaseFrame>::CreateControlBar()
{
	auto notebook = new wxNotebook(this, GetNewId());
	notebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

	auto sideSplittedPanel = new wxSplitterWindow(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,/*wxSP_3DSASH |*/ wxSP_NOBORDER);

	{
		auto navigatorView = std::make_unique<gui::wx::TreeView>(sideSplittedPanel);
		navigatorView->widget()->SetSizer(new wxBoxSizer(wxVERTICAL));
		m_navigator = std::move(navigatorView);
	}
	notebook->AddPage(sideSplittedPanel, _("data tree"), true);

	auto inspectorNotebook = new wxNotebook(sideSplittedPanel, wxID_ANY);
	inspectorNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
	inspectorNotebook->SetSizer(new wxBoxSizer(wxVERTICAL));
	sideSplittedPanel->SetMinimumPaneSize(BaseFrame::FromDIP(50));
	sideSplittedPanel->SplitHorizontally(m_navigator->widget(), inspectorNotebook);

	PropertyViewModel* pvm = nullptr;
	const bool doubleNotebook = true;
	if (doubleNotebook)
	{
		{
			auto propModel = std::make_shared<PropertyViewModel>();
			auto propView = std::make_unique<gui::wx::PropertyView>(inspectorNotebook);
			propView->setModel(propModel);
			inspectorNotebook->AddPage(propView->widget(), _("output"));
			m_outProperties = std::move(propView);
			m_outModel = std::move(propModel);
		}

		{
			auto propModel = std::make_shared<PropertyViewModel>();
			auto propView = std::make_unique<gui::wx::PropertyView>(inspectorNotebook);
			propView->setModel(propModel);
			inspectorNotebook->AddPage(propView->widget(), _("visual props"));
			m_pipeProperties = std::move(propView);
			m_pipeModel = std::move(propModel);
		}

		{
			auto propModel = std::make_shared<PropertyViewModel>();
			pvm = propModel.get();
			auto propView = std::make_unique<gui::wx::PropertyView>(inspectorNotebook);
			propView->setModel(propModel);
			inspectorNotebook->AddPage(propView->widget(), _("node"));
			m_nodeProperties = std::move(propView);
			m_nodeModel = std::move(propModel);
		}
	}
	else
	{
		{
			auto propModel = std::make_shared<PropertyViewModel>();// m_documentManager->get(0), * mm);
			auto propView = std::make_unique<gui::wx::PropertyView>(notebook);
			propView->setModel(propModel);
			notebook->AddPage(propView->widget(), _("node"));
			m_nodeProperties = std::move(propView);
			m_nodeModel = std::move(propModel);
		}

		auto m_VmeOutputPanel = new gui::wx::NamedPanel/*mafGUIHolder*/(sideSplittedPanel, wxID_ANY, false, true);
		m_VmeOutputPanel->Show(false);
		sideSplittedPanel->SetMinimumPaneSize(BaseFrame::FromDIP(5));
		sideSplittedPanel->SplitHorizontally(m_navigator->widget(), m_VmeOutputPanel);
	}

	{
		auto propModel = std::make_shared<PropertyViewModel>();
		auto propView = std::make_unique<gui::wx::PropertyView>(notebook);
		propView->setModel(propModel);
		notebook->AddPage(propView->widget(), _("view settings"));
		m_viewProperties = std::move(propView);
		m_viewModel = std::move(propModel);
	}

	{
		auto operationPage = new wxPanel(notebook);
		auto operationSizer = new wxBoxSizer(wxVERTICAL);

		auto editorPanel = new wxPanel(operationPage);
		auto editorSizer = new wxBoxSizer(wxVERTICAL);

		auto propModel = std::make_shared<PropertyViewModel>();
		auto propView = std::make_unique<gui::wx::PropertyView>(editorPanel);
		propView->setModel(propModel);

		editorSizer->Add(propView->widget(),1,wxEXPAND);

		auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		auto applyButton = new wxButton(editorPanel, wxID_APPLY, "Apply");
		auto cancelButton = new wxButton(editorPanel, wxID_CANCEL, "Cancel");

		applyButton->Bind(wxEVT_BUTTON, &AppFrame<BaseFrame>::OnOperationApply, this);
		cancelButton->Bind(wxEVT_BUTTON, &AppFrame<BaseFrame>::OnOperationCancel, this);

		buttonSizer->AddStretchSpacer();
		buttonSizer->Add(applyButton, 0, wxALL, 2);
		buttonSizer->Add(cancelButton, 0, wxALL, 2);

		editorSizer->Add(buttonSizer, 0, wxEXPAND);
		editorPanel->SetSizer(editorSizer);

		operationSizer->Add(editorPanel, 1, wxEXPAND);
		operationPage->SetSizer(operationSizer);
		notebook->AddPage(operationPage, _("operation"));

		m_operationProperties = std::move(propView);
		m_operationModel = std::move(propModel);

		editorPanel->Show(false);
		m_operationEditorPanel = editorPanel;
		m_applyButton = applyButton;
	}
	m_sideNotebook = notebook;
	return notebook;
}

template <class BaseFrame>
void AppFrame<BaseFrame>::RebuildMenu()
{
	auto editMenu = std::make_unique<wxMenu>();
	editMenu->Append(wxID_UNDO);
	editMenu->Append(wxID_REDO);
	editMenu->AppendSeparator();
	editMenu->Append(wxID_FIND);

	std::unique_ptr<wxMenu> importMenu;
	std::unique_ptr<wxMenu> exportMenu;
	std::unique_ptr<wxMenu> operationsMenu;

	m_operationsMenu.clear();
	if (m_operationsRegistry)
	{
		for (auto& name : m_operationsRegistry->operations())
		{
			wxMenu* current = nullptr;
			if (m_operationsRegistry->type(name) == OperationsRegistry::OperationType::Normal)
			{
				if (!operationsMenu)
				{
					operationsMenu = std::make_unique<wxMenu>();
				}
				current = operationsMenu.get();
			}

			else if (m_operationsRegistry->type(name) == OperationsRegistry::OperationType::Importer)
			{
				if (!importMenu)
				{
					importMenu = std::make_unique<wxMenu>();
				}
				current = importMenu.get();
			}

			else if (m_operationsRegistry->type(name) == OperationsRegistry::OperationType::Exporter)
			{
				if (!exportMenu)
				{
					exportMenu = std::make_unique<wxMenu>();
				}
				current = exportMenu.get();
			}

			else if (m_operationsRegistry->type(name) == OperationsRegistry::OperationType::Edit)
			{
				current = editMenu.get();
			}

			if (current)
			{
				int opId = GetNewId();
				AddToMenu(current, base::StringToStd(name), opId);
				m_operationsMenu.emplace(opId, name);
			}
		}
	}

	auto fileMenu = std::make_unique<wxMenu>();
	fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	fileMenu->AppendSeparator();
	if (importMenu || exportMenu)
	{
		if (importMenu)
		{
			fileMenu->Append(wxID_ANY, "Import", importMenu.release());
		}
		if (exportMenu)
		{
			fileMenu->Append(wxID_ANY, "Export", exportMenu.release());
		}
		fileMenu->AppendSeparator();
	}
	fileMenu->Append(wxID_EXIT);

	auto viewMenu = CreateViewMenu();

	auto toolsMenu = std::make_unique<wxMenu>();
	toolsMenu->Append(wxID_PREFERENCES);

	auto helpMenu = std::make_unique<wxMenu>();
	helpMenu->Append(wxID_ABOUT);

	auto menubar = std::make_unique<wxMenuBar>();
	menubar->Append(fileMenu.release(), wxGetStockLabel(wxID_FILE));
	menubar->Append(editMenu.release(), wxGetStockLabel(wxID_EDIT));

	if (viewMenu)
	{
		menubar->Append(viewMenu.release(), "View");
	}

	if (operationsMenu)
	{
		menubar->Append(operationsMenu.release(), "Operations");
	}
	menubar->Append(toolsMenu.release(), _("Tools"));
	menubar->Append(helpMenu.release(), wxGetStockLabel(wxID_HELP));
	BaseFrame::SetMenuBar(menubar.release());
}

template<class BaseFrame>
std::unique_ptr<wxMenu> AppFrame<BaseFrame>::CreateViewMenu()
{
	std::unique_ptr<wxMenu> viewMenu = std::make_unique<wxMenu>();
	std::unique_ptr<wxMenu> toolbarsMenu;
	std::unique_ptr<wxMenu> addViewMenu;
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
	}
	if (auto statusbar = BaseFrame::GetStatusBar())
	{
		viewMenu->Append(m_statusBarId, "Status bar", "", wxITEM_CHECK);
	}
	if (toolbarsMenu)
	{
		viewMenu->Insert(0, wxID_ANY, "Toolbars", toolbarsMenu.release());
	}

	m_viewMenu.clear();
	for (auto& name : m_viewManager->factories())
	{
		if (!addViewMenu)
		{
			addViewMenu = std::make_unique<wxMenu>();
		}
		int viewId = GetNewId();
		addViewMenu->Append(viewId, base::StringToStd(name));
		m_viewMenu.emplace(viewId, name);
	}
	if (addViewMenu)
	{
		viewMenu->Append(wxID_ANY, "Add View", addViewMenu.release());
	}
	return viewMenu;
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnMenu(wxCommandEvent& event)
{
	if (event.GetId() == m_statusBarId)
	{
		BaseFrame::ShowStatus(!BaseFrame::IsStatusShown());
		return;
	}

	for (auto& pi : BaseFrame::GetPanes())
	{
		if (event.GetId() == pi.window->GetId())
		{
			BaseFrame::ShowPane(pi, !BaseFrame::IsPaneShown(pi));
			return;
		}
	}

	if (event.GetId() == wxID_UNDO)
	{
		auto& context = m_documentManager->get(GetCurrentDocumentContext());
		auto om = context.getOperationManager();
		if (om->canUndo())
		{
			om->undo();
			return;
		}
		return;
	}

	if (event.GetId() == wxID_REDO)
	{
		auto& context = m_documentManager->get(GetCurrentDocumentContext());
		auto om = context.getOperationManager();
		if (om->canRedo())
		{
			om->redo();
		}
		return;
	}

	if (auto viewIt = m_viewMenu.find(event.GetId()); viewIt != m_viewMenu.end())
	{
		auto name = base::StdToString(std::string(event.GetString().c_str()));
		auto childFrame = new wxMDIChildFrame(this, wxID_ANY, base::StringToStd(viewIt->second));
		auto view = m_viewManager->create(viewIt->second, childFrame);
		childFrame->Bind(wxEVT_ACTIVATE, [
			this, view](wxActivateEvent& event)
			{
				if (event.GetActive())
				{
					m_viewManager->setActive(view);
				}
				else if (m_viewManager->getActive() == view)
				{
					m_viewManager->setActive(nullptr);
				}
			});
			childFrame->Bind(wxEVT_CLOSE_WINDOW, [](wxCloseEvent& event) {});
			childFrame->Bind(wxEVT_CLOSE_WINDOW, [this, view, childFrame](wxCloseEvent& event) {m_viewManager->remove(view); childFrame->Destroy(); });
			childFrame->Show();
			return;
	}

	if (auto operationIt = m_operationsMenu.find(event.GetId()); operationIt != m_operationsMenu.end())
	{
		if (GetCurrentDocumentContext() == DocumentManager::npos)
		{
			return;
		}

		auto& context = m_documentManager->get(GetCurrentDocumentContext());
		m_currentOperation = m_operationsRegistry->createOperation(operationIt->second, context);
		if (auto props = m_currentOperation->getProperties(); props.empty())
		{
			context.getOperationManager()->submit(std::move(m_currentOperation));
		}
		else
		{
			m_operationModel->setProperties(std::move(props));
			m_operationEditorPanel->Show(true);
			m_operationEditorPanel->GetParent()->Layout();
			m_sideNotebook->SetSelection(2);
			m_applyButton->Enable(m_currentOperation->isConfigured());
			m_currentOperationChanged = m_currentOperation->connectValuesChanged([this]()
				{
					if (m_applyButton)
					{
						m_applyButton->Enable(m_currentOperation && m_currentOperation->isConfigured());
					}
				});
		}
		return;
	}

	event.Skip();
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnUpdateUI(wxUpdateUIEvent& event)
{
	if (event.GetId() == m_statusBarId)
	{
		event.Check(BaseFrame::IsStatusShown());
		return;
	}

	for (auto& pi : BaseFrame::GetPanes())
	{
		if (event.GetId() == pi.window->GetId())
		{
			event.Check(BaseFrame::IsPaneShown(pi));
			return;
		}
	}

	if (event.GetId() == wxID_UNDO)
	{
		if (GetCurrentDocumentContext() == DocumentManager::npos)
		{
			event.Enable(false);
			return;
		}
		auto& context = m_documentManager->get(GetCurrentDocumentContext());
		auto om = context.getOperationManager();
		event.Enable(om->canUndo());
		return;
	}

	if (event.GetId() == wxID_REDO)
	{
		if (GetCurrentDocumentContext() == DocumentManager::npos)
		{
			event.Enable(false);
			return;
		}
		auto& context = m_documentManager->get(GetCurrentDocumentContext());
		auto om = context.getOperationManager();
		event.Enable(om->canRedo());
		return;
	}

	if (auto operationIt = m_operationsMenu.find(event.GetId()); operationIt != m_operationsMenu.end())
	{
		if (GetCurrentDocumentContext() == DocumentManager::npos)
		{
			event.Enable(false);
			return;
		}

		auto& context = m_documentManager->get(GetCurrentDocumentContext());
		event.Enable(m_operationsRegistry->canExecute(operationIt->second, context));
		return;
	}

	event.Skip();
}

template <class BaseFrame>
bool AppFrame<BaseFrame>::FileNew()
{
	m_documentManager->create();

	auto& context = m_documentManager->get(GetCurrentDocumentContext());
	m_navigatorModel = std::make_shared<TreeStatusViewModel>(context);
	m_navigator->setStateImages(initializeImageList(m_navigator->widget()));
	m_navigator->setModel(m_navigatorModel);
	m_viewManager->attach(context);
	BindActiveView();
	BindSelectedNodeActiveView();

	m_connections.push_back(context.getSelectionController().connectSelectionChanged([this](model::data::Node* node) {OnSelectionChanged(node); BindSelectedNodeActiveView(); }));

	return true;
}

template <class BaseFrame>
bool AppFrame<BaseFrame>::FileOpen()
{
	core::FilePath url(base::StdToString(std::string(wxLoadFileSelector("", ".msf").c_str())));
	if (url.empty())
	{
		return false;
	}
	if (!m_documentManager->open(url))
	{
		return false;
	}
	auto& context = m_documentManager->get(GetCurrentDocumentContext());
	m_navigatorModel = std::make_shared<TreeStatusViewModel>(context);
	m_navigator->setStateImages(initializeImageList(m_navigator->widget()));
	m_navigator->setModel(m_navigatorModel);
	m_viewManager->attach(context);
	BindActiveView();
	BindSelectedNodeActiveView();

	m_connections.push_back(context.getSelectionController().connectSelectionChanged([this](model::data::Node* node) {OnSelectionChanged(node); BindSelectedNodeActiveView(); }));

	return true;
}

template <class BaseFrame>
bool AppFrame<BaseFrame>::FileSave()
{
	auto& context = m_documentManager->get(GetCurrentDocumentContext());
	if (auto& url = context.getURL(); !url.empty())
	{
		return context.save(url);
	}
	return FileSaveAs();
}

template <class BaseFrame>
bool AppFrame<BaseFrame>::FileSaveAs()
{
	core::FilePath url(base::StdToString(std::string(wxSaveFileSelector("", ".msf").c_str())));
	if (url.empty())
	{
		return false;
	}
	auto& context = m_documentManager->get(GetCurrentDocumentContext());
	return context.save(url);
}

template <class BaseFrame>
bool AppFrame<BaseFrame>::FileClose()
{
	if (GetCurrentDocumentContext() == DocumentManager::npos)
	{
		return false;
	}

	auto& context = m_documentManager->get(GetCurrentDocumentContext());
	if (context.getOperationManager()->isDirty())
	{
		int res = wxMessageBox("Save changes?", "Confirm", wxYES_NO | wxCANCEL);
		if (res == wxCANCEL || (res == wxYES && !FileSave()))
		{
			return false;
		}
	}
	return m_documentManager->close(GetCurrentDocumentContext());
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnFileNew(wxCommandEvent& event)
{
	FileNew();
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnFileOpen(wxCommandEvent& event)
{
	FileOpen();
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnFileSave(wxCommandEvent& event)
{
	FileSave();
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnFileSaveAs(wxCommandEvent& event)
{
	FileSaveAs();
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnFileClose(wxCommandEvent& event)
{
	FileClose();
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnFilesDrop(const wxDropFilesEvent& event)
{
	std::vector<core::FilePath> urls;
	for (int i = 0; i < event.GetNumberOfFiles(); i++)
	{
		core::FilePath url(base::StdToString(std::string(event.GetFiles()[i].c_str())));
		urls.emplace_back(url);
	}
	m_documentManager->open(urls);
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnSelectionChanged(model::data::Node* node)
{
	auto& context = m_documentManager->get(GetCurrentDocumentContext());
	auto& selectionController = context.getSelectionController();

	if (selectionController.isSelected(node))
	{
		if (auto sel = selectionController.selected(); sel.size() == 1)
		{
			m_selectedNodeChanged = node->connectValuesChanged([nodeModel = m_nodeModel]() {nodeModel->changed(); });
			m_nodeModel->setProperties(NodePropertiesBuilder(context, sel[0]).getProperties());
		}
		else
		{
			m_nodeModel->setProperties({});
			m_selectedNodeChanged = {};
		}
	}
}

template <class BaseFrame>
void AppFrame<BaseFrame>::BindActiveView()
{
	if (auto activeView = m_viewManager->getActive())
	{
		auto viewProps = activeView->getProperties();

		if (!viewProps.empty())
		{
			m_activeViewChanged = activeView->connectValuesChanged([this]() {m_viewModel->changed(); });
		}
		else
		{
			m_activeViewChanged = {};
		}

		if (auto model = activeView->getModel())
		{
			auto modelProps = model->getProperties();
			for (auto& prop : modelProps)
			{
				viewProps.push_back(std::move(prop));
			}
			m_activeViewModelChanged = model->connectValuesChanged([this]() {m_viewModel->changed(); });
		}
		else
		{
			m_activeViewModelChanged = {};
		}
		m_viewModel->setProperties(std::move(viewProps));
	}
	else
	{
		m_viewModel->setProperties({});
		m_activeViewChanged = {};
		m_activeViewModelChanged = {};
	}

	if (!m_navigatorModel)
	{
		return;
	}

	if (auto activeView = m_viewManager->getActive(); activeView && std::dynamic_pointer_cast<VTKViewModel>(activeView->getModel()))
	{
		auto vtkModel = std::dynamic_pointer_cast<VTKViewModel>(activeView->getModel());
		m_navigatorModel->setStatusController(std::make_shared<VisibilityStatusController>(vtkModel));
		m_navigatorModel->onActivated = [this, vtkModel](model::data::Node* node)
			{
				vtkModel->toggleVisibility(node);
				BindSelectedNodeActiveView();
			};
	}
	else
	{
		m_navigatorModel->onActivated = {};
		m_navigatorModel->setStatusController(nullptr);
	}
}

template <class BaseFrame>
void AppFrame<BaseFrame>::BindSelectedNodeActiveView()
{
	auto view = m_viewManager->getActive();
	if (!view)
	{
		return;
	}
	if (GetCurrentDocumentContext() == DocumentManager::npos)
	{
		m_pipeModel->setProperties({});
		m_currentVisualChanged = {};
		return;
	}
	auto viewModel = std::dynamic_pointer_cast<VTKViewModel>(view->getModel());
	auto& context = m_documentManager->get(GetCurrentDocumentContext());
	auto& selectionController = context.getSelectionController();
	auto selection = selectionController.selected();
	if (viewModel && selection.size() == 1)
	{
		m_pipeModel->setProperties(viewModel->getVisualProperties(selection[0]));
		m_currentVisualChanged = viewModel->connectVisualPropertiesChanged(selection[0], [this]() {m_pipeModel->changed(); });
	}
	else
	{
		m_pipeModel->setProperties({});
		m_currentVisualChanged = {};
	}
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnOperationApply(wxCommandEvent& event)
{
	if (!m_currentOperation || !m_currentOperation->isConfigured())
	{
		return;
	}
	m_currentOperationChanged = {};
	m_operationEditorPanel->Show(false);
	m_operationEditorPanel->GetParent()->Layout();
	m_operationModel->setProperties({});
	auto& context = m_documentManager->get(GetCurrentDocumentContext());
	context.getOperationManager()->submit(std::move(m_currentOperation));
	m_sideNotebook->SetSelection(0);
}

template <class BaseFrame>
void AppFrame<BaseFrame>::OnOperationCancel(wxCommandEvent& event)
{
	m_currentOperationChanged = {};
	m_operationEditorPanel->Show(false);
	m_operationEditorPanel->GetParent()->Layout();
	m_operationModel->setProperties({});
	m_currentOperation.reset();
	m_sideNotebook->SetSelection(0);
}

template <class BaseFrame>
int AppFrame<BaseFrame>::GetNewId()
{
	return m_nextId++;
}

END_FTK_NAMESPACE
