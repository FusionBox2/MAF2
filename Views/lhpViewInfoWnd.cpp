/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpViewInfoWnd.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:13:22 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpViewInfoWnd.h"
#include "mafNode.h"
#include "wx/file.h" 
#include "wx/colordlg.h" 

#include <stdio.h>
#include <set>

#include "lhpViewInfo.h"
#include "mafGUI.h"

#define MAXTEXTLEN           1024 

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// compulsory definitions
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(lhpViewInfoWnd, wxWindow)

//----------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void  lhpViewInfoWnd::AddInfo(mafNode *pInfo)
//----------------------------------------------------------------------------
{
  m_VNodes.push_back(pInfo);
  Refresh(false, NULL);
  return;
}

//----------------------------------------------------------------------------
void  lhpViewInfoWnd::RemInfo(mafNode *pGraphData)
  //----------------------------------------------------------------------------
{
  for(std::vector<mafNode *>::iterator it = m_VNodes.begin(); it != m_VNodes.end(); ++it)
  {
    if((*it) == pGraphData)
    {
      m_VNodes.erase(it);
      break;
    }
  }
  Refresh(false, NULL);
  return;
}
#ifdef TYRYTRYT

//----------------------------------------------------------------------------
void lhpViewInfoWnd::DrawGraph(wxDC *pCompatDC)
//----------------------------------------------------------------------------
{
  int          nClHeight, nClWidth;
  wxPoint      szTextSize;
  wxPoint      pt;
  wxRect       rTextRect;
  wxRect       rcGraphRect;
  wxPen        *pPen;
  wxFont       *pFont;
  char         sExp[MAXTEXTLEN + 1];


  pCompatDC->GetSize(&nClWidth, &nClHeight);
  pPen = new wxPen(wxColor(255,255,255), 1, wxSOLID);
  pCompatDC->SetPen(*pPen);
  pCompatDC->DrawRectangle(0, 0, nClWidth, nClHeight);
  pCompatDC->SetPen(wxNullPen);
  cppDEL(pPen);


  pFont = new wxFont(10, wxDEFAULT, wxNORMAL, wxLIGHT, false, wxEmptyString, wxFONTENCODING_SYSTEM) ;
  pCompatDC->SetFont(*pFont);
  //m_TitleFntHeight = pCompatDC->GetCharHeight();
  rcGraphRect.SetLeft(0);
  rcGraphRect.SetRight(nClWidth);
  //set it to N*1.5 graphs 
  rcGraphRect.SetTop(3 * pCompatDC->GetCharHeight() * m_VNodes.size() / 2);

  pt.x = 0;
  pt.y = 0;

  for(std::vector<mafNode*>::iterator it = m_VNodes.begin(); it != m_VNodes.end(); ++it)
  {
    _snprintf(sExp, MAXTEXTLEN, "%s", (*it)->GetName().GetCStr());
    pCompatDC->GetTextExtent(sExp, &szTextSize.x, &szTextSize.y);
    rTextRect.SetLeft(pt.x + 35);
    rTextRect.SetTop(pt.y);
    rTextRect.SetRight(pt.x + szTextSize.x + 35);
    rTextRect.SetBottom(20);
    pCompatDC->DrawText(sExp, rTextRect.GetLeft(), rTextRect.GetTop());
    pt.y += 30;
  }
  pCompatDC->SetFont(wxNullFont);
  cppDEL(pFont);
  return;
} // end of _saRedraw

//----------------------------------------------------------------------------
wxBitmap lhpViewInfoWnd::GetBitmap()
//----------------------------------------------------------------------------
{
  int        nHeight, nWidth;
  wxMemoryDC compatDC;
  GetClientSize(&nWidth, &nHeight);
  wxBitmap   pBitMap(nWidth, nHeight);
  compatDC.SelectObject(pBitMap);
  DrawGraph(&compatDC);
  compatDC.SelectObject(wxNullBitmap);
  return pBitMap;
}
/*
 * handler for WM_COMMAND message
 * @memo    
 * @param   event   actual event received
 * @author  Earnol
 */
