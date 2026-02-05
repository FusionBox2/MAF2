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
#endif // wxUSE_AUI
//#include <wx/mdi.h>
#include <wx/aui/aui.h>

//#include "doc.h"
//#include "view.h"

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

#include <list>
#include <memory>
#include <optional>


namespace FTK
{
	class Command
	{
	public:
		Command(bool canUndoIt = false, const wxString& name = wxEmptyString);

		virtual ~Command() = default;

		virtual bool Do() = 0;

		virtual bool Undo() = 0;

		virtual bool CanUndo() const { return m_canUndo; }

		virtual wxString GetName() const { return m_commandName; }

	protected:
		wxString m_commandName;
		bool     m_canUndo;
	};

	Command::Command(bool canUndoIt, const wxString& name)
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

		virtual ~CommandProcessor();

		// Pass a command to the processor. The processor calls Do(); if
		// successful, is appended to the command history unless storeIt is false.
		virtual bool Submit(std::unique_ptr<Command> command, bool storeIt = true);

		// just store the command without executing it
		virtual void Store(std::unique_ptr<Command> command);

		virtual bool Undo();
		virtual bool Redo();
		virtual bool CanUndo() const;
		virtual bool CanRedo() const;

		// Initialises the current command and menu strings.
		virtual void Initialize();

		// Sets the Undo/Redo menu strings for the current menu.
		virtual void SetMenuStrings();

		// Gets the current Undo menu label.
		wxString GetUndoMenuLabel() const;

		// Gets the current Undo menu label.
		wxString GetRedoMenuLabel() const;

#if wxUSE_MENUS
		// Call this to manage an edit menu.
		void SetEditMenu(wxMenu* menu) { m_commandEditMenu = menu; }
		wxMenu* GetEditMenu() const { return m_commandEditMenu; }
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

		// By default, the accelerators are "\tCtrl+Z" and "\tCtrl+Y"
		const wxString& GetUndoAccelerator() const { return m_undoAccelerator; }
		const wxString& GetRedoAccelerator() const { return m_redoAccelerator; }

		void SetUndoAccelerator(const wxString& accel) { m_undoAccelerator = accel; }
		void SetRedoAccelerator(const wxString& accel) { m_redoAccelerator = accel; }

	protected:
		// for further flexibility, command processor doesn't call ftkCommand::Do()
		// and Undo() directly but uses these functions which can be overridden in
		// the derived class
		virtual bool DoCommand(Command& cmd);
		virtual bool UndoCommand(Command& cmd);

		size_t                 m_maxNoCommands;
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
#endif // wxUSE_ACCEL
	{
	}

	CommandProcessor::~CommandProcessor() = default;

	bool CommandProcessor::DoCommand(Command& cmd)
	{
		return cmd.Do();
	}

	bool CommandProcessor::UndoCommand(Command& cmd)
	{
		return cmd.Undo();
	}

	// Pass a command to the processor. The processor calls Do();
	// if successful, is appended to the command history unless
	// storeIt is false.
	bool CommandProcessor::Submit(std::unique_ptr<Command> command, bool storeIt)
	{
		wxCHECK_MSG(command, false, wxT("no command in CommandProcessor::Submit"));

		if (!DoCommand(*command))
		{
			return false;
		}

		if (storeIt)
		{
			Store(std::move(command));
		}

		return true;
	}

	void CommandProcessor::Store(std::unique_ptr<Command> command)
	{
		wxCHECK_RET(command, wxT("no command in CommandProcessor::Store"));

		m_redoCommands.clear();

		if (m_maxNoCommands != 0)
		{
			if (m_undoCommands.size() == m_maxNoCommands)
			{
				if (m_lastSavedCommand == m_undoCommands.front().get())
				{
					m_lastSavedCommand = std::nullopt;
				}
				m_undoCommands.pop_front();
			}
			m_undoCommands.push_back(std::move(command));
		}
		SetMenuStrings();
	}

	bool CommandProcessor::Undo()
	{
		if (CanUndo())
		{
			if (UndoCommand(*m_undoCommands.back()))
			{
				m_redoCommands.splice(begin(m_redoCommands), m_undoCommands, --end(m_undoCommands));
				SetMenuStrings();
				return true;
			}
		}

		return false;
	}

	bool CommandProcessor::Redo()
	{
		if (CanRedo())
		{
			if (DoCommand(*m_redoCommands.front()))
			{
				m_undoCommands.splice(end(m_undoCommands), m_redoCommands, begin(m_redoCommands));
				SetMenuStrings();
				return true;
			}
		}
		return false;
	}

	bool CommandProcessor::CanUndo() const
	{
		return !m_undoCommands.empty() && m_undoCommands.back()->CanUndo();
	}

	bool CommandProcessor::CanRedo() const
	{
		return !m_redoCommands.empty();
	}

	void CommandProcessor::Initialize()
	{
		SetMenuStrings();
	}

	void CommandProcessor::SetMenuStrings()
	{
#if wxUSE_MENUS
		if (m_commandEditMenu)
		{
			wxString undoLabel = GetUndoMenuLabel();
			wxString redoLabel = GetRedoMenuLabel();

			m_commandEditMenu->SetLabel(wxID_UNDO, undoLabel);
			m_commandEditMenu->Enable(wxID_UNDO, CanUndo());

			m_commandEditMenu->SetLabel(wxID_REDO, redoLabel);
			m_commandEditMenu->Enable(wxID_REDO, CanRedo());
		}
#endif // wxUSE_MENUS
	}

	// Gets the current Undo menu label.
	wxString CommandProcessor::GetUndoMenuLabel() const
	{
		wxString buf;
		if (!m_undoCommands.empty())
		{
			auto& command = m_undoCommands.back();
			wxString commandName(command->GetName());
			if (commandName.empty()) commandName = _("Unnamed command");
			bool canUndo = command->CanUndo();
			if (canUndo)
				buf = wxString(_("&Undo ")) + commandName + m_undoAccelerator;
			else
				buf = wxString(_("Can't &Undo ")) + commandName + m_undoAccelerator;
		}
		else
		{
			buf = _("&Undo") + m_undoAccelerator;
		}

		return buf;
	}

	// Gets the current Undo menu label.
	wxString CommandProcessor::GetRedoMenuLabel() const
	{
		wxString buf;
		if (!m_redoCommands.empty())
		{
			auto& redoCommand = m_redoCommands.front();
			wxString redoCommandName(redoCommand->GetName());
			if (redoCommandName.empty()) redoCommandName = _("Unnamed command");
			buf = wxString(_("&Redo ")) + redoCommandName + m_redoAccelerator;
		}
		else
		{
			buf = _("&Redo") + m_redoAccelerator;
		}
		return buf;
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
}

class ftkView;
class ftkDocTemplate;
class ftkDocManager;

class ftkDocChildFrameAnyBase;

// Flags for wxDocManager (can be combined).
enum
{
	wxDOC_NEW = 1,
	wxDOC_SILENT = 2
};

// Document template flags
enum
{
	wxTEMPLATE_VISIBLE = 1,
	wxTEMPLATE_INVISIBLE = 2,
	wxDEFAULT_TEMPLATE_FLAGS = wxTEMPLATE_VISIBLE
};

class ftkDocument : public wxEvtHandler
{
public:
	ftkDocument();

	ftkDocument(const ftkDocument&) = delete;
	ftkDocument& operator=(const ftkDocument&) = delete;

	~ftkDocument() override;

	// accessors
	void SetFilename(const wxString& filename, bool notifyViews = false);
	wxString GetFilename() const { return m_documentFile; }

	void SetTitle(const wxString& title) { m_documentTitle = title; }
	wxString GetTitle() const { return m_documentTitle; }

	void SetDocumentName(const wxString& name) { m_documentTypeName = name; }
	wxString GetDocumentName() const { return m_documentTypeName; }

	// access the flag indicating whether this document had been already saved,
	// SetDocumentSaved() is only used internally, don't call it
	bool GetDocumentSaved() const { return m_savedYet; }
	void SetDocumentSaved(bool saved = true) { m_savedYet = saved; }

	// activate the first view of the document if any
	void Activate();

	// return true if the document hasn't been modified since the last time it
	// was saved (implying that it returns false if it was never saved, even if
	// the document is not modified)
	bool AlreadySaved() const { return !IsModified() && GetDocumentSaved(); }

	virtual bool Close();
	virtual bool Save();
	virtual bool SaveAs();
	virtual bool Revert();

#if wxUSE_STD_IOSTREAM
	virtual std::ostream& SaveObject(std::ostream& stream);
	virtual std::istream& LoadObject(std::istream& stream);
#else
	virtual wxOutputStream& SaveObject(wxOutputStream& stream);
	virtual wxInputStream& LoadObject(wxInputStream& stream);
#endif

	// Called by wxWidgets
	virtual bool OnSaveDocument(const wxString& filename);
	virtual bool OnOpenDocument(const wxString& filename);
	virtual bool OnNewDocument();
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
	virtual bool OnCreate(const wxString& path, long flags);

	// By default, creates a base ftkCommandProcessor.
	virtual FTK::CommandProcessor* OnCreateCommandProcessor();
	virtual FTK::CommandProcessor* GetCommandProcessor() const
	{
		return m_commandProcessor;
	}
	virtual void SetCommandProcessor(FTK::CommandProcessor* proc)
	{
		m_commandProcessor = proc;
	}

	// Called after a view is added or removed. The default implementation
	// deletes the document if this is there are no more views.
	virtual void OnChangedViewList();

	// Called from OnCloseDocument(), does nothing by default but may be
	// overridden. Return value is ignored.
	virtual bool DeleteContents();

	virtual bool Draw(wxDC&);
	virtual bool IsModified() const { return m_documentModified; }
	virtual void Modify(bool mod);

	virtual bool AddView(ftkView* view);
	virtual bool RemoveView(ftkView* view);

	std::vector<ftkView*> GetViewsVector() const;

	std::list<ftkView*>& GetViews() { return m_documentViews; }
	const std::list<ftkView*>& GetViews() const { return m_documentViews; }

	ftkView* GetFirstView() const;

	virtual void UpdateAllViews(ftkView* sender = nullptr, wxObject* hint = nullptr);
	virtual void NotifyClosing();

	// Remove all views (because we're closing the document)
	virtual bool DeleteAllViews();

	// Other stuff
	virtual ftkDocManager* GetDocumentManager() const;
	virtual ftkDocTemplate* GetDocumentTemplate() const
	{
		return m_documentTemplate;
	}
	virtual void SetDocumentTemplate(ftkDocTemplate* temp)
	{
		m_documentTemplate = temp;
	}

	// Get the document name to be shown to the user: the title if there is
	// any, otherwise the filename if the document was saved and, finally,
	// "unnamed" otherwise
	virtual wxString GetUserReadableName() const;

	// Returns a window that can be used as a parent for document-related
	// dialogs. Override if necessary.
	virtual wxWindow* GetDocumentWindow() const;

	// Ask the user if the document should be saved if it's modified and save
	// it if necessary.
	//
	// Returns false if the user cancelled closing or if saving failed.
	bool CanClose();

protected:
	std::list<ftkView*>   m_documentViews;
	wxString              m_documentFile;
	wxString              m_documentTitle;
	wxString              m_documentTypeName;
	ftkDocTemplate*       m_documentTemplate = nullptr;
	FTK::CommandProcessor* m_commandProcessor = nullptr;
	bool                  m_documentModified = false;
	bool                  m_savedYet = false;

