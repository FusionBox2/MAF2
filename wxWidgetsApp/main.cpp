// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#include "wx/string.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/dc.h"
#include "wx/dialog.h"
#include "wx/menu.h"
#include "wx/filedlg.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/msgdlg.h"
#include "wx/mdi.h"
#include "wx/choicdlg.h"
#endif

#include "wx/frame.h"
#include "wx/filehistory.h"
#include "wx/vector.h"
#include "wx/textctrl.h"

#if wxUSE_STD_IOSTREAM
#include "wx/iosfwrap.h"
#else
#include "wx/stream.h"
#endif


#if wxUSE_PRINTING_ARCHITECTURE
#include "wx/prntbase.h"
#include "wx/printdlg.h"
#endif

#include "wx/confbase.h"
#include "wx/filename.h"
#include "wx/file.h"
#include "wx/ffile.h"
#include "wx/cmdproc.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/scopedarray.h"
#include "wx/scopeguard.h"

#if wxUSE_STD_IOSTREAM
#include "wx/beforestd.h"
#include <fstream>
#include <iostream>
#include "wx/afterstd.h"
#else
#include "wx/wfstream.h"
#endif

#if wxUSE_AUI
#include "wx/aui/tabmdi.h"
#include <wx/aui/aui.h>
#endif // wxUSE_AUI
//#include <wx/mdi.h>

#include "wx/cmdline.h"
#include "wx/config.h"

#ifdef __WXMAC__
#include "wx/filename.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "doc.xpm"
#include "chart.xpm"
#include "notepad.xpm"
#endif

#if wxUSE_PRINTING_ARCHITECTURE
#include "wx/print.h"
#endif

#include "ftk/Gui/wxw/MainFrame.h"
#include "MainWindow.h"

#include "Base/mfString.h"

#include <list>
#include <map>
#include <memory>
#include <optional>

// constants for the command line options names
namespace CmdLineOption
{

#if wxUSE_MDI_ARCHITECTURE
	const char* const MDI = "mdi";
#endif
#if wxUSE_AUI
	const char* const AUI = "aui";
#endif
	const char* const SDI = "sdi";
	const char* const SINGLE = "single";
}

class ftkDocChildFrameAnyBase;

class MyCanvas;

namespace FTK
{
	class DocManager;

	class Command
	{
	public:
		Command(bool canUndoIt = false, const mafString& name = _R(""));

		virtual ~Command() = default;

		virtual bool Do() = 0;

		virtual bool Undo() = 0;

		virtual bool CanUndo() const { return m_canUndo; }

		const mafString& GetName() const { return m_commandName; }

	protected:
		mafString m_commandName;
		bool     m_canUndo;
	};

	Command::Command(bool canUndoIt, const mafString& name)
		: m_commandName(name)
		, m_canUndo(canUndoIt)
	{
	}

	class CommandProcessor
	{
	public:
		CommandProcessor(size_t maxCommands = std::numeric_limits<size_t>::max());

		CommandProcessor(const CommandProcessor&) = delete;
		CommandProcessor& operator=(const CommandProcessor&) = delete;

		virtual ~CommandProcessor() = default;

		// Pass a command to the processor. The processor calls Do(); if
		// successful, is appended to the command history unless storeIt is false.
		virtual bool Submit(std::unique_ptr<Command> command, bool storeIt = true);

		// just store the command without executing it
		virtual void Store(std::unique_ptr<Command> command);

		virtual bool Undo();
		virtual bool Redo();
		virtual bool CanUndo() const;
		virtual bool CanRedo() const;

		// Sets the Undo/Redo menu strings for the current menu.
		virtual void SetMenuStrings_();

		// Gets the current Undo command name.
		mafString GetLastCommandName() const;

		// Gets the current Redo command name.
		mafString GetNextCommandName() const;

#if wxUSE_MENUS
		// Call this to manage an edit menu.
		void SetEditMenu_(wxMenu* menu) { m_commandEditMenu = menu; }
#endif // wxUSE_MENUS

		size_t GetMaxCommands() const { return m_maxNoCommands; }

		virtual void ClearCommands();

		// Has the current project been changed?
		virtual bool IsDirty() const;

		// Mark the current command as the one where the last save took place
		void MarkAsSaved()
		{
			m_lastSavedCommand = !m_undoCommands.empty() ? m_undoCommands.back().get() : nullptr;
		}
	protected:
		void ReduceUndoList(size_t size);
		size_t m_maxNoCommands;
		std::list<std::unique_ptr<Command> > m_undoCommands;
		std::list<std::unique_ptr<Command> > m_redoCommands;
		std::optional<Command*> m_lastSavedCommand = nullptr;

#if wxUSE_MENUS
		wxMenu* m_commandEditMenu = nullptr;
#endif // wxUSE_MENUS

		wxString      m_undoAccelerator;
		wxString      m_redoAccelerator;
	};

	CommandProcessor::CommandProcessor(size_t maxCommands)
		: m_maxNoCommands(maxCommands)
#if wxUSE_ACCEL
		, m_undoAccelerator('\t' + wxAcceleratorEntry(wxACCEL_CTRL, 'Z').ToString())
		, m_redoAccelerator('\t' + wxAcceleratorEntry(wxACCEL_CTRL, 'Y').ToString())
#endif
	{
	}

	// Pass a command to the processor. The processor calls Do();
	// if successful, is appended to the command history unless
	// storeIt is false.
	bool CommandProcessor::Submit(std::unique_ptr<Command> command, bool storeIt)
	{
		if (!command)
		{
			return false;
		}

		if (!command->Do())
		{
			return false;
		}

		if (!command->CanUndo())
		{
			ReduceUndoList(0);
		}
		else if (storeIt)
		{
			Store(std::move(command));
		}
		return true;
	}

	void CommandProcessor::Store(std::unique_ptr<Command> command)
	{
		if (!command)
		{
			return;
		}

		m_redoCommands.clear();

		if (m_maxNoCommands > 0)
		{
			ReduceUndoList(m_maxNoCommands - 1);
			m_undoCommands.push_back(std::move(command));
		}
	}

	void CommandProcessor::ReduceUndoList(size_t size)
	{
		while (m_undoCommands.size() > size)
		{
			if (m_lastSavedCommand == m_undoCommands.front().get())
			{
				m_lastSavedCommand = std::nullopt;
			}
			m_undoCommands.pop_front();
		}
	}

	bool CommandProcessor::Undo()
	{
		if (CanUndo())
		{
			if (m_undoCommands.back()->Undo())
			{
				m_redoCommands.splice(begin(m_redoCommands), m_undoCommands, --end(m_undoCommands));
				return true;
			}
		}
		return false;
	}

	bool CommandProcessor::Redo()
	{
		if (CanRedo())
		{
			if (m_redoCommands.front()->Do())
			{
				m_undoCommands.splice(end(m_undoCommands), m_redoCommands, begin(m_redoCommands));
				return true;
			}
		}
		return false;
	}

	bool CommandProcessor::CanUndo() const
	{
		return !m_undoCommands.empty();
	}

	bool CommandProcessor::CanRedo() const
	{
		return !m_redoCommands.empty();
	}

	mafString CommandProcessor::GetLastCommandName() const
	{
		if (!m_undoCommands.empty())
		{
			return m_undoCommands.back()->GetName();
		}
		return _R("");
	}

	mafString CommandProcessor::GetNextCommandName() const
	{
		if (!m_redoCommands.empty())
		{
			return m_redoCommands.front()->GetName();
		}
		return _R("");
	}

	void CommandProcessor::SetMenuStrings_()
	{
#if wxUSE_MENUS
		if (m_commandEditMenu)
		{
			wxString undoLabel = _("&Undo");
			wxString buf = mafStringToWx(GetLastCommandName());
			if (!buf.empty())
			{
				undoLabel += " ";
				undoLabel += buf;
			}
			undoLabel += m_undoAccelerator;

			wxString redoLabel = _("&Redo");
			buf = mafStringToWx(GetNextCommandName());
			if (!buf.empty())
			{
				redoLabel += " ";
				redoLabel += buf;
			}
			redoLabel += m_redoAccelerator;

			m_commandEditMenu->SetLabel(wxID_UNDO, undoLabel);
			m_commandEditMenu->Enable(wxID_UNDO, CanUndo());

			m_commandEditMenu->SetLabel(wxID_REDO, redoLabel);
			m_commandEditMenu->Enable(wxID_REDO, CanRedo());
		}
#endif // wxUSE_MENUS
	}

	void CommandProcessor::ClearCommands()
	{
		m_undoCommands.clear();
		m_redoCommands.clear();
		if (m_lastSavedCommand != nullptr)
		{
			m_lastSavedCommand = std::nullopt;
		}
	}

	bool CommandProcessor::IsDirty() const
	{
		if (!m_lastSavedCommand)
		{
			return true;
		}
		if (m_undoCommands.empty())
		{
			return m_lastSavedCommand != nullptr;
		}
		return m_lastSavedCommand != m_undoCommands.back().get();
	}

	class DocTemplate;

	class DocumentObserver
	{
	public:
		virtual ~DocumentObserver() = 0;
		virtual void Update() = 0;
		virtual void ChangeFilename() = 0;
		virtual void Closing() = 0;
	};

	DocumentObserver::~DocumentObserver() = default;

	class Document// : public wxEvtHandler
	{
	public:
		Document();

		Document(const Document&) = delete;
		Document& operator=(const Document&) = delete;

		//~Document() override;
		virtual ~Document();

		void SetFilename(const mafString& filename, bool notifyViews = false);
		const mafString& GetFilename() const { return m_documentFile; }

		void SetTitle(const mafString& title) { m_documentTitle = title; }
		const mafString& GetTitle() const { return m_documentTitle; }

		// access the flag indicating whether this document had been already saved,
		// SetDocumentSaved() is only used internally, don't call it
		bool GetDocumentSaved() const { return m_savedYet; }
		void SetDocumentSaved(bool saved = true) { m_savedYet = saved; }

		// return true if the document hasn't been modified since the last time it
		// was saved (implying that it returns false if it was never saved, even if
		// the document is not modified)
		bool AlreadySaved() const { return !IsModified() && GetDocumentSaved(); }

		virtual bool Close();
		virtual bool Save();
		virtual bool SaveAs();
		virtual bool Revert();

		virtual std::ostream& SaveObject(std::ostream& stream) { return stream; }
		virtual std::istream& LoadObject(std::istream& stream) { return stream; }

		// Called by wxWidgets
		virtual bool OnSaveDocument(const mafString& filename);
		virtual bool OnOpenDocument(const mafString& filename);
		virtual bool OnNewDocument(const mafString& name);
		virtual bool OnCloseDocument();

		// Prompts for saving if about to close a modified document. Returns true
		// if ok to close the document (may have saved in the meantime, or set
		// modified to false)
		virtual bool OnSaveModified();

		// Similar to OnSaveModified() but doesn't allow the user to prevent the
		// document from closing as it will be closed unconditionally.
		virtual void OnSaveBeforeForceClose();

		// if you override, remember to call the default
		// implementation (ftkDocument::OnChangeFilename)
		virtual void OnChangeFilename(bool notifyViews);

		// Called by framework if created automatically by the default document
		// manager: gives document a chance to initialise and (usually) create a
		// view
		virtual bool OnCreate(const mafString& path);

		// Called from OnCloseDocument(), does nothing by default but may be
		// overridden. Return value is ignored.
		virtual bool DeleteContents();

		virtual bool IsModified() const { return m_documentModified; }
		virtual void Modify(bool mod);

		virtual bool AddObserver(DocumentObserver* observer);
		virtual bool RemoveObserver(DocumentObserver* observer);

		virtual void NotifyUpdate();
		virtual void NotifyClosing();

		// Get the document name to be shown to the user: the title if there is
		// any, otherwise the filename if the document was saved and, finally,
		// "unnamed" otherwise
		virtual mafString GetUserReadableName() const;

		// Returns a window that can be used as a parent for document-related
		// dialogs. Override if necessary.
		virtual wxWindow* GetDocumentWindow() const;

		// Ask the user if the document should be saved if it's modified and save
		// it if necessary.
		//
		// Returns false if the user cancelled closing or if saving failed.
		bool CanClose();

		std::shared_ptr<DocTemplate> GetDocTemplate() const { return m_docTemplate; }
		void SetDocTemplate(std::shared_ptr<DocTemplate> docTemplate) { m_docTemplate = std::move(docTemplate); }

	protected:
		std::shared_ptr<DocTemplate> m_docTemplate;
		std::list<DocumentObserver*> m_documentObservers;
		mafString m_documentFile;
		mafString m_documentTitle;
		bool m_documentModified = false;
		bool m_savedYet = false;

		FTK::CommandProcessor* m_commandProcessor = nullptr;

		// Called by OnSaveDocument and OnOpenDocument to implement standard
		// Save/Load behaviour. Re-implement in derived class for custom
		// behaviour.
		virtual bool DoSaveDocument(const mafString& file);
		virtual bool DoOpenDocument(const mafString& file);

		// the default implementation of GetUserReadableName()
		mafString DoGetUserReadableName() const;
	};

	class View : public wxEvtHandler, public DocumentObserver
	{
	public:
		// this sample can be launched in several different ways:
		enum class Mode
		{
#if wxUSE_MDI_ARCHITECTURE
			Mode_MDI,   // MDI mode: multiple documents, single top level window
#endif // wxUSE_MDI_ARCHITECTURE
#if wxUSE_AUI
			Mode_AUI,   // MDI AUI mode
#endif // wxUSE_AUI
			Mode_SDI,   // SDI mode: multiple documents, multiple top level windows
			Mode_Single // single document mode (and hence single top level window)
		};

		View() = default;

		View(const View&) = delete;
		View& operator=(const View&) = delete;

		~View() override;

		Document* GetDocument() const { return m_viewDocument.get(); }

		const mafString& GetViewName() const { return m_viewTypeName; }
		void SetViewName(const mafString& name) { m_viewTypeName = name; }

		wxWindow* GetFrame() const { return m_viewFrame; }
		void SetFrame(wxWindow* frame) { m_viewFrame = frame; }

		virtual void OnActivateView(bool activate, View* activeView, View* deactiveView);
		virtual void OnDraw(wxDC* dc) = 0;
		virtual void OnPrint(wxDC* dc, wxObject* info);
		virtual void OnUpdate(View* sender, wxObject* hint = nullptr);
		virtual void OnClosingDocument() {}
		virtual void OnChangeFilename();

		void Update() override { OnUpdate(nullptr); }
		void ChangeFilename() override { OnChangeFilename(); }
		void Closing() override { OnClosingDocument(); }


		// Called by framework if created automatically by the default document
		// manager class: gives view a chance to initialise
		virtual bool OnCreate(std::shared_ptr<Document> doc);

		// Checks if the view is the last one for the document; if so, asks user
		// to confirm save data (if modified). If ok, deletes itself and returns
		// true.
		virtual bool Close(bool deleteWindow = true);

		// Override to do cleanup/veto close
		virtual bool OnClose(bool deleteWindow);

		// A view's window can call this to notify the view it is (in)active.
		// The function then notifies the document manager.
		virtual void Activate(bool activate);

		DocManager* GetDocumentManager() const;

#if wxUSE_PRINTING_ARCHITECTURE
		virtual wxPrintout* OnCreatePrintout();
#endif

		// implementation only
		// -------------------

		// set the associated frame, it is used to reset its view when we're
		// destroyed
		void SetDocChildFrame(ftkDocChildFrameAnyBase* docChildFrame);

		// get the associated frame, may be null during destruction
		ftkDocChildFrameAnyBase* GetDocChildFrame() const { return m_docChildFrame; }

