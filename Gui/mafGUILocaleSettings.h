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

#ifndef __mafGUILocaleSettings_H__
#define __mafGUILocaleSettings_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  class name :mafGUILocaleSettings 
  class that handle gui panel for locale settings inside application settings.
*/
class mafGUILocaleSettings : public mafGUISettings
{
public:
  /** constructor */
	mafGUILocaleSettings(mafBaseEventHandler *Listener, const mafString &label = _L("Interface language"));
  /** destructor */
	~mafGUILocaleSettings(); 

  enum LOCALE_WIDGET_ID
  {
    LANGUAGE_ID = MINID,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Setter for Language Flag  */
  void SetEnableLanguage(bool flag){m_EnableLanguage = flag;};
  /** Getter for Language Flag  */
  bool GetEnableLanguage(){return m_EnableLanguage;};
  /** Enable Language Flag */
  void EnableLanguageOn(){m_EnableLanguage = true;};
  /** Disable Language Flag */
  void EnableLanguageOff(){m_EnableLanguage = false;};

  /*Function for set language directory*/
  void SetLanguageDirectory(const char* prefix, const char* languageDirectory);

  /* Function for set a different language*/
  void ChangeLanguage(wxLanguage languageEnum = wxLANGUAGE_ENGLISH , const char *languageAcronym = "en");

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize language used into the application.*/
  void InitializeSettings();

  bool         m_EnableLanguage;
  int          m_LanguageId;
  wxLocale     m_Locale;
  wxLanguage   m_Language;
  mafString    m_LanguageDictionary;
};
#endif
