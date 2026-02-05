#ifdef jkhhkkhkhkhkh
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "ftk/Gui/wxw/MainFrame.h"

//#include <wx/mdi.h>
#include <wx/aui/aui.h>



#include "MainWindow.h"

class View
{
public:
	bool OnCreate();// { return true; }
};

View* CreateView()
{
	auto view = std::make_unique<View>();
	if (!view)
	{
		return nullptr;
	}

	if (!view->OnCreate())
	{
		return nullptr;
	}

	return view.release();
}

class App : public wxApp
{
public:
	App()
	{
#ifdef WIN32
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF) | _CRTDBG_LEAK_CHECK_DF);
#endif
		SetAppName("wxWidgetsApp");
		SetAppDisplayName("wxWidgetsApp");
	}

	~App() override
	{
	}

	bool OnInit() override
	{
		if (!wxApp::OnInit())
			return false;
		auto frame = wxw::CreateFrame(GetAppDisplayName());

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
		CreateView();
		return true;
	}

	int  OnExit() override
	{
		return wxApp::OnExit();
	}

	wxFrame* CreateChildFrame(View* view, bool isCanvas);

};

wxDECLARE_APP(App);
wxIMPLEMENT_APP(App);

class ChildFrameAnyBase
{
public:
	// default ctor, use Create() after it
	ChildFrameAnyBase()
	{
		//m_childDocument = nullptr;
		m_childView = nullptr;
		m_win = nullptr;
		m_lastEvent = nullptr;
	}

	// full ctor equivalent to using the default one and Create()
	ChildFrameAnyBase(
		//wxDocument* doc,
		View* view,
		wxWindow* win)
	{
		Create(
			//doc,
			view,
			win);
	}

	// method which must be called for an object created using the default ctor
	//
	// note that it returns bool just for consistency with Create() methods in
	// other classes, we never return false from here
	bool Create(
		//wxDocument* doc,
		View* view,
		wxWindow* win)
	{
		//m_childDocument = doc;
		m_childView = view;
		m_win = win;

		//if (view)
			//view->SetDocChildFrame(this);

		return true;
	}

	// dtor doesn't need to be virtual, an object should never be destroyed via
	// a pointer to this class
	~ChildFrameAnyBase()
	{
		// prevent the view from deleting us if we're being deleted directly
		// (and not via Close() + Destroy())
		//if (m_childView)
			//m_childView->SetDocChildFrame(nullptr);
	}

	//wxDocument* GetDocument() const { return m_childDocument; }
	View* GetView() const { return m_childView; }
	//void SetDocument(wxDocument* doc) { m_childDocument = doc; }
	void SetView(View* view) { m_childView = view; }

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
	//bool TryProcessEvent(wxEvent& event);

	// called from EVT_CLOSE handler in the frame: check if we can close and do
	// cleanup if so; veto the event otherwise
	//bool CloseView(wxCloseEvent& event);


	//wxDocument* m_childDocument;
	View* m_childView;

	// the associated window: having it here is not terribly elegant but it
	// allows us to avoid having any virtual functions in this class
	wxWindow* m_win;

private:
	// Pointer to the last processed event used to avoid sending the same event
	// twice to wxDocManager, from here and from wxDocParentFrameAnyBase.
	wxEvent* m_lastEvent;
};

template <class ChildFrame, class ParentFrame>
class ChildFrameAny : public ChildFrame,
	public ChildFrameAnyBase
{
public:
	typedef ChildFrame BaseClass;

	// default ctor, use Create after it
	ChildFrameAny() = default;

	// ctor for a frame showing the given view of the specified document
	ChildFrameAny(
		//wxDocument* doc,
		View* view,
		ParentFrame* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr))
	{
		Create(
			//doc,
			view, parent, id, title, pos, size, style, name);
	}

	bool Create(
		//wxDocument* doc,
		View* view,
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

		if (!ChildFrameAnyBase::Create(
			//doc,
			view, this))
			return false;

		if (!BaseClass::Create(parent, id, title, pos, size, style, name))
			return false;

		return true;
	}

protected:
	// hook the child view into event handlers chain here
	//virtual bool TryBefore(wxEvent& event) override
	//{
	//	return TryProcessEvent(event) || BaseClass::TryBefore(event);
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
typedef
ChildFrameAny<wxMDIChildFrame, wxMDIParentFrame> MDIChildFrameBase;

class MDIChildFrame : public MDIChildFrameBase
{
public:
	MDIChildFrame() = default;

	MDIChildFrame(
		//wxDocument* doc,
		View* view,
		wxMDIParentFrame* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr))
		: MDIChildFrameBase(
			//doc,
			view,
			parent, id, title, pos, size, style, name)
	{
	}
};


bool View::OnCreate()
{
	auto& app = wxGetApp();
	app.CreateChildFrame(this, true);
	return true;
}

wxFrame* App::CreateChildFrame(View* view, bool isCanvas)
{
	wxFrame* result = nullptr;
	result = new MDIChildFrame
		(
//			doc,
			view,
			static_cast<wxw::MainFrame<wxMDIParentFrame, wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL>*> (GetTopWindow()),
			wxID_ANY,
			"Child Frame",
			wxDefaultPosition,
			wxSize(300, 300)
		);
	result->Show();

	return result;
}

#endif




/////////////////////////////////////////////////////////////////////////////
// Name:        wx/docview.h
// Purpose:     Doc/View classes
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCH__
#define _WX_DOCH__

#include "wx/defs.h"

#if wxUSE_DOC_VIEW_ARCHITECTURE

#include "wx/string.h"
#include "wx/frame.h"
#include "wx/filehistory.h"
#include "wx/vector.h"
#include "wx/textctrl.h"

#if wxUSE_PRINTING_ARCHITECTURE
#include "wx/print.h"
#endif

#include <list>

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxDocument;
class WXDLLIMPEXP_FWD_CORE wxView;
class WXDLLIMPEXP_FWD_CORE wxDocTemplate;
class WXDLLIMPEXP_FWD_CORE wxDocManager;
class WXDLLIMPEXP_FWD_CORE wxPrintInfo;
class WXDLLIMPEXP_FWD_CORE wxCommandProcessor;
class WXDLLIMPEXP_FWD_BASE wxConfigBase;

class wxDocChildFrameAnyBase;

#if wxUSE_STD_IOSTREAM
#include "wx/iosfwrap.h"
#else
#include "wx/stream.h"
#endif

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

#define wxMAX_FILE_HISTORY 9

typedef wxVector<wxDocument*> wxDocVector;
typedef wxVector<wxView*> wxViewVector;
typedef wxVector<wxDocTemplate*> wxDocTemplateVector;

class WXDLLIMPEXP_CORE wxDocument : public wxEvtHandler
{
public:
    wxDocument(wxDocument* parent = nullptr);
    virtual ~wxDocument();

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
    // implementation (wxDocument::OnChangeFilename)
    virtual void OnChangeFilename(bool notifyViews);

    // Called by framework if created automatically by the default document
    // manager: gives document a chance to initialise and (usually) create a
    // view
    virtual bool OnCreate(const wxString& path, long flags);

    // By default, creates a base wxCommandProcessor.
    virtual wxCommandProcessor* OnCreateCommandProcessor();
    virtual wxCommandProcessor* GetCommandProcessor() const
    {
        return m_commandProcessor;
    }
    virtual void SetCommandProcessor(wxCommandProcessor* proc)
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

    virtual bool AddView(wxView* view);
    virtual bool RemoveView(wxView* view);

    wxViewVector GetViewsVector() const;

    wxList& GetViews() { return m_documentViews; }
    const wxList& GetViews() const { return m_documentViews; }

    wxView* GetFirstView() const;

    virtual void UpdateAllViews(wxView* sender = nullptr, wxObject* hint = nullptr);
    virtual void NotifyClosing();

    // Remove all views (because we're closing the document)
    virtual bool DeleteAllViews();

    // Other stuff
    virtual wxDocManager* GetDocumentManager() const;
    virtual wxDocTemplate* GetDocumentTemplate() const
    {
        return m_documentTemplate;
    }
    virtual void SetDocumentTemplate(wxDocTemplate* temp)
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

    // Returns true if this document is a child document corresponding to a
    // part of the parent document and not a disk file as usual.
    bool IsChildDocument() const { return m_documentParent != nullptr; }

    // Ask the user if the document should be saved if it's modified and save
    // it if necessary.
    //
    // Returns false if the user cancelled closing or if saving failed.
    bool CanClose();

protected:
    wxList                m_documentViews;
    wxString              m_documentFile;
    wxString              m_documentTitle;
    wxString              m_documentTypeName;
    wxDocTemplate* m_documentTemplate;
    bool                  m_documentModified;

    // if the document parent is non-null, it's a pseudo-document corresponding
    // to a part of the parent document which can't be saved or loaded
    // independently of its parent and is always closed when its parent is
    wxDocument* m_documentParent;

    wxCommandProcessor* m_commandProcessor;
    bool                  m_savedYet;

    // Called by OnSaveDocument and OnOpenDocument to implement standard
    // Save/Load behaviour. Re-implement in derived class for custom
    // behaviour.
    virtual bool DoSaveDocument(const wxString& file);
    virtual bool DoOpenDocument(const wxString& file);

    // the default implementation of GetUserReadableName()
    wxString DoGetUserReadableName() const;

private:
    // list of all documents whose m_documentParent is this one
    std::list<wxDocument*> m_childDocuments;

    wxDECLARE_ABSTRACT_CLASS(wxDocument);
    wxDECLARE_NO_COPY_CLASS(wxDocument);
};

class WXDLLIMPEXP_CORE wxView : public wxEvtHandler
{
public:
    wxView();
    virtual ~wxView();

    wxDocument* GetDocument() const { return m_viewDocument; }
    virtual void SetDocument(wxDocument* doc);

    wxString GetViewName() const { return m_viewTypeName; }
    void SetViewName(const wxString& name) { m_viewTypeName = name; }

    wxWindow* GetFrame() const { return m_viewFrame; }
    void SetFrame(wxWindow* frame) { m_viewFrame = frame; }

