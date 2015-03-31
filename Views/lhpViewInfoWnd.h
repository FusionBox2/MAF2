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

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// lhpViewInfoWnd
//----------------------------------------------------------------------------
class lhpViewInfoWnd: public wxWindow, public mafBaseEventHandler, public mafEventSender
{
  DECLARE_DYNAMIC_CLASS(lhpViewInfoWnd)
public:
  lhpViewInfoWnd(const wxString& label = "", mafBaseEventHandler *listener = NULL);
  ~lhpViewInfoWnd(void);

  wxBitmap       GetBitmap();
  mafGUI         *GetGui() {return m_Gui;};
  void           CreateGui();
  void           OnEvent(mafEventBase *maf_event);
  void           Update(void);

  void           AddInfo(mafNode *pInfo);
  void           RemInfo(mafNode *pInfo);

protected:

  mafGUI                            *m_Gui;

  void           OnCommand(wxCommandEvent& event);
  void           OnPaint(wxPaintEvent &event);
  void           OnSize(wxSizeEvent &event);
  void           OnCloseWindow  (wxCloseEvent& event);
  void           OnRightMouseButtonDown(wxMouseEvent &event);
  void           OnLeftMouseButtonDown(wxMouseEvent &event);

  //must have for table hook
  DECLARE_EVENT_TABLE();


private:
  std::vector<mafNode *> m_VNodes;

  void      DrawGraph(wxDC *pCompatDC);
  void      DrawAxes(wxDC *pDC, wxRect *prc, double rXMin, double rXMax, double rYMin, double rYMax, double rXCoef, double rYCoef, bool bGrid, bool bPreciseGrid);
  void      DrawXAxis(wxDC *pDC, wxRect *prc, double rXMin, double rXMax, double rXCoef, bool bGrid, bool bPreciseGrid);
  void      DrawYAxis(wxDC *pDC, wxRect *prc, double rYMin, double rYMax, double rYCoef, bool bGrid, bool bPreciseGrid);
  void      Init();

};

#endif