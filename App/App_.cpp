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

#include <wx/aboutdlg.h>

#include "ftk/Gui/wx/MainFrame.h"
#include "ftk/Core/Node.h"
#include "ftk/IO/Parse.h"
#include "ftk/IO/StorageElement.h"

#include <memory>

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

App::App()
{
	SetVendorName("ftkApp");
	SetAppName("ftkApp");
	SetAppDisplayName("ftkApp");
#if wxUSE_MDI_ARCHITECTURE
	m_mode = Mode::Mode_MDI;
#else
	m_mode = Mode::Mode_SDI;
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

	auto frame = CreateMainFrame();
	if (!frame)
	{
		return false;
	}
	frame->DragAcceptFiles(true);

#ifdef __WIN32__
	frame->Bind(wxEVT_IDLE,
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

	frame->Bind(wxEVT_DROP_FILES,
		[this](const wxDropFilesEvent& event)
		{
			for (int i = 0; i < event.GetNumberOfFiles(); i++)
			{
			}
		}
	);

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

int App::OnExit()
{
	return wxApp::OnExit();
}

void App::OnInitCmdLine(wxCmdLineParser& parser)
{
	wxApp::OnInitCmdLine(parser);
#if wxUSE_MDI_ARCHITECTURE
	parser.AddSwitch("", MDI, "run in MDI mode: multiple documents, single window");
#endif
#if wxUSE_AUI
	parser.AddSwitch("", AUI, "run in MDI mode using AUI: multiple documents, single window");
#endif
	parser.AddSwitch("", SDI, "run in SDI mode: multiple documents, multiple windows");
	parser.AddSwitch("", SINGLE, "run in single document mode");

	parser.AddParam("filename", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL);
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser)
{
	int numModeOptions = 0;

#if wxUSE_MDI_ARCHITECTURE
	if (parser.Found(MDI))
	{
		m_mode = Mode::Mode_MDI;
		numModeOptions++;
	}
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	if (parser.Found(AUI))
	{
		m_mode = Mode::Mode_AUI;
		numModeOptions++;
	}
#endif // wxUSE_AUI

	if (parser.Found(SDI))
	{
		m_mode = Mode::Mode_SDI;
		numModeOptions++;
	}

	if (parser.Found(SINGLE))
	{
		m_mode = Mode::Mode_Single;
		numModeOptions++;
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

#ifdef __WXMAC__
void App::MacNewFile()
{
	//FTK::DocManager::GetDocumentManager()->CreateNewDocument();
}
#endif

void App::CreateMenuBarForFrame(wxFrame* frame, wxMenu* file)
{
	auto menubar = new wxMenuBar;

	menubar->Append(file, wxGetStockLabel(wxID_FILE));

	auto help = new wxMenu;
	help->Append(wxID_ABOUT);
	menubar->Append(help, wxGetStockLabel(wxID_HELP));

	frame->SetMenuBar(menubar);
}

wxFrame* App::CreateMainFrame()
{
	switch (m_mode)
	{
#if wxUSE_MDI_ARCHITECTURE
	case Mode::Mode_MDI:
		{
			auto result = new FTK::MainFrameAny<PaneFrame<wxMDIParentFrame> >//new FTK::ParentFrameAny<wxMDIParentFrame>
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
	case Mode::Mode_AUI:
		{
			auto result = new PaneFrame<wxAuiMDIParentFrame>
				(
					nullptr, wxID_ANY,
					GetAppDisplayName(),
					wxDefaultPosition,
					wxWindow::FromDIP(wxSize(1280, 720), nullptr)
				);
			return result;
		}
#endif // wxUSE_AUI

	case Mode::Mode_SDI:
	case Mode::Mode_Single:
		{
			auto result = new PaneFrame<wxFrame>//new FTK::ParentFrameAny<wxFrame>
				(
					nullptr, wxID_ANY,
					GetAppDisplayName(),
					wxDefaultPosition,
					wxWindow::FromDIP(wxSize(1280, 720), nullptr)
				);
			return result;
		}
	}
	return nullptr;
}

//void App::OnForceCloseAll(wxCommandEvent& WXUNUSED(event))
//{
	// Pass "true" here to force closing just for testing this functionality,
	// there is no real reason to force the issue here.
	//FTK::DocManager::GetDocumentManager()->CloseDocuments(true);
//}

void App::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info; info.SetVersion("0.1");
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

wxDECLARE_APP(App);
wxIMPLEMENT_APP(App);
