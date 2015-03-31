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

#include "mafGUIDockSettings.h"
#include "mafGUIDockManager.h"
#include "mafGUI.h"
#include "mafGUIDialog.h"

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
  m_PaneBorderSize                = m_Mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE);
  m_SashSize                      = m_Mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_SASH_SIZE);
  m_CaptionSize                   = m_Mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);
  m_BackgroundColor               = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_BACKGROUND_COLOUR);
  m_SashColor                     = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_SASH_COLOUR);
  m_InactiveCaptionColor          = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
  m_InactiveCaptionGradientColor  = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR);
  m_InactiveCaptionTextColor      = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR);
  m_ActiveCaptionColor            = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR);
  m_ActiveCaptionGradientColor    = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR);
  m_ActiveCaptionTextColor        = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR);
  m_BorderColor                   = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_BORDER_COLOUR);
  m_GripperColor                  = m_Mgr.GetArtProvider()->GetColor(wxAUI_DOCKART_GRIPPER_COLOUR);

  m_AllowFloating   = (m_Mgr.GetFlags() & wxAUI_MGR_ALLOW_FLOATING)    ? 1 : 0;
  m_AllowActivePane = (m_Mgr.GetFlags() & wxAUI_MGR_ALLOW_ACTIVE_PANE) ? 1 : 0;

  if( m_Mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE)        m_CaptionMode = 0;
  if( m_Mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL)    m_CaptionMode = 1;
  if( m_Mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL)  m_CaptionMode = 2;

  m_CaptionModeLabels[0] = "paint caption with flat color";
  m_CaptionModeLabels[1] = "paint caption with horizontal gradient";
  m_CaptionModeLabels[2] = "paint caption with vertical gradient";
}
//----------------------------------------------------------------------------
void mafGUIDockSettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  m_Gui->Bool( ID_AllowFloating,   "Allow pane floating",&m_AllowFloating, 1);
  //m_Gui->Bool( ID_AllowActivePane, "highlight active pane",&m_AllowActivePane, 1);

  //m_Gui->Divider(1);
  //m_Gui->Label("Pane caption mode");
  m_Gui->Radio(ID_CaptionMode,"",&m_CaptionMode,3,m_CaptionModeLabels);
  m_Gui->Divider(0);

  m_Gui->Label("Pane metrics",false);
  m_Gui->Slider(ID_PaneBorderSize,"BorderSize",&m_PaneBorderSize,0,10);
  m_Gui->Slider(ID_SashSize,"SashSize",&m_SashSize,0,10);
  m_Gui->Slider(ID_CaptionSize,"CaptionSize",&m_CaptionSize,2,24);
  m_Gui->Divider(1);

  m_Gui->Label("Pane colors",false);
  m_Gui->Color(ID_BackgroundColor,"Background",&m_BackgroundColor);
  m_Gui->Color(ID_SashColor,"Sash",&m_SashColor);
  m_Gui->Color(ID_BorderColor,"Border",&m_BorderColor);
  m_Gui->Color(ID_GripperColor,"Gripper",&m_GripperColor);
  m_Gui->Divider(1);

  m_Gui->Label("Caption colors",false);
  //m_Gui->Label("Inactive caption colors",false);
  m_Gui->Color(ID_InactiveCaptionColor,"Color",&m_InactiveCaptionColor);
  m_Gui->Color(ID_InactiveCaptionGradientColor,"Gradient",&m_InactiveCaptionGradientColor);
  m_Gui->Color(ID_InactiveCaptionTextColor,"Text",&m_InactiveCaptionTextColor);
  m_Gui->Divider(1);

  //m_Gui->Label("Active caption colors",false);
  //m_Gui->Color(ID_ActiveCaptionColor,"Color",&m_ActiveCaptionColor);
  //m_Gui->Color(ID_ActiveCaptionGradientColor,"Gradient",&m_ActiveCaptionGradientColor);
  //m_Gui->Color(ID_ActiveCaptionTextColor,"Text",&m_ActiveCaptionTextColor);
  //m_Gui->Label("");
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
      m_Mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,m_PaneBorderSize);
      break;
    case ID_SashSize:
      m_Mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE,m_SashSize);
      break;
    case ID_CaptionSize:
      m_Mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE,m_CaptionSize);
      break;
    case ID_BackgroundColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR,m_BackgroundColor);
      break;
    case ID_SashColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR,m_SashColor);
      break;
    case ID_InactiveCaptionColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR,m_InactiveCaptionColor);
      break;
    case ID_InactiveCaptionGradientColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR,m_InactiveCaptionGradientColor);
      break;
    case ID_InactiveCaptionTextColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR,m_InactiveCaptionTextColor);
      break;
    case ID_ActiveCaptionColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR,m_ActiveCaptionColor);
      break;
    case ID_ActiveCaptionGradientColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR,m_ActiveCaptionGradientColor);
      break;
    case ID_ActiveCaptionTextColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR,m_ActiveCaptionTextColor);
      break;
    case ID_BorderColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BORDER_COLOUR,m_BorderColor);
      break;
    case ID_GripperColor:
      m_Mgr.GetArtProvider()->SetColor(wxAUI_DOCKART_GRIPPER_COLOUR,m_GripperColor);
      break;
    case ID_AllowFloating:
      if(m_AllowFloating)
        m_Mgr.SetFlags(m_Mgr.GetFlags() | wxAUI_MGR_ALLOW_FLOATING);
      else
        m_Mgr.SetFlags(m_Mgr.GetFlags() & ~ wxAUI_MGR_ALLOW_FLOATING);
      break;
    case ID_AllowActivePane:
      if(m_AllowActivePane)
        m_Mgr.SetFlags(m_Mgr.GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE);
      else
        m_Mgr.SetFlags(m_Mgr.GetFlags() & ~ wxAUI_MGR_ALLOW_ACTIVE_PANE);
      break;
    case ID_CaptionMode:
      if(m_CaptionMode == 0) m_Mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);
      if(m_CaptionMode == 1) m_Mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_VERTICAL);
      if(m_CaptionMode == 2) m_Mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
      break;
    default: 
      e->Log();
    break;
    }
    m_Mgr.Update();
  }
}