	protected:
		// hook the document into event handlers chain here
		bool TryBefore(wxEvent& event) override;

		std::shared_ptr<Document> m_viewDocument;
		mafString m_viewTypeName;
		wxWindow* m_viewFrame = nullptr;

		ftkDocChildFrameAnyBase* m_docChildFrame = nullptr;
	};

	class DocTemplate
	{
	public:
		DocTemplate(
			const mafString& description,
			const mafString& filter,
			const mafString& ext,
			const mafString& docTypeName,
			std::function<std::unique_ptr<Document>()> docCreate);

		DocTemplate(const DocTemplate&) = delete;
		DocTemplate& operator=(const DocTemplate&) = delete;

		virtual ~DocTemplate() = default;

		virtual std::shared_ptr<Document> CreateDocument(const mafString& path);

		const mafString& GetDefaultExtension() const { return m_defaultExt; }
		const mafString& GetDescription() const { return m_description; }
		const mafString& GetFileFilter() const { return m_fileFilter; }
		const mafString& GetDocumentName() const { return m_docTypeName; }

		void SetFileFilter(const mafString& filter) { m_fileFilter = filter; }
		void SetDescription(const mafString& description) { m_description = description; }
		void SetDefaultExtension(const mafString& ext) { m_defaultExt = ext; }

		virtual bool FileMatchesTemplate(const mafString& path);

	protected:
		mafString m_fileFilter;
		mafString m_description;
		mafString m_defaultExt;
		mafString m_docTypeName;
		std::function<std::unique_ptr<Document>()> m_docCreate;
	};

	class ViewTemplate
	{
	public:
		ViewTemplate(const mafString& viewTypeName, std::function<std::unique_ptr<View>()> viewCreate);

		ViewTemplate(const ViewTemplate&) = delete;
		ViewTemplate& operator=(const ViewTemplate&) = delete;

		virtual ~ViewTemplate() = default;

		virtual std::unique_ptr<View> CreateView(std::shared_ptr<Document> doc);

		const mafString& GetViewName() const { return m_viewTypeName; }

	protected:
		mafString m_viewTypeName;
		std::function<std::unique_ptr<View>()> m_viewCreate;
	};

	class DocViewTemplate : public DocTemplate, public ViewTemplate
	{
	public:
		DocViewTemplate(
			const mafString& description,
			const mafString& filter,
			const mafString& ext,
			const mafString& docTypeName,
			const mafString& viewTypeName,
			std::function<std::unique_ptr<FTK::Document>()> docCreate,
			std::function<std::unique_ptr<FTK::View>()> viewCreate)
			: DocTemplate(description, filter, ext, docTypeName, docCreate)
			, ViewTemplate(viewTypeName, viewCreate)
		{
		}
	};


	// One object of this class may be created in an application, to manage all
	// the templates and documents.
	class DocManager : public wxEvtHandler
	{
	public:
		enum
		{
			wxDOC_NEW = 1,
			wxDOC_SILENT = 2
		};

		DocManager();

		DocManager(const DocManager&) = delete;
		DocManager& operator=(const DocManager&) = delete;

		~DocManager() override;

		// Handlers for common user commands
		void OnFileClose(wxCommandEvent& event);
		void OnFileCloseAll(wxCommandEvent& event);
		void OnFileNew(wxCommandEvent& event);
		void OnFileOpen(wxCommandEvent& event);
		void OnFileRevert(wxCommandEvent& event);
		void OnFileSave(wxCommandEvent& event);
		void OnFileSaveAs(wxCommandEvent& event);
		void OnMRUFile(wxCommandEvent& event);
#if wxUSE_PRINTING_ARCHITECTURE
		void OnPrint(wxCommandEvent& event);
		void OnPreview(wxCommandEvent& event);
		void OnPageSetup(wxCommandEvent& event);
#endif // wxUSE_PRINTING_ARCHITECTURE
		void OnUndo(wxCommandEvent& event);
		void OnRedo(wxCommandEvent& event);

		// Handlers for UI update commands
		void OnUpdateFileOpen(wxUpdateUIEvent& event);
		void OnUpdateDisableIfNoDoc(wxUpdateUIEvent& event);
		void OnUpdateFileRevert(wxUpdateUIEvent& event);
		void OnUpdateFileNew(wxUpdateUIEvent& event);
		void OnUpdateFileSave(wxUpdateUIEvent& event);
		void OnUpdateFileSaveAs(wxUpdateUIEvent& event);
		void OnUpdateUndo(wxUpdateUIEvent& event);
		void OnUpdateRedo(wxUpdateUIEvent& event);

		// called when file format detection didn't work, can be overridden to do
		// something in this case
		virtual void OnOpenFileFailure() {}

		virtual std::shared_ptr<FTK::Document> CreateDocument(const wxString& path, long flags = 0);

		// wrapper around CreateDocument() with a more clear name
		std::shared_ptr<FTK::Document> CreateNewDocument()
		{
			return CreateDocument(wxString(), wxDOC_NEW);
		}

		virtual std::unique_ptr<FTK::View> CreateView(std::shared_ptr<FTK::Document> doc);
		virtual void DeleteTemplate(FTK::DocViewTemplate* temp, long flags = 0);
		virtual bool FlushDoc(FTK::Document* doc);
		virtual std::shared_ptr<FTK::DocViewTemplate> MatchTemplate(const wxString& path);
		virtual std::shared_ptr<FTK::DocViewTemplate> SelectDocumentPath(wxString& path, long flags, bool save = false);
		virtual std::shared_ptr<FTK::DocViewTemplate> SelectDocumentType();
		virtual std::shared_ptr<FTK::DocViewTemplate> SelectViewType();
		virtual std::shared_ptr<FTK::DocViewTemplate> FindTemplateForPath(const wxString& path);

		virtual bool AddView(FTK::View* view);
		virtual bool RemoveView(FTK::View* view);

		FTK::CommandProcessor* GetCommandProcessor(FTK::Document* doc) const;

		void AssociateTemplate(std::shared_ptr<FTK::DocViewTemplate> temp);
		void DisassociateTemplate(FTK::DocViewTemplate* temp);

		// Find template from document class info, may return nullptr.
		//ftkDocTemplate* FindTemplate(const wxClassInfo* documentClassInfo);

		// Find document from file name, may return nullptr.
		std::shared_ptr<FTK::Document> FindDocumentByPath(const wxString& path) const;

		FTK::Document* GetCurrentDocument() const;

		void SetMaxDocsOpen(int n) { m_maxDocsOpen = n; }
		int GetMaxDocsOpen() const { return m_maxDocsOpen; }

		// closes all currently open documents
		bool CloseDocuments(bool force = true);

		// closes the specified document
		bool CloseDocument(FTK::Document* doc, bool force = false);

		// Clear remaining documents and templates
		bool Clear(bool force = true);

		// Views or windows should inform the document manager
		// when a view is going in or out of focus
		virtual void ActivateView(FTK::View* view, bool activate = true);
		virtual FTK::View* GetCurrentView() const { return m_currentView; }

		// This method tries to find an active view harder than GetCurrentView():
		// if the latter is null, it also checks if we don't have just a single
		// view and returns it then.
		FTK::View* GetAnyUsableView() const;

		std::vector<std::shared_ptr<FTK::DocViewTemplate> >& GetTemplates() { return m_templates; }

		// Return the default name for a new document (by default returns strings
		// in the form "unnamed <counter>" but can be overridden)
		virtual wxString MakeNewDocumentName();

		// Make a frame title (override this to do something different)
		virtual wxString MakeFrameTitle(FTK::Document* doc);

		virtual wxFileHistory* GetFileHistory() const { return m_fileHistory.get(); }

		// File history management
		virtual void AddFileToHistory(const wxString& file);
		virtual void RemoveFileFromHistory(size_t i);
		virtual size_t GetHistoryFilesCount() const;
		virtual wxString GetHistoryFile(size_t i) const;
		virtual void FileHistoryUseMenu(wxMenu* menu);
		virtual void FileHistoryRemoveMenu(wxMenu* menu);
#if wxUSE_CONFIG
		virtual void FileHistoryLoad(const wxConfigBase& config);
		virtual void FileHistorySave(wxConfigBase& config);
#endif // wxUSE_CONFIG

		virtual void FileHistoryAddFilesToMenu();
		virtual void FileHistoryAddFilesToMenu(wxMenu* menu);

		wxString GetLastDirectory() const;
		void SetLastDirectory(const wxString& dir) { m_lastDirectory = dir; }

		// Get the current document manager
		static DocManager* GetDocumentManager() { return sm_docManager; }

#if wxUSE_PRINTING_ARCHITECTURE
		wxPageSetupDialogData& GetPageSetupDialogData()
		{
			return m_pageSetupDialogData;
		}
		const wxPageSetupDialogData& GetPageSetupDialogData() const
		{
			return m_pageSetupDialogData;
		}
#endif // wxUSE_PRINTING_ARCHITECTURE

	protected:

		struct DocInfo
		{
			std::list<FTK::View*> m_views;
			std::unique_ptr<FTK::CommandProcessor> m_commandProcessor;
		};

		// Called when a file selected from the MRU list doesn't exist any more.
		// The default behaviour is to remove the file from the MRU and notify the
		// user about it but this method can be overridden to customize it.
		virtual void OnMRUFileNotExist(unsigned n, const wxString& filename);

		// Open the MRU file with the given index in our associated file history.
		void DoOpenMRUFile(unsigned n);
#if wxUSE_PRINTING_ARCHITECTURE
		virtual wxPreviewFrame* CreatePreviewFrame(wxPrintPreviewBase* preview,
			wxWindow* parent,
			const wxString& title);
#endif // wxUSE_PRINTING_ARCHITECTURE

		// hook the currently active view into event handlers chain here
		virtual bool TryBefore(wxEvent& event) override;

		// return the command processor for the current document, if any
		FTK::CommandProcessor* GetCurrentCommandProcessor() const;

		int m_defaultDocumentNameCounter = 1;
		int m_maxDocsOpen = std::numeric_limits<int>::max();
		std::vector<std::shared_ptr<FTK::DocViewTemplate> > m_templates;
		std::list<std::pair<std::shared_ptr<FTK::Document>, DocInfo > > m_documentsInfo;
		FTK::View* m_currentView = nullptr;
		std::unique_ptr<wxFileHistory> m_fileHistory;
		wxString m_lastDirectory;
		static DocManager* sm_docManager;

#if wxUSE_PRINTING_ARCHITECTURE
		wxPageSetupDialogData m_pageSetupDialogData;
#endif // wxUSE_PRINTING_ARCHITECTURE
	};
}

// ----------------------------------------------------------------------------
// Base class for child frames -- this is what wxView renders itself into
//
// Notice that this is a mix-in class so it doesn't derive from wxWindow, only
// wxDocChildFrameAny does
// ----------------------------------------------------------------------------

class ftkDocChildFrameAnyBase
{
public:
	// default ctor, use Create() after it
	ftkDocChildFrameAnyBase() = default;

	ftkDocChildFrameAnyBase(const ftkDocChildFrameAnyBase&) = delete;
	ftkDocChildFrameAnyBase& operator=(const ftkDocChildFrameAnyBase&) = delete;

	// full ctor equivalent to using the default one and Create()
	ftkDocChildFrameAnyBase(FTK::View* view, wxWindow* win)
	{
		Create(view, win);
	}

	// method which must be called for an object created using the default ctor
	//
	// note that it returns bool just for consistency with Create() methods in
	// other classes, we never return false from here
	bool Create(FTK::View* view, wxWindow* win)
	{
		m_childView = view;
		m_win = win;

		if (view)
			view->SetDocChildFrame(this);

		return true;
	}

	// dtor doesn't need to be virtual, an object should never be destroyed via
	// a pointer to this class
	~ftkDocChildFrameAnyBase()
	{
		// prevent the view from deleting us if we're being deleted directly
		// (and not via Close() + Destroy())
		if (m_childView)
			m_childView->SetDocChildFrame(nullptr);
	}

	FTK::View* GetView() const { return m_childView; }
	void SetView(FTK::View* view) { m_childView = view; }

	wxWindow* GetWindow() const { return m_win; }

	// implementation only

	// Check if this event had been just processed in this frame.
	bool HasAlreadyProcessed(wxEvent& event) const
	{
		return m_lastEvent == &event;
	}

protected:
	// we're not a wxEvtHandler but we provide this wxEvtHandler-like function
	// which is called from TryBefore() of the derived classes to give our view
	// a chance to process the message before the frame event handlers are used
	bool TryProcessEvent(wxEvent& event);

	// called from EVT_CLOSE handler in the frame: check if we can close and do
	// cleanup if so; veto the event otherwise
	bool CloseView(wxCloseEvent& event);


	FTK::View* m_childView = nullptr;

	// the associated window: having it here is not terribly elegant but it
	// allows us to avoid having any virtual functions in this class
	wxWindow* m_win = nullptr;

private:
	// Pointer to the last processed event used to avoid sending the same event
	// twice to wxDocManager, from here and from wxDocParentFrameAnyBase.
	wxEvent* m_lastEvent = nullptr;
};

// ----------------------------------------------------------------------------
// Template implementing child frame concept using the given wxFrame-like class
//
// This is used to define wxDocChildFrame and wxDocMDIChildFrame: ChildFrame is
// a wxFrame or wxMDIChildFrame (although in theory it could be any wxWindow-
// derived class as long as it provided a ctor with the same signature as
// wxFrame and OnActivate() method) and ParentFrame is either wxFrame or
// wxMDIParentFrame.
// ----------------------------------------------------------------------------

// Note that we intentionally do not use WXDLLIMPEXP_CORE for this class as it
// has only inline methods.

template <class ChildFrame, class ParentFrame>
class ftkDocChildFrameAny : public ChildFrame, public ftkDocChildFrameAnyBase
{
public:
	typedef ChildFrame BaseClass;

	// default ctor, use Create after it
	ftkDocChildFrameAny() = default;

	ftkDocChildFrameAny(const ftkDocChildFrameAny&) = delete;
	ftkDocChildFrameAny& operator=(const ftkDocChildFrameAny&) = delete;

	// ctor for a frame showing the given view of the specified document
	ftkDocChildFrameAny(FTK::View* view,
		ParentFrame* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr))
	{
		Create(view, parent, id, title, pos, size, style, name);
	}

	bool Create(FTK::View* view,
		ParentFrame* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr))
	{
		this->Bind(wxEVT_ACTIVATE, &ftkDocChildFrameAny::OnActivate, this);
		this->Bind(wxEVT_CLOSE_WINDOW, &ftkDocChildFrameAny::OnCloseWindow, this);

		if (!ftkDocChildFrameAnyBase::Create(view, this))
			return false;

		if (!BaseClass::Create(parent, id, title, pos, size, style, name))
			return false;

		return true;
	}

protected:
	// hook the child view into event handlers chain here
	virtual bool TryBefore(wxEvent& event) override
	{
		return TryProcessEvent(event) || BaseClass::TryBefore(event);
	}

private:
	void OnActivate(wxActivateEvent& event)
	{
		BaseClass::OnActivate(event);

		if (m_childView)
			m_childView->Activate(event.GetActive());
	}

	void OnCloseWindow(wxCloseEvent& event)
	{
		if (CloseView(event))
			this->Destroy();
		//else: vetoed
	}
};

// ----------------------------------------------------------------------------
// wxDocParentFrame and related classes.
//
// As with wxDocChildFrame we define a template base class used by both normal
// and MDI versions
// ----------------------------------------------------------------------------

