/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile:  .h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:13:22 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpViewInfoWnd_H___
#define __lhpViewInfoWnd_H___
    
//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "mafString.h"
#include "mafEventSender.h"
#include "mafGUI.h"
#include "wx/wxHtml.h"

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// lhpViewInfoWnd
//----------------------------------------------------------------------------
class lhpViewInfoWnd: public wxHtmlWindow, public mafBaseEventHandler, public mafEventSender
{
  DECLARE_DYNAMIC_CLASS(lhpViewInfoWnd)
public:
  lhpViewInfoWnd(const wxString& label = "", mafBaseEventHandler *listener = NULL);
  ~lhpViewInfoWnd(void);

 // wxBitmap       GetBitmap();
  mafGUI         *GetGui() {return m_Gui;};
  void           CreateGui();
  void           OnEvent(mafEventBase *maf_event);
  void           Update(void);

  void           AddInfo(mafNode *pInfo);
  void           RemInfo(mafNode *pInfo);

protected:

  mafGUI                            *m_Gui;

  /*void           OnCommand(wxCommandEvent& event);
  void           OnPaint(wxPaintEvent &event);
  void           OnSize(wxSizeEvent &event);
  void           OnCloseWindow  (wxCloseEvent& event);
  void           OnRightMouseButtonDown(wxMouseEvent &event);
  void           OnLeftMouseButtonDown(wxMouseEvent &event);*/

  //must have for table hook
  DECLARE_EVENT_TABLE();


private:
  std::vector<mafNode *> m_VNodes;

  //void      DrawGraph(wxDC *pCompatDC);
  //void      Init();

};

#endif