	// Called by OnSaveDocument and OnOpenDocument to implement standard
	// Save/Load behaviour. Re-implement in derived class for custom
	// behaviour.
	virtual bool DoSaveDocument(const wxString& file);
	virtual bool DoOpenDocument(const wxString& file);

	// the default implementation of GetUserReadableName()
	wxString DoGetUserReadableName() const;
};

class ftkView : public wxEvtHandler
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

	ftkView(Mode mode);

	ftkView(const ftkView&) = delete;
	ftkView& operator=(const ftkView&) = delete;

	virtual ~ftkView();

	Mode GetMode() const { return m_mode; }

	ftkDocument* GetDocument() const { return m_viewDocument; }
	virtual void SetDocument(ftkDocument* doc);

	wxString GetViewName() const { return m_viewTypeName; }
	void SetViewName(const wxString& name) { m_viewTypeName = name; }

	wxWindow* GetFrame() const { return m_viewFrame; }
	void SetFrame(wxWindow* frame) { m_viewFrame = frame; }

	virtual void OnActivateView(bool activate,
		ftkView* activeView,
		ftkView* deactiveView);
	virtual void OnDraw(wxDC* dc) = 0;
	virtual void OnPrint(wxDC* dc, wxObject* info);
	virtual void OnUpdate(ftkView* sender, wxObject* hint = nullptr);
	virtual void OnClosingDocument() {}
	virtual void OnChangeFilename();

	// Called by framework if created automatically by the default document
	// manager class: gives view a chance to initialise
	virtual bool OnCreate(ftkDocument* WXUNUSED(doc), long WXUNUSED(flags))
	{
		return true;
	}

	// Checks if the view is the last one for the document; if so, asks user
	// to confirm save data (if modified). If ok, deletes itself and returns
	// true.
	virtual bool Close(bool deleteWindow = true);

	// Override to do cleanup/veto close
	virtual bool OnClose(bool deleteWindow);

	// A view's window can call this to notify the view it is (in)active.
	// The function then notifies the document manager.
	virtual void Activate(bool activate);

	ftkDocManager* GetDocumentManager() const
	{
		return m_viewDocument->GetDocumentManager();
	}

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
	virtual bool TryBefore(wxEvent& event) override;

	ftkDocument* m_viewDocument = nullptr;
	wxString     m_viewTypeName;
	wxWindow* m_viewFrame = nullptr;

	ftkDocChildFrameAnyBase* m_docChildFrame = nullptr;

private:
	// the currently used mode
	Mode m_mode;
};

class TextEditDocument : public ftkDocument
{
public:
	TextEditDocument() = default;

	TextEditDocument(const TextEditDocument&) = delete;
	TextEditDocument& operator=(const TextEditDocument&) = delete;

	bool OnCreate(const wxString& path, long flags) override;

	bool IsModified() const override;
	void Modify(bool mod) override;

protected:
	bool DoSaveDocument(const wxString& filename) override;
	bool DoOpenDocument(const wxString& filename) override;

private:
	wxTextCtrl* GetTextCtrl() const;

	void OnTextChange(wxCommandEvent& event);
};



class TextEditView : public ftkView
{
public:
	TextEditView(Mode mode) : ftkView(mode) {}

	bool OnCreate(ftkDocument* doc, long flags) override;
	void OnDraw(wxDC* dc) override;
	bool OnClose(bool deleteWindow = true) override;

	wxTextCtrl* GetText() const { return m_text; }

private:
	void OnCopy(wxCommandEvent& WXUNUSED(event)) { m_text->Copy(); }
	void OnPaste(wxCommandEvent& WXUNUSED(event)) { m_text->Paste(); }
	void OnSelectAll(wxCommandEvent& WXUNUSED(event)) { m_text->SelectAll(); }

	wxTextCtrl* m_text = nullptr;

	wxDECLARE_EVENT_TABLE();
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

typedef wxVector<DoodleLine> DoodleLines;

// Contains a list of lines: represents a mouse-down doodle
class DoodleSegment
{
public:
	//DocumentOstream& SaveObject(DocumentOstream& stream);
	//DocumentIstream& LoadObject(DocumentIstream& stream);

	bool IsEmpty() const { return m_lines.empty(); }
	void AddLine(const wxPoint& pt1, const wxPoint& pt2)
	{
		m_lines.push_back(DoodleLine(pt1, pt2));
	}
	const DoodleLines& GetLines() const { return m_lines; }

private:
	DoodleLines m_lines;
};

typedef wxVector<DoodleSegment> DoodleSegments;

class MyCanvas;


// The drawing document (model) class itself
class DrawingDocument : public ftkDocument
{
public:
	DrawingDocument() = default;

	//DocumentOstream& SaveObject(DocumentOstream& stream) override;
	//DocumentIstream& LoadObject(DocumentIstream& stream) override;

	// add a new segment to the document
	void AddDoodleSegment(const DoodleSegment& segment);

	// remove the last segment, if any, and copy it in the provided pointer if
	// not null and return true or return false and do nothing if there are no
	// segments
	bool PopLastSegment(DoodleSegment* segment);

	// get direct access to our segments (for DrawingView)
	const DoodleSegments& GetSegments() const { return m_doodleSegments; }

private:
	DoodleSegments m_doodleSegments;
};

// The view using MyCanvas to show its contents
class DrawingView : public ftkView
{
public:
	DrawingView(ftkView::Mode mode) : ftkView(mode) {}

	bool OnCreate(ftkDocument* doc, long flags) override;
	void OnDraw(wxDC* dc) override;
	void OnUpdate(ftkView* sender, wxObject* hint = nullptr) override;
	bool OnClose(bool deleteWindow = true) override;

	DrawingDocument* GetDocument();

private:
	void OnCut(wxCommandEvent& event);

	MyCanvas* m_canvas = nullptr;

	wxDECLARE_EVENT_TABLE();
};

// Represents user interface (and other) properties of documents and views
class ftkDocTemplate
{
public:
	// Associate document and view types. They're for identifying what view is
	// associated with what template/document type
	ftkDocTemplate(
		const wxString& descr,
		const wxString& filter,
		const wxString& dir,
		const wxString& ext,
		const wxString& docTypeName,
		const wxString& viewTypeName,
		std::function<ftkDocument* ()> docCreate,
		std::function<ftkView* (ftkView::Mode mode)> viewCreate,
		long flags = wxDEFAULT_TEMPLATE_FLAGS);

	ftkDocTemplate(const ftkDocTemplate&) = delete;
	ftkDocTemplate& operator=(const ftkDocTemplate&) = delete;

	virtual ~ftkDocTemplate() = default;;

	virtual ftkDocument* CreateDocument(const wxString& path, long flags = 0);
	virtual ftkView* CreateView(ftkDocument* doc, ftkView::Mode mode, long flags = 0);

	// Helper method for CreateDocument; also allows you to do your own document
	// creation
	virtual bool InitDocument(ftkDocument* doc, const wxString& path, long flags = 0);

	const wxString& GetDefaultExtension() const { return m_defaultExt; }
	const wxString& GetDescription() const { return m_description; }
	const wxString& GetDirectory() const { return m_directory; }
	const wxString& GetFileFilter() const { return m_fileFilter; }
	long GetFlags() const { return m_flags; }
	virtual const wxString& GetViewName() const { return m_viewTypeName; }
	virtual const wxString& GetDocumentName() const { return m_docTypeName; }

	void SetFileFilter(const wxString& filter) { m_fileFilter = filter; }
	void SetDirectory(const wxString& dir) { m_directory = dir; }
	void SetDescription(const wxString& descr) { m_description = descr; }
	void SetDefaultExtension(const wxString& ext) { m_defaultExt = ext; }
	void SetFlags(long flags) { m_flags = flags; }

	bool IsVisible() const { return (m_flags & wxTEMPLATE_VISIBLE) != 0; }

	virtual bool FileMatchesTemplate(const wxString& path);

protected:
	long              m_flags;
	wxString          m_fileFilter;
	wxString          m_directory;
	wxString          m_description;
	wxString          m_defaultExt;
	wxString          m_docTypeName;
	wxString          m_viewTypeName;

	// For dynamic creation of appropriate instances.
	std::function<ftkDocument* ()> m_docCreate;
	std::function<ftkView* (ftkView::Mode mode)> m_viewCreate;

	virtual ftkDocument* DoCreateDocument();
	virtual ftkView* DoCreateView(ftkView::Mode mode);
};

// One object of this class may be created in an application, to manage all
// the templates and documents.
class ftkDocManager : public wxEvtHandler
{
public:
	// NB: flags are unused, don't pass wxDOC_XXX to this ctor
	ftkDocManager(long flags = 0, bool initialize = true);

	ftkDocManager(const ftkDocManager&) = delete;
	ftkDocManager& operator=(const ftkDocManager&) = delete;

	~ftkDocManager() override;

	virtual bool Initialize();

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

	virtual ftkDocument* CreateDocument(const wxString& path, long flags = 0);

	// wrapper around CreateDocument() with a more clear name
	ftkDocument* CreateNewDocument()
	{
		return CreateDocument(wxString(), wxDOC_NEW);
	}

	virtual ftkView* CreateView(ftkDocument* doc, long flags = 0);
	virtual void DeleteTemplate(ftkDocTemplate* temp, long flags = 0);
	virtual bool FlushDoc(ftkDocument* doc);
	virtual ftkDocTemplate* MatchTemplate(const wxString& path);
	virtual ftkDocTemplate* SelectDocumentPath(const std::vector<ftkDocTemplate*>& templates, wxString& path, long flags, bool save = false);
	virtual ftkDocTemplate* SelectDocumentType(const std::vector<ftkDocTemplate*>& templates, bool sort = false);
	virtual ftkDocTemplate* SelectViewType(const std::vector<ftkDocTemplate*>& templates, bool sort = false);
	virtual ftkDocTemplate* FindTemplateForPath(const wxString& path);

	void AssociateTemplate(std::unique_ptr<ftkDocTemplate> temp);
	void DisassociateTemplate(ftkDocTemplate* temp);

	// Find template from document class info, may return nullptr.
	//ftkDocTemplate* FindTemplate(const wxClassInfo* documentClassInfo);

	// Find document from file name, may return nullptr.
	ftkDocument* FindDocumentByPath(const wxString& path) const;

	ftkDocument* GetCurrentDocument() const;

	void SetMaxDocsOpen(int n) { m_maxDocsOpen = n; }
	int GetMaxDocsOpen() const { return m_maxDocsOpen; }

	// Add and remove a document from the manager's list
	void AddDocument(ftkDocument* doc);
	void RemoveDocument(ftkDocument* doc);

	// closes all currently open documents
	bool CloseDocuments(bool force = true);

	// closes the specified document
	bool CloseDocument(ftkDocument* doc, bool force = false);

	// Clear remaining documents and templates
	bool Clear(bool force = true);

	// Views or windows should inform the document manager
	// when a view is going in or out of focus
	virtual void ActivateView(ftkView* view, bool activate = true);
	virtual ftkView* GetCurrentView() const { return m_currentView; }