// Base class containing type-independent code of wxDocParentFrameAny
//
// Similarly to wxDocChildFrameAnyBase, this class is a mix-in and doesn't
// derive from wxWindow.
class ftkDocParentFrameAnyBase
{
public:
	ftkDocParentFrameAnyBase(wxWindow* frame)
		: m_frame(frame)
	{
		m_docManager = nullptr;
	}

	ftkDocParentFrameAnyBase(const ftkDocParentFrameAnyBase&) = delete;
	ftkDocParentFrameAnyBase& operator=(const ftkDocParentFrameAnyBase&) = delete;

	FTK::DocManager* GetDocumentManager() const { return m_docManager; }

protected:
	// This is similar to wxDocChildFrameAnyBase method with the same name:
	// while we're not an event handler ourselves and so can't override
	// TryBefore(), we provide a helper that the derived template class can use
	// from its TryBefore() implementation.
	bool TryProcessEvent(wxEvent& event);

	wxWindow* const m_frame;
	FTK::DocManager* m_docManager;
};

// This is similar to wxDocChildFrameAny and is used to provide common
// implementation for both wxDocParentFrame and wxDocMDIParentFrame
template <class BaseFrame>
class ftkDocParentFrameAny : public BaseFrame, public ftkDocParentFrameAnyBase
{
public:
	ftkDocParentFrameAny() : ftkDocParentFrameAnyBase(this) {}

	ftkDocParentFrameAny(const ftkDocParentFrameAny&) = delete;
	ftkDocParentFrameAny& operator=(const ftkDocParentFrameAny&) = delete;

	ftkDocParentFrameAny(
		FTK::DocManager* manager,
		wxFrame* frame,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr))
		: ftkDocParentFrameAnyBase(this)
	{
		Create(manager, frame, id, title, pos, size, style, name);
	}

	bool Create(
		FTK::DocManager* manager,
		wxFrame* frame,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr))
	{
		m_docManager = manager;

		if (!BaseFrame::Create(frame, id, title, pos, size, style, name))
			return false;

		this->Bind(wxEVT_MENU, &ftkDocParentFrameAny::OnExit, this, wxID_EXIT);
		this->Bind(wxEVT_CLOSE_WINDOW, &ftkDocParentFrameAny::OnCloseWindow, this);

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
		this->Close();
	}

	void OnCloseWindow(wxCloseEvent& event)
	{
		if (m_docManager && !m_docManager->Clear(!event.CanVeto()))
		{
			// The user decided not to close finally, abort.
			event.Veto();
		}
		else
		{
			// Just skip the event, base class handler will destroy the window.
			event.Skip();
		}
	}
};

#if wxUSE_PRINTING_ARCHITECTURE
class ftkDocPrintout : public wxPrintout
{
public:
	ftkDocPrintout(FTK::View* view = nullptr, const wxString& title = wxString());

	ftkDocPrintout(const ftkDocPrintout&) = delete;
	ftkDocPrintout& operator=(const ftkDocPrintout&) = delete;

	// implement wxPrintout methods
	bool OnPrintPage(int page) override;
	bool HasPage(int page) override;
	bool OnBeginDocument(int startPage, int endPage) override;
	void GetPageInfo(int* minPage, int* maxPage,
		int* selPageFrom, int* selPageTo) override;

	virtual FTK::View* GetView() { return m_printoutView; }

protected:
	static wxString GetAppropriateTitle(const FTK::View* view, const wxString& titleGiven);
	FTK::View* m_printoutView;

private:
	wxDECLARE_DYNAMIC_CLASS(ftkDocPrintout);
};
#endif // wxUSE_PRINTING_ARCHITECTURE

#if wxUSE_PRINTING_ARCHITECTURE
wxIMPLEMENT_DYNAMIC_CLASS(ftkDocPrintout, wxPrintout);
#endif

class TextEditDocument : public FTK::Document
{
public:
	TextEditDocument() = default;

	TextEditDocument(const TextEditDocument&) = delete;
	TextEditDocument& operator=(const TextEditDocument&) = delete;

	bool OnCreate(const mafString& path) override;

	bool IsModified() const override;
	void Modify(bool mod) override;

protected:
	bool DoSaveDocument(const mafString& filename) override;
	bool DoOpenDocument(const mafString& filename) override;

private:
	wxTextCtrl* GetTextCtrl() const;

	void OnTextChange(wxCommandEvent& event);
};

class TextEditView : public FTK::View
{
public:
	TextEditView();

	bool OnCreate(std::shared_ptr<FTK::Document> doc) override;
	void OnDraw(wxDC* dc) override;
	bool OnClose(bool deleteWindow = true) override;

	wxTextCtrl* GetText() const { return m_text; }

private:
	void OnCopy(wxCommandEvent& WXUNUSED(event)) { m_text->Copy(); }
	void OnPaste(wxCommandEvent& WXUNUSED(event)) { m_text->Paste(); }
	void OnSelectAll(wxCommandEvent& WXUNUSED(event)) { m_text->SelectAll(); }

	wxTextCtrl* m_text = nullptr;
};

// Represents a line from one point to the other
struct DoodleLine
{
	DoodleLine() { /* leave fields uninitialized */ }

	DoodleLine(const wxPoint& pt1, const wxPoint& pt2)
		: x1(pt1.x), y1(pt1.y), x2(pt2.x), y2(pt2.y)
	{
	}

	wxInt32 x1;
	wxInt32 y1;
	wxInt32 x2;
	wxInt32 y2;
};

// Contains a list of lines: represents a mouse-down doodle
class DoodleSegment
{
public:
	std::ostream& SaveObject(std::ostream& stream);
	std::istream& LoadObject(std::istream& stream);

	bool IsEmpty() const { return m_lines.empty(); }
	void AddLine(const wxPoint& pt1, const wxPoint& pt2)
	{
		m_lines.emplace_back(pt1, pt2);
	}
	const std::vector<DoodleLine>& GetLines() const { return m_lines; }

private:
	std::vector<DoodleLine> m_lines;
};

// The drawing document (model) class itself
class DrawingDocument : public FTK::Document
{
public:
	DrawingDocument() = default;

	std::ostream& SaveObject(std::ostream& stream) override;
	std::istream& LoadObject(std::istream& stream) override;

	// add a new segment to the document
	void AddDoodleSegment(const DoodleSegment& segment);

	// remove the last segment, if any, and copy it in the provided pointer if
	// not null and return true or return false and do nothing if there are no
	// segments
	bool PopLastSegment(DoodleSegment* segment);

	// get direct access to our segments (for DrawingView)
	const std::vector<DoodleSegment>& GetSegments() const { return m_doodleSegments; }

private:
	std::vector<DoodleSegment> m_doodleSegments;
};

// The view using MyCanvas to show its contents
class DrawingView : public FTK::View
{
public:
	DrawingView();

	bool OnCreate(std::shared_ptr<FTK::Document> doc) override;
	void OnDraw(wxDC* dc) override;
	void OnUpdate(FTK::View* sender, wxObject* hint = nullptr) override;
	bool OnClose(bool deleteWindow = true) override;

	DrawingDocument* GetDocument();

private:
	void OnCut(wxCommandEvent& event);

	MyCanvas* m_canvas = nullptr;
};

namespace FTK
{
	Document::Document() = default;

	Document::~Document() = default;

	bool Document::DeleteContents()
	{
		return true;
	}

	bool Document::CanClose()
	{
		if (!OnSaveModified())
			return false;
		return true;
	}

	bool Document::Close()
	{
		// First check if this document itself and all its children can be closed.
		if (!CanClose())
			return false;
		return OnCloseDocument();
	}

	bool Document::OnCloseDocument()
	{
		// Tell all views that we're about to close
		NotifyClosing();
		DeleteContents();
		Modify(false);
		return true;
	}

	void Document::Modify(bool mod)
	{
		if (mod != m_documentModified)
		{
			m_documentModified = mod;
			// Allow views to append asterix to the title
			for (auto observer : m_documentObservers)
			{
				observer->ChangeFilename();
			}
		}
	}

	bool Document::OnNewDocument(const mafString& name)
	{
		// notice that there is no need to either reset nor even check the
		// modified flag here as the document itself is a new object (this is only
		// called from CreateDocument()) and so it shouldn't be saved anyhow even
		// if it is modified -- this could happen if the user code creates
		// documents pre-filled with some user-entered (and which hence must not be
		// lost) information

		SetDocumentSaved(false);

		SetTitle(name);
		SetFilename(name, true);

		return true;
	}

	bool Document::Save()
	{
		if (AlreadySaved())
			return true;

		if (m_documentFile.empty() || !m_savedYet)
			return SaveAs();

		return OnSaveDocument(m_documentFile);
	}

	bool Document::SaveAs()
	{
#ifdef wxHAS_MULTIPLE_FILEDLG_FILTERS
		wxString filter = mafStringToWx(m_docTemplate->GetDescription()) + wxT(" (") +
			mafStringToWx(m_docTemplate->GetFileFilter()) + wxT(")|") +
			mafStringToWx(m_docTemplate->GetFileFilter());

		// Now see if there are some other template with identical view and document
		// classes, whose filters may also be used.
		/*if (docTemplate->GetViewClassInfo() && docTemplate->GetDocClassInfo())
		{
			auto& templates = docTemplate->GetDocumentManager()->GetTemplates();
			for (auto& t : templates)
			{
				if (t->IsVisible() && t != docTemplate &&
					t->GetViewClassInfo() == docTemplate->GetViewClassInfo() &&
					t->GetDocClassInfo() == docTemplate->GetDocClassInfo())
				{
					// add a '|' to separate this filter from the previous one
					if (!filter.empty())
						filter << wxT('|');

					filter << t->GetDescription()
						<< wxT(" (") << t->GetFileFilter() << wxT(") |")
						<< t->GetFileFilter();
				}
			}
		}*/
#else
		wxString filter = docTemplate->GetFileFilter();
#endif

		wxString defaultDir = wxPathOnly(mafStringToWx(GetFilename()));
		if (defaultDir.empty())
		{
			defaultDir = FTK::DocManager::GetDocumentManager()->GetLastDirectory();
		}

		wxString fileName = wxFileSelector(_("Save As"),
			defaultDir,
			wxFileNameFromPath(mafStringToWx(GetFilename())),
			mafStringToWx(m_docTemplate->GetDefaultExtension()),
			filter,
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
			GetDocumentWindow());

		if (fileName.empty())
			return false; // cancelled by user

		// Files that were not saved correctly are not added to the FileHistory.
		if (!OnSaveDocument(mafWxToString(fileName)))
			return false;

		SetTitle(mafWxToString(wxFileNameFromPath(fileName)));
		SetFilename(mafWxToString(fileName), true);    // will call OnChangeFileName automatically

		// A file that doesn't use the default extension of its document template
		// cannot be opened via the FileHistory, so we do not add it.
		//if (docTemplate->FileMatchesTemplate(fileName))
		{
			FTK::DocManager::GetDocumentManager()->AddFileToHistory(fileName);
		}
		//else: the user will probably not be able to open the file again, so we
		//      could warn about the wrong file-extension here

		return true;
	}

	bool Document::OnSaveDocument(const mafString& file)
	{
		if (file.empty())
			return false;

		if (!DoSaveDocument(file))
			return false;

		if (m_commandProcessor)
			m_commandProcessor->MarkAsSaved();

		Modify(false);
		SetFilename(file);
		SetDocumentSaved(true);
		return true;
	}

	bool Document::OnOpenDocument(const mafString& file)
	{
		// notice that there is no need to check the modified flag here for the
		// reasons explained in OnNewDocument()

		if (!DoOpenDocument(file))
			return false;

		SetFilename(file, true);

		// stretching the logic a little this does make sense because the document
		// had been saved into the file we just loaded it from, it just could have
		// happened during a previous program execution, it's just that the name of
		// this method is a bit unfortunate, it should probably have been called
		// HasAssociatedFileName()
		SetDocumentSaved(true);

		NotifyUpdate();

		return true;
	}

	bool Document::Revert()
	{
		if (!DoOpenDocument(GetFilename()))
			return false;

		Modify(false);
		NotifyUpdate();

		return true;
	}


	// Get title, or filename if no title, else unnamed
	mafString Document::GetUserReadableName() const
	{
		return DoGetUserReadableName();
	}

	mafString Document::DoGetUserReadableName() const
	{
		if (!m_documentTitle.empty())
			return m_documentTitle;

		if (!m_documentFile.empty())
			return mafWxToString(wxFileNameFromPath(mafStringToWx(m_documentFile)));

		return _L("unnamed");
	}

	wxWindow* Document::GetDocumentWindow() const
	{
		View* view = nullptr;// GetFirstView();

		return view ? view->GetFrame() : wxTheApp->GetTopWindow();
	}

	// true if safe to close
	bool Document::OnSaveModified()
	{
		if (IsModified())
		{
			wxMessageDialog dialogSave
			(
				GetDocumentWindow(),
				wxString::Format
				(
					_("Do you want to save changes to %s?"),
					mafStringToWx(GetUserReadableName())
				),
				wxTheApp->GetAppDisplayName(),
				wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE
			);
			dialogSave.SetYesNoCancelLabels
			(
				_("&Save"),
				_("&Discard changes"),
				_("Do&n't close")
			);

			switch (dialogSave.ShowModal())
			{
			case wxID_NO:
				Modify(false);
				break;

			case wxID_YES:
				return Save();

			case wxID_CANCEL:
				return false;
			}
		}

		return true;
	}

	void Document::OnSaveBeforeForceClose()
	{
		if (!IsModified())
			return;

		wxMessageDialog dialogSave
		(
			GetDocumentWindow(),
			wxString::Format
			(
				_("Do you want to save changes to %s before closing it?"),
				mafStringToWx(GetUserReadableName())
			),
			wxTheApp->GetAppDisplayName(),
			wxYES_NO | wxICON_QUESTION | wxCENTRE
		);
		dialogSave.SetExtendedMessage(_("The document must be closed."));
		dialogSave.SetYesNoLabels(_("&Save"), _("&Discard changes"));

		if (dialogSave.ShowModal() == wxID_YES)
		{
			while (!Save())
			{
				wxMessageDialog dialogRetry
				(
					GetDocumentWindow(),
					wxString::Format
					(
						_("Saving %s failed, would you like to retry?"),
						mafStringToWx(GetUserReadableName())
					),
					wxTheApp->GetAppDisplayName(),
					wxYES_NO | wxICON_ERROR | wxCENTRE
				);
				dialogRetry.SetYesNoLabels(_("Retry"), _("Discard changes"));

				if (dialogRetry.ShowModal() != wxID_YES)
					break;
			}
		}

		Modify(false);
	}

	bool Document::AddObserver(DocumentObserver* observer)
	{
		if (std::find(begin(m_documentObservers), end(m_documentObservers), observer) == end(m_documentObservers))
		{
			m_documentObservers.push_back(observer);
		}
		return true;
	}

	bool Document::RemoveObserver(DocumentObserver* observer)
	{
		if (auto it = std::find(begin(m_documentObservers), end(m_documentObservers), observer); it != end(m_documentObservers))
		{
			m_documentObservers.erase(it);
			return true;
		}
		return false;
	}

	bool Document::OnCreate(const mafString& WXUNUSED(path))
	{
		return true;
	}

	void Document::NotifyUpdate()
	{
		for (auto& observer : m_documentObservers)
		{
			observer->Update();
		}
	}

	void Document::NotifyClosing()
	{
		for (auto& observer : m_documentObservers)
		{
			observer->Closing();
		}
	}

	void Document::SetFilename(const mafString& filename, bool notifyViews)
	{
		m_documentFile = filename;
		OnChangeFilename(notifyViews);
	}

