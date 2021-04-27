/*=========================================================================

 Program: MAF2
 Module: mafGUISettings
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUISettings_H__
#define __mafGUISettings_H__

#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include <wx/config.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafGUI;

/**
 class name: mafGUISettings
  base class for more complex specified classes regarding the setting of application.
  It returns a gui and generally can access to registry keys.
*/
class MAF_EXPORT mafGUISettings : public mafBaseEventHandler, public mafEventSender
{
public:
  /** constructor */
	mafGUISettings(mafBaseEventHandler *Listener, const mafString &label = _L("Settings"));
  /** destructor */
	~mafGUISettings(); 

  /** Answer to the messages coming from interface. */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Return the GUI of the setting panel.*/
  mafGUI* GetGui();

  /** Return the settings' label*/
  const char *GetLabel() {return m_Label.GetCStr();};

protected:
  /** Create the GUI for the setting panel.*/
  virtual void CreateGui();

  /** Initialize the settings.*/
  virtual void InitializeSettings();

  mafGUI *m_Gui;
  mafString m_Label;

  wxConfig *m_Config;
};
#endif
