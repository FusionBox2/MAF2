#pragma once

#include "mafGUISettings.h"

struct DockSettings
{
	wxColour m_BackgroundColor;
	wxColour m_SashColor;
	wxColour m_InactiveCaptionColor;
	wxColour m_InactiveCaptionGradientColor;
	wxColour m_InactiveCaptionTextColor;
	wxColour m_ActiveCaptionColor;
	wxColour m_ActiveCaptionGradientColor;
	wxColour m_ActiveCaptionTextColor;
	wxColour m_BorderColor;
	wxColour m_GripperColor;
	int      m_PaneBorderSize;
	int      m_SashSize;
	int      m_CaptionSize;
	int      m_AllowFloating;
	int      m_AllowActivePane;
	int      m_CaptionMode;
};


DockSettings GetDockSettings(wxAuiManager& manager);
void ApplyDockSettings(wxAuiManager& manager, const DockSettings& settings);

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class wxAuiManager;

/**
  class name: mafGUIDockSettings
  class that handle the dock settings panel.
*/
class mafGUIDockSettings: public mafGUISettings
{
public:
 /** constructor */
 mafGUIDockSettings(wxAuiManager& mgr, const mafString &label = _L("User Interface Preferences"));
 /** destructor */
 ~mafGUIDockSettings() override;

  /** Answer to the messages coming from interface. */
 void OnEvent(mafEventBase *evt) override;

 static mafGUI* FillGui(mafGUI* gui, DockSettings& settings);
protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui() override;

  wxAuiManager& m_Mgr;
  DockSettings m_settings;
};