	void Document::OnChangeFilename(bool notifyViews)
	{
		if (notifyViews)
		{
			// Notify the views that the filename has changed
			for (auto& observer : m_documentObservers)
			{
				observer->ChangeFilename();
			}
		}
	}

	bool Document::DoSaveDocument(const mafString& file)
	{
		std::ofstream store(file.c_str(), std::ios::binary);
		if (!store)
		{
			wxLogError(_("File \"%s\" could not be opened for writing."), mafStringToWx(file));
			return false;
		}

		if (!SaveObject(store))
		{
			wxLogError(_("Failed to save document to the file \"%s\"."), mafStringToWx(file));
			return false;
		}

		return true;
	}

	bool Document::DoOpenDocument(const mafString& file)
	{
		std::ifstream store(file.c_str(), std::ios::binary);
		if (!store)
		{
			wxLogError(_("File \"%s\" could not be opened for reading."), mafStringToWx(file));
			return false;
		}

		LoadObject(store);
		if (!store)
		{
			wxLogError(_("Failed to read document from the file \"%s\"."), mafStringToWx(file));
			return false;
		}

		return true;
	}

	View::~View()
	{
		if (auto docManager = GetDocumentManager())
		{
			docManager->ActivateView(this, false);
			docManager->RemoveView(this);
		}

		// reset our frame view first, before removing it from the document as
		// SetView(nullptr) is a simple call while RemoveView() may result in user
		// code being executed and this user code can, for example, show a message
		// box which would result in an activation event for m_docChildFrame and so
		// could reactivate the view being destroyed -- unless we reset it first
		if (m_docChildFrame && m_docChildFrame->GetView() == this)
		{
			// prevent it from doing anything with us
			m_docChildFrame->SetView(nullptr);

			// it doesn't make sense to leave the frame alive if its associated
			// view doesn't exist any more so unconditionally close it as well
			//
			// notice that we only get here if m_docChildFrame is non-null in the
			// first place and it will be always nullptr if we're deleted because our
			// frame was closed, so this only catches the case of directly deleting
			// the view, as it happens if its creation fails in wxDocTemplate::
			// CreateView() for example
			m_docChildFrame->GetWindow()->Destroy();
		}

		if (m_viewDocument)
		{
			m_viewDocument->RemoveObserver(this);
		}
	}

	DocManager* View::GetDocumentManager() const
	{
		if (m_viewDocument)
			return DocManager::GetDocumentManager();
		return nullptr;
	}
	void View::SetDocChildFrame(ftkDocChildFrameAnyBase* docChildFrame)
	{
		SetFrame(docChildFrame ? docChildFrame->GetWindow() : nullptr);
		m_docChildFrame = docChildFrame;
	}

	bool View::TryBefore(wxEvent& event)
	{
		if (auto doc = GetDocument())
		{
			//return doc->ProcessEventLocally(event);
		}
		return false;
	}

	void View::OnActivateView(bool WXUNUSED(activate),
		View* WXUNUSED(activeView),
		View* WXUNUSED(deactiveView))
	{
	}

	void View::OnPrint(wxDC* dc, wxObject* WXUNUSED(info))
	{
		OnDraw(dc);
	}

	void View::OnUpdate(View* WXUNUSED(sender), wxObject* WXUNUSED(hint))
	{
		OnChangeFilename();
	}

	void View::OnChangeFilename()
	{
		// GetFrame can return wxWindow rather than wxTopLevelWindow due to
		// generic MDI implementation so use SetLabel rather than SetTitle.
		// It should cause SetTitle() for top level windows.
		auto win = GetFrame();
		if (!win)
		{
			return;
		}

		auto doc = GetDocument();
		if (!doc)
		{
			return;
		}

		mafString label = doc->GetUserReadableName();
		if (doc->IsModified())
		{
			label += _R("*");
		}
		win->SetLabel(mafStringToWx(label));
	}

	bool View::OnCreate(std::shared_ptr<Document> doc)
	{
		m_viewDocument = std::move(doc);
		if (m_viewDocument)
		{
			m_viewDocument->AddObserver(this);
		}
		return true;
	}

	bool View::Close(bool deleteWindow)
	{
		return OnClose(deleteWindow);
	}

	void View::Activate(bool activate)
	{
		if (auto docManager = GetDocumentManager())
		{
			OnActivateView(activate, this, docManager->GetCurrentView());
			docManager->ActivateView(this, activate);
		}
	}

	bool View::OnClose(bool WXUNUSED(deleteWindow))
	{
		/*if (auto doc = GetDocument())
		{
			if (doc->GetViews().size() == 1)
			{
				return doc->Close();
			}
		}*/
		return true;
	}

#if wxUSE_PRINTING_ARCHITECTURE
	wxPrintout* View::OnCreatePrintout()
	{
		return new ftkDocPrintout(this);
	}
#endif // wxUSE_PRINTING_ARCHITECTURE

	DocTemplate::DocTemplate(
		const mafString& description,
		const mafString& filter,
		const mafString& ext,
		const mafString& docTypeName,
		std::function<std::unique_ptr<Document>()> docCreate)
		: m_fileFilter(filter)
		, m_description(description)
		, m_defaultExt(ext)
		, m_docTypeName(docTypeName)
		, m_docCreate(docCreate)
	{
	}

	// Tries to dynamically construct an object of the right class.
	std::shared_ptr<Document> DocTemplate::CreateDocument(const mafString& path)
	{
		// InitDocument() is supposed to delete the document object if its
		// initialization fails so don't use unique_ptr<> here: this is fragile
		// but unavoidable because the default implementation uses CreateView()
		// which may -- or not -- create a wxView and if it does create it and its
		// initialization fails then the view destructor will delete the document
		// (via RemoveView()) and as we can't distinguish between the two cases we
		// just have to assume that it always deletes it in case of failure
		if (!m_docCreate)
		{
			return nullptr;
		}
		if (auto doc = m_docCreate())
		{
			doc->SetFilename(path);
			if (!doc->OnCreate(path))
				return nullptr;
			return std::move(doc);
		}
		return nullptr;
	}

	// The default (very primitive) format detection: check is the extension is
	// that of the template
	bool DocTemplate::FileMatchesTemplate(const mafString& path)
	{
		auto FindExtension = [](const wxString& path)
			{
				wxString ext;
				wxFileName::SplitPath(path, nullptr, nullptr, &ext);

				// VZ: extensions are considered not case sensitive - is this really a good
				//     idea?
				return ext.MakeLower();
			};
		wxStringTokenizer parser(mafStringToWx(GetFileFilter()), wxT(";"));
		wxString anything = wxT("*");
		while (parser.HasMoreTokens())
		{
			wxString filter = parser.GetNextToken();
			wxString filterExt = FindExtension(filter);
			if (filter.IsSameAs(anything) ||
				filterExt.IsSameAs(anything) ||
				filterExt.IsSameAs(FindExtension(mafStringToWx(path))))
				return true;
		}
		return mafStringToWx(GetDefaultExtension()).IsSameAs(FindExtension(mafStringToWx(path)));
	}

	ViewTemplate::ViewTemplate(const mafString& viewTypeName, std::function<std::unique_ptr<View>()> viewCreate)
		: m_viewTypeName(viewTypeName)
		, m_viewCreate(viewCreate)
	{
	}

	std::unique_ptr<View> ViewTemplate::CreateView(std::shared_ptr<Document> doc)
	{
		if (!m_viewCreate)
		{
			return nullptr;
		}
		auto view = m_viewCreate();
		if (!view || !view->OnCreate(doc))
		{
			return nullptr;
		}
		return std::move(view);
	}

	class App : public wxApp
	{
	public:
		App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		bool OnInit() override;
		int OnExit() override;

		void OnInitCmdLine(wxCmdLineParser& parser) override;
		bool OnCmdLineParsed(wxCmdLineParser& parser) override;

#ifdef __WXMAC__
		void MacNewFile() override;
#endif // __WXMAC__

		wxFrame* CreateMainFrame(FTK::DocManager* docManager);
		wxFrame* CreateChildFrame(FTK::View* view, bool isCanvas);
		MyCanvas* GetMainWindowCanvas() const { return nullptr; }

		FTK::View::Mode GetMode() const { return m_mode; }
	private:
		void CreateMenuBarForFrame(wxFrame* frame, wxMenu* file);

		FTK::View::Mode m_mode;
	};


DocManager* DocManager::sm_docManager = nullptr;

DocManager::DocManager()
{
	Bind(wxEVT_MENU, &DocManager::OnFileNew, this, wxID_NEW);
	Bind(wxEVT_MENU, &DocManager::OnFileOpen, this, wxID_OPEN);
	Bind(wxEVT_MENU, &DocManager::OnFileSave, this, wxID_SAVE);
	Bind(wxEVT_MENU, &DocManager::OnFileSaveAs, this, wxID_SAVEAS);
	Bind(wxEVT_MENU, &DocManager::OnFileClose, this, wxID_CLOSE);
	Bind(wxEVT_MENU, &DocManager::OnFileCloseAll, this, wxID_CLOSE_ALL);
	Bind(wxEVT_MENU, &DocManager::OnFileRevert, this, wxID_REVERT);
	Bind(wxEVT_MENU, &DocManager::OnUndo, this, wxID_UNDO);
	Bind(wxEVT_MENU, &DocManager::OnRedo, this, wxID_REDO);

	Bind(wxEVT_MENU, &DocManager::OnMRUFile, this, wxID_ANY);

	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateFileNew, this, wxID_NEW);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateFileOpen, this, wxID_OPEN);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateFileSave, this, wxID_SAVE);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateFileSaveAs, this, wxID_SAVEAS);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateDisableIfNoDoc, this, wxID_CLOSE);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateDisableIfNoDoc, this, wxID_CLOSE_ALL);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateFileRevert, this, wxID_REVERT);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateUndo, this, wxID_UNDO);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateRedo, this, wxID_REDO);

#if wxUSE_PRINTING_ARCHITECTURE
	Bind(wxEVT_MENU, &DocManager::OnPrint, this, wxID_PRINT);
	Bind(wxEVT_MENU, &DocManager::OnPreview, this, wxID_PREVIEW);
	Bind(wxEVT_MENU, &DocManager::OnPageSetup, this, wxID_PRINT_SETUP);
	// NB: we keep "Print setup" menu item always enabled as it can be used
	//     even without an active document
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateDisableIfNoDoc, this, wxID_PRINT);
	Bind(wxEVT_UPDATE_UI, &DocManager::OnUpdateDisableIfNoDoc, this, wxID_PREVIEW);
#endif // wxUSE_PRINTING_ARCHITECTURE 
	sm_docManager = this;
	m_fileHistory = std::make_unique<wxFileHistory>();
}

DocManager::~DocManager()
{
	Clear();
	m_fileHistory.reset();
	sm_docManager = nullptr;
}

// closes the specified document
bool DocManager::CloseDocument(FTK::Document* doc, bool force)
{
	if (force)
	{
		// We need to close, but at least ask the user if the document should
		// be saved before doing it.
		doc->OnSaveBeforeForceClose();
	}
	else // Allow the user to cancel closing too.
	{
		if (!doc->CanClose())
			return false;
	}

	// Note that by now the document is certain not to be modified any longer.

	// Implicitly deletes the document when
	// the last view is deleted
	//doc->DeleteAllViews_();
	{
		// first check if all views agree to be closed
		if (auto it = std::find_if(begin(m_documentsInfo), end(m_documentsInfo), [&](auto& elem) {return elem.first.get() == doc; }); it != end(m_documentsInfo))
		{
			auto& documentViews = it->second.m_views;
			if (std::all_of(begin(documentViews), end(documentViews), [](auto& v) {return v->Close();}))
			{
				// all views agreed to close, now do close them
				for (auto it = begin(documentViews); it != end(documentViews);)
				{
					delete* it++;
				}
			}
			m_documentsInfo.erase(it);
		}
	}
	return true;
}

bool DocManager::CloseDocuments(bool force)
{
	for (auto it = m_documentsInfo.begin(); it != m_documentsInfo.end();)
	{
		if (!CloseDocument((*it++).first.get(), force))
			return false;
		// This assumes that documents are not connected in
		// any way, i.e. deleting one document does NOT
		// delete another.
	}
	return true;
}

bool DocManager::Clear(bool force)
{
	if (!CloseDocuments(force))
		return false;

	m_currentView = nullptr;

	m_templates.clear();

	return true;
}

wxString DocManager::GetLastDirectory() const
{
	// if we haven't determined the last used directory yet, do it now
	if (m_lastDirectory.empty())
	{
		// we're going to modify m_lastDirectory in this const method, so do it
		// via non-const self pointer instead of const this one
		auto self = const_cast<DocManager*>(this);

		// first try to reuse the directory of the most recently opened file:
		// this ensures that if the user opens a file, closes the program and
		// runs it again the "Open file" dialog will open in the directory of
		// the last file he used
		if (m_fileHistory && m_fileHistory->GetCount())
		{
			const wxString lastOpened = m_fileHistory->GetHistoryFile(0);
			const wxFileName fn(lastOpened);
			if (fn.DirExists())
			{
				self->m_lastDirectory = fn.GetPath();
			}
			//else: should we try the next one?
		}
		//else: no history yet

		// if we don't have any files in the history (yet?), use the
		// system-dependent default location for the document files
		if (m_lastDirectory.empty())
		{
			self->m_lastDirectory = wxStandardPaths::Get().GetAppDocumentsDir();
		}
	}

	return m_lastDirectory;
}

void DocManager::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
	if (auto doc = GetCurrentDocument())
	{
		CloseDocument(doc);
	}
}

void DocManager::OnFileCloseAll(wxCommandEvent& WXUNUSED(event))
{
	CloseDocuments(false);
}

void DocManager::OnFileNew(wxCommandEvent& WXUNUSED(event))
{
	CreateNewDocument();
}

void DocManager::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
	if (!CreateDocument(wxString()))
	{
		OnOpenFileFailure();
	}
}

void DocManager::OnFileRevert(wxCommandEvent& WXUNUSED(event))
{
	if (auto doc = GetCurrentDocument())
	{
		if (wxMessageBox
		(
			_("Discard changes and reload the last saved version?"),
			wxTheApp->GetAppDisplayName(),
			wxYES_NO | wxCANCEL | wxICON_QUESTION,
			doc->GetDocumentWindow()
		) != wxYES)
			return;
		doc->Revert();
	}
}

void DocManager::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
	if (auto doc = GetCurrentDocument())
	{
		doc->Save();
	}
}

void DocManager::OnFileSaveAs(wxCommandEvent& WXUNUSED(event))
{
	if (auto doc = GetCurrentDocument())
	{
		doc->SaveAs();
	}
}

void DocManager::OnMRUFile(wxCommandEvent& event)
{
	if (m_fileHistory)
	{
		// Check if the id is in the range assigned to MRU list entries.
		const int id = event.GetId();
		if (id >= wxID_FILE1 &&
			id < wxID_FILE1 + static_cast<int>(m_fileHistory->GetCount()))
		{
			DoOpenMRUFile(id - wxID_FILE1);

			// Don't skip the event below.
			return;
		}
	}

	event.Skip();
}

void DocManager::DoOpenMRUFile(unsigned n)
{
	wxString filename(GetHistoryFile(n));
	if (filename.empty())
		return;

	if (wxFile::Exists(filename))
	{
		// Try to open it but don't give an error if it failed: this could be
		// normal, e.g. because the user cancelled opening it, and we don't
		// have any useful information to put in the error message anyhow, so
		// we assume that in case of an error the appropriate message had been
		// already logged.
		(void)CreateDocument(filename, wxDOC_SILENT);
	}
	else // file doesn't exist
	{
		OnMRUFileNotExist(n, filename);
	}
}

