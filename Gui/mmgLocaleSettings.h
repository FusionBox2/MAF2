/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgLocaleSettings.h,v $
Language:  C++
Date:      $Date: 2006-06-14 14:46:33 $
Version:   $Revision: 1.3 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgLocaleSettings_H__
#define __mmgLocaleSettings_H__

#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;

//----------------------------------------------------------------------------
// mmgLocaleSettings :
//----------------------------------------------------------------------------
/**
*/
class mmgLocaleSettings : public mafObserver
{
public:
	mmgLocaleSettings(mafObserver *Listener);
	~mmgLocaleSettings(); 

  enum LOCALE_WIDGET_ID
  {
    LANGUAGE_ID = MINID,
  };

  /** 
  Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Show/Hide the settings interface.*/
	//void ChooseLocale();
	
	mmgGui* GetGui() {return m_Gui;};

protected:
  /** Initialize language used into the application.*/
  void InitializeLanguage();

  mafObserver *m_Listener;
  int          m_LanguageId;
  wxLocale     m_Locale;
  wxLanguage   m_Language;
  mafString    m_LanguageDictionary;

  mmgGui*      m_Gui;
};
#endif