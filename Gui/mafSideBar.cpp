/*=========================================================================

 Program: MAF2
 Module: mafSideBar
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

#include "mafSideBar.h"
#include "mafView.h"
#include "mafViewVTK.h"
#include "mafPipe.h"
#include "mafGUI.h"
#include "mafGUIHolder.h"
#include "mafGUIPanelStack.h"
#include "mafGUIPanel.h"
#include "mafGUISplittedPanel.h"
#include "mafGUITree.h"

#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMERoot.h"

//----------------------------------------------------------------------------
mafSideBar::mafSideBar(wxWindow* parent, int id, mafObserver *Listener, long style)
//----------------------------------------------------------------------------
{
  m_SelectedVme  = NULL;
  m_SelectedView = NULL;
  m_CurrentVmeGui = NULL;
  m_CurrentPipeGui = NULL;
  m_Listener = Listener;
  m_Style = style;

  //splitted panel  
  m_Notebook = new wxNotebook(parent,id);
  m_Notebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
  m_SideSplittedPanel = new wxSplitterWindow(m_Notebook, -1, wxDefaultPosition, wxSize(-1,-1),/*wxSP_3DSASH |*/ wxSP_FULLSASH);

  //tree ----------------------------
  m_Tree = new mafGUICheckTree(m_SideSplittedPanel,-1,false,true);
  m_Tree->SetListener(Listener);
  m_Tree->SetSize(-1,300);
  m_Tree->SetTitle(" vme hierarchy: ");
  m_Notebook->AddPage(m_SideSplittedPanel,_("data tree"),true);

  //view property panel
  m_ViewPropertyPanel = new mafGUIHolder(m_Notebook,-1,false,true);
  m_ViewPropertyPanel->SetTitle(_("no view selected:"));
  m_Notebook->AddPage(m_ViewPropertyPanel,_("view settings"));

  //op_panel ----------------------------
  m_OpPanel  = new mafGUIPanelStack(m_Notebook ,-1);
  mafGUINamedPanel *empty_op = new mafGUINamedPanel(m_OpPanel ,-1,false,true);
  empty_op->SetTitle(_(" no operation running:"));
  m_OpPanel->Push(empty_op);
  m_Notebook->AddPage(m_OpPanel ,_("operation"));

  if (style == DOUBLE_NOTEBOOK)
  {
    m_VmeNotebook = new wxNotebook(m_SideSplittedPanel,-1);
    m_VmeNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

    m_VmeOutputPanel = new mafGUIHolder(m_VmeNotebook,-1,false,true);
    m_VmeNotebook->AddPage(m_VmeOutputPanel,_(" vme output "));
    m_VmePipePanel = new mafGUIHolder(m_VmeNotebook,-1,false,true);
    m_VmeNotebook->AddPage(m_VmePipePanel,_(" visual props "));
    m_VmePanel = new mafGUIHolder(m_VmeNotebook,-1,false,true);
    m_VmeNotebook->AddPage(m_VmePanel,_("vme"));

    m_SideSplittedPanel->SetMinimumPaneSize(50);
    m_SideSplittedPanel->SplitHorizontally(m_Tree,m_VmeNotebook);
  }
  else
  {
    m_VmePanel = new mafGUIHolder(m_Notebook,-1,false,true);
    m_Notebook->AddPage(m_VmePanel ,_("vme"));

    m_VmeOutputPanel = new mafGUIHolder(m_SideSplittedPanel,-1,false,true);
    m_VmeOutputPanel->Show(false);
    m_SideSplittedPanel->SetMinimumPaneSize(5);
    m_SideSplittedPanel->SplitHorizontally(m_Tree,m_VmeOutputPanel);

//     m_VmePipePanel = new mafGUIHolder(m_Notebook,-1,false,true);
//     m_VmePipePanel->Show(false);
  }
}
//----------------------------------------------------------------------------
mafSideBar::~mafSideBar() 
//----------------------------------------------------------------------------
{
	cppDEL(m_Notebook);
}
//----------------------------------------------------------------------------
void mafSideBar::OpShowGui(bool push_gui, mafGUIPanel *panel)
//----------------------------------------------------------------------------
{
	m_Notebook->Show(true);
	if(push_gui)
	{
		m_Notebook->SetSelection(2);
		m_OpPanel->Push(panel);
	}
}
//----------------------------------------------------------------------------
void mafSideBar::OpHideGui(bool view_closed)
//----------------------------------------------------------------------------
{
	if(view_closed)
	{
		this->m_Notebook->SetSelection(0);
		this->m_ViewPropertyPanel->RemoveCurrentGui();
		this->m_ViewPropertyPanel->SetTitle("");
	}
	else
	{
		m_OpPanel->Pop();
		m_Notebook->SetSelection(0);
	}
}
//----------------------------------------------------------------------------
void mafSideBar::ViewSelect(mafView *view)
//----------------------------------------------------------------------------
{
	m_Tree->ViewSelected(view);
	if(view)
	{
		wxString s = " ";
		s += wxStripMenuCodes(view->GetLabel());
		s += " props:";
		m_ViewPropertyPanel->SetTitle(s);
		mafGUI *gui = view->GetGui();
		if(gui) 
			m_ViewPropertyPanel->Put(gui);
		else
			m_ViewPropertyPanel->RemoveCurrentGui();
	}
	else
	{
    m_ViewPropertyPanel->SetTitle(_("no view selected:"));
		m_ViewPropertyPanel->RemoveCurrentGui();
	}
  m_SelectedView = view;
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::ViewDeleted(mafView *view)
//----------------------------------------------------------------------------
{
	m_Tree->ViewDeleted(view);
  ViewSelect(NULL);
}
//----------------------------------------------------------------------------
void mafSideBar::EnableSelect(bool enable)
//----------------------------------------------------------------------------
{
	m_Tree->EnableSelect(enable);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_Tree->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_Tree->VmeRemove(vme);
  if (vme == m_SelectedVme)
  {
    m_SelectedVme = NULL;
    UpdateVmePanel();
  }  
}
//----------------------------------------------------------------------------
void mafSideBar::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_Tree->VmeModified(vme);
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::VmeShow(mafNode *vme, bool visibility)
//----------------------------------------------------------------------------
{
	m_Tree->VmeShow(vme,visibility);
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::VmeSelected(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_SelectedVme = vme;
  UpdateVmePanel();
  m_Tree->VmeSelected(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmePropertyRemove(mafGUI *gui)
//----------------------------------------------------------------------------
{
	m_VmePanel->Remove(gui);
}
//----------------------------------------------------------------------------
void mafSideBar::Show()
//----------------------------------------------------------------------------
{
	m_Notebook->Show(!m_Notebook->IsShown());
}
//----------------------------------------------------------------------------
void mafSideBar::UpdateVmePanel()
//----------------------------------------------------------------------------
{
  mafVMEOutput *vme_out = NULL;
  mafPipe      *vme_pipe = NULL;
  mafGUI       *vme_gui = NULL;
  mafGUI       *vme_out_gui = NULL;
  mafGUI       *vme_pipe_gui = NULL;

  if (m_Style == SINGLE_NOTEBOOK)
  {
	  if(m_SelectedVme)
	  {
	    vme_gui = m_SelectedVme->GetGui();
	
	    if(m_SelectedVme->IsMAFType(mafVME))
	    {
	      mafVME *v = (mafVME*) m_SelectedVme;
	      vme_out = v->GetOutput();
	      if(!vme_out->IsA("mafVMEOutputNULL")) // Paolo 2005-05-05
	      {
	        vme_out_gui = vme_out->GetGui();
	        if (!v->IsDataAvailable())
	        {
	          vme_out->Update();
	        }
	      }
	      else
	        vme_out = NULL;
	    }
	
	    if (m_OldAppendingGUI)
	    {
	      if (m_CurrentVmeGui)
	      {
	        m_OldAppendingGUI->Remove(m_CurrentVmeGui);
	      }
	      /*if (m_CurrentPipeGui)
	      {
	        m_OldAppendingGUI->Remove(m_CurrentPipeGui);
	      }*/
	    }
	
	    if(m_SelectedView)
	    {
	      vme_pipe = m_SelectedView->GetNodePipe(m_SelectedVme);
	
	      if(vme_pipe)
	      {
	        vme_pipe_gui = vme_pipe->GetGui();
	      }
	    }
	  }
	
	  m_NewAppendingGUI = NULL;
	
	  m_CurrentPipeGui = vme_pipe_gui;
	  m_CurrentVmeGui = vme_gui;
	
	  if (vme_gui || vme_pipe_gui)
	  {
	    m_NewAppendingGUI = new mafGUI(NULL);
	
	    if (vme_gui)
	    {
	      m_NewAppendingGUI->AddGui(vme_gui);
	      m_NewAppendingGUI->FitGui();
	      m_NewAppendingGUI->Update();
	    }
	    if (vme_pipe_gui)
	    {
	      m_NewAppendingGUI->Label(_("GUI visual Pipes"),true);
	      m_NewAppendingGUI->AddGui(vme_pipe_gui);
	      m_NewAppendingGUI->FitGui();
	      m_NewAppendingGUI->Update();
	    }
	  }
	
	
	  m_VmePanel->Put(m_NewAppendingGUI);
	
	  m_OldAppendingGUI = m_NewAppendingGUI;
	
	  m_VmeOutputPanel->Put(vme_out_gui);
  }
  else if (m_Style == DOUBLE_NOTEBOOK)
  {
    if(m_SelectedVme)
    {
      vme_gui = m_SelectedVme->GetGui();

      if(m_SelectedVme->IsMAFType(mafVME))
      {
        mafVME *v = (mafVME*) m_SelectedVme;
        vme_out = v->GetOutput();
        if(!vme_out->IsA("mafVMEOutputNULL")) // Paolo 2005-05-05
        {
          vme_out_gui = vme_out->GetGui();
          if (!v->IsDataAvailable())
          {
            vme_out->Update();
          }
        }
        else
          vme_out = NULL;
      }

      if(m_SelectedView)
      {
        vme_pipe = m_SelectedView->GetNodePipe(m_SelectedVme);
        if(vme_pipe)
          vme_pipe_gui = vme_pipe->GetGui();
      }
    }

    m_VmePanel->Put(vme_gui);
    m_VmeOutputPanel->Put(vme_out_gui);
    m_VmePipePanel->Put(vme_pipe_gui);
  }

  /* code stub to future support of dynamic creation/destruction of GUI
  // vme_gui changed 
  if( vme_gui != m_VmePanel->GetCurrentGui() ) 
  {
    m_VmePanel->Put(vme_gui);
    
    //if(last_vme_gui && last_vme)
       //last_vme->DeleteGui(); // - what if last_vme was destroyed ?
  }
  
  // vme_out changed 
  if( vme_out_gui != m_VmeOutputPanel->GetCurrentGui() ) 
  {
    m_VmeOutputPanel->Put(vme_out_gui);

    //if(last_vme_out_gui && last_vme_out)
      // last_vme_out->DeleteGui(); // - what if last_vme_out was destroyed ?
  }

  // vme_pipe changed 
  if( vme_pipe_gui != m_VmePipePanel->GetCurrentGui() ) 
  {
    m_VmePipePanel->Put(vme_pipe_gui);

    //if(last_vme_pipe_gui && last_vme_pipe)
      // last_vme_pipe->DeleteGui(); // - what if last_vme_pipe was destroyed ?
  }*/
}