void DocManager::OnMRUFileNotExist(unsigned n, const wxString& filename)
{
	// remove the file which we can't open from the MRU list
	RemoveFileFromHistory(n);

	// and tell the user about it
	wxLogError(_("The file '%s' doesn't exist and couldn't be opened.\n"
		"It has been removed from the most recently used files list."),
		filename);
}

#if wxUSE_PRINTING_ARCHITECTURE

void DocManager::OnPrint(wxCommandEvent& WXUNUSED(event))
{
	auto view = GetAnyUsableView();
	if (!view)
		return;

	if (auto printout = view->OnCreatePrintout())
	{
		wxPrintDialogData printDialogData(m_pageSetupDialogData.GetPrintData());
		wxPrinter printer(&printDialogData);
		printer.Print(view->GetFrame(), printout, true);

		delete printout;
	}
}

void DocManager::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
	wxPageSetupDialog dlg(wxTheApp->GetTopWindow(), &m_pageSetupDialogData);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_pageSetupDialogData = dlg.GetPageSetupData();
	}
}

wxPreviewFrame* DocManager::CreatePreviewFrame(wxPrintPreviewBase* preview,
	wxWindow* parent,
	const wxString& title)
{
	return new wxPreviewFrame(preview, parent, title);
}

void DocManager::OnPreview(wxCommandEvent& WXUNUSED(event))
{
	wxBusyCursor busy;
	auto view = GetAnyUsableView();
	if (!view)
		return;

	if (auto printout = view->OnCreatePrintout())
	{
		wxPrintDialogData printDialogData(m_pageSetupDialogData.GetPrintData());

		// Pass two printout objects: for preview, and possible printing.
		wxPrintPreviewBase*
			preview = new wxPrintPreview(printout,
				view->OnCreatePrintout(),
				&printDialogData);
		if (!preview->IsOk())
		{
			delete preview;
			wxLogError(_("Print preview creation failed."));
			return;
		}

		wxPreviewFrame* frame = CreatePreviewFrame(preview,
			wxTheApp->GetTopWindow(),
			_("Print Preview"));
		wxCHECK_RET(frame, "should create a print preview frame");

		frame->Centre(wxBOTH);
		frame->Initialize();
		frame->Show(true);
	}
}
#endif // wxUSE_PRINTING_ARCHITECTURE

void DocManager::OnUndo(wxCommandEvent& event)
{
	auto cmdproc = GetCurrentCommandProcessor();
	if (!cmdproc)
	{
		event.Skip();
		return;
	}

	if (cmdproc->Undo())
	{
		cmdproc->SetMenuStrings_();
	}
	GetCurrentDocument()->NotifyUpdate();
}

void DocManager::OnRedo(wxCommandEvent& event)
{
	auto cmdproc = GetCurrentCommandProcessor();
	if (!cmdproc)
	{
		event.Skip();
		return;
	}

	if (cmdproc->Redo())
	{
		cmdproc->SetMenuStrings_();
	}
	GetCurrentDocument()->NotifyUpdate();
}

// Handlers for UI update commands

void DocManager::OnUpdateFileOpen(wxUpdateUIEvent& event)
{
	// CreateDocument() (which is called from OnFileOpen) may succeed
	// only when there is at least a template:
	event.Enable(!GetTemplates().empty());
}

void DocManager::OnUpdateDisableIfNoDoc(wxUpdateUIEvent& event)
{
	event.Enable(GetCurrentDocument() != nullptr);
}

void DocManager::OnUpdateFileRevert(wxUpdateUIEvent& event)
{
	auto doc = GetCurrentDocument();
	event.Enable(doc && doc->IsModified() && doc->GetDocumentSaved());
}

void DocManager::OnUpdateFileNew(wxUpdateUIEvent& event)
{
	// CreateDocument() (which is called from OnFileNew) may succeed
	// only when there is at least a template:
	event.Enable(!GetTemplates().empty());
}

void DocManager::OnUpdateFileSave(wxUpdateUIEvent& event)
{
	auto doc = GetCurrentDocument();
	event.Enable(doc && !doc->AlreadySaved());
}

void DocManager::OnUpdateFileSaveAs(wxUpdateUIEvent& event)
{
	auto const doc = GetCurrentDocument();
	event.Enable(doc);
}

void DocManager::OnUpdateUndo(wxUpdateUIEvent& event)
{
	auto cmdproc = GetCurrentCommandProcessor();
	if (!cmdproc)
	{
		// If we don't have any document at all, the menu item should really be
		// disabled.
		if (!GetCurrentDocument())
			event.Enable(false);
		else // But if we do have it, it might handle wxID_UNDO on its own
			event.Skip();
		return;
	}
	event.Enable(cmdproc->CanUndo());
	cmdproc->SetMenuStrings_();
}

void DocManager::OnUpdateRedo(wxUpdateUIEvent& event)
{
	auto cmdproc = GetCurrentCommandProcessor();
	if (!cmdproc)
	{
		// Use same logic as in OnUpdateUndo() above.
		if (!GetCurrentDocument())
			event.Enable(false);
		else
			event.Skip();
		return;
	}
	event.Enable(cmdproc->CanRedo());
	cmdproc->SetMenuStrings_();
}

FTK::View* DocManager::GetAnyUsableView() const
{
	auto view = GetCurrentView();

	if (!view && !m_documentsInfo.empty())
	{
		// if we have exactly one document, consider its view to be the current
		// one
		//
		// VZ: I'm not exactly sure why is this needed but this is how this
		//     code used to behave before the bug #9518 was fixed and it seems
		//     safer to preserve the old logic
		if (m_documentsInfo.size() == 1)
		{
			auto& info = m_documentsInfo.front().second;
			if (!info.m_views.empty())
				view = info.m_views.front();
		}
		//else: we have more than one document
	}

	return view;
}

bool DocManager::TryBefore(wxEvent& event)
{
	auto view = GetAnyUsableView();
	return view && view->ProcessEventLocally(event);
}

std::shared_ptr<FTK::Document> DocManager::FindDocumentByPath(const wxString& path) const
{
	const wxFileName fileName(path);
	for (auto& docInfo : m_documentsInfo)
	{
		if (fileName == wxFileName(mafStringToWx(docInfo.first->GetFilename())))
			return docInfo.first;
	}
	return nullptr;
}

std::shared_ptr<FTK::Document> DocManager::CreateDocument(const wxString& pathOrig, long flags)
{
	if (m_templates.empty())
	{
		return nullptr;
	}

	// normally user should select the template to use but wxDOC_SILENT flag we
	// choose one ourselves
	wxString path = pathOrig;   // may be modified below
	std::shared_ptr<FTK::DocViewTemplate> temp;
	if (flags & wxDOC_SILENT)
	{
		wxASSERT_MSG(!path.empty(),"using empty path with wxDOC_SILENT doesn't make sense");

		temp = FindTemplateForPath(path);
		if (!temp)
		{
			wxLogWarning(_("The format of file '%s' couldn't be determined."), path);
		}
	}
	else // not silent, ask the user
	{
		// for the new file we need just the template, for an existing one we
		// need the template and the path, unless it's already specified
		if ((flags & wxDOC_NEW) || !path.empty())
			temp = SelectDocumentType();
		else
			temp = SelectDocumentPath(path, flags);
	}

	if (!temp)
		return nullptr;

	// check whether the document with this path is already opened
	if (!path.empty())
	{
		if (auto doc = FindDocumentByPath(path))
		{
			// file already open, just activate it and return
			//doc->Activate();
			auto infoIt = std::find_if(begin(m_documentsInfo), end(m_documentsInfo), [&](auto& elem) {return elem.first == doc; });
			if (infoIt != end(m_documentsInfo) && !infoIt->second.m_views.empty())
			{
				auto view = infoIt->second.m_views.front();
				view->Activate(true);
				if (auto win = view->GetFrame())
					win->Raise();
			}

			return doc;
		}
	}

	// no, we need to create a new document

	// if we've reached the max number of docs, close the first one.
	if ((int)m_documentsInfo.size() >= m_maxDocsOpen)
	{
		if (!CloseDocument(m_documentsInfo.front().first.get()))
		{
			// can't open the new document if closing the old one failed
			return nullptr;
		}
	}

	// do create and initialize the new document finally
	auto docNew = temp->CreateDocument(mafWxToString(path));
	if (!docNew)
	{
		return nullptr;
	}
	docNew->SetDocTemplate(temp);
	m_documentsInfo.emplace_back(docNew, DocInfo{});
	m_documentsInfo.back().second.m_commandProcessor = std::make_unique<FTK::CommandProcessor>();

	// call the appropriate function depending on whether we're creating a
	// new file or opening an existing one
	if (!(flags & wxDOC_NEW ? docNew->OnNewDocument(mafWxToString(MakeNewDocumentName()))
		: docNew->OnOpenDocument(mafWxToString(path))))
	{
		return nullptr;
	}

	// add the successfully opened file to MRU, but only if we're going to be
	// able to reopen it successfully later which requires the template for
	// this document to be retrievable from the file extension
	if (!(flags & wxDOC_NEW) && temp->FileMatchesTemplate(mafWxToString(path)))
		AddFileToHistory(path);

	auto v1 = temp->CreateView(docNew);
	auto v2 = temp->CreateView(docNew);
	v1->OnChangeFilename();
	v2->OnChangeFilename();
	m_documentsInfo.back().second.m_views.push_back(v1.release());
	m_documentsInfo.back().second.m_views.push_back(v2.release());

	// at least under Mac (where views are top level windows) it seems to be
	// necessary to manually activate the new document to bring it to the
	// forefront -- and it shouldn't hurt doing this under the other platforms
	//docNew->Activate();
	auto& info = m_documentsInfo.back();
	if (!info.second.m_views.empty())
	{
		auto view = info.second.m_views.front();
		view->Activate(true);
		if (auto win = view->GetFrame())
			win->Raise();
	}

	return docNew;
}

std::unique_ptr<FTK::View> DocManager::CreateView(std::shared_ptr<FTK::Document> doc)
{
	if (m_templates.empty() == 0)
		return nullptr;

	std::shared_ptr<FTK::DocViewTemplate>
		temp = m_templates.size() == 1 ? m_templates.front()
		: SelectViewType();

	if (!temp)
		return nullptr;

	auto view = temp->CreateView(doc);
	if (view)
		view->SetViewName(temp->GetViewName());
	return view;
}

bool DocManager::AddView(FTK::View* view)
{
	/*if (std::find(begin(m_documentViews), end(m_documentViews), view) == end(m_documentViews))
	{
		m_documentViews.push_back(view);
		OnChangedViewList();
	}*/
	return true;
}

bool DocManager::RemoveView(FTK::View* view)
{
	auto doc = view->GetDocument();
	if (auto it = std::find_if(begin(m_documentsInfo), end(m_documentsInfo), [&](auto& elem) {return elem.first.get() == doc; }); it != end(m_documentsInfo))
	{
		auto& documentViews = it->second.m_views;
		auto viewListIt = std::find(begin(documentViews), end(documentViews), view);
		documentViews.erase(viewListIt);
	}
	return false;
}

// Not yet implemented
void DocManager::DeleteTemplate(FTK::DocViewTemplate* WXUNUSED(temp), long WXUNUSED(flags))
{
}

// Not yet implemented
bool DocManager::FlushDoc(FTK::Document* WXUNUSED(doc))
{
	return false;
}

FTK::Document* DocManager::GetCurrentDocument() const
{
	auto view = GetAnyUsableView();
	return view ? view->GetDocument() : nullptr;
}

FTK::CommandProcessor* DocManager::GetCommandProcessor(FTK::Document* doc) const
{
	if (auto it = std::find_if(begin(m_documentsInfo), end(m_documentsInfo), [&](auto& elem) {return elem.first.get() == doc; }); it != end(m_documentsInfo))
	{
		return it->second.m_commandProcessor.get();
	}
	return nullptr;
}

FTK::CommandProcessor* DocManager::GetCurrentCommandProcessor() const
{
	auto doc = GetCurrentDocument();
	return doc ? GetCommandProcessor(doc) : nullptr;
}

// Make a default name for a new document
wxString DocManager::MakeNewDocumentName()
{
	wxString name;

	name.Printf(_("unnamed%d"), m_defaultDocumentNameCounter);
	m_defaultDocumentNameCounter++;

	return name;
}

// Make a frame title (override this to do something different)
// If docName is empty, a document is not currently active.
wxString DocManager::MakeFrameTitle(FTK::Document* doc)
{
	wxString appName = wxTheApp->GetAppDisplayName();
	wxString title;
	if (!doc)
		title = appName;
	else
	{
		wxString docName = mafStringToWx(doc->GetUserReadableName());
		title = docName + wxString(_(" - ")) + appName;
	}
	return title;
}

// Not yet implemented
std::shared_ptr<FTK::DocViewTemplate> DocManager::MatchTemplate(const wxString& WXUNUSED(path))
{
	return nullptr;
}

// File history management
void DocManager::AddFileToHistory(const wxString& file)
{
	if (m_fileHistory)
		m_fileHistory->AddFileToHistory(file);
}

void DocManager::RemoveFileFromHistory(size_t i)
{
	if (m_fileHistory)
		m_fileHistory->RemoveFileFromHistory(i);
}

wxString DocManager::GetHistoryFile(size_t i) const
{
	wxString histFile;

	if (m_fileHistory)
		histFile = m_fileHistory->GetHistoryFile(i);

	return histFile;
}

void DocManager::FileHistoryUseMenu(wxMenu* menu)
{
	if (m_fileHistory)
		m_fileHistory->UseMenu(menu);
}

void DocManager::FileHistoryRemoveMenu(wxMenu* menu)
{
	if (m_fileHistory)
		m_fileHistory->RemoveMenu(menu);
}

#if wxUSE_CONFIG
void DocManager::FileHistoryLoad(const wxConfigBase& config)
{
	if (m_fileHistory)
		m_fileHistory->Load(config);
}

void DocManager::FileHistorySave(wxConfigBase& config)
{
	if (m_fileHistory)
		m_fileHistory->Save(config);
}
#endif

void DocManager::FileHistoryAddFilesToMenu(wxMenu* menu)
{
	if (m_fileHistory)
		m_fileHistory->AddFilesToMenu(menu);
}

void DocManager::FileHistoryAddFilesToMenu()
{
	if (m_fileHistory)
		m_fileHistory->AddFilesToMenu();
}

size_t DocManager::GetHistoryFilesCount() const
{
	return m_fileHistory ? m_fileHistory->GetCount() : 0;
}


// Find out the document template via matching in the document file format
// against that of the template
std::shared_ptr<FTK::DocViewTemplate> DocManager::FindTemplateForPath(const wxString& path)
{
	// Find the template which this extension corresponds to
	for (auto& temp : m_templates)
	{
		if (temp->FileMatchesTemplate(mafWxToString(path)))
		{
			return temp;
		}
	}
	return nullptr;
}

// Prompts user to open a file, using file specs in templates.
// Must extend the file selector dialog or implement own; OR
// match the extension to the template extension.

