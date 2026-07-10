/*=========================================================================

 Program: MAF2
 Module: mafGUIDockSettings
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
#include <wx/aui/aui.h>

#include "mafGUIDockSettings.h"
#include "mafGUI.h"
#include "mafGUIDialog.h"

DockSettings GetDockSettings(wxAuiManager& manager)
{
    DockSettings settings;
    settings.m_PaneBorderSize = manager.GetArtProvider()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE);
    settings.m_SashSize = manager.GetArtProvider()->GetMetric(wxAUI_DOCKART_SASH_SIZE);
    settings.m_CaptionSize = manager.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);
    settings.m_BackgroundColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_BACKGROUND_COLOUR);
    settings.m_SashColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_SASH_COLOUR);
    settings.m_InactiveCaptionColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
    settings.m_InactiveCaptionGradientColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR);
    settings.m_InactiveCaptionTextColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR);
    settings.m_ActiveCaptionColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR);
    settings.m_ActiveCaptionGradientColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR);
    settings.m_ActiveCaptionTextColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR);
    settings.m_BorderColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_BORDER_COLOUR);
    settings.m_GripperColor = manager.GetArtProvider()->GetColor(wxAUI_DOCKART_GRIPPER_COLOUR);

    settings.m_AllowFloating = (manager.GetFlags() & wxAUI_MGR_ALLOW_FLOATING) ? 1 : 0;
    settings.m_AllowActivePane = (manager.GetFlags() & wxAUI_MGR_ALLOW_ACTIVE_PANE) ? 1 : 0;

    if (manager.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE)        settings.m_CaptionMode = 0;
    if (manager.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL)    settings.m_CaptionMode = 1;
    if (manager.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL)  settings.m_CaptionMode = 2;
    return settings;
}

void ApplyDockSettings(wxAuiManager& manager, const DockSettings& settings)
{
    manager.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, settings.m_PaneBorderSize);
    manager.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, settings.m_SashSize);
    manager.GetArtProvider()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, settings.m_CaptionSize);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, settings.m_BackgroundColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR, settings.m_SashColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, settings.m_InactiveCaptionColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, settings.m_InactiveCaptionGradientColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, settings.m_InactiveCaptionTextColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, settings.m_ActiveCaptionColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, settings.m_ActiveCaptionGradientColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, settings.m_ActiveCaptionTextColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_BORDER_COLOUR, settings.m_BorderColor);
    manager.GetArtProvider()->SetColor(wxAUI_DOCKART_GRIPPER_COLOUR, settings.m_GripperColor);
    if (settings.m_AllowFloating)
    {
        manager.SetFlags(manager.GetFlags() | wxAUI_MGR_ALLOW_FLOATING);
    }
    else
    {
        manager.SetFlags(manager.GetFlags() & ~wxAUI_MGR_ALLOW_FLOATING);
    }
    if (settings.m_AllowActivePane)
    {
        manager.SetFlags(manager.GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE);
    }
    else
    {
        manager.SetFlags(manager.GetFlags() & ~wxAUI_MGR_ALLOW_ACTIVE_PANE);
    }
    if (settings.m_CaptionMode == 0) manager.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);
    if (settings.m_CaptionMode == 1) manager.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_VERTICAL);
    if (settings.m_CaptionMode == 2) manager.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
    manager.Update();
}


//----------------------------------------------------------------------------
// widgets' IDs
//----------------------------------------------------------------------------
enum DOCK_SETTINGS_ID
{
  ID_PaneBorderSize = MINID,
  ID_SashSize,
  ID_CaptionSize,
  ID_BackgroundColor,
  ID_SashColor,
  ID_InactiveCaptionColor,
  ID_InactiveCaptionGradientColor,
  ID_InactiveCaptionTextColor,
  ID_ActiveCaptionColor,
  ID_ActiveCaptionGradientColor,
  ID_ActiveCaptionTextColor,
  ID_BorderColor,
  ID_GripperColor,

  ID_AllowFloating,
  ID_AllowActivePane,
  ID_CaptionMode
};
//----------------------------------------------------------------------------
mafGUIDockSettings::mafGUIDockSettings(wxAuiManager& mgr, const mafString &label) :
m_Mgr(mgr), mafGUISettings(NULL, label)
//----------------------------------------------------------------------------
{
    m_settings = GetDockSettings(m_Mgr);
}
//----------------------------------------------------------------------------
mafGUI* mafGUIDockSettings::FillGui(mafGUI* gui, DockSettings& settings)
//----------------------------------------------------------------------------
{
    gui->Bool(ID_AllowFloating, _R("Allow pane floating"), &settings.m_AllowFloating, 1);
    //m_Gui->Bool( ID_AllowActivePane, "highlight active pane",&m_AllowActivePane, 1);

    //m_Gui->Divider(1);
    //m_Gui->Label("Pane caption mode");
    mafString captionModeLabels[3];
    captionModeLabels[0] = _R("paint caption with flat color");
    captionModeLabels[1] = _R("paint caption with horizontal gradient");
    captionModeLabels[2] = _R("paint caption with vertical gradient");
    gui->Radio(ID_CaptionMode, _R(""), &settings.m_CaptionMode, 3, captionModeLabels);
    gui->Divider(0);

    gui->Label(_R("Pane metrics"), false);
    gui->Slider(ID_PaneBorderSize, _R("BorderSize"), &settings.m_PaneBorderSize, 0, 10);
    gui->Slider(ID_SashSize, _R("SashSize"), &settings.m_SashSize, 0, 10);
    gui->Slider(ID_CaptionSize, _R("CaptionSize"), &settings.m_CaptionSize, 2, 24);
    gui->Divider(1);

    gui->Label(_R("Pane colors"), false);
    gui->Color(ID_BackgroundColor, _R("Background"), &settings.m_BackgroundColor);
    gui->Color(ID_SashColor, _R("Sash"), &settings.m_SashColor);
    gui->Color(ID_BorderColor, _R("Border"), &settings.m_BorderColor);
    gui->Color(ID_GripperColor, _R("Gripper"), &settings.m_GripperColor);
    gui->Divider(1);

    gui->Label(_R("Caption colors"), false);
    //m_Gui->Label("Inactive caption colors",false);
    gui->Color(ID_InactiveCaptionColor, _R("Color"), &settings.m_InactiveCaptionColor);
    gui->Color(ID_InactiveCaptionGradientColor, _R("Gradient"), &settings.m_InactiveCaptionGradientColor);
    gui->Color(ID_InactiveCaptionTextColor, _R("Text"), &settings.m_InactiveCaptionTextColor);
    gui->Divider(1);

    //m_Gui->Label("Active caption colors",false);
    //m_Gui->Color(ID_ActiveCaptionColor,"Color",&m_ActiveCaptionColor);
    //m_Gui->Color(ID_ActiveCaptionGradientColor,"Gradient",&m_ActiveCaptionGradientColor);
    //m_Gui->Color(ID_ActiveCaptionTextColor,"Text",&m_ActiveCaptionTextColor);
    //m_Gui->Label("");
    return gui;
}
//----------------------------------------------------------------------------
void mafGUIDockSettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = FillGui(new mafGUI(this), m_settings);
}
//----------------------------------------------------------------------------
mafGUIDockSettings::~mafGUIDockSettings()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIDockSettings::OnEvent(mafEventBase *evt)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(evt)) 
  {
    switch(e->GetId())
    {
    case ID_PaneBorderSize:
    case ID_SashSize:
    case ID_CaptionSize:
    case ID_BackgroundColor:
    case ID_SashColor:
    case ID_InactiveCaptionColor:
    case ID_InactiveCaptionGradientColor:
    case ID_InactiveCaptionTextColor:
    case ID_ActiveCaptionColor:
    case ID_ActiveCaptionGradientColor:
    case ID_ActiveCaptionTextColor:
    case ID_BorderColor:
    case ID_GripperColor:
    case ID_AllowFloating:
    case ID_AllowActivePane:
    case ID_CaptionMode:
        ApplyDockSettings(m_Mgr, m_settings);
        break;
    default: 
      e->Log();
    break;
    }
  }
}