    virtual void OnActivateView(bool activate,
        wxView* activeView,
        wxView* deactiveView);
    virtual void OnDraw(wxDC* dc) = 0;
    virtual void OnPrint(wxDC* dc, wxObject* info);
    virtual void OnUpdate(wxView* sender, wxObject* hint = nullptr);
    virtual void OnClosingDocument() {}
    virtual void OnChangeFilename();

    // Called by framework if created automatically by the default document
    // manager class: gives view a chance to initialise
    virtual bool OnCreate(wxDocument* WXUNUSED(doc), long WXUNUSED(flags))
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

    wxDocManager* GetDocumentManager() const
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
    void SetDocChildFrame(wxDocChildFrameAnyBase* docChildFrame);

    // get the associated frame, may be null during destruction
    wxDocChildFrameAnyBase* GetDocChildFrame() const { return m_docChildFrame; }

protected:
    // hook the document into event handlers chain here
    virtual bool TryBefore(wxEvent& event) override;

    wxDocument* m_viewDocument;
    wxString          m_viewTypeName;
    wxWindow* m_viewFrame;

    wxDocChildFrameAnyBase* m_docChildFrame;

private:
    wxDECLARE_ABSTRACT_CLASS(wxView);
    wxDECLARE_NO_COPY_CLASS(wxView);
};

class TextEditDocument : public wxDocument
{
public:
    TextEditDocument() : wxDocument() {}
    TextEditDocument(const TextEditDocument&) = delete;
    TextEditDocument& operator=(const TextEditDocument&) = delete;

    virtual bool OnCreate(const wxString& path, long flags) override;

    virtual bool IsModified() const override;
    virtual void Modify(bool mod) override;

protected:
    virtual bool DoSaveDocument(const wxString& filename) override;
    virtual bool DoOpenDocument(const wxString& filename) override;

private:
    wxTextCtrl* GetTextCtrl() const;

    void OnTextChange(wxCommandEvent& event);

    wxDECLARE_DYNAMIC_CLASS(TextEditDocument);
};



class TextEditView : public wxView
{
public:
    TextEditView() : wxView(), m_text(nullptr) {}

    virtual bool OnCreate(wxDocument* doc, long flags) override;
    virtual void OnDraw(wxDC* dc) override;
    virtual bool OnClose(bool deleteWindow = true) override;

    wxTextCtrl* GetText() const { return m_text; }

private:
    void OnCopy(wxCommandEvent& WXUNUSED(event)) { m_text->Copy(); }
    void OnPaste(wxCommandEvent& WXUNUSED(event)) { m_text->Paste(); }
    void OnSelectAll(wxCommandEvent& WXUNUSED(event)) { m_text->SelectAll(); }

    wxTextCtrl* m_text;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(TextEditView);
};

// Represents user interface (and other) properties of documents and views
class WXDLLIMPEXP_CORE wxDocTemplate : public wxObject
{

    friend class WXDLLIMPEXP_FWD_CORE wxDocManager;

public:
    // Associate document and view types. They're for identifying what view is
    // associated with what template/document type
    wxDocTemplate(wxDocManager* manager,
        const wxString& descr,
        const wxString& filter,
        const wxString& dir,
        const wxString& ext,
        const wxString& docTypeName,
        const wxString& viewTypeName,
        wxClassInfo* docClassInfo = nullptr,
        wxClassInfo* viewClassInfo = nullptr,
        long flags = wxDEFAULT_TEMPLATE_FLAGS);

    virtual ~wxDocTemplate();

    // By default, these two member functions dynamically creates document and
    // view using dynamic instance construction. Override these if you need a
    // different method of construction.
    virtual wxDocument* CreateDocument(const wxString& path, long flags = 0);
    virtual wxView* CreateView(wxDocument* doc, long flags = 0);

    // Helper method for CreateDocument; also allows you to do your own document
    // creation
    virtual bool InitDocument(wxDocument* doc,
        const wxString& path,
        long flags = 0);

    wxString GetDefaultExtension() const { return m_defaultExt; }
    wxString GetDescription() const { return m_description; }
    wxString GetDirectory() const { return m_directory; }
    wxDocManager* GetDocumentManager() const { return m_documentManager; }
    void SetDocumentManager(wxDocManager* manager)
    {
        m_documentManager = manager;
    }
    wxString GetFileFilter() const { return m_fileFilter; }
    long GetFlags() const { return m_flags; }
    virtual wxString GetViewName() const { return m_viewTypeName; }
    virtual wxString GetDocumentName() const { return m_docTypeName; }

    void SetFileFilter(const wxString& filter) { m_fileFilter = filter; }
    void SetDirectory(const wxString& dir) { m_directory = dir; }
    void SetDescription(const wxString& descr) { m_description = descr; }
    void SetDefaultExtension(const wxString& ext) { m_defaultExt = ext; }
    void SetFlags(long flags) { m_flags = flags; }

    bool IsVisible() const { return (m_flags & wxTEMPLATE_VISIBLE) != 0; }

    wxClassInfo* GetDocClassInfo() const { return m_docClassInfo; }
    wxClassInfo* GetViewClassInfo() const { return m_viewClassInfo; }

    virtual bool FileMatchesTemplate(const wxString& path);

protected:
    long              m_flags;
    wxString          m_fileFilter;
    wxString          m_directory;
    wxString          m_description;
    wxString          m_defaultExt;
    wxString          m_docTypeName;
    wxString          m_viewTypeName;
    wxDocManager* m_documentManager;

    // For dynamic creation of appropriate instances.
    wxClassInfo* m_docClassInfo;
    wxClassInfo* m_viewClassInfo;

    // Called by CreateDocument and CreateView to create the actual
    // document/view object.
    //
    // By default uses the ClassInfo provided to the constructor. Override
    // these functions to provide a different method of creation.
    virtual wxDocument* DoCreateDocument();
    virtual wxView* DoCreateView();

private:
    wxDECLARE_CLASS(wxDocTemplate);
    wxDECLARE_NO_COPY_CLASS(wxDocTemplate);
};

// One object of this class may be created in an application, to manage all
// the templates and documents.
class WXDLLIMPEXP_CORE wxDocManager : public wxEvtHandler
{
public:
    // NB: flags are unused, don't pass wxDOC_XXX to this ctor
    wxDocManager(long flags = 0, bool initialize = true);
    virtual ~wxDocManager();

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

    virtual wxDocument* CreateDocument(const wxString& path, long flags = 0);

    // wrapper around CreateDocument() with a more clear name
    wxDocument* CreateNewDocument()
    {
        return CreateDocument(wxString(), wxDOC_NEW);
    }

    virtual wxView* CreateView(wxDocument* doc, long flags = 0);
    virtual void DeleteTemplate(wxDocTemplate* temp, long flags = 0);
    virtual bool FlushDoc(wxDocument* doc);
    virtual wxDocTemplate* MatchTemplate(const wxString& path);
    virtual wxDocTemplate* SelectDocumentPath(wxDocTemplate** templates,
        int noTemplates, wxString& path, long flags, bool save = false);
    virtual wxDocTemplate* SelectDocumentType(wxDocTemplate** templates,
        int noTemplates, bool sort = false);
    virtual wxDocTemplate* SelectViewType(wxDocTemplate** templates,
        int noTemplates, bool sort = false);
    virtual wxDocTemplate* FindTemplateForPath(const wxString& path);

    void AssociateTemplate(wxDocTemplate* temp);
    void DisassociateTemplate(wxDocTemplate* temp);

    // Find template from document class info, may return nullptr.
    wxDocTemplate* FindTemplate(const wxClassInfo* documentClassInfo);

    // Find document from file name, may return nullptr.
    wxDocument* FindDocumentByPath(const wxString& path) const;

    wxDocument* GetCurrentDocument() const;

    void SetMaxDocsOpen(int n) { m_maxDocsOpen = n; }
    int GetMaxDocsOpen() const { return m_maxDocsOpen; }

    // Add and remove a document from the manager's list
    void AddDocument(wxDocument* doc);
    void RemoveDocument(wxDocument* doc);

    // closes all currently open documents
    bool CloseDocuments(bool force = true);

    // closes the specified document
    bool CloseDocument(wxDocument* doc, bool force = false);

    // Clear remaining documents and templates
    bool Clear(bool force = true);

    // Views or windows should inform the document manager
    // when a view is going in or out of focus
    virtual void ActivateView(wxView* view, bool activate = true);
    virtual wxView* GetCurrentView() const { return m_currentView; }

    // This method tries to find an active view harder than GetCurrentView():
    // if the latter is null, it also checks if we don't have just a single
    // view and returns it then.
    wxView* GetAnyUsableView() const;


    wxDocVector GetDocumentsVector() const;
    wxDocTemplateVector GetTemplatesVector() const;

    wxList& GetDocuments() { return m_docs; }
    wxList& GetTemplates() { return m_templates; }

    // Return the default name for a new document (by default returns strings
    // in the form "unnamed <counter>" but can be overridden)
    virtual wxString MakeNewDocumentName();

    // Make a frame title (override this to do something different)
    virtual wxString MakeFrameTitle(wxDocument* doc);

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
    static wxDocManager* GetDocumentManager() { return sm_docManager; }

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
    wxCommandProcessor* GetCurrentCommandProcessor() const;

    int               m_defaultDocumentNameCounter;
    int               m_maxDocsOpen;
    wxList            m_docs;
    wxList            m_templates;
    wxView* m_currentView;
    wxFileHistory* m_fileHistory;
    wxString          m_lastDirectory;
    static wxDocManager* sm_docManager;

#if wxUSE_PRINTING_ARCHITECTURE
    wxPageSetupDialogData m_pageSetupDialogData;
#endif // wxUSE_PRINTING_ARCHITECTURE

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxDocManager);
    wxDECLARE_NO_COPY_CLASS(wxDocManager);
};

