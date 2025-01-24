#include "ftkMainWnd.h"

#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/aboutdlg.h>

//#include "wxwin16x16.xpm"
//#include "new.xpm"
//#include "fileopen.xpm"
//#include "filesave.xpm"
//#include "htmfoldr.xpm"

//#include "cut.xpm"
//#include "copy.xpm"
//#include "find.xpm"


namespace ftk
{

enum MENU_ID_ENUM
{
  MENU_FILE_QUIT = wxID_EXIT,
  MENU_FILE_OPEN = wxID_OPEN,
  MENU_FILE_NEW = wxID_NEW,
  // it is important for the id corresponding to the "About" command to have
  // this standard value as otherwise it won't be handled properly under Mac
  // (where it is special and put into the "Apple" menu)
  MENU_ABOUT = wxID_ABOUT,
  ID_START = wxID_LAST,
  ID_STANDARD_TOOLBAR = ID_START,
  ID_ADDITIONAL_TOOLBAR,
  ID_NOTEBOOK,
  ID_SIDE_NOTEBOOK,
  //ID_INFO_TREE,
  ID_STATUSBAR,
  ID_LOGTEXTCTRL,
  ID_END,
  ID_LOAD_LAYOUT,
  ID_SAVE_LAYOUT,
  ID_OP_START,
  ID_OP_END = ID_OP_START + 1000,
  ID_VIEW_START = ID_OP_END,
  ID_VIEW_END = ID_VIEW_START + 1000
};

BEGIN_EVENT_TABLE(MainWnd, MainWndBase)
  EVT_CLOSE(MainWnd::OnCloseWindow)
/*  EVT_DROP_FILES(MainWnd::OnDropFile)
  EVT_IDLE(MainWnd::OnIdle)  
  EVT_MENU(MENU_FILE_QUIT,MainWnd::OnMenu)
  EVT_MENU(MENU_FILE_OPEN,MainWnd::OnMenu)
  EVT_MENU(MENU_FILE_NEW,MainWnd::OnMenu)
  //wxEVT_COMMAND_MENU_SELECTED(MainWnd::OnMenu);
  EVT_MENU(MENU_ABOUT,MainWnd::OnAbout)
  EVT_MENU_RANGE(ID_START,ID_END,MainWnd::OnMenu)
  EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MainWnd::OnMenu)
  EVT_SIZE(MainWnd::OnSize)
  EVT_UPDATE_UI_RANGE(ID_START,ID_END,MainWnd::OnUpdateUI)
  EVT_MENU(ID_LOAD_LAYOUT, MainWnd::OnLoadLayout)
  EVT_MENU(ID_SAVE_LAYOUT, MainWnd::OnSaveLayout)*/
END_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(ftk::MainWnd, MainWndBase);

MainWnd::MainWnd() : MainWndBase()//,/* m_ToolBar(nullptr),*/ m_AuiManager(nullptr)
{

}

MainWnd::MainWnd(wxWindow *parent, wxWindowID id, const wxString& title, 
                                   const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
  MainWndBase()//,/* m_ToolBar(nullptr),*/ //m_AuiManager(nullptr)
{
  Create(parent, id, title, pos, size, style, name);

  // Paolo 10 Jul 2006: due to position correctly the progress bar.
  /*wxRect pr;
  GetStatusBar()->GetFieldRect(4,pr);*/
}

MainWnd::~MainWnd()
{
  //delete m_NodeManager;
//   if(m_AuiManager)
//   {
//     m_AuiManager->UnInit();
//     delete m_AuiManager;
//   }
}

bool MainWnd::Create(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                              const wxSize& size, long style, const wxString& name)
{
  if(!MainWndBase::Create(parent, id, title, pos, size, style, name))
  //if(!wxAuiMDIParentFrame::Create(parent, id, title, wxDefaultPosition, wxSize(600,500), wxDEFAULT_FRAME_STYLE|wxFRAME_NO_WINDOW_MENU))
    return false;



  /*m_NodeManager = new mafNodeManager;
  {
    ptr::shared_ptr<mafNode> node1(mafNode::New());
    node1->SetName(_MST("Root"));
    ptr::shared_ptr<mafNode> node2(mafNode::New());
    node2->SetName(_MST("Node2"));
    ptr::shared_ptr<mafNode> node3(mafNode::New());
    node3->SetName(_MST("Node3"));
    ptr::shared_ptr<mafNode> node4(mafNode::New());
    node4->SetName(_MST("Node4"));
    node1->AddChild(node2);
    node1->AddChild(node3);
    node2->AddChild(node4);
    /if(*node1 == *node2)
    {
      return 0;
    }/
    //const mafProperties& props = node1->GetProperties();
    m_NodeManager->SetRoot(node1);
  }*/

//  CreateControls();
  //wxMDIChildFrame *cf = new wxMDIChildFrame(this, -1, "child",wxDefaultPosition, wxDefaultSize/*, 0*/);
  //cf->Show();
  return true;
}

/*void MainWnd::OnCloseWindow(wxCloseEvent& event)
{
  MainWndBase::OnCloseWindow(event);
}
void MainWnd::OnDropFile(wxDropFilesEvent &event)
{

}*/
#include <malloc.h>
size_t GetFreeMemorySize(size_t* pLargest)
{
  MEMORY_BASIC_INFORMATION ssMemInfo;
  memset(&ssMemInfo, 0, sizeof(ssMemInfo));
  ssMemInfo.BaseAddress = (LPVOID)0x10000;    //First 64 KB is reserved

  size_t nTotalMem = 0;
  size_t nLargestFree = 0;
  while (VirtualQuery(ssMemInfo.BaseAddress, &ssMemInfo, sizeof(ssMemInfo)) != 0)
  {    
    if (ssMemInfo.State == MEM_FREE)
    {
      nTotalMem += ssMemInfo.RegionSize;
      if (ssMemInfo.RegionSize > nLargestFree)
        nLargestFree = ssMemInfo.RegionSize;
    }

    ssMemInfo.BaseAddress = ((BYTE*)ssMemInfo.BaseAddress) + ssMemInfo.RegionSize;
    //VirtualQuery returns 0 if the passed address goes into the kernel region
    //which is on top addresses
  }

  //we may have also some free memory in the CRT heap
  _HEAPINFO hinfo;
  int heapstatus;  
  hinfo._pentry = NULL;

  while((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
  {
    if (hinfo._useflag != _USEDENTRY)
    {
      nTotalMem += hinfo._size;    
      if (hinfo._size > nLargestFree)
        nLargestFree = hinfo._size;      
    }
  }

  if (pLargest != NULL)
    *pLargest = nLargestFree;
  return nTotalMem;
}
#ifdef iuoiujkhhk

void MainWnd::OnIdle(wxIdleEvent& event)
{
  /*MEMORYSTATUS ms;
  GlobalMemoryStatus( &ms );
  wxString s;
  int current_free_memory = ms.dwAvailPhys/1000000;
  s << "free mem " << current_free_memory << " mb";   
  SetStatusText(s,5);*/
  /*static DWORD dwLastTime = 0;
  DWORD dwCurTime = GetTickCount();
  if (dwCurTime - dwLastTime < 1000)
    return; //1 second is the minimal time

  //BES: 30.5.2008 - GlobalMemoryStatus actually measures available free physical 
  //memory in the whole system, it only tells you that your next memory operations
  //may be slow as some data will have to be swapped on disk and vice versa.
  //it is NOT related to the memory consumption of the running application
  //	MEMORYSTATUS ms;
  //	GlobalMemoryStatus( &ms );         

  size_t nLargest;
  int current_free_memory = (int)(GetFreeMemorySize(&nLargest) / (1024*1024));
  //ms.dwAvailPhys/1000000;

  static bool bShowTotal = false;
  if (bShowTotal)
  {
    //6 seconds for total free
    if ((dwCurTime - dwLastTime) >= 6000)
    {
      dwLastTime = dwCurTime;
      bShowTotal = false;
    }
  }
  else
  {
    //3 seconds for total free
    if ((dwCurTime - dwLastTime) >= 3000)
    {
      dwLastTime = dwCurTime;
      bShowTotal = true;
    }
  }

  wxString s;
  if (bShowTotal)
    s << "free mem: " << current_free_memory << " mb";
  else
    s << "largest: " << (int)(nLargest / (1024*1024)) << " mb";
  SetStatusText(s,5);  
  //GetStatusBar()->SetToolTip(s);
  /*if (current_free_memory < m_MemoryLimitAlert && !m_UserAlerted)
  {
    m_UserAlerted = true;
    int answere = wxMessageBox(_("Program is running with few free memory!! \nFree memory used by UnDo stack?."), _("Warning"), wxYES_NO);
    if (answere == wxYES)
    {
      // Clear UnDo stack to gain memory.
      mafEventMacro(mafEvent(this, CLEAR_UNDO_STACK));
    }
  }*/
}
void MainWnd::OnLayout(wxCommandEvent& event)
{
}
void MainWnd::OnMenu(wxCommandEvent& e)
{
  //if(e.GetId() > SASH_START && e.GetId() < SASH_END )
  //  ShowDockPane(e.GetId(), !DockPaneIsShown( (wxWindow*)(e.GetEventObject())) );
  // else
  //mafEventMacro(mafEvent(this,e.GetId()));
  long panels_id[]  = {ID_LOGTEXTCTRL, ID_STANDARD_TOOLBAR, ID_ADDITIONAL_TOOLBAR, ID_SIDE_NOTEBOOK};
  wxString panels[] = {wxT("Help"), wxT("Standard Toolbar"), wxT("Additional Toolbar"), wxT("Information")};
  for(int i = 0; i < 4; i++)
  {
    if(e.GetId() == panels_id[i])
    {
      TogglePaneVisibility(panels[i]);
      m_AuiManager->Update();
      return;
    }
  }

  if(e.GetId() == MENU_FILE_QUIT)
  {
    Close();
    return;
  }
  if(e.GetId() == MENU_FILE_NEW)
  {
    return;
  }
}
//void MainWnd::OnSashDrag(wxSashEvent& event);
//{
//}
void MainWnd::OnSize(wxSizeEvent& event)
{
}
void MainWnd::OnUpdateUI(wxUpdateUIEvent& e)
{
  long panels_id[]  = {ID_LOGTEXTCTRL, ID_STANDARD_TOOLBAR, ID_ADDITIONAL_TOOLBAR, ID_SIDE_NOTEBOOK};
  wxString panels[] = {wxT("Help"), wxT("Standard Toolbar"), wxT("Additional Toolbar"), wxT("Information")};
  for(int i = 0; i < 4; i++)
  {
    if(e.GetId() == panels_id[i])
      e.Check(GetPaneVisibility(panels[i]));
  }
}
void MainWnd::CreateControls()
{
  CreateMenuBar();
  
  //m_Frame->CreateToolBar();
  m_AuiManager = new wxAuiManager;
  m_AuiManager->SetManagedWindow(this);

  //GetNotebook()->SetWindowStyleFlag(wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxNO_BORDER);

  static const int widths[]={-1,60,60,60,210,100}; // enlarged the last tab for the "free mem watch" information
  CreateStatusBar(6); 
  /*GetStatusBar()->SetStatusWidths(6,widths);
  SetStatusText( _("welcome"),0);
  SetStatusText( " ",1);
  SetStatusText( " ",2);
  SetStatusText( " ",3);*/


  m_LOGTextCtrl = new wxTextCtrl(this, ID_LOGTEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(250, 100), wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER);

  CreateStdToolBar();
  CreateAddToolBar();
  CreateInfoTree();

  //m_Notebook = new wxAuiNotebook(this, ID_NOTEBOOK, wxDefaultPosition, wxSize(600, 450), wxAUI_NB_DEFAULT_STYLE|wxNO_BORDER);
  //m_Page1 = new wxPanel(m_Notebook, wxID_ANY);
  //m_Page2 = new wxPanel(m_Notebook, wxID_ANY);
  //m_Notebook->AddPage(m_Page1, _("Page1"));
  //m_Notebook->AddPage(m_Page2, _("Page2"));
  //m_Page1 = new wxPanel(GetNotebook(), wxID_ANY);
  //m_Page2 = new wxPanel(GetNotebook(), wxID_ANY);
  //GetNotebook()->AddPage(m_Page1, _("Page1"));
  //GetNotebook()->AddPage(m_Page2, _("Page2"));


  //m_AuiManager->AddPane(m_Notebook, wxAuiPaneInfo().CenterPane());
  //m_AuiManager->AddPane(m_StdToolBar, wxAuiPaneInfo().ToolbarPane().Name(wxT("Standard Toolbar")).Top().Floatable(false));
  //m_AuiManager->AddPane(m_AddToolBar, wxAuiPaneInfo().ToolbarPane().Name(wxT("Additional Toolbar")).Top().Floatable(false));//Layer(2)
  //m_AuiManager->AddPane(m_InfoTree, wxAuiPaneInfo().Left().Layer(1).PinButton().MinimizeButton().MaximizeButton().Name(wxT("Information")).Caption(wxT("Information")));
  AddPane(m_StdToolBar, wxAuiPaneInfo().ToolbarPane().Caption(wxT("Standard")).Name(wxT("Standard Toolbar")).Top().Floatable(false), wxT("&View"), wxT("Toolbars"));
  AddPane(m_AddToolBar, wxAuiPaneInfo().ToolbarPane().Caption(wxT("Additional")).Name(wxT("Additional Toolbar")).Top().Floatable(false), wxT("&View"), wxT("Toolbars"));//Layer(2)
  AddPane(m_SideNotebook, wxAuiPaneInfo().Right().Layer(1)/*.PinButton().MinimizeButton().MaximizeButton()*/.Name(wxT("Information")).Caption(wxT("Information")).MinSize(240,450));

  //AddPane(GetStatusBar(), wxAuiPaneInfo().Bottom().Layer(1).Name(wxT("Statusbar")).Caption(wxT("Statusbar")));

  //RemovePane(m_StdToolBar);
  //delete m_StdToolBar;

  //m_AuiManager->AddPane(GetNotebook(), wxAuiPaneInfo().Name(wxT("CenterPane")).CenterPane());
  //m_AuiManager->AddPane(m_LOGTextCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Help")).Caption(_("Help Window")));
  AddPane(m_LOGTextCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Help")).Caption(_("Help Window")));


  //m_Page1 = new wxAuiMDIChildFrame(this, wxID_ANY, _("Page1"));
  //m_Page2 = new wxAuiMDIChildFrame(this, wxID_ANY, _("Page2"));
  //GetNotebook()->AddPage(m_Page1, _("Page1"));
  //GetNotebook()->AddPage(m_Page2, _("Page2"));
  //GetNotebook()->SetSelection(0);


  /*m_AuiManager->AddPane(m_Notebook, wxAuiPaneInfo().CenterPane());
  m_AuiManager->AddPane(m_StdToolBar, wxAuiPaneInfo().ToolbarPane().Top().Floatable(false));
  m_AuiManager->AddPane(m_AddToolBar, wxAuiPaneInfo().ToolbarPane().Top().Position(2).Floatable(false));
  m_AuiManager->AddPane(m_InfoTree, wxAuiPaneInfo().Left().Layer(1).PinButton().MinimizeButton().MaximizeButton().Caption(wxT("Information")));*/

  /*m_AuiManager->GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,0 );
  m_AuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, m_AuiManager->GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR));
  m_AuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_AuiManager->GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR));
  m_AuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, m_AuiManager->GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR));*/

  m_AuiManager->Update();
}

void MainWnd::CreateMenuBar()
{
  wxMenuBar * menuBar = new wxMenuBar;
  SetMenuBar(menuBar);

  wxMenu * fileMenu = new wxMenu;
  fileMenu->Append(wxID_NEW, _("New\tCtrl+N"));
  fileMenu->Append(wxID_OPEN, _("Open\tCtrl+O"));
  fileMenu->Append(wxID_SAVE, _("Save\tCtrl+S"));
  fileMenu->AppendSeparator();
  fileMenu->Append(MENU_FILE_QUIT, _("Exit\tAlt+F4"));

  wxMenu * toolbarsMenu = new wxMenu;
  //toolbarsMenu->AppendCheckItem(ID_TOGGLE_STANDARD_TOOLBAR, _("Standard"));
  //toolbarsMenu->AppendCheckItem(ID_TOGGLE_ADDITIONAL_TOOLBAR, _("Additional"));

  wxMenu * layoutMenu = new wxMenu;
  layoutMenu->Append(ID_LOAD_LAYOUT, _("Load"));
  layoutMenu->Append(ID_SAVE_LAYOUT, _("Save"));

  wxMenu * viewMenu = new wxMenu;
  viewMenu->Append(wxID_ANY, wxT("Toolbars"), toolbarsMenu);
  viewMenu->Append(wxID_ANY, _("Layout"), layoutMenu);
  //viewMenu->AppendCheckItem(ID_TOGGLE_INFO_TREE, _("Tree"));
  //viewMenu->AppendCheckItem(ID_TOGGLE_STATUSBAR, _("Status Bar"));
  //viewMenu->AppendCheckItem(ID_TOGGLE_LOGTEXTCTRL, _("Toggle Help Window"));

  wxMenu * opMenu = new wxMenu;


  wxMenu * helpMenu = new wxMenu;
  helpMenu->Append(MENU_ABOUT, _("About..."));

  menuBar->Append(fileMenu, _("File"));
  menuBar->Append(viewMenu, _("View"));
  menuBar->Append(opMenu, _("Operation"));
  menuBar->Append(helpMenu, _("Help"));
}

void MainWnd::CreateStdToolBar()
{
  m_StdToolBar = new wxToolBar(this, ID_STANDARD_TOOLBAR, wxDefaultPosition,
    wxDefaultSize, wxBORDER_NONE|wxTB_HORIZONTAL|wxTB_NODIVIDER|wxTB_FLAT);
  m_StdToolBar->SetToolBitmapSize(wxSize(16, 15));
  m_StdToolBar->AddTool(wxID_NEW, _("New"), wxBitmap(new_xpm));
  m_StdToolBar->AddTool(wxID_OPEN, _("Open"), wxBitmap(fileopen_xpm));
  m_StdToolBar->AddTool(wxID_SAVE, _("Save"), wxBitmap(filesave_xpm));
  m_StdToolBar->AddSeparator();
  m_StdToolBar->AddTool(wxID_ABOUT, _("About..."), wxBitmap(htmfoldr_xpm));
  m_StdToolBar->Realize();
}
void MainWnd::CreateAddToolBar()
{
  m_AddToolBar = new wxToolBar(this, ID_ADDITIONAL_TOOLBAR, wxDefaultPosition,
    wxDefaultSize, wxBORDER_NONE|wxTB_HORIZONTAL|wxTB_NODIVIDER|wxTB_FLAT);
  m_AddToolBar->SetToolBitmapSize(wxSize(16, 15));
  m_StdToolBar->AddSeparator();
  m_AddToolBar->AddTool(wxID_CUT, _("Cut"), wxBitmap(cut_xpm));
  m_AddToolBar->AddTool(wxID_COPY, _("Copy"), wxBitmap(copy_xpm));
  m_AddToolBar->AddTool(wxID_FIND, _("Find"), wxBitmap(find_xpm));
  m_AddToolBar->Realize();

}
/*void FillTree(wxTreeCtrl *tree, const wxTreeItemId& id, mafNode::Pointer node)
{
  for(size_t i = 0; i < node->GetNumberOfChildren(); i++)
  {
    mafNode::Pointer child = node->GetChild(i);
    FillTree(tree, tree->AppendItem(id, child->GetName().c_str()), child);
  }
}*/
void MainWnd::CreateInfoTree()
{
  m_SideNotebook = new fbg::ControlPanel(this, ID_SIDE_NOTEBOOK, wxDefaultPosition, wxSize(200, 500));
}

void MainWnd::AddPane(wxWindow *window, wxAuiPaneInfo& pane_info, const wxString& menu, const wxString& subMenu)
{
  m_AuiManager->AddPane(window, pane_info);
  m_AuiManager->Update();
  //AddMenuItem(window, pane_info.caption, menu, subMenu);
  if(pane_info.caption.IsEmpty())
    return;
  wxMenuBar* menubar = GetMenuBar();
  if(!menubar)
    return;
  int idx = menubar->FindMenu(menu);
  if(idx == wxNOT_FOUND)
    return;
  wxMenu *m = menubar->GetMenu(idx);
  if(subMenu != wxEmptyString)
  {
    int ids = m->FindItem(subMenu);
    wxMenuItem *sm = m->FindItem(ids);
    if(ids != wxNOT_FOUND && sm->GetSubMenu())
    {
      sm->GetSubMenu()->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
    }
  }
  else
  {
    m->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
  }
  //m_MenuBar->Check(id,);
}

bool MainWnd::GetPaneVisibility(const wxString& pane_name)
{
  /*wxAuiPaneInfoArray& all_panes = m_AuiManager->GetAllPanes();
  size_t i, count;
  for (i = 0, count = all_panes.GetCount(); i < count; ++i)
  {       
    if(all_panes.Item(i).name == pane_name)
    {
      return all_panes.Item(i).IsShown();
    }
  }
  return false;*/
  return IsPaneVisible(pane_name);
}

void MainWnd::TogglePaneVisibility(wxWindow *window)
{
  /*wxAuiPaneInfoArray& all_panes = m_AuiManager->GetAllPanes();
  size_t i, count;
  for (i = 0, count = all_panes.GetCount(); i < count; ++i)
  {       
    if(all_panes.Item(i).name == pane_name)
    {
      all_panes.Item(i).Show(!all_panes.Item(i).IsShown());
      m_AuiManager->Update();
      break;
    }
  }
  return;*/
  wxAuiPaneInfo& pi = m_AuiManager->GetPane(window);
  if(pi.IsOk())
    pi.Show(!pi.IsShown());
}

void MainWnd::TogglePaneVisibility(const wxString& pane_name)
{
  /*wxAuiPaneInfoArray& all_panes = m_AuiManager->GetAllPanes();
  size_t i, count;
  for (i = 0, count = all_panes.GetCount(); i < count; ++i)
  {       
    if(all_panes.Item(i).name == pane_name)
    {
      all_panes.Item(i).Show(!all_panes.Item(i).IsShown());
      m_AuiManager->Update();
      break;
    }
  }
  return;*/
  wxAuiPaneInfo& pi = m_AuiManager->GetPane(pane_name);
  if(pi.IsOk())
    pi.Show(!pi.IsShown());
}
void MainWnd::ShowPane(const wxString& pane_name, bool show)
{
  wxAuiPaneInfo& pi = m_AuiManager->GetPane(pane_name);
  if(pi.IsOk())
  {
    pi.Show(show);
    m_AuiManager->Update();
  }
}
void MainWnd::ShowPane(wxWindow *window, bool show)
{
  wxAuiPaneInfo& pi = m_AuiManager->GetPane(window);
  if(pi.IsOk())
  {
    pi.Show(show);
    m_AuiManager->Update();
  }
}
void MainWnd::OnAbout(wxCommandEvent & event)
{
  wxAboutDialogInfo info;
  info.SetName(wxT("FusionBox"));
  info.SetVersion(wxT("v1.0"));
  //info.SetWebSite(wxT("http://wxwidgets.info"));
  info.AddDeveloper(wxT("Fedor Moiseev (FedorMsv)"));
  wxAboutBox(info);
}

bool MainWnd::IsPaneVisible(const wxString& pane_name)
{
  wxAuiPaneInfo& pi = m_AuiManager->GetPane(pane_name);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}
bool MainWnd::IsPaneVisible(wxWindow *window)
{
  wxAuiPaneInfo& pi = m_AuiManager->GetPane(window);
  if(pi.IsOk())
    return pi.IsShown();
  return false;
}
void MainWnd::RemovePane(const wxString& pane_name)
{
  wxAuiPaneInfo& pi = m_AuiManager->GetPane(pane_name);
  if(pi.IsOk())
  {
    wxMenuBar* menubar = GetMenuBar();
    wxMenu *menu;
    if(menubar->FindItem(pi.window->GetId(), &menu))
      delete menu->Remove(pi.window->GetId());
    m_AuiManager->DetachPane(pi.window);
    m_AuiManager->Update();
  }
}
void MainWnd::RemovePane(wxWindow *window)
{
  wxAuiPaneInfo& pi = m_AuiManager->GetPane(window);
  if(pi.IsOk())
  {
    wxMenuBar* menubar = GetMenuBar();
    wxMenu *menu;
    if(menubar->FindItem(pi.window->GetId(), &menu))
      delete menu->Remove(pi.window->GetId());
    m_AuiManager->DetachPane(pi.window);
    m_AuiManager->Update();
  }
}

/*void MainWnd::OnToggleStatusbarUpdateUI(wxUpdateUIEvent & event)
{
  event.Check(GetStatusBar() != NULL);
}


void MainWnd::OnToggleStatusbar(wxCommandEvent & event)
{
  wxStatusBar * statusBar = GetStatusBar();
  if(statusBar != NULL)
  {
    SetStatusBar(NULL);
    statusBar->Destroy();
  }
  else
  {
    CreateStatusBar();
  }
  m_AuiManager->Update();
}*/

void MainWnd::OnLoadLayout(wxCommandEvent & event)
{
  wxFileDialog dlg(this, wxT("Choose a file"), wxEmptyString, wxEmptyString,
    wxT("wxAUI Layout (*.layout)|*.layout"), wxFD_OPEN);
  if(dlg.ShowModal() == wxID_OK)
  {
    LoadLayout(dlg.GetPath());
  }
}

void MainWnd::OnSaveLayout(wxCommandEvent & event)
{
  wxFileDialog dlg(this, wxT("Choose a file"), wxEmptyString, wxEmptyString,
    wxT("wxAUI Layout (*.layout)|*.layout"), wxFD_SAVE);
  if(dlg.ShowModal() == wxID_OK)
  {
    SaveLayout(dlg.GetPath());
  }
}

void MainWnd::LoadLayout(const wxString& filename)
{
  if(!wxFileExists(filename)) return;
  wxFileInputStream stream(filename);
  if(!stream.Ok()) return;
  int cnt = stream.GetLength()/sizeof(wxChar);
  wxChar * tmp = new wxChar[cnt+1];
  stream.Read(tmp, stream.GetLength());
  tmp[cnt] = wxChar(0);
  wxString perspective(tmp);
  delete [] tmp;
  m_AuiManager->LoadPerspective(perspective);
}

void MainWnd::SaveLayout(const wxString& filename)
{
  wxString perspective = m_AuiManager->SavePerspective(); 
  wxFileOutputStream stream(filename);
  if(!stream.Ok()) return;
  stream.Write(perspective.GetData(), perspective.Length()*sizeof(wxChar));
}




//void MainWnd::Configure()
//{
  //if(CreateMenuBar())
    //m_Frame->SetMenuBar(m_MenuBar);
  //m_Frame->CreateToolBar();
  /*if(CreateToolBar())
  {
    m_Frame->SetToolBar(m_ToolBar);
    /*m_AuiManager->AddPane(m_ToolBar,  wxAuiPaneInfo()
      .Name("toolbar")
      .Caption(wxT("ToolBar"))
      .Top()
      .Layer(2)
      .ToolbarPane()
      .LeftDockable(false)
      .RightDockable(false)
      .Floatable(false)
      .Movable(false)
      .Gripper(false)
      );
    m_AuiManager->Update();
  }*/
//}
/*bool MainWnd::CreateToolBar()
{
  m_ToolBar = new wxToolBar(m_Frame, 1025/*MENU_VIEW_TOOLBAR, wxPoint(0,0), wxSize(-1,-1), wxTB_FLAT | wxTB_NODIVIDER );
  m_ToolBar->SetMargins(0,0);
  m_ToolBar->SetToolSeparation(2);
  m_ToolBar->SetToolBitmapSize(wxSize(20,20));
  //m_ToolBar->AddTool(MENU_FILE_NEW,mafPics.GetBmp("FILE_NEW"),    _("new msf storage file"));
  //m_ToolBar->AddTool(MENU_FILE_OPEN,mafPics.GetBmp("FILE_OPEN"),  _("open msf storage file"));
  //m_ToolBar->AddTool(MENU_FILE_SAVE,mafPics.GetBmp("FILE_SAVE"),  _("save current msf storage file"));
  m_ToolBar->Realize();
  return true;
  return false;
}*/
#endif
    wxFrame* CreateFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                         const wxSize& size, long style, const wxString& name)
    {
        //auto p = new mafLogicWithGUI(nullptr);
        //return p->GetTopWin();
        return new MainWnd(parent, id, title, pos, size, style, name);
        //return new MainWnd(title, pos, size);
    }
}//namespace ftk