	// This method tries to find an active view harder than GetCurrentView():
	// if the latter is null, it also checks if we don't have just a single
	// view and returns it then.
	ftkView* GetAnyUsableView() const;


	std::vector<ftkDocument*> GetDocumentsVector() const;
	std::vector<ftkDocTemplate*> GetTemplatesVector() const;

	std::list<ftkDocument*>& GetDocuments() { return m_docs; }
	std::list<std::unique_ptr<ftkDocTemplate> >& GetTemplates() { return m_templates; }

	// Return the default name for a new document (by default returns strings
	// in the form "unnamed <counter>" but can be overridden)
	virtual wxString MakeNewDocumentName();

	// Make a frame title (override this to do something different)
	virtual wxString MakeFrameTitle(ftkDocument* doc);

	virtual wxFileHistory* OnCreateFileHistory();
	virtual wxFileHistory* GetFileHistory() const { return m_fileHistory; }

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
	static ftkDocManager* GetDocumentManager() { return sm_docManager; }

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

	int               m_defaultDocumentNameCounter = 1;
	int               m_maxDocsOpen = std::numeric_limits<int>::max();
	std::list<ftkDocument*> m_docs;
	std::list<std::unique_ptr<ftkDocTemplate> > m_templates;
	ftkView* m_currentView = nullptr;
	wxFileHistory* m_fileHistory = nullptr;
	wxString          m_lastDirectory;
	static ftkDocManager* sm_docManager;

#if wxUSE_PRINTING_ARCHITECTURE
	wxPageSetupDialogData m_pageSetupDialogData;
#endif // wxUSE_PRINTING_ARCHITECTURE

	wxDECLARE_EVENT_TABLE();
};

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
	ftkDocChildFrameAnyBase(ftkDocument* doc, ftkView* view, wxWindow* win)
	{
		Create(doc, view, win);
	}

