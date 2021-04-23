/*=========================================================================

 Program: MAF2
 Module: mafGUIApplicationSettings
 Authors: Paolo Quadrani - Daniele Giunchi
 
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

#include "mafGUIApplicationSettings.h"
#include "mafCrypt.h"

#include "mafDecl.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
mafGUIApplicationSettings::mafGUIApplicationSettings(mafBaseEventHandler *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  // Default values for the application.
  m_LogToFile   = 1; //logging enabled for correcting bug #820
  m_VerboseLog  = 0;
  m_LogFolder = mafWxToString(wxGetCwd());

	m_ImageTypeId = PNG;

  m_WarnUserFlag = true;
  
  m_UseDefaultPasPhrase = 1;
  m_PassPhrase = _R(mafDefaultPassPhrase());

  InitializeSettings();

  m_EnableLogDirChoices = true;
}
//----------------------------------------------------------------------------
mafGUIApplicationSettings::~mafGUIApplicationSettings()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->Label(_L("Application general settings"));
  m_Gui->Bool(ID_USE_DEFAULT_PASSPHRASE,_L("use default passphrase"),&m_UseDefaultPasPhrase,1);
  m_Gui->String(ID_PASSPHRASE,_L("passphrase"),&m_PassPhrase,_R(""),false,true);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_LOG_TO_FILE,_L("log to file"),&m_LogToFile,1);
  m_Gui->Bool(ID_LOG_VERBOSE,_L("log verbose"),&m_VerboseLog,1);
  if (m_EnableLogDirChoices)
  {
  	m_Gui->DirOpen(ID_LOG_DIR,_L("log dir"),&m_LogFolder);
  }
  m_Gui->Divider(2);
  m_Gui->Bool(ID_WARN_UNDO, _L("warn on undoable"), &m_WarnUserFlag, 1, _L("If checked the use is warned when an operation \nthat not support the undo is executed."));
  EnableItems();
  m_Gui->Label(_L("changes will take effect when the \napplication restart"),false,true);
  m_Gui->Label(_R(""));
  mafString id_array[3] = {_L("JPG") , _L("BMP"), _L("PNG")};
  m_Gui->Combo(IMAGE_TYPE_ID,_L("image type"), &m_ImageTypeId,3,id_array);
  m_Gui->Divider(2);
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LOG_VERBOSE,m_LogToFile != 0);
  m_Gui->Enable(ID_LOG_DIR,m_LogToFile != 0);
  m_Gui->Enable(ID_PASSPHRASE,m_UseDefaultPasPhrase == 0);
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case ID_LOG_TO_FILE:
      m_Config->Write("LogToFile",m_LogToFile);
    break;
    case ID_LOG_VERBOSE:
      m_Config->Write("LogVerbose",m_VerboseLog);
      mafEventBase::SetLogVerbose(m_VerboseLog != 0);
    break;
    case ID_LOG_DIR:
      m_Config->Write("LogFolder",m_LogFolder.toWx());
    break;
    case ID_WARN_UNDO:
      m_Config->Write("WarnUser",m_WarnUserFlag);
      mafEventMacro(mafEvent(this,MENU_OPTION_APPLICATION_SETTINGS));
    break;
    case ID_PASSPHRASE:
    break;
    case ID_USE_DEFAULT_PASSPHRASE:
      m_Config->Write("UseDefaultPassphrase",m_UseDefaultPasPhrase);
      if (m_UseDefaultPasPhrase != 0)
      {
        m_PassPhrase = _R(mafDefaultPassPhrase());
        mafWarningMessage(_M(mafString(_L("Passphrase resetted to default one!"))));
      }
      else
      {
        m_PassPhrase = mafWxToString(wxGetPasswordFromUser(_("Insert passphrase"),_("Passphrase"),wxEmptyString));
      }
      EnableItems();
      m_Gui->Update();
    break;
		case IMAGE_TYPE_ID:
		{
			m_Config->Write("ImageType",m_ImageTypeId);
		}
		break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  EnableItems();
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
  wxString string_item;
  long long_item;
  if(m_Config->Read("LogToFile", &long_item))
  {
    m_LogToFile = long_item;
  }
  else
  {
    m_Config->Write("LogToFile",m_LogToFile);
  }
  if(m_Config->Read("LogVerbose", &long_item))
  {
    m_VerboseLog = long_item;
    mafEventBase::SetLogVerbose(m_VerboseLog != 0);
  }
  else
  {
    m_Config->Write("LogVerbose",m_VerboseLog);
  }
  if(m_Config->Read("LogFolder", &string_item))
  {
    m_LogFolder = mafWxToString(string_item);
  }
  else
  {
    m_Config->Write("LogFolder",m_LogFolder.toWx());
  }
  if (m_Config->Read("WarnUser", &long_item))
  {
    m_WarnUserFlag = long_item;
  }
  else
  {
    m_Config->Write("WarnUser",m_WarnUserFlag);
  }
  if (m_Config->Read("UseDefaultPassphrase", &long_item))
  {
    m_UseDefaultPasPhrase = long_item;
  }
  else
  {
    m_Config->Write("UseDefaultPassphrase",m_UseDefaultPasPhrase);
  }
  if (m_UseDefaultPasPhrase == 0)
  {
    m_PassPhrase = mafWxToString(wxGetPasswordFromUser(_("Insert passphrase"),_("Passphrase"),wxEmptyString));
  }
  if(m_Config->Read("ImageType", &long_item))
  {
    m_ImageTypeId = long_item;
  }
  else
  {
    m_Config->Write("ImageType", m_ImageTypeId);
  }
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetLogFolder(mafString log_folder)
//----------------------------------------------------------------------------
{
  if (m_LogFolder != log_folder)
  {
    m_LogFolder = log_folder;
    m_Config->Write("LogFolder",m_LogFolder.toWx());
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetLogFileStatus(int log_status)
//----------------------------------------------------------------------------
{
  if (m_LogToFile != log_status)
  {
    m_LogToFile = log_status;
    m_Config->Write("LogToFile",m_LogToFile);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetLogVerboseStatus(int log_verbose)
//----------------------------------------------------------------------------
{
  if (m_VerboseLog != log_verbose)
  {
    m_VerboseLog = log_verbose;
    m_Config->Write("LogVerbose",m_VerboseLog);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetUseDefaultPassPhrase(int use_default, const mafString& passphrase)
//----------------------------------------------------------------------------
{
  if (m_UseDefaultPasPhrase != use_default)
  {
    m_UseDefaultPasPhrase = use_default;
    m_Config->Write("UseDefaultPassphrase",m_UseDefaultPasPhrase);
    m_Config->Flush();
    if (m_UseDefaultPasPhrase != 0)
    {
      m_PassPhrase = _R(mafDefaultPassPhrase());
    }
    else
    {
      if (passphrase.IsEmpty())
      {
        m_PassPhrase = mafWxToString(wxGetPasswordFromUser(_("Insert passphrase"),_("Passphrase"),wxEmptyString));
      }
      else
      {
        m_PassPhrase = passphrase;
      }
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetPassPhrase(const mafString& pass_phrase)
//----------------------------------------------------------------------------
{
  m_PassPhrase = pass_phrase;
}