// ----------------------------------------------------------------------------
// Base class for child frames -- this is what wxView renders itself into
//
// Notice that this is a mix-in class so it doesn't derive from wxWindow, only
// wxDocChildFrameAny does
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDocChildFrameAnyBase
{
public:
    // default ctor, use Create() after it
    wxDocChildFrameAnyBase()
    {
        m_childDocument = nullptr;
        m_childView = nullptr;
        m_win = nullptr;
        m_lastEvent = nullptr;
    }

    // full ctor equivalent to using the default one and Create()
    wxDocChildFrameAnyBase(wxDocument* doc, wxView* view, wxWindow* win)
    {
        Create(doc, view, win);
    }

    // method which must be called for an object created using the default ctor
    //
    // note that it returns bool just for consistency with Create() methods in
    // other classes, we never return false from here
    bool Create(wxDocument* doc, wxView* view, wxWindow* win)
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
    ~wxDocChildFrameAnyBase()
    {
        // prevent the view from deleting us if we're being deleted directly
        // (and not via Close() + Destroy())
        if (m_childView)
            m_childView->SetDocChildFrame(nullptr);
    }

    wxDocument* GetDocument() const { return m_childDocument; }
    wxView* GetView() const { return m_childView; }
    void SetDocument(wxDocument* doc) { m_childDocument = doc; }
    void SetView(wxView* view) { m_childView = view; }

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


    wxDocument* m_childDocument;
    wxView* m_childView;

    // the associated window: having it here is not terribly elegant but it
    // allows us to avoid having any virtual functions in this class
    wxWindow* m_win;

private:
    // Pointer to the last processed event used to avoid sending the same event
    // twice to wxDocManager, from here and from wxDocParentFrameAnyBase.
    wxEvent* m_lastEvent;

    wxDECLARE_NO_COPY_CLASS(wxDocChildFrameAnyBase);
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
class wxDocChildFrameAny : public ChildFrame,
    public wxDocChildFrameAnyBase
{
public:
    typedef ChildFrame BaseClass;

    // default ctor, use Create after it
    wxDocChildFrameAny() = default;

    // ctor for a frame showing the given view of the specified document
    wxDocChildFrameAny(wxDocument* doc,
        wxView* view,
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

    bool Create(wxDocument* doc,
        wxView* view,
        ParentFrame* parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        this->Bind(wxEVT_ACTIVATE, &wxDocChildFrameAny::OnActivate, this);
        this->Bind(wxEVT_CLOSE_WINDOW, &wxDocChildFrameAny::OnCloseWindow, this);

        if (!wxDocChildFrameAnyBase::Create(doc, view, this))
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

    wxDECLARE_NO_COPY_TEMPLATE_CLASS_2(wxDocChildFrameAny,
        ChildFrame, ParentFrame);
};

// ----------------------------------------------------------------------------
// A default child frame: we need to define it as a class just for wxRTTI,
// otherwise we could simply typedef it
// ----------------------------------------------------------------------------

typedef wxDocChildFrameAny<wxFrame, wxFrame> wxDocChildFrameBase;

class WXDLLIMPEXP_CORE wxDocChildFrame : public wxDocChildFrameBase
{
public:
    wxDocChildFrame()
    {
    }

    wxDocChildFrame(wxDocument* doc,
        wxView* view,
        wxFrame* parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxASCII_STR(wxFrameNameStr))
        : wxDocChildFrameBase(doc, view,
            parent, id, title, pos, size, style, name)
    {
    }

    bool Create(wxDocument* doc,
        wxView* view,
        wxFrame* parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        return wxDocChildFrameBase::Create
        (
            doc, view,
            parent, id, title, pos, size, style, name
        );
    }

private:
    wxDECLARE_CLASS(wxDocChildFrame);
    wxDECLARE_NO_COPY_CLASS(wxDocChildFrame);
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
class WXDLLIMPEXP_CORE wxDocParentFrameAnyBase
{
public:
    wxDocParentFrameAnyBase(wxWindow* frame)
        : m_frame(frame)
    {
        m_docManager = nullptr;
    }

    wxDocManager* GetDocumentManager() const { return m_docManager; }

protected:
    // This is similar to wxDocChildFrameAnyBase method with the same name:
    // while we're not an event handler ourselves and so can't override
    // TryBefore(), we provide a helper that the derived template class can use
    // from its TryBefore() implementation.
    bool TryProcessEvent(wxEvent& event);

    wxWindow* const m_frame;
    wxDocManager* m_docManager;

    wxDECLARE_NO_COPY_CLASS(wxDocParentFrameAnyBase);
};

// This is similar to wxDocChildFrameAny and is used to provide common
// implementation for both wxDocParentFrame and wxDocMDIParentFrame
template <class BaseFrame>
class wxDocParentFrameAny : public BaseFrame,
    public wxDocParentFrameAnyBase
{
public:
    wxDocParentFrameAny() : wxDocParentFrameAnyBase(this) {}
    wxDocParentFrameAny(
        wxDocManager* manager,
        wxFrame* frame,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxASCII_STR(wxFrameNameStr))
        : wxDocParentFrameAnyBase(this)
    {
        Create(manager, frame, id, title, pos, size, style, name);
    }

    bool Create(
        wxDocManager* manager,
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

        this->Bind(wxEVT_MENU, &wxDocParentFrameAny::OnExit, this, wxID_EXIT);
        this->Bind(wxEVT_CLOSE_WINDOW, &wxDocParentFrameAny::OnCloseWindow, this);

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


    wxDECLARE_NO_COPY_CLASS(wxDocParentFrameAny);
};

typedef wxDocParentFrameAny<wxFrame> wxDocParentFrameBase;

class WXDLLIMPEXP_CORE wxDocParentFrame : public wxDocParentFrameBase
{
public:
    wxDocParentFrame() : wxDocParentFrameBase() {}

    wxDocParentFrame(
        wxDocManager* manager,
        wxFrame* parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxASCII_STR(wxFrameNameStr))
        : wxDocParentFrameBase(manager,
            parent, id, title, pos, size, style, name)
    {
    }

    bool Create(
        wxDocManager* manager,
        wxFrame* parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        return wxDocParentFrameBase::Create(manager,
            parent, id, title,
            pos, size, style, name);
    }

private:
    wxDECLARE_CLASS(wxDocParentFrame);
    wxDECLARE_NO_COPY_CLASS(wxDocParentFrame);
};

// ----------------------------------------------------------------------------
// Provide simple default printing facilities
// ----------------------------------------------------------------------------

#if wxUSE_PRINTING_ARCHITECTURE
class WXDLLIMPEXP_CORE wxDocPrintout : public wxPrintout
{
public:
    wxDocPrintout(wxView* view = nullptr, const wxString& title = wxString());

    // implement wxPrintout methods
    virtual bool OnPrintPage(int page) override;
    virtual bool HasPage(int page) override;
    virtual bool OnBeginDocument(int startPage, int endPage) override;
    virtual void GetPageInfo(int* minPage, int* maxPage,
        int* selPageFrom, int* selPageTo) override;

    virtual wxView* GetView() { return m_printoutView; }

protected:
    wxView* m_printoutView;

private:
    wxDECLARE_DYNAMIC_CLASS(wxDocPrintout);
    wxDECLARE_NO_COPY_CLASS(wxDocPrintout);
};
#endif // wxUSE_PRINTING_ARCHITECTURE

// For compatibility with existing file formats:
// converts from/to a stream to/from a temporary file.
#if wxUSE_STD_IOSTREAM
bool WXDLLIMPEXP_CORE
wxTransferFileToStream(const wxString& filename, std::ostream& stream);
bool WXDLLIMPEXP_CORE
wxTransferStreamToFile(std::istream& stream, const wxString& filename);
#else
bool WXDLLIMPEXP_CORE
wxTransferFileToStream(const wxString& filename, wxOutputStream& stream);
bool WXDLLIMPEXP_CORE
wxTransferStreamToFile(wxInputStream& stream, const wxString& filename);
#endif // wxUSE_STD_IOSTREAM

inline wxViewVector wxDocument::GetViewsVector() const
{
    return m_documentViews.AsVector<wxView*>();
}

inline wxDocVector wxDocManager::GetDocumentsVector() const
{
    return m_docs.AsVector<wxDocument*>();
}

inline wxDocTemplateVector wxDocManager::GetTemplatesVector() const
{
    return m_templates.AsVector<wxDocTemplate*>();
}

#endif // wxUSE_DOC_VIEW_ARCHITECTURE

#endif // _WX_DOCH__



/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/docview.cpp
// Purpose:     Document/view classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_DOC_VIEW_ARCHITECTURE

#ifndef WX_PRECOMP
#include "wx/list.h"
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
#include "wx/vector.h"
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

#include <memory>

// ----------------------------------------------------------------------------
// wxWidgets macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxDocument, wxEvtHandler);
wxIMPLEMENT_ABSTRACT_CLASS(wxView, wxEvtHandler);
wxIMPLEMENT_ABSTRACT_CLASS(wxDocTemplate, wxObject);
wxIMPLEMENT_DYNAMIC_CLASS(wxDocManager, wxEvtHandler);
wxIMPLEMENT_CLASS(wxDocChildFrame, wxFrame);
wxIMPLEMENT_CLASS(wxDocParentFrame, wxFrame);

#if wxUSE_PRINTING_ARCHITECTURE
wxIMPLEMENT_DYNAMIC_CLASS(wxDocPrintout, wxPrintout);
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// Definition of wxDocument
// ----------------------------------------------------------------------------

wxDocument::wxDocument(wxDocument* parent)
{
    m_documentModified = false;
    m_documentTemplate = nullptr;

    m_documentParent = parent;
    if (parent)
        parent->m_childDocuments.push_back(this);

    m_commandProcessor = nullptr;
    m_savedYet = false;
}

bool wxDocument::DeleteContents()
{
    return true;
}

wxDocument::~wxDocument()
{
    delete m_commandProcessor;

    if (GetDocumentManager())
        GetDocumentManager()->RemoveDocument(this);

    if (m_documentParent)
        m_documentParent->m_childDocuments.remove(this);

    // Not safe to do here, since it'll invoke virtual view functions
    // expecting to see valid derived objects: and by the time we get here,
    // we've called destructors higher up.
    //DeleteAllViews();
}

bool wxDocument::CanClose()
{
    if (!OnSaveModified())
        return false;

    // When the parent document closes, its children must be closed as well as
    // they can't exist without the parent, so ask them too.

    for (auto& childDoc : m_childDocuments)
    {
        if (!childDoc->OnSaveModified())
        {
            // Leave the parent document opened if a child can't close.
            return false;
        }
    }

    return true;
}

bool wxDocument::Close()
{
    // First check if this document itself and all its children can be closed.
    if (!CanClose())
        return false;

    // Now that they all did, do close them: as m_childDocuments is modified as
    // we iterate over it, don't use the usual for-style iteration here.
    while (!m_childDocuments.empty())
    {
        wxDocument* const childDoc = m_childDocuments.front();

        // This will call OnSaveModified() once again but it shouldn't do
        // anything as the document was just saved or marked as not needing to
        // be saved by the call to OnSaveModified() that returned true above.
        if (!childDoc->Close())
        {
            wxFAIL_MSG("Closing the child document unexpectedly failed "
                "after its OnSaveModified() returned true");
        }

        // Delete the child document by deleting all its views.
        childDoc->DeleteAllViews();
    }


    return OnCloseDocument();
}

bool wxDocument::OnCloseDocument()
{
    // Tell all views that we're about to close
    NotifyClosing();
    DeleteContents();
    Modify(false);
    return true;
}

// Note that this implicitly deletes the document when the last view is
// deleted.
bool wxDocument::DeleteAllViews()
{
    wxDocManager* manager = GetDocumentManager();

    // first check if all views agree to be closed
    const wxList::iterator end = m_documentViews.end();
    for (wxList::iterator i = m_documentViews.begin(); i != end; ++i)
    {
        wxView* view = (wxView*)*i;
        if (!view->Close())
            return false;
    }

    // all views agreed to close, now do close them
    if (m_documentViews.empty())
    {
        // normally the document would be implicitly deleted when the last view
        // is, but if don't have any views, do it here instead
        if (manager && manager->GetDocuments().Member(this))
            delete this;
    }
    else // have views
    {
        // as we delete elements we iterate over, don't use the usual "from
        // begin to end" loop
        for (;; )
        {
            wxView* view = (wxView*)*m_documentViews.begin();

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

wxView* wxDocument::GetFirstView() const
{
    if (m_documentViews.empty())
        return nullptr;

    return static_cast<wxView*>(m_documentViews.GetFirst()->GetData());
}

void wxDocument::Modify(bool mod)
{
    if (mod != m_documentModified)
    {
        m_documentModified = mod;

        // Allow views to append asterix to the title
        wxView* view = GetFirstView();
        if (view) view->OnChangeFilename();
    }
}

wxDocManager* wxDocument::GetDocumentManager() const
{
    // For child documents we use the same document manager as the parent, even
    // though we don't have our own template (as children are not opened/saved
    // directly).
    if (m_documentParent)
        return m_documentParent->GetDocumentManager();

    if (m_documentTemplate)
        return m_documentTemplate->GetDocumentManager();

    // Fall back on the global manager if the document doesn't have a template,
    // code elsewhere, notably in DeleteAllViews(), relies on the document
    // always being managed by some manager.
    return wxDocManager::GetDocumentManager();
}

bool wxDocument::OnNewDocument()
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

bool wxDocument::Save()
{
    if (AlreadySaved())
        return true;

    if (m_documentFile.empty() || !m_savedYet)
        return SaveAs();

    return OnSaveDocument(m_documentFile);
}

bool wxDocument::SaveAs()
{
    wxDocTemplate* docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return false;

#ifdef wxHAS_MULTIPLE_FILEDLG_FILTERS
    wxString filter = docTemplate->GetDescription() + wxT(" (") +
        docTemplate->GetFileFilter() + wxT(")|") +
        docTemplate->GetFileFilter();

    // Now see if there are some other template with identical view and document
    // classes, whose filters may also be used.
    if (docTemplate->GetViewClassInfo() && docTemplate->GetDocClassInfo())
    {
        wxList::compatibility_iterator
            node = docTemplate->GetDocumentManager()->GetTemplates().GetFirst();
        while (node)
        {
            wxDocTemplate* t = (wxDocTemplate*)node->GetData();

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

            node = node->GetNext();
        }
    }
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

bool wxDocument::OnSaveDocument(const wxString& file)
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

bool wxDocument::OnOpenDocument(const wxString& file)
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
std::istream& wxDocument::LoadObject(std::istream& stream)
#else
wxInputStream& wxDocument::LoadObject(wxInputStream& stream)
#endif
{
    return stream;
}

#if wxUSE_STD_IOSTREAM
std::ostream& wxDocument::SaveObject(std::ostream& stream)
#else
wxOutputStream& wxDocument::SaveObject(wxOutputStream& stream)
#endif
{
    return stream;
}

bool wxDocument::Revert()
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
wxString wxDocument::GetUserReadableName() const
{
    return DoGetUserReadableName();
}

wxString wxDocument::DoGetUserReadableName() const
{
    if (!m_documentTitle.empty())
        return m_documentTitle;

    if (!m_documentFile.empty())
        return wxFileNameFromPath(m_documentFile);

    return _("unnamed");
}

wxWindow* wxDocument::GetDocumentWindow() const
{
    wxView* const view = GetFirstView();

    return view ? view->GetFrame() : wxTheApp->GetTopWindow();
}

wxCommandProcessor* wxDocument::OnCreateCommandProcessor()
{
    return new wxCommandProcessor;
}

// true if safe to close
bool wxDocument::OnSaveModified()
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

void wxDocument::OnSaveBeforeForceClose()
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

bool wxDocument::Draw(wxDC& WXUNUSED(context))
{
    return true;
}

bool wxDocument::AddView(wxView* view)
{
    if (!m_documentViews.Member(view))
    {
        m_documentViews.Append(view);
        OnChangedViewList();
    }
    return true;
}

bool wxDocument::RemoveView(wxView* view)
{
    if (!m_documentViews.DeleteObject(view))
        return false;

    OnChangedViewList();
    return true;
}

bool wxDocument::OnCreate(const wxString& WXUNUSED(path), long flags)
{
    return GetDocumentTemplate()->CreateView(this, flags) != nullptr;
}

// Called after a view is added or removed.
// The default implementation deletes the document if
// there are no more views.
void wxDocument::OnChangedViewList()
{
    if (m_documentViews.empty() && OnSaveModified())
        delete this;
}

void wxDocument::UpdateAllViews(wxView* sender, wxObject* hint)
{
    wxList::compatibility_iterator node = m_documentViews.GetFirst();
    while (node)
    {
        wxView* view = (wxView*)node->GetData();
        if (view != sender)
            view->OnUpdate(sender, hint);
        node = node->GetNext();
    }
}

void wxDocument::NotifyClosing()
{
    wxList::compatibility_iterator node = m_documentViews.GetFirst();
    while (node)
    {
        wxView* view = (wxView*)node->GetData();
        view->OnClosingDocument();
        node = node->GetNext();
    }
}

void wxDocument::SetFilename(const wxString& filename, bool notifyViews)
{
    m_documentFile = filename;
    OnChangeFilename(notifyViews);
}

void wxDocument::OnChangeFilename(bool notifyViews)
{
    if (notifyViews)
    {
        // Notify the views that the filename has changed
        wxList::compatibility_iterator node = m_documentViews.GetFirst();
        while (node)
        {
            wxView* view = (wxView*)node->GetData();
            view->OnChangeFilename();
            node = node->GetNext();
        }
    }
}

bool wxDocument::DoSaveDocument(const wxString& file)
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

bool wxDocument::DoOpenDocument(const wxString& file)
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

wxView::wxView()
{
    m_viewDocument = nullptr;

    m_viewFrame = nullptr;

    m_docChildFrame = nullptr;
}

wxView::~wxView()
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

void wxView::SetDocChildFrame(wxDocChildFrameAnyBase* docChildFrame)
{
    SetFrame(docChildFrame ? docChildFrame->GetWindow() : nullptr);
    m_docChildFrame = docChildFrame;
}

bool wxView::TryBefore(wxEvent& event)
{
    wxDocument* const doc = GetDocument();
    return doc && doc->ProcessEventLocally(event);
}

void wxView::OnActivateView(bool WXUNUSED(activate),
    wxView* WXUNUSED(activeView),
    wxView* WXUNUSED(deactiveView))
{
}

void wxView::OnPrint(wxDC* dc, wxObject* WXUNUSED(info))
{
    OnDraw(dc);
}

void wxView::OnUpdate(wxView* WXUNUSED(sender), wxObject* WXUNUSED(hint))
{
}

void wxView::OnChangeFilename()
{
    // GetFrame can return wxWindow rather than wxTopLevelWindow due to
    // generic MDI implementation so use SetLabel rather than SetTitle.
    // It should cause SetTitle() for top level windows.
    wxWindow* win = GetFrame();
    if (!win) return;

    wxDocument* doc = GetDocument();
    if (!doc) return;

    wxString label = doc->GetUserReadableName();
    if (doc->IsModified())
    {
        label += "*";
    }
    win->SetLabel(label);
}

void wxView::SetDocument(wxDocument* doc)
{
    m_viewDocument = doc;
    if (doc)
        doc->AddView(this);
}

bool wxView::Close(bool deleteWindow)
{
    return OnClose(deleteWindow);
}

void wxView::Activate(bool activate)
{
    if (GetDocument() && GetDocumentManager())
    {
        OnActivateView(activate, this, GetDocumentManager()->GetCurrentView());
        GetDocumentManager()->ActivateView(this, activate);
    }
}

bool wxView::OnClose(bool WXUNUSED(deleteWindow))
{
    return GetDocument() ? GetDocument()->Close() : true;
}

#if wxUSE_PRINTING_ARCHITECTURE
wxPrintout* wxView::OnCreatePrintout()
{
    return new wxDocPrintout(this);
}
#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// wxDocTemplate
// ----------------------------------------------------------------------------

wxDocTemplate::wxDocTemplate(wxDocManager* manager,
    const wxString& descr,
    const wxString& filter,
    const wxString& dir,
    const wxString& ext,
    const wxString& docTypeName,
    const wxString& viewTypeName,
    wxClassInfo* docClassInfo,
    wxClassInfo* viewClassInfo,
    long flags)
    : m_fileFilter(filter)
    , m_directory(dir)
    , m_description(descr)
    , m_defaultExt(ext)
    , m_docTypeName(docTypeName)
    , m_viewTypeName(viewTypeName)
{
    m_documentManager = manager;
    m_flags = flags;
    m_documentManager->AssociateTemplate(this);

    m_docClassInfo = docClassInfo;
    m_viewClassInfo = viewClassInfo;
}

wxDocTemplate::~wxDocTemplate()
{
    m_documentManager->DisassociateTemplate(this);
}

// Tries to dynamically construct an object of the right class.
wxDocument* wxDocTemplate::CreateDocument(const wxString& path, long flags)
{
    // InitDocument() is supposed to delete the document object if its
    // initialization fails so don't use unique_ptr<> here: this is fragile
    // but unavoidable because the default implementation uses CreateView()
    // which may -- or not -- create a wxView and if it does create it and its
    // initialization fails then the view destructor will delete the document
    // (via RemoveView()) and as we can't distinguish between the two cases we
    // just have to assume that it always deletes it in case of failure
    wxDocument* const doc = DoCreateDocument();

    return doc && InitDocument(doc, path, flags) ? doc : nullptr;
}

bool
wxDocTemplate::InitDocument(wxDocument* doc, const wxString& path, long flags)
{
    wxScopeGuard guard = wxMakeGuard([&, this]()
        {
            // The document may be already destroyed, this happens if its view
            // creation fails as then the view being created is destroyed
            // triggering the destruction of the document as this first view is
            // also the last one. However if OnCreate() fails for any reason other
            // than view creation failure, the document is still alive and we need
            // to clean it up ourselves to avoid having a zombie document.
            if (GetDocumentManager()->GetDocuments().Member(doc))
                doc->DeleteAllViews();
        });

    doc->SetFilename(path);
    doc->SetDocumentTemplate(this);
    GetDocumentManager()->AddDocument(doc);
    doc->SetCommandProcessor(doc->OnCreateCommandProcessor());

    if (!doc->OnCreate(path, flags))
        return false;

    guard.Dismiss();

    return true;
}

wxView* wxDocTemplate::CreateView(wxDocument* doc, long flags)
{
    std::unique_ptr<wxView> view(DoCreateView());
    if (!view)
        return nullptr;

    view->SetDocument(doc);
    if (!view->OnCreate(doc, flags))
        return nullptr;

    return view.release();
}

// The default (very primitive) format detection: check is the extension is
// that of the template
bool wxDocTemplate::FileMatchesTemplate(const wxString& path)
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

wxDocument* wxDocTemplate::DoCreateDocument()
{
    if (!m_docClassInfo)
        return nullptr;

    return static_cast<wxDocument*>(m_docClassInfo->CreateObject());
}

wxView* wxDocTemplate::DoCreateView()
{
    if (!m_viewClassInfo)
        return nullptr;

    return static_cast<wxView*>(m_viewClassInfo->CreateObject());
}

// ----------------------------------------------------------------------------
// wxDocManager
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxDocManager, wxEvtHandler)
EVT_MENU(wxID_OPEN, wxDocManager::OnFileOpen)
EVT_MENU(wxID_CLOSE, wxDocManager::OnFileClose)
EVT_MENU(wxID_CLOSE_ALL, wxDocManager::OnFileCloseAll)
EVT_MENU(wxID_REVERT, wxDocManager::OnFileRevert)
EVT_MENU(wxID_NEW, wxDocManager::OnFileNew)
EVT_MENU(wxID_SAVE, wxDocManager::OnFileSave)
EVT_MENU(wxID_SAVEAS, wxDocManager::OnFileSaveAs)
EVT_MENU(wxID_UNDO, wxDocManager::OnUndo)
EVT_MENU(wxID_REDO, wxDocManager::OnRedo)

// We don't know in advance how many items can there be in the MRU files
// list so set up OnMRUFile() as a handler for all menu events and do the
// check for the id of the menu item clicked inside it.
EVT_MENU(wxID_ANY, wxDocManager::OnMRUFile)

EVT_UPDATE_UI(wxID_OPEN, wxDocManager::OnUpdateFileOpen)
EVT_UPDATE_UI(wxID_CLOSE, wxDocManager::OnUpdateDisableIfNoDoc)
EVT_UPDATE_UI(wxID_CLOSE_ALL, wxDocManager::OnUpdateDisableIfNoDoc)
EVT_UPDATE_UI(wxID_REVERT, wxDocManager::OnUpdateFileRevert)
EVT_UPDATE_UI(wxID_NEW, wxDocManager::OnUpdateFileNew)
EVT_UPDATE_UI(wxID_SAVE, wxDocManager::OnUpdateFileSave)
EVT_UPDATE_UI(wxID_SAVEAS, wxDocManager::OnUpdateFileSaveAs)
EVT_UPDATE_UI(wxID_UNDO, wxDocManager::OnUpdateUndo)
EVT_UPDATE_UI(wxID_REDO, wxDocManager::OnUpdateRedo)

#if wxUSE_PRINTING_ARCHITECTURE
EVT_MENU(wxID_PRINT, wxDocManager::OnPrint)
EVT_MENU(wxID_PREVIEW, wxDocManager::OnPreview)
EVT_MENU(wxID_PRINT_SETUP, wxDocManager::OnPageSetup)

EVT_UPDATE_UI(wxID_PRINT, wxDocManager::OnUpdateDisableIfNoDoc)
EVT_UPDATE_UI(wxID_PREVIEW, wxDocManager::OnUpdateDisableIfNoDoc)
// NB: we keep "Print setup" menu item always enabled as it can be used
//     even without an active document
#endif // wxUSE_PRINTING_ARCHITECTURE
wxEND_EVENT_TABLE()

wxDocManager* wxDocManager::sm_docManager = nullptr;

wxDocManager::wxDocManager(long WXUNUSED(flags), bool initialize)
{
    sm_docManager = this;

    m_defaultDocumentNameCounter = 1;
    m_currentView = nullptr;
    m_maxDocsOpen = INT_MAX;
    m_fileHistory = nullptr;
    if (initialize)
        Initialize();
}

wxDocManager::~wxDocManager()
{
    Clear();
    delete m_fileHistory;
    sm_docManager = nullptr;
}

// closes the specified document
bool wxDocManager::CloseDocument(wxDocument* doc, bool force)
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

    wxASSERT(!m_docs.Member(doc));

    return true;
}

bool wxDocManager::CloseDocuments(bool force)
{
    wxList::compatibility_iterator node = m_docs.GetFirst();
    while (node)
    {
        wxDocument* doc = (wxDocument*)node->GetData();
        wxList::compatibility_iterator next = node->GetNext();

        if (!CloseDocument(doc, force))
            return false;

        // This assumes that documents are not connected in
        // any way, i.e. deleting one document does NOT
        // delete another.
        node = next;
    }
    return true;
}

bool wxDocManager::Clear(bool force)
{
    if (!CloseDocuments(force))
        return false;

    m_currentView = nullptr;

    wxList::compatibility_iterator node = m_templates.GetFirst();
    while (node)
    {
        wxDocTemplate* templ = (wxDocTemplate*)node->GetData();
        wxList::compatibility_iterator next = node->GetNext();
        delete templ;
        node = next;
    }
    return true;
}

bool wxDocManager::Initialize()
{
    m_fileHistory = OnCreateFileHistory();
    return true;
}

wxString wxDocManager::GetLastDirectory() const
{
    // if we haven't determined the last used directory yet, do it now
    if (m_lastDirectory.empty())
    {
        // we're going to modify m_lastDirectory in this const method, so do it
        // via non-const self pointer instead of const this one
        wxDocManager* const self = const_cast<wxDocManager*>(this);

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

wxFileHistory* wxDocManager::OnCreateFileHistory()
{
    return new wxFileHistory;
}

void wxDocManager::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
    wxDocument* doc = GetCurrentDocument();
    if (doc)
        CloseDocument(doc);
}

void wxDocManager::OnFileCloseAll(wxCommandEvent& WXUNUSED(event))
{
    CloseDocuments(false);
}

void wxDocManager::OnFileNew(wxCommandEvent& WXUNUSED(event))
{
    CreateNewDocument();
}

void wxDocManager::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
    if (!CreateDocument(wxString()))
    {
        OnOpenFileFailure();
    }
}

void wxDocManager::OnFileRevert(wxCommandEvent& WXUNUSED(event))
{
    wxDocument* doc = GetCurrentDocument();
    if (!doc)
        return;
    doc->Revert();
}

void wxDocManager::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
    wxDocument* doc = GetCurrentDocument();
    if (!doc)
        return;
    doc->Save();
}

void wxDocManager::OnFileSaveAs(wxCommandEvent& WXUNUSED(event))
{
    wxDocument* doc = GetCurrentDocument();
    if (!doc)
        return;
    doc->SaveAs();
}

void wxDocManager::OnMRUFile(wxCommandEvent& event)
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

void wxDocManager::DoOpenMRUFile(unsigned n)
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

void wxDocManager::OnMRUFileNotExist(unsigned n, const wxString& filename)
{
    // remove the file which we can't open from the MRU list
    RemoveFileFromHistory(n);

    // and tell the user about it
    wxLogError(_("The file '%s' doesn't exist and couldn't be opened.\n"
        "It has been removed from the most recently used files list."),
        filename);
}

#if wxUSE_PRINTING_ARCHITECTURE

void wxDocManager::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    wxView* view = GetAnyUsableView();
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

void wxDocManager::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
    wxPageSetupDialog dlg(wxTheApp->GetTopWindow(), &m_pageSetupDialogData);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_pageSetupDialogData = dlg.GetPageSetupData();
    }
}

wxPreviewFrame* wxDocManager::CreatePreviewFrame(wxPrintPreviewBase* preview,
    wxWindow* parent,
    const wxString& title)
{
    return new wxPreviewFrame(preview, parent, title);
}

void wxDocManager::OnPreview(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor busy;
    wxView* view = GetAnyUsableView();
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

void wxDocManager::OnUndo(wxCommandEvent& event)
{
    wxCommandProcessor* const cmdproc = GetCurrentCommandProcessor();
    if (!cmdproc)
    {
        event.Skip();
        return;
    }

    cmdproc->Undo();
}

void wxDocManager::OnRedo(wxCommandEvent& event)
{
    wxCommandProcessor* const cmdproc = GetCurrentCommandProcessor();
    if (!cmdproc)
    {
        event.Skip();
        return;
    }

    cmdproc->Redo();
}

// Handlers for UI update commands

void wxDocManager::OnUpdateFileOpen(wxUpdateUIEvent& event)
{
    // CreateDocument() (which is called from OnFileOpen) may succeed
    // only when there is at least a template:
    event.Enable(GetTemplates().GetCount() > 0);
}

void wxDocManager::OnUpdateDisableIfNoDoc(wxUpdateUIEvent& event)
{
    event.Enable(GetCurrentDocument() != nullptr);
}

void wxDocManager::OnUpdateFileRevert(wxUpdateUIEvent& event)
{
    wxDocument* doc = GetCurrentDocument();
    event.Enable(doc && doc->IsModified() && doc->GetDocumentSaved());
}

void wxDocManager::OnUpdateFileNew(wxUpdateUIEvent& event)
{
    // CreateDocument() (which is called from OnFileNew) may succeed
    // only when there is at least a template:
    event.Enable(GetTemplates().GetCount() > 0);
}

void wxDocManager::OnUpdateFileSave(wxUpdateUIEvent& event)
{
    wxDocument* const doc = GetCurrentDocument();
    event.Enable(doc && !doc->IsChildDocument() && !doc->AlreadySaved());
}

void wxDocManager::OnUpdateFileSaveAs(wxUpdateUIEvent& event)
{
    wxDocument* const doc = GetCurrentDocument();
    event.Enable(doc && !doc->IsChildDocument());
}

void wxDocManager::OnUpdateUndo(wxUpdateUIEvent& event)
{
    wxCommandProcessor* const cmdproc = GetCurrentCommandProcessor();
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

void wxDocManager::OnUpdateRedo(wxUpdateUIEvent& event)
{
    wxCommandProcessor* const cmdproc = GetCurrentCommandProcessor();
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

wxView* wxDocManager::GetAnyUsableView() const
{
    wxView* view = GetCurrentView();

    if (!view && !m_docs.empty())
    {
        // if we have exactly one document, consider its view to be the current
        // one
        //
        // VZ: I'm not exactly sure why is this needed but this is how this
        //     code used to behave before the bug #9518 was fixed and it seems
        //     safer to preserve the old logic
        wxList::compatibility_iterator node = m_docs.GetFirst();
        if (!node->GetNext())
        {
            wxDocument* doc = static_cast<wxDocument*>(node->GetData());
            view = doc->GetFirstView();
        }
        //else: we have more than one document
    }

    return view;
}

bool wxDocManager::TryBefore(wxEvent& event)
{
    wxView* const view = GetAnyUsableView();
    return view && view->ProcessEventLocally(event);
}

namespace
{

    // helper function: return only the visible templates
    wxDocTemplateVector GetVisibleTemplates(const wxList& allTemplates)
    {
        // select only the visible templates
        const size_t totalNumTemplates = allTemplates.GetCount();
        wxDocTemplateVector templates;
        if (totalNumTemplates)
        {
            templates.reserve(totalNumTemplates);

            for (wxList::const_iterator i = allTemplates.begin(),
                end = allTemplates.end();
                i != end;
                ++i)
            {
                wxDocTemplate* const temp = (wxDocTemplate*)*i;
                if (temp->IsVisible())
                    templates.push_back(temp);
            }
        }

        return templates;
    }

} // anonymous namespace

void wxDocument::Activate()
{
    wxView* const view = GetFirstView();
    if (!view)
        return;

    view->Activate(true);
    if (wxWindow* win = view->GetFrame())
        win->Raise();
}

wxDocument* wxDocManager::FindDocumentByPath(const wxString& path) const
{
    const wxFileName fileName(path);
    for (wxList::const_iterator i = m_docs.begin(); i != m_docs.end(); ++i)
    {
        wxDocument* const doc = wxStaticCast(*i, wxDocument);

        if (fileName == wxFileName(doc->GetFilename()))
            return doc;
    }
    return nullptr;
}

wxDocument* wxDocManager::CreateDocument(const wxString& pathOrig, long flags)
{
    // this ought to be const but SelectDocumentType/Path() are not
    // const-correct and can't be changed as, being virtual, this risks
    // breaking user code overriding them
    wxDocTemplateVector templates(GetVisibleTemplates(m_templates));
    const size_t numTemplates = templates.size();
    if (!numTemplates)
    {
        // no templates can be used, can't create document
        return nullptr;
    }


    // normally user should select the template to use but wxDOC_SILENT flag we
    // choose one ourselves
    wxString path = pathOrig;   // may be modified below
    wxDocTemplate* temp;
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
            temp = SelectDocumentType(&templates[0], numTemplates);
        else
            temp = SelectDocumentPath(&templates[0], numTemplates, path, flags);
    }

    if (!temp)
        return nullptr;

    // check whether the document with this path is already opened
    if (!path.empty())
    {
        wxDocument* const doc = FindDocumentByPath(path);
        if (doc)
        {
            // file already open, just activate it and return
            doc->Activate();
            return doc;
        }
    }

    // no, we need to create a new document


    // if we've reached the max number of docs, close the first one.
    if ((int)GetDocuments().GetCount() >= m_maxDocsOpen)
    {
        if (!CloseDocument((wxDocument*)GetDocuments().GetFirst()->GetData()))
        {
            // can't open the new document if closing the old one failed
            return nullptr;
        }
    }


    // do create and initialize the new document finally
    wxDocument* const docNew = temp->CreateDocument(path, flags);
    if (!docNew)
        return nullptr;

    docNew->SetDocumentName(temp->GetDocumentName());

    wxScopeGuard guard = wxMakeObjGuard(*docNew, &wxDocument::DeleteAllViews);

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

wxView* wxDocManager::CreateView(wxDocument* doc, long flags)
{
    wxDocTemplateVector templates(GetVisibleTemplates(m_templates));
    const size_t numTemplates = templates.size();

    if (numTemplates == 0)
        return nullptr;

    wxDocTemplate* const
        temp = numTemplates == 1 ? templates[0]
        : SelectViewType(&templates[0], numTemplates);

    if (!temp)
        return nullptr;

    wxView* view = temp->CreateView(doc, flags);
    if (view)
        view->SetViewName(temp->GetViewName());
    return view;
}

// Not yet implemented
void
wxDocManager::DeleteTemplate(wxDocTemplate* WXUNUSED(temp), long WXUNUSED(flags))
{
}

// Not yet implemented
bool wxDocManager::FlushDoc(wxDocument* WXUNUSED(doc))
{
    return false;
}

wxDocument* wxDocManager::GetCurrentDocument() const
{
    wxView* const view = GetAnyUsableView();
    return view ? view->GetDocument() : nullptr;
}

wxCommandProcessor* wxDocManager::GetCurrentCommandProcessor() const
{
    wxDocument* const doc = GetCurrentDocument();
    return doc ? doc->GetCommandProcessor() : nullptr;
}

// Make a default name for a new document
wxString wxDocManager::MakeNewDocumentName()
{
    wxString name;

    name.Printf(_("unnamed%d"), m_defaultDocumentNameCounter);
    m_defaultDocumentNameCounter++;

    return name;
}

// Make a frame title (override this to do something different)
// If docName is empty, a document is not currently active.
wxString wxDocManager::MakeFrameTitle(wxDocument* doc)
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
wxDocTemplate* wxDocManager::MatchTemplate(const wxString& WXUNUSED(path))
{
    return nullptr;
}

// File history management
void wxDocManager::AddFileToHistory(const wxString& file)
{
    if (m_fileHistory)
        m_fileHistory->AddFileToHistory(file);
}

void wxDocManager::RemoveFileFromHistory(size_t i)
{
    if (m_fileHistory)
        m_fileHistory->RemoveFileFromHistory(i);
}

wxString wxDocManager::GetHistoryFile(size_t i) const
{
    wxString histFile;

    if (m_fileHistory)
        histFile = m_fileHistory->GetHistoryFile(i);

    return histFile;
}

void wxDocManager::FileHistoryUseMenu(wxMenu* menu)
{
    if (m_fileHistory)
        m_fileHistory->UseMenu(menu);
}

void wxDocManager::FileHistoryRemoveMenu(wxMenu* menu)
{
    if (m_fileHistory)
        m_fileHistory->RemoveMenu(menu);
}

#if wxUSE_CONFIG
void wxDocManager::FileHistoryLoad(const wxConfigBase& config)
{
    if (m_fileHistory)
        m_fileHistory->Load(config);
}

void wxDocManager::FileHistorySave(wxConfigBase& config)
{
    if (m_fileHistory)
        m_fileHistory->Save(config);
}
#endif

void wxDocManager::FileHistoryAddFilesToMenu(wxMenu* menu)
{
    if (m_fileHistory)
        m_fileHistory->AddFilesToMenu(menu);
}

void wxDocManager::FileHistoryAddFilesToMenu()
{
    if (m_fileHistory)
        m_fileHistory->AddFilesToMenu();
}

size_t wxDocManager::GetHistoryFilesCount() const
{
    return m_fileHistory ? m_fileHistory->GetCount() : 0;
}


// Find out the document template via matching in the document file format
// against that of the template
wxDocTemplate* wxDocManager::FindTemplateForPath(const wxString& path)
{
    wxDocTemplate* theTemplate = nullptr;

    // Find the template which this extension corresponds to
    for (size_t i = 0; i < m_templates.GetCount(); i++)
    {
        wxDocTemplate* temp = (wxDocTemplate*)m_templates.Item(i)->GetData();
        if (temp->FileMatchesTemplate(path))
        {
            theTemplate = temp;
            break;
        }
    }
    return theTemplate;
}

// Prompts user to open a file, using file specs in templates.
// Must extend the file selector dialog or implement own; OR
// match the extension to the template extension.

wxDocTemplate* wxDocManager::SelectDocumentPath(wxDocTemplate** templates,
    int noTemplates,
    wxString& path,
    long WXUNUSED(flags),
    bool WXUNUSED(save))
{
#ifdef wxHAS_MULTIPLE_FILEDLG_FILTERS
    wxString descrBuf;

    for (int i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible())
        {
            // add a '|' to separate this filter from the previous one
            if (!descrBuf.empty())
                descrBuf << wxT('|');

            descrBuf << templates[i]->GetDescription()
                << wxT(" (") << templates[i]->GetFileFilter() << wxT(") |")
                << templates[i]->GetFileFilter();
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

    wxDocTemplate* theTemplate = nullptr;
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

wxDocTemplate* wxDocManager::SelectDocumentType(wxDocTemplate** templates,
    int noTemplates, bool sort)
{
    wxArrayString strings;
    wxScopedArray<wxDocTemplate*> data(noTemplates);
    int i;
    int n = 0;

    for (i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible())
        {
            int j;
            bool want = true;
            for (j = 0; j < n; j++)
            {
                //filter out NOT unique documents + view combinations
                if (templates[i]->m_docTypeName == data[j]->m_docTypeName &&
                    templates[i]->m_viewTypeName == data[j]->m_viewTypeName
                    )
                    want = false;
            }

            if (want)
            {
                strings.Add(templates[i]->m_description);

                data[n] = templates[i];
                n++;
            }
        }
    }  // for

    if (sort)
    {
        strings.Sort(); // ascending sort
        // Yes, this will be slow, but template lists
        // are typically short.
        int j;
        n = strings.Count();
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < noTemplates; j++)
            {
                if (strings[i] == templates[j]->m_description)
                    data[i] = templates[j];
            }
        }
    }

    wxDocTemplate* theTemplate;

    switch (n)
    {
    case 0:
        // no visible templates, hence nothing to choose from
        theTemplate = nullptr;
        break;

    case 1:
        // don't propose the user to choose if he has no choice
        theTemplate = data[0];
        break;

    default:
        // propose the user to choose one of several
        theTemplate = (wxDocTemplate*)wxGetSingleChoiceData
        (
            _("Select a document template"),
            _("Templates"),
            strings,
            (void**)data.get()
        );
    }

    return theTemplate;
}

wxDocTemplate* wxDocManager::SelectViewType(wxDocTemplate** templates,
    int noTemplates, bool sort)
{
    wxArrayString strings;
    wxScopedArray<wxDocTemplate*> data(noTemplates);
    int i;
    int n = 0;

    for (i = 0; i < noTemplates; i++)
    {
        wxDocTemplate* templ = templates[i];
        if (templ->IsVisible() && !templ->GetViewName().empty())
        {
            int j;
            bool want = true;
            for (j = 0; j < n; j++)
            {
                //filter out NOT unique views
                if (templates[i]->m_viewTypeName == data[j]->m_viewTypeName)
                    want = false;
            }

            if (want)
            {
                strings.Add(templ->m_viewTypeName);
                data[n] = templ;
                n++;
            }
        }
    }

    if (sort)
    {
        strings.Sort(); // ascending sort
        // Yes, this will be slow, but template lists
        // are typically short.
        int j;
        n = strings.Count();
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < noTemplates; j++)
            {
                if (strings[i] == templates[j]->m_viewTypeName)
                    data[i] = templates[j];
            }
        }
    }

    wxDocTemplate* theTemplate;

    // the same logic as above
    switch (n)
    {
    case 0:
        theTemplate = nullptr;
        break;

    case 1:
        theTemplate = data[0];
        break;

    default:
        theTemplate = (wxDocTemplate*)wxGetSingleChoiceData
        (
            _("Select a document view"),
            _("Views"),
            strings,
            (void**)data.get()
        );

    }

    return theTemplate;
}

void wxDocManager::AssociateTemplate(wxDocTemplate* temp)
{
    if (!m_templates.Member(temp))
        m_templates.Append(temp);
}

void wxDocManager::DisassociateTemplate(wxDocTemplate* temp)
{
    m_templates.DeleteObject(temp);
}

wxDocTemplate* wxDocManager::FindTemplate(const wxClassInfo* classinfo)
{
    for (wxList::compatibility_iterator node = m_templates.GetFirst();
        node;
        node = node->GetNext())
    {
        wxDocTemplate* t = wxStaticCast(node->GetData(), wxDocTemplate);
        if (t->GetDocClassInfo() == classinfo)
            return t;
    }

    return nullptr;
}

// Add and remove a document from the manager's list
void wxDocManager::AddDocument(wxDocument* doc)
{
    if (!m_docs.Member(doc))
        m_docs.Append(doc);
}

void wxDocManager::RemoveDocument(wxDocument* doc)
{
    m_docs.DeleteObject(doc);
}

// Views or windows should inform the document manager
// when a view is going in or out of focus
void wxDocManager::ActivateView(wxView* view, bool activate)
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

// ----------------------------------------------------------------------------
// wxDocChildFrameAnyBase
// ----------------------------------------------------------------------------

bool wxDocChildFrameAnyBase::TryProcessEvent(wxEvent& event)
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

bool wxDocChildFrameAnyBase::CloseView(wxCloseEvent& event)
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
        wxDELETE(m_childView);
    }

    m_childDocument = nullptr;

    return true;
}

// ----------------------------------------------------------------------------
// wxDocParentFrameAnyBase
// ----------------------------------------------------------------------------

bool wxDocParentFrameAnyBase::TryProcessEvent(wxEvent& event)
{
    if (!m_docManager)
        return false;

    // If we have an active view, its associated child frame may have
    // already forwarded the event to wxDocManager, check for this:
    if (wxView* const view = m_docManager->GetAnyUsableView())
    {
        wxDocChildFrameAnyBase* const childFrame = view->GetDocChildFrame();
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

    wxString GetAppropriateTitle(const wxView* view, const wxString& titleGiven)
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

wxDocPrintout::wxDocPrintout(wxView* view, const wxString& title)
    : wxPrintout(GetAppropriateTitle(view, title))
{
    m_printoutView = view;
}

bool wxDocPrintout::OnPrintPage(int WXUNUSED(page))
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

bool wxDocPrintout::HasPage(int pageNum)
{
    return (pageNum == 1);
}

bool wxDocPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void wxDocPrintout::GetPageInfo(int* minPage, int* maxPage,
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

#endif // wxUSE_DOC_VIEW_ARCHITECTURE


/////////////////////////////////////////////////////////////////////////////
// Name:        wx/docmdi.h
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) 1997 Julian Smart
//              (c) 2010 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCMDI_H_
#define _WX_DOCMDI_H_

#include "wx/defs.h"

#if wxUSE_MDI_ARCHITECTURE

#include "wx/docview.h"
#include "wx/mdi.h"

// Define MDI versions of the doc-view frame classes. Note that we need to
// define them as classes for wxRTTI, otherwise we could simply define them as
// typedefs.

// ----------------------------------------------------------------------------
// An MDI document parent frame
// ----------------------------------------------------------------------------

typedef
wxDocParentFrameAny<wxMDIParentFrame> wxDocMDIParentFrameBase;

class WXDLLIMPEXP_CORE wxDocMDIParentFrame : public wxDocMDIParentFrameBase
{
public:
    wxDocMDIParentFrame() : wxDocMDIParentFrameBase() {}

    wxDocMDIParentFrame(wxDocManager* manager,
        wxFrame* parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxASCII_STR(wxFrameNameStr))
        : wxDocMDIParentFrameBase(manager,
            parent, id, title, pos, size, style, name)
    {
    }

private:
    wxDECLARE_CLASS(wxDocMDIParentFrame);
    wxDECLARE_NO_COPY_CLASS(wxDocMDIParentFrame);
};

// ----------------------------------------------------------------------------
// An MDI document child frame
// ----------------------------------------------------------------------------

typedef
wxDocChildFrameAny<wxMDIChildFrame, wxMDIParentFrame> wxDocMDIChildFrameBase;

class WXDLLIMPEXP_CORE wxDocMDIChildFrame : public wxDocMDIChildFrameBase
{
public:
    wxDocMDIChildFrame() = default;

    wxDocMDIChildFrame(wxDocument* doc,
        wxView* view,
        wxMDIParentFrame* parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxASCII_STR(wxFrameNameStr))
        : wxDocMDIChildFrameBase(doc, view,
            parent, id, title, pos, size, style, name)
    {
    }

private:
    wxDECLARE_CLASS(wxDocMDIChildFrame);
    wxDECLARE_NO_COPY_CLASS(wxDocMDIChildFrame);
};

#endif // wxUSE_MDI_ARCHITECTURE

#endif // _WX_DOCMDI_H_


wxIMPLEMENT_CLASS(wxDocMDIParentFrame, wxMDIParentFrame);
wxIMPLEMENT_CLASS(wxDocMDIChildFrame, wxMDIChildFrame);


class MyCanvas;

// Define a new application
class MyApp : public wxApp
{
public:
    // this sample can be launched in several different ways:
    enum Mode
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

    MyApp();
    MyApp(const MyApp&) = delete;
    MyApp& operator=(const MyApp&) = delete;

    // override some wxApp virtual methods
    virtual bool OnInit() override;
    virtual int OnExit() override;

    virtual void OnInitCmdLine(wxCmdLineParser& parser) override;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;

#ifdef __WXMAC__
    virtual void MacNewFile() override;
#endif // __WXMAC__

    // our specific methods
    Mode GetMode() const { return m_mode; }
    wxFrame* CreateChildFrame(wxView* view, bool isCanvas);

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
    wxVector<wxString> m_filesFromCmdLine;

    // the currently used mode
    Mode m_mode;

    // only used if m_mode == Mode_Single
    MyCanvas* m_canvas;
    wxMenu* m_menuEdit;

    wxDECLARE_EVENT_TABLE();
};

wxDECLARE_APP(MyApp);


/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.cpp
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

 // ----------------------------------------------------------------------------
 // headers
 // ----------------------------------------------------------------------------

 // For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"

#if wxUSE_AUI
#include "wx/aui/tabmdi.h"
#endif // wxUSE_AUI

//#include "doc.h"
//#include "view.h"

// The window showing the drawing itself
class MyCanvas : public wxScrolledWindow
{
public:
    // view may be null if we're not associated with one yet, but parent must
    // be a valid pointer
    MyCanvas(wxView* view, wxWindow* parent = nullptr);
    virtual ~MyCanvas();

    virtual void OnDraw(wxDC& dc) override;

    // in a normal multiple document application a canvas is associated with
    // one view from the beginning until the end, but to support the single
    // document mode in which all documents reuse the same MyApp::GetCanvas()
    // we need to allow switching the canvas from one view to another one

    void SetView(wxView* view)
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

    wxView* m_view;

    // the segment being currently drawn or nullptr if none
    //DoodleSegment* m_currentSegment;

    // the last mouse press position
    wxPoint m_lastMousePos;

    wxDECLARE_EVENT_TABLE();
};



wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
wxEND_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxView* view, wxWindow* parent)
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

    // is this the start of a new segment?

    m_lastMousePos = pt;
}



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

// ----------------------------------------------------------------------------
// MyApp implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyApp, wxApp)
EVT_MENU(wxID_ABOUT, MyApp::OnAbout)
EVT_MENU(wxID_CLEAR, MyApp::OnForceCloseAll)
wxEND_EVENT_TABLE()

MyApp::MyApp()
{
#if wxUSE_MDI_ARCHITECTURE
    m_mode = Mode_MDI;
#else
    m_mode = Mode_SDI;
#endif

    m_canvas = nullptr;
    m_menuEdit = nullptr;
}

// constants for the command line options names
namespace CmdLineOption
{

#if wxUSE_MDI_ARCHITECTURE
    const char* const MDI = "mdi";
#endif // wxUSE_MDI_ARCHITECTURE
#if wxUSE_AUI
    const char* const AUI = "aui";
#endif // wxUSE_AUI
    const char* const SDI = "sdi";
    const char* const SINGLE = "single";

} // namespace CmdLineOption

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
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

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    int numModeOptions = 0;

#if wxUSE_MDI_ARCHITECTURE
    if (parser.Found(CmdLineOption::MDI))
    {
        m_mode = Mode_MDI;
        numModeOptions++;
    }
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
    if (parser.Found(CmdLineOption::AUI))
    {
        m_mode = Mode_AUI;
        numModeOptions++;
    }
#endif // wxUSE_AUI

    if (parser.Found(CmdLineOption::SDI))
    {
        m_mode = Mode_SDI;
        numModeOptions++;
    }

    if (parser.Found(CmdLineOption::SINGLE))
    {
        m_mode = Mode_Single;
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
void MyApp::MacNewFile()
{
    wxDocManager::GetDocumentManager()->CreateNewDocument();
}
#endif // __WXMAC__

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    ::wxInitAllImageHandlers();

    // Fill in the application information fields before creating wxConfig.
    SetVendorName("wxWidgets_Samples");
    SetAppName("docview");
    SetAppDisplayName("wxWidgets DocView Sample");

    //// Create a document manager
    wxDocManager* docManager = new wxDocManager;

    //// Create a template relating drawing documents to their views
    //new wxDocTemplate(docManager, "Drawing", "*.drw", "", "drw",
    //    "Drawing Doc", "Drawing View",
    //    CLASSINFO(DrawingDocument), CLASSINFO(DrawingView));

    if (m_mode == Mode_Single)
    {
        // If we've only got one window, we only get to edit one document at a
        // time. Therefore no text editing, just doodling.
        docManager->SetMaxDocsOpen(1);
    }
    else // multiple documents mode: allow documents of different types
    {
        // Create a template relating text documents to their views
        new wxDocTemplate(docManager, "Text", "*.txt;*.text", "", "txt;text",
            "Text Doc", "Text View",
            CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));
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
    case Mode_MDI:
        frame = new wxDocMDIParentFrame(docManager, nullptr, wxID_ANY,
            GetAppDisplayName(),
            wxDefaultPosition,
            wxWindow::FromDIP(wxSize(1280, 720), nullptr));
        break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
    case Mode_AUI:
        frame = new wxDocParentFrameAny<wxAuiMDIParentFrame>
            (
                docManager, nullptr, wxID_ANY,
                GetAppDisplayName(),
                wxDefaultPosition,
                wxWindow::FromDIP(wxSize(1280, 720), nullptr)
            );
        break;
#endif // wxUSE_AUI

    case Mode_SDI:
    case Mode_Single:
        frame = new wxDocParentFrame(docManager, nullptr, wxID_ANY,
            GetAppDisplayName(),
            wxDefaultPosition,
            wxWindow::FromDIP(wxSize(1280, 720), nullptr));
        break;
    }

    // and its menu bar
    wxMenu* menuFile = new wxMenu;

    menuFile->Append(wxID_NEW);
    menuFile->Append(wxID_OPEN);

    if (m_mode == Mode_Single)
        AppendDocumentFileCommands(menuFile, true);

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    // A nice touch: a history of files visited. Use this menu.
    docManager->FileHistoryUseMenu(menuFile);
#if wxUSE_CONFIG
    docManager->FileHistoryLoad(*wxConfig::Get());
#endif // wxUSE_CONFIG


    if (m_mode == Mode_Single)
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
        for (size_t i = 0; i != m_filesFromCmdLine.size(); ++i)
            docManager->CreateDocument(m_filesFromCmdLine[i], wxDOC_SILENT);
    }

    return true;
}