std::shared_ptr<FTK::DocViewTemplate> DocManager::SelectDocumentPath(wxString& path, long WXUNUSED(flags), bool WXUNUSED(save))
{
#ifdef wxHAS_MULTIPLE_FILEDLG_FILTERS
	wxString descrBuf;

	for (auto& temp : m_templates)
	{
		// add a '|' to separate this filter from the previous one
		if (!descrBuf.empty())
			descrBuf << wxT('|');

		descrBuf << mafStringToWx(temp->GetDescription())
			<< wxT(" (") << mafStringToWx(temp->GetFileFilter()) << wxT(") |")
			<< mafStringToWx(temp->GetFileFilter());
	}
#else
	wxString descrBuf = wxT("*.*");
	wxUnusedVar(noTemplates);
#endif

	int FilterIndex = -1;

	wxString pathTmp = wxFileSelectorEx(_("Open File"),
		GetLastDirectory(),
		wxEmptyString,
		&FilterIndex,
		descrBuf,
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	std::shared_ptr<FTK::DocViewTemplate> theTemplate;
	if (!pathTmp.empty())
	{
		if (!wxFileExists(pathTmp))
		{
			wxString msgTitle;
			if (!wxTheApp->GetAppDisplayName().empty())
				msgTitle = wxTheApp->GetAppDisplayName();
			else
				msgTitle = wxString(_("File error"));

			wxMessageBox(_("Sorry, could not open this file."),
				msgTitle,
				wxOK | wxICON_EXCLAMATION | wxCENTRE);

			path.clear();
			return nullptr;
		}

		SetLastDirectory(wxPathOnly(pathTmp));

		path = pathTmp;

		// first choose the template using the extension, if this fails (i.e.
		// wxFileSelectorEx() didn't fill it), then use the path
		if (FilterIndex != -1)
		{
			theTemplate = m_templates[FilterIndex];
			if (theTemplate)
			{
				// But don't use this template if it doesn't match the path as
				// can happen if the user specified the extension explicitly
				// but didn't bother changing the filter.
				if (!theTemplate->FileMatchesTemplate(mafWxToString(path)))
					theTemplate = nullptr;
			}
		}

		if (!theTemplate)
			theTemplate = FindTemplateForPath(path);
		if (!theTemplate)
		{
			// Since we do not add files with non-default extensions to the
			// file history this can only happen if the application changes the
			// allowed templates in runtime.
			wxMessageBox(_("Sorry, the format for this file is unknown."),
				_("Open File"),
				wxOK | wxICON_EXCLAMATION | wxCENTRE);
		}
	}
	else
	{
		path.clear();
	}

	return theTemplate;
}

std::shared_ptr<FTK::DocViewTemplate> DocManager::SelectDocumentType()
{
	std::vector<wxString> strings;
	std::vector<std::shared_ptr<FTK::DocViewTemplate> > data;

	for (auto& templ : m_templates)
	{
		data.push_back(templ);
	}
	std::sort(begin(data), end(data), 
		[](auto& p1, auto& p2)
		{
			return std::tie(p1->GetDocumentName(), p1->GetViewName()) < std::tie(p2->GetDocumentName(), p2->GetViewName());
		}
	);
	auto lastIt = std::unique(begin(data), end(data), 
		[](auto& p1, auto& p2)
		{
			return std::tie(p1->GetDocumentName(), p1->GetViewName()) == std::tie(p2->GetDocumentName(), p2->GetViewName());
		}
	);
	data.erase(lastIt, end(data));
	for (auto& templ : data)
	{
		strings.push_back(mafStringToWx(templ->GetDescription()));
	}

	if (data.empty())
	{
		return nullptr;
	}
	if (data.size() == 1)
	{
		return data.front();
	}
	// propose the user to choose one of several
	if (int idx = wxGetSingleChoiceIndex(_("Select a document template"), _("Templates"), strings); idx != -1)
	{
		return data[idx];
	}
	return nullptr;
}

std::shared_ptr<FTK::DocViewTemplate> DocManager::SelectViewType()
{
	std::vector<wxString> strings;
	std::vector<std::shared_ptr<FTK::DocViewTemplate>> data;

	for (auto& templ : m_templates)
	{
		if (!templ->GetViewName().empty())
		{
			if (std::find_if(begin(data), end(data), [&](auto& t) {return templ->GetViewName() == t->GetViewName(); }) == end(data))
			{
				strings.push_back(mafStringToWx(templ->GetViewName()));
				data.push_back(templ);
			}
		}
	}

	if (data.empty())
	{
		return nullptr;
	}
	if (data.size() == 1)
	{
		return data.front();
	}
	// the same logic as above
	if (int idx = wxGetSingleChoiceIndex(_("Select a document view"), _("Views"), strings); idx != -1)
	{
		return data[idx];
	}
	return nullptr;
}

void DocManager::AssociateTemplate(std::shared_ptr<FTK::DocViewTemplate> temp)
{
	if (std::find(begin(m_templates), end(m_templates), temp) == end(m_templates))
	{
		m_templates.push_back(std::move(temp));
	}
}

void DocManager::DisassociateTemplate(FTK::DocViewTemplate* temp)
{
	if (auto it = std::find_if(begin(m_templates), end(m_templates), [temp](auto& elem) {return elem.get() == temp; }); it != end(m_templates))
	{
		m_templates.erase(it);
	}
}

// Views or windows should inform the document manager
// when a view is going in or out of focus
void DocManager::ActivateView(FTK::View* view, bool activate)
{
	if (activate)
	{
		m_currentView = view;
	}
	else // deactivate
	{
		if (m_currentView == view)
		{
			// don't keep stale pointer
			m_currentView = nullptr;
		}
	}
}
}

bool ftkDocChildFrameAnyBase::TryProcessEvent(wxEvent& event)
{
	if (!m_childView)
	{
		// We must be being destroyed, don't forward events anywhere as
		// m_childDocument could be invalid by now.
		return false;
	}

	// Store a (non-owning) pointer to the last processed event here to be able
	// to recognize this event again if it bubbles up to the parent frame, see
	// the code in wxDocParentFrameAnyBase::TryProcessEvent().
	m_lastEvent = &event;

	// Forward the event to the document manager which will, in turn, forward
	// it to its active view which must be our m_childView.
	//
	// Notice that we do things in this roundabout way to guarantee the correct
	// event handlers call order: first the document, then the view and then the
	// document manager itself. And if we forwarded the event directly to the
	// view, then the document manager would do it once again when we forwarded
	// it to it.
	//return m_childDocument->GetDocumentManager()->ProcessEventLocally(event);
	return FTK::DocManager::GetDocumentManager()->ProcessEventLocally(event);
}

bool ftkDocChildFrameAnyBase::CloseView(wxCloseEvent& event)
{
	if (m_childView)
	{
		// notice that we must call wxView::Close() and OnClose() called from
		// it in any case, even if we know that we are going to close anyhow
		if (!m_childView->Close(false) && event.CanVeto())
		{
			event.Veto();
			return false;
		}

		m_childView->Activate(false);

		// it is important to reset m_childView frame pointer to nullptr before
		// deleting it because while normally it is the frame which deletes the
		// view when it's closed, the view also closes the frame if it is
		// deleted directly not by us as indicated by its doc child frame
		// pointer still being set
		m_childView->SetDocChildFrame(nullptr);
		delete m_childView;
		m_childView = nullptr;
	}

	return true;
}

bool ftkDocParentFrameAnyBase::TryProcessEvent(wxEvent& event)
{
	if (!m_docManager)
		return false;

	// If we have an active view, its associated child frame may have
	// already forwarded the event to wxDocManager, check for this:
	if (auto view = m_docManager->GetAnyUsableView())
	{
		if (auto childFrame = view->GetDocChildFrame(); childFrame && childFrame->HasAlreadyProcessed(event))
			return false;
	}

	// But forward the event to wxDocManager ourselves if there are no views at
	// all or if this event hadn't been sent to the child frame previously.
	return m_docManager->ProcessEventLocally(event);
}

// ----------------------------------------------------------------------------
// Printing support
// ----------------------------------------------------------------------------

#if wxUSE_PRINTING_ARCHITECTURE

wxString ftkDocPrintout::GetAppropriateTitle(const FTK::View* view, const wxString& titleGiven)
{
	wxString title(titleGiven);
	if (title.empty())
	{
		if (view && view->GetDocument())
			title = mafStringToWx(view->GetDocument()->GetUserReadableName());
		else
			title = _("Printout");
	}

	return title;
}

ftkDocPrintout::ftkDocPrintout(FTK::View* view, const wxString& title)
	: wxPrintout(GetAppropriateTitle(view, title))
{
	m_printoutView = view;
}

bool ftkDocPrintout::OnPrintPage(int WXUNUSED(page))
{
	wxDC* dc = GetDC();

	// Get the logical pixels per inch of screen and printer
	int ppiScreenX, ppiScreenY;
	GetPPIScreen(&ppiScreenX, &ppiScreenY);
	wxUnusedVar(ppiScreenY);
	int ppiPrinterX, ppiPrinterY;
	GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
	wxUnusedVar(ppiPrinterY);

	// This scales the DC so that the printout roughly represents
	// the screen scaling. The text point size _should_ be the right size
	// but in fact is too small for some reason. This is a detail that will
	// need to be addressed at some point but can be fudged for the
	// moment.
	double scale = double(ppiPrinterX) / ppiScreenX;

	// Now we have to check in case our real page size is reduced
	// (e.g. because we're drawing to a print preview memory DC)
	int pageWidth, pageHeight;
	int w, h;
	dc->GetSize(&w, &h);
	GetPageSizePixels(&pageWidth, &pageHeight);
	wxUnusedVar(pageHeight);

	// If printer pageWidth == current DC width, then this doesn't
	// change. But w might be the preview bitmap width, so scale down.
	double overallScale = scale * w / pageWidth;
	dc->SetUserScale(overallScale, overallScale);

	if (m_printoutView)
	{
		m_printoutView->OnDraw(dc);
	}
	return true;
}

bool ftkDocPrintout::HasPage(int pageNum)
{
	return (pageNum == 1);
}

bool ftkDocPrintout::OnBeginDocument(int startPage, int endPage)
{
	if (!wxPrintout::OnBeginDocument(startPage, endPage))
		return false;

	return true;
}

void ftkDocPrintout::GetPageInfo(int* minPage, int* maxPage,
	int* selPageFrom, int* selPageTo)
{
	*minPage = 1;
	*maxPage = 1;
	*selPageFrom = 1;
	*selPageTo = 1;
}

#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// Permits compatibility with existing file formats and functions that
// manipulate files directly
// ----------------------------------------------------------------------------

bool wxTransferFileToStream(const wxString& filename, std::ostream& stream)
{
#if wxUSE_FFILE
	wxFFile file(filename, wxT("rb"));
#elif wxUSE_FILE
	wxFile file(filename, wxFile::read);
#endif
	if (!file.IsOpened())
		return false;

	do
	{
		char buf[4096];
		size_t nRead;
		nRead = file.Read(buf, WXSIZEOF(buf));
		if (file.Error())
			return false;

		stream.write(buf, nRead);
		if (!stream)
			return false;
	} while (!file.Eof());

	return true;
}

bool wxTransferStreamToFile(std::istream& stream, const wxString& filename)
{
#if wxUSE_FFILE
	wxFFile file(filename, wxT("wb"));
#elif wxUSE_FILE
	wxFile file(filename, wxFile::write);
#endif
	if (!file.IsOpened())
		return false;

	char buf[4096];
	do
	{
		stream.read(buf, WXSIZEOF(buf));
		if (!stream.bad()) // fail may be set on EOF, don't use operator!()
		{
			if (!file.Write(buf, stream.gcount()))
				return false;
		}
	} while (!stream.eof());

	return true;
}

// Define a new application
class App : public wxApp
{
public:

	App();
	App(const App&) = delete;
	App& operator=(const App&) = delete;

	// override some wxApp virtual methods
	bool OnInit() override;
	int OnExit() override;

	void OnInitCmdLine(wxCmdLineParser& parser) override;
	bool OnCmdLineParsed(wxCmdLineParser& parser) override;

#ifdef __WXMAC__
	void MacNewFile() override;
#endif // __WXMAC__

	// our specific methods
	wxFrame* CreateChildFrame(FTK::View* view, bool isCanvas);

	// these accessors should only be called in single document mode, otherwise
	// the pointers are null and an assert is triggered
	MyCanvas* GetMainWindowCanvas() const
	{
		wxASSERT(m_canvas); return m_canvas;
	}
	wxMenu* GetMainWindowEditMenu() const
	{
		wxASSERT(m_menuEdit); return m_menuEdit;
	}

	FTK::View::Mode GetMode() const { return m_mode; }

private:
	// append the standard document-oriented menu commands to this menu
	void AppendDocumentFileCommands(wxMenu* menu, bool supportsPrinting);

	// create the edit menu for drawing documents
	wxMenu* CreateDrawingEditMenu();

	// create and associate with the given frame the menu bar containing the
	// given file and edit (possibly null) menus as well as the standard help
	// one
	void CreateMenuBarForFrame(wxFrame* frame, wxMenu* file, wxMenu* edit);

	// force close all windows
	void OnForceCloseAll(wxCommandEvent& event);

	// show the about box: as we can have different frames it's more
	// convenient, even if somewhat less usual, to handle this in the
	// application object itself
	void OnAbout(wxCommandEvent& event);

	// contains the file names given on the command line, possibly empty
	std::vector<wxString> m_filesFromCmdLine;

	// only used if m_mode == Mode_Single
	MyCanvas* m_canvas = nullptr;
	wxMenu* m_menuEdit = nullptr;

	FTK::View::Mode m_mode;
};

wxDECLARE_APP(App);
//wxDECLARE_APP(FTK::App);
//wxIMPLEMENT_APP(FTK::App);

namespace FTK
{
	App::App()
	{
#ifdef WIN32
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF) | _CRTDBG_LEAK_CHECK_DF);
#endif
		SetAppName("ftkApp");
		SetAppDisplayName("ftkApp");
#if wxUSE_MDI_ARCHITECTURE
		m_mode = FTK::View::Mode::Mode_MDI;
#else
		m_mode = FTK::View::Mode::Mode_SDI;
