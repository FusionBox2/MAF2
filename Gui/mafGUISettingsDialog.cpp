#include <wx/event.h>
#include "wx/busyinfo.h"
#include <wx/settings.h>
#include <wx/laywin.h>
#include <wx/imaglist.h>

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUISettingsDialog.h"
#include "ftk/Gui/wx/Tree.h"
#include "mafGUI.h"
#include "mafGUIHolder.h"
#include "mafPics.h"
#include "mafGUIDialog.h"

//----------------------------------------------------------------------------
// constant :
//----------------------------------------------------------------------------
enum mafGUISettingsDialog_IDS
{
	ID_SELECT_PAGE = MINID,
  ID_ROOT,
};
//----------------------------------------------------------------------------
mafGUISettingsDialog::mafGUISettingsDialog(const mafString& dialog_title)
//----------------------------------------------------------------------------
{  
  #include <pic/FOLDER.xpm>
  mafPictureFactory::GetPictureFactory()->Add(_R("FOLDER"), FOLDER_xpm);
  auto tree_images = std::make_unique<wxImageList>(20,20,FALSE,1);
  tree_images->Add(mafPictureFactory::GetPictureFactory()->GetBmp(_R("FOLDER")));

  m_Dialog = new mafGUIDialog(dialog_title);
	m_Tree   = new gui::wx::Tree(m_Dialog,ID_SELECT_PAGE,false,true);
	m_Guih   = new mafGUIHolder(m_Dialog,-1,false,true);

  m_Tree->SetListener(this);
	//m_Tree->SetTreeStyle( wxSIMPLE_BORDER | wxTR_HAS_BUTTONS );//| wxTR_HIDE_ROOT );
	m_Tree->SetSize(wxWindow::FromDIP(wxSize(270,400), nullptr));
	m_Tree->SetMinSize(wxWindow::FromDIP(wxSize(270, 400), nullptr));
  m_Tree->SetImageList(std::move(tree_images));

  m_Guih->SetSize(wxWindow::FromDIP(wxSize(270, 400), nullptr));
  m_Guih->SetMinSize(wxWindow::FromDIP(wxSize(270, 400), nullptr));
	
  wxBoxSizer *vsz = new wxBoxSizer(wxVERTICAL);
  vsz->Add(m_Guih,1,wxEXPAND);

	wxBoxSizer *hsz = new wxBoxSizer(wxHORIZONTAL);
	hsz->Add(m_Tree,1,wxEXPAND | wxALL, 6);
	hsz->Add(vsz,0,wxEXPAND | wxALL, 6);

  m_Dialog->Add(hsz,1,wxEXPAND);

  AddRoot();
}
//----------------------------------------------------------------------------
mafGUISettingsDialog::~mafGUISettingsDialog()
//----------------------------------------------------------------------------
{
  for (int i = 0; i < m_List.size(); i++)
  {
    delete m_List[i];
  }
  m_List.clear();
};
//----------------------------------------------------------------------------
bool mafGUISettingsDialog::ShowModal()
//----------------------------------------------------------------------------
{
  return m_Dialog->ShowModal() != 0;
}
//----------------------------------------------------------------------------
void mafGUISettingsDialog::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case VME_SELECT:
      {
        Page *page = (Page*)(e->GetArg());
        if(page)
          m_Guih->Put(page->ui);
      }
      break; 
      default:
        e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsDialog::AddRoot()
//----------------------------------------------------------------------------
{
  // the Root is m_List[0]
  
  Page *page = new Page();
  page->label = "General";
  page->parent = 0;
  page->ui = new mafGUI(this);
  m_List.push_back(page);

  m_Tree->AddNode((intptr_t)page,0,mafWxToString(page->label),0);
}
//----------------------------------------------------------------------------
void mafGUISettingsDialog::AddPage(mafGUI *ui, wxString label, wxString parent_label)
//----------------------------------------------------------------------------
{
  if(GetPageByName(label)) return; // page already exist

  Page *parent = GetPageByName(parent_label);
  if(!parent) parent = m_List[0];

  Page *page = new Page();
  page->label = label;
  page->parent = parent;
  page->ui = ui;
  m_List.push_back(page);
  
  m_Tree->AddNode((intptr_t)page,(intptr_t)parent,mafWxToString(label),0);
}
//----------------------------------------------------------------------------
void mafGUISettingsDialog::RemovePage(wxString label)
//----------------------------------------------------------------------------
{
  if(label == "General") return; // prevent removing the Root
  
  Page *page = GetPageByName(label);
  if(!page) return;
  m_Tree->DeleteNode((intptr_t)(page));

  // remove page from the List --- and shift up all following elements
  int i;
  for(i=0 ; i < m_List.size(); i++)
  {
    Page *p = m_List[i];
    if(p && p->label == label) delete p;
  }
  for( ; i < m_List.size()-1; i++ )
  {
    m_List[i] = m_List[i+1];
  }
  m_List.pop_back();
  
}
//----------------------------------------------------------------------------
mafGUISettingsDialog::Page* mafGUISettingsDialog::GetPageByName(wxString name)
//----------------------------------------------------------------------------
{
  for(int i=0 ; i < m_List.size(); i++)
  {
    Page *p = m_List[i];
    if(p && p->label == name) return p;
  }
  return NULL;
}
