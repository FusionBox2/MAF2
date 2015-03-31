/*=========================================================================

 Program: MAF2
 Module: mafGUIListBox
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUIListBox.h"
#include "mafGUIValidator.h"

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
#define ID_CLB 100
//----------------------------------------------------------------------------
// mafGUIListBox
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIListBox,mafGUIPanel)
    EVT_LISTBOX ( ID_CLB, mafGUIListBox::OnSelect)
    EVT_SIZE(mafGUIListBox::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIListBox::mafGUIListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:mafGUIPanel(parent,id,pos,size,style) 
//----------------------------------------------------------------------------
{
  m_PreventNotify = false;
  m_ListBox = new wxListBox(this, ID_CLB, wxDefaultPosition, wxDefaultSize, 0,NULL,wxLB_SINGLE | wxLB_HSCROLL | wxLB_OWNERDRAW );
}
//----------------------------------------------------------------------------
mafGUIListBox::~mafGUIListBox( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIListBox::Clear()
//----------------------------------------------------------------------------
{
   m_ListBox->Clear();
}
//----------------------------------------------------------------------------
void mafGUIListBox::AddItem(wxString label, bool checked)
//----------------------------------------------------------------------------
{
	m_ListBox->Append(label);
	int id = m_ListBox->GetCount()-1;
	m_ListBox->SetSelection(id);

	if(checked)
	{
		this->CheckItem(id);
	}
}
//----------------------------------------------------------------------------
void mafGUIListBox::RemoveItem(long id)
//----------------------------------------------------------------------------
{
	m_ListBox->Delete(id);
}
//----------------------------------------------------------------------------
bool mafGUIListBox::IsChecked()
//----------------------------------------------------------------------------
{
	int id = m_ListBox->GetSelection();
	//@@@ if (m_ListBox->GetItem(id)->GetBackgroundColour() == wxColour("YELLOW"))
	//@@@	 return true;
	return false;
}
//----------------------------------------------------------------------------
void mafGUIListBox::CheckItem(long id)
//----------------------------------------------------------------------------
{
	//@@@ m_ListBox->GetItem(id)->SetBackgroundColour(wxColour("YELLOW"));
}
//----------------------------------------------------------------------------
void mafGUIListBox::CheckSelectedItem()
//----------------------------------------------------------------------------
{
	CheckItem((long)this->m_ListBox->GetSelection());
}
//----------------------------------------------------------------------------
void mafGUIListBox::SetItemLabel(long id, wxString label)
//----------------------------------------------------------------------------
{
	//bool check = this->IsChecked();
	//m_ListBox->Delete(id);
	//this->AddItem(label,check);

  int idx = m_ListBox->GetSelection();
  m_ListBox->SetSelection(idx,false);
  m_ListBox->SetString(id,label); 
  m_ListBox->SetSelection(idx,true);
}
//----------------------------------------------------------------------------
void mafGUIListBox::SetItemLabel(wxString label)
//----------------------------------------------------------------------------
{
  int idx = m_ListBox->GetSelection();
	SetItemLabel(idx,label);
}
//----------------------------------------------------------------------------
void mafGUIListBox::OnSelect(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
   int widget_id = this->GetId();
   int index = event.GetInt();
   m_ItemSelected = this->m_ListBox->GetStringSelection();
   mafEventMacro(mafEvent(this, widget_id, (long)index) );
}
//----------------------------------------------------------------------------
void mafGUIListBox::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
   m_ListBox->SetSize(event.GetSize());
}
//----------------------------------------------------------------------------
void mafGUIListBox::Select( int id )
//----------------------------------------------------------------------------
{
  m_ListBox->SetSelection(id);// needed line below because 
                              // SetSelection doesn't rise event
                              // wxEVT_COMMAND_LISTBOX_SELECTED
                              // trapped from mafGUI
  ((mafGUIValidator *)m_ListBox->GetValidator())->TransferFromWindow();
}