#endif

		/*m_canvas = nullptr;
		m_menuEdit = nullptr;*/
	}

	bool App::OnInit()
	{
		if (!wxApp::OnInit())
			return false;
		::wxInitAllImageHandlers();

		/*auto frame = wxw::CreateFrame(GetAppDisplayName());

		wxMenu* file = new wxMenu;

		file->Append(wxID_NEW);
		file->Append(wxID_OPEN);

		//if (m_mode == Mode_Single)
			//AppendDocumentFileCommands(menuFile, true);

		file->AppendSeparator();
		file->Append(wxID_EXIT);

		wxMenuBar* menubar = new wxMenuBar;

		menubar->Append(file, wxGetStockLabel(wxID_FILE));
		wxMenu* help = new wxMenu;
		help->Append(wxID_ABOUT);
		menubar->Append(help, wxGetStockLabel(wxID_HELP));

		frame->SetMenuBar(menubar);

		frame->Show();
		//CreateView();
		return true;*/

		// Fill in the application information fields before creating wxConfig.
		SetVendorName("ftkApp");
		SetAppName("ftkApp");
		SetAppDisplayName("ftkApp");

		auto docManager = new FTK::DocManager;

		auto frame = CreateMainFrame(docManager);

		// and its menu bar
		auto menuFile = new wxMenu;

		menuFile->Append(wxID_NEW);
		menuFile->Append(wxID_OPEN);

		/*if (m_mode == FTK::View::Mode::Mode_Single)
			AppendDocumentFileCommands(menuFile, true);*/

		menuFile->AppendSeparator();
		menuFile->Append(wxID_EXIT);

		// A nice touch: a history of files visited. Use this menu.
		docManager->FileHistoryUseMenu(menuFile);
#if wxUSE_CONFIG
		docManager->FileHistoryLoad(*wxConfig::Get());
#endif // wxUSE_CONFIG


		CreateMenuBarForFrame(frame, menuFile);

		frame->SetIcon(wxICON(doc));
		frame->Centre();
		frame->Show();

		return true;
	}

	int App::OnExit()
	{
		auto manager = FTK::DocManager::GetDocumentManager();
#if wxUSE_CONFIG
		manager->FileHistorySave(*wxConfig::Get());
#endif // wxUSE_CONFIG
		delete manager;
		return wxApp::OnExit();
	}

	void App::OnInitCmdLine(wxCmdLineParser& parser)
	{
		wxApp::OnInitCmdLine(parser);
#if wxUSE_MDI_ARCHITECTURE
		parser.AddSwitch("", CmdLineOption::MDI, "run in MDI mode: multiple documents, single window");
#endif
#if wxUSE_AUI
		parser.AddSwitch("", CmdLineOption::AUI, "run in MDI mode using AUI: multiple documents, single window");
#endif
		parser.AddSwitch("", CmdLineOption::SDI, "run in SDI mode: multiple documents, multiple windows");
		parser.AddSwitch("", CmdLineOption::SINGLE, "run in single document mode");

		parser.AddParam("filename", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL);
	}

	bool App::OnCmdLineParsed(wxCmdLineParser& parser)
	{
		int numModeOptions = 0;
		auto m_mode = FTK::View::Mode::Mode_MDI;

#if wxUSE_MDI_ARCHITECTURE
		if (parser.Found(CmdLineOption::MDI))
		{
			m_mode = FTK::View::Mode::Mode_MDI;
			numModeOptions++;
		}
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
		if (parser.Found(CmdLineOption::AUI))
		{
			m_mode = FTK::View::Mode::Mode_AUI;
			numModeOptions++;
		}
#endif // wxUSE_AUI

		if (parser.Found(CmdLineOption::SDI))
		{
			m_mode = FTK::View::Mode::Mode_SDI;
			numModeOptions++;
		}

		if (parser.Found(CmdLineOption::SINGLE))
		{
			m_mode = FTK::View::Mode::Mode_Single;
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
			//m_filesFromCmdLine.push_back(parser.GetParam(i));
		return wxApp::OnCmdLineParsed(parser);
	}

#ifdef __WXMAC__
	void App::MacNewFile()
	{
		wxApp::MacNewFile();
	}
#endif // __WXMAC__

	wxFrame* App::CreateMainFrame(FTK::DocManager* docManager)
	{
		switch (m_mode)
		{
#if wxUSE_MDI_ARCHITECTURE
		case FTK::View::Mode::Mode_MDI:
			return new ftkDocParentFrameAny<wxMDIParentFrame>(docManager, nullptr, wxID_ANY,
				GetAppDisplayName(),
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(1280, 720), nullptr));
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
		case FTK::View::Mode::Mode_AUI:
			return new ftkDocParentFrameAny<wxAuiMDIParentFrame>
				(
					docManager, nullptr, wxID_ANY,
					GetAppDisplayName(),
					wxDefaultPosition,
					wxWindow::FromDIP(wxSize(1280, 720), nullptr)
				);
#endif // wxUSE_AUI

		case FTK::View::Mode::Mode_SDI:
		case FTK::View::Mode::Mode_Single:
			return new ftkDocParentFrameAny<wxFrame>(docManager, nullptr, wxID_ANY,
				GetAppDisplayName(),
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(1280, 720), nullptr));
		}
		return nullptr;
	}

	wxFrame* App::CreateChildFrame(FTK::View* view, bool isCanvas)
	{
		// create a child frame of appropriate class for the current mode
		wxFrame* subframe = nullptr;
		auto doc = view->GetDocument();
		switch (m_mode)
#if wxUSE_MDI_ARCHITECTURE
		{
		case FTK::View::Mode::Mode_MDI:
			subframe = new ftkDocChildFrameAny<wxMDIChildFrame, wxMDIParentFrame>
			(
				view,
				static_cast<ftkDocParentFrameAny<wxMDIParentFrame>*>(GetTopWindow()),
				wxID_ANY,
				"Child Frame",
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(640, 480), nullptr)
			);
			break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
		case FTK::View::Mode::Mode_AUI:
			subframe = new ftkDocChildFrameAny<wxAuiMDIChildFrame, wxAuiMDIParentFrame>
				(
					view,
					static_cast<ftkDocParentFrameAny<wxAuiMDIParentFrame>*>(GetTopWindow()),
					wxID_ANY,
					"Child Frame",
					wxDefaultPosition,
					wxWindow::FromDIP(wxSize(640, 480), nullptr)
				);
			break;
#endif // wxUSE_AUI

		case FTK::View::Mode::Mode_SDI:
		case FTK::View::Mode::Mode_Single:
			subframe = new ftkDocChildFrameAny<wxFrame, wxFrame>
			(
				view,
				static_cast<ftkDocParentFrameAny<wxFrame>* >(GetTopWindow()),
				wxID_ANY,
				"Child Frame",
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(640, 480), nullptr)
			);

			subframe->Centre();
			break;
		}

		auto menuFile = new wxMenu;

		menuFile->Append(wxID_NEW);
		menuFile->Append(wxID_OPEN);
		//AppendDocumentFileCommands(menuFile, isCanvas);
		menuFile->AppendSeparator();
		menuFile->Append(wxID_EXIT);

		wxMenu* menuEdit = nullptr;
		if (isCanvas)
		{
			//menuEdit = CreateDrawingEditMenu();

			FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc)->SetEditMenu_(menuEdit);
			FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc)->SetMenuStrings_();// Initialize_();
		}
		else // text frame
		{
			menuEdit = new wxMenu;
			menuEdit->Append(wxID_COPY);
			menuEdit->Append(wxID_PASTE);
			menuEdit->Append(wxID_SELECTALL);
		}

		CreateMenuBarForFrame(subframe, menuFile);// , menuEdit);

		subframe->SetIcon(isCanvas ? wxICON(chrt) : wxICON(notepad));

		return subframe;
	}

	void App::CreateMenuBarForFrame(wxFrame* frame, wxMenu* file)
	{
		auto menubar = new wxMenuBar;
		menubar->Append(file, wxGetStockLabel(wxID_FILE));

		auto help = new wxMenu;
		help->Append(wxID_ABOUT);

		menubar->Append(help, wxGetStockLabel(wxID_HELP));

		frame->SetMenuBar(menubar);
	}
}
/*
	This sample show document/view support in wxWidgets.

	It can be run in several ways:
		* With "--mdi" command line option to use multiple MDI child frames
		  for the multiple documents (this is the default).
		* With "--sdi" command line option to use multiple top level windows
		  for the multiple documents
		* With "--single" command line option to support opening a single
		  document only

	Notice that doing it like this somewhat complicates the code, you could
	make things much simpler in your own programs by using either
	wxDocParentFrame or wxDocMDIParentFrame unconditionally (and never using
	the single mode) instead of supporting all of them as this sample does.
 */

// The window showing the drawing itself
class MyCanvas : public wxScrolledWindow
{
public:
	// view may be null if we're not associated with one yet, but parent must
	// be a valid pointer
	MyCanvas(FTK::View* view, wxWindow* parent = nullptr);
	~MyCanvas() override;

	void OnDraw(wxDC& dc) override;

	// in a normal multiple document application a canvas is associated with
	// one view from the beginning until the end, but to support the single
	// document mode in which all documents reuse the same App::GetCanvas()
	// we need to allow switching the canvas from one view to another one

	void SetView(FTK::View* view)
	{
		wxASSERT_MSG(!m_view, "shouldn't be already associated with a view");

		m_view = view;
	}

	void ResetView()
	{
		wxASSERT_MSG(m_view, "should be associated with a view");

		m_view = nullptr;
	}

private:
	void OnMouseEvent(wxMouseEvent& event);

	FTK::View* m_view = nullptr;

	// the segment being currently drawn or nullptr if none
	std::unique_ptr<DoodleSegment> m_currentSegment;

	// the last mouse press position
	wxPoint m_lastMousePos;
};

wxIMPLEMENT_APP(App);

App::App()
{
#ifdef WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF) | _CRTDBG_LEAK_CHECK_DF);
#endif
	Bind(wxEVT_MENU, &App::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &App::OnForceCloseAll, this, wxID_CLEAR);
	SetAppName("wxWidgetsApp");
	SetAppDisplayName("wxWidgetsApp");
#if wxUSE_MDI_ARCHITECTURE
	m_mode = FTK::View::Mode::Mode_MDI;
#else
	m_mode = FTK::View::Mode_SDI;
#endif
}

void App::OnInitCmdLine(wxCmdLineParser& parser)
{
	wxApp::OnInitCmdLine(parser);

#if wxUSE_MDI_ARCHITECTURE
	parser.AddSwitch("", CmdLineOption::MDI,
		"run in MDI mode: multiple documents, single window");
#endif // wxUSE_MDI_ARCHITECTURE
#if wxUSE_AUI
	parser.AddSwitch("", CmdLineOption::AUI,
		"run in MDI mode using AUI: multiple documents, single window");
#endif // wxUSE_AUI
	parser.AddSwitch("", CmdLineOption::SDI,
		"run in SDI mode: multiple documents, multiple windows");
	parser.AddSwitch("", CmdLineOption::SINGLE,
		"run in single document mode");

	parser.AddParam("document-file",
		wxCMD_LINE_VAL_STRING,
		wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL);
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser)
{
	int numModeOptions = 0;

#if wxUSE_MDI_ARCHITECTURE
	if (parser.Found(CmdLineOption::MDI))
	{
		m_mode = FTK::View::Mode::Mode_MDI;
		numModeOptions++;
	}
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	if (parser.Found(CmdLineOption::AUI))
	{
		m_mode = FTK::View::Mode::Mode_AUI;
		numModeOptions++;
	}
#endif // wxUSE_AUI

	if (parser.Found(CmdLineOption::SDI))
	{
		m_mode = FTK::View::Mode::Mode_SDI;
		numModeOptions++;
	}

	if (parser.Found(CmdLineOption::SINGLE))
	{
		m_mode = FTK::View::Mode::Mode_Single;
		numModeOptions++;
	}

	if (numModeOptions > 1)
	{
		wxLogError("Only a single option choosing the mode can be given.");
		return false;
	}

	// save any files given on the command line: we'll open them in OnInit()
	// later, after creating the frame
	for (size_t i = 0; i != parser.GetParamCount(); ++i)
		m_filesFromCmdLine.push_back(parser.GetParam(i));

	return wxApp::OnCmdLineParsed(parser);
}

#ifdef __WXMAC__
void App::MacNewFile()
{
	FTK::DocManager::GetDocumentManager()->CreateNewDocument();
}
#endif // __WXMAC__

bool App::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	::wxInitAllImageHandlers();

	/*auto frame = wxw::CreateFrame(GetAppDisplayName());

	wxMenu* file = new wxMenu;

	file->Append(wxID_NEW);
	file->Append(wxID_OPEN);

	//if (m_mode == Mode_Single)
		//AppendDocumentFileCommands(menuFile, true);

	file->AppendSeparator();
	file->Append(wxID_EXIT);

	wxMenuBar* menubar = new wxMenuBar;

	menubar->Append(file, wxGetStockLabel(wxID_FILE));
	wxMenu* help = new wxMenu;
	help->Append(wxID_ABOUT);
	menubar->Append(help, wxGetStockLabel(wxID_HELP));

	frame->SetMenuBar(menubar);

	frame->Show();
	//CreateView();
	return true;*/

	// Fill in the application information fields before creating wxConfig.
	SetVendorName("wxWidgets_Samples");
	SetAppName("docview");
	SetAppDisplayName("wxWidgets DocView Sample");

	//// Create a document manager
	auto docManager = new FTK::DocManager;

	//// Create a template relating drawing documents to their views
	docManager->AssociateTemplate(std::make_shared<FTK::DocViewTemplate>(_R("Drawing"), _R("*.drw"), _R("drw"),
	    _R("Drawing Doc"), _R("Drawing View"),
		[]() {return std::make_unique<DrawingDocument>(); }, []() {return std::make_unique<DrawingView>(); }));
	docManager->AssociateTemplate(std::make_shared<FTK::DocViewTemplate>(_R("Drawing"), _R("*.drz"), _R("drz"),
		_R("Drawing Doc"), _R("Drawing View"),
		[]() {return std::make_unique<DrawingDocument>(); }, []() {return std::make_unique<DrawingView>(); }));

	if (m_mode == FTK::View::Mode::Mode_Single)
	{
		// If we've only got one window, we only get to edit one document at a
		// time. Therefore no text editing, just doodling.
		docManager->SetMaxDocsOpen(1);
	}
	else // multiple documents mode: allow documents of different types
	{
		// Create a template relating text documents to their views
		docManager->AssociateTemplate(std::make_shared<FTK::DocViewTemplate>(_R("Text"), _R("*.txt;*.text"), _R("txt;text"),
			_R("Text Doc"), _R("Text View"),
			[]() { return std::make_unique<TextEditDocument>(); }, []() {return std::make_unique<TextEditView>(); }));
		// Create a template relating image documents to their views
		//new wxDocTemplate(docManager, "Image", "*.png;*.jpg", "", "png;jpg",
			//"Image Doc", "Image View",
			//CLASSINFO(ImageDocument), CLASSINFO(ImageView));
	}

	// create the main frame window
	wxFrame* frame = nullptr;
	switch (m_mode)
	{
#if wxUSE_MDI_ARCHITECTURE
	case FTK::View::Mode::Mode_MDI:
		frame = new ftkDocParentFrameAny<wxMDIParentFrame>(docManager, nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr));
		break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	case FTK::View::Mode::Mode_AUI:
		frame = new ftkDocParentFrameAny<wxAuiMDIParentFrame>
			(
				docManager, nullptr, wxID_ANY,
				GetAppDisplayName(),
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(1280, 720), nullptr)
			);
		break;
#endif // wxUSE_AUI

	case FTK::View::Mode::Mode_SDI:
	case FTK::View::Mode::Mode_Single:
		frame = new ftkDocParentFrameAny<wxFrame>(docManager, nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr));
		break;
	}

	// and its menu bar
	wxMenu* menuFile = new wxMenu;

	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);

	if (m_mode == FTK::View::Mode::Mode_Single)
		AppendDocumentFileCommands(menuFile, true);

	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	// A nice touch: a history of files visited. Use this menu.
	docManager->FileHistoryUseMenu(menuFile);
#if wxUSE_CONFIG
	docManager->FileHistoryLoad(*wxConfig::Get());
#endif // wxUSE_CONFIG


	if (m_mode == FTK::View::Mode::Mode_Single)
	{
		m_canvas = new MyCanvas(nullptr, frame);
		m_menuEdit = CreateDrawingEditMenu();
	}

	CreateMenuBarForFrame(frame, menuFile, m_menuEdit);

	frame->SetIcon(wxICON(doc));
	frame->Centre();
	frame->Show();

	if (m_filesFromCmdLine.empty())
	{
		// on macOS the dialog will be shown by MacNewFile
#ifndef __WXMAC__
		docManager->CreateNewDocument();
#endif
	}
	else // we have files to open on command line
	{
		for (auto& file : m_filesFromCmdLine)
			docManager->CreateDocument(file, FTK::DocManager::wxDOC_SILENT);
	}

	return true;
}

int App::OnExit()
{
	auto manager = FTK::DocManager::GetDocumentManager();
#if wxUSE_CONFIG
	manager->FileHistorySave(*wxConfig::Get());
#endif // wxUSE_CONFIG
	delete manager;

	return wxApp::OnExit();
}

void App::AppendDocumentFileCommands(wxMenu* menu, bool supportsPrinting)
{
	menu->Append(wxID_CLOSE);
	menu->Append(wxID_SAVE);
	menu->Append(wxID_SAVEAS);
	menu->Append(wxID_REVERT, _("Re&vert..."));
	menu->Append(wxID_CLEAR, "&Force close all");

	if (supportsPrinting)
	{
		menu->AppendSeparator();
		menu->Append(wxID_PRINT);
		menu->Append(wxID_PRINT_SETUP, "Print &Setup...");
		menu->Append(wxID_PREVIEW);
	}
}