	// method which must be called for an object created using the default ctor
	//
	// note that it returns bool just for consistency with Create() methods in
	// other classes, we never return false from here
	bool Create(ftkDocument* doc, ftkView* view, wxWindow* win)
	{
		m_childDocument = doc;
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

	ftkDocument* GetDocument() const { return m_childDocument; }
	ftkView* GetView() const { return m_childView; }
	void SetDocument(ftkDocument* doc) { m_childDocument = doc; }
	void SetView(ftkView* view) { m_childView = view; }

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


	ftkDocument* m_childDocument = nullptr;
	ftkView* m_childView = nullptr;

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
class ftkDocChildFrameAny : public ChildFrame,
	public ftkDocChildFrameAnyBase
{
public:
	typedef ChildFrame BaseClass;

	// default ctor, use Create after it
	ftkDocChildFrameAny() = default;

	ftkDocChildFrameAny(const ftkDocChildFrameAny&) = delete;
	ftkDocChildFrameAny& operator=(const ftkDocChildFrameAny&) = delete;

	// ctor for a frame showing the given view of the specified document
	ftkDocChildFrameAny(ftkDocument* doc,
		ftkView* view,
		ParentFrame* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr))
	{
		Create(doc, view, parent, id, title, pos, size, style, name);
	}

	bool Create(ftkDocument* doc,
		ftkView* view,
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

		if (!ftkDocChildFrameAnyBase::Create(doc, view, this))
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
// A default child frame: we need to define it as a class just for wxRTTI,
// otherwise we could simply typedef it
// ----------------------------------------------------------------------------

using ftkDocChildFrame = ftkDocChildFrameAny<wxFrame, wxFrame>;

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

	ftkDocManager* GetDocumentManager() const { return m_docManager; }

protected:
	// This is similar to wxDocChildFrameAnyBase method with the same name:
	// while we're not an event handler ourselves and so can't override
	// TryBefore(), we provide a helper that the derived template class can use
	// from its TryBefore() implementation.
	bool TryProcessEvent(wxEvent& event);

	wxWindow* const m_frame;
	ftkDocManager* m_docManager;
};

// This is similar to wxDocChildFrameAny and is used to provide common
// implementation for both wxDocParentFrame and wxDocMDIParentFrame
template <class BaseFrame>
class ftkDocParentFrameAny : public BaseFrame,
	public ftkDocParentFrameAnyBase
{
public:
	ftkDocParentFrameAny() : ftkDocParentFrameAnyBase(this) {}

	ftkDocParentFrameAny(const ftkDocParentFrameAny&) = delete;
	ftkDocParentFrameAny& operator=(const ftkDocParentFrameAny&) = delete;

	ftkDocParentFrameAny(
		ftkDocManager* manager,
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
		ftkDocManager* manager,
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

using ftkDocParentFrame = ftkDocParentFrameAny<wxFrame>;


// ----------------------------------------------------------------------------
// Provide simple default printing facilities
// ----------------------------------------------------------------------------

#if wxUSE_PRINTING_ARCHITECTURE
class WXDLLIMPEXP_CORE ftkDocPrintout : public wxPrintout
{
public:
	ftkDocPrintout(ftkView* view = nullptr, const wxString& title = wxString());

	ftkDocPrintout(const ftkDocPrintout&) = delete;
	ftkDocPrintout& operator=(const ftkDocPrintout&) = delete;

	// implement wxPrintout methods
	bool OnPrintPage(int page) override;
	bool HasPage(int page) override;
	bool OnBeginDocument(int startPage, int endPage) override;
	void GetPageInfo(int* minPage, int* maxPage,
		int* selPageFrom, int* selPageTo) override;

	virtual ftkView* GetView() { return m_printoutView; }

protected:
	ftkView* m_printoutView;

private:
	wxDECLARE_DYNAMIC_CLASS(ftkDocPrintout);
};
#endif // wxUSE_PRINTING_ARCHITECTURE

// For compatibility with existing file formats:
// converts from/to a stream to/from a temporary file.
#if wxUSE_STD_IOSTREAM
bool wxTransferFileToStream(const wxString& filename, std::ostream& stream);
bool wxTransferStreamToFile(std::istream& stream, const wxString& filename);
#else
bool WXDLLIMPEXP_CORE
wxTransferFileToStream(const wxString& filename, wxOutputStream& stream);
bool WXDLLIMPEXP_CORE
wxTransferStreamToFile(wxInputStream& stream, const wxString& filename);
#endif // wxUSE_STD_IOSTREAM

inline std::vector<ftkView*> ftkDocument::GetViewsVector() const
{
	return { begin(m_documentViews), end(m_documentViews) };
}

inline std::vector<ftkDocument*> ftkDocManager::GetDocumentsVector() const
{
	return { begin(m_docs), end(m_docs) };
}

inline std::vector<ftkDocTemplate*> ftkDocManager::GetTemplatesVector() const
{
	std::vector<ftkDocTemplate*> templates;
	templates.reserve(m_templates.size());
	for (auto& temp : m_templates)
	{
		templates.push_back(temp.get());
	}
	return templates;
}

#if wxUSE_PRINTING_ARCHITECTURE
wxIMPLEMENT_DYNAMIC_CLASS(ftkDocPrintout, wxPrintout);
#endif

namespace
{

	wxString FindExtension(const wxString& path)
	{
		wxString ext;
		wxFileName::SplitPath(path, nullptr, nullptr, &ext);

		// VZ: extensions are considered not case sensitive - is this really a good
		//     idea?
		return ext.MakeLower();
	}

} // anonymous namespace

ftkDocument::ftkDocument() = default;

bool ftkDocument::DeleteContents()
{
	return true;
}

ftkDocument::~ftkDocument()
{
	delete m_commandProcessor;

	if (GetDocumentManager())
		GetDocumentManager()->RemoveDocument(this);

	// Not safe to do here, since it'll invoke virtual view functions
	// expecting to see valid derived objects: and by the time we get here,
	// we've called destructors higher up.
	//DeleteAllViews();
}

bool ftkDocument::CanClose()
{
	if (!OnSaveModified())
		return false;
	return true;
}

bool ftkDocument::Close()
{
	// First check if this document itself and all its children can be closed.
	if (!CanClose())
		return false;
	return OnCloseDocument();
}

bool ftkDocument::OnCloseDocument()
{
	// Tell all views that we're about to close
	NotifyClosing();
	DeleteContents();
	Modify(false);
	return true;
}

// Note that this implicitly deletes the document when the last view is
// deleted.
bool ftkDocument::DeleteAllViews()
{
	auto manager = GetDocumentManager();

	// first check if all views agree to be closed
	for (auto& view : m_documentViews)
	{
		if (!view->Close())
			return false;
	}

	// all views agreed to close, now do close them
	if (m_documentViews.empty())
	{
		// normally the document would be implicitly deleted when the last view
		// is, but if don't have any views, do it here instead
		if (manager && std::find(begin(manager->GetDocuments()), end(manager->GetDocuments()), this) != end(manager->GetDocuments()))
			delete this;
	}
	else // have views
	{
		// as we delete elements we iterate over, don't use the usual "from
		// begin to end" loop
		while (!m_documentViews.empty())
		{
			auto view = m_documentViews.front();

			bool isLastOne = m_documentViews.size() == 1;

			// this always deletes the node implicitly and if this is the last
			// view also deletes this object itself (also implicitly, great),
			// so we can't test for m_documentViews.empty() after calling this!
			delete view;

			if (isLastOne)
				break;
		}
	}

	return true;
}

ftkView* ftkDocument::GetFirstView() const
{
	if (m_documentViews.empty())
		return nullptr;

	return m_documentViews.front();
}

void ftkDocument::Modify(bool mod)
{
	if (mod != m_documentModified)
	{
		m_documentModified = mod;

		// Allow views to append asterix to the title
		ftkView* view = GetFirstView();
		if (view) view->OnChangeFilename();
	}
}

ftkDocManager* ftkDocument::GetDocumentManager() const
{
	//if (m_documentTemplate)
	//	return m_documentTemplate->GetDocumentManager();

	// Fall back on the global manager if the document doesn't have a template,
	// code elsewhere, notably in DeleteAllViews(), relies on the document
	// always being managed by some manager.
	return ftkDocManager::GetDocumentManager();
}

bool ftkDocument::OnNewDocument()
{
	// notice that there is no need to either reset nor even check the
	// modified flag here as the document itself is a new object (this is only
	// called from CreateDocument()) and so it shouldn't be saved anyhow even
	// if it is modified -- this could happen if the user code creates
	// documents pre-filled with some user-entered (and which hence must not be
	// lost) information

	SetDocumentSaved(false);

	const wxString name = GetDocumentManager()->MakeNewDocumentName();
	SetTitle(name);
	SetFilename(name, true);

	return true;
}

bool ftkDocument::Save()
{
	if (AlreadySaved())
		return true;

	if (m_documentFile.empty() || !m_savedYet)
		return SaveAs();

	return OnSaveDocument(m_documentFile);
}

bool ftkDocument::SaveAs()
{
	ftkDocTemplate* docTemplate = GetDocumentTemplate();
	if (!docTemplate)
		return false;

#ifdef wxHAS_MULTIPLE_FILEDLG_FILTERS
	wxString filter = docTemplate->GetDescription() + wxT(" (") +
		docTemplate->GetFileFilter() + wxT(")|") +
		docTemplate->GetFileFilter();

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

	wxString defaultDir = docTemplate->GetDirectory();
	if (defaultDir.empty())
	{
		defaultDir = wxPathOnly(GetFilename());
		if (defaultDir.empty())
			defaultDir = GetDocumentManager()->GetLastDirectory();
	}

	wxString fileName = wxFileSelector(_("Save As"),
		defaultDir,
		wxFileNameFromPath(GetFilename()),
		docTemplate->GetDefaultExtension(),
		filter,
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
		GetDocumentWindow());

	if (fileName.empty())
		return false; // cancelled by user

	// Files that were not saved correctly are not added to the FileHistory.
	if (!OnSaveDocument(fileName))
		return false;

	SetTitle(wxFileNameFromPath(fileName));
	SetFilename(fileName, true);    // will call OnChangeFileName automatically

	// A file that doesn't use the default extension of its document template
	// cannot be opened via the FileHistory, so we do not add it.
	if (docTemplate->FileMatchesTemplate(fileName))
	{
		GetDocumentManager()->AddFileToHistory(fileName);
	}
	//else: the user will probably not be able to open the file again, so we
	//      could warn about the wrong file-extension here

	return true;
}

bool ftkDocument::OnSaveDocument(const wxString& file)
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

bool ftkDocument::OnOpenDocument(const wxString& file)
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

	UpdateAllViews();

	return true;
}

#if wxUSE_STD_IOSTREAM
std::istream& ftkDocument::LoadObject(std::istream& stream)
#else
wxInputStream& ftkDocument::LoadObject(wxInputStream& stream)
#endif
{
	return stream;
}

#if wxUSE_STD_IOSTREAM
std::ostream& ftkDocument::SaveObject(std::ostream& stream)
#else
wxOutputStream& ftkDocument::SaveObject(wxOutputStream& stream)
#endif
{
	return stream;
}

bool ftkDocument::Revert()
{
	if (wxMessageBox
	(
		_("Discard changes and reload the last saved version?"),
		wxTheApp->GetAppDisplayName(),
		wxYES_NO | wxCANCEL | wxICON_QUESTION,
		GetDocumentWindow()
	) != wxYES)
		return false;

	if (!DoOpenDocument(GetFilename()))
		return false;

	Modify(false);
	UpdateAllViews();

	return true;
}


// Get title, or filename if no title, else unnamed
wxString ftkDocument::GetUserReadableName() const
{
	return DoGetUserReadableName();
}

wxString ftkDocument::DoGetUserReadableName() const
{
	if (!m_documentTitle.empty())
		return m_documentTitle;

	if (!m_documentFile.empty())
		return wxFileNameFromPath(m_documentFile);

	return _("unnamed");
}

wxWindow* ftkDocument::GetDocumentWindow() const
{
	ftkView* const view = GetFirstView();

	return view ? view->GetFrame() : wxTheApp->GetTopWindow();
}

FTK::CommandProcessor* ftkDocument::OnCreateCommandProcessor()
{
	return new FTK::CommandProcessor;
}

// true if safe to close
bool ftkDocument::OnSaveModified()
{
	if (IsModified())
	{
		wxMessageDialog dialogSave
		(
			GetDocumentWindow(),
			wxString::Format
			(
				_("Do you want to save changes to %s?"),
				GetUserReadableName()
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

void ftkDocument::OnSaveBeforeForceClose()
{
	if (!IsModified())
		return;

	wxMessageDialog dialogSave
	(
		GetDocumentWindow(),
		wxString::Format
		(
			_("Do you want to save changes to %s before closing it?"),
			GetUserReadableName()
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
					GetUserReadableName()
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

bool ftkDocument::Draw(wxDC& WXUNUSED(context))
{
	return true;
}

bool ftkDocument::AddView(ftkView* view)
{
	if (std::find(begin(m_documentViews), end(m_documentViews), view) == end(m_documentViews))
	{
		m_documentViews.push_back(view);
		OnChangedViewList();
	}
	return true;
}

bool ftkDocument::RemoveView(ftkView* view)
{
	if (auto it = std::find(begin(m_documentViews), end(m_documentViews), view); it != end(m_documentViews))
	{
		m_documentViews.erase(it);
		OnChangedViewList();
		return true;
	}
	return false;
}

bool ftkDocument::OnCreate(const wxString& WXUNUSED(path), long flags)
{
	return GetDocumentTemplate()->CreateView(this, ftkView::Mode::Mode_MDI, flags) != nullptr && GetDocumentTemplate()->CreateView(this, ftkView::Mode::Mode_MDI, flags) != nullptr;
}

// Called after a view is added or removed.
// The default implementation deletes the document if
// there are no more views.
void ftkDocument::OnChangedViewList()
{
	if (m_documentViews.empty() && OnSaveModified())
		delete this;
}

void ftkDocument::UpdateAllViews(ftkView* sender, wxObject* hint)
{
	for (auto& view : m_documentViews)
	{
		if (view != sender)
			view->OnUpdate(sender, hint);
	}
}

void ftkDocument::NotifyClosing()
{
	for (auto& view : m_documentViews)
	{
		view->OnClosingDocument();
	}
}

void ftkDocument::SetFilename(const wxString& filename, bool notifyViews)
{
	m_documentFile = filename;
	OnChangeFilename(notifyViews);
}

void ftkDocument::OnChangeFilename(bool notifyViews)
{
	if (notifyViews)
	{
		// Notify the views that the filename has changed
		for (auto& view : m_documentViews)
		{
			view->OnChangeFilename();
		}
	}
}

bool ftkDocument::DoSaveDocument(const wxString& file)
{
#if wxUSE_STD_IOSTREAM
	std::ofstream store(file.mb_str(), std::ios::binary);
	if (!store)
#else
	wxFileOutputStream store(file);
	if (store.GetLastError() != wxSTREAM_NO_ERROR)
#endif
	{
		wxLogError(_("File \"%s\" could not be opened for writing."), file);
		return false;
	}

	if (!SaveObject(store))
	{
		wxLogError(_("Failed to save document to the file \"%s\"."), file);
		return false;
	}

	return true;
}

bool ftkDocument::DoOpenDocument(const wxString& file)
{
#if wxUSE_STD_IOSTREAM
	std::ifstream store(file.mb_str(), std::ios::binary);
	if (!store)
#else
	wxFileInputStream store(file);
	if (store.GetLastError() != wxSTREAM_NO_ERROR || !store.IsOk())
#endif
	{
		wxLogError(_("File \"%s\" could not be opened for reading."), file);
		return false;
	}

#if wxUSE_STD_IOSTREAM
	LoadObject(store);
	if (!store)
#else
	int res = LoadObject(store).GetLastError();
	if (res != wxSTREAM_NO_ERROR && res != wxSTREAM_EOF)
#endif
	{
		wxLogError(_("Failed to read document from the file \"%s\"."), file);
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
// Document view
// ----------------------------------------------------------------------------

ftkView::ftkView(Mode mode) : m_mode(mode)
{
}

ftkView::~ftkView()
{
	if (m_viewDocument && GetDocumentManager())
		GetDocumentManager()->ActivateView(this, false);

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
		m_viewDocument->RemoveView(this);
}

void ftkView::SetDocChildFrame(ftkDocChildFrameAnyBase* docChildFrame)
{
	SetFrame(docChildFrame ? docChildFrame->GetWindow() : nullptr);
	m_docChildFrame = docChildFrame;
}

bool ftkView::TryBefore(wxEvent& event)
{
	ftkDocument* const doc = GetDocument();
	return doc && doc->ProcessEventLocally(event);
}

void ftkView::OnActivateView(bool WXUNUSED(activate),
	ftkView* WXUNUSED(activeView),
	ftkView* WXUNUSED(deactiveView))
{
}

void ftkView::OnPrint(wxDC* dc, wxObject* WXUNUSED(info))
{
	OnDraw(dc);
}

void ftkView::OnUpdate(ftkView* WXUNUSED(sender), wxObject* WXUNUSED(hint))
{
	OnChangeFilename();
}

void ftkView::OnChangeFilename()
{
	// GetFrame can return wxWindow rather than wxTopLevelWindow due to
	// generic MDI implementation so use SetLabel rather than SetTitle.
	// It should cause SetTitle() for top level windows.
	wxWindow* win = GetFrame();
	if (!win) return;

	ftkDocument* doc = GetDocument();
	if (!doc) return;

	wxString label = doc->GetUserReadableName();
	if (doc->IsModified())
	{
		label += "*";
	}
	win->SetLabel(label);
}

void ftkView::SetDocument(ftkDocument* doc)
{
	m_viewDocument = doc;
	if (doc)
		doc->AddView(this);
}

bool ftkView::Close(bool deleteWindow)
{
	return OnClose(deleteWindow);
}

void ftkView::Activate(bool activate)
{
	if (GetDocument() && GetDocumentManager())
	{
		OnActivateView(activate, this, GetDocumentManager()->GetCurrentView());
		GetDocumentManager()->ActivateView(this, activate);
	}
}

bool ftkView::OnClose(bool WXUNUSED(deleteWindow))
{
	if (auto doc = GetDocument())
	{
		if (doc->GetViews().size() == 1)
		{
			return doc->Close();
		}
	}
	return true;
}

#if wxUSE_PRINTING_ARCHITECTURE
wxPrintout* ftkView::OnCreatePrintout()
{
	return new ftkDocPrintout(this);
}
#endif // wxUSE_PRINTING_ARCHITECTURE

ftkDocTemplate::ftkDocTemplate(
	const wxString& descr,
	const wxString& filter,
	const wxString& dir,
	const wxString& ext,
	const wxString& docTypeName,
	const wxString& viewTypeName,
	std::function<ftkDocument*()> docCreate,
	std::function<ftkView* (ftkView::Mode mode)> viewCreate,
	long flags)
	: m_fileFilter(filter)
	, m_directory(dir)
	, m_description(descr)
	, m_defaultExt(ext)
	, m_docTypeName(docTypeName)
	, m_viewTypeName(viewTypeName)
{
	m_flags = flags;

	m_docCreate = docCreate;
	m_viewCreate = viewCreate;
}

// Tries to dynamically construct an object of the right class.
ftkDocument* ftkDocTemplate::CreateDocument(const wxString& path, long flags)
{
	// InitDocument() is supposed to delete the document object if its
	// initialization fails so don't use unique_ptr<> here: this is fragile
	// but unavoidable because the default implementation uses CreateView()
	// which may -- or not -- create a wxView and if it does create it and its
	// initialization fails then the view destructor will delete the document
	// (via RemoveView()) and as we can't distinguish between the two cases we
	// just have to assume that it always deletes it in case of failure
	if (auto doc = DoCreateDocument())
	{
		if (InitDocument(doc, path, flags))
		{
			doc->SetDocumentName(GetDocumentName());
			return doc;
		}
	}

	return nullptr;
}

bool
ftkDocTemplate::InitDocument(ftkDocument* doc, const wxString& path, long flags)
{
	wxScopeGuard guard = wxMakeGuard([&, this]()
		{
			// The document may be already destroyed, this happens if its view
			// creation fails as then the view being created is destroyed
			// triggering the destruction of the document as this first view is
			// also the last one. However if OnCreate() fails for any reason other
			// than view creation failure, the document is still alive and we need
			// to clean it up ourselves to avoid having a zombie document.
			//if (std::find(begin(GetDocumentManager()->GetDocuments()), end(GetDocumentManager()->GetDocuments()), doc) != end(GetDocumentManager()->GetDocuments()))
				//doc->DeleteAllViews();
		});

	doc->SetFilename(path);
	doc->SetDocumentTemplate(this);
	doc->SetCommandProcessor(doc->OnCreateCommandProcessor());

	if (!doc->OnCreate(path, flags))
		return false;

	guard.Dismiss();

	return true;
}

ftkView* ftkDocTemplate::CreateView(ftkDocument* doc, ftkView::Mode mode, long flags)
{
	std::unique_ptr<ftkView> view(DoCreateView(mode));
	if (!view)
		return nullptr;

	view->SetDocument(doc);
	if (!view->OnCreate(doc, flags))
		return nullptr;

	return view.release();
}

// The default (very primitive) format detection: check is the extension is
// that of the template
bool ftkDocTemplate::FileMatchesTemplate(const wxString& path)
{
	wxStringTokenizer parser(GetFileFilter(), wxT(";"));
	wxString anything = wxT("*");
	while (parser.HasMoreTokens())
	{
		wxString filter = parser.GetNextToken();
		wxString filterExt = FindExtension(filter);
		if (filter.IsSameAs(anything) ||
			filterExt.IsSameAs(anything) ||
			filterExt.IsSameAs(FindExtension(path)))
			return true;
	}
	return GetDefaultExtension().IsSameAs(FindExtension(path));
}

ftkDocument* ftkDocTemplate::DoCreateDocument()
{
	if (!m_docCreate)
	{
		return nullptr;
	}

	return m_docCreate();
}

ftkView* ftkDocTemplate::DoCreateView(ftkView::Mode mode)
{
	if (!m_viewCreate)
	{
		return nullptr;
	}

	return m_viewCreate(mode);
}

wxBEGIN_EVENT_TABLE(ftkDocManager, wxEvtHandler)
EVT_MENU(wxID_OPEN, ftkDocManager::OnFileOpen)
EVT_MENU(wxID_CLOSE, ftkDocManager::OnFileClose)
EVT_MENU(wxID_CLOSE_ALL, ftkDocManager::OnFileCloseAll)
EVT_MENU(wxID_REVERT, ftkDocManager::OnFileRevert)
EVT_MENU(wxID_NEW, ftkDocManager::OnFileNew)
EVT_MENU(wxID_SAVE, ftkDocManager::OnFileSave)
EVT_MENU(wxID_SAVEAS, ftkDocManager::OnFileSaveAs)
EVT_MENU(wxID_UNDO, ftkDocManager::OnUndo)
EVT_MENU(wxID_REDO, ftkDocManager::OnRedo)

// We don't know in advance how many items can there be in the MRU files
// list so set up OnMRUFile() as a handler for all menu events and do the
// check for the id of the menu item clicked inside it.
EVT_MENU(wxID_ANY, ftkDocManager::OnMRUFile)

EVT_UPDATE_UI(wxID_OPEN, ftkDocManager::OnUpdateFileOpen)
EVT_UPDATE_UI(wxID_CLOSE, ftkDocManager::OnUpdateDisableIfNoDoc)
EVT_UPDATE_UI(wxID_CLOSE_ALL, ftkDocManager::OnUpdateDisableIfNoDoc)
EVT_UPDATE_UI(wxID_REVERT, ftkDocManager::OnUpdateFileRevert)
EVT_UPDATE_UI(wxID_NEW, ftkDocManager::OnUpdateFileNew)
EVT_UPDATE_UI(wxID_SAVE, ftkDocManager::OnUpdateFileSave)
EVT_UPDATE_UI(wxID_SAVEAS, ftkDocManager::OnUpdateFileSaveAs)
EVT_UPDATE_UI(wxID_UNDO, ftkDocManager::OnUpdateUndo)
EVT_UPDATE_UI(wxID_REDO, ftkDocManager::OnUpdateRedo)

#if wxUSE_PRINTING_ARCHITECTURE
EVT_MENU(wxID_PRINT, ftkDocManager::OnPrint)
EVT_MENU(wxID_PREVIEW, ftkDocManager::OnPreview)
EVT_MENU(wxID_PRINT_SETUP, ftkDocManager::OnPageSetup)

EVT_UPDATE_UI(wxID_PRINT, ftkDocManager::OnUpdateDisableIfNoDoc)
EVT_UPDATE_UI(wxID_PREVIEW, ftkDocManager::OnUpdateDisableIfNoDoc)
// NB: we keep "Print setup" menu item always enabled as it can be used
//     even without an active document
#endif // wxUSE_PRINTING_ARCHITECTURE
wxEND_EVENT_TABLE()

ftkDocManager* ftkDocManager::sm_docManager = nullptr;

ftkDocManager::ftkDocManager(long WXUNUSED(flags), bool initialize)
{
	sm_docManager = this;
	if (initialize)
		Initialize();
}

ftkDocManager::~ftkDocManager()
{
	Clear();
	delete m_fileHistory;
	sm_docManager = nullptr;
}

// closes the specified document
bool ftkDocManager::CloseDocument(ftkDocument* doc, bool force)
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
	doc->DeleteAllViews();
	return true;
}

bool ftkDocManager::CloseDocuments(bool force)
{
	for (auto it = m_docs.begin(); it != m_docs.end();)
	{
		if (!CloseDocument(*it++, force))
			return false;
		// This assumes that documents are not connected in
		// any way, i.e. deleting one document does NOT
		// delete another.
	}
	return true;
}

bool ftkDocManager::Clear(bool force)
{
	if (!CloseDocuments(force))
		return false;

	m_currentView = nullptr;

	m_templates.clear();

	return true;
}

bool ftkDocManager::Initialize()
{
	m_fileHistory = OnCreateFileHistory();
	return true;
}

wxString ftkDocManager::GetLastDirectory() const
{
	// if we haven't determined the last used directory yet, do it now
	if (m_lastDirectory.empty())
	{
		// we're going to modify m_lastDirectory in this const method, so do it
		// via non-const self pointer instead of const this one
		ftkDocManager* const self = const_cast<ftkDocManager*>(this);

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

wxFileHistory* ftkDocManager::OnCreateFileHistory()
{
	return new wxFileHistory;
}

void ftkDocManager::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
	auto doc = GetCurrentDocument();
	if (doc)
		CloseDocument(doc);
}

void ftkDocManager::OnFileCloseAll(wxCommandEvent& WXUNUSED(event))
{
	CloseDocuments(false);
}

void ftkDocManager::OnFileNew(wxCommandEvent& WXUNUSED(event))
{
	CreateNewDocument();
}

void ftkDocManager::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
	if (!CreateDocument(wxString()))
	{
		OnOpenFileFailure();
	}
}

void ftkDocManager::OnFileRevert(wxCommandEvent& WXUNUSED(event))
{
	auto doc = GetCurrentDocument();
	if (!doc)
		return;
	doc->Revert();
}

void ftkDocManager::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
	auto doc = GetCurrentDocument();
	if (!doc)
		return;
	doc->Save();
}

void ftkDocManager::OnFileSaveAs(wxCommandEvent& WXUNUSED(event))
{
	auto doc = GetCurrentDocument();
	if (!doc)
		return;
	doc->SaveAs();
}

void ftkDocManager::OnMRUFile(wxCommandEvent& event)
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

void ftkDocManager::DoOpenMRUFile(unsigned n)
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

void ftkDocManager::OnMRUFileNotExist(unsigned n, const wxString& filename)
{
	// remove the file which we can't open from the MRU list
	RemoveFileFromHistory(n);

	// and tell the user about it
	wxLogError(_("The file '%s' doesn't exist and couldn't be opened.\n"
		"It has been removed from the most recently used files list."),
		filename);
}

#if wxUSE_PRINTING_ARCHITECTURE

void ftkDocManager::OnPrint(wxCommandEvent& WXUNUSED(event))
{
	auto view = GetAnyUsableView();
	if (!view)
		return;

	wxPrintout* printout = view->OnCreatePrintout();
	if (printout)
	{
		wxPrintDialogData printDialogData(m_pageSetupDialogData.GetPrintData());
		wxPrinter printer(&printDialogData);
		printer.Print(view->GetFrame(), printout, true);

		delete printout;
	}
}

void ftkDocManager::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
	wxPageSetupDialog dlg(wxTheApp->GetTopWindow(), &m_pageSetupDialogData);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_pageSetupDialogData = dlg.GetPageSetupData();
	}
}

wxPreviewFrame* ftkDocManager::CreatePreviewFrame(wxPrintPreviewBase* preview,
	wxWindow* parent,
	const wxString& title)
{
	return new wxPreviewFrame(preview, parent, title);
}

void ftkDocManager::OnPreview(wxCommandEvent& WXUNUSED(event))
{
	wxBusyCursor busy;
	auto view = GetAnyUsableView();
	if (!view)
		return;

	wxPrintout* printout = view->OnCreatePrintout();
	if (printout)
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

void ftkDocManager::OnUndo(wxCommandEvent& event)
{
	FTK::CommandProcessor* const cmdproc = GetCurrentCommandProcessor();
	if (!cmdproc)
	{
		event.Skip();
		return;
	}

	cmdproc->Undo();
	GetCurrentDocument()->UpdateAllViews();
}

void ftkDocManager::OnRedo(wxCommandEvent& event)
{
	FTK::CommandProcessor* const cmdproc = GetCurrentCommandProcessor();
	if (!cmdproc)
	{
		event.Skip();
		return;
	}

	cmdproc->Redo();
	GetCurrentDocument()->UpdateAllViews();
}

// Handlers for UI update commands

void ftkDocManager::OnUpdateFileOpen(wxUpdateUIEvent& event)
{
	// CreateDocument() (which is called from OnFileOpen) may succeed
	// only when there is at least a template:
	event.Enable(!GetTemplates().empty());
}

void ftkDocManager::OnUpdateDisableIfNoDoc(wxUpdateUIEvent& event)
{
	event.Enable(GetCurrentDocument() != nullptr);
}

void ftkDocManager::OnUpdateFileRevert(wxUpdateUIEvent& event)
{
	auto doc = GetCurrentDocument();
	event.Enable(doc && doc->IsModified() && doc->GetDocumentSaved());
}

void ftkDocManager::OnUpdateFileNew(wxUpdateUIEvent& event)
{
	// CreateDocument() (which is called from OnFileNew) may succeed
	// only when there is at least a template:
	event.Enable(!GetTemplates().empty());
}

void ftkDocManager::OnUpdateFileSave(wxUpdateUIEvent& event)
{
	ftkDocument* const doc = GetCurrentDocument();
	event.Enable(doc && !doc->AlreadySaved());
}

void ftkDocManager::OnUpdateFileSaveAs(wxUpdateUIEvent& event)
{
	auto const doc = GetCurrentDocument();
	event.Enable(doc);
}

void ftkDocManager::OnUpdateUndo(wxUpdateUIEvent& event)
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
	cmdproc->SetMenuStrings();
}

void ftkDocManager::OnUpdateRedo(wxUpdateUIEvent& event)
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
	cmdproc->SetMenuStrings();
}

ftkView* ftkDocManager::GetAnyUsableView() const
{
	auto view = GetCurrentView();

	if (!view && !m_docs.empty())
	{
		// if we have exactly one document, consider its view to be the current
		// one
		//
		// VZ: I'm not exactly sure why is this needed but this is how this
		//     code used to behave before the bug #9518 was fixed and it seems
		//     safer to preserve the old logic
		if (m_docs.size() == 1)
		{
			view = m_docs.front()->GetFirstView();
		}
		//else: we have more than one document
	}

	return view;
}

bool ftkDocManager::TryBefore(wxEvent& event)
{
	ftkView* const view = GetAnyUsableView();
	return view && view->ProcessEventLocally(event);
}

namespace
{

	// helper function: return only the visible templates
	std::vector<ftkDocTemplate*> GetVisibleTemplates(const std::list<std::unique_ptr<ftkDocTemplate> >& allTemplates)
	{
		// select only the visible templates
		std::vector<ftkDocTemplate*> templates;
		if (!allTemplates.empty())
		{
			templates.reserve(allTemplates.size());
			for (auto& temp : allTemplates)
			{
				if (temp->IsVisible())
				{
					templates.push_back(temp.get());
				}
			}
		}
		return templates;
	}

} // anonymous namespace

void ftkDocument::Activate()
{
	if (auto view = GetFirstView())
	{
		view->Activate(true);
		if (wxWindow* win = view->GetFrame())
			win->Raise();
	}
}

ftkDocument* ftkDocManager::FindDocumentByPath(const wxString& path) const
{
	const wxFileName fileName(path);
	for (auto& doc : m_docs)
	{
		if (fileName == wxFileName(doc->GetFilename()))
			return doc;
	}
	return nullptr;
}

ftkDocument* ftkDocManager::CreateDocument(const wxString& pathOrig, long flags)
{
	// this ought to be const but SelectDocumentType/Path() are not
	// const-correct and can't be changed as, being virtual, this risks
	// breaking user code overriding them
	auto  templates = GetVisibleTemplates(m_templates);
	if (templates.empty())
	{
		// no templates can be used, can't create document
		return nullptr;
	}


	// normally user should select the template to use but wxDOC_SILENT flag we
	// choose one ourselves
	wxString path = pathOrig;   // may be modified below
	ftkDocTemplate* temp;
	if (flags & wxDOC_SILENT)
	{
		wxASSERT_MSG(!path.empty(),
			"using empty path with wxDOC_SILENT doesn't make sense");

		temp = FindTemplateForPath(path);
		if (!temp)
		{
			wxLogWarning(_("The format of file '%s' couldn't be determined."),
				path);
		}
	}
	else // not silent, ask the user
	{
		// for the new file we need just the template, for an existing one we
		// need the template and the path, unless it's already specified
		if ((flags & wxDOC_NEW) || !path.empty())
			temp = SelectDocumentType(templates);
		else
			temp = SelectDocumentPath(templates, path, flags);
	}

	if (!temp)
		return nullptr;

	// check whether the document with this path is already opened
	if (!path.empty())
	{
		ftkDocument* const doc = FindDocumentByPath(path);
		if (doc)
		{
			// file already open, just activate it and return
			doc->Activate();
			return doc;
		}
	}

	// no, we need to create a new document


	// if we've reached the max number of docs, close the first one.
	if ((int)GetDocuments().size() >= m_maxDocsOpen)
	{
		if (!CloseDocument(GetDocuments().front()))
		{
			// can't open the new document if closing the old one failed
			return nullptr;
		}
	}


	// do create and initialize the new document finally
	auto docNew = temp->CreateDocument(path, flags);
	if (!docNew)
		return nullptr;
	AddDocument(docNew);

	wxScopeGuard guard = wxMakeObjGuard(*docNew, &ftkDocument::DeleteAllViews);

	// call the appropriate function depending on whether we're creating a
	// new file or opening an existing one
	if (!(flags & wxDOC_NEW ? docNew->OnNewDocument()
		: docNew->OnOpenDocument(path)))
	{
		return nullptr;
	}

	guard.Dismiss();

	// add the successfully opened file to MRU, but only if we're going to be
	// able to reopen it successfully later which requires the template for
	// this document to be retrievable from the file extension
	if (!(flags & wxDOC_NEW) && temp->FileMatchesTemplate(path))
		AddFileToHistory(path);

	// at least under Mac (where views are top level windows) it seems to be
	// necessary to manually activate the new document to bring it to the
	// forefront -- and it shouldn't hurt doing this under the other platforms
	docNew->Activate();

	return docNew;
}

ftkView* ftkDocManager::CreateView(ftkDocument* doc, long flags)
{
	auto templates = GetVisibleTemplates(m_templates);

	if (templates.empty() == 0)
		return nullptr;

	ftkDocTemplate* const
		temp = templates.size() == 1 ? templates.front()
		: SelectViewType(templates);

	if (!temp)
		return nullptr;

	auto view = temp->CreateView(doc, ftkView::Mode::Mode_MDI, flags);
	if (view)
		view->SetViewName(temp->GetViewName());
	return view;
}

// Not yet implemented
void
ftkDocManager::DeleteTemplate(ftkDocTemplate* WXUNUSED(temp), long WXUNUSED(flags))
{
}

// Not yet implemented
bool ftkDocManager::FlushDoc(ftkDocument* WXUNUSED(doc))
{
	return false;
}

ftkDocument* ftkDocManager::GetCurrentDocument() const
{
	ftkView* const view = GetAnyUsableView();
	return view ? view->GetDocument() : nullptr;
}

FTK::CommandProcessor* ftkDocManager::GetCurrentCommandProcessor() const
{
	ftkDocument* const doc = GetCurrentDocument();
	return doc ? doc->GetCommandProcessor() : nullptr;
}

// Make a default name for a new document
wxString ftkDocManager::MakeNewDocumentName()
{
	wxString name;

	name.Printf(_("unnamed%d"), m_defaultDocumentNameCounter);
	m_defaultDocumentNameCounter++;

	return name;
}

// Make a frame title (override this to do something different)
// If docName is empty, a document is not currently active.
wxString ftkDocManager::MakeFrameTitle(ftkDocument* doc)
{
	wxString appName = wxTheApp->GetAppDisplayName();
	wxString title;
	if (!doc)
		title = appName;
	else
	{
		wxString docName = doc->GetUserReadableName();
		title = docName + wxString(_(" - ")) + appName;
	}
	return title;
}


// Not yet implemented
ftkDocTemplate* ftkDocManager::MatchTemplate(const wxString& WXUNUSED(path))
{
	return nullptr;
}

// File history management
void ftkDocManager::AddFileToHistory(const wxString& file)
{
	if (m_fileHistory)
		m_fileHistory->AddFileToHistory(file);
}

void ftkDocManager::RemoveFileFromHistory(size_t i)
{
	if (m_fileHistory)
		m_fileHistory->RemoveFileFromHistory(i);
}

wxString ftkDocManager::GetHistoryFile(size_t i) const
{
	wxString histFile;

	if (m_fileHistory)
		histFile = m_fileHistory->GetHistoryFile(i);

	return histFile;
}

void ftkDocManager::FileHistoryUseMenu(wxMenu* menu)
{
	if (m_fileHistory)
		m_fileHistory->UseMenu(menu);
}

void ftkDocManager::FileHistoryRemoveMenu(wxMenu* menu)
{
	if (m_fileHistory)
		m_fileHistory->RemoveMenu(menu);
}

#if wxUSE_CONFIG
void ftkDocManager::FileHistoryLoad(const wxConfigBase& config)
{
	if (m_fileHistory)
		m_fileHistory->Load(config);
}

void ftkDocManager::FileHistorySave(wxConfigBase& config)
{
	if (m_fileHistory)
		m_fileHistory->Save(config);
}
#endif

void ftkDocManager::FileHistoryAddFilesToMenu(wxMenu* menu)
{
	if (m_fileHistory)
		m_fileHistory->AddFilesToMenu(menu);
}

void ftkDocManager::FileHistoryAddFilesToMenu()
{
	if (m_fileHistory)
		m_fileHistory->AddFilesToMenu();
}

size_t ftkDocManager::GetHistoryFilesCount() const
{
	return m_fileHistory ? m_fileHistory->GetCount() : 0;
}


// Find out the document template via matching in the document file format
// against that of the template
ftkDocTemplate* ftkDocManager::FindTemplateForPath(const wxString& path)
{
	// Find the template which this extension corresponds to
	for (auto& temp : m_templates)
	{
		if (temp->FileMatchesTemplate(path))
		{
			return temp.get();
		}
	}
	return nullptr;
}

// Prompts user to open a file, using file specs in templates.
// Must extend the file selector dialog or implement own; OR
// match the extension to the template extension.

ftkDocTemplate* ftkDocManager::SelectDocumentPath(const std::vector<ftkDocTemplate*>& templates,
	wxString& path,
	long WXUNUSED(flags),
	bool WXUNUSED(save))
{
#ifdef wxHAS_MULTIPLE_FILEDLG_FILTERS
	wxString descrBuf;

	for (auto& temp : templates)
	{
		if (temp->IsVisible())
		{
			// add a '|' to separate this filter from the previous one
			if (!descrBuf.empty())
				descrBuf << wxT('|');

			descrBuf << temp->GetDescription()
				<< wxT(" (") << temp->GetFileFilter() << wxT(") |")
				<< temp->GetFileFilter();
		}
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

	ftkDocTemplate* theTemplate = nullptr;
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
			theTemplate = templates[FilterIndex];
			if (theTemplate)
			{
				// But don't use this template if it doesn't match the path as
				// can happen if the user specified the extension explicitly
				// but didn't bother changing the filter.
				if (!theTemplate->FileMatchesTemplate(path))
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

ftkDocTemplate* ftkDocManager::SelectDocumentType(const std::vector<ftkDocTemplate*>& templates, bool sort)
{
	std::vector<wxString> strings;
	std::vector<ftkDocTemplate*> data;

	for (auto& templ : templates)
	{
		if (templ->IsVisible())
		{
			if (std::find_if(begin(data), end(data), [&](auto& t) {return templ->GetDocumentName() == t->GetDocumentName() && templ->GetViewName() == t->GetViewName();}) == end(data))
			{
				strings.push_back(templ->GetDescription());
				data.push_back(templ);
			}
		}
	}

	if (sort)
	{
		std::vector<std::pair<wxString, ftkDocTemplate*> > pairs;
		std::transform(begin(strings), end(strings), begin(data), std::back_inserter(pairs), std::make_pair<const wxString&, ftkDocTemplate* const &>);
		std::sort(begin(pairs), end(pairs)); // ascending sort
		for (size_t i = 0; i < pairs.size(); i++)
		{
			strings[i] = pairs[i].first;
			data[i] = pairs[i].second;
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
	// propose the user to choose one of several
	return (ftkDocTemplate*)wxGetSingleChoiceData
	(
		_("Select a document template"),
		_("Templates"),
		strings,
		(void**)data.data()
	);
}

ftkDocTemplate* ftkDocManager::SelectViewType(const std::vector<ftkDocTemplate*>& templates, bool sort)
{
	std::vector<wxString> strings;
	std::vector<ftkDocTemplate*> data;

	for (auto& templ : templates)
	{
		if (templ->IsVisible() && !templ->GetViewName().empty())
		{
			if (std::find_if(begin(data), end(data), [&](ftkDocTemplate* t) {return templ->GetViewName() == t->GetViewName(); }) == end(data))
			{
				strings.push_back(templ->GetViewName());
				data.push_back(templ);
			}
		}
	}

	if (sort)
	{
		std::vector<std::pair<wxString, ftkDocTemplate*> > pairs;
		std::transform(begin(strings), end(strings), begin(data), std::back_inserter(pairs), std::make_pair<const wxString&, ftkDocTemplate* const&>);
		std::sort(begin(pairs), end(pairs)); // ascending sort
		for (size_t i = 0; i < pairs.size(); i++)
		{
			strings[i] = pairs[i].first;
			data[i] = pairs[i].second;
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
	return (ftkDocTemplate*)wxGetSingleChoiceData
	(
		_("Select a document view"),
		_("Views"),
		strings,
		(void**)data.data()
	);
}

void ftkDocManager::AssociateTemplate(std::unique_ptr<ftkDocTemplate> temp)
{
	if (std::find(begin(m_templates), end(m_templates), temp) == end(m_templates))
	{
		m_templates.push_back(std::move(temp));
	}
}

void ftkDocManager::DisassociateTemplate(ftkDocTemplate* temp)
{
	m_templates.remove_if([temp](auto& elem) {return elem.get() == temp; });
}

// Add and remove a document from the manager's list
void ftkDocManager::AddDocument(ftkDocument* doc)
{
	if (std::find(begin(m_docs), end(m_docs), doc) == end(m_docs))
	{
		m_docs.push_back(doc);
	}
}

void ftkDocManager::RemoveDocument(ftkDocument* doc)
{
	if (auto it = std::find(begin(m_docs), end(m_docs), doc); it != end(m_docs))
	{
		m_docs.erase(it);
	}
}

// Views or windows should inform the document manager
// when a view is going in or out of focus
void ftkDocManager::ActivateView(ftkView* view, bool activate)
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
	return m_childDocument->GetDocumentManager()->ProcessEventLocally(event);
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

	m_childDocument = nullptr;

	return true;
}

bool ftkDocParentFrameAnyBase::TryProcessEvent(wxEvent& event)
{
	if (!m_docManager)
		return false;

	// If we have an active view, its associated child frame may have
	// already forwarded the event to wxDocManager, check for this:
	if (ftkView* const view = m_docManager->GetAnyUsableView())
	{
		ftkDocChildFrameAnyBase* const childFrame = view->GetDocChildFrame();
		if (childFrame && childFrame->HasAlreadyProcessed(event))
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

namespace
{

	wxString GetAppropriateTitle(const ftkView* view, const wxString& titleGiven)
	{
		wxString title(titleGiven);
		if (title.empty())
		{
			if (view && view->GetDocument())
				title = view->GetDocument()->GetUserReadableName();
			else
				title = _("Printout");
		}

		return title;
	}

} // anonymous namespace

ftkDocPrintout::ftkDocPrintout(ftkView* view, const wxString& title)
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

#if wxUSE_STD_IOSTREAM

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

#else // !wxUSE_STD_IOSTREAM

bool wxTransferFileToStream(const wxString& filename, wxOutputStream& stream)
{
#if wxUSE_FFILE
	wxFFile file(filename, wxT("rb"));
#elif wxUSE_FILE
	wxFile file(filename, wxFile::read);
#endif
	if (!file.IsOpened())
		return false;

	char buf[4096];

	size_t nRead;
	do
	{
		nRead = file.Read(buf, WXSIZEOF(buf));
		if (file.Error())
			return false;

		stream.Write(buf, nRead);
		if (!stream)
			return false;
	} while (!file.Eof());

	return true;
}

bool wxTransferStreamToFile(wxInputStream& stream, const wxString& filename)
{
#if wxUSE_FFILE
	wxFFile file(filename, wxT("wb"));
#elif wxUSE_FILE
	wxFile file(filename, wxFile::write);
#endif
	if (!file.IsOpened())
		return false;

	char buf[4096];
	for (;; )
	{
		stream.Read(buf, WXSIZEOF(buf));

		const size_t nRead = stream.LastRead();
		if (!nRead)
		{
			if (stream.Eof())
				break;

			return false;
		}

		if (!file.Write(buf, nRead))
			return false;
	}

	return true;
}

#endif // wxUSE_STD_IOSTREAM/!wxUSE_STD_IOSTREAM



#if wxUSE_MDI_ARCHITECTURE
using ftkDocMDIParentFrame = ftkDocParentFrameAny<wxMDIParentFrame> ;
using ftkDocMDIChildFrame = ftkDocChildFrameAny<wxMDIChildFrame, wxMDIParentFrame>;
#endif // wxUSE_MDI_ARCHITECTURE


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
	wxFrame* CreateChildFrame(ftkView* view, bool isCanvas);

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
	MyCanvas* m_canvas;
	wxMenu* m_menuEdit;

	ftkView::Mode m_mode;

	wxDECLARE_EVENT_TABLE();
};

//wxDECLARE_APP(App);

class ftkApp : public wxApp
{
public:
	ftkApp();

	ftkApp(const ftkApp&) = delete;
	ftkApp& operator=(const ftkApp&) = delete;

	bool OnInit() override;
	int OnExit() override;

	void OnInitCmdLine(wxCmdLineParser& parser) override;
	bool OnCmdLineParsed(wxCmdLineParser& parser) override;
};

wxDECLARE_APP(ftkApp);
wxIMPLEMENT_APP(ftkApp);

ftkApp::ftkApp()
{
#ifdef WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF) | _CRTDBG_LEAK_CHECK_DF);
#endif
	SetAppName("ftkApp");
	SetAppDisplayName("ftkApp");
/*#if wxUSE_MDI_ARCHITECTURE
	m_mode = ftkView::Mode_MDI;
#else
	m_mode = ftkView::Mode_SDI;
#endif

	m_canvas = nullptr;
	m_menuEdit = nullptr;*/
}

bool ftkApp::OnInit()
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

	wxFrame* frame = nullptr;
	auto m_mode = ftkView::Mode::Mode_MDI;
	ftkDocManager* docManager = nullptr;

	switch (m_mode)
	{
#if wxUSE_MDI_ARCHITECTURE
	case ftkView::Mode::Mode_MDI:
		frame = new ftkDocMDIParentFrame(docManager, nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr));
		break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	case ftkView::Mode::Mode_AUI:
		frame = new ftkDocParentFrameAny<wxAuiMDIParentFrame>
			(
				docManager, nullptr, wxID_ANY,
				GetAppDisplayName(),
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(1280, 720), nullptr)
			);
		break;
#endif // wxUSE_AUI

	case ftkView::Mode::Mode_SDI:
	case ftkView::Mode::Mode_Single:
		frame = new ftkDocParentFrame(docManager, nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr));
		break;
	}

	frame->SetIcon(wxICON(doc));
	frame->Centre();
	frame->Show();

	return true;
}

int ftkApp::OnExit()
{
	return wxApp::OnExit();
}

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

} // namespace CmdLineOption

void ftkApp::OnInitCmdLine(wxCmdLineParser& parser)
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

bool ftkApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	int numModeOptions = 0;
	auto m_mode = ftkView::Mode::Mode_MDI;

#if wxUSE_MDI_ARCHITECTURE
	if (parser.Found(CmdLineOption::MDI))
	{
		m_mode = ftkView::Mode::Mode_MDI;
		numModeOptions++;
	}
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	if (parser.Found(CmdLineOption::AUI))
	{
		m_mode = ftkView::Mode::Mode_AUI;
		numModeOptions++;
	}
#endif // wxUSE_AUI

	if (parser.Found(CmdLineOption::SDI))
	{
		m_mode = ftkView::Mode::Mode_SDI;
		numModeOptions++;
	}

	if (parser.Found(CmdLineOption::SINGLE))
	{
		m_mode = ftkView::Mode::Mode_Single;
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
	MyCanvas(ftkView* view, wxWindow* parent = nullptr);
	~MyCanvas() override;

	void OnDraw(wxDC& dc) override;

	// in a normal multiple document application a canvas is associated with
	// one view from the beginning until the end, but to support the single
	// document mode in which all documents reuse the same App::GetCanvas()
	// we need to allow switching the canvas from one view to another one

	void SetView(ftkView* view)
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

	ftkView* m_view;

	// the segment being currently drawn or nullptr if none
	DoodleSegment* m_currentSegment = nullptr;

	// the last mouse press position
	wxPoint m_lastMousePos;

	wxDECLARE_EVENT_TABLE();
};

//wxIMPLEMENT_APP(App);

wxBEGIN_EVENT_TABLE(App, wxApp)
EVT_MENU(wxID_ABOUT, App::OnAbout)
EVT_MENU(wxID_CLEAR, App::OnForceCloseAll)
wxEND_EVENT_TABLE()

App::App()
{
#ifdef WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF) | _CRTDBG_LEAK_CHECK_DF);
#endif
	SetAppName("wxWidgetsApp");
	SetAppDisplayName("wxWidgetsApp");
#if wxUSE_MDI_ARCHITECTURE
	m_mode = ftkView::Mode::Mode_MDI;
#else
	m_mode = ftkView::Mode_SDI;
#endif

	m_canvas = nullptr;
	m_menuEdit = nullptr;
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
		m_mode = ftkView::Mode::Mode_MDI;
		numModeOptions++;
	}
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	if (parser.Found(CmdLineOption::AUI))
	{
		m_mode = ftkView::Mode::Mode_AUI;
		numModeOptions++;
	}
#endif // wxUSE_AUI

	if (parser.Found(CmdLineOption::SDI))
	{
		m_mode = ftkView::Mode::Mode_SDI;
		numModeOptions++;
	}

	if (parser.Found(CmdLineOption::SINGLE))
	{
		m_mode = ftkView::Mode::Mode_Single;
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
	wxDocManager::GetDocumentManager()->CreateNewDocument();
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
	auto docManager = new ftkDocManager;

	//// Create a template relating drawing documents to their views
	docManager->AssociateTemplate(std::make_unique<ftkDocTemplate>("Drawing", "*.drw", "", "drw",
	    "Drawing Doc", "Drawing View",
		[]() {return new DrawingDocument; }, [](ftkView::Mode mode) {return new DrawingView(mode); }));

	if (m_mode == ftkView::Mode::Mode_Single)
	{
		// If we've only got one window, we only get to edit one document at a
		// time. Therefore no text editing, just doodling.
		docManager->SetMaxDocsOpen(1);
	}
	else // multiple documents mode: allow documents of different types
	{
		// Create a template relating text documents to their views
		docManager->AssociateTemplate(std::make_unique<ftkDocTemplate>("Text", "*.txt;*.text", "", "txt;text",
			"Text Doc", "Text View",
			[]() { return new TextEditDocument; }, [](ftkView::Mode mode) {return new TextEditView(mode); }));
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
	case ftkView::Mode::Mode_MDI:
		frame = new ftkDocMDIParentFrame(docManager, nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr));
		break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	case ftkView::Mode::Mode_AUI:
		frame = new ftkDocParentFrameAny<wxAuiMDIParentFrame>
			(
				docManager, nullptr, wxID_ANY,
				GetAppDisplayName(),
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(1280, 720), nullptr)
			);
		break;
#endif // wxUSE_AUI

	case ftkView::Mode::Mode_SDI:
	case ftkView::Mode::Mode_Single:
		frame = new ftkDocParentFrame(docManager, nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr));
		break;
	}

	// and its menu bar
	wxMenu* menuFile = new wxMenu;

	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);

	if (m_mode == ftkView::Mode::Mode_Single)
		AppendDocumentFileCommands(menuFile, true);

	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	// A nice touch: a history of files visited. Use this menu.
	docManager->FileHistoryUseMenu(menuFile);
#if wxUSE_CONFIG
	docManager->FileHistoryLoad(*wxConfig::Get());
#endif // wxUSE_CONFIG


	if (m_mode == ftkView::Mode::Mode_Single)
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
			docManager->CreateDocument(file, wxDOC_SILENT);
	}

	return true;
}

int App::OnExit()
{
	auto manager = ftkDocManager::GetDocumentManager();
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
	wxMenuBar* menubar = new wxMenuBar;

	menubar->Append(file, wxGetStockLabel(wxID_FILE));

	if (edit)
		menubar->Append(edit, wxGetStockLabel(wxID_EDIT));

	wxMenu* help = new wxMenu;
	help->Append(wxID_ABOUT);
	menubar->Append(help, wxGetStockLabel(wxID_HELP));

	frame->SetMenuBar(menubar);
}

wxFrame* App::CreateChildFrame(ftkView* view, bool isCanvas)
{
	// create a child frame of appropriate class for the current mode
	wxFrame* subframe = nullptr;
	ftkDocument* doc = view->GetDocument();
	switch (view->GetMode())
#if wxUSE_MDI_ARCHITECTURE
	{
	case ftkView::Mode::Mode_MDI:
		subframe = new ftkDocMDIChildFrame
		(
			doc,
			view,
			wxStaticCast(GetTopWindow(), ftkDocMDIParentFrame),
			wxID_ANY,
			"Child Frame",
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(640, 480), nullptr)
		);
		break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
	case ftkView::Mode::Mode_AUI:
		subframe = new ftkDocChildFrameAny<wxAuiMDIChildFrame, wxAuiMDIParentFrame>
			(
				doc,
				view,
				wxStaticCast(GetTopWindow(), wxAuiMDIParentFrame),
				wxID_ANY,
				"Child Frame",
				wxDefaultPosition,
				wxWindow::FromDIP(wxSize(640, 480), nullptr)
			);
		break;
#endif // wxUSE_AUI

	case ftkView::Mode::Mode_SDI:
	case ftkView::Mode::Mode_Single:
		subframe = new ftkDocChildFrame
		(
			doc,
			view,
			wxStaticCast(GetTopWindow(), ftkDocParentFrame),
			wxID_ANY,
			"Child Frame",
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(640, 480), nullptr)
		);

		subframe->Centre();
		break;
	}

	wxMenu* menuFile = new wxMenu;

	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);
	AppendDocumentFileCommands(menuFile, isCanvas);
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuEdit;
	if (isCanvas)
	{
		menuEdit = CreateDrawingEditMenu();

		doc->GetCommandProcessor()->SetEditMenu(menuEdit);
		doc->GetCommandProcessor()->Initialize();
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
	ftkDocManager::GetDocumentManager()->CloseDocuments(true);
}

void App::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString modeName = "Mixed mode";

	const int docsCount =
		ftkDocManager::GetDocumentManager()->GetDocumentsVector().size();

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


bool TextEditDocument::OnCreate(const wxString& path, long flags)
{
	if (!ftkDocument::OnCreate(path, flags))
		return false;

	// subscribe to changes in the text control to update the document state
	// when it's modified
	GetTextCtrl()->Bind(wxEVT_TEXT, &TextEditDocument::OnTextChange, this);

	return true;
}

// Since text windows have their own method for saving to/loading from files,
// we override DoSave/OpenDocument instead of Save/LoadObject
bool TextEditDocument::DoSaveDocument(const wxString& filename)
{
	return GetTextCtrl()->SaveFile(filename);
}

bool TextEditDocument::DoOpenDocument(const wxString& filename)
{
	if (!GetTextCtrl()->LoadFile(filename))
		return false;

	// we're not modified by the user yet
	Modify(false);

	return true;
}

bool TextEditDocument::IsModified() const
{
	wxTextCtrl* wnd = GetTextCtrl();
	return ftkDocument::IsModified() || (wnd && wnd->IsModified());
}

void TextEditDocument::Modify(bool modified)
{
	ftkDocument::Modify(modified);

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
	ftkView* view = GetFirstView();
	return view ? wxStaticCast(view, TextEditView)->GetText() : nullptr;
}

wxBEGIN_EVENT_TABLE(TextEditView, ftkView)
EVT_MENU(wxID_COPY, TextEditView::OnCopy)
EVT_MENU(wxID_PASTE, TextEditView::OnPaste)
EVT_MENU(wxID_SELECTALL, TextEditView::OnSelectAll)
wxEND_EVENT_TABLE()

bool TextEditView::OnCreate(ftkDocument* doc, long flags)
{
	if (!ftkView::OnCreate(doc, flags))
		return false;

	wxFrame* frame = nullptr;// wxGetApp().CreateChildFrame(this, false);
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
	if (!ftkView::OnClose(deleteWindow))
		return false;

	Activate(false);

	if (GetMode() == ftkView::Mode::Mode_Single)
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
		const wxString& name,
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
		: DrawingCommand(doc, "Add new segment", segment)
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
		: DrawingCommand(doc, "Remove last segment")
	{
	}

	virtual bool Do() override { return DoRemove(); }
	virtual bool Undo() override { return DoAdd(); }
};

/*DocumentOstream& DrawingDocument::SaveObject(DocumentOstream& ostream)
{
#if wxUSE_STD_IOSTREAM
	DocumentOstream& stream = ostream;
#else
	wxTextOutputStream stream(ostream);
#endif

	ftkDocument::SaveObject(ostream);

	const wxInt32 count = m_doodleSegments.size();
	stream << count << '\n';

	for (int n = 0; n < count; n++)
	{
		m_doodleSegments[n].SaveObject(ostream);
		stream << '\n';
	}

	return ostream;
}*/

/*DocumentIstream& DrawingDocument::LoadObject(DocumentIstream& istream)
{
#if wxUSE_STD_IOSTREAM
	DocumentIstream& stream = istream;
#else
	wxTextInputStream stream(istream);
#endif

	ftkDocument::LoadObject(istream);

	wxInt32 count = 0;
	stream >> count;
	if (count < 0)
	{
		wxLogWarning("Drawing document corrupted: invalid segments count.");
#if wxUSE_STD_IOSTREAM
		istream.clear(std::ios::badbit);
#else
		istream.Reset(wxSTREAM_READ_ERROR);
#endif
		return istream;
	}

	for (int n = 0; n < count; n++)
	{
		DoodleSegment segment;
		segment.LoadObject(istream);
		m_doodleSegments.push_back(segment);
	}

	return istream;
}*/

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

/*DocumentOstream& DoodleSegment::SaveObject(DocumentOstream& ostream)
{
#if wxUSE_STD_IOSTREAM
	DocumentOstream& stream = ostream;
#else
	wxTextOutputStream stream(ostream);
#endif

	const wxInt32 count = m_lines.size();
	stream << count << '\n';

	for (int n = 0; n < count; n++)
	{
		const DoodleLine& line = m_lines[n];
		stream
			<< line.x1 << ' '
			<< line.y1 << ' '
			<< line.x2 << ' '
			<< line.y2 << '\n';
	}

	return ostream;
}

DocumentIstream& DoodleSegment::LoadObject(DocumentIstream& istream)
{
#if wxUSE_STD_IOSTREAM
	DocumentIstream& stream = istream;
#else
	wxTextInputStream stream(istream);
#endif

	wxInt32 count = 0;
	stream >> count;

	for (int n = 0; n < count; n++)
	{
		DoodleLine line;
		stream
			>> line.x1
			>> line.y1
			>> line.x2
			>> line.y2;
		m_lines.push_back(line);
	}

	return istream;
}*/
wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
wxEND_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(ftkView* view, wxWindow* parent)
	: wxScrolledWindow(parent ? parent : view->GetFrame())
{
	m_view = view;
	m_lastMousePos = wxDefaultPosition;

	SetCursor(wxCursor(wxCURSOR_PENCIL));

	// this is completely arbitrary and is done just for illustration purposes
	SetVirtualSize(1000, 1000);
	SetScrollRate(20, 20);

	SetBackgroundColour(*wxWHITE);
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
			DrawingDocument* const
				doc = wxStaticCast(m_view->GetDocument(), DrawingDocument);

			doc->GetCommandProcessor()->Submit(
				std::make_unique<DrawingAddSegmentCommand>(doc, *m_currentSegment));

			doc->Modify(true);
			doc->UpdateAllViews();
		}

		wxDELETE(m_currentSegment);
	}

