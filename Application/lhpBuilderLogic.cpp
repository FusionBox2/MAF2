/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2009-01-15 11:19:05 $
  Version:   $Revision: 1.9.2.6 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpBuilderLogic.h"
#include "lhpBuilderDecl.h"
#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafOp.h"
#include "mafOpManager.h"
#include "mafGUISettingsDialog.h"
#include "lhpGUIPythonSettings.h"
#include "lhpUser.h"

#include "psLoaderGUIContextualMenu.h"
#include "mafViewManager.h"
#include "mafGUIMDIChild.h"
#include "mafGUIMDIFrame.h"

//----------------------------------------------------------------------------
lhpBuilderLogic::lhpBuilderLogic()
//----------------------------------------------------------------------------
{
  m_PythonSettings = new lhpGUIPythonSettings(this);
  m_User = new lhpUser(this);
}
//----------------------------------------------------------------------------
lhpBuilderLogic::~lhpBuilderLogic()
//----------------------------------------------------------------------------
{
  cppDEL(m_PythonSettings);
}
//----------------------------------------------------------------------------
void lhpBuilderLogic::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ABOUT_APPLICATION:
			{
				wxString message = m_AppTitle.GetCStr();
				message += _(" Application ");
				message += m_Revision;
				wxMessageBox(message, "About Application");
				mafLogMessage(wxString::Format("%s",m_Revision.GetCStr()));
			}
			break;
      case ID_REQUEST_APPLICATION_NAME:
      {
        e->SetString(&m_AppTitle);
      }
      break;

    case ID_MSF_DATA_CACHE:
      {
        //comunicate to operation msf directory
        e->SetString(&m_VMEManager->GetFileName());
      }
      break;
    case ID_REQUEST_USER:
      {
        e->SetMafObject((mafObject*)m_User);
      }
      break;
    case ID_REQUEST_PYTHON_EXE_INTERPRETER:
      {
        if(m_PythonSettings->GetPythonExe())
        {
          e->SetString(&m_PythonSettings->GetPythonExe());
        }
      }
      break;
    
    case ID_REQUEST_PYTHONW_EXE_INTERPRETER:
      {
        if(m_PythonSettings->GetPythonwExe())
        {

          e->SetString(&m_PythonSettings->GetPythonwExe());
        }
      }
      break;

    default:
			mafLogicWithManagers::OnEvent(maf_event);
			break; 
		} // end switch case
	} // end if SafeDowncast
}
//----------------------------------------------------------------------------
void lhpBuilderLogic::VmeAdded(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::VmeAdded(vme);
  // check for the presence of the LHDL attribute

  mafTagArray *lhdlArray = mafTagArray::SafeDownCast(vme->GetAttribute("LHDL"));
  if (lhdlArray == NULL)
  {
    lhdlArray = mafTagArray::New();
    lhdlArray->SetName("LHDL");
    vme->SetAttribute("LHDL",lhdlArray);
  }
}
//----------------------------------------------------------------------------
void lhpBuilderLogic::Configure()
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Configure(); // create the GUI - and calls CreateMenu
  if (m_PythonSettings)
  {
    m_SettingsDialog->AddPage(m_PythonSettings->GetGui(), m_PythonSettings->GetLabel());
  }
}
//----------------------------------------------------------------------------
void lhpBuilderLogic::GetCredentials()
//----------------------------------------------------------------------------
{
  bool retry = false;
  retry = m_User->CheckUserCredentials();

  while (retry)
  {
    retry = m_User->CheckUserCredentials();
  }
  m_OpManager->SetMafUser(m_User);
}

//----------------------------------------------------------------------------
void lhpBuilderLogic::ViewContextualMenu(bool vme_menu)
//----------------------------------------------------------------------------
{
  if (m_AppTitle == "PSLoader")
  {
    psLoaderGUIContextualMenu *contextMenu = new psLoaderGUIContextualMenu();
    contextMenu->SetListener(this);
    mafView *v = m_ViewManager->GetSelectedView();
    mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
    if(c != NULL)
      contextMenu->ShowContextualMenu(c,v,vme_menu);
    cppDEL(contextMenu);
  }
  else
  {
    medLogicWithManagers::ViewContextualMenu(vme_menu);
  }
}