wxMenu* App::CreateDrawingEditMenu()
{
	wxMenu* const menu = new wxMenu;
	menu->Append(wxID_UNDO);
	menu->Append(wxID_REDO);
	menu->AppendSeparator();
	menu->Append(wxID_CUT, "&Cut last segment");

	return menu;
}

void App::CreateMenuBarForFrame(wxFrame* frame, wxMenu* file, wxMenu* edit)
{
	auto menubar = new wxMenuBar;

	menubar->Append(file, wxGetStockLabel(wxID_FILE));

	if (edit)
		menubar->Append(edit, wxGetStockLabel(wxID_EDIT));

	auto help = new wxMenu;
	help->Append(wxID_ABOUT);
	menubar->Append(help, wxGetStockLabel(wxID_HELP));

	frame->SetMenuBar(menubar);
}

wxFrame* App::CreateChildFrame(FTK::View* view, bool isCanvas)
{
	// create a child frame of appropriate class for the current mode
	wxFrame* subframe = nullptr;
	auto doc = view->GetDocument();
	switch (m_mode)
#if wxUSE_MDI_ARCHITECTURE
	{
	case FTK::View::Mode::Mode_MDI:
		subframe = new ftkDocChildFrameAny<wxMDIChildFrame, wxMDIParentFrame>
		(
			view,
			static_cast<ftkDocParentFrameAny<wxMDIParentFrame>*>(GetTopWindow()),
			wxID_ANY,
			"Child Frame",
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(640, 480), nullptr)
		);
		break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	case FTK::View::Mode::Mode_AUI:
		subframe = new ftkDocChildFrameAny<wxAuiMDIChildFrame, wxAuiMDIParentFrame>
			(
				view,
				static_cast<ftkDocParentFrameAny<wxAuiMDIParentFrame>*>(GetTopWindow()),
				wxID_ANY,
				"Child Frame",
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(640, 480), nullptr)
			);
		break;
#endif // wxUSE_AUI

	case FTK::View::Mode::Mode_SDI:
	case FTK::View::Mode::Mode_Single:
		subframe = new ftkDocChildFrameAny<wxFrame, wxFrame>
		(
			view,
			static_cast<ftkDocParentFrameAny<wxFrame>* >(GetTopWindow()),
			wxID_ANY,
			"Child Frame",
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(640, 480), nullptr)
		);

		subframe->Centre();
		break;
	}

	auto menuFile = new wxMenu;

	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);
	AppendDocumentFileCommands(menuFile, isCanvas);
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuEdit = nullptr;
	if (isCanvas)
	{
		menuEdit = CreateDrawingEditMenu();

		FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc)->SetEditMenu_(menuEdit);
		FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc)->SetMenuStrings_();// Initialize_();
	}
	else // text frame
	{
		menuEdit = new wxMenu;
		menuEdit->Append(wxID_COPY);
		menuEdit->Append(wxID_PASTE);
		menuEdit->Append(wxID_SELECTALL);
	}

	CreateMenuBarForFrame(subframe, menuFile, menuEdit);

	subframe->SetIcon(isCanvas ? wxICON(chrt) : wxICON(notepad));

	return subframe;
}

void App::OnForceCloseAll(wxCommandEvent& WXUNUSED(event))
{
	// Pass "true" here to force closing just for testing this functionality,
	// there is no real reason to force the issue here.
	FTK::DocManager::GetDocumentManager()->CloseDocuments(true);
}

void App::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString modeName = "Mixed mode";

	const int docsCount = 0;// ftkDocManager::GetDocumentManager()->GetDocuments().size();

	wxLogMessage
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
	);
}

bool TextEditDocument::OnCreate(const mafString& path)
{
	if (!FTK::Document::OnCreate(path))
		return false;

	// subscribe to changes in the text control to update the document state
	// when it's modified
	GetTextCtrl()->Bind(wxEVT_TEXT, &TextEditDocument::OnTextChange, this);

	return true;
}

// Since text windows have their own method for saving to/loading from files,
// we override DoSave/OpenDocument instead of Save/LoadObject
bool TextEditDocument::DoSaveDocument(const mafString& filename)
{
	return GetTextCtrl()->SaveFile(mafStringToWx(filename));
}

bool TextEditDocument::DoOpenDocument(const mafString& filename)
{
	if (!GetTextCtrl()->LoadFile(mafStringToWx(filename)))
		return false;

	// we're not modified by the user yet
	Modify(false);

	return true;
}

bool TextEditDocument::IsModified() const
{
	wxTextCtrl* wnd = GetTextCtrl();
	return FTK::Document::IsModified() || (wnd && wnd->IsModified());
}

void TextEditDocument::Modify(bool modified)
{
	FTK::Document::Modify(modified);

	wxTextCtrl* wnd = GetTextCtrl();
	if (wnd && !modified)
	{
		wnd->DiscardEdits();
	}
}

void TextEditDocument::OnTextChange(wxCommandEvent& event)
{
	Modify(true);

	event.Skip();
}

wxTextCtrl* TextEditDocument::GetTextCtrl() const
{
	return nullptr;
	//auto view = GetFirstView();
	//return view ? wxStaticCast(view, TextEditView)->GetText() : nullptr;
}

TextEditView::TextEditView()
{
	Bind(wxEVT_MENU, &TextEditView::OnCopy, this, wxID_COPY);
	Bind(wxEVT_MENU, &TextEditView::OnPaste, this, wxID_PASTE);
	Bind(wxEVT_MENU, &TextEditView::OnSelectAll, this, wxID_SELECTALL);
}


bool TextEditView::OnCreate(std::shared_ptr<FTK::Document> doc)
{
	if (!FTK::View::OnCreate(doc))
		return false;

	auto frame = wxGetApp().CreateChildFrame(this, false);
	wxASSERT(frame == GetFrame());
	m_text = new wxTextCtrl(frame, wxID_ANY, "",
		wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE);
	frame->Show();

	return true;
}

void TextEditView::OnDraw(wxDC* WXUNUSED(dc))
{
	// nothing to do here, wxTextCtrl draws itself
}

bool TextEditView::OnClose(bool deleteWindow)
{
	if (!FTK::View::OnClose(deleteWindow))
		return false;

	Activate(false);

	if (wxGetApp().GetMode() == FTK::View::Mode::Mode_Single)
	{
		m_text->Clear();
	}
	else // not single window mode
	{
		if (deleteWindow)
		{
			GetFrame()->Destroy();
			SetFrame(nullptr);
		}
	}
	return true;
}


// ----------------------------------------------------------------------------
// Some operations (which can be done and undone by the view) on the document:
// ----------------------------------------------------------------------------

// Base class for all operations on DrawingDocument
class DrawingCommand : public FTK::Command
{
public:
	DrawingCommand(DrawingDocument* doc,
		const mafString& name,
		const DoodleSegment& segment = DoodleSegment())
		: FTK::Command(true, name),
		m_doc(doc),
		m_segment(segment)
	{
	}

protected:
	bool DoAdd() { m_doc->AddDoodleSegment(m_segment); return true; }
	bool DoRemove() { return m_doc->PopLastSegment(&m_segment); }

private:
	DrawingDocument* const m_doc;
	DoodleSegment m_segment;
};

// The command for adding a new segment
class DrawingAddSegmentCommand : public DrawingCommand
{
public:
	DrawingAddSegmentCommand(DrawingDocument* doc, const DoodleSegment& segment)
		: DrawingCommand(doc, _R("Add new segment"), segment)
	{
	}

	virtual bool Do() override { return DoAdd(); }
	virtual bool Undo() override { return DoRemove(); }
};

// The command for removing the last segment
class DrawingRemoveSegmentCommand : public DrawingCommand
{
public:
	DrawingRemoveSegmentCommand(DrawingDocument* doc)
		: DrawingCommand(doc, _R("Remove last segment"))
	{
	}

	virtual bool Do() override { return DoRemove(); }
	virtual bool Undo() override { return DoAdd(); }
};

std::ostream& DrawingDocument::SaveObject(std::ostream& ostream)
{
	FTK::Document::SaveObject(ostream);

	const wxInt32 count = m_doodleSegments.size();
	ostream << count << '\n';

	for (int n = 0; n < count; n++)
	{
		m_doodleSegments[n].SaveObject(ostream);
		ostream << '\n';
	}
	return ostream;
}

std::istream& DrawingDocument::LoadObject(std::istream& istream)
{
	FTK::Document::LoadObject(istream);

	wxInt32 count = 0;
	istream >> count;
	if (count < 0)
	{
		wxLogWarning("Drawing document corrupted: invalid segments count.");
		istream.clear(std::ios::badbit);
		return istream;
	}

	for (int n = 0; n < count; n++)
	{
		DoodleSegment segment;
		segment.LoadObject(istream);
		m_doodleSegments.push_back(segment);
	}

	return istream;
}

void DrawingDocument::AddDoodleSegment(const DoodleSegment& segment)
{
	m_doodleSegments.push_back(segment);
}

bool DrawingDocument::PopLastSegment(DoodleSegment* segment)
{
	if (m_doodleSegments.empty())
		return false;

	if (segment)
		*segment = m_doodleSegments.back();

	m_doodleSegments.pop_back();

	return true;
}

// ----------------------------------------------------------------------------
// DoodleSegment implementation
// ----------------------------------------------------------------------------

std::ostream& DoodleSegment::SaveObject(std::ostream& ostream)
{
	const wxInt32 count = m_lines.size();
	ostream << count << '\n';

	for (int n = 0; n < count; n++)
	{
		const DoodleLine& line = m_lines[n];
		ostream
			<< line.x1 << ' '
			<< line.y1 << ' '
			<< line.x2 << ' '
			<< line.y2 << '\n';
	}

	return ostream;
}

std::istream& DoodleSegment::LoadObject(std::istream& istream)
{
	wxInt32 count = 0;
	istream >> count;

	for (int n = 0; n < count; n++)
	{
		DoodleLine line;
		istream
			>> line.x1
			>> line.y1
			>> line.x2
			>> line.y2;
		m_lines.push_back(line);
	}

	return istream;
}

// Define a constructor for my canvas
MyCanvas::MyCanvas(FTK::View* view, wxWindow* parent)
	: wxScrolledWindow(parent ? parent : view->GetFrame())
{
	m_view = view;
	m_lastMousePos = wxDefaultPosition;

	SetCursor(wxCursor(wxCURSOR_PENCIL));

	// this is completely arbitrary and is done just for illustration purposes
	SetVirtualSize(1000, 1000);
	SetScrollRate(20, 20);

	SetBackgroundColour(*wxWHITE);


	
	Bind(wxEVT_LEFT_DOWN, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_LEFT_UP, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_LEFT_DCLICK, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_MIDDLE_DOWN, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_MIDDLE_UP, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_MIDDLE_DCLICK, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_RIGHT_DOWN, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_RIGHT_UP, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_RIGHT_DCLICK, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_AUX1_DOWN, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_AUX1_UP, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_AUX1_DCLICK, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_AUX2_DOWN, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_AUX2_UP, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_AUX2_DCLICK, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_MOTION, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_LEAVE_WINDOW, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_ENTER_WINDOW, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_MOUSEWHEEL, &MyCanvas::OnMouseEvent, this);
	Bind(wxEVT_MAGNIFY, &MyCanvas::OnMouseEvent, this);
}

MyCanvas::~MyCanvas()
{
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
	if (m_view)
		m_view->OnDraw(&dc);
}

// This implements a tiny doodling program. Drag the mouse using the left
// button.
void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
	if (!m_view)
		return;

	wxClientDC dc(this);
	PrepareDC(dc);

	dc.SetPen(*wxBLACK_PEN);

	const wxPoint pt(event.GetLogicalPosition(dc));

	// is this the end of the current segment?
	if (m_currentSegment && event.LeftUp())
	{
		if (!m_currentSegment->IsEmpty())
		{
			// We've got a valid segment on mouse left up, so store it.
			auto doc = static_cast<DrawingDocument*>(m_view->GetDocument());

			FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc)->Submit(std::make_unique<DrawingAddSegmentCommand>(doc, *m_currentSegment));
			FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc)->SetMenuStrings_();

			doc->Modify(true);
			doc->NotifyUpdate();
		}

		m_currentSegment.reset();
	}

	// is this the start of a new segment?
	if (m_lastMousePos != wxDefaultPosition && event.Dragging())
	{
		if (!m_currentSegment)
			m_currentSegment = std::make_unique<DoodleSegment>();

		m_currentSegment->AddLine(m_lastMousePos, pt);

		dc.DrawLine(m_lastMousePos, pt);
	}

	m_lastMousePos = pt;
}

DrawingView::DrawingView()
{
	Bind(wxEVT_MENU, &DrawingView::OnCut, this, wxID_CUT);
}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(std::shared_ptr<FTK::Document> doc)
{
	if (!FTK::View::OnCreate(doc))
		return false;

	auto& app = wxGetApp();
	if (wxGetApp().GetMode() != FTK::View::Mode::Mode_Single)
	{
		// create a new window and canvas inside it
		auto frame = app.CreateChildFrame(this, true);
		wxASSERT(frame == GetFrame());
		m_canvas = new MyCanvas(this);
		frame->Show();
	}
	else // single document mode
	{
		// reuse the existing window and canvas
		m_canvas = app.GetMainWindowCanvas();
		m_canvas->SetView(this);

		// Initialize the edit menu Undo and Redo items
		//doc->GetCommandProcessor()->SetEditMenu(app.GetMainWindowEditMenu());
		FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc.get())->SetMenuStrings_();// Initialize_();
	}

	return true;
}

// Sneakily gets used for default print/preview as well as drawing on the
// screen.
void DrawingView::OnDraw(wxDC* dc)
{
	dc->SetPen(*wxBLACK_PEN);

	// simply draw all lines of all segments
	const auto& segments = GetDocument()->GetSegments();
	for (auto& seg : segments)
	{
		for (auto& line : seg.GetLines())
		{
			dc->DrawLine(line.x1, line.y1, line.x2, line.y2);
		}
	}
}

DrawingDocument* DrawingView::GetDocument()
{
	return static_cast<DrawingDocument*>(FTK::View::GetDocument());
}

void DrawingView::OnUpdate(FTK::View* sender, wxObject* hint)
{
	FTK::View::OnUpdate(sender, hint);
	if (m_canvas)
		m_canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool DrawingView::OnClose(bool deleteWindow)
{
	if (!FTK::View::OnClose(deleteWindow))
		return false;

	Activate(false);

	// Clear the canvas in single-window mode in which it stays alive
	if (wxGetApp().GetMode() == FTK::View::Mode::Mode_Single)
	{
		m_canvas->ClearBackground();
		m_canvas->ResetView();
		m_canvas = nullptr;

		if (GetFrame())
			wxStaticCast(GetFrame(), wxFrame)->SetTitle(wxTheApp->GetAppDisplayName());
	}
	else // not single window mode
	{
		if (deleteWindow)
		{
			GetFrame()->Destroy();
			SetFrame(nullptr);
		}
	}
	return true;
}

void DrawingView::OnCut(wxCommandEvent& WXUNUSED(event))
{
	auto doc = GetDocument();

	FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc)->Submit(std::make_unique<DrawingRemoveSegmentCommand>(doc));
	FTK::DocManager::GetDocumentManager()->GetCommandProcessor(doc)->SetMenuStrings_();
	doc->Modify(true);
	doc->NotifyUpdate();
}