	// is this the start of a new segment?
	if (m_lastMousePos != wxDefaultPosition && event.Dragging())
	{
		if (!m_currentSegment)
			m_currentSegment = new DoodleSegment;

		m_currentSegment->AddLine(m_lastMousePos, pt);

		dc.DrawLine(m_lastMousePos, pt);
	}

	m_lastMousePos = pt;
}

wxBEGIN_EVENT_TABLE(DrawingView, ftkView)
EVT_MENU(wxID_CUT, DrawingView::OnCut)
wxEND_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(ftkDocument* doc, long flags)
{
	if (!ftkView::OnCreate(doc, flags))
		return false;

	auto& app = wxGetApp();
	if (GetMode() != ftkView::Mode::Mode_Single)
	{
		// create a new window and canvas inside it
		wxFrame* frame = nullptr;// app.CreateChildFrame(this, true);
		wxASSERT(frame == GetFrame());
		m_canvas = new MyCanvas(this);
		frame->Show();
	}
	else // single document mode
	{
		// reuse the existing window and canvas
		//m_canvas = app.GetMainWindowCanvas();
		//m_canvas->SetView(this);

		// Initialize the edit menu Undo and Redo items
		//doc->GetCommandProcessor()->SetEditMenu(app.GetMainWindowEditMenu());
		doc->GetCommandProcessor()->Initialize();
	}

	return true;
}

// Sneakily gets used for default print/preview as well as drawing on the
// screen.
void DrawingView::OnDraw(wxDC* dc)
{
	dc->SetPen(*wxBLACK_PEN);

	// simply draw all lines of all segments
	const DoodleSegments& segments = GetDocument()->GetSegments();
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
	return wxStaticCast(ftkView::GetDocument(), DrawingDocument);
}
void DrawingView::OnUpdate(ftkView* sender, wxObject* hint)
{
	ftkView::OnUpdate(sender, hint);
	if (m_canvas)
		m_canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool DrawingView::OnClose(bool deleteWindow)
{
	if (!ftkView::OnClose(deleteWindow))
		return false;

	Activate(false);

	// Clear the canvas in single-window mode in which it stays alive
	if (GetMode() == ftkView::Mode::Mode_Single)
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
	DrawingDocument* const doc = GetDocument();

	doc->GetCommandProcessor()->Submit(std::make_unique<DrawingRemoveSegmentCommand>(doc));
	doc->Modify(true);
	doc->UpdateAllViews();
}