int MyApp::OnExit()
{
    wxDocManager* const manager = wxDocManager::GetDocumentManager();
#if wxUSE_CONFIG
    manager->FileHistorySave(*wxConfig::Get());
#endif // wxUSE_CONFIG
    delete manager;

    return wxApp::OnExit();
}

void MyApp::AppendDocumentFileCommands(wxMenu* menu, bool supportsPrinting)
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

wxMenu* MyApp::CreateDrawingEditMenu()
{
    wxMenu* const menu = new wxMenu;
    menu->Append(wxID_UNDO);
    menu->Append(wxID_REDO);
    menu->AppendSeparator();
    menu->Append(wxID_CUT, "&Cut last segment");

    return menu;
}

void MyApp::CreateMenuBarForFrame(wxFrame* frame, wxMenu* file, wxMenu* edit)
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

wxFrame* MyApp::CreateChildFrame(wxView* view, bool isCanvas)
{
    // create a child frame of appropriate class for the current mode
    wxFrame* subframe = nullptr;
    wxDocument* doc = view->GetDocument();
    switch (GetMode())
#if wxUSE_MDI_ARCHITECTURE
    {
    case Mode_MDI:
        subframe = new wxDocMDIChildFrame
        (
            doc,
            view,
            wxStaticCast(GetTopWindow(), wxDocMDIParentFrame),
            wxID_ANY,
            "Child Frame",
            wxDefaultPosition,
            wxWindow::FromDIP(wxSize(640, 480), nullptr)
        );
        break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
    case Mode_AUI:
        subframe = new wxDocChildFrameAny<wxAuiMDIChildFrame, wxAuiMDIParentFrame>
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

    case Mode_SDI:
    case Mode_Single:
        subframe = new wxDocChildFrame
        (
            doc,
            view,
            wxStaticCast(GetTopWindow(), wxDocParentFrame),
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

void MyApp::OnForceCloseAll(wxCommandEvent& WXUNUSED(event))
{
    // Pass "true" here to force closing just for testing this functionality,
    // there is no real reason to force the issue here.
    wxDocManager::GetDocumentManager()->CloseDocuments(true);
}

void MyApp::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString modeName;
    switch (m_mode)
    {
#if wxUSE_MDI_ARCHITECTURE
    case Mode_MDI:
        modeName = "MDI";
        break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
    case Mode_AUI:
        modeName = "AUI";
        break;
#endif // wxUSE_AUI

    case Mode_SDI:
        modeName = "SDI";
        break;

    case Mode_Single:
        modeName = "single document";
        break;

    default:
        wxFAIL_MSG("unknown mode ");
    }

    const int docsCount =
        wxDocManager::GetDocumentManager()->GetDocumentsVector().size();

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


wxIMPLEMENT_DYNAMIC_CLASS(TextEditDocument, wxDocument);

bool TextEditDocument::OnCreate(const wxString& path, long flags)
{
    if (!wxDocument::OnCreate(path, flags))
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
    return wxDocument::IsModified() || (wnd && wnd->IsModified());
}

void TextEditDocument::Modify(bool modified)
{
    wxDocument::Modify(modified);

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
    wxView* view = GetFirstView();
    return view ? wxStaticCast(view, TextEditView)->GetText() : nullptr;
}
wxIMPLEMENT_DYNAMIC_CLASS(TextEditView, wxView);

wxBEGIN_EVENT_TABLE(TextEditView, wxView)
EVT_MENU(wxID_COPY, TextEditView::OnCopy)
EVT_MENU(wxID_PASTE, TextEditView::OnPaste)
EVT_MENU(wxID_SELECTALL, TextEditView::OnSelectAll)
wxEND_EVENT_TABLE()

bool TextEditView::OnCreate(wxDocument* doc, long flags)
{
    if (!wxView::OnCreate(doc, flags))
        return false;

    wxFrame* frame = wxGetApp().CreateChildFrame(this, false);
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
    if (!wxView::OnClose(deleteWindow))
        return false;

    Activate(false);

    if (wxGetApp().GetMode() == MyApp::Mode_Single)
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

