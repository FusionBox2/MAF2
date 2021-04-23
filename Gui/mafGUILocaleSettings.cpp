/*=========================================================================

 Program: MAF2
 Module: mafGUILocaleSettings
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

#include "mafGUILocaleSettings.h"
#include <wx/intl.h>
#include "mafGUI.h"
#include "mafGUIDialog.h"

//----------------------------------------------------------------------------
mafGUILocaleSettings::mafGUILocaleSettings(mafBaseEventHandler *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  m_EnableLanguage = false; //29-03-2007 disabling for bug #218
  m_LanguageId = 0;
  InitializeSettings();
}
//----------------------------------------------------------------------------
mafGUILocaleSettings::~mafGUILocaleSettings() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::CreateGui()
//----------------------------------------------------------------------------
{
  mafString lang_array[8] = {_R("English"),_R("French"),_R("German"),_R("Italian"),_R("Spanish"),_R("Russian"),_R("Polish"),_R("Czech")};

  m_Gui = new mafGUI(this);   
  m_Gui->Label(_L("User Interface Language"));
  m_Gui->Radio(LANGUAGE_ID,_R(""), &m_LanguageId, 8,lang_array);
	m_Gui->Enable(LANGUAGE_ID,m_EnableLanguage); 
  m_Gui->Label(_L("changes will take effect when \nthe application restart"),false,true);
  m_Gui->Label(_R(""));
}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case LANGUAGE_ID:
    {
      switch(m_LanguageId) 
      {
      case 1:
        m_Language = wxLANGUAGE_FRENCH;
        m_LanguageDictionary = _R("fr");
        break;
      case 2:
        m_Language = wxLANGUAGE_GERMAN;
        m_LanguageDictionary = _R("de");
        break;
      case 3:
        m_Language = wxLANGUAGE_ITALIAN;
        m_LanguageDictionary = _R("it");
        break;
      case 4:
        m_Language = wxLANGUAGE_SPANISH;
        m_LanguageDictionary = _R("es");
        break;
	  case 5:
		m_Language = wxLANGUAGE_RUSSIAN;
        m_LanguageDictionary = _R("ru");
        break;
	  case 6:
		m_Language = wxLANGUAGE_POLISH;
        m_LanguageDictionary = _R("pl");
        break;
	  case 7:
		m_Language = wxLANGUAGE_CZECH;
		m_LanguageDictionary = _R("cs");
		break;
	  case 8:
		  m_Language = wxLANGUAGE_CHINESE_SIMPLIFIED;
		  m_LanguageDictionary = _R("zh");
		  break;
      default:
        m_Language = wxLANGUAGE_ENGLISH;
        m_LanguageDictionary = _R("en");
      }
      m_Config->Write("Language",m_Language);
      m_Config->Write("Dictionary",m_LanguageDictionary.toWx());
      m_Config->Flush();
    }
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
  long lang;
  wxString dict;
  if(m_Config->Read("Language", &lang))
  {
    m_Language = (wxLanguage)lang;
    m_Config->Read("Dictionary", &dict);
    m_LanguageDictionary = mafWxToString(dict);
  }
  else
  {
    // no language set; use default language: English
    m_Config->Write("Language",wxLANGUAGE_ENGLISH);
    m_Config->Write("Dictionary","en");
    m_Language = wxLANGUAGE_ENGLISH;
    m_LanguageDictionary = _R("en");
  }

  m_Config->Flush();

  wxString prefix = wxGetCwd();
  prefix += "\\Language\\";
  m_Locale.Init(m_Language);
  m_Locale.AddCatalogLookupPathPrefix(prefix);
  m_Locale.AddCatalog(m_LanguageDictionary.toWx());
#ifndef WIN32
  m_Locale.AddCatalog("fileutils");
#endif

  switch(m_Language) 
  {
  case wxLANGUAGE_FRENCH:
    m_LanguageId =1;
    break;
  case wxLANGUAGE_GERMAN:
    m_LanguageId =2;
    break;
  case wxLANGUAGE_ITALIAN:
    m_LanguageId =3;
    break;
  case wxLANGUAGE_SPANISH:
    m_LanguageId =4;
    break;
  case wxLANGUAGE_RUSSIAN:
    m_LanguageId =5;
    break;
  case wxLANGUAGE_POLISH:
    m_LanguageId =6;
    break;
  case wxLANGUAGE_CZECH:
	m_LanguageId =7;
	break;
  case wxLANGUAGE_CHINESE_SIMPLIFIED:
	  m_LanguageId =8;
	  break;
  default: //wxLANGUAGE_ENGLISH;
    m_LanguageId =0; 
  }
}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::SetLanguageDirectory(const char* prefix, const char* languageDirectory)
//----------------------------------------------------------------------------
{
  m_LanguageDictionary = _R(languageDirectory);
  m_Locale.AddCatalogLookupPathPrefix(prefix);

  m_Locale.AddCatalog(m_LanguageDictionary.toWx());
  if (m_LanguageDictionary == _R("fr"))
  {
  	m_Language		= wxLANGUAGE_FRENCH;
	m_LanguageId	= 1;
  }
  else if (m_LanguageDictionary == _R("de"))
  {
    m_Language		= wxLANGUAGE_GERMAN;
	m_LanguageId	= 2;
  }
  else if (m_LanguageDictionary == _R("it"))
  {
	  m_Language	= wxLANGUAGE_ITALIAN;
	m_LanguageId	= 3; 
  }
  else if (m_LanguageDictionary == _R("es"))
  {
    m_Language		= wxLANGUAGE_SPANISH;
	m_LanguageId	= 4;
  }
  else if (m_LanguageDictionary == _R("ru"))
  {
    m_Language		= wxLANGUAGE_RUSSIAN;
	m_LanguageId	= 5;
  }
  else if (m_LanguageDictionary == _R("pl"))
  {
    m_Language		= wxLANGUAGE_POLISH;
	m_LanguageId	= 6;
  }
  else if (m_LanguageDictionary == _R("cs"))
  {
	m_Language		= wxLANGUAGE_CZECH;
	m_LanguageId	= 7;
  }
  else if (m_LanguageDictionary == _R("zh"))
  {
	  m_Language		= wxLANGUAGE_CHINESE_SIMPLIFIED;
	  m_LanguageId	= 8;
  }
  else if (m_LanguageDictionary == _R("en"))
  {
    m_Language		= wxLANGUAGE_ENGLISH;
	m_LanguageId	= 0;
  }

}
//----------------------------------------------------------------------------
void mafGUILocaleSettings::ChangeLanguage(wxLanguage languageEnum, const char *languageAcronym)
//----------------------------------------------------------------------------
{
  m_Config->Write("Language",languageEnum);
  m_Config->Write("Dictionary",languageAcronym);
  m_Language = languageEnum;
  m_LanguageDictionary = _R(languageAcronym);
}