//----------------------------------------------------------------------------
void lhpViewInfoWnd::OnCommand(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  Refresh(false, NULL);
  return;
} // end of SaIntGraphWndDoCommandProc

//----------------------------------------------------------------------------
void lhpViewInfoWnd::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  return;
} // end of lhpViewInfoWnd::OnLeftMouseButtonDown

/*
 * handler for WM_RBUTTONDOWN message
 *
 * @memo    
 * @param   hWnd    standart
 * @param   nMsg    windows
 * @param   wParam  params
 * @param   lParam  and
 * @return          return value for this message
 * @author  Earnol
 */
//----------------------------------------------------------------------------
void lhpViewInfoWnd::OnRightMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  return;
} // end of lhpViewInfoWnd::OnRightMouseButtonDown

/*
 * handler for WM_PAINT message
 *
 * @memo    
 * @param   event   paint event
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void lhpViewInfoWnd::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  Hide();
  Destroy();
}

/*
 * handler for WM_PAINT message
 *
 * @memo    
 * @param   event   paint event
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void lhpViewInfoWnd::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pDC(this);
  int   nHeight, nWidth;
  wxMemoryDC compatDC;

  if(this == NULL)
  {
    //in case of weird destroys
    return;
  }
  GetClientSize(&nWidth, &nHeight);
  wxBitmap   pBitMap(nWidth, nHeight);
  compatDC.SelectObject(pBitMap);  
  DrawGraph(&compatDC);  
  pDC.Blit(0, 0, nWidth, nHeight, &compatDC,0,0, wxCOPY);
  compatDC.SelectObject(wxNullBitmap);
  return;
} // end of SaIntGraphWndDoPaintProc

/*
 * handler for WM_SIZE message
 *
 * @memo    
 * @param   hWnd    standart
 * @param   nMsg    windows
 * @param   wParam  params
 * @param   lParam  and
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void lhpViewInfoWnd::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  Refresh(false, NULL);

  return;
} // end of lhpViewInfoWnd::OnSize

#endif


//----------------------------------------------------------------------------
void lhpViewInfoWnd::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  m_Gui->Update();
}

//----------------------------------------------------------------------------
void lhpViewInfoWnd::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      default:
        Update();
        break;
    }
  }
}
//----------------------------------------------------------------------------

/*
 * handler for IGWM_UPDATE message
 *
 * @memo    
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void  lhpViewInfoWnd::Update(void)
//----------------------------------------------------------------------------
{
  Refresh(false, NULL);
} // end of SaIntGraphWndDoUpdateProc



BEGIN_EVENT_TABLE(lhpViewInfoWnd, wxHtmlWindow)
  /*EVT_PAINT      (lhpViewInfoWnd::OnPaint)
  EVT_CLOSE      (lhpViewInfoWnd::OnCloseWindow)
  EVT_RIGHT_DOWN (lhpViewInfoWnd::OnRightMouseButtonDown)
  EVT_LEFT_DOWN  (lhpViewInfoWnd::OnLeftMouseButtonDown)
  EVT_SIZE       (lhpViewInfoWnd::OnSize)
  EVT_COMMAND_RANGE(0, 0xFFFF, wxEVT_COMMAND_MENU_SELECTED, lhpViewInfoWnd::OnCommand)*/
END_EVENT_TABLE()

/**
 * Actual constructor
 *
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
lhpViewInfoWnd::lhpViewInfoWnd(const wxString& label, mafBaseEventHandler *listener):
wxHtmlWindow(mafGetFrame(), -1, wxDefaultPosition, wxDefaultSize, 0, label)
//----------------------------------------------------------------------------
{
  SetListener(listener);
  m_Gui              = NULL;
  CreateGui();
}


/*
 * Just a destructor
 *
 * @memo    
 * @return  Nothing
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
lhpViewInfoWnd::~lhpViewInfoWnd()
//----------------------------------------------------------------------------
{
}
